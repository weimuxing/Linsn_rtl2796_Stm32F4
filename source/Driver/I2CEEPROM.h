#ifndef	__I2CEEPROM_h__
#define	__I2CEEPROM_h__
//	write your header here
// define i2c eeprom type
#include "define.h"

#define AT24C512B 1
#define AT24C64	  2


//#define I2CEEPROM_TYPE AT24C512B
#define I2CEEPROM_TYPE AT24C64

typedef enum
{
	I2CEEPROM_PORTA,
	I2CEEPROM_PORTB,
	I2CEEPROM_PORTC,
	I2CEEPROM_PORTD,
	I2CEEPROM_PORTE,
	I2CEEPROM_PORTF,
	I2CEEPROM_PORTG,
	I2CEEPROM_PORTH,
	I2CEEPROM_PORTI,
} eepromPort_TypeDef;


#if (I2CEEPROM_TYPE == AT24C512B)
#define EEPROM_WRITE_PAGE 128
#define EEPROM_I2C_ADDRESS 0xA0
#define EEPROM_I2C_ADDRESS_V10 0xA6  //V1.1版本的eeprom地址
#define EEPROM_I2C_ADDRESS_V11 0xA8  //V1.2版本的eeprom地址

#define EEPROM_I2C_ADDRESS_TYPE 2   // 2 byte address
#define EEPROM_WRITE_TIME 5   // ms
#define EEPROM_SIZE		(512/8*1024)

#elif(I2CEEPROM_TYPE == AT24C64)
#define EEPROM_WRITE_PAGE 32
// Device Address 1010 A2 A1 A0 RW,Vip Smart A2 A1 A0 = 000[做了硬件修改]
//#define EEPROM_I2C_ADDRESS 0xA2
#define EEPROM_I2C_ADDRESS 0xA0
#define EEPROM_I2C_ADDRESS_V10 0xA0  //V1.0版本的eeprom地址
#define EEPROM_I2C_ADDRESS_V11 0xA4  //V1.1版本的eeprom地址
#define EEPROM_I2C_ADDRESS_TYPE 2   // 2 byte address
#define EEPROM_WRITE_TIME 5   // ms
#define EEPROM_SIZE		(64/8*1024)

#endif

void eeprom_write(eepromPort_TypeDef I2CEEPROM_PORTx , u16 address, BYTE * buffer, u16 size);
void eeprom_read(eepromPort_TypeDef I2CEEPROM_PORTx, u16 address, BYTE * buffer, u16 size);

char CheckEeprom(void);

#endif
