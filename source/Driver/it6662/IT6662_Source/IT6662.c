///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#include "it6662/include/IT6662.h"
#include "it6662/include/ITE_Edid.h"
#include "it6662/include/Cec.h"

extern iTE_u8	g_u8EdidMode;
extern EdidInfo*	g_stEdidInfo;
extern iTE_u8 g_u8TxSel, g_u8TxOffset, g_u8TxBank, g_u8TxShift;
extern IT6662_Var	*stIt6662;

//****************************************************************************

//****************************************************************************
iTE_u1 IT6662_Detect(void)
{
	iTE_u8	ucVenID[2], ucDevID[2], ucRevID;

	HdmiRx_Rb(0x00, 2, ucVenID);
	HdmiRx_Rb(0x02, 2, ucDevID);

	HdmiRx_Rb(0x00, 2, ucVenID);
	HdmiRx_Rb(0x02, 2, ucDevID);

	ucRevID = HdmiRx_R(0x04);

	iTE_Msg(("IT6662 -> 20151028\n"));

//	iTE_Msg(("*****TEST*****\n"));
	iTE_Msg(("###############################################\r\n"));
	iTE_Msg(("#           			IT6662 Detect           					#\r\n"));
	iTE_Msg(("###############################################\r\n"));

	iTE_Msg(("Current VenID=%02X-%02X\r\n", (int)ucVenID[1], (int)ucVenID[0]));
	iTE_Msg(("Current DevID=%02X-%02X\r\n", (int)ucDevID[1], (int)ucDevID[0]));
	iTE_Msg(("Current RevID=%02X\r\n", (int)ucRevID));

	if( ucVenID[0]!=0x54 || ucVenID[1]!=0x49 || ucDevID[0]!=0x62 || ucDevID[1]!=0x66){// || (ucRevID!=0xB0 && ucRevID!=0xB1)) {	//V1.06_1
		iTE_Msg(("\nERROR: Can not find iTE Device !!!\r\n"));
		//return iTE_FALSE;
     }

	return iTE_TRUE;
}
//****************************************************************************
iTE_u1 IT6662_Init(void)
{
	iTE_Msg(("\nIT6662_Init S\r\n"));
	if(iTE_TRUE == IT6662_Detect()){
		IT6662_VarInit(stIt6662);
		HdmiRx_Init();
		MhlRx_Init();
		HdmiTx_Init();
#if (USING_CEC == iTE_TRUE)
		Cec_BlockSel(CEC_RX_SEL);
		HdmiRx_EnCec();
		Cec_Init();
		Cec_BlockSel(CEC_TX_SEL);
		HdmiTx_EnCec();
		Cec_Init();
#endif
		iTE_Msg(("IT6662_Init E\r\n"));
		return iTE_TRUE;
	}

	return iTE_FALSE;
}
//****************************************************************************
void IT6662_Irq(void)
{
	iTE_u8	u8IntSta;

//	iTE_Msg(("IT6662_Irq S\r\n"));

	u8IntSta = HdmiRx_R(0xE0);
#if (A0_WorkAround == iTE_TRUE)
	u8IntSta = 0x70;
#endif
	if(u8IntSta & 0x40) {
		Mhl_CBusIrq();
		Mhl_MscIrq();
	}
	if(u8IntSta & 0x20) {
		HdmiRx_Irq();
	}

	if(u8IntSta & 0x1F) {	//
		HdmiTx_Irq();
	}

#if (USING_CEC == iTE_TRUE)
	u8IntSta = HdmiRx_R(0xE1);
	if(u8IntSta & 0x02){
		Cec_BlockSel(CEC_TX_SEL);
		Cec_Irq();
	}

	if(u8IntSta & 0x01){
		Cec_BlockSel(CEC_RX_SEL);
		Cec_Irq();
	}

#endif
}
//****************************************************************************
void IT6662_VarInit(IT6662_Var* stCurIt6662Var)
{
	iTE_u16	u16Temp = sizeof(IT6662_Var);
	iTE_pu8	pu8Tmp = (iTE_u8 *)stCurIt6662Var;

	while(u16Temp--){
		*pu8Tmp++ = 0;
	};
	u16Temp = sizeof(EdidInfo)*4;
	pu8Tmp = (iTE_u8 *)g_stEdidInfo;
	while(u16Temp--){
		*pu8Tmp++ = 0;
	};
}
//****************************************************************************
void IT6662_ClrHpdCnt(void)
{
	stIt6662->u8TxTrgRxHpd = 0;
	stIt6662->u8TxHpdCnt[0]  = 0;
	stIt6662->u8TxHpdCnt[1]  = 0;
	stIt6662->u8TxHpdCnt[2]  = 0;
	stIt6662->u8TxHpdCnt[3]  = 0;
}
//****************************************************************************
void IT6662_RxHpdTrg(void)
{
#if (USING_CASCADE == iTE_TRUE)
	if(stIt6662->u8TxTrgRxHpd){
		iTE_Msg(("%d TxTrgRxHpd=0x%02X\n", stIt6662->bRxHpdStatus, stIt6662->u8TxTrgRxHpd ));
		if(stIt6662->bRxHpdStatus){
#if (USING_REPEATER == iTE_TRUE)
			HdmiRx_TrgHpd(10);
#else
			if(stIt6662->u8TxTrgRxHpd & 0x80){
				HdmiRx_TrgHpd(10);
			}
#endif
		}else {
			HdmiRx_TrgHpd(1);
		}

		IT6662_ClrHpdCnt();
	}else{
//		iTE_Msg(("%d TxTrgRxHpd=0x%02X\n", stIt6662->bRxHpdStatus, stIt6662->u8TxHpdStatus ));
		if(stIt6662->bRxHpdStatus && ((stIt6662->u8TxHpdStatus & TX_HPD_MASK) == 0)){
			HdmiRx_HpdClr(0);
		}
	}
#endif
}
//****************************************************************************

//****************************************************************************
iTE_u8	IT6662_GetReg(iTE_u8	u8I2cAdr, iTE_u16 u16Reg)
{
	iTE_u8	u8RegValue;
	iTE_u8	u8Temp = u16Reg >> 8;
	if(u8Temp){
		iTE_I2C_WriteBurst(u8I2cAdr, 0x0F, 1, &u8Temp);
	}
	iTE_I2C_ReadBurst(u8I2cAdr, u16Reg & 0xFF, 1, &u8RegValue);
	if(u8Temp){
		u8Temp = 0;
		iTE_I2C_WriteBurst(u8I2cAdr, 0x0F, 1, &u8Temp);
	}
	return u8RegValue;
}
//****************************************************************************
void	IT6662_SetReg(iTE_u8	u8I2cAdr, iTE_u16 u16Reg, iTE_u8 u8Data)
{
	iTE_u8	u8Temp = u16Reg >> 8;
	if(u8Temp){
		iTE_I2C_WriteBurst(u8I2cAdr, 0x0F, 1, &u8Temp);
	}
	iTE_I2C_WriteBurst(u8I2cAdr, u16Reg & 0xFF, 1, &u8Data);
	if(u8Temp){
		u8Temp = 0;
		iTE_I2C_WriteBurst(u8I2cAdr, 0x0F, 1, &u8Temp);
	}
}
