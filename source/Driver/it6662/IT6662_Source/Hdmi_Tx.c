///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Hdmi_Tx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Hdmi_Tx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/

#include "it6662/include/Hdmi_Rx.h"
#define HDMI_TX_INIT_TABLE
#include "it6662/include/Hdmi_Tx.h"
//****************************************************************************
iTE_u8 g_u8TxSel = 0;
iTE_u8 g_u8TxOffset = 0;
iTE_u8 g_u8TxBank = 0;
iTE_u8 g_u8TxShift = 0;

#if (USING_AUTO_EQ == iTE_TRUE)
extern iTE_u8 g_u8VdoParChgCnt;
#endif
extern IT6662_Var	*stIt6662;
extern iTE_u8 g_u8CurDev;
extern iTE_u1 g_u1HdcpEn;
extern iTE_u8 g_u8InClr;
extern EdidInfo*	g_stEdidInfo;
extern iTE_u8 g_u8EqChkCnt;
extern iTE_u1 g_bAutoEqDone;
#if (USING_1to8 == iTE_TRUE)
extern iTE_u8 u8I2CAdr;
extern void IT6662_DeviceSelect(iTE_u8 u8Device);
#endif
//****************************************************************************
void HdmiTx_EnCec(void)
{
	HdmiTx_Set(0x06, 0x20, 0x00);
}
//****************************************************************************
void HdmiTx_Init(void)
{
	iTE_u8 u8Temp;
	iTE_HRx_Msg(("HdmiTx_Init S\n"));
	for(u8Temp=0; u8Temp < HDMI_TX_INIT_TAB_SIZE; u8Temp++){
		HdmiTx_Set(HDMI_TX_INIT_TAB[u8Temp][0], HDMI_TX_INIT_TAB[u8Temp][1], HDMI_TX_INIT_TAB[u8Temp][2]);
	}

#if (USING_CEC == iTE_TRUE)
	HdmiTx_W(0xF0, CEC_ADR + CEC_TX_SEL + 0x01 + u8I2CAdr);
#endif
	iTE_HRx_Msg(("HdmiTx_Init E\n"));
}
//****************************************************************************
void HdmiTx_Select(iTE_u8 u8HdmiTxSel)
{
	g_u8TxSel = u8HdmiTxSel;

	if(g_u8TxSel & 0x01){
		g_u8TxOffset = 0x40;
	}else{
		g_u8TxOffset = 0x00;
	}

	if(g_u8TxSel & 0x02){
		g_u8TxBank = 1;
	}else{
		g_u8TxBank = 0;
	}

	g_u8TxShift = 0x01 << u8HdmiTxSel;
}
//****************************************************************************
void HdmiTx_Irq(void)
{
	iTE_u8 u8Reg08;
	iTE_u8 u8Tmp;
	iTE_u32 u32HDMITxPCLK;

	u8Reg08 = HdmiTx_R(0x08);
	HdmiTx_W(0x08, u8Reg08 & 0xFB);

	if(u8Reg08 && (u8Reg08 !=0x08) && (u8Reg08 != 0x10)) {
		iTE_HTx_Msg(("HdmiTx_Irq S 0x%02X\n", u8Reg08));

		if(u8Reg08 & 0x04) {
			u8Tmp = HdmiTx_R(0x21)&0xF0;

			iTE_MEQ_Msg(("Video Parameter Change Interrupt ... 0x%02X\n", u8Tmp));
			switch( u8Tmp ) {
				case 0x00: iTE_HTx_Msg(("No Video Parameter Change !!!\n\n")); 		break;
				case 0x10: iTE_HTx_Msg(("VSync2nd Width Change !!!\n\n")); 			break;
				case 0x20: iTE_HTx_Msg(("VSync Width Change !!!\n\n")); 			break;
				case 0x30: iTE_HTx_Msg(("VDE2nd End Change !!!\n\n")); 				break;
				case 0x40: iTE_HTx_Msg(("VDE2nd Start Change !!!\n\n")); 			break;
				case 0x50: iTE_HTx_Msg(("VSync2nd Start Change !!!\n\n")); 			break;
				case 0x60: iTE_HTx_Msg(("VDE End Change !!!\n\n")); 				break;
				case 0x70: iTE_HTx_Msg(("VDE Start Change !!!\n\n")); 				break;
				case 0x80: iTE_HTx_Msg(("Field2nd VRRise Change !!!\n\n")); 			break;
				case 0x90: iTE_HTx_Msg(("HSync Width Change !!!\n\n")); 			break;
				case 0xA0: iTE_HTx_Msg(("HDE End Change !!!\n\n")); 				break;
				case 0xB0: iTE_HTx_Msg(("HDE Start Change !!!\n\n")); 				break;
				case 0xC0: iTE_HTx_Msg(("Line Cnt Change !!!\n\n")); 				break;
				case 0xD0: iTE_HTx_Msg(("Pixel Cnt Change !!!\n\n")); 				break;
				default:															break;
			}
			HdmiTx_W(0x08, 0x04);

#if (USING_AUTO_EQ == iTE_TRUE)
			if(g_u8CurDev == IT6662_ROOT){
				HdmiRx_EqChk();
						g_u8VdoParChgCnt++;
			}
#endif
		}

		if(u8Reg08 & 0x02) {
			iTE_HTx_Msg(("Input Video Is UnStable, 0x%02X\n", HdmiRx_R(0x07)));
//			HdmiTx_EnTmds();
			HdmiTx_Set(0x0A, 0x10, 0x10);		// disable Tx FIFO error
			HdmiTx_Set(0xC1, stIt6662->u8TxPortEn, 0x0F);
		}

		if(u8Reg08 & 0x01){
//			iTE_HTx_Msg(("abc\n"));
//			iTE_Sleep_ms(10);
			iTE_HTx_Msg(("Input Video Is Stable, 0x%02X\n", HdmiRx_R(0x07)));

			u32HDMITxPCLK = cal_pclk();
#if 0
			if(u32HDMITxPCLK > 80000) {				//Set HDMI Tx AFE
#else
			if(HdmiRx_R(0x0A) & 0x10) {
#endif
//				HdmiTx_Set(0xC6, 0x92, 0x80);
				HdmiTx_W(0xC6, 0x88);
			}else{
//				HdmiTx_Set(0xC6, 0x92, 0x12);
				HdmiTx_W(0xC6, 0x1A);
			}

			HdmiTx_EnTmds();
			if((HdmiTx_R(0xC1) & 0x0F) == 0x0F){
//				iTE_HTx_Msg(("********************************************ERROR:************************\n"));
			}

			if(USING_FORCE_TX_HDCP_ON || g_u1HdcpEn){
				if((HdmiTx_R(0x10) & 0x03) == 0x03){
					if(stIt6662->bRxTrgTxHdcp == iTE_FALSE){
						stIt6662->bRxTrgTxHdcp = iTE_TRUE;
						if((USING_CASCADE == iTE_TRUE) || (g_u8CurDev != IT6662_ROOT)){
							HdmiTx_AuthHdcp();
						}
					}
				}
			}
		}



		if(u8Reg08 & 0x10) {
//			iTE_HTx_Msg(("HDMI Tx FIFO error ...\n"));
			HdmiTx_Set(0x18, 0x02, 0x02 );
			HdmiTx_Set(0x18, 0x02, 0x00 );
//			iTE_HTx_Msg(("\n Reset HDMI Tx FIFO ...\n"));
		}

		if(u8Reg08 & 0x20) {
//			iTE_HTx_Msg(("HDMI Tx FIFO Over Flow error ...\n"));
			HdmiTx_Set(0x18, 0x02, 0x02 );
			HdmiTx_Set(0x18, 0x02, 0x00 );
//			iTE_HTx_Msg(("Reset HDMI Tx FIFO ...\n"));
		}

		if(u8Reg08 & 0x40) {
			iTE_HTx_Msg(("HDMI Tx Packet FIFO Auto Reset ...\n"));
			u8Tmp = 0;
			if(((HdmiTx_R(0x48) & 0x03) == 0x03) || ((HdmiTx_R(0x88) & 0x03) == 0x03)){
				u8Tmp = 1;
			}
			HdmiTx_Bank(1);
			if(((HdmiTx_R(0x48) & 0x03) == 0x03) || ((HdmiTx_R(0x88) & 0x03) == 0x03)){
				u8Tmp = 1;
			}
			HdmiTx_Bank(0);
			if(u8Tmp){
				HdmiTx_Set(0x1A, 0x10, 0x10);
				iTE_Sleep_ms(1);
				HdmiTx_Set(0x1A, 0x10, 0x00);
				iTE_HTx_Msg(("\n Reset HDMI Tx Packet FIFO ...\n"));
			}
		}
#if 0
		if(((HdmiTx_R(0x48) & 0x03) != 0x03) && ((HdmiTx_R(0x88) & 0x03) != 0x03)){
			HdmiTx_Bank(1);
			if(((HdmiTx_R(0x48) & 0x03) != 0x03) && ((HdmiTx_R(0x88) & 0x03) != 0x03)){
				HdmiTx_Bank(0);
				HdmiTx_Set(0x1A, 0x10, 0x10);
				HdmiTx_Set(0x04, 0x0F, 0x0F);
				HdmiTx_Set(0x19, 0x0F, 0x00);
				HdmiTx_Set(0xC1, 0x0F, 0x0F);
			}
			HdmiTx_Bank(0);
		}
#endif
		iTE_HTx_Msg(("HdmiTx_Irq E \n"));
	}
	u8Tmp = HdmiTx_R(0x10);
/*
	if(u8Tmp & 0x01) {
		HdmiTx_Set(0x05, 0x0C, 0x00);
	}else{
		HdmiTx_Set(0x05, 0x0C, 0x0C);
	}
*/
#if (USING_IT6661 == iTE_FALSE)
	if(A0_WorkAround || u8Tmp & 0x10) {
		HdmiTx_Select(HDMI_TX_A);
		HdmiTx_PortIRQ();
	}
#endif
	if(A0_WorkAround || u8Tmp & 0x20) {
		HdmiTx_Select(HDMI_TX_B);
		HdmiTx_PortIRQ();
	}
	if(A0_WorkAround || u8Tmp & 0x40) {
		HdmiTx_Select(HDMI_TX_C);
		HdmiTx_PortIRQ();
	}
	if(A0_WorkAround || u8Tmp & 0x80) {
		HdmiTx_Select(HDMI_TX_D);
		HdmiTx_PortIRQ();
	}

#if 0	// move HdmiTx_PreSpenHdcp from HdmiTx_PortEnable to here
	if(stIt6662->u8TxSpenHdcp){
		iTE_Sleep_ms(10);
		for(u8Tmp = 0; u8Tmp < 0x04; u8Tmp++){
			if(stIt6662->u8TxSpenHdcp & (1<<u8Tmp)){
				HdmiTx_Select(u8Tmp);
				stIt6662->u8TxDoHdcp |= g_u8TxShift;
//				stIt6662->u8HDCPFireCnt[g_u8TxSel]=0;
				HdmiTx_PreSpenHdcp(iTE_TRUE);
			}
		}
	}
#endif
	HdmiTx_Select(HDMI_TX_A);			//??
#if 0
	if(stIt6662->u8TxHpdStatus){
//		HdmiRx_HpdSet();
	}else{
		HdmiRx_HpdClr();
	}
//	iTE_HTx_Msg(("==============================================\n"));
#endif

}
//****************************************************************************
iTE_u32 cal_pclk(void)
{
	iTE_u32	u32PClk;
	iTE_u16	u16Temp;
	iTE_u8	u8PreDivSel;
#if 0//(USING_CEC == iTE_FALSE)
	HdmiTx_Set(0x06, 0x20, 0x00);
	Cec_W(0x0C, stIt6662->u32RefClk * 16 / 10);

	HdmiTx_Set(0x06, 0x20, 0x20);
#endif
	HdmiTx_Set(0x14, 0xF0, 0x80);
	iTE_Sleep_ms(1);			// at least 100us
	HdmiTx_Set(0x14, 0xF0, 0x00);
	iTE_Sleep_ms(1);			// at least 100us

	u16Temp = (HdmiTx_R(0x14) & 0x0F) << 8;
	u16Temp += HdmiTx_R(0x15);
//	iTE_HTx_Msg(("u16Temp = 0x%04X\n", u16Temp));
	u8PreDivSel = 0;
	while(u16Temp >> u8PreDivSel){
		u8PreDivSel++;
	};

	if(u8PreDivSel > 7) {
		u8PreDivSel = 0;
	} else {
		u8PreDivSel = 7 - u8PreDivSel;
	}
//	iTE_HTx_Msg(("u8PreDivSel = 0x%02X\n", u8PreDivSel));
	HdmiTx_Set(0x14, 0xF0, 0x80 | (u8PreDivSel << 4));
	iTE_Sleep_ms(1);			// at least 100us
	HdmiTx_Set(0x14, 0x80, 0x00);
	iTE_Sleep_ms(1);			// at least 100us

	u16Temp = (HdmiTx_R(0x14) & 0x0F) << 8;
	u16Temp += HdmiTx_R(0x15);
//	iTE_HTx_Msg(("u16Temp = 0x%04X\n", u16Temp));
	if(HDMITxRCLKFreqSel) {
		u16Temp *=2;
	}
//	u16Temp /= (1<<u8PreDivSel);

	if(u16Temp){
		u32PClk = stIt6662->u32RefClk * 2048 / u16Temp;
		u32PClk <<= u8PreDivSel;
		iTE_HTx_Msg(("TxPCLK = %d.%dMHz \n", (int)(u32PClk/1000), (int)(u32PClk%1000)));
		return u32PClk;
	}else{
		iTE_MsgE(("Reg0x14, 0x15 == 0x00\n"));
		return 0;
	}

}
//****************************************************************************
void HdmiTx_EnTmds(void)
{
	if((HdmiTx_R(0x10) & 0x03) == 0x03){
		iTE_Sleep_ms(100);
		if((HdmiTx_R(0x10) & 0x03) == 0x03){
			HdmiTx_Set(0x04, stIt6662->u8TxPortEn | (stIt6662->u8TxPortEn<<4), stIt6662->u8TxPortEn<<4);			// release reset Tx port after video stable
			HdmiTx_Set(0x04, stIt6662->u8TxPortEn<<4, 0x00);
			stIt6662->bRxTrgTxHdcp = iTE_FALSE;			// after reset HDCP should do Tx HDCP if necessary
			HdmiTx_Set(0xC1, stIt6662->u8TxPortEn, 0x00);															// Enable TMDS after video stable
			HdmiTx_Set(0x0A, 0x10, 0x00);					// Enable Tx FIFO error.
		}else{
			HdmiTx_Set(0xC1, stIt6662->u8TxPortEn, stIt6662->u8TxPortEn);
		}
	}else{
		HdmiTx_Set(0xC1, stIt6662->u8TxPortEn, stIt6662->u8TxPortEn);
	}

	iTE_HTx_Msg(("HdmiTx_EnTmds : 0x%X\n", HdmiTx_R(0xC1) & 0x0F));
}
//****************************************************************************
void HdmiTx_PortIRQ(void)
{
	iTE_u8 u8Reg40[2], u8Reg48;
	iTE_u8 u8Bank = g_u8TxBank;

	HdmiTx_Bank(u8Bank);
	HdmiTx_Rb(0x40 + g_u8TxOffset, 0x02, u8Reg40);
	u8Reg48 = HdmiTx_R(0x48 + g_u8TxOffset);
#if 1
	HdmiTx_W(0x40 + g_u8TxOffset, u8Reg40[0]);
	HdmiTx_W(0x41 + g_u8TxOffset, u8Reg40[1] & ~0x04);
	HdmiTx_W(0x48 + g_u8TxOffset, u8Reg48);
#else
	HdmiTx_Wb(0x40 + g_u8TxOffset, 0x02, u8Reg40);
	HdmiTx_W(0x48 + g_u8TxOffset, u8Reg48);
#endif
	HdmiTx_Bank(0);

//	if(g_u8TxSel == 0){
//		u8Reg40[1] &= 0xF7;
//	}
if(u8Reg40[0] || u8Reg40[1]){
iTE_HTx_Msg(("\nHdmiTx_PortIRQ_%d  0x%02X, 0x%02X S\n ",g_u8TxSel, u8Reg40[0], u8Reg40[1]));
	if(u8Reg40[0] & 0x01) {
		if(u8Reg48 & 0x01){
			iTE_HTx_Msg(("HPD Change Interrupt => HPD ON \n"));
			if(stIt6662->u8TxHpdStatus & g_u8TxShift){
#if 1
				if((stIt6662->u8TxDoHdcp & g_u8TxShift) == 0){

//					if(++stIt6662->u8TxHpdCnt[g_u8TxSel] > 2){
						HdmiTx_PortEnable(u8Reg48 & 0x02);
//						stIt6662->u8TxTrgRxHpd |= g_u8TxShift;
						iTE_HTx_Msg(("HPD H2H\n"));
						stIt6662->u8TxHpdCnt[g_u8TxSel] = 0;
//					}
				}
#endif
//				HdmiTx_PortEnable();
			}else{
				iTE_HTx_Msg(("HPD L2H\n"));
				HdmiTx_PortEnable(u8Reg48 & 0x02);

				stIt6662->u8EdidCheck |= g_u8TxShift;
			}
#if 1//(USING_HDCP_CTS == iTE_TRUE)	// HDCP CTS
				stIt6662->u8EdidCheck |= g_u8TxShift;
#endif
		}else{
			iTE_HTx_Msg(("HPD Change Interrupt => HPD OFF \n"));
			HdmiRx_W(0x84, TIME_BASE | 0x80);	// Enable Timer 100ms
			stIt6662->u8TxHpdChkTimer[g_u8TxSel] = TX_HPD_CHK_CNT;
#if (USING_HDCP_CTS == iTE_TRUE)
			HdmiTx_PortDisable();
#endif
		}
	}


	if(u8Reg40[0] & 0x02) {
		if(u8Reg48 & 0x02) {
			iTE_HTx_Msg(("RxSen Change Interrupt => RxSen ON \n"));
			if(u8Reg48 & 0x01) {
				iTE_HTx_Msg(("HPD ON\n"));
				if((stIt6662->u8TxPortEn & g_u8TxShift) == 0){
					HdmiTx_PortEnable(1);
				}
			}else{
				iTE_HTx_Msg(("HPD OFF\n"));
			}
		}else{
			iTE_HTx_Msg(("RxSen Change Interrupt => RxSen OFF \n"));
#if (USING_HDCP_CTS == iTE_TRUE)
			HdmiTx_PortDisable();
#endif
		}
	}

	if(u8Reg40[0] & 0x08) {
		iTE_HTx_Msg(("************************DDC Bus Hang!!!\n"));
#if 0
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x01);	//Enable PC DDC Mode
		HdmiTx_W(0x4F, 0x0F);					//Abort DDC Command
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x00);	//Disable DDC Mode
#else
		HdmiTx_Bank(u8Bank);
		HdmiTx_Set(0x55 + g_u8TxOffset, 0x80, 0x00);		//Disable CPDesired
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x01);		//Enable PC DDC Mode
		HdmiTx_Bank(0);
		HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
		HdmiTx_Bank(u8Bank);
		HdmiTx_W(0x4F + g_u8TxOffset, 0x0F);				//Abort DDC Command
		iTE_Sleep_ms(1);
//			HdmiTx_W(0x4F + g_u8TxOffset, 0x0F);				//Abort DDC Command
		HdmiTx_W(0x4F + g_u8TxOffset, 0x09);				//Abort DDC Command
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x00);		//Disable PC DDC Mode



		HdmiTx_Bank(0);
		HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
		HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x00);
#if (A0_WorkAround == iTE_TRUE)
#if (USING_IT6661==iTE_TRUE || USING_CASCADE==iTE_TRUE)
{	// workaround
if((g_u8CurDev == IT6662_ROOT) && (g_u8TxSel==HDMI_TX_A)){
	if((stIt6662->u8TxHpdStatus & 0x01) == 0){
		HdmiTx_Set(0x45, 0x08, 0x08);
	}
}
}
#endif
#endif
#endif
	}
	if(u8Reg40[0] & 0x80) {
		iTE_HTx_Msg(("\nDDC No Ack!!!\n"));
#if 1
		HdmiTx_Bank(u8Bank);
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x01);	//Enable PC DDC Mode
		HdmiTx_W(0x4F, 0x0F);					//Abort DDC Command
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x00);	//Disable DDC Mode
		HdmiTx_Bank(0);
#else
		HdmiTx_Bank(u8Bank);
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x01);		//Enable PC DDC Mode
		HdmiTx_Bank(0);
		HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
		HdmiTx_Bank(u8Bank);
		HdmiTx_W(0x4F + g_u8TxOffset, 0x0F);				//Abort DDC Command
		HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x00);		//Disable PC DDC Mode

	HdmiTx_Set(0x55 + g_u8TxOffset, 0x80, 0x00);		//Disable CPDesired

		HdmiTx_Bank(0);
		HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
		HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x00);
#endif
		}
		#if 0 //@lv 20160329
		if(u8Reg40[1] & 0x08) {
			iTE_HTx_Msg(("************************DDC Cmd Fail!!!\n"));
#if 0
			HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x01);	//Enable PC DDC Mode
			HdmiTx_W(0x4F, 0x0F);					//Abort DDC Command
			HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x00);	//Disable DDC Mode
#else
			HdmiTx_Bank(u8Bank);
			HdmiTx_Set(0x55 + g_u8TxOffset, 0x80, 0x00);		//Disable CPDesired
			HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x01);		//Enable PC DDC Mode
			HdmiTx_Bank(0);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
			HdmiTx_Bank(u8Bank);
			HdmiTx_W(0x4F + g_u8TxOffset, 0x0F);				//Abort DDC Command
			iTE_Sleep_ms(1);
//			HdmiTx_W(0x4F + g_u8TxOffset, 0x0F);				//Abort DDC Command
			HdmiTx_W(0x4F + g_u8TxOffset, 0x09);				//Abort DDC Command
			HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x00);		//Disable PC DDC Mode



			HdmiTx_Bank(0);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x00);
#if (A0_WorkAround == iTE_TRUE)
#if (USING_IT6661==iTE_TRUE || USING_CASCADE==iTE_TRUE)
{	// workaround
	if((g_u8CurDev == IT6662_ROOT) && (g_u8TxSel==HDMI_TX_A)){
		if((stIt6662->u8TxHpdStatus & 0x01) == 0){
			HdmiTx_Set(0x45, 0x08, 0x08);
		}
	}
}
#endif
#endif
#endif
		}
		#endif //@lv 20160329 end
		if(u8Reg40[1] & 0x10) {
			iTE_HTx_Msg(("***********************DDC FIFO Error!!!\n"));
			HdmiTx_Bank(u8Bank);
			HdmiTx_DdcDisable();
			HdmiTx_Bank(0);
		}

if(stIt6662->u8TxPortEn & g_u8TxShift) {
	if(u8Reg40[1] & 0x01) {
		iTE_HTx_Msg(("HDCP Authentication Fail Interrupt\n"));
		HdmiTx_HdcpStatus();
#if 0
		if(u8Reg40[1] & 0x08) {
			u8Reg40[1] &= ~0x08;
			iTE_HTx_Msg(("************************DDC Bus Hang!!!\n"));

			HdmiTx_Bank(u8Bank);
			HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x01);		//Enable PC DDC Mode
			HdmiTx_Bank(0);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
			HdmiTx_Bank(u8Bank);
			HdmiTx_W(0x4F + g_u8TxOffset, 0x0F);				//Abort DDC Command
			HdmiTx_Set(0x4A + g_u8TxOffset, 0x01, 0x00);		//Disable PC DDC Mode

			HdmiTx_Set(0x55 + g_u8TxOffset, 0x80, 0x00);		//Disable CPDesired

			HdmiTx_Bank(0);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x00);
		}
#endif


		if(EnAutoReAuth == iTE_FALSE) {
//			if(stIt6662->u8HdmiTxDoHdcp & g_u8TxShift){
			if(1){
				if(stIt6662->u8TxPortEn & g_u8TxShift){				//should be removed
					HdmiTx_Set(0x04, 0x10 << g_u8TxSel, 0x10 << g_u8TxSel);
					HdmiTx_Set(0x04, 0x10 << g_u8TxSel, 0x00);
					stIt6662->u8HDCPFireCnt[g_u8TxSel]++;
					iTE_HTx_Msg(("Enable HDCP Fire %d\n", stIt6662->u8HDCPFireCnt[g_u8TxSel]));
//					iTE_HTx_Msg(("Reg0x55 = %x\n", HdmiTx_R(0x55+g_u8TxOffset)));
//					if(HdmiTx_R(0x55+g_u8TxOffset) & 0x80){		// don't reAuthentication if HDCP off, // if Ri check fail and Rx video unstable happen at same time
					if(g_u1HdcpEn){				// turn on HDCP if HDCP is work
						HdmiTx_PreSpenHdcp(iTE_FALSE);
					}else{
						stIt6662->u8TxDoHdcp &= ~g_u8TxShift;
					}
				}
			}else{
				HdmiTx_AuthHdcp();
			}
		}
		if(stIt6662->u8HDCPFireCnt[g_u8TxSel] > HDCPFireMax) {
#if 0
			HdmiTx_Bank(u8Bank);
			HdmiTx_Set(0x55 + g_u8TxOffset, 0x80, 0x00);						//Disable CPDesired
			HdmiTx_Bank(0);
			HdmiTx_Set(0x22, 0x10 << g_u8TxSel, 0x10 << g_u8TxSel);		//Black Screen
#else
			HdmiTx_PortDisable();
			stIt6662->u8TxHpdStatus &= ~g_u8TxShift;
#endif
			iTE_HTx_Msg(("**********************Error: Set Black Screen because of HDCPFireCnt>%d !!!\n", HDCPFireMax));
		}
	}


	if(u8Reg40[1] & 0x02) {
		iTE_HTx_Msg(("HDCP Authentication Done Interrupt ...\n"));
		HdmiTx_HdcpStatus();
		HdmiTx_SaveKvsHdcp();
		stIt6662->u8TxSyncDetCnt[g_u8TxSel] = 0;
	}

	if(u8Reg40[1] & 0x04) {
		iTE_HTx_Msg(("HDCP Authentication KSV List Check Interrupt ...\n"));
		HdmiTx_Bank(g_u8TxBank);
		HdmiTx_PostSpenHdcp();
		HdmiTx_Bank(0);
	}

}

	if(u8Reg40[1] & 0x80){
		stIt6662->u8TxSyncDetCnt[g_u8TxSel] ++;
		iTE_HTx_Msg(("HDCP Sync Detect Fail, %d\n", stIt6662->u8TxSyncDetCnt[g_u8TxSel] ));
		HdmiTx_HdcpStatus();
		if(stIt6662->u8TxSyncDetCnt[g_u8TxSel] > 3){
			if(stIt6662->u8TxPortEn & g_u8TxShift){				//should be removed
				HdmiTx_Set(0x04, 0x10 << g_u8TxSel, 0x10 << g_u8TxSel);
				HdmiTx_Set(0x04, 0x10 << g_u8TxSel, 0x00);
				HdmiTx_PreSpenHdcp(iTE_FALSE);
			}
		}
	}else if(u8Reg40[1] & 0x20) {
		iTE_u8 u8Tmp[2];
		HdmiTx_Bank(u8Bank);
		iTE_HTx_Msg(("HDCP Ri Check Done Interrupt ... %d\n", ++stIt6662->u8HDCPRiChkCnt[g_u8TxSel]));
		HdmiTx_Rb(0x68 + g_u8TxOffset, 0x02, u8Tmp);
		iTE_HTx_Msg(("ARi = 0x%02X%02X\n", u8Tmp[1], u8Tmp[0]));
		HdmiTx_Rb(0x70 + g_u8TxOffset, 0x02, u8Tmp);
		iTE_HTx_Msg(("BRi = 0x%02X%02X\n", u8Tmp[1], u8Tmp[0]));
		stIt6662->u8TxHpdCnt[g_u8TxSel] = 0;
		if(USING_FORCE_TX_HDCP_ON == iTE_FALSE)
		{
			iTE_u8	u8TmpDev = g_u8CurDev;

			IT6662_DeviceSelect(IT6662_ROOT);
			if(HdmiRx_R(0xA8) & 0x02){
				HdmiTx_Set(0x55+g_u8TxOffset, 0x80, 0x00);
				iTE_HTx_Msg(("Tx HDCP off, Disable Rx HDCP\n"));
				stIt6662->bRxTrgTxHdcp = iTE_FALSE;
			}
			IT6662_DeviceSelect(u8TmpDev);
		}
		HdmiTx_Bank(0);
		stIt6662->u8TxSyncDetCnt[g_u8TxSel] = 0;
	}
	iTE_HTx_Msg(("\nHdmiTx_PortIRQ_%d E\n ",g_u8TxSel));
}
}
//****************************************************************************
iTE_u8 HdmiTx_Sw_Timer_IRQ(iTE_u8 u8TxSel)
{
	if(stIt6662->u8TxHpdChkTimer[u8TxSel]){
		HdmiTx_Select(u8TxSel);
		HdmiTx_Bank(g_u8TxBank);

		if(HdmiTx_R(0x48 + g_u8TxOffset) & 0x01){
			stIt6662->u8TxHpdChkTimer[u8TxSel] = 0;
			iTE_HTx_Msg(("HPD H2H\n"));
		}else{
			stIt6662->u8TxHpdChkTimer[u8TxSel]--;
			if(stIt6662->u8TxHpdChkTimer[u8TxSel] == 0){
				iTE_HTx_Msg(("HPD H2L\n"));
				HdmiTx_PortDisable();
				stIt6662->u8TxHpdStatus &= ~g_u8TxShift;
				stIt6662->u8EdidCheck |= g_u8TxShift;
			}
		}
		HdmiTx_Bank(0);
	}
#if (USING_HDCP_RPT_RDY_CHK == iTE_TRUE)
	if(stIt6662->u8TxRptRdyCnt[u8TxSel] ){
		stIt6662->u8TxRptRdyCnt[u8TxSel] --;
		iTE_HTx_Msg(("[%d]RptRdyCnt = %d\n", u8TxSel, stIt6662->u8TxRptRdyCnt[u8TxSel]));
	}
	return stIt6662->u8TxHpdChkTimer[u8TxSel] | stIt6662->u8TxRptRdyCnt[u8TxSel] ;
#else
	return stIt6662->u8TxHpdChkTimer[u8TxSel];
#endif

}
//****************************************************************************
void HdmiTx_PortEnable(iTE_u8 u8RxSen)
{
	iTE_HTx_Msg(("HdmiTx_PortEnable %d\n", u8RxSen));
	HdmiTx_Bank(g_u8TxBank);
	HdmiTx_Set(0x44 + g_u8TxOffset, 0x08, 0x00);		// add for fix DDC Bus Hang always occure on PortA & PortD of IT6661
	HdmiTx_Bank(0);
	HdmiTx_Set(0xC1, 0x10 << g_u8TxSel, 0x00);				// dong ask disable Tx driver for power saving
	if(u8RxSen){
		HdmiTx_Bank(g_u8TxBank);
		HdmiTx_Set(0x55 + g_u8TxOffset, 0x80, 0x00);

		HdmiTx_Bank(0);
		HdmiTx_Set(0x19, g_u8TxShift, g_u8TxShift);
		HdmiTx_Set(0x1A, 0x10, 0x00);

		stIt6662->u8TxPortEn |= g_u8TxShift;
		if((HdmiTx_R(0x10) & 0x03) == 0x03){
	//		iTE_Sleep_ms(100);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x10 << g_u8TxSel);
			HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x00);
			HdmiTx_Set(0xC1, g_u8TxShift, 0x00);
			iTE_HTx_Msg(("oHdmiTx_EnTmds : 0x%X\n", HdmiTx_R(0xC1) & 0x0F));
#if (USING_REPEATER == iTE_FALSE)
			HdmiTx_Bank(g_u8TxBank);
			if(g_u1HdcpEn){				// turn on HDCP if HDCP is work
				if((USING_CASCADE == iTE_TRUE) || (g_u8CurDev != IT6662_ROOT)){
		//			HdmiTx_Set(0x55 + g_u8TxOffset, 0x80, 0x80);
					stIt6662->u8TxSpenHdcp |= g_u8TxShift;
#if 0	// move to HdmiTx_Irq
					stIt6662->u8TxDoHdcp |= g_u8TxShift;
					stIt6662->u8HDCPFireCnt[g_u8TxSel]=0;
					HdmiTx_PreSpenHdcp(iTE_TRUE);
#endif
				}
			}
			HdmiTx_Bank(0);
#endif
		}else{
			HdmiTx_Set(0xC1, g_u8TxShift, g_u8TxShift);
		}
	}
		stIt6662->u8TxTrgRxHpd |= g_u8TxShift;
		stIt6662->u8TxHpdStatus |= g_u8TxShift;


#if 0//(USING_1to8 == iTE_TRUE)
	if(g_u8CurDev != IT6662_ROOT){
		HdmiRx_Bank(1);
		HdmiRx_Set(0xB0, 0x03, 0x03);		// Set HPD
		HdmiRx_Bank(0);
	}
#endif
#if (USING_IT6661==iTE_TRUE || USING_CASCADE==iTE_TRUE)
{	// workaround
	if((g_u8CurDev == IT6662_ROOT) && (g_u8TxSel==HDMI_TX_A)){
		HdmiTx_Set(0x45, 0x08, 0x00);
	}
}
#endif
}
//****************************************************************************
void HdmiTx_PortDisable(void)
{
	iTE_u8	u8Offset = g_u8TxOffset;

	HdmiTx_Bank(g_u8TxBank);
	HdmiTx_Set(0x44 + g_u8TxOffset, 0x08, 0x08);		// add for fix DDC Bus Hang always occure on PortA & PortD of IT6661
	HdmiTx_Set(0x55 + u8Offset, 0x80, 0x00);		//Disable CPDesired
	HdmiTx_Set(0x4A + u8Offset, 0x01, 0x01);		//Enable PC DDC Mode
	HdmiTx_Bank(0);
	HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
	HdmiTx_Bank(g_u8TxBank);
	HdmiTx_W(0x4F + u8Offset, 0x0F);				//Abort DDC Command
	iTE_Sleep_ms(1);
	HdmiTx_W(0x4F + u8Offset, 0x09);				//Abort DDC Command
	HdmiTx_Set(0x4A + u8Offset, 0x01, 0x00);		//Disable PC DDC Mode

//	HdmiTx_Set(0x55 + u8Offset, 0x80, 0x00);		//Disable CPDesired

//	HdmiTx_W(0x40 + u8Offset, 0xFF);
//	HdmiTx_W(0x41 + u8Offset, 0xFF);

	HdmiTx_Bank(0);

	HdmiTx_Set(0x04, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);
	HdmiTx_Set(0x19, g_u8TxShift, 0x00);
//	HdmiTx_Set(0xC1, g_u8TxShift, g_u8TxShift);
	HdmiTx_Set(0xC1, 0x11 << g_u8TxSel, 0x11 << g_u8TxSel);				// dong ask disable Tx driver for power saving
	stIt6662->u8TxSpenHdcp  &= ~g_u8TxShift;
	stIt6662->u8TxDoHdcp &= ~g_u8TxShift;
	stIt6662->u8TxPortEn &= ~g_u8TxShift;
//	stIt6662->u8TxHpdStatus &= ~g_u8TxShift;
#if 0
	if(stIt6662->u8TxHpdStatus == 0){
		HdmiRx_HpdClr();
	}
#endif
//	stIt6662->u8HDCPFireCnt[g_u8TxSel] = 0;

	if(stIt6662->u8TxPortEn == 0){
		HdmiTx_Set(0x1A, 0x10, 0x10);				// reset Tx packet FIFO overflow when no port enable
	}
#if 0//(USING_CASCADE == iTE_TRUE)
	if(stIt6662->u8TxHpdStatus == 0){
		if(g_u8CurDev != IT6662_ROOT){
			HdmiRx_Bank(1);
			HdmiRx_Set(0xB0, 0x03, 0x00);		// Clear HPD
			HdmiRx_Bank(0);
		}
	}
#endif
	iTE_HTx_Msg(("HdmiTx_PortDisable\n"));
}
//****************************************************************************
void HdmiTx_HdcpStatus(void)
{
	iTE_u8 	u8HdcpTemp[8];
	iTE_u8	u8Offset = g_u8TxOffset;

	HdmiTx_Rb(0x3B, 0x05, u8HdcpTemp);
	iTE_HTx_Msg(("AKSV = 0x%02X%02X%02X%02X%02X\n",
		u8HdcpTemp[4], u8HdcpTemp[3], u8HdcpTemp[2], u8HdcpTemp[1], u8HdcpTemp[0]));

	HdmiTx_Bank(g_u8TxBank);

	HdmiTx_Rb(0x6B + u8Offset, 0x07, &u8HdcpTemp[1]);
	iTE_HTx_Msg(("BKSV = 0x%02X%02X%02X%02X%02X\n",
		u8HdcpTemp[5], u8HdcpTemp[4], u8HdcpTemp[3], u8HdcpTemp[2], u8HdcpTemp[1]));
	iTE_HTx_Msg(("Current BR0 = 0x%02X%02X\n", u8HdcpTemp[7], u8HdcpTemp[6]));

	HdmiTx_Set(0x77 + u8Offset, 0x08, 0x08);
	HdmiTx_Rb(0x7D + u8Offset, 0x02, u8HdcpTemp);
	iTE_HTx_Msg(("Previous BR0 = 0x%02X%02X\n", u8HdcpTemp[1], u8HdcpTemp[0]));

	HdmiTx_Rb(0x68 + u8Offset, 0x02, &u8HdcpTemp[2]);
	iTE_HTx_Msg(("Current AR0 = 0x%02X%02X\n", u8HdcpTemp[3], u8HdcpTemp[2]));
	HdmiTx_Set(0x77 + u8Offset, 0x08, 0x00);
	HdmiTx_Rb(0x7D + u8Offset, 0x02, &u8HdcpTemp[4]);
	iTE_HTx_Msg(("Previous AR0 = 0x%02X%02X\n", u8HdcpTemp[5], u8HdcpTemp[4]));

	if((u8HdcpTemp[6] == u8HdcpTemp[4]) && (u8HdcpTemp[7] == u8HdcpTemp[5])){
		iTE_HTx_Msg(("CurBRi==PreARi ==> Sync Detection Error because of Miss CTLx signal\n"));
	}
	if((u8HdcpTemp[2] == u8HdcpTemp[0]) && (u8HdcpTemp[3] == u8HdcpTemp[1] )) {
		iTE_HTx_Msg(("CurARi==PreBRi ==> Sync Detection Error because of Extra CTLx signal\n"));
	}

	HdmiTx_Rb(0x58 + u8Offset, 0x08, u8HdcpTemp);
	iTE_HTx_Msg(("An = 0x%02X%02X%02X%02X%02X%02X%02X%02X\n",
		u8HdcpTemp[7], u8HdcpTemp[6], u8HdcpTemp[5], u8HdcpTemp[4], u8HdcpTemp[3], u8HdcpTemp[2], u8HdcpTemp[1], u8HdcpTemp[0]));

	HdmiTx_Rb(0x73, 0x04, u8HdcpTemp);
	HdmiTx_Bank(0);

	iTE_HTx_Msg(("BStatus = 0x%02X%02X\n", u8HdcpTemp[2], u8HdcpTemp[1]));
	iTE_HTx_Msg(("Rx HDCP Fast Reauthentication = %d\n", u8HdcpTemp[0] & 0x01));
	iTE_HTx_Msg(("Rx HDCP 1.1 Features = %d, %s.\n", (u8HdcpTemp[0] & 0x02) >> 1, (EnHDCP1p1 && (u8HdcpTemp[0] & 0x02))? "Enabled" : "Disabled" ));
	iTE_HTx_Msg(("Rx HDCP Maximum DDC Speed = %sKHz\n", (u8HdcpTemp[0] & 0x10)?"400":"100"));
	iTE_HTx_Msg(("Rx HDCP Repeater = %d \n", (u8HdcpTemp[0] & 0x40)>>6));
	iTE_HTx_Msg(("Tx Authentication Status = 0x%02X\n", u8HdcpTemp[3]));
	if(u8HdcpTemp[3] & 0x80){
		iTE_HTx_Msg(("Tx Authentication Done\n"));
	}else{
		if(u8HdcpTemp[3] & 0x01){			iTE_HTx_Msg(("Auth Fail: DDC NAck too may times !!!\n"));				}
		if(u8HdcpTemp[3] & 0x02){			iTE_HTx_Msg(("Auth Fail: BKSV Invalid !!!\n"));						}
		if(u8HdcpTemp[3] & 0x04){			iTE_HTx_Msg(("Auth Fail: Link Check Fail (AR0/=BR0) !!!\n"));			}
		if(u8HdcpTemp[3] & 0x08){			iTE_HTx_Msg(("Auth Fail: Link Integrity Ri Check Fail !!!\n"));			}
		if(u8HdcpTemp[3] & 0x10){			iTE_HTx_Msg(("Auth Fail: Link Integrity Pj Check Fail !!!\n"));			}
		if(u8HdcpTemp[3] & 0x20){			iTE_HTx_Msg(("Auth Fail: Link Integrity Sync Detect Fail !!!\n"));			}
		if(u8HdcpTemp[3] & 0x40){			iTE_HTx_Msg(("Auth Fail: DDC Bus Hang TimeOut !!!\n"));				}
	}

}
//****************************************************************************
// 20140826_S
void HdmiTx_PreSpenHdcp(iTE_u1 b1stSpen)
{
	iTE_u8	u8Offset = g_u8TxOffset;
#if	(GenAn == iTE_TRUE)
	iTE_u8	u8RegTemp[8];
#else
	iTE_u8 	u8DefaultAn[8] = {0x91, 0x2B, 0x39, 0xB7, 0x38, 0x19, 0x6C, 0xD3};
#endif

	stIt6662->u8TxSpenHdcp |= g_u8TxShift;
	stIt6662->u8TxDoHdcp &= ~g_u8TxShift;
	if(USING_FORCE_TX_HDCP_OFF == iTE_TRUE){
		stIt6662->u8TxSpenHdcp &= ~(g_u8TxShift);
		iTE_HTx_Msg(("FORCE TX HDCP OFF\n"));
		return;
	}

	if(( stIt6662->u8TxHpdStatus & g_u8TxShift) == 0x00){
		stIt6662->u8TxSpenHdcp &= ~(g_u8TxShift);
		iTE_HTx_Msg(("HpdStatus = %x, g_u8TxShift=%x\n", stIt6662->u8TxHpdStatus, g_u8TxShift));
		return;
	}
	iTE_HTx_Msg(("\n HdmiTx_PreSpenHdcp S %d\n", g_u8TxSel));
//	stIt6662->u8TxHpdCnt[g_u8TxSel]  = 0;

	HdmiTx_Bank(g_u8TxBank);
{
	iTE_u8	BStatus[2];

	HdmiTx_DdcReset();
	if(b1stSpen){
		iTE_u8	u8Bksv[5];
		iTE_u16	u8Temp;
		if(HdmiTx_HdcpRead(0x00, 0x05) == iTE_FALSE){
			//HdmiTx_DdcDisable();
			HdmiTx_Bank(0);
			iTE_HTx_Msg(("*0x00\n"));
			return;
		}
		HdmiTx_Rb(0x6B + g_u8TxOffset, 0x05, u8Bksv);

		for(u8Temp = 0; u8Temp < HDMI_TX_INVALID_BKSV_SIZE; u8Temp++){		// HDCP CTS 1A-08
			if((InvalidBKsv[u8Temp][0] == u8Bksv[0]) && (InvalidBKsv[u8Temp][1] == u8Bksv[1]) && (InvalidBKsv[u8Temp][2] == u8Bksv[2]) && (InvalidBKsv[u8Temp][3] == u8Bksv[3]) && (InvalidBKsv[u8Temp][4] == u8Bksv[4])){
				iTE_HTx_Msg(("*************************Invalid BKsv**********************Disable HDCP\n"));
				stIt6662->u8TxSpenHdcp &= ~(g_u8TxShift);
				HdmiTx_PortDisable();
				HdmiTx_Bank(0);
				return;
			}
		}
		stIt6662->u8HDCPFireCnt[g_u8TxSel]=0;
	}
// read HDCP status[12]

	if(HdmiTx_HdcpRead(0x41, 2) == iTE_FALSE){
		//HdmiTx_DdcDisable();
		HdmiTx_Bank(0);
		iTE_HTx_Msg(("*0x41\n"));
		return;
	}
	HdmiTx_Rb(0x74 + g_u8TxOffset, 0x02, BStatus);
	HdmiTx_DdcDisable();

	HdmiTx_Bank(0);
	if(HdmiTx_R(0x22) & g_u8TxShift){			//Tx is HDMI
		if((BStatus[1] & 0x10) == 0x00){			// HDCP status is DVI
			iTE_HTx_Msg(("DVI != HDMI\n"));
			HdmiRx_W(0x84, 0x80 | TIME_BASE);
			return;
		}
	}else{									// Tx is DVI
		if(BStatus[1] & 0x10){					// HDCP status is HDMI
			iTE_HTx_Msg(("HDMI != DVI\n"));
			HdmiRx_W(0x84, 0x80 | TIME_BASE);
			return;
		}
	}
	HdmiTx_Bank(g_u8TxBank);
}
	HdmiTx_Set(0x45 + u8Offset, 0x07, 0x00);

	HdmiTx_W(0x41 + u8Offset, 0x63);//	ming modified 0x60);
#if (EnRiPjInt == iTE_TRUE)
		HdmiTx_Set(0x45 + u8Offset, 0x60, 0x00);
#else
		HdmiTx_Set(0x45 + u8Offset, 0x60, 0x60);
#endif

#if (GenAn == iTE_TRUE)
		HdmiTx_Set(0x54 + u8Offset, 0x01, 0x01);
		iTE_Sleep_ms(1);
		HdmiTx_Set(0x54 + u8Offset, 0x01, 0x00);

		HdmiTx_Rb(0x60 + u8Offset, 0x08, u8RegTemp);
		HdmiTx_Wb(0x58 + u8Offset, 0x08, u8RegTemp);
#else
		HdmiTx_Wb(0x58 + u8Offset, 0x08, u8DefaultAn);
#endif

//	if((HdmiTx_R(0x48+u8Offset) & 0x01) || (stIt6662->bRxTrgTxHdcp)){
	if(1 || (stIt6662->u8TxPortEn & g_u8TxShift) || (stIt6662->bRxTrgTxHdcp)){
		HdmiTx_Set(0x55 + u8Offset, 0x80, 0x00);		// Disable Portx Tx HDCP
		iTE_Sleep_ms(1);
		HdmiTx_Set(0x55 + u8Offset, 0x80, 0x80);		// Enable Portx Tx HDCP

		HdmiTx_W(0x45 + u8Offset, 0x00);				// Enable Portx HDCP Interrupt
		HdmiTx_Set(0x55 + u8Offset, 0x01, 0x01);		// Fire Portx HDCP
		iTE_HTx_Msg(("\n Fire HDCP\n"));
#if (USING_HDCP_RPT_RDY_CHK == iTE_TRUE)
		stIt6662->u8TxRptRdyCnt[g_u8TxSel] = Tx_HDCP_REPEATER_CNT;
		HdmiRx_W(0x84, TIME_BASE | 0x80);	// Enable Timer 100ms
#endif
		stIt6662->u8TxSpenHdcp &= ~(g_u8TxShift);
		stIt6662->u8TxDoHdcp |= g_u8TxShift;
	}

	HdmiTx_Bank(0);
	HdmiTx_Set(0x23, g_u8TxShift, 0x00);			// Disable Tx AVMute
	iTE_HTx_Msg(("\n HdmiTx_PreSpenHdcp E %d\n", g_u8TxSel));
}
//****************************************************************************
iTE_u1 HdmiTx_PostSpenHdcp(void)
{
	iTE_u8	u8RegRd;
//	iTE_u8 	u8HdmiTxSel = g_u8TxSel;
	iTE_u8	u8Temp, u8DevCnt;
	iTE_u8	u8Buf[10];
	iTE_pu8	M0, BStatus;

	iTE_HTx_Msg(("\n HdmiTx_PostSpenHdcp S %d\n", g_u8TxSel));
	BStatus = u8Buf;
	M0 = &u8Buf[2];

//	iTE_HTx_Msg(("HDCP Interrupt Status = 0x%02X\n", HdmiTx_R(0x41 + g_u8TxOffset)));
	HdmiTx_DdcReset();

	HdmiTx_HdcpRead(0x40, 1);
	u8RegRd = HdmiTx_R(0x73 + g_u8TxOffset);
	iTE_HTx_Msg(("BCap = 0x%02X\n", u8RegRd));
	if(u8RegRd & 0x20) {
		HdmiTx_W(0x41 + g_u8TxOffset, 0x04);
		HdmiTx_Set(0x77 + g_u8TxOffset, 0x07, 0x05);
		HdmiTx_Rb(0x78 + g_u8TxOffset, 0x04, M0);
		for(u8Temp = 0; u8Temp < 0x04; u8Temp++) {
			HdmiTx_Set(0x77 + g_u8TxOffset, 0x07, u8Temp);
			M0[4+u8Temp] = HdmiTx_R(0x7C + g_u8TxOffset);
		}
//		iTE_HTx_Msg(("M0: 0x%02X%02X%02X%02X%02X%02X%02X%02X\n", M0[7], M0[6], M0[5], M0[4], M0[3], M0[2], M0[1], M0[0]));

		HdmiTx_HdcpRead(0x41, 2);
		HdmiTx_Rb(0x74 + g_u8TxOffset, 0x02, BStatus);
		u8DevCnt = BStatus[0] & 0x7F;
		iTE_HTx_Msg(("Device Count = 0x%X\n", BStatus[0]));
		u8Temp = BStatus[1] & 0x07;
		if(u8Temp > stIt6662->u8MaxDepth){
			stIt6662->u8MaxDepth = u8Temp;
		}
		iTE_HTx_Msg(("Depth = 0x%X\n", u8Temp));
		iTE_HTx_Msg(("Max. Cascade Exceeded = %d\n", (BStatus[1] & 0x08) >> 3));
		iTE_HTx_Msg(("HDMI_MODE = %d\n", (BStatus[1] & 0x10) >> 4));

		if((BStatus[0] & 0x80) || (BStatus[1] & 0x08)) {
			HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x30);
			HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x00);
			iTE_MsgE(("Max. Device or Cascade Exceeded\n"));
			return 0;
		} else {
			iTE_u32 	u32Hash[5];
			iTE_u16	u16Temp = (iTE_u16)u8DevCnt * 5 ;
			iTE_u1	u1ShaPass = iTE_TRUE;
			iTE_u8	u8BV[4];
			iTE_pu8	pu8Hash = (iTE_pu8) u32Hash;

			if(u8DevCnt) {
			HdmiTx_Bank(0);
			HdmiTx_W(0x34, stIt6662->u16DevCnt);
			HdmiTx_Bank(g_u8TxBank);
				HdmiTx_HdcpRead(0x43, u16Temp);
			}else{
				iTE_HTx_Msg(("WARNING:  Device Count = 0\n"));
			}

			HdmiRx_Bank(1);
#if 0
iTE_HRx_Msg(("g_u8DevCnt = 0x%d\n", stIt6662->u16DevCnt));
HdmiRx_W(0x2D, 0);
HdmiRx_Rb(0x28, stIt6662->u16DevCnt*5, pu8KsvList);
for(u8Temp =0; u8Temp < stIt6662->u16DevCnt; u8Temp++){
	iTE_HRx_Msg((" KSV[%d] = %02X%02X%02X%02X%02X\n", u8Temp, pu8KsvList[u8Temp*5], pu8KsvList[u8Temp*5+1], pu8KsvList[u8Temp*5+2], pu8KsvList[u8Temp*5+3], pu8KsvList[u8Temp*5+4]));
}
#endif
			HdmiRx_W(0x2D, stIt6662->u16DevCnt);
//			HdmiRx_W(0x2D, u8DevCnt);

			HdmiRx_ShaCheck(u8Buf, u32Hash, u16Temp);
			HdmiRx_Bank(0);

#if 0
			for(u8Temp = 0; u8Temp < 5; u8Temp++){
				iTE_u8	u8BV[4];
				HdmiTx_HdcpRead(0x20 + (u8Temp<<2), 4);
				HdmiTx_Set(0x77 + g_u8TxOffset, 0x07, u8Temp);
				HdmiTx_Rb(0x78 + g_u8TxOffset, 4, u8BV);

				if(((u32Hash[u8Temp] & 0xFF) != u8BV[0])
					|| (((u32Hash[u8Temp] >>8) & 0xFF) != u8BV[1])
					|| (((u32Hash[u8Temp] >> 16) & 0xFF) != u8BV[2])
					|| (((u32Hash[u8Temp] >> 24) & 0xFF) != u8BV[3]) ){
					u1ShaPass = iTE_FALSE;
					iTE_HTx_Msg(("[%d] BV.H: 0x%02X%02X%02X%02X,  AV.H:0x%08X \n", u8Temp, u8BV[3], u8BV[2], u8BV[1], u8BV[0], u32Hash[u8Temp]));
					break;

				}
			}
#else
			for(u8Temp = 0; u8Temp < 20; ){
				HdmiTx_HdcpRead(0x20 + u8Temp, 4);
				HdmiTx_Set(0x77 + g_u8TxOffset, 0x07, u8Temp >> 2);
				HdmiTx_Rb(0x78 + g_u8TxOffset, 4, u8BV);

				if((pu8Hash[u8Temp++]  != u8BV[0])
					|| (pu8Hash[u8Temp++]  != u8BV[1])
					|| (pu8Hash[u8Temp++] != u8BV[2])
					|| (pu8Hash[u8Temp++]  != u8BV[3]) ){
					u1ShaPass = iTE_FALSE;
					iTE_HTx_Msg(("[%d] BV.H: 0x%02X%02X%02X%02X,  AV.H:0x%08X \n", u8Temp>>2, u8BV[3], u8BV[2], u8BV[1], u8BV[0], u32Hash[u8Temp]));
					break;

				}
			}




#endif

			iTE_HTx_Msg(("SHA Check Result = %s \n", u1ShaPass? "PASS" : "FAIL"));
			if(u1ShaPass) {
				HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x10);
				HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x00);
				HdmiTx_Set(0x45 + g_u8TxOffset, 0x02, 0x02);		// clear Auth Done status
			}else{
				HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x30);
				HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x00);
				return 0;
			}

			if(stIt6662->u8TxDoHdcp & g_u8TxShift){
				stIt6662->u16DevCnt += u8DevCnt;
			}
		}
		HdmiTx_Bank(0);
		HdmiTx_SaveKvsHdcp();
		iTE_HTx_Msg(("\n HdmiTx_PostSpenHdcp E %d\n", g_u8TxSel));
#if (USING_HDCP_RPT_RDY_CHK == iTE_TRUE)
	}else{
		iTE_HTx_Msg(("[%d]u8TxRptRdyCnt=%d\n", g_u8TxSel, stIt6662->u8TxRptRdyCnt[g_u8TxSel]));
		if(stIt6662->u8TxRptRdyCnt[g_u8TxSel]== 0){
			HdmiTx_W(0x41 + g_u8TxOffset, 0x04);
			HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x30);
			HdmiTx_Set(0x55 + g_u8TxOffset, 0x30, 0x00);
			iTE_HTx_Msg(("Cnt 0\n"));
			return 0;
		}
#endif
	}
	return 1;
}
//****************************************************************************
//****************************************************************************
void HdmiTx_SaveKvsHdcp(void)
{
#if (USING_REPEATER == iTE_TRUE)
#if (USING_CASCADE == iTE_TRUE)
	iTE_HTx_Msg(("HdmiTx_SaveKvsHdcp S %d\n", g_u8TxSel));
	if(stIt6662->u8TxDoHdcp & g_u8TxShift){
		stIt6662->u16DevCnt++;
		HdmiTx_W(0x3A, 0x10 << g_u8TxSel);
		stIt6662->u8TxDoHdcp &= ~g_u8TxShift;
	}
	iTE_HTx_Msg(("u8DevCnt=%d\n", stIt6662->u16DevCnt));



#if 0
{
	iTE_u8	u8KsvList[127*5], u8Temp;

	HdmiRx_Bank(1);
	HdmiRx_W(0x2D, 0x00);
	HdmiRx_Rb(0x28, stIt6662->u16DevCnt*5, u8KsvList);
	HdmiRx_Bank(0);
	for(u8Temp=0; u8Temp < stIt6662->u16DevCnt; u8Temp++){
		iTE_HTx_Msg(("KSV List [%d] = 0x %02X %02X %02X %02X %02X\n", u8Temp, u8KsvList[u8Temp*5+4],u8KsvList[u8Temp*5+3],u8KsvList[u8Temp*5+2],u8KsvList[u8Temp*5+1],u8KsvList[u8Temp*5]));
	}
}
#endif

	iTE_HTx_Msg(("Reg0x19=0x%02X\n", HdmiTx_R(0x19)));

	if((stIt6662->u8TxDoHdcp & HdmiTx_R(0x19)) == 0){
//	g_u8HdmiTxDoHdcp &= HdmiTx_R(0x19);
//	if(g_u8HdmiTxDoHdcp == 0){
		HdmiRx_SpenHdcp();
	}
#else
	iTE_u8 u8CurDev, u8Temp, u8KsvList[5], u8DevCnt;
	iTE_u8 u8Ignor = 1;
	iTE_HTx_Msg(("HdmiTx_SaveKvsHdcp S %d\n", g_u8TxSel));
	if(stIt6662->u8TxDoHdcp & g_u8TxShift){
		stIt6662->u16DevCnt++;
		HdmiTx_W(0x3A, 0x10 << g_u8TxSel);
		stIt6662->u8TxDoHdcp &= ~g_u8TxShift;
		u8Ignor = 0;
	}
	iTE_HTx_Msg(("u8DevCnt=%d\n", stIt6662->u16DevCnt));

	iTE_HTx_Msg(("Reg0x19=0x%02X\n", HdmiTx_R(0x19)));

	u8CurDev = g_u8CurDev;
	if(u8Ignor == 0){
	if((stIt6662->u8TxDoHdcp & HdmiTx_R(0x19)) == 0){
		u8DevCnt = stIt6662->u16DevCnt;
		HdmiRx_Bank(1);
//		for(u8Temp = 0; u8Temp < stIt6662->u16DevCnt; u8Temp++){
		for(u8Temp = 0; u8Temp < u8DevCnt; u8Temp++){
			HdmiRx_W(0x2D, u8Temp);
			HdmiRx_Rb(0x28, 5, u8KsvList);
			IT6662_DeviceSelect(IT6662_C);
			HdmiTx_W(0x34, stIt6662->u16DevCnt++);
			HdmiTx_Wb(0x35, 5, u8KsvList);
			IT6662_DeviceSelect(u8CurDev);
			iTE_HTx_Msg(("KSV List [%d] = 0x %02X %02X %02X %02X %02X\n", u8Temp, u8KsvList[4],u8KsvList[3],u8KsvList[2],u8KsvList[1],u8KsvList[0]));
		}
		HdmiRx_Bank(0);
	}
	}

	IT6662_DeviceSelect(IT6662_A);
	if((stIt6662->u8TxDoHdcp & HdmiTx_R(0x19)) == 0){
		IT6662_DeviceSelect(IT6662_B);
		if((stIt6662->u8TxDoHdcp & HdmiTx_R(0x19)) == 0){
			IT6662_DeviceSelect(IT6662_C);
			HdmiRx_SpenHdcp();
		}
	}
	IT6662_DeviceSelect(u8CurDev);
#endif
#else
	stIt6662->u8TxDoHdcp &= ~g_u8TxShift;
#endif
	stIt6662->u8HDCPFireCnt[g_u8TxSel] = 0;
	stIt6662->u8HDCPRiChkCnt[g_u8TxSel] = 0;
//	HdmiTx_Bank(0);
	HdmiTx_Set(0x22, 0x10 << g_u8TxSel, 0x00);
//	HdmiTx_Bank(g_u8TxBank);
	iTE_HTx_Msg(("HdmiTx_SaveKvsHdcp E %d\n", g_u8TxSel));
}
void HdmiTx_AuthHdcp(void)
{
	stIt6662->u16DevCnt = 0;
	stIt6662->u8MaxDepth = 0;

	HdmiTx_W(0x34, 0x00);

#if (A0_WorkAround == iTE_TRUE)
//	EnHDCP = iTE_TRUE;
//	if(EnHDCP == iTE_TRUE))
	{		// FW Workaround for AKSV Issue
		HdmiTx_Set(0x55, 0x81, 0x81);
		iTE_Sleep_ms(1);
		HdmiTx_Set(0x04, 0x10, 0x10);
		HdmiTx_Set(0x04, 0x10, 0x00);
		HdmiTx_Set(0x55, 0x81, 0x00);
	}
#endif
	iTE_HTx_Msg(("\nHdmiTx_AuthHdcp S\n"));
//	stIt6662->u8HdmiTxDoHdcp = HdmiTx_R(0x19) & 0x0F;
#if 1
	stIt6662->u8TxSpenHdcp = HdmiTx_R(0x19) & 0x0F;
#else
	if(stIt6662->u8TxDoHdcp){
		if(stIt6662->u8TxDoHdcp & 0x01){
			HdmiTx_Select(HDMI_TX_A);
			stIt6662->u8HDCPFireCnt[HDMI_TX_A]=0;
			HdmiTx_PreSpenHdcp();
		}
		if(stIt6662->u8TxDoHdcp & 0x02){
			HdmiTx_Select(HDMI_TX_B);
			stIt6662->u8HDCPFireCnt[HDMI_TX_B]=0;
			HdmiTx_PreSpenHdcp();
		}
		if(stIt6662->u8TxDoHdcp & 0x04){
			HdmiTx_Select(HDMI_TX_C);
			stIt6662->u8HDCPFireCnt[HDMI_TX_C]=0;
			HdmiTx_PreSpenHdcp();
		}
		if(stIt6662->u8TxDoHdcp & 0x08){
			HdmiTx_Select(HDMI_TX_D);
			stIt6662->u8HDCPFireCnt[HDMI_TX_D]=0;
			HdmiTx_PreSpenHdcp();
		}
	}else{
//			HdmiRx_SpenHdcp();
	}
#endif

	//HdmiTx_W(0x23, 0x0F);				// Enable all Tx AVMute //@LV 20160329 no video out 
	iTE_HTx_Msg(("\nHdmiTx_AuthHdcp E\n"));
}
// 20140826_E
//****************************************************************************
void HdmiTx_SetOutputMode(EDID_STAT eStatus)
{
	HdmiTx_Set(0x04, g_u8TxShift, g_u8TxShift);
	if(EDID_NO_ERR == eStatus){
		HdmiTx_Set(0x22, g_u8TxShift, g_u8TxShift);
		iTE_HTx_Msg(("HTx is HDMI\n"));
	}else{
		HdmiTx_Set(0x22, g_u8TxShift, 0x00);
		iTE_HTx_Msg(("HTx is DVI\n"));
	}
	HdmiTx_Set(0x04, g_u8TxShift, 0x00);
}

//****************************************************************************
iTE_u8 HdmiTx_CscOutputSet(void)
{
	iTE_u8	u8YuvSup = 0x30;
	iTE_u8	u8Temp, u8TmpShift;
	iTE_u8	aOutClr[4] = {RGB444, YCbCr422, YCbCr444, YCbCr444};
	iTE_u8	u8TmpInClr = g_u8InClr << 4;

//	iTE_u8	u8OutClr;

	for(u8Temp=0; u8Temp<4; u8Temp++){
		u8TmpShift = 0x01<<u8Temp;
		if(stIt6662->u8TxHpdStatus & u8TmpShift){
			if(stIt6662->bForceCsc){												// force output RGB444
				HdmiTx_Set(0x23, u8TmpShift, u8TmpShift);				// AVMute if CSC enable
				HdmiTx_Set(0x19, 0x10 << u8Temp, 0x10 << u8Temp);				// turn on CSC
				iTE_Msg(("CSC Turn On %d\n", u8Temp));
				u8YuvSup = RGB444;
			}else{
				u8YuvSup &= g_stEdidInfo[u8Temp].u8CeaB3;
				iTE_HTx_Msg(("%d CeaB3=0x%02X\n", u8Temp, g_stEdidInfo[u8Temp].u8CeaB3));
				if((u8TmpInClr & g_stEdidInfo[u8Temp].u8CeaB3) == u8TmpInClr){
					HdmiTx_Set(0x19, 0x10 << u8Temp, 0x00);							// Turn off CSC if sink support input color
				}else{
					if((HdmiTx_R(0x19) & (0x10 << u8Temp)) == 0){
					HdmiTx_Set(0x23, u8TmpShift, u8TmpShift);				// AVMute if CSC enable
					HdmiTx_Set(0x19, 0x10 << u8Temp, 0x10 << u8Temp);				// turn on CSC if sink not support input color
					}
					iTE_Msg(("CSC Turn On %d\n", u8Temp));
				}
			}


			if((HdmiRx_R(0x0A) & 0x02) && g_stEdidInfo[u8Temp].pstVsdbInfo.u8PaAdr){
				if((HdmiTx_R(0x22) & u8TmpShift) == 0){
					HdmiTx_Set(0x04, u8TmpShift, u8TmpShift);
					HdmiTx_Set(0x22, u8TmpShift, u8TmpShift);						// Tx output HDMI
					HdmiTx_Set(0x04, u8TmpShift, 0x00);
				}
				iTE_HTx_Msg(("HTx %X HDMI output\n", u8Temp));
			}else{
				if(HdmiTx_R(0x22) & u8TmpShift){
					HdmiTx_Set(0x04, u8TmpShift, u8TmpShift);
					HdmiTx_Set(0x22, u8TmpShift, 0x00);							// Tx output DVI
					HdmiTx_Set(0x04, u8TmpShift, 0x00);
				}
				iTE_HTx_Msg(("HTx %X DVI output\n", u8Temp));
			}

		}else{
			HdmiTx_Set(0x19, 0x10 << u8Temp, 0x00);								// turn off CSC if Sink removed
		}
	}

	return aOutClr[u8YuvSup>>4];
}
//****************************************************************************
void HdmiTx_HdcpDisable(void)
{
	iTE_u8	u8Temp = HdmiTx_R(0x0F);

	stIt6662->u8TxSpenHdcp = 0;
	stIt6662->u8TxDoHdcp = 0;
	HdmiTx_Bank(0);
	HdmiTx_W(0x23, 0x00);				// Disable all Tx AVMute
	HdmiTx_Set(0x04, 0xF0, 0xF0);
	HdmiTx_Set(0x04, 0xF0, 0x00);
	HdmiTx_Set(0x55, 0x81, 0x00);
	HdmiTx_Set(0x95, 0x81, 0x00);
	HdmiTx_Bank(1);
	HdmiTx_Set(0x55, 0x81, 0x00);
	HdmiTx_Set(0x95, 0x81, 0x00);
	HdmiTx_W(0x0F, u8Temp);
}
//****************************************************************************
iTE_u1 HdmiTx_HdcpRead(iTE_u8 u8HdcpOffset, iTE_u16 u16ByteNum)
{
	iTE_u1	bStatus = iTE_TRUE;
	iTE_u8	u8Offset = g_u8TxOffset;

	HdmiTx_Set(0x4A + u8Offset, 0x01, 0x01);					// Enable PC DDC Mode
	HdmiTx_W(0x4F + u8Offset, 0x09);							// DDC FIFO Clear
	HdmiTx_Set(0x45 + u8Offset, 0x08, 0x00);					// Enable DDC Bus Hang Interrupt

	HdmiTx_W(0x4B + u8Offset, 0x74);							// HDCP Address
	HdmiTx_W(0x4C + u8Offset, u8HdcpOffset);					// HDCP Offset
	HdmiTx_W(0x4D + u8Offset, u16ByteNum);					// Read Byte Num
	HdmiTx_W(0x4F + u8Offset, (u16ByteNum & 0x300)>> 4);	// HDCP Read Fire

	if(HdmiTx_DdcWait() == iTE_FALSE) {
		iTE_MsgE(("TX DDC HDCP Read Fail !!!\n"));
		bStatus = iTE_FALSE;
	}
	HdmiTx_Set(0x4A + u8Offset, 0x01, 0x00);					// Disable PC DDC Mode
	return bStatus;
}
//****************************************************************************
iTE_u1 HdmiTx_DdcWait(void)
{
#define DDCWAITTIME     (1)
#define DDCWAITNUM      (50)

	iTE_u8	u8DdcWaitCnt = 0;
	iTE_u8	u8Offset = g_u8TxOffset;
	iTE_u8	u8Temp;

	do{
		iTE_Sleep_ms(DDCWAITTIME);
		u8Temp = HdmiTx_R(0x50 + u8Offset);
	}while(((u8Temp & 0xB0) == 0x00) && (++u8DdcWaitCnt < DDCWAITNUM));

	if(u8DdcWaitCnt >= DDCWAITNUM) {
		iTE_MsgE(("DDC Bus Wait Time Out: TxSel=0x%X, Status=0x%X\n",g_u8TxSel, u8Temp));
		if(HdmiTx_R(0x41 + u8Offset) & 0x08) {
			iTE_HTx_Msg(("DDC Bus Hang\n"));
			HdmiTx_W(0x4F + u8Offset, 0x0F);			// Abort DDC Command
			HdmiTx_W(0x41 + u8Offset, 0x08);			// Clear Interrupt
		}else{
			iTE_HTx_Msg(("Unknown Issue \n"));
		}
		return iTE_FALSE;
	}else if(u8Temp & 0x80){
		return iTE_TRUE;
	}else{
		iTE_MsgE(("DDC Bus Wait Fail: TxSel=0x%X, Status=0x%X, u8DdcWaitCnt=%d\n",g_u8TxSel, u8Temp, u8DdcWaitCnt));
		return iTE_FALSE;
	}

}
//****************************************************************************
void HdmiTx_DdcDisable(void)
{
	iTE_u8	u8Offset = g_u8TxOffset;
	HdmiTx_Set(0x4A + u8Offset, 0x01, 0x01);	//Enable PC DDC Mode
	HdmiTx_W(0x4F + u8Offset, 0x09);			//Clear DDC FIFO
	HdmiTx_Set(0x4A + u8Offset, 0x01, 0x00);	//Disable DDC Mode
}
//****************************************************************************
void HdmiTx_DdcReset(void)
{
	iTE_u8	u8Offset = g_u8TxOffset;
	HdmiTx_Set(0x4A + u8Offset, 0x01, 0x01);					// Enable PC DDC Mode
	HdmiTx_W(0x4F + u8Offset, 0x0F);							// Abort DDC Command
	HdmiTx_Set(0x45 + u8Offset, 0x08, 0x00);					// Enable DDC Bus Hang Interrupt
}
//****************************************************************************
EDID_STAT HdmiTx_FireEdidRead(iTE_u8 u8RdStart, iTE_u8 u8RdCnt, iTE_u8 u8EdidBlk)
{
	iTE_u8	u8Offset = g_u8TxOffset;

	HdmiTx_W(0x4F + u8Offset, 0x09);								// DDC FIFO Clear
	HdmiTx_W(0x4B + u8Offset, 0xA0);						// HDCP Address	//0x74 for HDCP, 0xA0 for EDID
	HdmiTx_W(0x4C + u8Offset, u8RdStart);				// HDCP Offset
	HdmiTx_W(0x4D + u8Offset, u8RdCnt);					// Read Byte Num
	HdmiTx_W(0x4E + u8Offset, u8EdidBlk / 2);
	HdmiTx_W(0x4F + u8Offset, 0x03);	// HDCP Read Fire

	if(iTE_FALSE == HdmiTx_DdcWait()){
		iTE_MsgE(("HdmiTx_FireEdidRead Read Fail: 0x%02X\n", u8RdStart));
		return EDID_READ_ERR;
	}else{
		return EDID_NO_ERR;
	}
}
//****************************************************************************
void HdmiTx_EdidRead(iTE_u8	u8RdCnt, iTE_pu8 pu8RdEdid)
{
	HdmiTx_Rb(0x51+g_u8TxOffset, u8RdCnt, pu8RdEdid);
}
//****************************************************************************

