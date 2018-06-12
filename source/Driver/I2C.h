#ifndef	__I2C_h__
#define	__I2C_h__
//	write your header here

#include "define.h"





#define I2C_ACCESS_DUMMY_TIME   3
#define I2C_ACKNOWLEDGE        Bit_RESET
#define I2C_NON_ACKNOWLEDGE    Bit_SET
#define _BIT0	0x0001
#define I2C_DEVICE_ADR_WRITE(slave_adr)   (slave_adr & ~_BIT0)
#define I2C_DEVICE_ADR_READ(slave_adr)    (slave_adr | _BIT0)

typedef enum _I2cIoTransType
{
    I2C_TRANS_READ,
    I2C_TRANS_WRITE
}I2cIoTransType;

typedef enum 
{
	I2C_A,
	I2C_B,
	I2C_C,
	I2C_D,
	I2C_E,
	I2C_F,
	I2C_G,
	I2C_H,
	I2C_I,
	I2C_J,
	I2C_MAX_SIZE,
} I2CPort_TypeDef;

 // I2C_A
 //
 #define I2C_A_SDA_PORT	GPIOH			//9134
 #define I2C_A_SDA_PIN 	GPIO_Pin_10	
 #define I2C_A_SCL_PORT	GPIOH
 #define I2C_A_SCL_PIN	GPIO_Pin_9

/*
 * I2C_B
 */
 #define I2C_B_SDA_PORT	GPIOI			//9134
 #define I2C_B_SDA_PIN 	GPIO_Pin_11
 #define I2C_B_SCL_PORT	GPIOI
 #define I2C_B_SCL_PIN	GPIO_Pin_9

 /*
 * I2C_C
 */
 #define I2C_C_SDA_PORT	GPIOE			//9134
 #define I2C_C_SDA_PIN 	GPIO_Pin_6
 #define I2C_C_SCL_PORT	GPIOE
 #define I2C_C_SCL_PIN	GPIO_Pin_5

 /*
 * I2C_D
 */
 #define I2C_D_SDA_PORT	GPIOE			//9134
 #define I2C_D_SDA_PIN 	GPIO_Pin_4
 #define I2C_D_SCL_PORT	GPIOE
 #define I2C_D_SCL_PIN	GPIO_Pin_3

 /*
 * I2C_E
 */
 #define I2C_E_SDA_PORT	GPIOB			//24C64
 #define I2C_E_SDA_PIN 	GPIO_Pin_9
 #define I2C_E_SCL_PORT	GPIOB
 #define I2C_E_SCL_PIN	GPIO_Pin_8

 /*
 * I2C_F
 */
 #define I2C_F_SDA_PORT	GPIOB			//EDID HDMI1
 #define I2C_F_SDA_PIN 	GPIO_Pin_7
 #define I2C_F_SCL_PORT	GPIOB
 #define I2C_F_SCL_PIN	GPIO_Pin_6

 /*
 * I2C_G
 */
 #define I2C_G_SDA_PORT	GPIOH			//EDID DVI
 #define I2C_G_SDA_PIN 	GPIO_Pin_8			
 #define I2C_G_SCL_PORT	GPIOH
 #define I2C_G_SCL_PIN	GPIO_Pin_7
 
/*
* I2C_H
*/
 #define I2C_H_SDA_PORT	GPIOI			//5901
 #define I2C_H_SDA_PIN 	GPIO_Pin_8
 #define I2C_H_SCL_PORT	GPIOC
 #define I2C_H_SCL_PIN	GPIO_Pin_13

/*
* I2C_I
*/
#define I2C_I_SDA_PORT GPIOA			//EDID HDMI2
#define I2C_I_SDA_PIN  GPIO_Pin_4
#define I2C_I_SCL_PORT GPIOA
#define I2C_I_SCL_PIN  GPIO_Pin_6


/*
* I2C_J
*/
#define I2C_J_SDA_PORT GPIOI			//KEY BOARD
#define I2C_J_SDA_PIN  GPIO_Pin_1
#define I2C_J_SCL_PORT GPIOD
#define I2C_J_SCL_PIN  GPIO_Pin_7




//#define I2C_EDID_1 I2C_E
#define I2C_EDID_1 I2C_I
#define I2C_EDID_2 I2C_F
#define I2C_EDID_3 I2C_G
 


 void i2c_Init(I2CPort_TypeDef I2C_x); 
 BYTE i2cReadByte(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, BYTE ucSubAdr);
 void i2cWriteByte(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, BYTE ucSubAdr, BYTE ucVal);	
 void i2cBurstWriteBytes(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, BYTE ucSubAdr, BYTE *pBuf, u16 ucBufLen);
 void i2cBurstReadBytes(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, BYTE ucSubAdr, BYTE *pBuf, u16 ucBufLen);
 void i2cBurstWriteBytes2(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, u16 ucSubAdr, BYTE *pBuf, u16 ucBufLen);
 void i2cBurstReadBytes2(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, u16 ucSubAdr, BYTE *pBuf, u16 ucBufLen);


#endif
