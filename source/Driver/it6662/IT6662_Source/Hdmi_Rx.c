///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Hdmi_Rx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Hdmi_Rx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#define HDMI_RX_INIT_TABLE
#include "it6662/include/Hdmi_Rx.h"
#include "it6662/include/Csc.h"
#include "it6662/include/Mhl_Rx.h"
#include "it6662/include/Hdmi_Tx.h"
#include "it6662/include/Cec.h"
extern void EXT_SET(iTE_u1 bSet);
//****************************************************************************
extern MSC_CMD TxMscCmd;
extern iTE_u8 g_u8TxSel, g_u8TxOffset, g_u8TxBank, g_u8TxShift;
extern IT6662_Var	*stIt6662;

iTE_u1 g_u1HdcpEn = 0;
iTE_u1 g_bCscChk = 0;
iTE_u8 g_u8InClr = 0;
iTE_u8 g_u8MhlHdcpCnt = 0;
iTE_u8 g_u8MhlClkWait = 0;
#if (USING_AUTO_EQ == iTE_TRUE)
iTE_u8 g_u8EqChkCnt = 0;
iTE_u8 g_u8AutoEqTog = 0xFF;
iTE_u1 g_bAutoEqDone = 1;
iTE_u8 g_u8ManulEqCnt = 0;
iTE_u8 g_u8ManulEqIdx = 0;
iTE_u8 g_u8VdoParChgCnt = 0;
iTE_u1 g_bManulEqEn = 0;
iTE_u1 g_bManulEqEnDone = 0;
iTE_u8 g_u8EqReChkTimer = 0;
iTE_u8 g_u8EqReChkCnt = 0;
iTE_u16 g_u16EqErrCnt[ManulEqMax];
iTE_u8 g_u8EqDelayTrigger = 0;
#endif

#if (USING_1to8 == iTE_TRUE)
extern iTE_u8 u8I2CAdr;
extern void IT6662_DeviceSelect(iTE_u8 u8Device);
#endif
//****************************************************************************
void HdmiRx_EnCec(void)
{
	HdmiRx_Set(0x0E, 0x01, 0x01);
}
//****************************************************************************
void HdmiRx_Init(void)
{
	iTE_u8 u8Temp;
	iTE_HRx_Msg(("HdmiRx_Init S\n"));
	for(u8Temp=0; u8Temp < HDMI_RX_INIT_TAB_SIZE; u8Temp++){
		HdmiRx_Set(HDMI_RX_INIT_TAB[u8Temp][0], HDMI_RX_INIT_TAB[u8Temp][1], HDMI_RX_INIT_TAB[u8Temp][2]);
	}

	HdmiRx_W(0x34, MHL_RX_ADR + 0x01 + u8I2CAdr);
#if (EnIntEDID == iTE_TRUE)
	HdmiRx_W(0x87, EDID_ADR + 0x01 + u8I2CAdr);
#else
	HdmiRx_W(0x87, EDID_ADR + u8I2CAdr);
#endif
	HdmiRx_W(0x8D, HDMI_TX_ADR + 0x01 + u8I2CAdr);

#if (USING_CEC == iTE_TRUE)
	HdmiRx_W(0x86, CEC_ADR + CEC_RX_SEL + 0x01 +  u8I2CAdr);
#endif

#if (USING_AUTO_EQ == iTE_TRUE)
	for(u8Temp = 0; u8Temp < ManulEqMax; u8Temp++){
		g_u16EqErrCnt[u8Temp] = 0xFFFF;
	}
#endif
	iTE_HRx_Msg(("HdmiRx_Init E\n"));
}
//****************************************************************************
void HdmiRx_Irq(void)
{
	iTE_u8	u8Reg05[6], u8RdData;

	HdmiRx_Rb(0x05, 0x06, u8Reg05);
	HdmiRx_W(0x05, u8Reg05[0]);// & 0xF7);
	HdmiRx_Wb(0x06, 0x05, &u8Reg05[1]);

	if(u8Reg05[0] ||u8Reg05[1] ||u8Reg05[2] ||u8Reg05[3] ||u8Reg05[4] ){
		iTE_HRx_Msg(("HdmiRx_Irq 1 S 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", u8Reg05[0], u8Reg05[1],u8Reg05[2], u8Reg05[3], u8Reg05[4]));
		if(u8Reg05[0]) {
			if(u8Reg05[0] & 0x01){
				if(u8Reg05[5] & 0x01){
					iTE_HRx_Msg(("HDMI Rx Power 5V ON\n"));
#if (USING_AUTO_EQ == iTE_TRUE)
					g_u8ManulEqCnt = 0;
#endif
					HdmiTx_Set(0x05, 0x0C , 0x00);
					if(!(HdmiRx_R(0x51) & 0x01)){
					}
#if (USING_CEC == iTE_TRUE)
					// reset CEC
					if(g_u8CurDev == IT6662_ROOT){
						Cec_BlockSel(CEC_RX_SEL);
						Cec_TxPolling(IT6662_RX_LA);
					}
#endif
#if 1			// added for clear HPD if power 5v removed
					if(stIt6662->bRxHpdStatus){
						if(HdmiRx_R(0x0A) & 0x40){
//							while(Msc_Fire(MSC_PKT_SET_HPD, &TxMscCmd)!=MSC_SUCCESS);
						}else{
//							HdmiRx_HpdOut(IT6662_HPD_H);
							HdmiRx_HpdSet();
						}
					}
#endif
				}else{
					iTE_HRx_Msg(("HDMI Rx Power 5V OFF\n"));
					HdmiRx_Set(0x31, 0x40, 0x00);
					HdmiRx_Set(0x11, 0x02, 0x02);
					HdmiRx_Set(0x11, 0x02, 0x00);
					HdmiTx_Set(0x05, 0x0C, 0x0C);

					HdmiTx_HdcpDisable();

					stIt6662->u8EccErrCnt = 0;
					g_u1HdcpEn = 0;
					stIt6662->bRxTrgTxHdcp	= iTE_FALSE;
#if 1			// added for clear HPD if power 5v removed

					if(HdmiRx_R(0x0A) & 0x40){
//						while(Msc_Fire(MSC_PKT_CLR_HPD, &TxMscCmd)!=MSC_SUCCESS);
					}else{
//						HdmiRx_HpdOut(IT6662_HPD_L);
						HdmiRx_HpdClr(stIt6662->bRxHpdStatus);
					}
#endif
				}
			}
			if(u8Reg05[0] & 0x02) {		iTE_HRx_Msg(("HDMI Rx CLK On\n"));			}
			if(u8Reg05[0] & 0x04) {
				g_u8MhlClkWait = 0;
				u8RdData = HdmiRx_R(0x0C) & 0x40;
				iTE_MEQ_Msg(("HDMI Rx CLK Stable Change to 0x%X\n", u8RdData));
				if(u8RdData == 0){	// Disable Tx HDCP
					if(g_u8CurDev == IT6662_ROOT){
						g_u1HdcpEn = 0;
					}
					stIt6662->bRxTrgTxHdcp	= iTE_FALSE;
					HdmiTx_HdcpDisable();
					stIt6662->bForceCsc = 0;
				}else{
					g_bCscChk = 1;
				}

				HdmiRx_Set(0x10, 0x01, 0x01);
				iTE_Sleep_ms(100);
				HdmiRx_Set(0x10, 0x01, 0x00);
//				iTE_Sleep_ms(100);
				if(u8RdData){
					HdmiRx_GetTmdsClk();
#if (USING_AUTO_EQ == iTE_TRUE)
					if(g_u8CurDev == IT6662_ROOT){
#if 0
						g_u8EqDelayTrigger = 1;
						HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
#else
#if (USING_MANUL_EQ == iTE_TRUE)
						HdmiRx_Set(0x11, 0x10, 0x10);
						HdmiRx_Set(0x11, 0x10, 0x00);
						HdmiRx_ManulEqTrigger();
#else
						HdmiRx_AutoEqTrigger();
#endif
#endif
					}
#endif
				}
			}
			if(u8Reg05[0] & 0x20) {
				iTE_HRx_Msg(("Input %s Mode\n", (u8Reg05[5] & 0x02)? "HDMI" : "DVI"));
				if((u8Reg05[5] & 0x02) == 0x00){
					g_u8InClr = 0;
				}
				g_bCscChk = 1;
				HdmiRx_Set(0x6E, 0x01, 0x01);
#if (USING_1to8 == iTE_TRUE)
				if(g_u8CurDev == IT6662_ROOT){
					IT6662_DeviceSelect(IT6662_A);
					HdmiTx_HdcpDisable();
					IT6662_DeviceSelect(IT6662_B);
					HdmiTx_HdcpDisable();
					IT6662_DeviceSelect(IT6662_ROOT);
				}
#else
				HdmiTx_HdcpDisable();
#endif
			}
			if(u8Reg05[0] & 0x08) {
				iTE_HRx_Msg(("HDCP Authentication Start \n"));
				if(HdmiRx_R(0x0A) & 0x40){
					g_u8MhlHdcpCnt = 2;
					HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
				}
			}
			if(u8Reg05[0] & 0x10) {
				iTE_HRx_Msg(("HDCP Authentication Done. 0x%02X\n", HdmiRx_R(0x93) & 0xF0));
				if(HdmiRx_R(0x0A) & 0x40){
					g_u8MhlHdcpCnt = 0;
				}
				if(HdmiRx_R(0x0C) & 0x40){
					g_u1HdcpEn = 1;
				}
//				HdmiRx_ShowHdcpStatus();
#if (USING_CASCADE== iTE_TRUE)
					if((HdmiTx_R(0xC1) & 0x0F) != 0x0F) {
						if((HdmiTx_R(0x10) & 0x03) == 0x03){
							if(stIt6662->bRxTrgTxHdcp == iTE_FALSE){
								stIt6662->bRxTrgTxHdcp = iTE_TRUE;
								HdmiTx_AuthHdcp();
							}
						}
					}
#else
					u8RdData = g_u8CurDev;
					IT6662_DeviceSelect(IT6662_A);
					if((HdmiTx_R(0xC1) & 0x0F) != 0x0F) {
						if((HdmiTx_R(0x10) & 0x03) == 0x03){
							if(stIt6662->bRxTrgTxHdcp == iTE_FALSE){
								stIt6662->bRxTrgTxHdcp = iTE_TRUE;
								HdmiTx_AuthHdcp();
							}
						}
					}
					IT6662_DeviceSelect(IT6662_B);
					if((HdmiTx_R(0xC1) & 0x0F) != 0x0F) {
						if((HdmiTx_R(0x10) & 0x03) == 0x03){
							if(stIt6662->bRxTrgTxHdcp == iTE_FALSE){
								stIt6662->bRxTrgTxHdcp = iTE_TRUE;
								HdmiTx_AuthHdcp();
							}
						}
					}
					IT6662_DeviceSelect(u8RdData);
//						stIt6662->u16DevCnt = 0;
//						stIt6662->u8MaxDepth = 0;

#endif
			}
			if(u8Reg05[0] & 0x40) {
#if (USING_AUTO_EQ == iTE_TRUE)
				if(g_u8CurDev == IT6662_ROOT){
					HdmiRx_EqChk();
				}


				if(g_bManulEqEn || HdmiRx_R(0x22)){
				}else
#endif
				{
					if((++stIt6662->u8EccErrCnt % 10) == 0){
						if((stIt6662->u8EccErrCnt % 20) == 0){
							HdmiRx_AfeReset();
						}
						if((stIt6662->u8EccErrCnt % 40) == 0){
							HdmiRx_TrgHpd(10);
						}
					}
				}
				iTE_HRx_Msg(("ECC error %d\n", stIt6662->u8EccErrCnt));
			}
			if(u8Reg05[0] & 0x80) {
				iTE_HRx_Msg(("HDCP OFF \n"));
				HdmiRx_Set(0x11, 0x02, 0x02);
				HdmiRx_Set(0x11, 0x02, 0x00);
//				EnHDCP = iTE_FALSE;

			}
		}

		if(u8Reg05[2]) {
			if(u8Reg05[2] & 0x01) {
				iTE_HRx_Msg(("CDSen = 0x%X\n", u8Reg05[5] & 0x40));
				if(u8Reg05[5] & 0x40){
					HdmiRx_HpdOut(IT6662_HPD_AUTO);
				}else{
					if(stIt6662->bRxHpdStatus == 1){
						HdmiRx_HpdSet();
					}else{
						HdmiRx_HpdClr(0);
					}
				}
			}
			if(u8Reg05[2] & 0x02) {
				if(u8Reg05[5] & 0x80){
					iTE_HRx_Msg(("Video Stable\n"));
				}else{
					iTE_HRx_Msg(("Video UnStable\n"));
				}
			}
			if(u8Reg05[2] & 0x04) {		iTE_HRx_Msg(("Video Mode Changed \n"));		}
			if(u8Reg05[2] & 0x08) {		HdmiRx_Sw_Timer_IRQ();							}
			if(u8Reg05[2] & 0x10) {		iTE_HRx_Msg(("Set Mute Packet Is Received\n"));		}
			if(u8Reg05[2] & 0x20) {		iTE_HRx_Msg(("Left Mute Packet Is Received\n"));		}
			if(u8Reg05[2] & 0x40) {
				iTE_HRx_Msg(("HDMI Rx DDC Bus Hang\n"));
				HdmiRx_Set(0xC0, 0x20, 0x20);
				iTE_Sleep_ms(1);
				HdmiRx_Set(0xC0, 0x20, 0x00);
			}
		}

		if(u8Reg05[3]) {
			if(u8Reg05[3] & 0x01) {		iTE_HRx_Msg(("ISRC1 Packet Detect \n"));			}
			if(u8Reg05[3] & 0x02) {		iTE_HRx_Msg(("ISRC2 Packet Detect \n"));			}
			if(u8Reg05[3] & 0x04) {		iTE_HRx_Msg(("General Packet Detect \n"));			}
			if(u8Reg05[3] & 0x08) {
				u8RdData = HdmiRx_R(0x99) >> 4;
				iTE_HRx_Msg(("Color Depth Change to  %d bits\n", (u8RdData == 7)?48:u8RdData*6));
				if(u8RdData == 0x05) {
					HdmiRx_Set(0x65, 0x0C, 0x04);
				}else if(u8RdData == 0x06) {
					HdmiRx_Set(0x65, 0x0C, 0x08);
				}else {
					HdmiRx_Set(0x65, 0x0C, 0x00);
				}
			}
			if(u8Reg05[3] & 0x10) {
				iTE_HRx_Msg(("No AVI InfoFrame Received\n"));
				g_bCscChk = 1;
			}
			if(u8Reg05[3] & 0x20) {		iTE_HRx_Msg(("No Audio InfoFrame Received\n"));		}
			if(u8Reg05[3] & 0x40) {		iTE_HRx_Msg(("No General Packet Recieved\n"));		}
		}

		if(u8Reg05[4]) {
			if(u8Reg05[4] & 0x01) {
				iTE_HRx_Msg(("New AVI InfoFrame Received\n"));
				HdmiRx_Set(0x6E, 0x01, 0x00);
				if (g_u8CurDev == IT6662_ROOT){
					HdmiRx_ShowVidInfo();
					g_bCscChk = 1;
				}
			}
			if(u8Reg05[4] & 0x10) {		iTE_HRx_Msg(("New Audio InfoFrame Received\n"));	}
			if(u8Reg05[4] & 0x20) {
				iTE_HRx_Msg(("**** DeSkew Error **** %d\n", stIt6662->u8DeskewFailCnt));
				if(++stIt6662->u8DeskewFailCnt >= 0x20){
					stIt6662->u8DeskewFailCnt = 0;
					HdmiRx_AfeReset();
				}
			}
			if(u8Reg05[4] & 0x80) {		iTE_HRx_Msg(("H2V Buffer Skew Fail\n"));			}
		}
		iTE_HRx_Msg(("HdmiRx_Irq 1 E\n"));
	}

	u8RdData = HdmiRx_R(0xD0);
	HdmiRx_W(0xD0, u8RdData);

	if(u8RdData) {
		iTE_MEQ_Msg(("\r\n HdmiRx_Irq AutoEQ S %02X\n", u8RdData));
		if(u8RdData & 0x01) {		iTE_HRx_Msg(("Auto EQ Update\n"));					}
		if(u8RdData & 0x04) {		iTE_HRx_Msg(("Rx Clock Change Detect \n"));			}
#if (USING_AUTO_EQ == iTE_TRUE)
		if(u8RdData & 0x30) {
				iTE_u8	u8Reg1D5[3];
				HdmiRx_Bank(1);
				HdmiRx_Rb(0xD5, 0x03, u8Reg1D5);
				HdmiRx_Bank(0);
				iTE_MEQ_Msg((" EQ Value = 0x%02X, %02X, %02X\n", u8Reg1D5[0], u8Reg1D5[1], u8Reg1D5[2]));
				g_u8EqChkCnt = 0;
			if(u8RdData & 0x10){
				iTE_MEQ_Msg(("##########AutoEQ Done 0x%02X, 0x%02X, 0x%02X#####\n", u8Reg1D5[0], u8Reg1D5[1], u8Reg1D5[2]));
//				g_u8EqDelayTrigger = 50;
				HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
g_u8VdoParChgCnt = 0x00;
				HdmiRx_EqChk();
			}
			if(u8RdData & 0x20){
				iTE_MEQ_Msg(("############AutoEQ Fail######\n"));
g_u8VdoParChgCnt = 0xFF;
				HdmiRx_EccChk();

			}
		}
#endif
		iTE_HRx_Msg(("HdmiRx_Irq AutoEQ E\n"));
	}
}
//****************************************************************************
void HdmiRx_Sw_Timer_IRQ(void)
{
	iTE_u8	u8Temp;

//	iTE_MEQ_Msg(("HdmiRx_Sw_Timer_IRQ %d, %d\n", g_u8EqChkCnt, g_u8EqReChkTimer));

	u8Temp = HdmiTx_Sw_Timer_IRQ(HDMI_TX_A);
	u8Temp |= HdmiTx_Sw_Timer_IRQ(HDMI_TX_B);
	u8Temp |= HdmiTx_Sw_Timer_IRQ(HDMI_TX_C);
	u8Temp |= HdmiTx_Sw_Timer_IRQ(HDMI_TX_D);


	if((USING_CASCADE == iTE_TRUE) || (g_u8CurDev == IT6662_ROOT)){
#if (USING_AUTO_EQ == iTE_TRUE)
		if(g_u8EqChkCnt){
			iTE_u8	u8RegB2[5] = {1,1,1,1,1};
			if(--g_u8EqChkCnt){
				g_u8VdoParChgCnt = 0;
//				HdmiRx_Rb(0xB2, 0x05, u8RegB2);
				HdmiRx_Wb(0xB2, 0x05, u8RegB2);
				u8Temp = 1;
			}else{
				HdmiRx_EccChk();
			}
//			iTE_MEQ_Msg(("g_u8EqChkCnt = %d, Reg20=0x%X\n", g_u8EqChkCnt, HdmiRx_R(0x20)));
		}
		if(g_u8EqReChkTimer){
			if(--g_u8EqReChkTimer){
				u8Temp = 1;
			}else{
				if(HdmiRx_R(0x0A) & 0x80){
					HdmiRx_EccChk();
				}else{
					// if video unstable then re AutoEQ
#if (USING_MANUL_EQ == iTE_TRUE)
					HdmiRx_ManulEqTrigger();
#else
					HdmiRx_AutoEqTrigger();
#endif
				}
				//HdmiRx_AutoEqTrigger();
			}
		}
		if(g_u8EqDelayTrigger){
			if(--g_u8EqDelayTrigger){
				u8Temp = 1;
			}else{
#if (USING_MANUL_EQ == iTE_TRUE)
				HdmiRx_ManulEqTrigger();
#else
				HdmiRx_AutoEqTrigger();
#endif
			}
		}
#endif

		if(g_u8MhlClkWait){
			if(--g_u8MhlClkWait){
				u8Temp = 1;
			}else{
				if(HdmiRx_R(0x0A) & 0x40){
					HdmiRx_HpdClr(stIt6662->bRxHpdStatus);
				}
			}
		}
		if(g_u8MhlHdcpCnt){
			if(--g_u8MhlHdcpCnt){
				u8Temp = 1;
			}else{
				if(HdmiRx_R(0x0A) & 0x40){
					HdmiRx_HpdClr(stIt6662->bRxHpdStatus);
				}
			}
		}

		if(stIt6662->u8WakeFailSleepCnt){
			stIt6662->u8WakeFailSleepCnt--;
			if(stIt6662->u8WakeFailSleepCnt){
				u8Temp = 1;
			}else{
				MhlRx_Set(0x0A, 0x02, 0x00);
			}
		}
		if(stIt6662->u8HpdTimerCnt){
			stIt6662->u8HpdTimerCnt--;
			if(stIt6662->u8HpdTimerCnt){
				u8Temp = 1;
			}else{
				HdmiRx_HpdSet();
			}
		}
	}


	if(u8Temp){
		HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
	}else{

	}
	//Disable Sw Timer
}
//****************************************************************************
void HdmiRx_GetTmdsClk(void)
{
	iTE_u8 u8RdData;
	iTE_u32 u32TmdsClk;

	u8RdData = HdmiRx_R(0x91) + 1;

	if(u8RdData == 0x00){
		iTE_MsgE(("Reg0x91 = 0xFF\n"));
		u8RdData = 0xFF;
	}

	u32TmdsClk = stIt6662->u32RefClk * 256 / u8RdData;	//10MHz
	if(PwrDnRCLK == iTE_FALSE) {
		if(HDMITxRCLKFreqSel) {
			u32TmdsClk = u32TmdsClk / 2;	// 20MHz
		}
	} else {
		if(RCLKPDSel == 0) {
			u32TmdsClk *= 2;
		} else {
			u32TmdsClk *= 4;
		}
	}

	u8RdData = HdmiRx_R(0x90);
	if(u8RdData & 0x80){
		u32TmdsClk *= 4;
	}else if(u8RdData & 0x40){
		u32TmdsClk *= 2;
	}
	iTE_MEQ_Msg(("TMDSCLK = %d.%03dMHz\n", (int)(u32TmdsClk/1000), (int)(u32TmdsClk%1000)));
	u8RdData = HdmiRx_R(0x0A);
	iTE_HRx_Msg(("TMDS High Speed = %x\n", u8RdData & 0x10));

	if(u8RdData & 0x40){
		if((MhlRx_R(0xB1) & 0x07) == 0x02){
			u32TmdsClk /= 2;
		}
	}


	if(u32TmdsClk > TMDS_4k2k){	// 4k2k
		HdmiTx_W(0xC2, 0x08);
		HdmiTx_Set(0xC0, 0x70, EYE_IMPRO_4k2k << 4);
		HdmiTx_Set(0xC3, 0x9F, 0x80 | DRV_RTERM_4k2k);
		HdmiTx_W(0xC4,  (P1Swing_4k2k<<4) | P0Swing_4k2k);
		HdmiTx_W(0xC5,  (P3Swing_4k2k<<4) | P2Swing_4k2k);
	}else{
		HdmiTx_W(0xC2, 0x00);
		HdmiTx_Set(0xC0, 0x70, EYE_IMPRO << 4);
		HdmiTx_Set(0xC3, 0x9F, DRV_RTERM);
		HdmiTx_W(0xC4,  (P1Swing<<4) | P0Swing);
		HdmiTx_W(0xC5,  (P3Swing<<4) | P2Swing);
	}
//	iTE_HRx_Msg(("HRx[0x91]=0x%02X, RClk=%d \n", u8RdData,stIt6662->u32RefClk));
#if (0 && USING_AUTO_EQ == iTE_TRUE)
	if(u32TmdsClk > 160000){
		HdmiRx_Set(0x20, 0x3F, 0x3F);
	}else{
		HdmiRx_Set(0x20, 0x3F, 0x00);
	}
#endif

}
//****************************************************************************
//****************************************************************************
void HdmiRx_ShowInfo(void)
{
	VTiming 	CurVTiming;
	iTE_u8	u8Vic;
	HdmiRx_GetVideoTiming(&CurVTiming);
	u8Vic = HdmiRx_GetVideoFormat(&CurVTiming);
	HdmiRx_ShowVidInfo();
}
//****************************************************************************
void HdmiRx_GetVideoTiming(VTiming *pCurVTiming)
{
	iTE_u8 u8Reg9C[13];
	iTE_u8 u8Temp;

	u8Temp = HdmiRx_R(0x9A);
	if(u8Temp == 0x00){
		iTE_MsgE(("Reg0x9A == 0x00\n"));
		return;
	}
#if 0
	iTE_u32 u32TmdsClk;

	u8RdData = HdmiRx_R(0x91);
	u32TmdsClk = stIt6662->u32RefClk * 256 / u8RdData;	//10MHz
	if(PwrDnRCLK == iTE_FALSE) {
		if(HDMITxRCLKFreqSel) {
			u32TmdsClk = u32TmdsClk / 2;	// 20MHz
		}
	} else {
		if(RCLKPDSel == 0) {
			u32TmdsClk *= 2;
		} else {
			u32TmdsClk *= 4;
		}
	}
#endif

	HdmiRx_Rb(0x9C, 0x0D, u8Reg9C);

	pCurVTiming->PCLK		= stIt6662->u32RefClk * 255 / u8Temp;
	pCurVTiming->HTotal		= (((iTE_u16)u8Reg9C[1] & 0x3F) << 8) + u8Reg9C[0];
	pCurVTiming->HActive	= (((iTE_u16)u8Reg9C[3] & 0x3F) << 8) + u8Reg9C[2];
	pCurVTiming->HSyncWidth	= (((iTE_u16)u8Reg9C[5] & 0x01) << 8) + u8Reg9C[4];
	pCurVTiming->HFrontPorch	= (((iTE_u16)u8Reg9C[5] & 0xF0) << 4) + u8Reg9C[6];
	pCurVTiming->HBackPorch= pCurVTiming->HTotal - pCurVTiming->HActive - pCurVTiming->HFrontPorch - pCurVTiming->HSyncWidth;
	pCurVTiming->VTotal		= (((iTE_u16)u8Reg9C[8] & 0x0F) << 8) + u8Reg9C[7];
	pCurVTiming->VActive	= (((iTE_u16)u8Reg9C[8] & 0xF0) << 4) + u8Reg9C[9];
	pCurVTiming->VSyncWidth	= u8Reg9C[10] & 0x1F;
	pCurVTiming->VFrontPorch	= u8Reg9C[11] & 0x3F;
	pCurVTiming->VBackPorch= pCurVTiming->VTotal - pCurVTiming->VActive - pCurVTiming->VFrontPorch - pCurVTiming->VSyncWidth;
	pCurVTiming->VPolarity	= (u8Reg9C[12] & 0x08) >> 3;
	pCurVTiming->HPolarity	= (u8Reg9C[12] & 0x04) >> 2;
	pCurVTiming->ScanMode	= (HdmiRx_R(0x99) & 0x02) >> 1;

//	iTE_HRx_Msg(("TMDSCLK = %d.%03dMHz\n", u32TmdsClk/1000, u32TmdsClk%1000));
	iTE_HRx_Msg(("PCLK = %d.%03dMHz\n", (int)(pCurVTiming->PCLK/1000), (int)(pCurVTiming->PCLK%1000)));
	iTE_HRx_Msg(("FrameRate = %dHz\n", (int)(pCurVTiming->PCLK * 1000 / pCurVTiming->HTotal/ pCurVTiming->VTotal)));
	iTE_HRx_Msg(("HActive = %d\n", pCurVTiming->HActive));
	iTE_HRx_Msg(("VActive = %d\n", pCurVTiming->VActive));
	iTE_HRx_Msg(("HTotal = %d\n", pCurVTiming->HTotal));
	iTE_HRx_Msg(("VTotal = %d\n", pCurVTiming->VTotal));

	iTE_HRx_Msg(("HFrontPorch = %d\n", pCurVTiming->HFrontPorch));
	iTE_HRx_Msg(("HSyncWidth = %d\n", pCurVTiming->HSyncWidth));
	iTE_HRx_Msg(("HBackPorch = %d\n", pCurVTiming->HBackPorch));
	iTE_HRx_Msg(("VFrontPorch = %d\n", pCurVTiming->VFrontPorch));
	iTE_HRx_Msg(("VSyncWidth = %d\n", pCurVTiming->VSyncWidth));
	iTE_HRx_Msg(("VBackPorch = %d\n", pCurVTiming->VBackPorch));
	iTE_HRx_Msg(("Scan Mode = %s\n", (pCurVTiming->ScanMode)?"Progressive":"InterLaced"));
	iTE_HRx_Msg(("HSync Pol = %s\n", (pCurVTiming->HPolarity)?"Positive":"Negative"));
	iTE_HRx_Msg(("VSync Pol = %s\n", (pCurVTiming->VPolarity)?"Positive":"Negative"));
}
//****************************************************************************
iTE_u8	HdmiRx_GetVideoFormat(VTiming *pCurVTiming)
{

	iTE_u8	u8Temp;
#if 0
	for(u8Temp = 0; u8Temp < (SizeofVMTable -1) ; u8Temp++) {
		if((abs(pCurVTiming->PCLK-s_VMTable[u8Temp].PCLK)>(s_VMTable[u8Temp].PCLK*5/100)) ||
		(pCurVTiming->HActive != s_VMTable[u8Temp].HActive ) ||
			//         (pCurVTiming->HBackPorch != s_VMTable[u8Temp].HBackPorch ) ||
			//         (pCurVTiming->HFrontPorch != s_VMTable[u8Temp].HFrontPorch ) ||
			//         (pCurVTiming->HSyncWidth != s_VMTable[u8Temp].HSyncWidth ) ||
		(pCurVTiming->HTotal != s_VMTable[u8Temp].HTotal ) ||
			//         (pCurVTiming->HPolarity != s_VMTable[u8Temp].HPolarity ) ||
		(pCurVTiming->VActive != s_VMTable[u8Temp].VActive ) ||
			//         (pCurVTiming->VBackPorch != s_VMTable[u8Temp].VBackPorch ) ||
			//         (pCurVTiming->VFrontPorch != s_VMTable[u8Temp].VFrontPorch ) ||
			//         (pCurVTiming->VSyncWidth != s_VMTable[u8Temp].VSyncWidth ) ||
		(pCurVTiming->VTotal != s_VMTable[u8Temp].VTotal ) )
			//         (pCurVTiming->VPolarity != s_VMTable[u8Temp].VPolarity ) ||
			//         (pCurVTiming->ScanMode != s_VMTable[u8Temp].ScanMode ) )
		continue;
		else
		break;
	}

	iTE_HRx_Msg(("Video Input Timing: %s\n", s_VMTable[u8Temp].format));
#endif
	return u8Temp;
}
//****************************************************************************
void HdmiRx_ShowVidInfo(void)
{
#if iTE_HdmiRx_Msg_Print
	iTE_ps8	strClrMode[4] = {"RGB444", "YCbCr422", "YCbCr444", "Reserved !!! *** ERROR ***"};
#endif
	iTE_u8	aTxClrDepth[2][2] = {{0x0C, 0x08}, {0x0A, 0x04}};		// Color Depth = 12, 10
	iTE_u8	u8InBPC, u8InBPP, u8GCP_CD;
	iTE_u8	u8Temp;

	u8GCP_CD = HdmiRx_R(0x99) >> 4;
	if((u8GCP_CD > 0x04) && (u8GCP_CD < 0x07)) {
		u8InBPC = aTxClrDepth[u8GCP_CD & 0x01][0];
		HdmiRx_Set(0x65, 0x0C, aTxClrDepth[u8GCP_CD & 0x01][1]);
	}else{
		u8InBPC = 0x08;
		HdmiRx_Set(0x65, 0x0C, 0x00);
	}

	iTE_HRx_Msg(("Input Color Depth = %d bits per pixel\n", u8InBPC * 3));
//	iTE_HRx_Msg(("Output Color Depth = %d bits per pixel\n", OutCD*6 + 24));

	HdmiRx_Bank(2);
	g_u8InClr = (HdmiRx_R(0x28) & 0x60) >> 5;	// 0x00:RGB, 0x20:YCbCr422, 0x40: YCbCr444, 0x60: Reserved.
	iTE_HRx_Msg(("Input Color Mode is %s\n", strClrMode[g_u8InClr]));
	HdmiRx_Bank(0);

	stIt6662->bForceCsc = 0;
	u8Temp = HdmiRx_R(0x0A);

	if(u8Temp & 0x40) {				// MHL_Mode
		u8Temp = MhlRx_R(0xB1) & 0x07;
		if( (u8Temp == 0x02) && (g_u8InClr != 0x01)){	// MHL PackPixel mode only support YUV422
			stIt6662->bForceCsc = 1;
			g_u8InClr = 0x01;
			iTE_HRx_Msg(("**************************************************************************\n"));
			iTE_HRx_Msg(("**************************************************************************\n"));
			iTE_HRx_Msg(("**************************************************************************\n"));
		}
		iTE_HRx_Msg(("BUS MODE: MHL %s Mode (%s)\n", (u8Temp == 0x02)?"PackPixel":"24 bits", (u8Temp & 0x02)?"HDMI":"DVI"));

	} else {							// HDMI Mode
		iTE_HRx_Msg(("BUS MODE:  %s Mode\n", (((u8Temp & 0x02) && (!(HdmiRx_R(0x51) & 0x01))) || ((HdmiRx_R(0x0B) & 0x02) && (HdmiRx_R(0x51) & 0x01)))?"HDMI" : "DVI"));
	}

	if(stIt6662->bForceCsc){
		HdmiRx_Set(0x71, 0x08, 0x08);
	}else{
		HdmiRx_Set(0x71, 0x08, 0x00);
	}

	if(g_u8InClr == 0x01) {	//YCbCr422
		u8InBPP = u8InBPC * 2;
	} else {
		u8InBPP = u8InBPC * 3;
	}


}
//****************************************************************************
void HdmiRx_CscOutputSet(iTE_u8 u8OutClr)
{
	iTE_u8	u8Reg0x1C0[3];
#if iTE_HdmiRx_Msg_Print
	iTE_ps8	strClrMode[4] = {"RGB444", "YCbCr422", "YCbCr444", "Reserved !!! *** ERROR ***"};
#endif
#if	(USING_CSC_OUT_RGB == iTE_TRUE)
	u8OutClr = 0;
#endif
	iTE_HRx_Msg(("HdmiRx_CscOutputSet %s\n", strClrMode[u8OutClr]));

	HdmiTx_Bank(1);
	HdmiTx_Rb(0xC0, 0x03, u8Reg0x1C0);											// read back AVI packet from RX
	HdmiTx_Set(0xC0, 0xE0, u8OutClr << 5);
	HdmiTx_Bank(0);

	if(g_u8InClr && (u8OutClr == 0)){		// YUV to RGB
		if(g_u8InClr == 1){
			Csc_Yuv422ToRgb();
		}else{
			Csc_Yuv444ToRgb();
		}
	}else if((g_u8InClr == 0) && u8OutClr){	//RGV to YUV
		if(u8OutClr == 1){
			Csc_RgbToYuv422();
		}else{
			Csc_RgbToYuv444();
		}
	}else{								// Bypass CSC
		if(u8OutClr == 1){
			HdmiRx_Set(0x65, 0x33, 0x10);
		}else{
			HdmiRx_Set(0x65, 0x33, 0x20);
		}
	}

//	HdmiTx_W(0x23, 0x00);	move to upper layer
//	stIt6662->u8OutClr = u8OutClr;
}
//****************************************************************************
void HdmiRx_AfeReset(void )
{
	HdmiRx_Set(0x11, 0x0C, 0x0C);
	iTE_Sleep_ms(1);
	HdmiRx_Set(0x11, 0x0C, 0x00);
}
//****************************************************************************

//****************************************************************************
#if iTE_HdmiRx_Msg_Print
void HdmiRx_ShowHdcpStatus(void)
{
	iTE_u8	u8Temp[8];

	HdmiRx_Bank(2);
	HdmiRx_Rb(0x9A, 0x05, u8Temp);
	iTE_HRx_Msg(("AKSV Received by Rx Port = 0x%02X%02X%02X%02X%02X\n", u8Temp[4], u8Temp[3], u8Temp[2], u8Temp[1], u8Temp[0]));
	HdmiRx_Rb(0x9F, 0x05, u8Temp);
	iTE_HRx_Msg(("Rx Port BKSV = 0x%02X%02X%02X%02X%02X\n", u8Temp[4], u8Temp[3], u8Temp[2], u8Temp[1], u8Temp[0]));
	HdmiRx_Rb(0xA4, 0x08, u8Temp);
	iTE_HRx_Msg(("M0 = 0x%02X%02X%02X%02X%02X%02X%02X%02X\n",(~u8Temp[7])&0xFF, (~u8Temp[6])&0xFF, (~u8Temp[5])&0xFF, (~u8Temp[4])&0xFF, (~u8Temp[3])&0xFF, (~u8Temp[2])&0xFF, (~u8Temp[1])&0xFF, (~u8Temp[0])&0xFF));
	HdmiRx_Bank(0);
}
#endif
//****************************************************************************
void HdmiRx_HpdOut(iTE_u8 u8OutStatus)
{
	iTE_u8	u8TmpBank = HdmiRx_R(0x0F);

	iTE_HRx_Msg(("HdmiRx_HpdOut %d\n", u8OutStatus));

	HdmiRx_Bank(1);
	HdmiRx_Set(0xB0, 0x03, u8OutStatus);

#if	(DISABLE_EDID_WHEN_HPD_LOW == iTE_TRUE)
	HdmiRx_Bank(0);
	if(u8OutStatus == IT6662_HPD_L){
		HdmiRx_Set(0xC0, 0x01, 0x01);
	}else{
		HdmiRx_Set(0xC0, 0x01, 0x00);
	}
#endif
	HdmiRx_W(0x0F, u8TmpBank);
}
//****************************************************************************
void HdmiRx_HpdClr(iTE_u1 bHpdSta)
{
	iTE_HRx_Msg(("HdmiRx_HpdClr %d\n", bHpdSta));
	stIt6662->bRxHpdStatus = bHpdSta;
	stIt6662->u8EccErrCnt = 0;
	stIt6662->u8DeskewFailCnt= 0;

	if(HdmiRx_R(0x0A) & 0x40){
		if(stIt6662->bCbusDiscov){
#if 0
			Mhl_MscFire(MSC_PKT_CLR_HPD, &TxMscCmd);

			MhlRx_Set(0x28, 0x50, 0x40);
			iTE_Sleep_ms(50);
			MhlRx_Set(0x28, 0x50, 0x00);
			iTE_Sleep_ms(50);
#else
			stIt6662->bCbusDiscov = 0;
			HdmiRx_HpdOut(IT6662_HPD_H);
			HdmiRx_Set(0x14, 0x10, 0x10);
			MhlRx_Set(0x28, 0x50, 0x40);
			iTE_Sleep_ms(20);
			MhlRx_Set(0x28, 0x50, 0x00);
			HdmiRx_Set(0x14, 0x10, 0x00);
			HdmiRx_HpdOut(IT6662_HPD_AUTO);
#endif

		}else{
//		mhl_CBusReDiscovery();


		}
	}else{
		HdmiRx_HpdOut(IT6662_HPD_L);
	}
}
//****************************************************************************
void HdmiRx_HpdSet(void)
{
	iTE_HRx_Msg(("HdmiRx_HpdSet\n"));
	stIt6662->bRxHpdStatus = 1;

#if 1		// reset Rx HDCP if HPD output low 1s
		stIt6662->u8EccErrCnt = 0;
		stIt6662->u8DeskewFailCnt= 0;
		g_u1HdcpEn = 0;
		stIt6662->bRxTrgTxHdcp	= iTE_FALSE;

		HdmiRx_Set(0x31, 0x40, 0x00);
		HdmiRx_Set(0x11, 0x02, 0x02);
		HdmiRx_Set(0x11, 0x02, 0x00);
#endif

	if(HdmiRx_R(0x0A) & 0x40){
		if(stIt6662->bCbusDiscov){
//			HdmiRx_Set(0x14, 0x10, 0x10);
//			iTE_Sleep_ms(CBUSINITIME*2);
//			HdmiRx_Set(0x14, 0x10, 0x00);

//			Mhl_MscFire(MSC_PKT_CLR_HPD, &TxMscCmd);

//			iTE_Sleep_ms(CBUSINITIME);

			g_u8MhlClkWait = 50;
			HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms

			TxMscCmd.ucOffset = 0x30;
			TxMscCmd.ucValue = MHL_STS_DCAP_RDY;
			Mhl_MscFire(MSC_PKT_WRITE_STAT, &TxMscCmd);
			iTE_MRx_Msg(("Set CapRdy by FW ...\n"));

			iTE_Sleep_ms(CBUSINITIME);
			TxMscCmd.ucOffset = 0x31;
			TxMscCmd.ucValue = MHL_STS_PATH_EN;
			Mhl_MscFire(MSC_PKT_WRITE_STAT, &TxMscCmd);
			iTE_MRx_Msg(("Set PathEn by FW ...\n"));

			iTE_Sleep_ms(CBUSINITIME);
			TxMscCmd.ucOffset = 0x21;
			TxMscCmd.ucValue = MHL_INT_EDID_CHG;
			Mhl_MscFire(MSC_PKT_WRITE_STAT, &TxMscCmd);
			iTE_MRx_Msg(("Set EDID change by FW ...\n"));			// for LG on-off power

			iTE_Sleep_ms(CBUSINITIME*30);	  	// min 8 for Galxy note Read DevCap after DCAP_RDY
			Mhl_MscFire(MSC_PKT_SET_HPD, &TxMscCmd);
			iTE_MRx_Msg(("Set HPD by FW ...\n"));
		}
	}else{
		HdmiRx_HpdOut(IT6662_HPD_H);
	}
}
//****************************************************************************
void HdmiRx_TrgHpd(iTE_u8 u8TimeCnt)
{
	iTE_u8	u8Temp = HdmiRx_R(0x0A);

	iTE_Msg(("HdmiRx_TrgHpd =%x, %02X, %02X\n", u8TimeCnt, u8Temp, stIt6662->u8HpdTimerCnt));

	if(u8Temp & 0x40){
		if((u8Temp & 0x01) != 0x01){
			u8TimeCnt = 0;
			stIt6662->bRxHpdStatus = 1;
		}else{
			u8TimeCnt = 10;
		}
	}

	if(u8TimeCnt){
		if(stIt6662->u8HpdTimerCnt == 0){
			stIt6662->u8HpdTimerCnt = u8TimeCnt;	// 1s
			HdmiRx_HpdClr(0);
			HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
			iTE_Msg(("*****TxTrgRxHPD*****  %d\n", stIt6662->u8HpdTimerCnt));

#if 0		// reset Rx HDCP if HPD output low 1s
			stIt6662->u8EccErrCnt = 0;
			g_u1HdcpEn = 0;
			stIt6662->bRxTrgTxHdcp	= iTE_FALSE;
#endif
		}
	}
}
//****************************************************************************
#if 0
void HdmiRx_SpenHdcp(void)
{
	iTE_u8	u8Temp, u8Bstatus[2];
	iTE_u8	pu8KsvList[74];
//	iTE_pu8	pu8KsvList = stHdcpData->u8KsvList;
	iTE_u16	u16Temp = stIt6662->u16DevCnt*5;
	iTE_u16	u16MsgCnt =(u16Temp + 10) << 3;
	iTE_u8	u8M0[8], u8BV[20];
	iTE_u32 	u32Hash[5];
	iTE_u8	u8Rst = 0;

	iTE_HTx_Msg(("HdmiRx_SpenHdcp S\n"));

	u32Hash[0] = HASH_0;
	u32Hash[1] = HASH_1;
	u32Hash[2] = HASH_2;
	u32Hash[3] = HASH_3;
	u32Hash[4] = HASH_4;

	u8Temp = HdmiRx_R(0x0A) & 0x02;
	if(stIt6662->u16DevCnt & 0xFF80){
		u8Bstatus[0] = 0x80;							//Max. Device Exceeded
	}else{
		u8Bstatus[0] = stIt6662->u16DevCnt & 0xFF;
	}

	if(stIt6662->u8MaxDepth > 0x07){
		u8Bstatus[1] = (u8Temp << 3) + 0x08;			//Max. Cascade Exceeded
	}else{
		u8Bstatus[1] = (u8Temp << 3) + (stIt6662->u8MaxDepth+1);
	}
	HdmiRx_Bank(2);
	HdmiRx_Rb(0xA4, 8, u8M0);
	HdmiRx_Bank(0);

	HdmiRx_Bank(1);
	HdmiRx_W(0x10, u8Bstatus[0]);
	HdmiRx_W(0x11, u8Bstatus[1]);
	HdmiRx_W(0x2D, 0x00);

	do{
		if((u16Temp + u8Rst)> 69) {
			if(u8Rst == 4){
				HdmiRx_Rb(0x28, 60, &pu8KsvList[u8Rst]);
				u8Rst = 0;
				u16Temp -= 60;
			}else{
				HdmiRx_Rb(0x28, 65, &pu8KsvList[u8Rst]);
				u8Rst++;
				u16Temp -= 65;
			}
			SHA_Simple64(pu8KsvList, u32Hash);
		}else{
			HdmiRx_Rb(0x28, u16Temp, &pu8KsvList[u8Rst]);
			if(u16Temp + u8Rst > 64){
				SHA_Simple64(pu8KsvList, u32Hash);
				u8Rst = u16Temp + u8Rst - 64;
				u16Temp = 0;
			}else{
				u8Rst = u16Temp + u8Rst;
				break;
			}
		}

		for(u8Temp = 0; u8Temp < u8Rst; u8Temp++) {
			pu8KsvList[u8Temp] = pu8KsvList[64+u8Temp];
		}
	}while(u16Temp > 0);
	pu8KsvList[u8Rst++] = u8Bstatus[0];
	pu8KsvList[u8Rst++] = u8Bstatus[1];
	pu8KsvList[u8Rst++] = u8M0[0];
	pu8KsvList[u8Rst++] = u8M0[1];
	pu8KsvList[u8Rst++] = u8M0[2];
	pu8KsvList[u8Rst++] = u8M0[3];
	pu8KsvList[u8Rst++] = u8M0[4];
	pu8KsvList[u8Rst++] = u8M0[5];
	pu8KsvList[u8Rst++] = u8M0[6];
	pu8KsvList[u8Rst++] = u8M0[7];
	pu8KsvList[u8Rst++] = 0x80;

	if(u8Rst > 64) {
		SHA_Simple64(pu8KsvList, u32Hash);
		u8Rst -= 64;
		for(u8Temp = 0; u8Temp < u8Rst; u8Temp++) {
			pu8KsvList[u8Temp] = pu8KsvList[64+u8Temp];
		}
	}else if(u8Rst > 56) {
		for(;u8Rst < 64;){
			pu8KsvList[u8Rst++] = 0;
		}
		SHA_Simple64(pu8KsvList, u32Hash);
		u8Rst = 0;
	}

	while(u8Rst < 62){
		pu8KsvList[u8Rst++] = 0;
	}

	pu8KsvList[62] = u16MsgCnt >> 8;
	pu8KsvList[63] = u16MsgCnt & 0xFF;

	SHA_Simple64(pu8KsvList, u32Hash);

	HdmiRx_Bank(0);

	HdmiRx_Bank(1);
	for(u8Temp = 0; u8Temp < 5; u8Temp++) {
		u8BV[u8Temp*4] = u32Hash[u8Temp] & 0xFF;
		u8BV[u8Temp*4 + 1] = (u32Hash[u8Temp] >> 8) & 0xFF;
		u8BV[u8Temp*4 + 2] = (u32Hash[u8Temp] >> 16) & 0xFF;
		u8BV[u8Temp*4 + 3] = (u32Hash[u8Temp] >> 24) & 0xFF;
//		iTE_HTx_Msg(("[%d] AV.H:0x%08X \n", u8Temp,  u32Hash[u8Temp]));

	}

	HdmiRx_Wb(0x14, 0x14, u8BV);
	HdmiRx_Bank(0);

	HdmiRx_Set(0x31, 0x40, 0x40);			// Set KSV Ready to '1'
	if( HdmiRx_R(0x31) & 0x40){
		iTE_HTx_Msg(("\n\n***KSV Set Ready Success, DevCnt = %d**\n", stIt6662->u16DevCnt) );
	}else {
		iTE_HTx_Msg(("*************************WARING: KSV Set Ready Fail, DevCnt = %d****************************************\n", stIt6662->u16DevCnt) );
	}
#if 0
	iTE_HTx_Msg(("M0: 0x%02X%02X%02X%02X%02X%02X%02X%02X\n", u8M0[7], u8M0[6], u8M0[5], u8M0[4], u8M0[3], u8M0[2], u8M0[1], u8M0[0]));

	iTE_HTx_Msg(("HDMI Rx BV.H : 0x%08X	0x%08X	0x%08X	0x%08X	0x%08X\n", u32Hash[0], u32Hash[1], u32Hash[2], u32Hash[3], u32Hash[4]));

	for(u8Temp = 0; u8Temp < 20; u8Temp++){
		iTE_HTx_Msg(("BV : 0x%2X\n", u8BV[u8Temp]));
	}
#endif
//stIt6662->u16DevCnt = 0;
	iTE_HTx_Msg(("HdmiRx_SpenHdcp E\n"));
}
#else
void HdmiRx_SpenHdcp(void)
{
	iTE_u8	u8Temp;
	iTE_u8	u8BV[20];
	iTE_u32 	u32Hash[5];
	iTE_u8	u8Buf[10];
	iTE_pu8	u8Bstatus, u8M0;
	iTE_pu8	pu8Hash = (iTE_pu8) u32Hash;
//	iTE_u8	u8Temp1;
	iTE_HTx_Msg(("HdmiRx_SpenHdcp S\n"));

	u8Bstatus = u8Buf;
	u8M0 = &u8Buf[2];

	u8Temp = HdmiRx_R(0x0A) & 0x02;
	if(stIt6662->u16DevCnt & 0xFF80){
		u8Bstatus[0] = 0x80;							//Max. Device Exceeded
	}else{
		u8Bstatus[0] = stIt6662->u16DevCnt & 0xFF;
	}

	if(stIt6662->u8MaxDepth > 0x07){
		u8Bstatus[1] = (u8Temp << 3) + 0x08;			//Max. Cascade Exceeded
	}else{
		u8Bstatus[1] = (u8Temp << 3) + (stIt6662->u8MaxDepth+1);
	}
	HdmiRx_Bank(2);
	HdmiRx_Rb(0xA4, 8, u8M0);
	HdmiRx_Bank(1);
#if 0
	HdmiRx_W(0x10, u8Bstatus[0]);
	HdmiRx_W(0x11, u8Bstatus[1]);
#else
	HdmiRx_Wb(0x10, 0x02, u8Bstatus);
#endif
	HdmiRx_W(0x2D, 0x00);

	HdmiRx_ShaCheck(u8Buf, u32Hash, stIt6662->u16DevCnt*5);
#if 0
	for(u8Temp = 0; u8Temp < 5; u8Temp++) {
		u8BV[u8Temp*4] = u32Hash[u8Temp] & 0xFF;
		u8BV[u8Temp*4 + 1] = (u32Hash[u8Temp] >> 8) & 0xFF;
		u8BV[u8Temp*4 + 2] = (u32Hash[u8Temp] >> 16) & 0xFF;
		u8BV[u8Temp*4 + 3] = (u32Hash[u8Temp] >> 24) & 0xFF;
		iTE_HTx_Msg(("[%d] AV.H:0x%08X \n", u8Temp,  u32Hash[u8Temp]));
	}
#else
	for(u8Temp = 0; u8Temp < 20; ) {
		u8BV[u8Temp] = pu8Hash[u8Temp];
		u8Temp++;
		u8BV[u8Temp] = pu8Hash[u8Temp];
		u8Temp++;
		u8BV[u8Temp] = pu8Hash[u8Temp];
		u8Temp++;
		u8BV[u8Temp] = pu8Hash[u8Temp];
		u8Temp++;
//		iTE_HTx_Msg(("a[%d] AV.H:0x%08X \n", u8Temp>>2,  u32Hash[u8Temp >> 2]));
	}
#endif
	HdmiRx_Wb(0x14, 0x14, u8BV);
	HdmiRx_Bank(0);

	HdmiRx_Set(0x31, 0x40, 0x40);			// Set KSV Ready to '1'
	if( HdmiRx_R(0x31) & 0x40){
		iTE_HTx_Msg(("\n\n***KSV Set Ready Success, DevCnt = %d**\n", stIt6662->u16DevCnt) );
	}else {
		iTE_HTx_Msg(("*************************WARING: KSV Set Ready Fail, DevCnt = %d****************************************\n", stIt6662->u16DevCnt) );
	}
#if 0
	iTE_HTx_Msg(("M0: 0x%02X%02X%02X%02X%02X%02X%02X%02X\n", u8M0[7], u8M0[6], u8M0[5], u8M0[4], u8M0[3], u8M0[2], u8M0[1], u8M0[0]));

	iTE_HTx_Msg(("HDMI Rx BV.H : 0x%08X	0x%08X	0x%08X	0x%08X	0x%08X\n", u32Hash[0], u32Hash[1], u32Hash[2], u32Hash[3], u32Hash[4]));

	for(u8Temp = 0; u8Temp < 20; u8Temp++){
		iTE_HTx_Msg(("BV : 0x%2X\n", u8BV[u8Temp]));
	}
#endif
//stIt6662->u16DevCnt = 0;
	iTE_HTx_Msg(("HdmiRx_SpenHdcp E\n"));
}
#endif
//****************************************************************************
void HdmiRx_ShaCheck(iTE_pu8 pu8Buf, iTE_pu32 pu32Hash, iTE_u16 u16DevCnt)
{
	iTE_u8	pu8KsvList[74];
	iTE_u8	u8Rst = 0;
	iTE_u8	u8Temp;
	iTE_u16	u16MsgCnt;

	u16MsgCnt = (u16DevCnt + 10) <<3;

	pu32Hash[0] = HASH_0;
	pu32Hash[1] = HASH_1;
	pu32Hash[2] = HASH_2;
	pu32Hash[3] = HASH_3;
	pu32Hash[4] = HASH_4;

	do{
		if((u16DevCnt + u8Rst)> 69) {
			if(u8Rst == 4){
				HdmiRx_Rb(0x28, 60, &pu8KsvList[u8Rst]);
				u8Rst = 0;
				u16DevCnt -= 60;
			}else{
				HdmiRx_Rb(0x28, 65, &pu8KsvList[u8Rst]);
				u8Rst++;
				u16DevCnt -= 65;
			}
			SHA_Simple64(pu8KsvList, pu32Hash);
		}else{
			HdmiRx_Rb(0x28, u16DevCnt, &pu8KsvList[u8Rst]);
			if(u16DevCnt + u8Rst > 64){
				SHA_Simple64(pu8KsvList, pu32Hash);
				u8Rst = u16DevCnt + u8Rst - 64;
				u16DevCnt = 0;
			}else{
				u8Rst = u16DevCnt + u8Rst;
				break;
			}
		}

		for(u8Temp = 0; u8Temp < u8Rst; u8Temp++) {
			pu8KsvList[u8Temp] = pu8KsvList[64+u8Temp];
		}
	}while(u16DevCnt > 0);
	pu8KsvList[u8Rst++] = pu8Buf[0];
	pu8KsvList[u8Rst++] = pu8Buf[1];
	pu8KsvList[u8Rst++] = pu8Buf[2];
	pu8KsvList[u8Rst++] = pu8Buf[3];
	pu8KsvList[u8Rst++] = pu8Buf[4];
	pu8KsvList[u8Rst++] = pu8Buf[5];
	pu8KsvList[u8Rst++] = pu8Buf[6];
	pu8KsvList[u8Rst++] = pu8Buf[7];
	pu8KsvList[u8Rst++] = pu8Buf[8];
	pu8KsvList[u8Rst++] = pu8Buf[9];
	pu8KsvList[u8Rst++] = 0x80;

	if(u8Rst > 64) {
		SHA_Simple64(pu8KsvList, pu32Hash);
		u8Rst -= 64;
		for(u8Temp = 0; u8Temp < u8Rst; u8Temp++) {
			pu8KsvList[u8Temp] = pu8KsvList[64+u8Temp];
		}
	}else if(u8Rst > 56) {
		for(;u8Rst < 64;){
			pu8KsvList[u8Rst++] = 0;
		}
		SHA_Simple64(pu8KsvList, pu32Hash);
		u8Rst = 0;
	}

	while(u8Rst < 62){
		pu8KsvList[u8Rst++] = 0;
	}

	pu8KsvList[62] = u16MsgCnt >> 8;
	pu8KsvList[63] = u16MsgCnt & 0xFF;

	SHA_Simple64(pu8KsvList, pu32Hash);
}
//****************************************************************************
#if (USING_KSV_PARTIAL_READ == iTE_FALSE)
#define WCNT	(16)

void SHA_Simple(iTE_u8 *pBuff, iTE_uDef uDefCnt, iTE_u32 *u32Hash)
{
	iTE_u32 u32Word[WCNT], u32Tmp;
	iTE_uDef uDefTmp;
	iTE_u8	u8Temp;

	for(uDefTmp = 0; uDefTmp < uDefCnt; uDefTmp++) {
		u32Tmp = pBuff[uDefTmp];
		if((uDefTmp & 0x03) == 0) {
			u32Word[(uDefTmp >> 2) & 0x0F] = u32Tmp << 24;
		}else{
			u32Word[(uDefTmp >> 2) & 0x0F] |= u32Tmp << (8*(3 - (uDefTmp & 0x03)));
		}

		if((uDefTmp & 0x3F) == 0x3F) {
			SHA_Transform(u32Hash, u32Word);
		}
	}


	u8Temp = (uDefTmp >> 2) & 0x0F;
	uDefTmp = uDefTmp & 0x03;
	if(uDefTmp) {
		u32Word[u8Temp] |= 0x80 << ((uDefTmp - 1) * 8);
	}else{
		u32Word[u8Temp] = 0x80000000;
	}

	if(++u8Temp < 15) {
		for(;u8Temp < 15;) {
			u32Word[u8Temp++] = 0x00000000;
		}
	}else{
		if(u8Temp == 14){
			u32Word[15] = 0x00000000;
		}
		SHA_Transform(u32Hash, u32Word);
		for(u8Temp=0; u8Temp < 14; u8Temp++) {
			u32Word[u8Temp] = 0x00000000;
		}
	}
	u32Word[14] = uDefCnt >> (32-3);
	u32Word[15] = (iTE_u32)uDefCnt << 3;

	SHA_Transform(u32Hash, u32Word);
}
#else
//****************************************************************************
void SHA_Simple64(iTE_u8 *pBuff, iTE_u32 *u32Hash)
{
	iTE_u8	u8Temp, u8Temp1;
	iTE_u32	u32Word[16];
	iTE_pu8	pu8Tmp = (iTE_u8*)u32Word;

	for(u8Temp1 = 0, u8Temp = 0; u8Temp < 64; u8Temp+=4){
		pu8Tmp[u8Temp+3] = pBuff[u8Temp1++];
		pu8Tmp[u8Temp+2] = pBuff[u8Temp1++];
		pu8Tmp[u8Temp+1] = pBuff[u8Temp1++];
		pu8Tmp[u8Temp] = pBuff[u8Temp1++];
//			iTE_HTx_Msg(("w[%d]=0x%08X\n", u8Temp>>2,u32Word[u8Temp>>2]));
	}
	SHA_Transform(u32Hash, u32Word);
}
#endif
//****************************************************************************

void SHA_Transform(iTE_u32 *u32Hash, iTE_pu32 u32Word)
{
#define SHA_ROL(x,y) (((x) << (y)) | (((iTE_u32)x) >> (32-y)))
	iTE_u32	u32Tmp;
	iTE_u8	u8Temp = 0;
	iTE_u32	u32HashTmp[5];


	u32HashTmp[0]  = u32Hash[0] ;
	u32HashTmp[1]  = u32Hash[1] ;
	u32HashTmp[2]  = u32Hash[2] ;
	u32HashTmp[3]  = u32Hash[3] ;
	u32HashTmp[4]  = u32Hash[4] ;

	do {
		if( u8Temp >= 16) {
			u32Tmp = u32Word[(u8Temp -3) & 0x0F] ^ u32Word[(u8Temp -8) & 0x0F] ^ u32Word[(u8Temp -14) & 0x0F] ^ u32Word[(u8Temp -16) & 0x0F];
			u32Word[u8Temp & 0x0F] = SHA_ROL(u32Tmp, 1);
		}else{
//			iTE_HTx_Msg(("w[%02d] = 0x%08X\n", u8Temp, u32Word[u8Temp]));
		}
//		iTE_HTx_Msg(("w[%02d] = 0x%08X, a=0x%08X, b=0x%08X, c=0x%08X, d=0x%08X, e=0x%08X\n",
//			u8Temp, u32Word[u8Temp & 0x0F], u32Hash[0], u32Hash[1], u32Hash[2], u32Hash[3], u32Hash[4]));

		if(u8Temp < 20){
			u32Tmp = SHA_ROL(u32Hash[0], 5) + ((u32Hash[1] & u32Hash[2]) | (~u32Hash[1] & u32Hash[3])) + 0x5A827999;
		}else if(u8Temp < 40) {
			u32Tmp = SHA_ROL(u32Hash[0], 5) + (u32Hash[1] ^ u32Hash[2] ^ u32Hash[3]) + 0x6ED9EBA1;
		}else if(u8Temp < 60) {
			u32Tmp = SHA_ROL(u32Hash[0], 5) + ((u32Hash[1] & (u32Hash[2] |  u32Hash[3])) | (u32Hash[2] & u32Hash[3])) + 0x8F1BBCDC;
		}else{
			u32Tmp = SHA_ROL(u32Hash[0], 5) + (u32Hash[1] ^ u32Hash[2] ^ u32Hash[3]) + 0xCA62C1D6;
		}
		u32Tmp += u32Hash[4] + u32Word[u8Temp & 0x0F];

		u32Hash[4] = u32Hash[3];
		u32Hash[3] = u32Hash[2];
		u32Hash[2] = SHA_ROL(u32Hash[1], 30);
		u32Hash[1] = u32Hash[0];
		u32Hash[0] = u32Tmp;
	}while(++u8Temp < 80);

//	iTE_HTx_Msg((" a=0x%08X, b=0x%08X, c=0x%08X, d=0x%08X, e=0x%08X\n",u32Hash[0], u32Hash[1], u32Hash[2], u32Hash[3], u32Hash[4]));

	u32Hash[0] += u32HashTmp[0];
	u32Hash[1] += u32HashTmp[1];
	u32Hash[2] += u32HashTmp[2];
	u32Hash[3] += u32HashTmp[3];
	u32Hash[4] += u32HashTmp[4];
//	iTE_HTx_Msg((" Sha =0x%08X, b=0x%08X, c=0x%08X, d=0x%08X, e=0x%08X\n",u32Hash[0], u32Hash[1], u32Hash[2], u32Hash[3], u32Hash[4]));
}
//****************************************************************************
#if (USING_AUTO_EQ == iTE_TRUE)
void HdmiRx_AutoEqTrigger(void)
{
	iTE_u8	u8Temp;

	u8Temp = HdmiRx_R(0x0A);
	iTE_MEQ_Msg(("HdmiRx_AutoEqTrigger\r\n"));
	if(g_u8CurDev == IT6662_ROOT){
		iTE_MEQ_Msg(("HdmiRx_AutoEqTrigger 0x%02X\n", u8Temp));
		if(HdmiRx_R(0x0C) & 0x40){
			iTE_MEQ_Msg(("Rx Clock Stable\r\n"));

			if(g_u8AutoEqTog != 0x01){
				if(u8Temp & 0x40){
					HdmiRx_Set(0x2C, 0x18, 0x00);
				}else{
					HdmiRx_Set(0x2C, 0x18, 0x08);//0x18);
				}
				HdmiRx_Bank(1);
				if(u8Temp & 0x40){	// MHL Mode
					HdmiRx_Set(0xB8, 0x80, 0x00);
					HdmiRx_Set(0xB6, 0x07, 0x00);
				}else{
					HdmiRx_Set(0xB8, 0x80, 0x00);
					HdmiRx_Set(0xB6, 0x07, 0x03);
				}
				HdmiRx_Bank(0);
					iTE_MEQ_Msg(("###Trigger AutoEQ###\n"));
				HdmiRx_Set(0x20, 0x3F, 0x00);
					HdmiRx_W(0x22, 0x38);
					HdmiRx_Set(0x26, 0x20, 0x00);
					HdmiRx_Set(0x11, 0x10, 0x10);
					HdmiRx_Set(0x11, 0x10, 0x00);
					HdmiRx_W(0xD0, 0x30);
					HdmiRx_W(0x22, 0x3C);
					HdmiRx_W(0x22, 0x38);
					g_bManulEqEn = 0;
					g_u8EqChkCnt = 10;
					g_u8AutoEqTog = 0;
					g_u8EqReChkCnt = 0;
					g_u8ManulEqIdx = 0;
					g_bManulEqEnDone = 0;
					g_u8EqReChkTimer = 0;
			}else{			// modify for Rx Clk re ON if AutoEq polarity toggle
				iTE_MEQ_Msg(("AutoEQ not finish\n"));
			}
		}else{
			iTE_MEQ_Msg(("Rx Clock UnStable\r\n"));
		}
	}
}

//****************************************************************************
void HdmiRx_AutoEQTogglePolarity(void )
{
	if(HdmiRx_R(0x22)){
	iTE_MEQ_Msg(("\r\n **************HdmiRx_AutoEQTogglePolarity\n"));
		HdmiRx_Bank(1);
		HdmiRx_Set(0xB5, 0x10, 0x00);
#if 1
		HdmiRx_W(0xB9, HdmiRx_R(0xB9) ^ 0x80);
#else
		if(HdmiRx_R(0xB9) & 0x80){
			HdmiRx_Set(0xB9, 0x80, 0x00);
		}else{
			HdmiRx_Set(0xB9, 0x80, 0x80);
		}
#endif
		HdmiRx_Set(0xB5, 0x10, 0x10);
		HdmiRx_Bank(0);
				HdmiRx_W(0x22, 0x38);
				HdmiRx_Set(0x26, 0x20, 0x00);
				HdmiRx_Set(0x11, 0x10, 0x10);
				HdmiRx_Set(0x11, 0x10, 0x00);
				HdmiRx_W(0xD0, 0x30);
				HdmiRx_W(0x22, 0x3C);
				HdmiRx_W(0x22, 0x38);

		g_u8AutoEqTog = 1;
//		g_bManulEqEn = 1;
		g_u8EqChkCnt = 10;
	}
}
//****************************************************************************
void HdmiRx_AutoEqFill(void)
{
	iTE_u8	u8Reg1D5[3];
	iTE_u8	u8Cnt1, u8Cnt2, u8Reg26, u8Temp1, u8Temp2 = 0;

	u8Reg26 = HdmiRx_R(0x26);
	if(u8Reg26 & 0x20 ){
		return;
	}
	iTE_MEQ_Msg(("\r\n **************HdmiRx_AutoEqFill\n"));
//	HdmiRx_W(0x26, u8Temp1 | 0x20);
//	HdmiRx_Set(0x26, 0x20, 0x20);
	HdmiRx_Bank(1);
	HdmiRx_Rb(0xD5, 0x03, u8Reg1D5);
	HdmiRx_Bank(0);
	u8Temp1 = HdmiRx_R(0x0A);
	if(u8Temp1 & 0x40){
		if(u8Reg1D5[0] < MinEQValue){
			u8Reg1D5[0] = MinEQValue;
			u8Reg1D5[1] = MinEQValue;
			u8Reg1D5[2] = MinEQValue;
		}else{
			u8Reg1D5[0] = u8Reg1D5[0] & 0x7F;
			u8Reg1D5[1] = u8Reg1D5[0];
			u8Reg1D5[2] = u8Reg1D5[0];
		}
	}else{
		u8Reg1D5[0] = u8Reg1D5[0] & 0x7F;
		u8Reg1D5[1] = u8Reg1D5[1] & 0x7F;
		u8Reg1D5[2] = u8Reg1D5[2] & 0x7F;
		u8Temp1 &= 0x10;
		if((u8Temp1 == 0x00) && (u8Reg1D5[0] < MinEQValue)){ u8Reg1D5[0] = MinEQValue;}
		if((u8Temp1 == 0x00) && (u8Reg1D5[1] < MinEQValue)){ u8Reg1D5[1] = MinEQValue;}
		if((u8Temp1 == 0x00) && (u8Reg1D5[2] < MinEQValue)){ u8Reg1D5[2] = MinEQValue;}
	}
	HdmiRx_W(0x22, 0x00);
	iTE_MEQ_Msg(("AutoEQ Value = 0x%02X, 0x%02X, 0x%02X\n", u8Reg1D5[0], u8Reg1D5[1], u8Reg1D5[2]));

	if(HdmiRx_R(0x0A) & 0x40){
		HdmiRx_Wb(0x27, 0x03, u8Reg1D5);
	}else{

#if 1
		for(u8Cnt1 = 0; u8Cnt1 < 3; u8Cnt1++){
			for(u8Cnt2 = 0; u8Cnt2 < 8; u8Cnt2++){
				if(u8Reg1D5[u8Cnt1] >>  u8Cnt2){
				}else{
					u8Temp1 = u8Cnt2;
	//				iTE_MEQ_Msg((" u8Cnt2 = %d\n", u8Cnt2));
					break;
				}
			}
			u8Temp2 += u8Temp1;
		}
		u8Temp2 /= 3;
		iTE_MEQ_Msg(("AutoEQ Value u8Temp2 = %d, %X, %X, %X\n", u8Temp2, u8EqValue[u8Temp2][0], u8EqValue[u8Temp2][1], u8EqValue[u8Temp2][2]));
		//HdmiRx_Wb(0x27, 0x03, u8Reg1D5);
		HdmiRx_Wb(0x27, 0x03, u8EqValue[u8Temp2]);
#else
		for(u8Cnt1 = 0; u8Cnt1 < 3; u8Cnt1++){
			for(u8Cnt2 = 0; u8Cnt2 < AutoEqLayerMax; u8Cnt2++){
				if(u8Reg1D5[u8Cnt1] <= u8AutoEqValue[u8Cnt2]){
					break;
				}
			}
			u8Temp2 += u8Cnt2;
		}
		u8Temp2 /= 3;
		u8Reg1D5[0] = u8AutoEqValue[u8Temp2];
		u8Reg1D5[1] = u8AutoEqValue[u8Temp2];
		u8Reg1D5[2] = u8AutoEqValue[u8Temp2];
		HdmiRx_Wb(0x27, 0x03, u8Reg1D5);
#endif

	}
	HdmiRx_W(0x26, u8Reg26 | 0x20);
//	g_bAutoEqDone = 1;
//	g_u8ManulEqCnt = 0;
	g_u8AutoEqTog = 0xFF;
}
//****************************************************************************
void HdmiRx_EqChk(void)
{
	if(g_u8CurDev == IT6662_ROOT){
		iTE_MEQ_Msg(("HdmiRx_EqChk %d\n", g_u8EqChkCnt));

		if(g_u8EqChkCnt == 0){
			g_u8EqReChkTimer = ReCheckEQTimer;
			g_u8EqChkCnt = 2;
			HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
		}
	}
}
//****************************************************************************
void HdmiRx_EccChk(void)
{
	iTE_u8	u8RegB2[5];

	iTE_MEQ_Msg(("HdmiRx_EccChk: %d, %d\n", g_bManulEqEn, g_u8VdoParChgCnt));

	HdmiRx_Rb(0xB2, 0x05, u8RegB2);
	iTE_MEQ_Msg(("Reg0xB2=%02X, %02X, %02X, %02X, %02X\n", u8RegB2[0], u8RegB2[1], u8RegB2[2], u8RegB2[3], u8RegB2[4]));

	if(u8RegB2[0] || (u8RegB2[1]&0xE0) || u8RegB2[2] || u8RegB2[3] || u8RegB2[4] || (g_u8VdoParChgCnt > 1)){
		iTE_MEQ_Msg(("EqCnt=%d, EqIdx=%d\n", g_u8ManulEqCnt,  g_u8ManulEqIdx));
		if(g_bManulEqEn){
			if(g_u8ManulEqCnt){
				g_u16EqErrCnt[u8ManulEqTab[g_u8ManulEqIdx - 1]] = u8RegB2[3] + u8RegB2[2];
				iTE_MEQ_Msg(("g_u16EqErrCnt[%d] = 0x%X\n", u8ManulEqTab[g_u8ManulEqIdx - 1], g_u16EqErrCnt[u8ManulEqTab[g_u8ManulEqIdx - 1]] ));
				HdmiRx_ManulEqSet();
			}else{
				iTE_MsgE(("should not happen\n"));
			}
		}else{
#if (USING_MANUL_EQ == iTE_FALSE)
			if(g_u8AutoEqTog == 0){
				HdmiRx_AutoEQTogglePolarity();
			}else{
				HdmiRx_ManulEqTrigger();
			}
#endif
		}
	}else{
		if(g_bManulEqEn){
			g_bManulEqEn = 0;
		}else{
#if (USING_MANUL_EQ == iTE_FALSE)
			if(g_u8AutoEqTog < 2){
				HdmiRx_AutoEqFill();
			}
#endif
		}
	}
	iTE_MEQ_Msg(("Reg0x27=0x%02X, 0x%02X, 0x%02X\r\n", HdmiRx_R(0x27), HdmiRx_R(0x28), HdmiRx_R(0x29)));
}
//****************************************************************************
void HdmiRx_ManulEqTrigger(void)
{
	iTE_MEQ_Msg(("\r\n*****ManulEqTrigger: %d****\n\n", g_bManulEqEn));

	if(g_bManulEqEnDone){
		if(HdmiRx_R(0x0A) & 0x40){
			HdmiRx_HpdClr(stIt6662->bRxHpdStatus);
		}
	}
	if(g_bManulEqEn == 0){
		iTE_u8	u8Temp;
		g_u8ManulEqCnt = 0;
		for(u8Temp = 0; u8Temp < ManulEqMax; u8Temp++){
			g_u16EqErrCnt[u8Temp] = 0xFFFF;
		}
		g_bManulEqEn = 1;
		HdmiRx_ManulEqSet();
		g_u8EqDelayTrigger = 0;
//		g_u8AutoEqTog = 0xFF;
	}
	g_u8AutoEqTog = 0xFF;
}
//****************************************************************************
void HdmiRx_ManulEqSet(void)
{
	iTE_MEQ_Msg(("HdmiRx_ManulEqSet: %d, %d\n", g_bManulEqEn, g_u8EqChkCnt));
	if(g_bManulEqEn && (g_u8EqChkCnt == 0)){
		g_u8VdoParChgCnt = 0;
		if(g_u8ManulEqCnt < ManulEqStage){
			g_u8ManulEqIdx %= ManulEqStage;
			HdmiRx_Set(0x26, 0x20, 0x20);
			HdmiRx_W(0x22, 0x00);
			//HdmiRx_Wb(0x27, 0x03, u8ManulEQ[g_u8ManulEqIdx++]);
			iTE_MEQ_Msg(("Manul EQ Value: %02X, %02X, %02X, %02X\n", u8EqValue[u8ManulEqTab[g_u8ManulEqIdx]][0], u8EqValue[u8ManulEqTab[g_u8ManulEqIdx]][1], u8EqValue[u8ManulEqTab[g_u8ManulEqIdx]][2], u8EqValue[u8ManulEqTab[g_u8ManulEqIdx]][3]));
			HdmiRx_Set(0x20, 0x3F, u8EqValue[u8ManulEqTab[g_u8ManulEqIdx]][3]);
			HdmiRx_Wb(0x27, 0x03, u8EqValue[u8ManulEqTab[g_u8ManulEqIdx++]]);
			HdmiRx_EqChk();
			g_u8ManulEqCnt++;
			g_u8VdoParChgCnt = 0;
			return;
		}else{
			iTE_u8	u8BestPtr, u8Temp, u8EqErrList;

			u8EqErrList = 0;
			for(u8Temp = 0; u8Temp < ManulEqMax; u8Temp++){
				iTE_MEQ_Msg((" 0x%X ", g_u16EqErrCnt[u8Temp]));
				if(g_u16EqErrCnt[u8Temp] == 0){
					u8EqErrList |= 1 << u8Temp;
				}
			}
			iTE_MEQ_Msg(("\r\n ErrList = 0x%02X\n", u8EqErrList));
			u8BestPtr = 0;
			iTE_MEQ_Msg(("\r\n*****u16EqErrCnt[0] = %X****\n\n", g_u16EqErrCnt[u8ManulEqTab[0]]));
			for(u8Temp = 1; u8Temp < ManulEqStage; u8Temp++){
				iTE_MEQ_Msg(("\r\n*****u16EqErrCnt[%d] = %X****\n\n", u8Temp, g_u16EqErrCnt[u8ManulEqTab[u8Temp]]));
				if(g_u16EqErrCnt[u8ManulEqTab[u8BestPtr]] > g_u16EqErrCnt[u8ManulEqTab[u8Temp]]){
					u8BestPtr = u8Temp;
				}
			}
			//HdmiRx_Wb(0x27, 0x03, u8ManulEQ[u8BestPtr]);
			//iTE_MEQ_Msg(("\r\n*****BestPtr=%d, ManulEqValue: %02X, %02X, %02X****\n\n", u8BestPtr, u8ManulEQ[u8BestPtr][0], u8ManulEQ[u8BestPtr][1], u8ManulEQ[u8BestPtr][2]));
			HdmiRx_Set(0x20, 0x3F, u8EqValue[u8ManulEqTab[u8BestPtr]][3]);
			HdmiRx_Wb(0x27, 0x03, u8EqValue[u8ManulEqTab[u8BestPtr]]);
			iTE_MEQ_Msg(("Manul EQ Value: %02X, %02X, %02X, %02X\n", u8EqValue[u8ManulEqTab[u8BestPtr]][0], u8EqValue[u8ManulEqTab[u8BestPtr]][1], u8EqValue[u8ManulEqTab[u8BestPtr]][2], u8EqValue[u8ManulEqTab[u8BestPtr]][3]));
			g_bManulEqEn = 0;
			g_bManulEqEnDone = 1;
			g_u8EqReChkCnt++;
			if(g_u8EqReChkCnt > ReCheckEQCnt){
				g_u8EqReChkTimer = 0;
				g_u8EqReChkCnt = 0;
			}
		}
	}
}
#endif


