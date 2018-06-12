///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_RcpUcpHandler.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_RcpUcpHandler.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _MHL_RCPUCPHANDLER_H_
#define _MHL_RCPUCPHANDLER_H_

#include "it6662/include/IT6662_IO.h"
#include "it6662/include/Mhl_Rx.h"

iTE_u1 Msc_RapHandler(RAP_ACTION_CODE eRapCmd);
iTE_u1 Msc_RcpHandler(iTE_u8 ucRcpKey);
iTE_u1 Msc_UcpHandler(iTE_u8 ucUcpCmd);
#endif
