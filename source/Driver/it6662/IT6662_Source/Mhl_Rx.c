///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Rx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Rx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#define _MHL_RX_INIT_TAB_
#include "it6662/include/Mhl_Rx.h"
#include "it6662/include/Hdmi_Rx.h"

//****************************************************************************
extern IT6662_Var	*stIt6662;
extern iTE_u8	g_u8MhlClkWait;
MSC_CMD TxMscCmd;
MHL3D_STATE e3DReqState = MHL3D_REQ_DONE;
//****************************************************************************
void MhlRx_Init(void)
{
	iTE_u8 u8Temp;
	iTE_MRx_Msg(("MhlRx_Init S\n"));
	cal_oclk();

	for(u8Temp=0; u8Temp < MHL_RX_INIT_TAB_SIZE; u8Temp++){
		MhlRx_Set(MHL_RX_INIT_TAB[u8Temp][0], MHL_RX_INIT_TAB[u8Temp][1], MHL_RX_INIT_TAB[u8Temp][2]);
	}
	iTE_MRx_Msg(("MhlRx_Init E\n"));
}
//****************************************************************************
void MhlRx_Enable(void)
{
	MhlRx_Set(0x2A, 0x01, 0x01);
	MhlRx_Set(0x0F, 0x10, 0x00);
}
//****************************************************************************
//#include "..\IO_Andes.h"
void cal_oclk(void)
{
	#define CAL_CNT (1)
	#define CAL_RETRY_CNT	(1)

	iTE_u32	u32OscClk = 0;
	iTE_u8	u8OscDiv;
	iTE_u16	u16Temp;
	iTE_u8	u8RdData[3];
	iTE_u8	u8Temp;
	iTE_u16	u16T10usInt, u16T10usFlt;

	u8Temp = 0;
	u16Temp = 0;

	do{
		MhlRx_W(0x01, 0x41);
		//GPDRG |= 0x08;
		u16Temp += iTE_BusyWait_ms(100);
		//GPDRG &= ~0x08;
		MhlRx_W(0x01, 0x40);

		MhlRx_Rb(0x12, 0x03, u8RdData);

		u32OscClk += (iTE_u32)u8RdData[0];
		u32OscClk += (iTE_u32)u8RdData[1]<<8;
		u32OscClk += (iTE_u32)u8RdData[2]<<16;
		iTE_MRx_Msg(("OSCCLK_0=0x%02x, %02x, %02x\n",(int)u8RdData[0], (int)u8RdData[1], (int)u8RdData[2]));
	}while(++u8Temp < CAL_CNT);

#if 0	//FW workaround
	if(u8RdData[2] > 0x60){
		u32OscClk >>= 1;
		iTE_MRx_Msg(("******************************************** IT6350 TIMER ERROR *********************\n"));
	}
#endif

	if(u16Temp == 0x00){
		iTE_MsgE(("Reg0x12~0x14 == 0x00\n"));
		return;
	}

	u32OscClk /= u16Temp;

 	iTE_MRx_Msg(("OSCCLK=%d.%dMHz\n", (int)(u32OscClk/1000), (int)(u32OscClk%1000)));

	u8Temp = (u32OscClk * 32 / 50000) + 1;
	HdmiRx_W(0x36, u8Temp);
	iTE_MRx_Msg(("HSThreshold = 0x%x\n", u8Temp));
	u8Temp = (u32OscClk * 21 / 50000) + 1;
	HdmiRx_Set(0x4E, 0x3F,  u8Temp);
	iTE_MRx_Msg(("HSThreshold2 = 0x%x\n", u8Temp));

	if(FixOCLKD4) {
		u8OscDiv = 4;
	} else if(FixOCLKD5) {
		u8OscDiv = 5;
	} else {
		u8OscDiv = u32OscClk / 10000 ;
		if(((u32OscClk/u8OscDiv)) > (2000*10 - u32OscClk/(u8OscDiv+1)))
			u8OscDiv++;
	}

	iTE_MRx_Msg(("osc_div = %d \n", (int)u8OscDiv));

	iTE_MRx_Msg(("OCLK=%d.%03dMHz\n", (int)(u32OscClk/u8OscDiv/1000), (int)((u32OscClk/u8OscDiv)%1000)));
	MhlRx_Set(0x01, 0x70, u8OscDiv<<4);
	if(PwrDnRCLK == iTE_FALSE){
		if(HDMITxRCLKFreqSel){
			stIt6662->u32RefClk = u32OscClk /2;		// 20MHz
		}else{
			stIt6662->u32RefClk = u32OscClk / 4;	// 10MHz
		}
	}else{
		if(RCLKPDSel == 0){
			stIt6662->u32RefClk = u32OscClk / 8;	// 5MHz
		}else{
			stIt6662->u32RefClk = u32OscClk / 16;	// 2.5MHz
		}
	}

	u16T10usInt = stIt6662->u32RefClk/100;
	u16T10usFlt = (stIt6662->u32RefClk%100) <<7;//* 128 ;
	u16T10usFlt /= 100 ;
     	MhlRx_W(0x02, (iTE_u8)(u16T10usInt&0xFF));
     	MhlRx_W(0x03, (iTE_u8)((u16T10usInt&0x100)>>1)+u16T10usFlt);
     	iTE_MRx_Msg(("RCLK=%3d.%03dMHz\n", (int)(stIt6662->u32RefClk/1000),(int)(stIt6662->u32RefClk%1000)));
     	iTE_MRx_Msg(("T10usInt=0x%03X, T10usFlt=0x%02X\n", (int)u16T10usInt, (int)u16T10usFlt));
}
//****************************************************************************
void Mhl_CBusIrq(void)
{
	iTE_u8	u8Reg04[3];
	iTE_u8	u8RdData;
	iTE_u8	u8RegTmp[2];

	MhlRx_Rb(0x04, 0x03, u8Reg04);
	MhlRx_Wb(0x04, 0x03, u8Reg04);

if(u8Reg04[0] ||u8Reg04[1] ||u8Reg04[2] ) {



//	iTE_MRx_Msg(("Mhl_CBusIrq S...\n"));
	if(u8Reg04[0]) {
		if(u8Reg04[0] & 0x01){
#if iTE_MhlRx_Msg_Print
			iTE_MRx_Msg(("CBUS Link Layer TX Packet Done Interrupt ...\n"));
			if(iTE_TRUE == EnCBusDbgMode){
				dbgcbustx();
			}
#endif
		}

		if(u8Reg04[0] & 0x02){
			iTE_MRx_Msg(("CBUS Link Layer TX Packet Fail Interrupt ...  \n"));
			u8RdData = MhlRx_R(0x15);
			MhlRx_W(0x15, u8RdData&0xF0);
			if( u8RdData&0x10 ) {			iTE_MRx_Msg(("TX Packet Fail when Retry > 32 times !!!\n"));		}
			if( u8RdData&0x20 ) {			iTE_MRx_Msg(("TX Packet TimeOut !!!\n"));						}
			if( u8RdData&0x40 ) {			iTE_MRx_Msg(("TX Initiator Arbitration Error !!!\n"));				}
			if( u8RdData&0x80 ) {			iTE_MRx_Msg(("CBUS Hang !!!\n"));							}
		}

		if(u8Reg04[0] & 0x04){
#if iTE_MhlRx_Msg_Print
			iTE_MRx_Msg(("CBUS Link Layer RX Packet Done Interrupt ...\n"));
			if(iTE_TRUE == EnCBusDbgMode){
				dbgcbusrx();
			}
#endif
		}

		if(u8Reg04[0] & 0x08){
			iTE_MRx_Msg(("CBUS Link Layer RX Packet Fail Interrupt ... \n"));
			u8RdData = MhlRx_R(0x15);
			MhlRx_W(0x15, u8RdData&0x0F);
			if( u8RdData&0x01 ) {			iTE_MRx_Msg(("RX Packet Fail !!!\n"));					}
			if( u8RdData&0x02 ) {
				iTE_MRx_Msg(("RX Packet TimeOut !!!\n"));
#if 0
				stIt6662->bCbusDiscov = 0;
		mhl_CBusReDiscovery();
/*
			MhlRx_Set(0x28, 0x50, 0x40);
			iTE_Sleep_ms(50);
			MhlRx_Set(0x28, 0x50, 0x00);
			iTE_Sleep_ms(10);
*/
#else
				HdmiRx_HpdClr(stIt6662->bRxHpdStatus);
#endif
			}
			if( u8RdData&0x04 ) {			iTE_MRx_Msg(("RX Parity Check Error !!!\n"));			}
			if( u8RdData&0x08 ) {			iTE_MRx_Msg(("Bi-Phase Error !!!\n"));					}
		}

		if(u8Reg04[0] & 0x10){
			iTE_MRx_Msg(("MSC RX MSC_MSG Interrupt ...\n"));
			Msc_MsgHandler();
		}

		if(u8Reg04[0] & 0x20) {		iTE_MRx_Msg(("MSC RX WRITE_STAT Interrupt ...\n"));			}
			if(u8Reg04[1] &0x10 ) {		iTE_MRx_Msg(("MHL Clock Mode Change to %s. \n", ((MhlRx_R(0xB1) & 0x07)==2)? " PackPixel Mode": "24 bit mode"));	}
			if(u8Reg04[2] & 0x10) {		iTE_MRx_Msg(("CBUS PATH_EN Change Interrupt ...\n PATH_EN = %d", (int)(MhlRx_R(0xB1) & 0x08)>>3));				}
			if(u8Reg04[2] & 0x20) {		iTE_MRx_Msg(("CBUS MUTE Change Interrupt ...\n Current CBUS MUTE Status = %d \n", (MhlRx_R(0xB1)&0x10)>>4));	}
			if(u8Reg04[2] & 0x40) {
				iTE_MRx_Msg(("CBUS DCapRdy Change Interrupt ...\n"));
				if(MhlRx_R(0xB0) & 0x01) {
//					Msc_Read_DevCap();
				} else {
					iTE_MRx_Msg(("DcapRdy change to 0\n"));
				}
			}

		if(u8Reg04[0] & 0x40) {
#if iTE_MhlRx_Msg_Print
			iTE_MRx_Msg(("MSC RX WRITE_BURST Interrupt ... count = %d\n", (int)MhlRx_R(0x64)));
			//printf("content of write_burst\n");
			for(u8RdData = 0; u8RdData < 16; u8RdData+=2){
				MhlRx_Rb(0xC0 + u8RdData, 0x02, u8RegTmp);
				iTE_MRx_Msg(("	0x%02X	0x%02X", u8RegTmp[0], u8RegTmp[1]));
			}
			iTE_MRx_Msg(("\n"));
#endif
		}

		if(u8Reg04[0] & 0x80) {		iTE_MRx_Msg(("CBus DDC Hang Interrupt ...\n"));			}
	}

	if(u8Reg04[1]) {
		if(u8Reg04[1] & 0x01 ) {		iTE_MRx_Msg(("MSC Req Done Interrupt ...\n"));			}
		if(u8Reg04[1] & 0x02 ) {
			MhlRx_Rb(0x18, 0x02, u8RegTmp);
			MhlRx_Wb(0x18, 0x02, u8RegTmp);
			if( MSC_RCVABORT == MHL_MscReqFailCheck(u8RegTmp)){
				if(Msc_Fire(MSC_PKT_GET_MSC_ERRORCODE, &TxMscCmd)==MSC_SUCCESS){
					iTE_MRx_Msg(("GET_MSC_ERRORCODE=0x%2X\n",(int)MhlRx_R(0x56)));
				}
			}
		}

		if(u8Reg04[1] & 0x04 ) {		iTE_MRx_Msg(("MSC Rpd Done Interrupt ...\n"));			}
		if(u8Reg04[1] & 0x08 ) {
			iTE_MRx_Msg(("MSC Rpd Fail Interrupt ...\n"));

			MhlRx_Rb(0x1A, 0x02, u8RegTmp);
			MhlRx_Wb(0x1A, 0x02, u8RegTmp);
			if( u8RegTmp[0]&0x01 ) {			iTE_MRx_Msg(("Initial Bad Offset !!!\n"));							}
			if( u8RegTmp[0]&0x02 ) {			iTE_MRx_Msg(("Incremental Bad Offset !!!\n"));						}
			if( u8RegTmp[0]&0x04 ) {			iTE_MRx_Msg(("Invalid Command !!!\n"));							}
			if( u8RegTmp[0]&0x08 ) {			iTE_MRx_Msg(("Receive dPacket in Responder Idle State !!!\n"));		}
			if( u8RegTmp[0]&0x10 ) {			iTE_MRx_Msg(("Incomplete Packet !!!\n"));							}
			if( u8RegTmp[0]&0x20 ) {			iTE_MRx_Msg(("100ms TimeOut !!!\n"));								}
			if( u8RegTmp[0]&0x40 ) {			iTE_MRx_Msg(("MSC_MSG Responder Busy ==> Return NACK Packet !!!\n"));		}
			if( u8RegTmp[0]&0x80 ) {			iTE_MRx_Msg(("Protocol Error !!!\n"));								}
			if( u8RegTmp[1]&0x01 ) {			iTE_MRx_Msg(("Retry > 32 times !!!\n"));							}
			if( u8RegTmp[1]&0x02 ) {
				iTE_MRx_Msg(("Receive ABORT Packet !!!\n"));
				iTE_Sleep_ms(WAITABORTNUM);
			}
			iTE_MRx_Msg(("\n"));
		}


	//	if(u8Reg04[1] &0x20 ) {			      	iTE_MRx_Msg(("DDC Req Fail Interrupt ...\n"));							}
//		if(u8Reg04[1] &0x40 ) {			iTE_MRx_Msg(("DDC Rpd Done Interrupt ...\n"));							}
		if(u8Reg04[1] &0x80 ) {
			iTE_MRx_Msg(("DDC Rpd Fail Interrupt ... \n"));
			//       printf("DDC Rpd Fail Status MHL Reg[16]=0x%02X\n", mhlrxrd(0x16));
			MhlRx_Rb(0x16, 0x02, u8RegTmp);
			MhlRx_Wb(0x16, 0x02, u8RegTmp);
			if( u8RegTmp[0]&0x01 ) {			iTE_MRx_Msg(("RxState=IDLE and Receive non-SOF Packet !!!\n"));		}
			if( u8RegTmp[0]&0x02 ) {			iTE_MRx_Msg(("RxState/=IDLE and Receive Unexpected Packet !!!\n"));	}
			if( u8RegTmp[0]&0x04 ) {			iTE_MRx_Msg(("100ms TimeOut !!!\n"));								}
			if( u8RegTmp[0]&0x08 ) {			iTE_MRx_Msg(("100ms TimeOut caused by Link Layer Error !!!\n"));		}
			if( u8RegTmp[0]&0x10 ) {			iTE_MRx_Msg(("Receive Unexpected STOP !!!\n"));					}
			if( u8RegTmp[0]&0x20 ) {			iTE_MRx_Msg(("Transmit Packet Fail !!!\n"));							}
			if( u8RegTmp[0]&0x40 ) {			iTE_MRx_Msg(("DDC Bus Hang !!!\n"));								}
			if( u8RegTmp[0]&0x80 ) {			iTE_MRx_Msg(("TxState/=IDLE, Receive new packet !!!\n"));			}
			if( u8RegTmp[1]&0x01 ) {			iTE_MRx_Msg(("TxState/=IDLE, Receive TxDDCArbLose !!!\n"));			}
		}
	}

	if(u8Reg04[2]) {
		if(u8Reg04[2] & 0x01) {
			iTE_MRx_Msg(("WakeUp Done Interrupt ...\n"));
			stIt6662->u16WakeFailCnt = 0;
			MhlRx_Set(0x28, 0x08, 0x00);		// Disable Discover Pulse Change
		}

		if(u8Reg04[2] & 0x02) {
			if((++stIt6662->u16WakeFailCnt & 0x07) == 0) {
				MhlRx_W(0x28, MhlRx_R(0x28) ^ 0x08);
			}
			iTE_MRx_Msg(("**************************************************************************\n"));
			iTE_MRx_Msg(("WakeUp Fail Interrupt ... %dth Fail !!!\n", stIt6662->u16WakeFailCnt));
			if(EnCBus1KTog && ((stIt6662->u16WakeFailCnt & 0x07)==0)){
				// CBus 1K change function
				HdmiRx_Bank(1);
				u8RdData= (HdmiRx_R(0xC0) & 0x1C) >> 2;

				HdmiRx_Set(0xC0, 0x1C, u8CBus1K[u8RdData]);
				HdmiRx_Bank(0);
				iTE_MRx_Msg(("WakeUp Fail: Change 1K pull-down to %d ...\n", u8RdData));
			}
			if(EnWakeFailSleep && ((stIt6662->u16WakeFailCnt % 4) == 0)){
				MhlRx_Set(0x0A, 0x02, 0x02);
				HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
				stIt6662->u8WakeFailSleepCnt = WAKE_FAIL_SLEEP_TIME;
			}
		}

		if(u8Reg04[2] & 0x04) {
			stIt6662->bCbusDiscov = 1;
			stIt6662->u8DisvFailCnt = 0;
			iTE_MRx_Msg(("CBUS Discovery Done Interrupt ...\n"));
			HdmiRx_Bank(1);
			HdmiRx_Set(0xC0, 0x1C, CBUS1KPD << 2);
			HdmiRx_Bank(0);

	//		MhlRx_W(0x08, (MaskRxPktDoneInt<<2)+MaskTxPktDoneInt);
	//		MhlRx_W(0x09, (MaskDDCDoneInt<<6)+(MaskDDCDoneInt<<4)+(MaskMSCDoneInt<<2)+MaskMSCDoneInt);
#if 1
			stIt6662->u8HpdTimerCnt = 0;	// ignor HPD control if CBUS re-discover
			g_u8MhlClkWait = 50;
			HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
			if(EnAutoCapRdy==iTE_FALSE ) {
				iTE_Sleep_ms(CBUSINITIME);
				TxMscCmd.ucOffset = 0x30;
				TxMscCmd.ucValue = MHL_STS_DCAP_RDY;
				if(Mhl_MscFire(MSC_PKT_WRITE_STAT, &TxMscCmd) != MSC_SUCCESS){return;}

				iTE_MRx_Msg(("Set CapRdy by FW ...\n"));
			}

			if( EnAutoPathEn==iTE_FALSE ) {
				iTE_Sleep_ms(CBUSINITIME);
				TxMscCmd.ucOffset = 0x31;
				TxMscCmd.ucValue = MHL_STS_PATH_EN;
				if(Mhl_MscFire(MSC_PKT_WRITE_STAT, &TxMscCmd)!=MSC_SUCCESS){return;}
				iTE_MRx_Msg(("Set PathEn by FW ...\n"));
			}

			if( EnAutoHPD==iTE_FALSE ) {
				iTE_Sleep_ms(CBUSINITIME*30);	  	// min 8 for Galxy note Read DevCap after DCAP_RDY
				if(Mhl_MscFire(MSC_PKT_SET_HPD, &TxMscCmd)!=MSC_SUCCESS){return;}
				iTE_MRx_Msg(("Set HPD by FW ...\n"));
			}
#else
			HdmiRx_HpdSet();
			stIt6662->u8HpdTimerCnt = 0;
#endif

		}

		if(u8Reg04[2] & 0x08) {	iTE_MRx_Msg(("CBUS Discovery Fail Interrupt ... %dth Fail !!!\n", stIt6662->u8DisvFailCnt++));	 /*mhl_CBusReDiscovery();*/}
		if(u8Reg04[2] & 0x80) {	iTE_MRx_Msg(("VBUS Status Change Interrupt ...\n Current VBUS Status = %d \n", (int)(MhlRx_R(0x10)&0x08)>>3));		}
	}
//	iTE_MRx_Msg(("Mhl_CBusIrq E...\n"));
}
}
//****************************************************************************

//****************************************************************************
#if iTE_MhlRx_Msg_Print
void dbgcbustx( void )
{
	iTE_MRx_Msg(("\n ----------------- CBus Tx Debug Msg -----------------\n"));
	mhl_DbgCBus(0x58);
	iTE_MRx_Msg(("\n ---------------------------------------------------\n"));
}
//****************************************************************************
void dbgcbusrx( void )
{
	iTE_MRx_Msg(("\n ----------------- CBus Rx Debug Msg -----------------\n"));
	mhl_DbgCBus(0x5A);
	iTE_MRx_Msg(("\n ---------------------------------------------------\n"));
}
//****************************************************************************
void mhl_DbgCBus(iTE_u8 ucTxRxReg)
{
	iTE_u8	ucTemp,ucCBusByteNo;
	iTE_u8	ucCBusData[0x20];

	ucCBusByteNo = (MhlRx_R(ucTxRxReg) >> 3);
	if(ucCBusByteNo){
		do{
			if(ucCBusByteNo > 16){
				iTE_MRx_Msg(("WARNING: ByteNo=%d > 16!!!\n", (int)ucCBusByteNo));
				ucCBusByteNo = 16;
			}
			iTE_MRx_Msg(("CBus Byte Number = %d\n",(int)ucCBusByteNo));
			MhlRx_Rb(ucTxRxReg, ucCBusByteNo*2, ucCBusData);
			for(ucTemp=0; ucTemp<ucCBusByteNo; ucTemp++){
				iTE_MRx_Msg(("CBus Byte[%d] = 0x%X%2X \n", (int)ucTemp,(int)(ucCBusData[ucTemp*2]&0x7),(int)ucCBusData[ucTemp*2+1]));
				switch(ucCBusData[ucTemp*2]&0x06){
					case	0x00:	iTE_MRx_Msg(("DDC "));				break;
					case	0x02:	iTE_MRx_Msg(("Vendor-Specific "));		break;
					case	0x04:	iTE_MRx_Msg(("MSC "));				break;
					case	0x06:	iTE_MRx_Msg(("Reserved "));			break;
				}
				if(ucCBusData[ucTemp*2] & 0x01){
					iTE_MRx_Msg(("Control Packet "));
					parse_ctrlpkt(&ucCBusData[ucTemp*2]);
				}else{
					iTE_MRx_Msg(("Data Packet =0x%2X \n",(int)ucCBusData[ucTemp*2+1]));
				}
			}
			ucCBusByteNo = (MhlRx_R(ucTxRxReg) >> 3);
		}while(ucCBusByteNo);
	}
}
//****************************************************************************
void parse_ctrlpkt( iTE_u8* ctrlpkt )
{
    if( (ctrlpkt[0]&0x07)==0x05 ) {
        switch( ctrlpkt[1] ) {
        case 0x32: iTE_MRx_Msg((" ==> EOF \n")); break;
        case 0x33: iTE_MRx_Msg((" ==> ACK \n")); break;
        case 0x34: iTE_MRx_Msg((" ==> NACK \n")); break;
        case 0x35: iTE_MRx_Msg((" ==> ABORT \n")); break;
        case 0x60: iTE_MRx_Msg((" ==> WRITE_STATE or SET_INT \n")); break;
        case 0x61: iTE_MRx_Msg((" ==> READ_DEVCAP \n")); break;
        case 0x62: iTE_MRx_Msg((" ==> GET_STATE \n")); break;
        case 0x63: iTE_MRx_Msg((" ==> GET_VENDOR_ID \n")); break;
        case 0x64: iTE_MRx_Msg((" ==> SET_HPD \n")); break;
        case 0x65: iTE_MRx_Msg((" ==> CLR_HPD \n")); break;
        case 0x68: iTE_MRx_Msg((" ==> MSC_MSG \n")); break;
        case 0x69: iTE_MRx_Msg((" ==> GET_SC1_ERRORCODE \n")); break;
        case 0x6A: iTE_MRx_Msg((" ==> GET_DDC_ERRORCODE \n")); break;
        case 0x6B: iTE_MRx_Msg((" ==> GET_MSC_ERRORCODE \n")); break;
        case 0x6C: iTE_MRx_Msg((" ==> WRITE_BURST \n")); break;
        case 0x6D: iTE_MRx_Msg((" ==> GET_SC3_ERRORCODE \n")); break;
        default  : iTE_MRx_Msg((" ==> ERROR !!! \n"));
        }
    }
    else if( (ctrlpkt[0]&0x07)==0x01 ) {
        switch( ctrlpkt[1] ) {
        case 0x30: iTE_MRx_Msg((" ==> SOF \n")); break;
        case 0x32: iTE_MRx_Msg((" ==> EOF \n")); break;
        case 0x33: iTE_MRx_Msg((" ==> ACK \n")); break;
        case 0x34: iTE_MRx_Msg((" ==> NACK \n")); break;
        case 0x35: iTE_MRx_Msg((" ==> ABORT \n")); break;
        case 0x50: iTE_MRx_Msg((" ==> CONT \n")); break;
        case 0x51: iTE_MRx_Msg((" ==> STOP \n")); break;
        default  : iTE_MRx_Msg((" ==> ERROR !!! \n"));
        }
    }
    else
        iTE_MRx_Msg((" ==> ERROR !!!\n"));
}
#endif
//****************************************************************************

//****************************************************************************
void Mhl_MscIrq(void)
{
	iTE_u8	u8RegA0[4];


	MhlRx_Rb(0xA0, 4, u8RegA0);
	MhlRx_Wb(0xA0, 4, u8RegA0);

if(u8RegA0[0] ||u8RegA0[1] ||u8RegA0[2] ||u8RegA0[3]) {
	iTE_MRx_Msg(("Mhl_MscIrq S...\n"));



	if(u8RegA0[0]) {
		if(u8RegA0[0] & 0x01){        	iTE_MRx_Msg(("MHL Device Capability Change Interrupt ...\n"));     	}
		if(u8RegA0[0] & 0x02){		iTE_MRx_Msg(("MHL DSCR_CHG Interrupt ...\n"));					}
		if(u8RegA0[0] & 0x04){         iTE_MRx_Msg(("MHL REQ_WRT Interrupt ...\n"));					}
		if(u8RegA0[0] & 0x08){
			iTE_MRx_Msg(("MHL GNT_WRT Interrupt ...\n"));
			if(e3DReqState == MHL3D_REQ_WRT){
				e3DReqState = MHL3D_GNT_WRT;
			}
		}
		if(u8RegA0[0] & 0x10){
			iTE_MRx_Msg(("MHL 3D_REQ Interrupt ...\n"));
			if((MHLVer == 0x20) || (MHLVer==0x21)){
				if(e3DReqState == MHL3D_REQ_DONE){
					e3DReqState = MHL3D_REQ_START;
				}
			}
		}

		if(e3DReqState != MHL3D_REQ_DONE){
			Msc_3DProcess(&e3DReqState);
		}

		if(u8RegA0[0] & 0xE0){		iTE_MRx_Msg(("ERROR: MHLA0[7:5] Reserved Interrupt ...\n"));		}
	}
	if(u8RegA0[1] & 0x01){		iTE_MRx_Msg(("ERROR: MHLA1[0] Reserved Interrupt ...\n"));		}
	if(u8RegA0[1] & 0x02){		iTE_MRx_Msg(("MHL EDID Change Interrupt ...\n"));				}
	if(u8RegA0[1] & 0xFC){		iTE_MRx_Msg(("ERROR: MHLA1[7:2] Reserved Interrupt ...\n"));		}
	if(u8RegA0[2] & 0xFF){		iTE_MRx_Msg(("ERROR: MHLA2[7:0] Reserved Interrupt ...\n"));		}
	if(u8RegA0[3] & 0xFF){		iTE_MRx_Msg(("ERROR: MHLA3[7:0] Reserved Interrupt ...\n"));		}

	iTE_MRx_Msg(("Mhl_MscIrq E...\n"));
}
}
//****************************************************************************
MSC_STATUS Mhl_MscFire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd)
{
#define MSC_RETRY_MAX	(0x20)
	iTE_u8	u8MscRetryCnt = 0;

	if(stIt6662->bCbusDiscov){
		while((Msc_Fire(eMscCtlPkt, MscCmd) != MSC_SUCCESS) && (++u8MscRetryCnt < MSC_RETRY_MAX));
		if(u8MscRetryCnt < MSC_RETRY_MAX){
			return MSC_SUCCESS;
		}else{
			HdmiRx_HpdClr(stIt6662->bRxHpdStatus);
		}
	}
	return MSC_FAIL;
}
//****************************************************************************

