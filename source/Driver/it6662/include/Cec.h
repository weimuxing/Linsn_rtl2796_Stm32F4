///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Cec.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Cec.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _CEC_H_
#define _CEC_H_

#include "it6662/include/IT6662_IO.h"
#include "it6662/include/CEC_typedef.h"

extern iTE_u8	g_u8CecSel;
extern iTE_u8 ucMyselfAddr;

#define CEC_RX_SELF_DIS				(1)
#define CEC_RST						(0)
#define CEC_NACK_EN					(0)
#define CEC_CAL_CNT					(1)
#define CEC_RESEND_MAX				(5)

#define IT6662_RX_LA				(DEVICE_ID_TV)
#define IT6662_TX_LA				(DEVICE_ID_FREEUSE)//(DEVICE_ID_TV)//(DEVICE_ID_FREEUSE)

#define IT6662_RX_TYPE				(CEC_DEV_TYPE_TV)
#define IT6662_TX_TYPE				(CEC_DEV_TYPE_VIDEO_PROCESSOR)//(CEC_DEV_TYPE_TV)//(CEC_DEV_TYPE_VIDEO_PROCESSOR)

#define CEC_DEV_VENDOR_ID_0		(0x00)
#define CEC_DEV_VENDOR_ID_1		(0x00)
#define CEC_DEV_VENDOR_ID_2		(0x00)

#define CEC_RX_QUEUE_SIZE	(0x01 << 6)
#define CEC_RX_QUEUE_MAX	(CEC_RX_QUEUE_SIZE - 1)
#define CEC_TX_QUEUE_SIZE	(0x01 << 6)
#define CEC_TX_QUEUE_MAX	(CEC_TX_QUEUE_SIZE - 1)

typedef struct _CEC_TX_QUEUE{
	iTE_u8	u8Rptr;
	iTE_u8	u8Wptr;
	iTE_u8	pu8Q[CEC_RX_QUEUE_SIZE];
}stCecTxQ;
typedef struct _CEC_RX_QUEUE{
	iTE_u8	u8Rptr;
	iTE_u8	u8Wptr;
	iTE_u8	pu8Q[CEC_RX_QUEUE_SIZE];
}stCecRxQ;

typedef struct _CEC_VARIABLE_{
	iTE_u8	u8MyLogAdr;
	iTE_u8	u8CecLA[4];
	iTE_u8	u8PaL, u8PaH;
	stCecRxQ	stRxQ;
	stCecTxQ		stTxQ;
	iTE_u8	u8RxTmpHeader[3];
	CEC_FRAME	stCecRxCmd, stCecTxCmd;
	iTE_u1	bTxQFull;
	iTE_u1	bTxCecDone;
	iTE_u8	u8TxCecFire;
	iTE_u8	u8TxCecInitDone;
}stCecVar;


void Cec_VarInit(iTE_u8 u8PaH, iTE_u8 u8PaL, iTE_u8 u8MyLogAdr);
void Cec_Init(void);
void Cec_Irq(void);
iTE_u1 Cec_FireStatus(iTE_u1 bTxDone);
iTE_u8 Cec_SwitchLA(iTE_u8 u8MyLA);
void Cec_RxFifoReset(void);

iTE_u1 Cec_RxCmdGet(void);
iTE_u1 Cec_RxCmdPush(iTE_pu8 pu8Header);
iTE_u1 Cec_RxCmdPull(void);
void Cec_TxFire(void);
void Cec_TxPolling(iTE_u8 u8LogicalAdr);
void Cec_TxFeatureAbort(iTE_u8 CecRxCmd, CecAbortReson eAbortReson);
void Cec_TxPollingMsg(iTE_u8 u8TxSel);
void Cec_TxReportPA(void);
void Cec_TxCmdPush(iTE_u8 ucFollower, eCEC_CMD TxCmd, iTE_u8 u8CecSize);
iTE_u1 Cec_TxCmdPull(void);
void Cec_TxSel(iTE_u8 u8TxSel);
void Cec_BlockSel(iTE_u8 u8CecSel);

#define Cec_Wb(u8Offset, u8Count, pu8Data)	iTE_I2C_WriteBurst(CEC_ADR+g_u8CecSel, u8Offset, u8Count, pu8Data)
#define Cec_Rb(u8Offset, u8Count, pu8Data)	iTE_I2C_ReadBurst(CEC_ADR+g_u8CecSel, u8Offset, u8Count, pu8Data)
#define Cec_Set(u8Offset, u8InvMask, u8Data)	iTE_I2C_SetByte(CEC_ADR+g_u8CecSel, u8Offset, u8InvMask, u8Data)
#define Cec_W(u8Offset, u8Data)				iTE_I2C_WriteByte(CEC_ADR+g_u8CecSel, u8Offset, u8Data)
#define Cec_R(u8Offset)						iTE_I2C_ReadByte(CEC_ADR+g_u8CecSel, u8Offset)
#endif
