#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "address.c" // Mapeamento de registradores do acelerômetro

int16_t accel_x_offset = 0;

/*Um ponteiro global para os registradores mapeados da interface I2C.
 Ele será usado para acessar o hardware diretamente via mmap.*/
volatile uint32_t *i2c0_regs;

/* Abre o /dev/mem e mapeia a memória do I2C, retornando o descritor do arquivo.
 Ele será usado mais tarde para fechar e desmapear a memória.*/
int open_and_map() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        printf("Erro ao abrir /dev/mem");
        return -1;
    }
    
    i2c0_regs = (volatile uint32_t *) mmap(NULL, I2C0_REG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, I2C0_BASE_ADDR);
    if (i2c0_regs == MAP_FAILED) {
        printf("Erro ao mapear /dev/mem");
        close(fd);
        return -1;
    }

    return fd;
}

// Desfaz o mapeamento de memória e fecha o descritor do arquivo.
void close_and_unmap(int fd) {
    if (i2c0_regs) {
        munmap((void *)i2c0_regs, I2C0_REG_SIZE);
    }
    close(fd);
}

/*Essas funções escrevem e leem valores dos registradores do I2C. A conversão offset / 4 
é necessária porque o volatile uint32_t *base aponta para uma memória de 32 bits,
enquanto os registradores têm offsets em bytes. */
void write_register(volatile uint32_t *base, uint32_t offset, int32_t value) {
    base[offset / 4] = value;
}

int32_t read_register(volatile uint32_t *base, uint32_t offset) {
    return base[offset / 4];
}

//Esta função configura o I2C. Ela habilita o controlador, define a taxa de clock, e define o endereço de destino do acelerômetro (0x53).
void I2C0_init() {
    write_register(i2c0_regs, I2C0_ENABLE, 0x02);
    while ((read_register(i2c0_regs, I2C0_ENABLE_STATUS) & 0x1) == 1) {}
    write_register(i2c0_regs, I2C0_CON, 0x65);
    write_register(i2c0_regs, I2C0_TAR, 0x53);
    write_register(i2c0_regs, I2C0_FS_SCL_HCNT, 90);
    write_register(i2c0_regs, I2C0_FS_SCL_LCNT, 160);
    write_register(i2c0_regs, I2C0_ENABLE, 0x01);
    while ((read_register(i2c0_regs, I2C0_ENABLE_STATUS) & 0x1) == 0) {}
}

// Envia comandos ao acelerômetro escrevendo valores nos registradores via I2C.
void accel_reg_write(uint8_t addressA, int8_t value)
{
    write_register(i2c0_regs, I2C0_DATA_CMD, addressA + 0x400);
    write_register(i2c0_regs, I2C0_DATA_CMD, value);
}

// Lê dados do acelerômetro.
void accel_reg_read(uint8_t addressA, int8_t *value) {
    write_register(i2c0_regs, I2C0_DATA_CMD, addressA + 0x400);
    write_register(i2c0_regs, I2C0_DATA_CMD, 0x100);
    while (read_register(i2c0_regs, I2C0_RXFLR) == 0) {}
    *value = (int8_t)read_register(i2c0_regs, I2C0_DATA_CMD);
}

// Função de calibração para capturar o valor do eixo X em 180 graus

/*Para calibrar o acelerômetro e definir que a posição em 180 graus será o ponto de referência (0),
 você pode implementar uma função de calibração que armazena o valor lido do eixo X quando a placa 
 está na posição de 180 graus e, em seguida, usa esse valor como um offset para ajustar as leituras
 futuras. Aqui está uma abordagem para implementar essa calibração:

    1.Leitura do valor de referência (offset): Quando você calibra o acelerômetro, deve ler o valor
      do eixo X na posição de 180 graus. Esse valor será armazenado como um "offset".

    2.Ajuste das leituras futuras: Para todas as leituras subsequentes do acelerômetro, você subtrai
     o valor do "offset" para que 180 graus seja considerado como 0.*/
void calibrate_accel_x() {
    int8_t x_data_0 = 0;
    int8_t x_data_1 = 0;
    accel_reg_read(DATA_X0, &x_data_0);  // Lê o valor atual do eixo X
    accel_reg_read(DATA_X1, &x_data_1);
    accel_x_offset = x_data_1;
    accel_x_offset = accel_x_offset << 8;
    accel_x_offset += x_data_0;
    printf("Calibração concluída. Offset do eixo X: %d\n", accel_x_offset);
}


// Configura o acelerômetro para o modo de medição com sensibilidade de ±16g e frequência de 200 Hz.
void accel_init() {
    accel_reg_write(DATA_FORMAT, 0x0B);
    accel_reg_write(BW_RATE, 0x0B);
    accel_reg_write(POWER_CTL, 0x00);
    accel_reg_write(POWER_CTL, 0x08);

    // Agora que o acelerômetro está inicializado, realiza a calibração
    calibrate_accel_x();  
}

// Função para obter o valor ajustado do eixo X
int get_calibrated_accel_x() {
    int8_t x_data_0 = 0;
    int8_t x_data_1 = 0;
    accel_reg_read(DATA_X0, &x_data_0);  // Lê o valor atual do eixo X
    accel_reg_read(DATA_X1, &x_data_1);  // Lê o valor atual do eixo X
    int16_t accel_x = x_data_1;
    accel_x = accel_x << 8;
    accel_x += x_data_0;
    accel_x -= accel_x_offset;  // Subtrai o valor do offset
    return accel_x;
}
