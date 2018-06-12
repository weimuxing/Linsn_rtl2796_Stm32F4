#ifndef _PCF_8563_H_
#define _PCF_8563_H_

//#include "type.h"
#include "define.h"
#include "i2c.h"

#define PCF8563_I2C_PORT I2C_B
#define PCF8563_I2C_ADDRESS 0xA2

#define PCF8563_Control1_address 0x00
#define PCF8563_CLKOUT_address 0x0d
#define PCF8563_Timer_address 0x0e

#define PCF8563_Control1_value 0x00

typedef struct
{
	u8 second; 	// BCD
	u8 minute;	// BCD
	u8 hour;	// BCD
	u8 day;		// BCD
	u8 weekday; // 0--Sunday,  6--Saturday
	u8 month;	// BCD
	u16 year;	// BCD
} Time_TypeDef;

void readReg_PCF8563(u8 address, u8 * buffer, u8 size);
void writeReg_PCF8563(u8 address, u8 * buffer, u8 size);

void init_PCF8563(void);
bool SetTime_PCF8563(Time_TypeDef * time);
bool SetCalendar_PCF8563(Time_TypeDef * time);
bool GetTime_PCF8563(Time_TypeDef *tiem);
int CompareTime(Time_TypeDef *time1, Time_TypeDef *time2);
bool isEqualTime(Time_TypeDef *time1, Time_TypeDef *time2);
void CopyTime(Time_TypeDef *from, Time_TypeDef *to);

char CheckPCF8563(void);
void pcf8563_test(void);

#endif

