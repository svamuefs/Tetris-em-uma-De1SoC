#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "address.h" // Mapeamento de registradores do acelerômetro

#define I2C0_BASE_ADDR 0xFFC04000  // Endereço base do controlador I2C0
#define I2C0_REG_SIZE  0x1000      

/* Definições dos offsets dos registradores do controlador I2C. Esses valores correspondem a endereços
de registradores dentro do bloco I2C que controlam a comunicação entre o FPGA e o acelerômetro.*/

#define I2C0_CON                0x00
#define I2C0_TAR                0x04
#define I2C0_DATA_CMD           0x10
#define I2C0_FS_SCL_HCNT        0x1C
#define I2C0_FS_SCL_LCNT        0x20
#define I2C0_CLR_INTR           0x40
#define I2C0_ENABLE             0x6C
#define I2C0_RXFLR              0x78
#define I2C0_ENABLE_STATUS      0x9C

int8_t accel_x_offset = 0;

/*Um ponteiro global para os registradores mapeados da interface I2C.
 Ele será usado para acessar o hardware diretamente via mmap.*/
volatile uint32_t *i2c0_regs;

/*Função que abre o dispositivo /dev/mem, o qual permite o acesso direto
 à memória física do sistema. Se a abertura falhar, ela retorna um erro.*/
int open_physical() {
    int fd;
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        printf("Erro ao abrir /dev/mem");
        return -1;
    }
    return fd;
}

//Fecha o descritor de arquivo retornado por open_physical().
void close_physical(int fd) {
    close(fd);
}

/*A função mmap_i2c0 mapeia a região de memória do I2C para o espaço de memória do processo,
 permitindo acesso direto aos registradores do I2C. O endereço base é I2C0_BASE_ADDR.*/
int mmap_i2c0(int fd) {
    i2c0_regs = (volatile uint32_t *) mmap(NULL, I2C0_REG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, I2C0_BASE_ADDR);
    if (i2c0_regs == MAP_FAILED) {
        printf("Erro ao mapear /dev/mem");
        return -1;
    }
    return 0;
}

/* Abre o /dev/mem e mapeia a memória do I2C, retornando o descritor do arquivo.
 Ele será usado mais tarde para fechar e desmapear a memória.*/
int open_and_mmap_dev_mem() {
    int fd = open_physical();
    if (fd == -1) {
        return -1;
    }
    if (mmap_i2c0(fd) == -1) {
        close(fd);
        return -1;
    }
    return fd;
}

// Desfaz o mapeamento de memória e fecha o descritor do arquivo.
void close_and_unmap_dev_mem(int fd) {
    if (i2c0_regs) {
        munmap((void *)i2c0_regs, I2C0_REG_SIZE);
    }
    close_physical(fd);
}

/*Essas funções escrevem e leem valores dos registradores do I2C. A conversão offset / 4 
é necessária porque o volatile uint32_t *base aponta para uma memória de 32 bits,
enquanto os registradores têm offsets em bytes. */
void write_register(volatile uint32_t *base, uint32_t offset, uint32_t value) {
    base[offset / 4] = value;
}
uint32_t read_register(volatile uint32_t *base, uint32_t offset) {
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
void accel_reg_write(uint8_t addressA, uint8_t value)
{
    write_register(i2c0_regs, I2C0_DATA_CMD, addressA + 0x400);
    write_register(i2c0_regs, I2C0_DATA_CMD, value);
}

// Lê dados do acelerômetro.
void accel_reg_read(uint8_t addressA, uint8_t *value) {
    write_register(i2c0_regs, I2C0_DATA_CMD, addressA + 0x400);
    write_register(i2c0_regs, I2C0_DATA_CMD, 0x100);
    while (read_register(i2c0_regs, I2C0_RXFLR) == 0) {}
    *value = (uint8_t)read_register(i2c0_regs, I2C0_DATA_CMD);
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
    uint8_t x_data = 0;
    accel_reg_read(DATA_X0, &x_data);  // Lê o valor do eixo X
    accel_x_offset = (int8_t)x_data;  // Armazena o valor como offset
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
    uint8_t x_data = 0;
    accel_reg_read(DATA_X0, &x_data);  // Lê o valor atual do eixo X
    int8_t accel_x = (int8_t)x_data;
    accel_x -= accel_x_offset;  // Subtrai o valor do offset
    return accel_x;
}


// Coordenadas iniciais do ponto e dimensões da tela.
int point_x = 160;
int point_y = 120;
int screen_width = 320;
int screen_height = 240;

/* Atualiza a posição do ponto com base no valor do eixo X do acelerômetro. 
Ele só se move para a direita ou esquerda dependendo do valor de accel_x e do threshold.*/
// void update_point_position(int accel_x) {
//     int threshold = 5;
//     if (accel_x > threshold && point_x < screen_width - 1) {
//         point_x += 1;
//     } else if (accel_x < -threshold && point_x > 0) {
//         point_x -= 1;
//     }
// }

// Limpa a tela e desenha o ponto na nova posição.
// void draw_point(video_display* vga) {
//     video_clear(vga, 0x000000); // Limpa a tela (cor preta)
//     video_draw_pixel(vga, point_x, point_y, 0xFFFFFF); // Ponto branco
// }

/* - A função principal inicializa a memória, configura o I2C e o acelerômetro, e abre o display VGA.
   - O loop principal lê os dados do acelerômetro e atualiza a posição do ponto na tela, chamando
     as funções de desenho.
   - O programa só será encerrado quando o processo for interrompido manualmente.*/
// int move_point_with_accel() {
//     int fd = open_and_mmap_dev_mem();
//     if (fd == -1) {
//         return -1;
//     }

//     I2C0_init();
//     accel_init();

//     // video_display* vga = video_open();

//     while (1) {
//         int accel_x = get_calibrated_accel_x();  // Lê o valor ajustado do eixo X
//         // update_point_position(accel_x);
//         // draw_point(vga);
//         printf("%d\n", accel_x);
//         usleep(10000);  // 10ms de pausa
//     }
//     // video_close();
//     close_and_unmap_dev_mem(fd);
//     return 0;
// }
