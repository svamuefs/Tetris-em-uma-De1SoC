/*********************************************************

####################---[ G-Sensor ]---####################
ESTE CÓDIGO FOI COPIADO E COLADO DE
  https://github.com/sparkfun/ADXL345_Eval/blob/master/firmware/Source%20Code/ADXL345.h

**********************************************************/

char adxl345_read(char address);
void adxl345_write(char address, char value);
void adxl345_defaults(void);
void adxl345_hw_setup(void);

#define SELECT_ACCEL()		ADXL_COM_PORT &= ~ACCEL_CS
#define UNSELECT_ACCEL()	ADXL_COM_PORT |= ACCEL_CS

#define CLOCK_HIGH()	ADXL_COM_PORT |= ACCEL_SCK
#define CLOCK_LOW()		ADXL_COM_PORT &= ~ACCEL_SCK

#define ADXL_COM_DIRECTION	DDRC
#define	ADXL_COM_PORT		PORTC
#define	ADXL_COM_PIN		PINC
#define ACCEL_CS		(1<<0)		//Port C.0
#define ACCEL_SCK		(1<<1)		//Port C.1
#define ACCEL_DI		(1<<2)		//Port C.2
#define ACCEL_DO		(1<<3)		//Port C.3

#define	ADXL_INT_DIRECTION	DDRD
#define ADXL_INT_PORT		PORTD
#define	ADXL_INT_PIN		PIND
#define	ACCEL_INT2		(1<<2)		//Port D.2
#define ACCEL_INT1		(1<<3)		//Port D.3

//*******************************************************
//						ADXL345 Definitions
//*******************************************************
#define READ	0x8000

//ADXL Register Map
#define	DEVID			0x00	//Device ID Register
#define THRESH_TAP		0x1D	//Tap Threshold
#define	OFSX			0x1E	//X-axis offset
#define	OFSY			0x1F	//Y-axis offset
#define	OFSZ			0x20	//Z-axis offset
#define	DUR				0x21	//Tap Duration
#define	Latent			0x22	//Tap latency
#define	Window			0x23	//Tap window
#define	THRESH_ACT		0x24	//Activity Threshold
#define	THRESH_INACT	0x25	//Inactivity Threshold
#define	TIME_INACT		0x26	//Inactivity Time
#define	ACT_INACT_CTL	0x27	//Axis enable control for activity and inactivity detection
#define	THRESH_FF		0x28	//free-fall threshold
#define	TIME_FF			0x29	//Free-Fall Time
#define	TAP_AXES		0x2A	//Axis control for tap/double tap
#define ACT_TAP_STATUS	0x2B	//Source of tap/double tap
#define	BW_RATE			0x2C	//Data rate and power mode control
#define POWER_CTL		0x2D	//Power Control Register
#define	INT_ENABLE		0x2E	//Interrupt Enable Control
#define	INT_MAP			0x2F	//Interrupt Mapping Control
#define	INT_SOURCE		0x30	//Source of interrupts
#define	DATA_FORMAT		0x31	//Data format control
#define DATAX0			0x32	//X-Axis Data 0
#define DATAX1			0x33	//X-Axis Data 1
#define DATAY0			0x34	//Y-Axis Data 0
#define DATAY1			0x35	//Y-Axis Data 1
#define DATAZ0			0x36	//Z-Axis Data 0
#define DATAZ1			0x37	//Z-Axis Data 1
#define	FIFO_CTL		0x38	//FIFO control
#define	FIFO_STATUS		0x39	//FIFO status

//Power Control Register Bits
#define WU_0		(1<<0)	//Wake Up Mode - Bit 0
#define	WU_1		(1<<1)	//Wake Up mode - Bit 1
#define SLEEP		(1<<2)	//Sleep Mode
#define	MEASURE		(1<<3)	//Measurement Mode
#define AUTO_SLP	(1<<4)	//Auto Sleep Mode bit
#define LINK		(1<<5)	//Link bit

//Interrupt Enable/Interrupt Map/Interrupt Source Register Bits
#define	OVERRUN		(1<<0)
#define	WATERMARK	(1<<1)
#define FREE_FALL	(1<<2)
#define	INACTIVITY	(1<<3)
#define	ACTIVITY	(1<<4)
#define DOUBLE_TAP	(1<<5)
#define	SINGLE_TAP	(1<<6)
#define	DATA_READY	(1<<7)

//Data Format Bits
#define RANGE_0		(1<<0)
#define	RANGE_1		(1<<1)
#define JUSTIFY		(1<<2)
#define	FULL_RES	(1<<3)

#define	INT_INVERT	(1<<5)
#define	SPI			(1<<6)
#define	SELF_TEST	(1<<7)free-fall threshold
#define	TIME_FF			0x29	//Free-Fall Time
#define	TAP_AXES		0x2A	//Axis control for tap/double tap
#define ACT_TAP_STATUS	0x2B	//Source of tap/double tap
#define	BW_RATE			0x2C	//Data rate and power mode control
#define POWER_CTL		0x2D	//Power Control Register
#define	INT_ENABLE		0x2E	//Interrupt Enable Control
#define	INT_MAP			0x2F	//Interrupt Mapping Control
#define	INT_SOURCE		0x30	//Source of interrupts
#define	DATA_FORMAT		0x31	//Data format control
#define DATAX0			0x32	//X-Axis Data 0
#define DATAX1			0x33	//X-Axis Data 1
#define DATAY0			0x34	//Y-Axis Data 0
#define DATAY1			0x35	//Y-Axis Data 1
#define DATAZ0			0x36	//Z-Axis Data 0
#define DATAZ1			0x37	//Z-Axis Data 1
#define	FIFO_CTL		0x38	//FIFO control
#define	FIFO_STATUS		0x39	//FIFO status

//Power Control Register Bits
#define WU_0		(1<<0)	//Wake Up Mode - Bit 0
#define	WU_1		(1<<1)	//Wake Up mode - Bit 1
#define SLEEP		(1<<2)	//Sleep Mode
#define	MEASURE		(1<<3)	//Measurement Mode
#define AUTO_SLP	(1<<4)	//Auto Sleep Mode bit
#define LINK		(1<<5)	//Link bit

//Interrupt Enable/Interrupt Map/Interrupt Source Register Bits
#define	OVERRUN		(1<<0)
#define	WATERMARK	(1<<1)
#define FREE_FALL	(1<<2)
#define	INACTIVITY	(1<<3)
#define	ACTIVITY	(1<<4)
#define DOUBLE_TAP	(1<<5)
#define	SINGLE_TAP	(1<<6)
#define	DATA_READY	(1<<7)

//Data Format Bits
#define RANGE_0		(1<<0)
#define	RANGE_1		(1<<1)
#define JUSTIFY		(1<<2)
#define	FULL_RES	(1<<3)

#define	INT_INVERT	(1<<5)
#define	SPI			(1<<6)
#define	SELF_TEST	(1<<7)