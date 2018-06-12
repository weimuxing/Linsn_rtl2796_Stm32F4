///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Rx.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Rx.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _MHL_RX_H_
#define _MHL_RX_H_

#include "it6662/include/IT6662_IO.h"
#include "it6662/include/IT6661.h"

#define MSC_3D_VIC		(0x0010)
#define MSC_3D_DTD		(0x0011)

typedef enum _MHL_INT{
	MHL_INT_3D_REQ = 0x10,
	MHL_INT_GNT_WRT = 0x08,
	MHL_INT_REQ_WRT = 0x04,
	MHL_INT_DSCR_CHG = 0x02,
	MHL_INT_EDID_CHG = 0x02,
}MHL_INT;
typedef enum _MHL_STS{
	MHL_STS_DCAP_RDY = 0x01,
	MHL_STS_MUTED = 0x10,
	MHL_STS_PATH_EN = 0x08,
	MHL_STS_CLK_24 = 0x03,
	MHL_STS_CLK_PP = 0x02
}MHL_STS;
typedef enum _MSC_MODE{
	MSC_FW_MODE,
	MSC_HW_MODE,
	MSC_WRITE_BURST_MODE
}MSC_MODE;

typedef struct _MSC_CMD{
	iTE_u8	ucOffset;
	iTE_u8	ucValue;
	iTE_u8	ucCnt;
}MSC_CMD, *P_MSC_CMD;

typedef enum _MSC_STATUS {
	MSC_SUCCESS = 0,
	MSC_FAIL,
	MSC_ABORT,
	MSC_RCVABORT,
	MSC_RCVNACK,
	MSC_ARBLOSE,
	MSC_FWTXFAIL,
	MSC_FWRXPKT
} MSC_STATUS ;


typedef struct _MSC_WB_INFO{
	iTE_u8	*pucTxScrpad;
	iTE_u8	ucCnt;
	iTE_u8	ucOffset;
}MSC_WB_INFO;

typedef enum _MSC_MSG_CMD{
	MSC_MSG_MSGE		= 0x02,
	MSC_MSG_RCP		= 0x10,
	MSC_MSG_RCPK		= 0x11,
	MSC_MSG_RCPE		= 0x12,
	MSC_MSG_RAP		= 0x20,
	MSC_MSG_RAPK		= 0x21,
	MSC_MSG_UCP		= 0x30,
	MSC_MSG_UCPK		= 0x31,
	MSC_MSG_UCPE		= 0x32
}MSC_MSG_CMD;

typedef enum _MSC_STATUS_CODE{
	MSC_NO_ERROR = 0x00,
	MSC_INVALID_CODE = 0x01
}MSC_STATUS_CODE;
typedef enum _RAP_ACTION_CODE{
	RAP_POLL = 0x00,
	RAP_CONTENT_ON = 0x10,
	RAP_CONTENT_OFF = 0x11
}RAP_ACTION_CODE;

typedef enum _RAP_ERROR{
	RAP_NO_ERROR = 0x00,
	RAP_UNRECOGN_ACTION_CODE = 0x01,
	RAP_UNSUPPORT_ACTION_CODE = 0x02,
	RAP_RESPONDER_BUSY = 0x03
}RAP_ERROR;

typedef enum _RCP_ERROR{
	RCP_NO_ERROR = 0x00,
	RCP_INEFFECTIVE_KEY = 0x01,
	RCP_RESPONDER_BUSY = 0x02
}RCP_ERROR;

typedef enum _UCP_ERROR{
	UCP_NO_ERROR = 0x00,
	UCP_INEFFECTIVE_KEY = 0x01
}UCP_ERROR;

typedef enum _MHL3D_STATE{
	MHL3D_REQ_START,
	MHL3D_REQ_WRT,
	MHL3D_GNT_WRT,
	MHL3D_WRT_BURST,
	MHL3D_REQ_DONE
}MHL3D_STATE;


typedef enum _MSC_PKT_ID{
	MSC_PKT_WRITE_STAT = 0,
	MSC_PKT_SET_INT,
	MSC_PKT_READ_DEVCAP,
	MSC_PKT_GET_STATE,
	MSC_PKT_GET_VENDOR_ID,
	MSC_PKT_SET_HPD,
	MSC_PKT_CLR_HPD,
	MSC_PKT_MSC_MSG,
	MSC_PKT_GET_DDC_ERRORCODE,
	MSC_PKT_GET_MSC_ERRORCODE,
	MSC_PKT_WRITE_BURST,
	MSC_PKT_GET_SC1_ERRORCODE,
	MSC_PKT_GET_SC3_ERRORCODE,
	MSC_PKT_EOF,
	MSC_PKT_ACK,
	MSC_PKT_NACK,
	MSC_PKT_ABORT,
	MSC_PKT_ID_END
}MSC_PKT_ID;

#ifdef _MHL_RX_INIT_TAB_
//static iTE_u8 _CODE Msc_CtlPktTab[MSC_PKT_ID_END]={0x33, 0x34, 0x35, 0x60, 0x60, 0x61, 0x62, 063, 0x64, 0x65, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x32};
//	iTE_u8 _CODE uc3DDtd[]={0x00, 0x11, 0xEE, 0x00, 0x01, 0x00};

static iTE_u8 _CODE MHL_RX_INIT_TAB[][3] = {
	{0x00, 0x8F, (EnCBusDbgMode<<7) | (MSCCBusDbgCtrl<<2) | DDCCBusDbgCtrl},
	{0x01, 0x8C, (EnCBusDeGlitch<<7) | (Reg100msTOAdj<<2)},
	{0x08, 0xFF, (MaskRxPktDoneInt<<2)+MaskTxPktDoneInt},
	{0x09, 0xFF, (MaskDDCDoneInt<<6)+(MaskDDCDoneInt<<4)+(MaskMSCDoneInt<<2)+MaskMSCDoneInt},
//
	{0x0A, 0xFF, 0x00},
	{0x0C, 0x88, (EnPPHDCPOpt<<7) | ( EnPPHDCPOpt2<<3)},
	{0x0F, 0x20, EnPPGBSwap<<5},
	{0x28, 0x0F, (CBUSFloatAdj<<3) | (CBUSEnState5<<2) | (CBUSForceStandby<<1)},
//	{0x29, 0x0C, 0x04},
	{0x29, 0x03, CDDGSel},
	{0x2A, 0x80, 0x80},
	{0x2B, 0x3C, (RegWKFailTSel<<4) | (RegWKFailFlt<<3) | (RegWKFailTog<<2)},
	{0x36, 0xFC, (AckHigh<<4) | (AckLow<<2)},
	{0x38, 0x20, EnDDCSendAbort<<5},
	{0x39, 0x80, 0x80},	//I2C Deglitch for MHL
	{0x5C, 0xFC, (EnPktFIFOBurst<<7) | (MSCBurstWrID<<6) | (MSCBurstWrOpt<<5) | (EnMSCBurstWr<<4) | (EnMSCHwRty<<3) | (MSCRxUCP2Nack<<2)},
	{0x66, 0x83, (EnMsg2nd << 7) | RegBurstWrTOSel},
	{0x04, 0xff, 0xff},
	{0x05, 0xff, 0xff},
	{0x06, 0xff, 0xff},
	{0x07, 0xff, 0xff},
#if(DisCBusBurst == iTE_TRUE)
	{0x32, 0xFF, 0x00},
#else
	{0x32, 0xFF, 0x0C},
#endif

	{0x81, 0xff, MHLVer},					//MHL Version
	{0x82, 0xff, DEV_CAT},				//DEV_CAT	// for 4.3.24.1 ATC
	{0x83, 0xff, ADOPTER_ID_H},			//ADOPTER_ID_H
	{0x84, 0xff, ADOPTER_ID_L},			//ADOPTER_ID_L
	{0x85, 0xff, SUPP_VID_LINK},			//VID_LINK_MODE
	{0x86, 0xff, AUD_CH},					//AUD_LINK_MODE
	{0x87, 0xff, VIDEO_TYPE},				//VIDEO_TYPE
	{0x88, 0xff, LOG_DEV_MAP},			//LOG_DEV_MAP
	{0x89, 0xff, BANDWIDTH},				//BANDWIDTH
	{0x8a, 0xff, FEATURE_FLAG},			//FEATURE_FLAG
	{0x8b, 0xff, DEVICE_ID_H},			// DEVICE_ID_H
	{0x8c, 0xff, DEVICE_ID_L},			// DEVICE_ID_L
	{0x8d, 0xff, SCRATCHPAD_SIZE},		//SCRATCHPAD_SIZE
	{0x8e, 0xff, INT_STA_SIZE},			// INT_STA_SIZE

	{0x2A, 0x06, 0x00},

};
#define MHL_RX_INIT_TAB_SIZE	sizeof(MHL_RX_INIT_TAB)/3


iTE_u8 _CODE u8CBus1K[8] = {0x14, 0x10, 0x1C, 0x18, 0x0C, 0x08, 0x04, 0x00};
// 0 -> -10 -> +10 -> -20 -> 0
// +5 -> -5 -> +15 -> -15 -> +5
#endif


void MhlRx_Init(void);
void MhlRx_Enable(void);
void cal_oclk(void);
void Mhl_CBusIrq(void);
MSC_STATUS MHL_MscReqFailCheck(iTE_u8 *ucMHL18);
void Msc_Read_DevCap(void);
#if iTE_MhlRx_Msg_Print
void Msc_ParseDevCap(iTE_u8 ucOffset, iTE_u8 ucData);
void dbgcbustx( void );
void dbgcbusrx( void );
void mhl_DbgCBus(iTE_u8 ucTxRxReg);
void parse_ctrlpkt( iTE_u8* ctrlpkt );
#endif
void mhl_CBusReDiscovery(void);
MSC_STATUS Msc_MscMsgFire(MSC_CMD *MscCmd);
MSC_STATUS Msc_MscFire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd);
iTE_u1 Msc_WriteBurstDataFill(iTE_u8 ucOffset, iTE_u8 ucByteNo, iTE_u8 *pucData);
MSC_STATUS Msc_Fire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd);

void Mhl_MscIrq(void);
MSC_STATUS Mhl_MscFire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd);
void Msc_3DProcess(MHL3D_STATE *e3DReqState);
MHL3D_STATE MSC_3DInforSend(iTE_u1 b3dDtd);
void Msc_MsgHandler(void);
#if iTE_MhlRx_Msg_Print
void	Msc_MsgeHandler(MSC_STATUS_CODE eMsgStatus);
#endif

#define MhlRx_Wb(u8Offset, u8Count, pu8Data)		iTE_I2C_WriteBurst(MHL_RX_ADR, u8Offset, u8Count, pu8Data)
#define MhlRx_Rb(u8Offset, u8Count, pu8Data)		iTE_I2C_ReadBurst(MHL_RX_ADR, u8Offset, u8Count, pu8Data)
#define MhlRx_Set(u8Offset, u8InvMask, u8Data)	iTE_I2C_SetByte(MHL_RX_ADR, u8Offset, u8InvMask, u8Data)
#define MhlRx_W(u8Offset, u8Data)				iTE_I2C_WriteByte(MHL_RX_ADR, u8Offset, u8Data)
#define MhlRx_R(u8Offset)							iTE_I2C_ReadByte(MHL_RX_ADR, u8Offset)
#endif
