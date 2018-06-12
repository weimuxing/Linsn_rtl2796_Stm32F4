///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IO_IT6350.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IO_IT6350.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _IO_IT6350_H_
#define _IO_IT6350_H_

#include "it6662/include/IT6662_config.h"

#if defined(_MCU_IT6350_)

//#ifdef  _MCU_ANDES_
//#include "include.h"

//#include "debug_print.h"

#define USING_I2C		(1)
#define USING_SMBUS	(1)

#define SMBusA				(3)
#define SMBusB				(4)
#define SMBusC				(5)
#define I2C_SMBusD			(0)
#define I2C_SMBusE			(1)
#define I2C_SMBusF			(2)


#ifdef _IO_IT6350_C_
typedef struct _IT6350_GPO_CONF{
	volatile unsigned char	*pu8Adr;
	iTE_u8	u8Mask;
	iTE_u8	u8Gpio;
}IT6350_GpoConf;

iTE_u8	_CODE LedAlloc[16] = { LED_0, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7, LED_8, LED_9, LED_A, LED_B, LED_C, LED_D, LED_E, LED_F};
IT6350_GpoConf	_CODE GpoConf[18] = {
	{0, 0, 0},
	{&GPDRB, BIT3, GPIOB3},
	{&GPDRB, BIT4, GPIOB4},
	{&GPDRF, BIT6, GPIOF6},
	{&GPDRF, BIT7, GPIOF7},
	{&GPDRA, BIT0, GPIOA0},
	{&GPDRA, BIT1, GPIOA1},
	{&GPDRG, BIT3, GPIOG3},
	{&GPDRG, BIT4, GPIOG4},
	{&GPDRG, BIT6, GPIOG6},
	{&GPDRA, BIT6, GPIOA6},
	{&GPDRI, BIT0, GPIOI0},
	{&GPDRI, BIT1, GPIOI1},
	{&GPDRI, BIT2, GPIOI2},
	{&GPDRI, BIT3, GPIOI3},
	{&GPDRI, BIT4, GPIOI4},
	{&GPDRI, BIT5, GPIOI5},
	{&GPDRG, BIT5, GPIOG5},
};
#endif

iTE_u1 i2c_read_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device );
iTE_u1 i2c_write_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device );
void ExtInt_Enable(iTE_u1 bEnable);
void mDelay(iTE_u16 Delay_Count);
void mSleep(iTE_u16 Delay_Count);
void GPO_Set(iTE_u16 u16LedSet);
//#endif
void MCU_Init();
iTE_u1 HOLD_STATUS(void);
iTE_u1 HDCP_REPEATER(void);
#endif

#endif		// #if defined(_MCU_IT6350_)