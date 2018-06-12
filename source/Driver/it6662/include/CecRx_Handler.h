///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CecRx_Handler.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CecRx_Handler.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _CEC_RX_HANDLER_H_
#define _CEC_RX_HANDLER_H_

#include "it6662/include/Cec_Typedef.h"


void CecRx_CdcFeature(void);
void CecRx_PushCmd2CecTx(iTE_u8 ucFollower, eCEC_CMD TxCmd, iTE_u8 u8CecSize);
void CecRx_OtpFeature(void);
void CecRx_RoutingCtlFeature(void);
void CecRx_StandbyFeature(void);
void CecRx_SysInfoFeature(void);
void CecRx_VenSpecCmdFeature(iTE_u8 u8CmdSize);
void CecRx_DeckCtlFeature(void);
void CecRx_RemoteCtlPassthroughFeature(void);
void CecRx_PwrStatusFeature(void);
void CecRx_SysAudioCtlFeature(void);
void CecRx_ARCFeature(void);
void CecRx_HECFeature(void);

#endif
