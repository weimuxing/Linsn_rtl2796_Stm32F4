///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CecTx_Handler.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CecTx_Handler.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _CEC_TX_HANDLER_H_
#define _CEC_TX_HANDLER_H_

#include "it6662/include/Cec_Typedef.h"

void CecTx_CdcFeature(void);
void CecTx_OtpFeature(void);
void CecTx_RoutingCtlFeature(void);
void CecTx_StandbyFeature(void);
void CecTx_SysInfoFeature(void);
void CecTx_VenSpecCmdFeature(iTE_u8 u8CmdSize);
void CecTx_DeckCtlFeature(void);
void CecTx_RemoteCtlPassthroughFeature(void);
void CecTx_PwrStatusFeature(void);
void CecTx_SysAudioCtlFeature(void);
void CecTx_ARCFeature(void);
void CecTx_HECFeature(void);

#endif
