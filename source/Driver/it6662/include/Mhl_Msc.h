///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Msc.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Msc.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _MHL_MSC_H_
#define _MHL_MSC_H_

#include "it6662/include/IT6662_IO.h"
#include "it6662/include/Mhl_Rx.h"

typedef struct _PARSE3D_STR{
	iTE_u8	uc3DEdidStart;
	iTE_u8	uc3DBlock;
	iTE_u8	*uc3DInfor;
	iTE_u8	ucVicStart;
	iTE_u8	ucVicCnt;
	iTE_u8	uc3DTempCnt;
	iTE_u8	ucDtdCnt;
}PARSE3D_STR;

MSC_STATUS MHL_MscReqFailCheck(iTE_u8 *ucMHL18);
void Msc_Read_DevCap(void);
#if iTE_MhlRx_Msg_Print
void Msc_ParseDevCap(iTE_u8 ucOffset, iTE_u8 ucData);
#endif
void mhl_CBusReDiscovery(void);
MSC_STATUS Msc_MscMsgFire(MSC_CMD *MscCmd);
MSC_STATUS Msc_MscFire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd);
iTE_u1 Msc_WriteBurstDataFill(iTE_u8 ucOffset, iTE_u8 ucByteNo, iTE_u8 *pucData);
MSC_STATUS Msc_Fire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd);
void Msc_MsgHandler(void);
#if iTE_MhlRx_Msg_Print
void	Msc_MsgeHandler(MSC_STATUS_CODE eMsgStatus);
void Msc_Rapk_Handler(RAP_ERROR eRapError);
void Msc_RcpkHandler(iTE_u8 ucRcpKey);
void Msc_UcpkHandler(iTE_u8 ucUcpkCode);
void Msc_RcpeHandler(RCP_ERROR eRcpError);
void Msc_UcpeHandler(UCP_ERROR eUcpeStatus);
#endif
void Msc_RcpKeyParse( iTE_u8 ucRcpKey );
#endif
