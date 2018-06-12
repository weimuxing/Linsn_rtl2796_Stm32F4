///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_IT6350_Main.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_IT6350_Main.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#include <stdio.h>
#include "define.h"

#include "it6662/include/IT6662_Sys.h"
#if (defined _MCU_IT6350_)
#include "it6662/include/IO_IT6350.h"
#endif

//extern void ExtInt_Enable(iTE_u1 bEnable);
//extern UINT8 u8IntEvent;

//****************************************************************************
iTE_u8	CurSysStatus[2] = {0};
//extern UINT8 	u8IntEvent; //@lv 20160318
//UINT8 	u8IntEvent[2];
iTE_u8 	u8IntEvent[2];


//****************************************************************************
iTE_u1 HoldSystem(void)
{
	static iTE_u8	u8DumpReg = 0;
	#if 0 //@lv 20160318
	if(HOLD_STATUS()){
		if(u8DumpReg){
			iTE_u8	u8RegData[0x10], i;
			printf("HDMI_Rx Bank0:\r\n");
			u8RegData[0] = 0x00;
			i2c_write_byte(HDMI_RX_ADR, 0x0F, 1, u8RegData, 1);
			for(i=0; i < 0x10; i++){
				i2c_read_byte( HDMI_RX_ADR, i << 4, 0x10, u8RegData, 1);
				printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
					i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
			}
			printf("HDMI_Rx Bank1:\r\n");
			u8RegData[0] = 0x01;
			i2c_write_byte(HDMI_RX_ADR, 0x0F, 1, u8RegData, 1);
			for(i=0; i < 0x10; i++){
				i2c_read_byte( HDMI_RX_ADR, i << 4, 0x10, u8RegData, 1);
				printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
					i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
			}
			printf("HDMI_Rx Bank2:\r\n");
			u8RegData[0] = 0x02;
			i2c_write_byte(HDMI_RX_ADR, 0x0F, 1, u8RegData, 1);
			for(i=0; i < 0x10; i++){
				i2c_read_byte( HDMI_RX_ADR, i << 4, 0x10, u8RegData, 1);
				printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
					i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
			}
			u8RegData[0] = 0x00;
			i2c_write_byte(HDMI_RX_ADR, 0x0F, 1, u8RegData, 1);

			printf("MHL_Rx Bank:\r\n");
			for(i=0; i < 0x10; i++){
				i2c_read_byte( MHL_RX_ADR, i << 4, 0x10, u8RegData, 1);
				printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
					i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
			}

			printf("HDMI_Tx Bank0:\r\n");
			u8RegData[0] = 0x00;
			i2c_write_byte(HDMI_TX_ADR, 0x0F, 1, u8RegData, 1);
			for(i=0; i < 0x10; i++){
				i2c_read_byte( HDMI_TX_ADR, i << 4, 0x10, u8RegData, 1);
				printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
					i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
			}
			printf("HDMI_Tx Bank1:\r\n");
			u8RegData[0] = 0x01;
			i2c_write_byte(HDMI_TX_ADR, 0x0F, 1, u8RegData, 1);
			for(i=0; i < 0x10; i++){
				i2c_read_byte( HDMI_TX_ADR, i << 4, 0x10, u8RegData, 1);
				printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
					i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
			}
			u8RegData[0] = 0x00;
			i2c_write_byte(HDMI_TX_ADR, 0x0F, 1, u8RegData, 1);

			printf("EDID Bank:\r\n");
			for(i=0; i < 0x10; i++){
				i2c_read_byte( EDID_ADR, i << 4, 0x10, u8RegData, 1);
				printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
					i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
			}
			u8DumpReg = 0;
		}
	     	printf("Hold\\\r");
	     	printf("Hold-\r");
	     	printf("Hold/\r");
	     	printf("Hold|\r");
		return iTE_TRUE;
	}else{
		u8DumpReg = 1;
		return iTE_FALSE;
	}
	#else
	u8DumpReg = 1;
	return iTE_FALSE;
	#endif
}
//****************************************************************************
void IT6662_Main(u8 device)
{
	static u32 taskIT6662_sysTime = 0;
	if(!isSysTickTimeout(taskIT6662_sysTime,300))
	{
		return;
	}
	taskIT6662_sysTime = GetSysTick();
    

	if(CurSysStatus[device] == 0){
		//MCU_Init();
		CurSysStatus[device] = 1;
	}else	if(!HoldSystem()){
		switch(CurSysStatus[device]) {
			case 0x1:
					if(IT6662_SysInit()){
						CurSysStatus[device] = 2;
					}
				break;
			case 0x2:
					if(1 || u8IntEvent[device]){
						u8IntEvent[device] = 0;
						IT6662_SysIrq();
					}
				break;
			default:
					CurSysStatus[device]=0;
				break;
		}
	}
}

