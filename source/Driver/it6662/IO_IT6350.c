///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IO_IT6350.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IO_IT6350.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
//#define _IO_IT6350_C_
#include "it6662/include/IO_IT6350.h"

//#ifdef  _MCU_ANDES_

#if defined(_MCU_IT6350_)


//#include "..\..\api\i2c\i2c_drv.h"
HANDLE hDrv;
extern 	iTE_u8	u8IntEvent;
extern 	iTE_u8	g_u8CurDev;
void MCU_Init()
{
	#if  0//@lv 20160318
//	DisableAllInterrupt();
//	Init_Timers();


	#ifdef __ENABLE_DBG_MSG__
	Uart_Init();
	#endif

	#if USING_I2C
	i2c_drv_init(I2C_SMBusD);
	i2c_drv_init(I2C_SMBusE);
	i2c_drv_init(I2C_SMBusF);
	#endif

	#if USING_SMBUS
	ResetSMBus(SMBusA - SMBusA);
	ResetSMBus(SMBusC - SMBusA);
	iTE_Msg((" 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", SMB4P7USL,  SMB4P0USH, SMB300NS, SMB250NS, SMB25MS, SMB45P3USL, SMB45P3USH, SMB4P7A4P0H));
	#endif

	GPIO_Operation_Mode(GPIOC3, OUTPUT, OutputType_Push_Pull);
	GPDRC &= 0xF7;

	GPIO_Operation_Mode(GpoConf[LED_0].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_1].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_2].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_3].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_4].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_5].u8Gpio, OUTPUT, OutputType_Push_Pull);

	GPIO_Operation_Mode(GpoConf[LED_6].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_7].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_8].u8Gpio, OUTPUT, OutputType_Push_Pull);
	GPIO_Operation_Mode(GpoConf[LED_9].u8Gpio, OUTPUT, OutputType_Push_Pull);

	GPIO_Operation_Mode(GPIOG7, INPUT | PULL_UP, OutputType_Push_Pull);
	GPIO_Operation_Mode(GPIOC5, INPUT | PULL_UP, OutputType_Push_Pull);
#if (PCB_VER == 11)
	GPIO_Operation_Mode(GPIOA0, INPUT | PULL_UP, OutputType_Push_Pull);
	GPIO_Operation_Mode(GPIOA1, INPUT | PULL_UP, OutputType_Push_Pull);
	GPIO_Operation_Mode(GPIOA6, INPUT | PULL_UP, OutputType_Push_Pull);

	INTC_Enable_INTx(Interrupt_INT91, INT_Trigger_Mode_Set_FallingEdge);
	INTC_Enable_INTx(Interrupt_INT96, INT_Trigger_Mode_Set_FallingEdge);
	INTC_Enable_INTx(Interrupt_INT97, INT_Trigger_Mode_Set_FallingEdge);

	WUC_Enable_WUx_Interrupt(WU83, WUC_Falling);
	WUC_Enable_WUx_Interrupt(WU91, WUC_Falling);
	WUC_Enable_WUx_Interrupt(WU92, WUC_Falling);
#else
	GPIO_Operation_Mode(GPIOI0, INPUT | PULL_UP, OutputType_Push_Pull);
	GPIO_Operation_Mode(GPIOI1, INPUT | PULL_UP, OutputType_Push_Pull);
	GPIO_Operation_Mode(GPIOI2, INPUT | PULL_UP, OutputType_Push_Pull);

	INTC_Enable_INTx(Interrupt_INT124, INT_Trigger_Mode_Set_FallingEdge);
	INTC_Enable_INTx(Interrupt_INT125, INT_Trigger_Mode_Set_FallingEdge);
	INTC_Enable_INTx(Interrupt_INT126, INT_Trigger_Mode_Set_FallingEdge);

	WUC_Enable_WUx_Interrupt(WU119, WUC_Falling);
	WUC_Enable_WUx_Interrupt(WU120, WUC_Falling);
	WUC_Enable_WUx_Interrupt(WU121, WUC_Falling);
#endif
	GPO_Set(0);
	mSleep(10);
	GPDRC |= 0x08;

//	InitEnableInterrupt();

	iTE_Msg((" I2C CTR:0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", I2C_CTR_F, I2C_CTR1_F, I2C_CTR2_F, I2C_CTR3_F, I2C_CTR4_F, I2C_CTR5_F));
	#endif
}

extern UINT8 i2c_block_dma_read(UINT8 ucCH, UINT8 ucSlaveID, UINT8* pInputBuf, UINT8 ucWCount, UINT8* pOutputBuf, UINT8 ucRCount);
extern UINT8 i2c_block_dma_write(UINT8 ucCH,UINT8 ucSlaveID, UINT8 ucAddr, UINT8* pInputBuf, UINT8 ucWCount);

iTE_u1 i2c_read_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device )
{
	if(device <= I2C_SMBusF){
#if USING_I2C
//	if(i2c_pio_block_read(device, address, &offset, 1, p_data, byteno)){
	if(i2c_block_dma_read(device, address, &offset, 1, p_data, byteno)){
		iTE_MsgE(("i2c_read_byte fail: device = 0x%02X, addr=0x%02X, offset=0x%02X, num=0x%02X\n", device, address, offset, byteno));
//		while(1);
		return 0;
	}else{
		return 1;
	}
#if 0
	if(i2c_drv_data_read(hDrv, address, offset, p_data, byteno) == -1){
		iTE_MsgE(("i2c_read_byte fail\r\n"));
		i2c_drv_reset_cmd(I2CBus);
		return 0;
	}else{
		return 1;
	}
#endif
#endif
	}else{
#if USING_SMBUS
		if(byteno > 1){
#if 1
			if(bSMBusSendByte(device - SMBusA, address, offset)){
				if(bI2C_comp_Rd_Block(device - SMBusA, address, byteno, p_data, 0)){
	//			if( bRSMBusBlock(device -3, SMbusRBK, address,offset,p_data)){
					return 1;
				}else{
				}
			}
#else
			iTE_u8 u8Cnt;

			for(u8Cnt = 0; u8Cnt < byteno; u8Cnt++){
				if(bRWSMBus(device - SMBusA,SMbusRB,address,offset,p_data, 0)){
				}else{
					break;
				}
			}
			if(u8Cnt == byteno){
				return 1;
			}
#endif
		}else{
			if(bRWSMBus(device - SMBusA,SMbusRB,address,offset,p_data, 0)){
				return 1;
			}else{
			}
		}
		iTE_MsgE(("i2c_read_byte fail: device = 0x%02X, addr=0x%02X, offset=0x%02X, num=0x%02X, data=0x%02X\n", device, address, offset, byteno, *p_data));
#endif
		return 0;
	}

}
iTE_u1 i2c_write_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device )
{
	if(device <= I2C_SMBusF){
#if USING_I2C
//	if(i2c_pio_block_write(device, address, offset, p_data, byteno) ){
	if(byteno > 0xFE){	iTE_MsgE(("I2C Max. write Length is 254\r\n")); return 0;	}
	if(i2c_block_dma_write(device, address, offset, p_data, byteno) ){
		iTE_MsgE(("i2c_write_byte fail: device = 0x%02X, addr=0x%02X, offset=0x%02X, num=0x%02X, data=0x%02X\n", device, address, offset, byteno, *p_data));
//		while(1);
		return 0;
	}else{
		return 1;
	}
#if 0
	if(i2c_drv_data_write(hDrv, address, offset, p_data, byteno) == -1){
		iTE_MsgE(("i2c_write_byte fail\r\n"));
		i2c_drv_reset_cmd(I2CBus);
		return 0;
	}else{
		return 1;
	}
#endif
#endif
	}else{
#if USING_SMBUS
		if(byteno > 1){
			if( bWSMBusBlock(device - SMBusA, SMbusWBK, address,offset,p_data, byteno, 0, 1)){
				return 1;
			}else{

			}
		}else{
			if(bRWSMBus(device - SMBusA,SMbusWB,address,offset,p_data, 0)){
				return 1;
			}else{
			}
		}
		iTE_MsgE(("i2c_write_byte fail: device = 0x%02X, addr=0x%02X, offset=0x%02X, num=0x%02X, data=0x%02X\n", device, address, offset, byteno, *p_data));
#endif
		return 0;
	}
}

void ExtInt_Enable(iTE_u1 bEnable)
{
	iTE_u8	ucPaSta;

#if (PCB_VER == 11)
//	iTE_Msg(("ExtInt_Enable\n"));
	if(bEnable){
		ucPaSta = GPDRA & 0x43;
//		iTE_Msg(("ucPaSta=0x%x\r\n", ucPaSta));
		if(ucPaSta < 0x40){
			u8IntEvent |= 0x07 - ucPaSta;
		}else{
			u8IntEvent |= 0x43 - ucPaSta;
//			IER11 |= 1 << (Interrupt_INT91 % 8);
WUC_Enable_WUx_Interrupt(WU83, WUC_Falling);
//	INTC_Enable_INTx(Interrupt_INT91, INT_Trigger_Mode_Set_FallingEdge);
//		iTE_Msg(("En_INT91_IT6662_B\r\n"));
		}
		if((u8IntEvent & 0x01) == 0){
//			IER12 |= 1 << (Interrupt_INT96 % 8);
WUC_Enable_WUx_Interrupt(WU91, WUC_Falling);
//	INTC_Enable_INTx(Interrupt_INT96, INT_Trigger_Mode_Set_FallingEdge);
//		iTE_Msg(("En_INT96_IT6662_C\r\n"));
		}
		if((u8IntEvent & 0x02) == 0){
//			IER12 |= 1 << (Interrupt_INT97 % 8);
WUC_Enable_WUx_Interrupt(WU92, WUC_Falling);
//	INTC_Enable_INTx(Interrupt_INT97, INT_Trigger_Mode_Set_FallingEdge);
//		iTE_Msg(("En_INT97_IT6662_A\r\n"));
		}
//		iTE_Msg(("u8IntEvent=0x%x\r\n", u8IntEvent));
	}else{
		IER11 &= ~(1 << (Interrupt_INT91 % 8));
		IER12 &= ~((1 << (Interrupt_INT96 % 8)) |(1 << (Interrupt_INT97 % 8)));
//		bIntEvent = iTE_FALSE;
	}
#else
	if(bEnable){
		ucPaSta = GPDRI & 0x07;

		u8IntEvent |= 0x07 - ucPaSta;
		if((u8IntEvent & 0x04) == 0){
			WUC_Enable_WUx_Interrupt(WU121, WUC_Falling);
//			iTE_Msg(("En_INT91_IT6662_B\r\n"));
		}
		if((u8IntEvent & 0x01) == 0){
			WUC_Enable_WUx_Interrupt(WU119, WUC_Falling);
//			iTE_Msg(("En_INT96_IT6662_C\r\n"));
		}
		if((u8IntEvent & 0x02) == 0){
			WUC_Enable_WUx_Interrupt(WU120, WUC_Falling);
//			iTE_Msg(("En_INT97_IT6662_A\r\n"));
		}
//		iTE_Msg(("ucPaSta=0x%x, u8IntEvent=0x%x\r\n", ucPaSta, u8IntEvent));
	}else{
		IER15 &= ~((1 << (Interrupt_INT124 % 8)) |(1 << (Interrupt_INT125 % 8)) |(1 << (Interrupt_INT126 % 8)));
	}
#endif
}

void mDelay(iTE_u16 Delay_Count)
{
	DelayXms(Delay_Count);
}
void mSleep(iTE_u16 Delay_Count)
{
	DelayXms(Delay_Count);
}

void GPO_Set(iTE_u16 u16LedSet)
{
	iTE_u8 u8Temp;
	volatile unsigned char *ps8LedAdr;
	iTE_u8 u8LedMask;
//	GPIO_Operation_Mode(GPIOF6, OUTPUT, OutputType_Push_Pull);
	for(u8Temp = 0; u8Temp < 16; u8Temp++){
		ps8LedAdr = GpoConf[LedAlloc[u8Temp]].pu8Adr;
		if(ps8LedAdr){
			u8LedMask = GpoConf[LedAlloc[u8Temp]].u8Mask;
			if(u16LedSet & (1 << u8Temp)){
				*ps8LedAdr |= u8LedMask;
			}else{
				*ps8LedAdr &= ~u8LedMask;
			}
		}
	}
}

iTE_u1 HOLD_STATUS(void)
{
//	if(GPDRG & 0x80)
	if(KSI & 0x80)
		return 0;
	else
		return 1;
}

iTE_u1 HDCP_REPEATER(void)
{
	if(KSI & 0x40)
		return 0;
	else
		return 1;
}
//#endif
#endif		// #if defined(_MCU_IT6350_)

