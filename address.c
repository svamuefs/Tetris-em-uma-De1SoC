#define DEVID 0x00

#define THRESH_TAP 0x1D
#define OFSX 0x1E
#define OFSY 0x1F
#define OFSZ 0x20
#define DUR 0x21
#define LATENT 0x22
#define WINDOW 0x23
#define THRESH_ACT 0x24
#define THRESH_INACT 0x25
#define TIME_INACT 0x26
#define ACT_INACT_CTL 0x27
#define THRESH_FF 0x28
#define TIME_FF 0x29
#define TAP_AXES 0x2A
#define ACT_TAP_STATUS 0x2B
#define BW_RATE 0x2C
#define POWER_CTL 0x2D
#define INT_ENABLE 0x2E
#define INT_MAP 0x2F
#define INT_SOURCE 0x30
#define DATA_FORMAT 0x31
#define DATA_X0 0x32
#define DATA_X1 0x33
#define DATA_Y0 0x34
#define DATA_Y1 0x35
#define DATA_Z0 0x36
#define DATA_Z1 0x37
#define FIFO_CTL 0x38
#define FIFO_STATUS 0x39

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
