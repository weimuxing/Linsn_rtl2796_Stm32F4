/*
#ifndef _IT6661_HDMI_TX_H
#define _IT6661_HDMI_TX_H

#define HDMI_TX_INIT_TABLE

#endif
*/

///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Hdmi_Tx.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Hdmi_Tx.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _IT6661_HDMI_TX_H
#define _IT6661_HDMI_TX_H

#include "it6662/include/IT6661.h"
#include "it6662/include/ITE_Edid.h"
#include "it6662/include/iTE_typedef.h"
#include "it6662/include/IT6661_config.h"


#define HDMI_TX_INIT_TABLE

#define HdmiTx_Bank(x,device)		HdmiTx_Set(0x0F, 0x03, x,device)
#define iTE_uDef	(iTE_u16)

#ifdef HDMI_TX_INIT_TABLE
static iTE_u8 _CODE HDMI_TX_INIT_TAB[][3] = {
{0x0F, 0x03, 0x00},

#if 0//(DefEnHDCP == iTE_TRUE)
{0x55, 0x81, 0x81},		// Fire Port0 Tx HDCP
{0x00, 0x00, 0x01},
{0x04, 0x10, 0x10},
{0x04, 0x10, 0x00},
{0x55, 0x81, 0x00},
#endif
{0x05, 0x0FF, (P3ForceRxOn<<7) | (P2ForceRxOn<<6) | (P1ForceRxOn<<5) | (P0ForceRxOn<<4) | 0x02},	//RCLK reset
{0x05, 0x0FF, (P3ForceRxOn<<7) | (P2ForceRxOn<<6) | (P1ForceRxOn<<5) | (P0ForceRxOn<<4) | 0x03},	//RCLK reset
{0x05, 0x0FF, (P3ForceRxOn<<7) | (P2ForceRxOn<<6) | (P1ForceRxOn<<5) | (P0ForceRxOn<<4) | 0x02},	//RCLK reset
{0x04, 0xFF, 0x0F},
{0x05, 0x0FF, (P3ForceRxOn<<7) | (P2ForceRxOn<<6) | (P1ForceRxOn<<5) | (P0ForceRxOn<<4) | 0x0C},	//RCLK reset
{0xFF, 0xFF, 0xC3},
{0xFF, 0xFF, 0xA5},
{0x06, 0x0F, (EnP3VCLKInv<<3) | (EnP2VCLKInv<<2) | (EnP1VCLKInv<<1) | (EnP0VCLKInv<<0)},
{0x07, 0x0F, (RCLKPDSel<<2) | (PwrDnRCLK<<1) | (HDMITxRCLKFreqSel)},
{0x0A, 0xFF, SystemIntMask},
{0x44, 0xFF, Port0IntMask_L},
{0x45, 0xFF, Port0IntMask_H},
{0x84, 0xFF, Port1IntMask_L},
{0x85, 0xFF, Port1IntMask_H},

{0x0F, 0x03, 0x00},
{0x0E, 0xFF, 0x02},	//[3:0]: PCI2C Hold Time
{0x18, 0xF8, (XPStableTime<<6) | (EnXPLockChk<<5) | (EnPLLBufRst<<4) | (EnFFAutoRst<<3)},
{0x19, 0xFF, (Port3CSCEn<<7) | (Port2CSCEn<<6) | (Port1CSCEn<<5) | (Port0CSCEn<<4) | (Port3En<<3) | (Port2En<<2) | (Port1En<<1) | (Port0En)},
{0x1A, 0xFF, (SkipNullPkt<<6) | 0x10},	//[4]: Pkt FIFO Reset
{0x20, 0x01, ForceVOut},	//[0]: force video out
{0x22, 0x0F, (EnPort3HDMI<<3) | (EnPort2HDMI<<2) | (EnPort1HDMI<<1) | (EnPort0HDMI)},
{0x23, 0xFF, 0x00},	//[3:0]: regavmute
{0x24, 0xFF, 0x40},//0x42},	//[6:4]: RegColorDepth
{0x26, 0xFF, 0x03},	//[1]: RegPktGen
{0x25, 0xFF, 0x08},
{0x2B, 0xFF, 0xA8},

{0x2C, 0xFF, 0x10},
{0x2D, 0xFF, 0x03},
{0x33, 0xFF, 0x00},
{0x0F, 0x03, 0x01},

{0x44, 0xFF, Port2IntMask_L},
{0x45, 0xFF, Port2IntMask_H},
{0x84, 0xFF, Port3IntMask_L},
{0x85, 0xFF, Port3IntMask_H},
{0xC0, 0xE0, OutColorMode<<5},
{0xC1, 0xFF, 0x00},
{0xC3, 0x01, 0x01},		// for CSC enable, read Rx AVI/VSI packet from 1C0 ~ 1C7
{0xC6, 0xFF, 0x00},		// 3D Info PB6
{0x0F, 0x03, 0x00},
{0x31, 0xFF, 0xA1},
{0x05, 0xFF, (P3ForceRxOn<<7) | ( P2ForceRxOn<<6) | (P1ForceRxOn<<5) | (P0ForceRxOn<<4) | 0x0C},	//unreset
{0x00, 0x00, 0x01},
{0x1A, 0x10, 0x10},
{0x00, 0x00, 0x01},
{0xC0, 0x03, TxRINGOSC},
//{0xC1, 0x0F, 0x0F},
{0xC1, 0xFF, 0xFF},		// dong ask disable Tx driver for power saving
#if (USING_IT6661)
{0xC2, 0x09, 0x09},
#endif
{0xC3, 0x9F, (DRV_TERMON << 7) | DRV_RTERM},
{0xC4, 0xFF, (P1Swing<<4) | P0Swing},
{0xC5, 0xFF, (P3Swing<<4) | P2Swing},
//{0xF0, 0xFF, CEC_ADR | 0x01},

{0xFF, 0xFF, 0xC3},
{0xFF, 0xFF, 0xA5},
{0x33, 0xF0, (P3EncDis << 7) | (P2EncDis << 6) | (P1EncDis << 5) | (P0EncDis << 4)},
{0x30, 0x0C, (EnRiCombRead << 3) | (EnR0CombRead << 2)},
{0xFF, 0xFF, 0xFF},
{0x32, 0x70, (RegKeepOutOpt<<6) | (EnSyncDetChk << 5) | (EnHDCP1p1 << 4)},
#if (EnSyncDetChk)
{0x30, 0xC0, TimeBaseSel << 6},
#else
{0x30, 0xC0, 0x00},
#endif
{0x32, 0x0F, (EnHDCPAutoMute << 3) | (EnSyncDet2FailInt << 2) | (EnRiChk2DoneInt << 1) | (EnAutoReAuth)},

};
#define HDMI_TX_INIT_TAB_SIZE	sizeof(HDMI_TX_INIT_TAB)/3
/*iTE_u8  InvalidBKsv[5][5]= {
#if (USING_HDCP_CTS == iTE_TRUE)
		{0x23, 0xDE, 0x5C, 0x43, 0x93},
#endif
};				
#define HDMI_TX_INVALID_BKSV_SIZE	sizeof(InvalidBKsv)/5 */
#define HDMI_TX_INVALID_BKSV_SIZE	25/5
#endif

#define HASH_0		(0x67452301)
#define HASH_1		(0xEFCDAB89)
#define HASH_2		(0x98BADCFE)
#define HASH_3		(0x10325476)
#define HASH_4		(0xC3D2E1F0)

void HdmiTx_EnCec(void);
void HdmiTx_Init(IT6661_TypeDef IT6661_SEL);

void HdmiTx_Select(iTE_u8 u8HdmiTxSel);
void HdmiTx_Irq(void);
iTE_u32 cal_pclk(void);
void HdmiTx_EnTmds(void);
void HdmiTx_PortIRQ(void);
iTE_u8 HdmiTx_Sw_Timer_IRQ(iTE_u8 u8TxSel);
void HdmiTx_PortEnable(iTE_u8 u8RxSen,IT6661_TypeDef IT6661_SEL);
void HdmiTx_PortDisable(IT6661_TypeDef IT6661_SEL);
void HdmiTx_HdcpStatus(void);
void HdmiTx_PreSpenHdcp(iTE_u1 b1stSpen);
iTE_u1 HdmiTx_PostSpenHdcp(void);
void HdmiTx_SaveKvsHdcp(void);
void HdmiTx_AuthHdcp(void);
void HdmiSpenHdcpPort(iTE_u32 *u32Hash);
void HdmiTx_SetOutputMode(EDID_STAT eStatus);
iTE_u8 HdmiTx_CscOutputSet(void);
void HdmiTx_HdcpDisable(void);
iTE_u1 HdmiTx_HdcpRead(iTE_u8 u8HdcpOffset, iTE_u16 u16ByteNum);
iTE_u1 HdmiTx_DdcWait(void);

void HdmiTx_DdcDisable(void);
void HdmiTx_DdcReset(void);
EDID_STAT HdmiTx_FireEdidRead(iTE_u8 u8RdStart, iTE_u8 u8RdCnt, iTE_u8 u8EdidBlk);
void HdmiTx_EdidRead(iTE_u8	u8RdCnt, iTE_pu8 pu8RdEdid);


#define HdmiTx_Wb(u8Offset, u8Count, pu8Data,device)	iTE_I2C_WriteBurst(HDMI_TX_ADR, u8Offset, u8Count, pu8Data,device)
#define HdmiTx_Rb(u8Offset, u8Count, pu8Data,device)	iTE_I2C_ReadBurst(HDMI_TX_ADR, u8Offset, u8Count, pu8Data,device)
#define HdmiTx_Set(u8Offset, u8InvMask, u8Data,device)	iTE_I2C_SetByte(HDMI_TX_ADR, u8Offset, u8InvMask, u8Data,device)
#define HdmiTx_W(u8Offset, u8Data,device)				iTE_I2C_WriteByte(HDMI_TX_ADR, u8Offset, u8Data,device)
#define HdmiTx_R(u8Offset,device)						iTE_I2C_ReadByte(HDMI_TX_ADR, u8Offset,device)  
#endif


