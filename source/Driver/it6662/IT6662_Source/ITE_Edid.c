///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Edid.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Edid.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#include "it6662/include/ITE_Edid.h"
#include "it6662/include/Hdmi_Tx.h"
#include "it6662/include/Edid_DtdInfo.h"

iTE_u8	_CODE	u8EdidBlk0Header[8] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
iTE_u8	_CODE	u8ExtVic[4] = {95, 94, 93, 98};
//extern EdidInfo	g_stEdidInfoAry[4];
extern EdidInfo*	g_stEdidInfo;
//****************************************************************************
extern iTE_u8 g_u8TxSel, g_u8TxOffset, g_u8TxBank, g_u8TxShift;
extern IT6662_Var	*stIt6662;
//****************************************************************************
void Edid_ParseInit(EdidInfo *pstEdidInfo)
{
	iTE_u16	u16Temp = EDID_INFO_STR_SIZE;
	iTE_pu8	pu8BufAdr = (iTE_u8 *)pstEdidInfo;

	iTE_Edid_Msg(("EDID_INFO_STR_SIZE = %d\n", EDID_INFO_STR_SIZE));
	while(u16Temp--){
		*pu8BufAdr++ = 0;
	};

	for(u16Temp = 0; u16Temp < DEFAULT_3D_CNT; u16Temp++){
		pstEdidInfo->u8Default3DVic[u16Temp] = 0xFF;
	}
}
//****************************************************************************
void Edid_FixTabLoad(iTE_pu8 pu8EdidTab, EdidInfo *pstEdidInfo )
{
//	iTE_u8	u8Temp, u8CheckSum = 0;
#if	(USING_1to8 == iTE_TRUE)
	iTE_u8	u8TmpCurDev = g_u8CurDev;
	IT6662_DeviceSelect(IT6662_C);
#endif
	iTE_HRx_Msg(("Edid_FixTabLoad S\n"));

	Edid_ParseInit(pstEdidInfo);

	Edid_FixTabBlkLoad(pu8EdidTab, 0);
	Edid_FixTabBlkLoad(pu8EdidTab+0x80, 1);

#if	(USING_1to8 == iTE_TRUE)
	IT6662_DeviceSelect(u8TmpCurDev);
#endif
	Edid_DtdParse(0x36, pstEdidInfo, 0xFF00, pu8EdidTab);
	Edid_FixTabCeaParse(&pu8EdidTab[0x80], pstEdidInfo, 1);

	iTE_Edid_Msg(("Edid_FixTabLoad E\n"));
}
//****************************************************************************
void Edid_FixTabBlkLoad(iTE_pu8 pu8EdidTab, iTE_u8 u8BlkNo)
{
	iTE_u8	u8Temp, u8CheckSum = 0;
#if	(USING_1to8 == iTE_TRUE)
	iTE_u8	u8TmpCurDev = g_u8CurDev;
	IT6662_DeviceSelect(IT6662_C);
#endif
#if 1
	for(u8Temp = 0; u8Temp<0x08; u8Temp++){
		iTE_Edid_Msg(("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X \n",
			u8Temp*0x10, pu8EdidTab[u8Temp*0x10], pu8EdidTab[u8Temp*0x10+0x01], pu8EdidTab[u8Temp*0x10+0x02], pu8EdidTab[u8Temp*0x10+0x03],
			pu8EdidTab[u8Temp*0x10+0x04], pu8EdidTab[u8Temp*0x10+0x05], pu8EdidTab[u8Temp*0x10+0x06], pu8EdidTab[u8Temp*0x10+0x07],
			pu8EdidTab[u8Temp*0x10+0x08], pu8EdidTab[u8Temp*0x10+0x09], pu8EdidTab[u8Temp*0x10+0x0A], pu8EdidTab[u8Temp*0x10+0x0B],
			pu8EdidTab[u8Temp*0x10+0x0C], pu8EdidTab[u8Temp*0x10+0x0D], pu8EdidTab[u8Temp*0x10+0x0E], pu8EdidTab[u8Temp*0x10+0x0F]));
	}
#endif
	u8BlkNo *= 0x80;
	Edid_Wb(u8BlkNo, 0x7F, pu8EdidTab);
	u8Temp = 0;
	while(u8Temp < 0x7F){
		u8CheckSum -= pu8EdidTab[u8Temp++];
	};
	Edid_W(u8BlkNo + 0x7F, u8CheckSum);
#if	(USING_1to8 == iTE_TRUE)
	IT6662_DeviceSelect(u8TmpCurDev);
#endif
}
//****************************************************************************
EDID_STAT Edid_FixTabCeaParse(iTE_u8 *u8EdidBuf, EdidInfo *pstEdidInfo, iTE_u1 bPDefault3DVic)
{
	iTE_u8	u8RdEnd = 0x80;
	VsdbInfo *pstVsdbInfo = &pstEdidInfo->pstVsdbInfo;
	iTE_u1	bCEAv23 = iTE_FALSE;
	EdidParStr	stEdidParPtr;

	if((u8EdidBuf[0] == 0x02) && (u8EdidBuf[1] == 0x03)){
		bCEAv23 = iTE_TRUE;
		stEdidParPtr.u8RdPtr = 4;
		stEdidParPtr.s8Length = 0;
		stEdidParPtr.eTag = CEA_TAG_NONE;
		stEdidParPtr.pu8RdBuf = u8EdidBuf;
		u8RdEnd = u8EdidBuf[2];
	}else{
		return EDID_HEADER_ERR;
	}


	for( ; stEdidParPtr.u8RdPtr < u8RdEnd; ) {
		Edid_CeaPrase(pstEdidInfo, &stEdidParPtr, u8EdidBuf, bPDefault3DVic);
		if(stEdidParPtr.eTag == CEA_VSDB_PA) {
			pstEdidInfo->pstVsdbInfo.u8PaAdr = 0x80 | stEdidParPtr.u8RdPtr;
		}
	}

	if(pstVsdbInfo->u8PaAdr){
//Blk1 Dtd
		Edid_DtdParse(u8RdEnd, pstEdidInfo, 0xFF01, u8EdidBuf);
		Edid_Dtd2Vic(pstEdidInfo);
		return EDID_NO_ERR;
	}else{
		return EDID_HEADER_ERR;
	}
}
//****************************************************************************
EDID_STAT Edid_ExtGetBlock(iTE_u8 u8EdidBlk)
{
	EDID_STAT eEdidRdSta;

	eEdidRdSta = 	Edid_ExtSaveBlock(u8EdidBlk);
	HdmiTx_DdcDisable();

	return eEdidRdSta;
}
//****************************************************************************
EDID_STAT Edid_ExtSaveBlock(iTE_u8 u8EdidBlk)
{
	iTE_u8	u8WrStart;
	iTE_u8	u8RdStart, u8RdEnd, u8RdCnt;
	iTE_u8	u8Temp = 0;
	iTE_u8	u8CheckSum = 0;
	iTE_u8	u8EdidBuf[EDID_READ_MAX];
#if	(USING_1to8 == iTE_TRUE)
			iTE_u8	u8TmpCurDev = g_u8CurDev;
#endif

	iTE_Edid_Msg(("Edid_ExtSaveBlock %d\n", u8EdidBlk));

	if(u8EdidBlk){
		u8WrStart = 0x80;
	}else{
		u8WrStart = 0x00;
	}

	u8RdStart = (u8EdidBlk&0x01) * 0x80;
	u8RdEnd = u8RdStart + 0x7F;

	HdmiTx_DdcReset();
	while(u8RdStart < u8RdEnd){
		if(u8RdStart + EDID_READ_MAX <= u8RdEnd){
			u8RdCnt = EDID_READ_MAX;
		}else{
			u8RdCnt = u8RdEnd - u8RdStart;
		}

		iTE_Edid_Msg(("%x, %x, %x\n", u8RdStart, u8RdCnt, u8EdidBlk));
		if(EDID_READ_ERR == HdmiTx_FireEdidRead(u8RdStart, u8RdCnt, u8EdidBlk)){
			return EDID_READ_ERR;
		}
		HdmiTx_EdidRead(u8RdCnt, u8EdidBuf);
#if 0
		if(u8EdidBlk == 0){
			for(u8Temp = 0; (u8RdStart+u8Temp) < 8; u8Temp++){
				if(u8RdEdid[u8Temp] != u8EdidBlk0Header[u8RdStart+u8Temp]){
					iTE_MsgE(("EDID Block 0 Header Check Fail, EDID_0[0x%02X] = 0x%02X\n", u8RdStart+u8Temp, u8EdidBuf[u8Temp]));
					return EDID_HEADER_ERR;
				}
			}
		}
#endif

#if	(USING_1to8 == iTE_TRUE)
			IT6662_DeviceSelect(IT6662_C);
#endif
		Edid_Wb(u8WrStart, u8RdCnt, u8EdidBuf);
#if	(USING_1to8 == iTE_TRUE)
			IT6662_DeviceSelect(u8TmpCurDev);
#endif
		u8WrStart += u8RdCnt;
		u8RdStart += u8RdCnt;
		u8Temp = u8RdCnt;
		while(u8Temp){
			u8CheckSum -= u8EdidBuf[--u8Temp];
		};
	}
#if	(USING_1to8 == iTE_TRUE)
			IT6662_DeviceSelect(IT6662_C);
#endif
	if(u8EdidBlk == 0){
		u8Temp = u8EdidBuf[u8RdCnt-1];
		if(u8Temp > 1){
			u8CheckSum = u8CheckSum + u8Temp - 1;
			u8EdidBuf[0] = 1;
			u8EdidBuf[1] = u8CheckSum;
			Edid_Wb(0x7E, 0x02, u8EdidBuf);
		}else{
			Edid_W(0x7F, u8CheckSum);
		}
	}else{
		Edid_W(0xFF, u8CheckSum);
	}
#if	(USING_1to8 == iTE_TRUE)
			IT6662_DeviceSelect(u8TmpCurDev);
#endif
	return EDID_NO_ERR;
}
//****************************************************************************
void Edid_ExtCheck(iTE_u8 u8TxSel)
{
	iTE_Msg(("Edid_ExtCheck %x\n", u8TxSel));

	if(stIt6662->u8EdidCheck & (0x01 << u8TxSel)){
		HdmiTx_Select(u8TxSel);
		if(stIt6662->u8TxHpdStatus & g_u8TxShift){
			EDID_STAT eStatus;
			iTE_u8	u8Cnt, u8Tmp;

			HdmiTx_Bank(g_u8TxBank);
			for(u8Cnt = 0; u8Cnt < EDID_RETRY_MAX; u8Cnt++){
				eStatus = Edid_ExtParse(&g_stEdidInfo[u8TxSel]);
				u8Tmp = HdmiTx_R(0x40 + g_u8TxOffset) & 0x01;
				if((EDID_READ_ERR != eStatus) || u8Tmp ){
					break;
				}
			}
			HdmiTx_Bank(0);

			if((EDID_READ_ERR == eStatus) && u8Tmp ){
				iTE_Msg((" Edid_ExtCheck Edid Block Read Fail  => Retry!! \n"));
				stIt6662->u8TxEdidRdFail |= g_u8TxShift;
				return;
			}else if(EDID_NO_ERR != eStatus){
				iTE_MsgE((" Edid_ExtCheck Edid Block Read Fail \n"));
			}else{
				Edid_InfoShow(&g_stEdidInfo[u8TxSel]);
				return;
			}
		}
		Edid_ParseInit(&g_stEdidInfo[u8TxSel]);
	}
}
//****************************************************************************
EDID_STAT Edid_ExtParse(EdidInfo *pstEdidInfo)
{
	EDID_STAT eEdidRdSta;
	iTE_u8	u8Temp;

	Edid_ParseInit(pstEdidInfo);
	eEdidRdSta = Edid_ExtBlk0Parse(pstEdidInfo);
	HdmiTx_DdcDisable();
	if(eEdidRdSta == EDID_NO_ERR){
		for(u8Temp=0; u8Temp<pstEdidInfo->u8ExtBlkCnt;){
			eEdidRdSta = Edid_ExtCeaParse(++u8Temp, pstEdidInfo, 0);
			HdmiTx_DdcDisable();
		}
	}

	if((eEdidRdSta == EDID_NO_ERR) && (pstEdidInfo->pstVsdbInfo.u8PaAdr)){
		return EDID_NO_ERR;
	}else{
		return eEdidRdSta;
	}
}
//****************************************************************************
EDID_STAT Edid_ExtGet(iTE_u8 u8HdmiTxPort)
{
	EdidInfo *pstEdidInfo = &g_stEdidInfo[u8HdmiTxPort];
	EDID_STAT eEdidRdSta;
	iTE_u8	u8Temp, u8TmpB6;
//	iTE_u8	u8TmpB6, u8TmpChkSum;
	iTE_Edid_Msg(("\nEdid_Get %d\n", g_u8TxSel));

	u8TmpB6 = Edid_R((pstEdidInfo->pstVsdbInfo.u8PaAdr + 2) | 0x80);
	Edid_ParseInit(pstEdidInfo);
	eEdidRdSta =  Edid_ExtBlk0Parse(pstEdidInfo);
	HdmiTx_DdcDisable();
	if(eEdidRdSta == EDID_NO_ERR){
		eEdidRdSta = Edid_ExtGetBlock(0);
		if(eEdidRdSta == EDID_NO_ERR){
			for(u8Temp = 0; u8Temp < pstEdidInfo->u8ExtBlkCnt;){
				eEdidRdSta = Edid_ExtCeaParse(++u8Temp, pstEdidInfo, 1);
				HdmiTx_DdcDisable();
				if(eEdidRdSta == EDID_NO_ERR){
					eEdidRdSta = Edid_ExtGetBlock(u8Temp);
				}
			}
		}
	}

	if(eEdidRdSta == EDID_NO_ERR){
		iTE_Edid_Msg(("VsdbInfo.u8PaAdr = 0x%x\n",  pstEdidInfo->pstVsdbInfo.u8PaAdr));
		u8Temp = pstEdidInfo->pstVsdbInfo.u8PaAdr | 0x80;
	if(u8Temp & 0x7F){
		iTE_u8	u8ChkSum = Edid_R(0xFF);
#if (USING_CEC == iTE_TRUE)
		iTE_u8	u8TmpPa[2];

		Edid_Rb(u8Temp, 2, u8TmpPa);
		u8ChkSum += u8TmpPa[0] + u8TmpPa[1] - DefaultEdid_PA_AB - DefaultEdid_PA_CD;
		u8TmpPa[0] = DefaultEdid_PA_AB;
		u8TmpPa[1] = DefaultEdid_PA_CD;
		Edid_Wb(u8Temp, 2, u8TmpPa);
#endif
		u8Temp += 2;
		u8ChkSum += Edid_R(u8Temp) - u8TmpB6;
		Edid_W(u8Temp, u8TmpB6);
		Edid_W(0xFF, u8ChkSum);
		return EDID_NO_ERR;
		}
	}
	Edid_ParseInit(pstEdidInfo);
	return eEdidRdSta;
}
//****************************************************************************
EDID_STAT Edid_ExtBlk0Parse(EdidInfo *pstEdidInfo)
{
	iTE_u8	u8RdStart, u8RdEnd, u8RdCnt;
	iTE_u8	u8Temp = 0;
	iTE_u8	u8EdidBuf[EDID_READ_MAX];

	u8RdStart = 0x00;
	u8RdEnd = 0x08;

	HdmiTx_DdcReset();
	while(u8RdStart < u8RdEnd){
		if(u8RdStart + EDID_READ_MAX <= u8RdEnd){
			u8RdCnt = EDID_READ_MAX;
		}else{
			u8RdCnt = u8RdEnd - u8RdStart;
		}

		if(EDID_READ_ERR == HdmiTx_FireEdidRead(u8RdStart, EDID_READ_MAX, 0)){
			return EDID_READ_ERR;
		}
		HdmiTx_EdidRead(EDID_READ_MAX, u8EdidBuf);

		for(u8Temp = 0; (u8RdStart+u8Temp) < 8; u8Temp++){
			if(u8EdidBuf[u8Temp] != u8EdidBlk0Header[u8RdStart+u8Temp]){
				iTE_MsgE(("EDID Block 0 Header Check Fail, EDID_0[0x%02X] = 0x%02X\n", u8RdStart+u8Temp, u8EdidBuf[u8Temp]));
				return EDID_HEADER_ERR;
			}
		}
		u8RdStart += u8RdCnt;
	};

	if(EDID_READ_ERR == Edid_DtdParse(0x36, pstEdidInfo, 0, NULL)){
		return EDID_READ_ERR;
	}

	if(EDID_READ_ERR == HdmiTx_FireEdidRead(0x7E, 1, 0)){
		return EDID_READ_ERR;
	}
	HdmiTx_EdidRead(1, &(pstEdidInfo->u8ExtBlkCnt));

	return EDID_NO_ERR;
}
//****************************************************************************
EDID_STAT Edid_ExtCeaParse(iTE_u8 u8EdidBlk, EdidInfo *pstEdidInfo, iTE_u1 bPDefault3DVic)
{
	iTE_u8	u8RdStart, u8RdEnd, u8RdCnt;
	iTE_u8	u8EdidBuf[EDID_READ_MAX];
	iTE_u1	bCEAv23 = iTE_FALSE;
	EdidParStr	stEdidParPtr;
	iTE_u1	bVsdbFound = iTE_FALSE;

	iTE_Edid_Msg(("Edid_ExtCeaParse %d\n", u8EdidBlk));
	u8RdStart = (u8EdidBlk & 0x01) * 0x80;
	u8RdEnd = u8RdStart + 0x7F;

	HdmiTx_DdcReset();
	while(u8RdStart < u8RdEnd){
		if(u8RdStart + EDID_READ_MAX <= u8RdEnd){
			u8RdCnt = EDID_READ_MAX;
		}else{
			u8RdCnt = u8RdEnd - u8RdStart;
		}

		if(EDID_READ_ERR == HdmiTx_FireEdidRead(u8RdStart, EDID_READ_MAX, u8EdidBlk)){
			return EDID_READ_ERR;
		}
		HdmiTx_EdidRead(EDID_READ_MAX, u8EdidBuf);

		if(bCEAv23 == iTE_FALSE) {
			if((u8EdidBuf[0] == 0x02) && (u8EdidBuf[1] == 0x03)){
				bCEAv23 = iTE_TRUE;
				stEdidParPtr.u8RdPtr = 4;
				stEdidParPtr.s8Length = 0;
				stEdidParPtr.eTag = CEA_TAG_NONE;
				stEdidParPtr.pu8RdBuf = u8EdidBuf;
				u8RdEnd = u8EdidBuf[2]+u8RdStart;		//
				pstEdidInfo->u8CeaB3 = u8EdidBuf[3];
			}else{
				return EDID_HEADER_ERR;
			}
		}else{
			stEdidParPtr.u8RdPtr = 0;
		}

#if	(USING_DEFAULT_3D_VIC == iTE_FALSE)
		for(;stEdidParPtr.u8RdPtr <= (EDID_READ_MAX - EDID_READ_MIN);){
			if((u8RdStart + stEdidParPtr.u8RdPtr) < u8RdEnd){
				Edid_CeaPrase(pstEdidInfo, &stEdidParPtr, u8EdidBuf, bPDefault3DVic);
				if(stEdidParPtr.eTag == CEA_VSDB_PA) {
					if(pstEdidInfo->pstVsdbInfo.u8PaAdr == 0){
						pstEdidInfo->pstVsdbInfo.u8PaAdr = u8RdStart+ stEdidParPtr.u8RdPtr;
						bVsdbFound = iTE_TRUE;
					}
				}
			}else{
				//stEdidParPtr.u8RdPtr = EDID_READ_MAX - EDID_READ_MIN + 1;
				u8RdStart = u8RdEnd;
				break;
			}
		}
		u8RdStart += stEdidParPtr.u8RdPtr;
	}
#endif

	if(bVsdbFound){
//Blk1 Dtd
		if(EDID_READ_ERR == Edid_DtdParse(u8RdEnd, pstEdidInfo, u8EdidBlk, NULL)){
			return EDID_READ_ERR;
		}
		Edid_Dtd2Vic(pstEdidInfo);
		return EDID_NO_ERR;
	}else{
		return EDID_VSDB_ERR;
	}
}
//****************************************************************************
iTE_u8 Edid_ColorDepthCheck(void)
{
	iTE_u8	u8Temp, u8TmpB6 = 0xFF;

	for(u8Temp=0; u8Temp<4; u8Temp++){
		if(stIt6662->u8TxHpdStatus & (1 << u8Temp)){
			u8TmpB6 = u8TmpB6 & (g_stEdidInfo[u8Temp].pstVsdbInfo.u8B6 & 0xBF);	//un-support DC_48 bits
		}
	}

	return u8TmpB6;
}
//****************************************************************************
iTE_u1 Edid_ColorDepthChange(iTE_u8 u8VsdbB6, iTE_u8 u8VsdbB6Adr)
{
	iTE_u8	u8ChkSum;

	u8ChkSum = Edid_R(u8VsdbB6Adr);
	iTE_Edid_Msg(("ColorDepthChange 0x%X 0x%X \n", u8ChkSum, u8VsdbB6));
	u8VsdbB6 &= u8ChkSum;

	u8ChkSum -= u8VsdbB6;
	if(u8ChkSum){
		u8ChkSum += Edid_R(0xFF);
		Edid_W(u8VsdbB6Adr, u8VsdbB6);
		Edid_W(0xFF, u8ChkSum);
		return 1;
	}
	return 0;
}
//****************************************************************************
void Edid_InfoShow(EdidInfo *pstEdidInfo)
{
#if	(USING_DEFAULT_3D_VIC == iTE_FALSE)
	iTE_Edid_Msg(("VSDB Info: \n"));
	iTE_Edid_Msg(("PA_Adr = 0x%02X, PA=0x%02X%02X\n", pstEdidInfo->pstVsdbInfo.u8PaAdr, pstEdidInfo->pstVsdbInfo.u8VsdbPaL, pstEdidInfo->pstVsdbInfo.u8VsdbPaH));
	iTE_Edid_Msg(("VSDB Byte6 = 0x%02X, Byte7 = 0x%02X\n", pstEdidInfo->pstVsdbInfo.u8B6, pstEdidInfo->pstVsdbInfo.u8B7));
	iTE_Edid_Msg(("ExtVic = 0x%02X, SbS=0x%04X, TaB=0x%04X, FP=0x%04X\n", pstEdidInfo->pstVsdbInfo.u8ExtVic, pstEdidInfo->pstVsdbInfo.u163dSbS, pstEdidInfo->pstVsdbInfo.u163dTaB, pstEdidInfo->pstVsdbInfo.u163dFP));
	iTE_Edid_Msg(("Blk0DtdVic = 0x%02X, 0x%02X, 0x%02X, 0x%02X\n Blk0UnSupDtd = 0x%02X\n", pstEdidInfo->u8Blk0Dtd[0], pstEdidInfo->u8Blk0Dtd[1], pstEdidInfo->u8Blk0Dtd[2], pstEdidInfo->u8Blk0Dtd[3], pstEdidInfo->u8UnSupDtd));
	iTE_Edid_Msg(("Blk1DtdVic = 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", pstEdidInfo->u8Blk1Dtd[0], pstEdidInfo->u8Blk1Dtd[1], pstEdidInfo->u8Blk1Dtd[2], pstEdidInfo->u8Blk1Dtd[3], pstEdidInfo->u8Blk1Dtd[4], pstEdidInfo->u8Blk1Dtd[5]));
	iTE_Edid_Msg(("VDB Info:\n"));
#if (SAVE_VIC_LIST == iTE_TRUE)
	iTE_Edid_Msg(("NatVic = 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", pstEdidInfo->pstVdbInfo.u8NatList[0], pstEdidInfo->pstVdbInfo.u8NatList[1], pstEdidInfo->pstVdbInfo.u8NatList[2], pstEdidInfo->pstVdbInfo.u8NatList[3]));
	iTE_Edid_Msg(("VicList = %02X %02X %02X %02X %02X %02X %02X %02X \n", pstEdidInfo->pstVdbInfo.u8VicList[15], pstEdidInfo->pstVdbInfo.u8VicList[14], pstEdidInfo->pstVdbInfo.u8VicList[13], pstEdidInfo->pstVdbInfo.u8VicList[12],
		pstEdidInfo->pstVdbInfo.u8VicList[11], pstEdidInfo->pstVdbInfo.u8VicList[10], pstEdidInfo->pstVdbInfo.u8VicList[9], pstEdidInfo->pstVdbInfo.u8VicList[8]));
	iTE_Edid_Msg(("VicList = %02X %02X %02X %02X %02X %02X %02X %02X \n", pstEdidInfo->pstVdbInfo.u8VicList[7], pstEdidInfo->pstVdbInfo.u8VicList[6], pstEdidInfo->pstVdbInfo.u8VicList[5], pstEdidInfo->pstVdbInfo.u8VicList[4],
		pstEdidInfo->pstVdbInfo.u8VicList[3], pstEdidInfo->pstVdbInfo.u8VicList[2], pstEdidInfo->pstVdbInfo.u8VicList[1], pstEdidInfo->pstVdbInfo.u8VicList[0]));
	iTE_Edid_Msg(("3DVicList = %02X %02X %02X %02X %02X %02X %02X %02X \n", pstEdidInfo->pstVdbInfo.u8Vic3dList[0], pstEdidInfo->pstVdbInfo.u8Vic3dList[1], pstEdidInfo->pstVdbInfo.u8Vic3dList[2], pstEdidInfo->pstVdbInfo.u8Vic3dList[3],
		pstEdidInfo->pstVdbInfo.u8Vic3dList[4], pstEdidInfo->pstVdbInfo.u8Vic3dList[5], pstEdidInfo->pstVdbInfo.u8Vic3dList[6], pstEdidInfo->pstVdbInfo.u8Vic3dList[7]));
	iTE_Edid_Msg(("3DVicList = %02X %02X %02X %02X %02X %02X %02X %02X \n", pstEdidInfo->pstVdbInfo.u8Vic3dList[8], pstEdidInfo->pstVdbInfo.u8Vic3dList[9], pstEdidInfo->pstVdbInfo.u8Vic3dList[10], pstEdidInfo->pstVdbInfo.u8Vic3dList[11],
		pstEdidInfo->pstVdbInfo.u8Vic3dList[12], pstEdidInfo->pstVdbInfo.u8Vic3dList[13], pstEdidInfo->pstVdbInfo.u8Vic3dList[14], pstEdidInfo->pstVdbInfo.u8Vic3dList[15]));
#endif
	iTE_Edid_Msg(("Vic Cnt = 0x%02X\n", pstEdidInfo->pstVdbInfo.u8VicCnt));
//	iTE_Edid_Msg(("Vic Cnt = 0x%02X, Vic Start = 0x%02X\n", pstEdidInfo->pstVdbInfo.u8VicCnt, pstEdidInfo->pstVdbInfo.u8VicStart));

#if (PARSE_CEA_ADB == iTE_TRUE)
{
	iTE_u8	u8Temp;
	iTE_Edid_Msg(("SPK Info:\t0x%02X\n", pstEdidInfo->u8SpkAlloc));

	iTE_Edid_Msg(("ADB Info:\n"));
	for(u8Temp = 0; u8Temp < pstEdidInfo->pstAdbInfo.u8AdoCnt; u8Temp++){
		iTE_Edid_Msg(("\t0x%02X, 0x%02X, 0x%02X\n", pstEdidInfo->pstAdbInfo.u8AdoDes[u8Temp][0], pstEdidInfo->pstAdbInfo.u8AdoDes[u8Temp][1], pstEdidInfo->pstAdbInfo.u8AdoDes[u8Temp][2]));
	}
}
#endif

#endif
}
//****************************************************************************
iTE_u8 Edid_GetDtdDes(iTE_u8 u8Vic, iTE_pu8 pu8DtdDes)
{
	iTE_u8	u8Temp, u8ChkSum = 0;

	if(u8Vic){
		u8Vic -= 1;
	}

	for(u8Temp = 0; u8Temp < 12; u8Temp++){
		pu8DtdDes[u8Temp] = Dtd18Byte[u8Vic][u8Temp];
		u8ChkSum += pu8DtdDes[u8Temp];
	}

	for(;u8Temp < 17; u8Temp++){
		u8ChkSum += pu8DtdDes[u8Temp];
	}
	u8Temp = Dtd18Byte[u8Vic][12];
	pu8DtdDes[0x11] =  u8Temp | (pu8DtdDes[0x11] & 0x61);
	u8ChkSum += pu8DtdDes[0x11];

	return u8ChkSum;
}
//****************************************************************************
EDID_STAT Edid_DtdParse(iTE_u8 u8RdStart, EdidInfo *pstEdidInfo,  iTE_u16 u16EdidBlk, iTE_pu8 pu8EdidBuf)
{
	iTE_u8	u8RdEnd, u8Cnt1 = 0;
	iTE_u8	u8DtdBuf[18];
	iTE_pu8	u8Dtd;
	iTE_u8	u8Temp;
	iTE_u8	u8Cnt2 = 0;
	iTE_pu8  pu8VicAry;

	if(u16EdidBlk & 0xFF){
		pu8VicAry = pstEdidInfo->u8Blk1Dtd;
	}else{
		pu8VicAry = pstEdidInfo->u8Blk0Dtd;
	}

	if(u8RdStart & 0x80){
		u8RdEnd = 0xFE - 0x12;
	}else{
		u8RdEnd = 0x7E - 0x12;
	}

	while(u8RdStart <= u8RdEnd){
		if(u16EdidBlk & 0xFF00){
			if(pu8EdidBuf != NULL){
				u8Dtd = &pu8EdidBuf[u8RdStart];
			}else{
				iTE_MsgE(("Edid_DtdParse, EdidBuf = NULL, %X\n", u16EdidBlk));
				return EDID_READ_ERR;
			}
		}else{
			if(EDID_READ_ERR == HdmiTx_FireEdidRead(u8RdStart, 0x12, u16EdidBlk & 0xFF)){
				return EDID_READ_ERR;
			}
			u8Dtd = u8DtdBuf;
			HdmiTx_EdidRead(0x12, u8Dtd);
		}

		if(u8Dtd[0] || u8Dtd[1]){
			for(u8Temp = 0; u8Temp < DTD18_MAX; u8Temp++){
				if(	(u8Dtd[0] == Dtd18Byte[u8Temp][0]) &&
					(u8Dtd[1] == Dtd18Byte[u8Temp][1]) &&
					(u8Dtd[2] == Dtd18Byte[u8Temp][2]) &&
					(u8Dtd[3] == Dtd18Byte[u8Temp][3]) &&
					(u8Dtd[4] == Dtd18Byte[u8Temp][4]) &&
					(u8Dtd[5] == Dtd18Byte[u8Temp][5]) &&
		//			(u8Dtd[6] == Dtd18Byte[u8Temp][6]) &&
					(u8Dtd[7] == Dtd18Byte[u8Temp][7]) &&
					(u8Dtd[8] == Dtd18Byte[u8Temp][8]) &&
					(u8Dtd[11] == Dtd18Byte[u8Temp][11])	&&
					((u8Dtd[0x11] & 0x98)== (Dtd18Byte[u8Temp][12] & 0x98))	){
//					((u8Dtd[0x11] & 0x9E)== (Dtd18Byte[u8Temp][12] & 0x9E))	){
						iTE_u16	u16Width, u16Height;

						u16Width = (iTE_u16)((u8Dtd[14] & 0xF0) << 4) + u8Dtd[12];
						u16Height = (iTE_u16)((u8Dtd[14] & 0x0F) << 8) + u8Dtd[13];
						u16Width <<= 4;
						if(u16Height){
							if((u16Width / u16Height) < Dtd18Byte[u8Temp][13]) {
								break;
							}
						}else{
							break;
						}
				}
			}
			if(u8Temp < DTD18_MAX){
				pu8VicAry[u8Cnt1++] = u8Temp + 1;
				iTE_Edid_Msg(("Cnt1=%d, Vic = %d\n", u8Cnt1, u8Temp+1));
			}else{
				if((u16EdidBlk & 0xFF) == 0){
					pstEdidInfo->u8UnSupDtd |= (0x01 << u8Cnt2);
				}
			}
		}else if((u8Dtd[2] == 0x00) && (u8Dtd[3] == 0xFC) && (u8Dtd[4] == 0x00)){		//Display Product Name
			pstEdidInfo->u8PNameAdr = u8RdStart;
		}
		u8Cnt2++;
		u8RdStart += 0x12;
	}
	return	EDID_NO_ERR;
}

//****************************************************************************
void Edid_CeaPrase(EdidInfo *pstEdidInfo, EdidParStr *pstEdidParPtr, iTE_pu8 u8EdidBuf, iTE_u1 bPDefault3DVic)
{
	iTE_u8	u8Temp;

	switch(pstEdidParPtr->eTag & 0xE0){
		case	CEA_TAG_NONE:
				u8Temp = u8EdidBuf[pstEdidParPtr->u8RdPtr++];
				pstEdidParPtr->s8Length = u8Temp & 0x1F;
				pstEdidParPtr->eTag = (CeaTag)(u8Temp & 0xE0);
			break;
		case	CEA_TAG_VSDB:
				if(pstEdidParPtr->s8Length > 0){
					Edid_ParseVSDB(pstEdidParPtr, &pstEdidInfo->pstVsdbInfo);
				}else{
					pstEdidParPtr->eTag = CEA_TAG_NONE;
				}
			break;
#if	((PARSE_CEA_VDB == iTE_TRUE) && (SAVE_VIC_LIST == iTE_TRUE))
		case	CEA_TAG_VDB:
				u8Temp = u8EdidBuf[pstEdidParPtr->u8RdPtr++];
				if(u8Temp & 0x80){
					if(pstEdidInfo->pstVdbInfo.u8NatCnt < EDID_NATIVE_MAX){
						pstEdidInfo->pstVdbInfo.u8NatList[pstEdidInfo->pstVdbInfo.u8NatCnt++] = u8Temp;
					}
					u8Temp &= 0x7F;
				}
				if(bPDefault3DVic){
					Edid_GetDefault3DVic(pstEdidInfo, u8Temp, pstEdidInfo->pstVdbInfo.u8VicCnt);
				}
				pstEdidInfo->pstVdbInfo.u8VicList[u8Temp >> 3] |= 0x01 << (u8Temp & 0x07);
				if( pstEdidInfo->pstVdbInfo.u8VicCnt < EDID_3D_SUPP_MAX){
					pstEdidInfo->pstVdbInfo.u8Vic3dList[ pstEdidInfo->pstVdbInfo.u8VicCnt] = u8Temp;
				}
				pstEdidInfo->pstVdbInfo.u8VicCnt++;
				if(--pstEdidParPtr->s8Length == 0){
					pstEdidParPtr->eTag = CEA_TAG_NONE;
				}
			break;
#endif
#if (PARSE_CEA_ADB == iTE_TRUE)
		case	CEA_TAG_ADB:
				if((pstEdidParPtr->s8Length % 3) && (pstEdidInfo->pstAdbInfo.u8AdoCnt == 0)){
					iTE_MsgE(("Wrong ADB Length = %d\n", pstEdidParPtr->s8Length));
					pstEdidParPtr->eTag = CEA_TAG_NONE;
					pstEdidParPtr->u8RdPtr += pstEdidParPtr->s8Length;
				}else{
					u8Temp = u8EdidBuf[pstEdidParPtr->u8RdPtr++];
	//							pstEdidInfo->pstAdbInfo.u8SupFmt |= 0x01 << ((u8Temp & 0x38) >> 3);
					pstEdidInfo->pstAdbInfo.u8AdoDes[pstEdidInfo->pstAdbInfo.u8AdoCnt][0] = u8Temp;
					pstEdidInfo->pstAdbInfo.u8AdoDes[pstEdidInfo->pstAdbInfo.u8AdoCnt][1] =  u8EdidBuf[pstEdidParPtr->u8RdPtr++];
					pstEdidInfo->pstAdbInfo.u8AdoDes[pstEdidInfo->pstAdbInfo.u8AdoCnt][2] =  u8EdidBuf[pstEdidParPtr->u8RdPtr++];
					pstEdidInfo->pstAdbInfo.u8AdoCnt++;
					pstEdidParPtr->s8Length -= 3;
					if(pstEdidParPtr->s8Length <= 0){
						pstEdidParPtr->eTag = CEA_TAG_NONE;
					}
				}
			break;
#endif
		case	CEA_MODE_SPK:
					pstEdidInfo->u8SpkAlloc = u8EdidBuf[pstEdidParPtr->u8RdPtr];
					pstEdidParPtr->eTag = CEA_TAG_NONE;
					pstEdidParPtr->u8RdPtr += pstEdidParPtr->s8Length;
			break;
		case	CEA_MODE_DTC:
		case	CEA_MODE_REV:
		case	CEA_MODE_EXT:
		default:
				pstEdidParPtr->eTag = CEA_TAG_NONE;
				pstEdidParPtr->u8RdPtr += pstEdidParPtr->s8Length;
			break;
	}
}
//****************************************************************************
void Edid_Dtd2Vic(EdidInfo *pstEdidInfo)
{
	iTE_u8	u8Temp, u8Vic;

	for(u8Temp = 0; u8Temp < EDID_BLK1_DTD_MAX; u8Temp++){
		u8Vic = pstEdidInfo->u8Blk1Dtd[u8Temp];
		if(u8Vic){
			Edid_VicConvert(u8Vic, &pstEdidInfo->pstVdbInfo);
		}else{
			break;
		}
	}
	for(u8Temp = 0; u8Temp < EDID_BLK0_DTD_MAX; u8Temp++){
		u8Vic = pstEdidInfo->u8Blk0Dtd[u8Temp];
		if(u8Vic){
			Edid_VicConvert(u8Vic, &pstEdidInfo->pstVdbInfo);
		}else{
			break;
		}
	}
	for(u8Temp = 1; u8Temp < 5; u8Temp++){
		if(pstEdidInfo->pstVsdbInfo.u8ExtVic & (1 << u8Temp)){
			u8Vic = u8ExtVic[u8Temp - 1];
			Edid_VicConvert(u8Vic, &pstEdidInfo->pstVdbInfo);
		}
	}

	for(u8Temp = 0; u8Temp < 4; u8Temp ++){
		if(pstEdidInfo->pstVdbInfo.u8VicList[u8ExtVic[u8Temp] >> 3] & (1<<(u8ExtVic[u8Temp] & 0x07))){
			pstEdidInfo->pstVsdbInfo.u8ExtVic |= (1<<(u8Temp + 1));
		}
	}

}
//****************************************************************************
void Edid_VicConvert(iTE_u8 u8Vic, VdbInfo *pstVdbInfo)
{
	if(pstVdbInfo->u8VicList[u8Vic >> 3] & (1<< (u8Vic & 0x07))){
	}else{
		pstVdbInfo->u8VicList[u8Vic >> 3] |= (1<< (u8Vic & 0x07));
		pstVdbInfo->u8VicCnt++;
	}
}
//****************************************************************************
void Edid_ParseVSDB(EdidParStr *pstEdidParPtr, VsdbInfo *pstVsdbInfo)
{
	iTE_u8	u8Temp;
	iTE_u8	u8RetValue = 0;
	iTE_u8	u8RdPtr = pstEdidParPtr->u8RdPtr;
	iTE_pu8	pu8EdidBuf = pstEdidParPtr->pu8RdBuf;
	iTE_u16	u16Temp;
//	CeaTag	*eTag = pstEdidParPtr->eTag;



	switch(pstEdidParPtr->eTag){
		case	CEA_VSDB_ID:
				if(pu8EdidBuf[u8RdPtr] == 0x03 && pu8EdidBuf[u8RdPtr+1] == 0x0C && pu8EdidBuf[u8RdPtr+2] == 0x00) {
					pstEdidParPtr->eTag = CEA_VSDB_PA;
					u8RetValue = 3;
				}else{
					pstEdidParPtr->eTag = CEA_TAG_NONE;
					u8RetValue = pstEdidParPtr->s8Length;
				}
			break;
		case	CEA_VSDB_PA:
				pstVsdbInfo->u8VsdbPaL = pu8EdidBuf[u8RdPtr];
				pstVsdbInfo->u8VsdbPaH = pu8EdidBuf[u8RdPtr+1];
				pstEdidParPtr->eTag = CEA_VSDB_CLR_DEP;
				u8RetValue = 2;
			break;
		case	CEA_VSDB_CLR_DEP:
				pstVsdbInfo->u8B6 = pu8EdidBuf[u8RdPtr];
				pstEdidParPtr->eTag = CEA_VSDB_TMDS_CLK;
				u8RetValue = 1;
			break;
		case	CEA_VSDB_TMDS_CLK:
				pstVsdbInfo->u8B7 = pu8EdidBuf[u8RdPtr];
				pstEdidParPtr->eTag = CEA_VSDB_HDMI_PRE;
				u8RetValue = 1;
				break;
		case	CEA_VSDB_HDMI_PRE:
				u8RetValue = 1;
				u8Temp = pu8EdidBuf[u8RdPtr];
				if(u8Temp & 0x80){
					u8RetValue += 2;
				}
				if(u8Temp & 0x40){
					u8RetValue += 2;
				}
				if(u8Temp & 0x20){
					pstEdidParPtr->eTag = CEA_VSDB_3D;
				}else{
					pstEdidParPtr->eTag = CEA_TAG_NONE;
					u8RetValue = pstEdidParPtr->s8Length;
				}
			break;
		case	CEA_VSDB_3D:
				pstVsdbInfo->u83dMulti = pu8EdidBuf[u8RdPtr];
				pstVsdbInfo->u83dLen = pu8EdidBuf[u8RdPtr+1];
				u8RetValue = 2;
				if(pstVsdbInfo->u83dMulti & 0x80){
//					pstVsdbInfo->u83dMulti &= 0x60;
					if(pstVsdbInfo->u83dLen & 0xE0){
						pstEdidParPtr->eTag = CEA_VSDB_4K2K;
					}else if(pstVsdbInfo->u83dLen & 0x1F){
						pstEdidParPtr->eTag = CEA_VSDB_STRUCT;
					}else{
						pstEdidParPtr->eTag = CEA_TAG_NONE;
						u8RetValue = pstEdidParPtr->s8Length;
					}
				}else{
					pstEdidParPtr->eTag = CEA_VSDB_4K2K;
					pstVsdbInfo->u83dLen &= 0xE0;
				}
			break;
		case	CEA_VSDB_4K2K:
				u8Temp = pu8EdidBuf[u8RdPtr];
				if(u8Temp >= 1 && u8Temp <= 4){
					pstVsdbInfo->u8ExtVic |= 1 << u8Temp;
				}
				if((pstVsdbInfo->u83dLen -= 0x20) <= 0x1F){
					if(pstVsdbInfo->u83dLen){
						pstEdidParPtr->eTag = CEA_VSDB_STRUCT;
					}else{
						pstEdidParPtr->eTag = CEA_TAG_NONE;
						u8RetValue = pstEdidParPtr->s8Length;
					}
				}
				u8RetValue = 1;
			break;
		case	CEA_VSDB_STRUCT:
				pstEdidParPtr->eTag = CEA_VSDB_3D_VIC;
				if((pstVsdbInfo->u83dMulti & 0x60)== 0x20){
					u16Temp = 0xFFFF;
					u8RetValue = 2;
				}else if((pstVsdbInfo->u83dMulti & 0x60) == 0x40){
					u16Temp = (iTE_u16)pu8EdidBuf[u8RdPtr+2]<<8;
					u16Temp += pu8EdidBuf[u8RdPtr+3];
					u8RetValue = 4;
				}else{
					break;
				}
				if(pu8EdidBuf[u8RdPtr] & 0x01){			// Side By Side
					pstVsdbInfo->u163dSbS = u16Temp;
				}
				u8Temp = pu8EdidBuf[u8RdPtr+1];
				if(u8Temp & 0x01){
					pstVsdbInfo->u163dFP = u16Temp;
				}
				if(u8Temp & 0x04){
					pstVsdbInfo->u163dTaB = u16Temp;
				}
				if(pstVsdbInfo->u83dLen <= u8RetValue){
					pstEdidParPtr->eTag = CEA_TAG_NONE;
					u8RetValue = pstEdidParPtr->s8Length;
				}
			break;
		case	CEA_VSDB_3D_VIC:
				u8Temp = pu8EdidBuf[u8RdPtr];
				u16Temp = 1 << (u8Temp >> 4);
				u8Temp &= 0x0F;

				if(u8Temp == 0){
					pstVsdbInfo->u163dFP |= u16Temp;
					u8RetValue = 1;
				}else if(u8Temp == 6){
					pstVsdbInfo->u163dTaB |= u16Temp;
					u8RetValue = 1;
				}else if(u8Temp == 8){
					pstVsdbInfo->u163dSbS |= u16Temp;
					u8RetValue = 2;
				}else{
					u8RetValue = 1;
				}
			break;
		case	CEA_VSDB_DONE:
				pstEdidParPtr->eTag = CEA_TAG_NONE;
				u8RetValue = pstEdidParPtr->s8Length;
			break;
		default:
			break;
	}
	pstEdidParPtr->u8RdPtr += u8RetValue;
	pstEdidParPtr->s8Length -= u8RetValue;
}
//****************************************************************************
void Edid_GetDefault3DVic(EdidInfo *pstEdidInfo, iTE_u8 u8Vic, iTE_u8 u8VicIndex)
{
	switch(u8Vic){
		case	0x04:
			if(pstEdidInfo->u8Default3DVic[0] == 0xFF){		pstEdidInfo->u8Default3DVic[0] = u8VicIndex;			}
			break;
		case	0x05:
			if(pstEdidInfo->u8Default3DVic[1] == 0xFF){		pstEdidInfo->u8Default3DVic[1] = u8VicIndex;			}
			break;
		case	0x13:
			if(pstEdidInfo->u8Default3DVic[2] == 0xFF){		pstEdidInfo->u8Default3DVic[2] = u8VicIndex;			}
			break;
		case	0x14:
			if(pstEdidInfo->u8Default3DVic[3] == 0xFF){		pstEdidInfo->u8Default3DVic[3] = u8VicIndex;			}
			break;
		case	0x20:
			if(pstEdidInfo->u8Default3DVic[4] == 0xFF){		pstEdidInfo->u8Default3DVic[4] = u8VicIndex;			}
			break;
		default:
			break;
	}
}
//****************************************************************************
iTE_u8	_CODE aVicCnt[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
//****************************************************************************
iTE_u8 Edid_GetBitCnt(iTE_u16 u16Word)
{
	iTE_u8	u8Temp;

	u8Temp = aVicCnt[u16Word & 0x0F];
	u8Temp += aVicCnt[(u16Word >> 4) & 0x0F];
	u8Temp += aVicCnt[(u16Word >> 8) & 0x0F];
	u8Temp += aVicCnt[(u16Word >> 12) & 0x0F];

	return u8Temp;
}
//****************************************************************************
void Edid_CalVicCnt(VdbInfo *pstVdbInfo)
{
	iTE_u8	u8TmpVicList[16], u8Cnt1, u8Temp;

	pstVdbInfo->u8VicCnt = 0;
	for(u8Cnt1 = 0; u8Cnt1 < 16; u8Cnt1++){
		u8TmpVicList[u8Cnt1] = pstVdbInfo->u8VicList[u8Cnt1];
		pstVdbInfo->u8VicCnt += Edid_GetBitCnt(u8TmpVicList[u8Cnt1]);
	}
	for(u8Cnt1 = 0; u8Cnt1 < EDID_3D_SUPP_MAX; u8Cnt1++){
		u8Temp = pstVdbInfo->u8Vic3dList[u8Cnt1];
		if(u8Temp){
			if(u8TmpVicList[u8Temp >> 3] & (0x01 << (u8Temp & 0x07))){
				u8TmpVicList[u8Temp >> 3] &= ~(0x01 << (u8Temp & 0x07));
			}else{
				pstVdbInfo->u8VicCnt++;
			}
		}else{
			break;
		}
	}
}
//****************************************************************************
iTE_u16 Edid_Analyze3dType(iTE_pu16 pu16SbS, iTE_pu16 pu16TaB, iTE_pu16 pu16FP, iTE_pu8 pu8Type, iTE_pu8 pu8Len)
{
	iTE_u16	u16Temp = -1;

//	pu8Type = 0;
	if(*pu16SbS){	u16Temp &= *pu16SbS;	*pu8Type|= 0x01;	}
	if(*pu16TaB){	u16Temp &= *pu16TaB;	*pu8Type|= 0x02;	}
	if(*pu16FP){		u16Temp &= *pu16FP;		*pu8Type|= 0x04;	}
	*pu8Len = Edid_GetBitCnt(u16Temp);
	switch(*pu8Type){
		case	0x00:
				*pu8Len = 0;
			break;
		case	0x01:
			if(*pu8Len > 1){
				*pu8Len = 4;
				*pu8Type |= 0x80;
				*pu16SbS = 0;
			}else{
				*pu8Len *= 2;
			}
			break;
		case	0x02:
		case	0x04:
			if(*pu8Len > 3){
				*pu8Len = 4;
				*pu8Type |= 0x80;
				*pu16TaB = 0;
				*pu16FP = 0;
			}
			break;
		default:
			if(*pu8Len > 1){
				*pu8Len = 4;
				*pu8Type |= 0x80;
			}else{
				*pu8Len = 0;
				u16Temp = 0;
			}
			if(*pu16SbS &= ~u16Temp){	*pu8Len += Edid_GetBitCnt(*pu16SbS)*2;	}
			if(*pu16TaB &= ~u16Temp){	*pu8Len += Edid_GetBitCnt(*pu16TaB);	}
			if(*pu16FP &= ~u16Temp){	*pu8Len += Edid_GetBitCnt(*pu16FP);		}
			break;
	}
	return u16Temp;
}
//****************************************************************************

//****************************************************************************
void Edid_RegenBlk1(EdidInfo *pstEdidInfo, iTE_pu8 u8Edid, iTE_u1 bPDefault3DVic)
{
	iTE_u8	u8Ptr, u8VicCnt, u8VicTmpCnt;
	iTE_u8	u8Temp, u8TmpVic;
	iTE_u8	u8Cnt1, u8Cnt2, u8Cnt3, u8NatCnt;
	iTE_u8	u8TmpVicList[EDID_VIC_LIST_MAX];
	iTE_u8	u8VdbCnt = 0;

	u8Edid[0] = 0x02;
	u8Edid[1] = 0x03;

	u8Edid[3] = (pstEdidInfo->u8CeaB3 & 0xF0) |0x01;
	u8Ptr = 4;

// VDB
	u8NatCnt = 0;
	for(u8Cnt2 = 0; u8Cnt2 < EDID_NATIVE_MAX; u8Cnt2++){
		if(pstEdidInfo->pstVdbInfo.u8NatList[u8Cnt2]){
			u8NatCnt++;
		}
	}

	u8Cnt2 = EDID_VIC_LIST_MAX;
	while(u8Cnt2--){
		u8TmpVicList[u8Cnt2] = 0;
	}

	u8VicCnt = 0;
	for(u8Cnt2 = 0; u8Cnt2 < 0x10; u8Cnt2++){				// add for if more than one same VIC in 3dVicList
		u8TmpVic = pstEdidInfo->pstVdbInfo.u8Vic3dList[u8Cnt2];
		if(u8TmpVic){
			if(u8TmpVicList[u8TmpVic >> 3] & (0x01 << (u8TmpVic & 0x07))){
				u8VicCnt++;
			}else{
				u8TmpVicList[u8TmpVic >> 3] |= (0x01 << (u8TmpVic & 0x07));
			}
		}
	}
#if 1
	for(u8Cnt2 = 0; u8Cnt2 < EDID_VIC_LIST_MAX; u8Cnt2++){
		u8TmpVicList[u8Cnt2] = pstEdidInfo->pstVdbInfo.u8VicList[u8Cnt2];
		if(u8TmpVicList[u8Cnt2]){
			for(u8Cnt1 = 0; u8Cnt1 < 0x08; u8Cnt1++){
				if(u8TmpVicList[u8Cnt2] & (0x01 << u8Cnt1)){
					u8VicCnt++;
				}
			}
		}
	}
#else
	u8VicCnt += pstEdidInfo->pstVdbInfo.u8VicCnt;
#endif

	if(u8VicCnt){
		if(u8VicCnt > 0x1F){
			u8VicTmpCnt = 0x1F;
		}else{
			u8VicTmpCnt = u8VicCnt;
		}
		u8VicCnt -= u8VicTmpCnt;
		u8Edid[u8Ptr++] = 0x40 | u8VicTmpCnt;


		for(u8Cnt1 = 0; u8Cnt1 < EDID_3D_SUPP_MAX; u8Cnt1++){															// 3D VicList
			u8TmpVic = pstEdidInfo->pstVdbInfo.u8Vic3dList[u8Cnt1];
			if(u8TmpVic){
				u8VicTmpCnt--;
				u8Edid[u8Ptr] = u8TmpVic;
				u8TmpVicList[u8TmpVic >> 3] &= ~(0x01 << (u8TmpVic&0x07));
				if(u8NatCnt){
					for(u8Cnt2 = 0; u8Cnt2 < EDID_NATIVE_MAX; u8Cnt2++){
						if(pstEdidInfo->pstVdbInfo.u8NatList[u8Cnt2]){
							if((u8TmpVic | 0x80) == pstEdidInfo->pstVdbInfo.u8NatList[u8Cnt2]){
								u8NatCnt--;
								u8Edid[u8Ptr] = u8TmpVic | 0x80;
								break;
							}
						}else{
							break;
						}
					}
				}
				if(bPDefault3DVic){
					Edid_GetDefault3DVic(pstEdidInfo, u8TmpVic, u8VdbCnt);
				}

				u8VdbCnt++;
				u8Ptr++;
			}else{
				break;
			}
		}

		for(u8Cnt1 =0; u8Cnt1 < 16; u8Cnt1++){																		// VicList
			u8Temp = u8TmpVicList[u8Cnt1];
			if(u8Temp){
				for(u8Cnt2 = 0; u8Cnt2 < 8; u8Cnt2++){
					if(u8Temp & (0x01 << u8Cnt2)){
						if(u8VicTmpCnt == 0){
							if(u8VicCnt > 0x1F){
								u8VicTmpCnt = 0x1F;
							}else{
								u8VicTmpCnt = u8VicCnt;
							}
							u8VicCnt -= u8VicTmpCnt;
							u8Edid[u8Ptr++] = 0x40 | u8VicTmpCnt;
						}
						u8VicTmpCnt--;
						u8TmpVic = u8Cnt1*8 + u8Cnt2;
						u8Edid[u8Ptr] = u8TmpVic;
						if(u8NatCnt){
							for(u8Cnt3 = 0; u8Cnt3 < EDID_NATIVE_MAX; u8Cnt3++){
								if(pstEdidInfo->pstVdbInfo.u8NatList[u8Cnt3]){
									if((u8TmpVic | 0x80) == pstEdidInfo->pstVdbInfo.u8NatList[u8Cnt3]){
										u8NatCnt--;
										u8Edid[u8Ptr] = u8TmpVic | 0x80;
										break;
									}
								}else{
									break;
								}
							}
						}
						if(bPDefault3DVic){
							Edid_GetDefault3DVic(pstEdidInfo, u8TmpVic, u8VdbCnt);
						}
						u8VdbCnt++;
						u8Ptr++;
					}
				}
			}
		}
	}
// ADB
	u8Temp = pstEdidInfo->pstAdbInfo.u8AdoCnt;
	if(u8Temp){
		do{
			if(u8Temp > 10){
				u8Cnt2 = 10;
			}else{
				u8Cnt2 = u8Temp;
			}
			u8Temp -= u8Cnt2;
			u8Edid[u8Ptr++] = 0x20 | u8Cnt2*3;
			for(u8Cnt1 = 0; u8Cnt1 < u8Cnt2; u8Cnt1++){
				u8Edid[u8Ptr++] = pstEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt1][0];
				u8Edid[u8Ptr++] = pstEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt1][1];
				u8Edid[u8Ptr++] = pstEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt1][2];
			}
		}while(u8Temp);
	}
// SADB
	if(pstEdidInfo->u8SpkAlloc){
		u8Edid[u8Ptr++] = 0x80 | 0x03;
		u8Edid[u8Ptr++] = pstEdidInfo->u8SpkAlloc;
		u8Edid[u8Ptr++] = 0;
		u8Edid[u8Ptr++] = 0;
	}
// VSDB
	u8Temp = u8Ptr++;
	u8Edid[u8Ptr++] = 0x03;
	u8Edid[u8Ptr++] = 0x0C;
	u8Edid[u8Ptr++] = 0x00;
	pstEdidInfo->pstVsdbInfo.u8PaAdr = u8Ptr;
	u8Edid[u8Ptr++] = DefaultEdid_PA_AB;
	u8Edid[u8Ptr++] = DefaultEdid_PA_CD;
	pstEdidInfo->pstVsdbInfo.u8VsdbPaL = DefaultEdid_PA_AB;
	pstEdidInfo->pstVsdbInfo.u8VsdbPaH = DefaultEdid_PA_CD;
	if(pstEdidInfo->pstVsdbInfo.u8B6 || pstEdidInfo->pstVsdbInfo.u8B7){
		u8Edid[u8Ptr++] = pstEdidInfo->pstVsdbInfo.u8B6;
		if(pstEdidInfo->pstVsdbInfo.u8B7){
			u8Edid[u8Ptr++] = pstEdidInfo->pstVsdbInfo.u8B7;
			if(pstEdidInfo->pstVsdbInfo.u8ExtVic || (pstEdidInfo->pstVsdbInfo.u83dMulti & 0x80)){//pstEdidInfo->pstVsdbInfo.u163dSbS ||  pstEdidInfo->pstVsdbInfo.u163dTaB ||  pstEdidInfo->pstVsdbInfo.u163dFP){
				iTE_u8	u83dLen, u83dType = 0;
				iTE_u16	u163dMask;

				u83dLen = 0;
				u8Edid[u8Ptr++] = 0x20;

				if(pstEdidInfo->pstVsdbInfo.u83dMulti & 0x80) {
					u163dMask = Edid_Analyze3dType(&pstEdidInfo->pstVsdbInfo.u163dSbS, &pstEdidInfo->pstVsdbInfo.u163dTaB, &pstEdidInfo->pstVsdbInfo.u163dFP, &u83dType, &u83dLen);
					if(u83dType & 0x80){
						u8Edid[u8Ptr++] = 0xC0;
					}else{
						u8Edid[u8Ptr++] = 0x80;
					}
				}else{
					u8Edid[u8Ptr++] = 0x00;
				}
				if(u83dLen > 0x1F){	iTE_MsgE(("3D Length Bigger Than 31\n"));	}
				u8Edid[u8Ptr++] = u83dLen | (Edid_GetBitCnt(pstEdidInfo->pstVsdbInfo.u8ExtVic) << 5);
				for(u8Cnt1 = 1; u8Cnt1 < 5; u8Cnt1++){
					if(pstEdidInfo->pstVsdbInfo.u8ExtVic & (1<<u8Cnt1)){
						u8Edid[u8Ptr++] = u8Cnt1;
					}
				}

				if(pstEdidInfo->pstVsdbInfo.u83dMulti & 0x80) {
					if(u83dType & 0x80){
						iTE_u8	u83dStruct[8][2] = {{0x00, 0x00}, {0x01, 0x00}, {0x00, 0x40}, {0x01, 0x40}, {0x00, 0x01}, {0x01, 0x01}, {0x00, 0x41}, {0x01, 0x41}};
						u8Edid[u8Ptr++] = u83dStruct[u83dType&0x07][0];
						u8Edid[u8Ptr++] = u83dStruct[u83dType&0x07][1];
						u8Edid[u8Ptr++] = u163dMask >> 8;
						u8Edid[u8Ptr++] = u163dMask & 0xFF;
					}
					u8Cnt1 = 0;
					while(pstEdidInfo->pstVsdbInfo.u163dSbS && (u8Cnt1 < 16)){
						if((1 << u8Cnt1) & pstEdidInfo->pstVsdbInfo.u163dSbS){
							pstEdidInfo->pstVsdbInfo.u163dSbS &= ~(1<<u8Cnt1);
							u8Edid[u8Ptr++] = (u8Cnt1<<4) | 0x08;
							u8Edid[u8Ptr++] = 0x10;
						}
						u8Cnt1++;
					}
					u8Cnt1 = 0;
					while(pstEdidInfo->pstVsdbInfo.u163dTaB && (u8Cnt1 < 16)){
						if((1 << u8Cnt1) & pstEdidInfo->pstVsdbInfo.u163dTaB){
							pstEdidInfo->pstVsdbInfo.u163dTaB &= ~(1<<u8Cnt1);
							u8Edid[u8Ptr++] = (u8Cnt1<<4) | 0x06;
						}
						u8Cnt1++;
					}
					u8Cnt1 = 0;
					while(pstEdidInfo->pstVsdbInfo.u163dFP && (u8Cnt1 < 16)){
						if((1 << u8Cnt1) & pstEdidInfo->pstVsdbInfo.u163dFP){
							pstEdidInfo->pstVsdbInfo.u163dFP &= ~(1<<u8Cnt1);
							u8Edid[u8Ptr++] = (u8Cnt1<<4) | 0x01;
						}
						u8Cnt1++;
					}
				}
			}
		}
	}
	u8Edid[2] = u8Ptr;
	u8Edid[u8Temp] = 0x60 | (u8Ptr - u8Temp -1);

	u8Cnt1 = u8Ptr;
	do{
		u8Temp -= u8Edid[u8Cnt1];
	}while(u8Cnt1--);

	while(u8Ptr < 0x7F){
		u8Edid[u8Ptr++] = 0;
	}

	u8Edid[0x7F] = u8Temp;
}
//****************************************************************************

