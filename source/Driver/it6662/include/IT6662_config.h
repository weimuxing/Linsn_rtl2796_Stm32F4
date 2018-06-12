///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_config.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_config.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _IT6662_CONFIG_H_
#define _IT6662_CONFIG_H_

#include "it6662/include/iTE_typedef.h"

#ifndef PCB_VER
#define PCB_VER						(11)
#endif

#ifndef USING_IT6661
//#define USING_IT6661				(iTE_FALSE)
#define USING_IT6661				(iTE_TRUE)

#else
#pragma message("USING_IT6661 predefined")
#endif

#ifndef USING_1to8
#define USING_1to8					(iTE_FALSE)
#else
#pragma message("USING_1to8 predefined")
#endif

#define A0_WorkAround				(iTE_FALSE)

#define USING_KSV_PARTIAL_READ	(iTE_TRUE)
#define USING_DEFAULT_3D_VIC		(iTE_FALSE)
#if (USING_1to8 == iTE_TRUE)
#define USING_CASCADE				(iTE_FALSE)//(iTE_FALSE)		//
#else
#define USING_CASCADE				(iTE_TRUE)	//always iTE_TRUE in not USING_1to8 mode
#endif
#define USING_REPEATER				(iTE_FALSE)
#define USING_CEC 					(iTE_FALSE)
#define USING_AUTO_EQ				(iTE_TRUE)
#define USING_EDID_CHG_HPD		(iTE_TRUE)

#define USING_COPY_IF_ONE_COMPOSE	(iTE_TRUE)

#define USING_FORCE_TX_HDCP_ON		(iTE_FALSE)
#define USING_FORCE_TX_HDCP_OFF		(iTE_FALSE)

#define USING_HDCP_CTS					(iTE_FALSE)
#if (USING_HDCP_CTS == iTE_TRUE)
#define USING_HDCP_RPT_RDY_CHK		(iTE_TRUE)
#else
#define USING_HDCP_RPT_RDY_CHK		(iTE_FALSE)
#endif

#define USING_MANUL_EQ					(iTE_FALSE)
#define USING_CSC_OUT_RGB				(iTE_TRUE)

#define HDMI_RX_ADR		(0x90)
#define HDMI_TX_ADR		(0x98)
#define MHL_RX_ADR			(0xE0)
#define EDID_ADR			(0xA8)
#define CEC_ADR				(0x9C)	//Rx:0x9C, Tx:0xDC  ==> |= 0x40

#define CEC_RX_SEL			(0x00)
#define CEC_TX_SEL			(0x40)


#define iTE_Msg_Print 			(1)
#define iTE_MhlRx_Msg_Print		(1)
#define iTE_HdmiRx_Msg_Print	(1)
#define iTE_HdmiTx_Msg_Print	(1)
#define iTE_Edid_Msg_Print		(1)
#define iTE_Cec_Msg_Print		(1)
#define iTE_EQ_Msg_Print			(1)
#define iTE_Error_Msg_Print		(1)

#if iTE_Msg_Print
	#include <stdio.h>
	#ifdef _MCU_IT6350_
		#include "..\..\IT6350\code\api\debug\debug_print.h"
		#if (USING_1to8 == iTE_TRUE)
		#define iTE_Msg(x)			do{	printf("\r[iTE %02X]", g_u8CurDev);printf x;}while(0)
		#else
		#define iTE_Msg(x)			do{	printf("\r[iTE]");printf x;}while(0)
		#endif
	#else
		//#define iTE_Msg(x)			do{printf x;}while(0) //@LV 20160319
		//#define iTE_Msg(x)			do{	printf("\r[iTE]");printf x;}while(0)
		#define iTE_Msg(x)			
	#endif
#else
	#define iTE_Msg(x)
#endif

#if iTE_MhlRx_Msg_Print
#define iTE_MRx_Msg(x)					iTE_Msg(x)
#else
#define iTE_MRx_Msg(x)
#endif

#if iTE_HdmiRx_Msg_Print
#define iTE_HRx_Msg(x)					iTE_Msg(x)
#else
#define iTE_HRx_Msg(x)
#endif

#if iTE_HdmiTx_Msg_Print
#define iTE_HTx_Msg(x)					iTE_Msg(x)
#else
#define iTE_HTx_Msg(x)
#endif

#if iTE_Edid_Msg_Print
#define iTE_Edid_Msg(x)					iTE_Msg(x)
#else
#define iTE_Edid_Msg(x)
#endif

#if iTE_Cec_Msg_Print
		#if (USING_1to8 == iTE_TRUE)
		#define iTE_Cec_Msg(x)					do{	printf("\r\t\t[iTE %02X %02X, %02X]", g_u8CurDev, g_u8CecSel, g_u8CecTxSel);printf x;}while(0)
		#else
		#define iTE_Cec_Msg(x)					do{	printf("\r\t\t[iTE %02X, %02X]", g_u8CecSel, g_u8CecTxSel);printf x;}while(0)
		#endif
#else
#define iTE_Cec_Msg(x)
#endif

#if iTE_EQ_Msg_Print
#define iTE_MEQ_Msg(x)					iTE_Msg(x)
#else
#define iTE_MEQ_Msg(x)
#endif

#if iTE_Error_Msg_Print
#define iTE_MsgE(x)						iTE_Msg(("****ERROR****: ")); 	iTE_Msg(x)
#else
#define iTE_MsgE(x)
#endif


#define FixOCLKD4			(iTE_FALSE)	//
#define FixOCLKD5			(iTE_FALSE)
#define PwrDnRCLK			(iTE_FALSE)	// If TRUE , must set "RCLKPDSel"
#define HDMITxRCLKFreqSel	(iTE_TRUE)	// FALSE : 10MHz,  TRUE  : 20 MHz
#define RCLKPDSel			(0)			// 0=1/2 , 1=1/4 , 2=1/8

#define EnAutoEQ				(iTE_FALSE)
#define INVHCLK				(iTE_TRUE)
#define P0INVCLKD5			(iTE_TRUE)
#define EnP0PLLBufRst		(iTE_TRUE)

#define STBMODE				(0)		//01: clock stb, 10: H stb, others: V stb.

#define RegSelVdoDly			(0x18)//(0x1F)	// suggest = 1B ~ 1F

//Reg

//#define InColorMode			(RGB444)
#define OutColorMode			(RGB444)
//#define OutCD				(OUT8B)
#define OutCD				(OUT10B) //@LV 20160329


#define OutFormat			(RGB444_SDR)

//HDCP
#define HDCPIntKey			(iTE_FALSE)
#if (USING_REPEATER == iTE_TRUE)
#define EnRepeater			(iTE_TRUE)//(iTE_FALSE)//(iTE_TRUE)
#else
#define EnRepeater			(iTE_FALSE)//(iTE_FALSE)//(iTE_TRUE)
#endif
#define En1p1Feature			(iTE_FALSE)		// Must "FALSE"
#define EnFastAuth			(iTE_FALSE)		// Check Pj (Must "FALSE")
#define BCap7B				(iTE_TRUE)
#define UpdRiSel				(iTE_TRUE)
#define HDCPInitial			(iTE_TRUE)
//WatchDog
#define DeltaNum				(1)
//Output Format


//Info Frame
#define GenPktRecType		(0x82)		// fix 0x82: to get AVI

#define EnDbg				(iTE_FALSE)
#define DbgPortSel			(0x18)
#define EnForceHDMI			(iTE_FALSE)
#define RegAVMuteSel		(iTE_FALSE)
#define RegPwdCSC			(iTE_FALSE)

//******************* Defination for EQ **************************//
#define DefaultEQ			(0x9F)//(0xBF)
#define DefaultEQB			(0xBF)
#define DefaultEQA			(0xBF)
#define MinEQValue			(0x03)
#define ManulEqMax			(16)
#define ManulEqStage			(9) // (3) (5) (8)

#ifdef HDMI_RX_INIT_TABLE
#define AutoEqLayerMax		(14)
iTE_u8 _CODE u8AutoEqValue[AutoEqLayerMax] = {0x00, 0x01, 0x02, 0x03, 0x6, 0x07, 0x0E, 0x0F, 0x1E, 0x1F, 0x3E, 0x3F, 0x7E, 0x7F};
iTE_u8  u8EqValue[ManulEqMax][4] = {
									{0x00, 0x00, 0x00, 0x00},	// 0 // 1
									{0x01, 0x01, 0x01, 0x00},	// 1 // 3
									{0x03, 0x03, 0x03, 0x00},	// 2 // 5
									{0x07, 0x07, 0x07, 0x00},	// 3 // 6
									{0x0F, 0x0F, 0x0F, 0x00},	// 4 // 7
									{0x1F, 0x1F, 0x1F, 0x00},	// 5 // 8
									{0x3F, 0x3F, 0x3F, 0x00},	// 6 // 9
									{0x7F, 0x7F, 0x7F, 0x00},	// 7 // 10
									{0x00, 0x00, 0x00, 0x3F},	// 8 // 0
									{0x01, 0x01, 0x01, 0x3F},	// 9 // 2
									{0x03, 0x03, 0x03, 0x3F},	// 10 // 4
									{0x07, 0x07, 0x07, 0x3F},	// 11 // 6
									{0x0F, 0x0F, 0x0F, 0x3F},	// 12 // 7
									{0x1F, 0x1F, 0x1F, 0x3F},	// 13 // 8
									{0x3F, 0x3F, 0x3F, 0x3F},	// 14 // 9
									{0x7F, 0x7F, 0x7F, 0x3F},	// 15 // 10
								};
iTE_u8  u8ManulEqTab[ManulEqMax] = { 10, 2, 7, 6, 4, 5, 3, 1, 9, 0, 8, 11, 12, 13, 14, 15};
//iTE_u8 _CODE u8ManulEqTab[ManulEqMax] = { 5, 6, 4, 3, 7, 2, 10, 1, 9, 0, 8};
//iTE_u8 _CODE u8ManulEqTab[ManulEqMax] = { 4, 6, 2, 7, 3, 1, 5, 0, 10, 9, 8};
//iTE_u8 _CODE u8ManulEqTab[11] = { 4, 6, 2, 7, 0, 3, 1, 5, 10, 9, 8};
#endif
#define ReCheckEQTimer		(10)
#define ReCheckEQCnt		(3)
//******************* Defination for EDID **************************//
//EDID
#define EDID_RETRY_MAX		(10)
#define DISABLE_EDID_WHEN_HPD_LOW	(iTE_TRUE)//(iTE_FALSE)
#define EDID_DEFAULT		(EDID_DEFAULT_FHD)
#if (USING_IT6661 == iTE_FALSE)
#define EDID_COPY_PORT		(HDMI_TX_A)
#else
#define EDID_COPY_PORT		(HDMI_TX_B)
#endif

#define EnMultiSeg			(iTE_FALSE)
#define EnIntEDID			(iTE_TRUE)
//#define EnIntEDID			(iTE_FALSE)//@LV 20160319

//#define EdidUpdateTxChange	(iTE_FALSE)		// iTE_FALSE for only change EDID when Tx inserted, iTE_TRUE for change EDID when either Tx plug in or out
#define EdidUpdateTxChange_Compose	(iTE_FALSE)
#define EdidUpdateTxChange_Copy		(iTE_TRUE)

#if	(USING_CEC == iTE_TRUE)
#define DefaultEdid_PA_AB		(0x00)
#define DefaultEdid_PA_CD		(0x00)
#else
#define DefaultEdid_PA_AB		(0x10)
#define DefaultEdid_PA_CD		(0x00)
#endif

#define KEY_EDID_MODE_UHD			(0x00)
#define KEY_EDID_MODE_COPY		(0x20)
#define KEY_EDID_MODE_COMPOSE	(0x80)
#define KEY_EDID_MODE_FHD			(0xA0)
//******************* Defination for HDMI TX **************************//
#define TMDS_4k2k			(165000)
#define DRV_TERMON			(iTE_FALSE)	 // TRUE : for 4K2K

#if (0)//(USING_IT6661 == iTE_TRUE)
#define EYE_IMPRO_4k2k		(0x07)
#define DRV_RTERM_4k2k		(0x1F)		 // "10111" for 4K2K
#define P3Swing_4k2k		(0x0A)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P2Swing_4k2k		(0x0A)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P1Swing_4k2k		(0x0A)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P0Swing_4k2k		(0x0A)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#else
#define EYE_IMPRO_4k2k		(0x07)
#define DRV_RTERM_4k2k		(0x1F)		 // "10111" for 4K2K
#define P3Swing_4k2k		(0x0F)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P2Swing_4k2k		(0x0F)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P1Swing_4k2k		(0x0F)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P0Swing_4k2k		(0x0F)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#endif
#define EYE_IMPRO			(0x00)
#define DRV_RTERM			(0x17)		 // "10111" for 4K2K
#define P3Swing				(0x01)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P2Swing				(0x01)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P1Swing				(0x01)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K
#define P0Swing				(0x01)		// "0001" : (PCLK < 148.5M) , "1100" : for 4K2K

#define P3ForceRxOn			(iTE_FALSE)
#define P2ForceRxOn			(iTE_FALSE)
#define P1ForceRxOn			(iTE_FALSE)
#define P0ForceRxOn			(iTE_FALSE)
#define EnP3VCLKInv			(iTE_FALSE)
#define EnP2VCLKInv			(iTE_FALSE)
#define EnP1VCLKInv			(iTE_FALSE)
#define EnP0VCLKInv			(iTE_FALSE)
#define SystemIntMask		(0x18)		// default disable Tx FIFO Error, VSync event

#define Port0IntMask_L		(0x0C)
#define Port0IntMask_H		(0x00)
#define Port1IntMask_L		(0x0C)
#define Port1IntMask_H		(0x00)
#define Port2IntMask_L		(0x0C)
#define Port2IntMask_H		(0x00)
#define Port3IntMask_L		(0x0C)
#define Port3IntMask_H		(0x00)

#define XPStableTime			(0)
#define EnXPLockChk			(iTE_FALSE)
#define EnFFAutoRst			(iTE_TRUE)
#define EnPLLBufRst			(iTE_TRUE)
#define Port0En				(iTE_FALSE)
#define Port1En				(iTE_FALSE)
#define Port2En				(iTE_FALSE)
#define Port3En				(iTE_FALSE)
#define Port0CSCEn			(iTE_FALSE)
#define Port1CSCEn			(iTE_FALSE)
#define Port2CSCEn			(iTE_FALSE)
#define Port3CSCEn			(iTE_FALSE)
#define EnPort0HDMI			(iTE_TRUE)
#define EnPort1HDMI			(iTE_TRUE)
#define EnPort2HDMI			(iTE_TRUE)
#define EnPort3HDMI			(iTE_TRUE)
#define ForceVOut			(iTE_FALSE)
#define TxRINGOSC			(0)			// 0: Normal(15MHz), 1: Fast(20MHz), 2: Slow(10MHz)

#define P0EncDis				(iTE_FALSE)
#define P1EncDis				(iTE_FALSE)
#define P2EncDis				(iTE_FALSE)
#define P3EncDis				(iTE_FALSE)

#define RegKeepOutOpt		(iTE_FALSE)	// "TRUE" (Keepout region more than HDMI Spec.)
                          							 // "FALSE" (default)= Keepout region is equal to HDMI Spec (suggested).
#define SkipNullPkt			(iTE_TRUE)//(iTE_FALSE)	// Skip Null Packet (default = FALSE) (B0)

#define EnRiCombRead		(iTE_TRUE)
#define EnR0CombRead		(iTE_FALSE)
#define EnSyncDetChk		(iTE_TRUE)	// default iTE_FALSE
#define TimeBaseSel			(0)

#define EnHDCPAutoMute		(iTE_TRUE)//(iTE_TRUE)
#define EnSyncDet2FailInt	(iTE_FALSE)//(iTE_TRUE)
#define EnRiChk2DoneInt		(iTE_FALSE)
//#define EnAutoReAuth		(iTE_FALSE)

#define EnRiPjInt				(iTE_TRUE)
#define GenAn				(iTE_TRUE)

#define EnAutoReAuth		(iTE_FALSE)
#define HDCPFireMax			(50)
#define EnEdidRead			(iTE_TRUE)
#define EnHDCP				(iTE_TRUE)
#define EnHDCP1p1			(iTE_FALSE)

#define Tx_HDCP_REPEATER_CNT	(50)
//#define HPD_CHK_CNT		(10)		// 1s
#define TX_HPD_CHK_CNT		(7)		// 1s
#define TIME_BASE			(0x0A)	// 100ms

#define TX_HPD_MASK		(0xFF)
//******************* Defination for MHL **************************//
//
#define EnAutoHPD 					(iTE_FALSE)//(iTE_TRUE)
#define EnAutoPathEn	 				(iTE_FALSE)//(iTE_TRUE)
#define EnAutoCapRdy	 			(iTE_FALSE)//(iTE_TRUE)

#define AutoHPDDly					(2)
#define AutoPathEnDly 				(1)
#define AutoCapRdyDly				(0)

// CBUS 1K PD
#define EnCBus1KTog					(iTE_TRUE)	// V1.16_1: 20131203
#define CBUS1KPD					(0)			// 0: default, 1~3: 5, 10, 15%, 4~7: -5, -10, -15, -20%	// V1.16_1: 20131203

#define EnCBusReDisv 				(iTE_FALSE)			// FALSE for ATC
#define CBUSRTYMAX      				(3)				// default=20, 3 for ATC
#define WAITABORTNUM  			  	(3000)//(300)			//change from 3000 to 5000 for ATC 6.3.6.5
#define CBUSINITIME   			  	(5)

#define WAKE_FAIL_SLEEP_TIME		(1)	//*100ms
#define ECC_ERROR_SLEEP_TIME		(1)	//*100ms
#define CLK_RDY_SLEEP_TIME			(1)	//*100ms
#define CLK_STAB_SLEEP_TIME		(1)	//*100ms
#define CLK_VAILD_SLEEP_TIME		(1)	//*100ms

#define EnWakeFailSleep				(iTE_TRUE)
#define EnEccErrSleep				(iTE_TRUE)
#define EnClkVaildSleep				(iTE_TRUE)
#define EnClkRdySleep				(iTE_TRUE)
#define EnClkStabSleep				(iTE_TRUE)
//******************* Defination for MHL **************************//
// MSC Firmware Mode

// CBUS INput Option
#define EnCBusDeGlitch			(iTE_TRUE)

// Debug Mode
#define EnCBusDbgMode			(iTE_FALSE)
#define MSCCBusDbgCtrl			(iTE_FALSE)
#define DDCCBusDbgCtrl			(iTE_TRUE)
#if (EnCBusDbgMode && (MSCCBusDbgCtrl || DDCCBusDbgCtrl))
#define MaskTxPktDoneInt 			(iTE_FALSE)
#define MaskRxPktDoneInt			(iTE_FALSE)
#else
#define  MaskTxPktDoneInt			(iTE_TRUE)
#define  MaskRxPktDoneInt			(iTE_TRUE)
#endif

// DDC Option
#define MaskDDCDoneInt				(iTE_FALSE)
// MSC Option
#define MaskMSCDoneInt				(iTE_FALSE) // MSC Rpd Done Mask and MSC Req Done Mask


#define EnPPHDCPOpt 			(iTE_TRUE)
#define EnPPHDCPOpt2 			(iTE_TRUE)//(iTE_FALSE)//(iTE_TRUE)
#define EnPPGBSwap 				(iTE_TRUE)
#define CDDGSel					(3)


#define RegBurstWrTOSel			(2) //0: 320ms, 1: 340ms, 2: 360ms   (ATC)
#define Reg100msTOAdj			(2) //00: 100ms, 01: 99ms, 10: 101ms (ATC)

#define EnDDCSendAbort			(iTE_TRUE)  // Send ABORT after segment write with EOF
#define EnMSCBurstWr			(iTE_TRUE)
#define MSCBurstWrID			(iTE_FALSE)  // TRUE: from MHL5E/MHL5F
#define MSCBurstWrOpt			(iTE_FALSE)  // TRUE: Not write Adopter ID into ScratchPad
#define EnPktFIFOBurst			(iTE_TRUE)
#define EnMSCHwRty				(iTE_FALSE)
#define MSCRxUCP2Nack			(iTE_TRUE)
#define EnMsg2nd				(iTE_TRUE)
// Link Layer Option
#define DisCBusBurst				(iTE_FALSE)   // TRUE: for CBUS single packet mode
#define AckHigh					(0xB)
#define AckLow					(1)
//Wakeup Fail
#define RegWKFailTSel			(01)
#define RegWKFailFlt				(iTE_FALSE)
#define RegWKFailTog			(iTE_TRUE)
//Discovery
#define CBUSFloatAdj				(iTE_FALSE)
#define CBUSEnState5				(iTE_FALSE)
#define CBUSForceStandby		(iTE_FALSE)

//******************* Defination for MHL RCP & RAP **************************//
#ifdef _MHL_MSC_INIT_TAB_
iTE_u8 _CODE SuppRCPCode[0x80]= {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,	// 0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// 1
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,	// 2
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,	// 3
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,	// 4
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// 5
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,	// 6
	0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 	// 7
};

iTE_u8 _CODE SuppRAPCode[0x20] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// 0
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	// 1
};
#endif

#ifdef _MSC_HANDLER_TAB_

iTE_u8 _CODE uc3DDtd[]={0x00};

#if (USING_DEFAULT_3D_VIC == iTE_TRUE)
iTE_u8 _CODE uc3DVic[]={0x00};
#endif

#endif
//******************* Defination for MHL DCAP **************************//
//MHL DCAP
#define MHLVer			 			(0x21)    // V1.2=0x12, V2.0=0x20

#define DEV_TYPE					(0x01)	//Sink:0x01, Source:0x02, Dongle:0x03
#define POW							(0x01)
#define PLIM							(0x01)	//500mA:0x00, 900mA:0x01, 1500mA:0x10
#define DEV_CAT						((PLIM<<5)|(POW<<4)|(DEV_TYPE))

#define ADOPTER_ID_H				(0x00)	//(0x02)
#define ADOPTER_ID_L				(0x00)	//(0x45)

#define SUPP_VGA					(iTE_TRUE)
#define SUPP_ISLANDS				(iTE_TRUE)
#define SUPP_PPIXEL					(iTE_TRUE)
#define SUPP_YCBCR422				(iTE_TRUE)
#define SUPP_YCBCR444				(iTE_TRUE)
#define SUPP_RGB444					(iTE_TRUE)
#define SUPP_VID_LINK				((SUPP_VGA<<5)|(SUPP_ISLANDS<<4)|(SUPP_PPIXEL<<3)|(SUPP_YCBCR422<<2)|(SUPP_YCBCR444<<1)|(SUPP_RGB444))
//#define SUPP_VID_LINK				(0x37)
//#define EnPackPix			 		(iTE_TRUE)


#define AUD_8CH						(iTE_TRUE)
#define AUD_2CH						(iTE_TRUE)
#define AUD_CH						((AUD_8CH<<1)|(AUD_2CH))
//#define AUD_CH 						(0x03)    // 2ch: 0x01, 8ch: 0x03

#define SUPP_VT						(iTE_FALSE)
#define VT_GAME						(iTE_FALSE)
#define VT_CINEMA					(iTE_FALSE)
#define VT_PHOTO					(iTE_FALSE)
#define VT_GRAPHICS					(iTE_FALSE)
#define VIDEO_TYPE					((SUPP_VT<<7)|(VT_GAME<<3)|(VT_CINEMA<<2)|(VT_PHOTO<<1)|(VT_GRAPHICS))
//#define VIDEO_TYPE					(0x00)

#define LD_GUI						(iTE_FALSE)
#define LD_SPEAKE					(iTE_TRUE)
#define LD_RECORD					(iTE_FALSE)
#define LD_TUNER					(iTE_FALSE)
#define LD_MEDIA					(iTE_FALSE)
#define LD_AUDIO					(iTE_FALSE)
#define LD_VIDEO					(iTE_FALSE)
#define LD_DISPLAY					(iTE_TRUE)
#define LOG_DEV_MAP				((LD_GUI<<7)|(LD_SPEAKE<<6)|(LD_RECORD<<5)|(LD_TUNER<<4)|(LD_MEDIA<<3)||(LD_AUDIO<<2)|(LD_VIDEO<<1)|(LD_DISPLAY))
//#define LOG_DEV_MAP				(0x41)

#define BANDWIDTH					(0x0F)

#define UCP_RECV_SUPP				(iTE_TRUE)
#define UCP_SEND_SUPP				(iTE_TRUE)
#define SP_SUPPORT					(iTE_TRUE)
#define RAP_SUPPORT					(iTE_TRUE)
#define RCP_SUPPORT					(iTE_TRUE)
#define FEATURE_FLAG				((UCP_RECV_SUPP<<4)|(UCP_SEND_SUPP<<3)|(SP_SUPPORT<<2)|(RAP_SUPPORT<<1)|(RCP_SUPPORT))
//#define FEATURE_FLAG				(0x1F)

#define DEVICE_ID_H					(0x00)	//(0x63)
#define DEVICE_ID_L					(0x00)	//(0x32)
#define SCRATCHPAD_SIZE			(0x10)
#define INT_STA_SIZE				(0x33)
//******************* Defination for LED **************************//
#if (PCB_VER == 11)
#define LED_0			(GPI2)
#define LED_1			(GPI3)
#define LED_2			(GPI4)
#define LED_3			(GPI5)
#define LED_4			(GPG3)
#define LED_5			(GPG4)
#define LED_6			(GPG5)
#define LED_7			(GPG6)
#define LED_8			(GPI0)
#define LED_9			(GPI1)
#define LED_A			(GP00)
#define LED_B			(GP00)
#define LED_C			(GP00)
#define LED_D			(GP00)
#define LED_E			(GP00)
#define LED_F			(GP00)
#else
#define LED_0			(GPF6)
#define LED_1			(GPF7)
#define LED_2			(GPA0)
#define LED_3			(GPA1)
#define LED_4			(GPA6)
#define LED_5			(GPG3)
#define LED_6			(GPG4)
#define LED_7			(GPG6)
#define LED_8			(GPB3)
#define LED_9			(GPB4)
#define LED_A			(GP00)
#define LED_B			(GP00)
#define LED_C			(GP00)
#define LED_D			(GP00)
#define LED_E			(GP00)
#define LED_F			(GP00)
#endif
//******************* Defination for Device **************************//
#define IT6662_A						(0x02)
#define IT6662_B						(0x21)	//(0x22) for old version
#define IT6662_C 						(0x01)
#define IT6662_ROOT					(IT6662_C)

#define IT6662_DEFAULT_CEC			(HDMI_TX_A)
#endif
