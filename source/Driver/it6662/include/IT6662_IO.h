///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_IO.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_IO.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _IT6662_IO_H
#define _IT6662_IO_H



#include "it6662/include/iTE_typedef.h"
#include "it6662/include/IT6662_config.h"

extern iTE_u8 g_u8CurDev;

#if (USING_1to8 == iTE_FALSE)
#define u8I2CAdr (0)
#endif
void IT6662_DeviceSelect(iTE_u8 u8Device);
iTE_u16 iTE_BusyWait_ms(iTE_u16 usMsCnt);
void iTE_Sleep_ms(iTE_u16 usMsCnt);
void iTE_I2C_ReadBurst(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);
void iTE_I2C_WriteBurst(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);
void iTE_I2C_WriteByte(iTE_u8 u8Adr, iTE_u8 u8Offset, iTE_u8 u8Data);
iTE_u8 iTE_I2C_ReadByte(iTE_u8 u8Adr, iTE_u8 u8Offset);
void iTE_I2C_SetByte(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8InvMask, iTE_u8 u8Data );
void iTE_ExtIntEn(void);
void iTE_GPO_Set(iTE_u16 u16LedSet);
#endif
