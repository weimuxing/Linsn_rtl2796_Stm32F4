///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_1to8.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_1to8.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#include "it6662/include/IT6662_Sys.h"

#if (USING_1to8 == iTE_TRUE)
#define _EDID_TAB_
#include "it6662/include/IT6662_DefaultEdid.h"
#include "it6662/include/IT6662.h"
hgjhjghjghj
#if (defined _MCU_IT6350_)
	#include "it6662/include/IO_IT6350.h"
#endif
//****************************************************************************
extern iTE_u1 g_u1HdcpEn;
extern iTE_u1 bEnHDCP;
extern iTE_u1 g_bCscChk;
extern EdidInfo*	g_stEdidInfo;
extern iTE_u8 g_u8TxSel, g_u8TxOffset, g_u8TxBank, g_u8TxShift;
extern IT6662_Var	*stIt6662;

EdidInfo	g_stTmpEdidInfo;
EdidInfo	g_stComEdidInfo;
extern EdidInfo	*stCurEdidInfo;
iTE_u8	g_u8EdidMode = EDID_DEFAULT;
#if (USING_CEC == iTE_TRUE)
iTE_u8	g_u8TxCecInitA = 0;
iTE_u8	g_u8TxCecInitB = 0;
#endif
extern iTE_u8	_CODE	u8ExtVic[4];
#if (defined _MCU_IT6350_)
extern unsigned int ui1msTick;
#endif
//****************************************************************************
//****************************************************************************
void IT6662_EdidCompose_Min(EdidInfo*	pstSrcEdidInfo);
void IT6662_EdidLoadDefault(iTE_u8 u8EdidMode)
{
	if(u8EdidMode == EDID_DEFAULT_4K2K){
		iTE_Msg(("IT6662_EdidLoadDefault 4k2k\n"));
		Edid_FixTabLoad(u8Edid4K2K, &g_stComEdidInfo);
	}else{
		iTE_Msg(("IT6662_EdidLoadDefault FHD\n"));
		Edid_FixTabLoad(u8EdidFHD, &g_stComEdidInfo);
	}
}
//****************************************************************************
void IT6662_EdidCopy(iTE_u8 u8Port)
{
	EDID_STAT eStatus;
	iTE_u8	u8Cnt, u8Temp;

	stCurEdidInfo = &g_stEdidInfo[u8Port];
	HdmiTx_Bank(g_u8TxBank);
	for(u8Cnt = 0; u8Cnt < EDID_RETRY_MAX; u8Cnt++){
		eStatus = Edid_ExtGet(u8Port);
		u8Temp = HdmiTx_R(0x40 + g_u8TxOffset) & 0x01;
		if((EDID_READ_ERR != eStatus) || u8Temp ){
			break;
		}
	}
	HdmiTx_Bank(0);
//					HdmiTx_SetOutputMode(eStatus);
	if((EDID_READ_ERR == eStatus) && u8Temp ){
		iTE_u8	u8CurDev = g_u8CurDev;
		IT6662_DeviceSelect(IT6662_ROOT);
		stIt6662->u8TxEdidRdFail |= g_u8TxShift;
		IT6662_DeviceSelect(u8CurDev);
		//retry
	}else if(EDID_NO_ERR != eStatus){
		IT6662_EdidLoadDefault(EDID_DEFAULT);
	}//EDID_CEA_ERR, EDID_VSDB_ERR, do nothing
}
//****************************************************************************
void IT6662_EdidCompose(iTE_u8 u8EdidMode)
{
	iTE_u8	u8EdidInit = 0;
	iTE_u8	u8Cnt1, u8Cnt2;
	EdidInfo*	pstSrcEdidInfo;
	iTE_u8	u8RecDev;

	for(u8Cnt1 = 0; u8Cnt1 < 2; u8Cnt1++){
		if(u8Cnt1 == 0)
			IT6662_DeviceSelect(IT6662_A);
		else
			IT6662_DeviceSelect(IT6662_B);

		for(u8Cnt2 = 0; u8Cnt2 < 4; u8Cnt2++){
//			if(stIt6662->u8TxHpdStatus & (0x01 << u8Cnt2)){
				if(g_stEdidInfo[u8Cnt2].pstVsdbInfo.u8PaAdr){
					if(u8EdidInit == 0){
						iTE_pu8	pu8Des = (iTE_pu8)&g_stComEdidInfo;
						iTE_pu8	pu8Cur;
						iTE_u16	u16Size = sizeof(EdidInfo);

						pu8Cur = (iTE_pu8)&g_stEdidInfo[u8Cnt2];
						while(u16Size--){
							*pu8Des++ = *pu8Cur++;
						}
						u8EdidInit = 1;
						u8RecDev = (u8Cnt1 << 4) | (u8Cnt2);
					}else{
						pstSrcEdidInfo = &g_stEdidInfo[u8Cnt2];
						if(EDID_COMPOSE_J == u8EdidMode){
							g_stComEdidInfo.pstVsdbInfo.u83dMulti |= pstSrcEdidInfo->pstVsdbInfo.u83dMulti & 0x80;
						}else{
							g_stComEdidInfo.pstVsdbInfo.u83dMulti &= pstSrcEdidInfo->pstVsdbInfo.u83dMulti & 0x80;
						}
						IT6662_EdidCompose_Min(pstSrcEdidInfo);
						u8EdidInit++;
					}
				}
//			}
		}
	}
#if (USING_COPY_IF_ONE_COMPOSE	== iTE_TRUE)
	if(u8EdidInit == 1){
		if(u8RecDev < 0x10){
			IT6662_DeviceSelect(IT6662_A);
		}else{
			IT6662_DeviceSelect(IT6662_B);
		}

		u8RecDev &= 0x03;
		HdmiTx_Select(u8RecDev);
		IT6662_EdidCopy(u8RecDev);
	}else
#endif
	{
		if(u8EdidInit){
			iTE_u8	u8Edid[0x80];
			iTE_u8	u8TxHpdSta, u8Temp;

	//	add extra Vic for JJ request
			if(EDID_COMPOSE_J == u8EdidMode){
				for(u8Cnt1 = 0; u8Cnt1 < 2; u8Cnt1++){
					if(u8Cnt1 == 0)
						IT6662_DeviceSelect(IT6662_A);
					else
						IT6662_DeviceSelect(IT6662_B);

					for(u8Cnt2 = 0; u8Cnt2 < 4; u8Cnt2++){
						for(u8Temp = 0; u8Temp < 4; u8Temp++){
							g_stComEdidInfo.pstVdbInfo.u8VicList[u8Temp] |=	g_stEdidInfo[u8Cnt2].pstVdbInfo.u8VicList[u8Temp];
						}
//						g_stComEdidInfo.pstVdbInfo.u8VicList[u8Temp] |= g_stEdidInfo[u8Cnt2].pstVdbInfo.u8VicList[u8Temp] & 0x07;
					}
				}
			}
	//	add extra Vic for JJ request
#if 0
			g_stComEdidInfo.pstVdbInfo.u8VicCnt = 0;
			for(u8Cnt1 = 0; u8Cnt1 < 16; u8Cnt1++){
				g_stComEdidInfo.pstVdbInfo.u8VicCnt += Edid_GetBitCnt(g_stComEdidInfo.pstVdbInfo.u8VicList[u8Cnt1]);
			}
#else
			Edid_CalVicCnt(&g_stComEdidInfo.pstVdbInfo);
#endif
			if(g_stComEdidInfo.pstVdbInfo.u8NatList[0] == 0){
				if(g_stComEdidInfo.pstVdbInfo.u8VicCnt){
					if(g_stComEdidInfo.pstVdbInfo.u8Vic3dList[0]){
						g_stComEdidInfo.pstVdbInfo.u8NatList[0] = g_stComEdidInfo.pstVdbInfo.u8Vic3dList[0] | 0x80;
					}else{		// assign Max VIC to Native VIC
						for(u8Cnt1 = 16; u8Cnt1 > 0; ){
							u8Cnt1--;
							if(g_stComEdidInfo.pstVdbInfo.u8VicList[u8Cnt1]){
								for(u8Cnt2 = 8; u8Cnt2 > 0; ){
									u8Cnt2--;
									if(g_stComEdidInfo.pstVdbInfo.u8VicList[u8Cnt1] & (0x01 << u8Cnt2)){
										g_stComEdidInfo.pstVdbInfo.u8Vic3dList[0] = u8Cnt1*8+u8Cnt2;
										g_stComEdidInfo.pstVdbInfo.u8NatList[0] = g_stComEdidInfo.pstVdbInfo.u8Vic3dList[0] | 0x80;
										break;
									}
								}
								break;
							}
						}
					}
				}else{		// No VIC
					g_stComEdidInfo.pstVdbInfo.u8Vic3dList[0] = 0x04;
					g_stComEdidInfo.pstVdbInfo.u8NatList[0] = 0x84;				// 720p60
					g_stComEdidInfo.pstVdbInfo.u8VicList[0] = 0x10;
					g_stComEdidInfo.pstVdbInfo.u8VicCnt = 1;
				}
				g_stComEdidInfo.u8CeaB3 |= 0x01;
			}else{
				for(u8Cnt1 = 0; u8Cnt1 < EDID_NATIVE_MAX; u8Cnt1++){
					if(g_stComEdidInfo.pstVdbInfo.u8NatList[u8Cnt1] == 0){
						break;
					}
				}
				g_stComEdidInfo.u8CeaB3 |= u8Cnt1;
			}

			Edid_InfoShow(&g_stComEdidInfo);
			Edid_RegenBlk1(&g_stComEdidInfo, u8Edid, 1);

	// DTD

			IT6662_DeviceSelect(IT6662_B);
			u8TxHpdSta = stIt6662->u8TxHpdStatus;
			if(u8TxHpdSta == 0){
				IT6662_DeviceSelect(IT6662_A);
				u8TxHpdSta = stIt6662->u8TxHpdStatus;
			}
			for(u8Temp=0; u8Temp<4; u8Temp++){
				if((g_stEdidInfo[u8Temp].pstVsdbInfo.u8PaAdr)&&((1<<u8Temp) & u8TxHpdSta)){
					iTE_u8	u8EdidDtd[18];
					iTE_u8	u8CheckSum=0;
					iTE_u8	u8TmpDtd;
					iTE_u8	u8NoSupDtd = 0;
					iTE_u8	u8Dtd4k2k = 0;
					EDID_STAT eEdidRdSta;
					EdidInfo*	 stTmpEdidInfo =& g_stEdidInfo[u8Temp];
					iTE_u8	u8Cnt, u8Tmp, u8TmpDev;

					HdmiTx_Select(u8Temp);
					HdmiTx_Bank(g_u8TxBank);
					HdmiTx_DdcDisable();
					for(u8Cnt = 0; u8Cnt < EDID_RETRY_MAX; u8Cnt++){
						eEdidRdSta = Edid_ExtGetBlock(0);
						u8Tmp = HdmiTx_R(0x40 + g_u8TxOffset) & 0x01;
						if((EDID_READ_ERR != eEdidRdSta) || u8Tmp ){
							break;
						}
					}
					HdmiTx_Bank(0);

					u8TmpDev = g_u8CurDev;
					IT6662_DeviceSelect(IT6662_C);
					if((EDID_READ_ERR == eEdidRdSta) && u8Tmp ){
						iTE_Msg((" IT6662_EdidComse Edid Block Read Fail  => Retry!! \n"));
						stIt6662->u8TxEdidRdFail |= g_u8TxShift;
						return;
					}else if(EDID_NO_ERR != eEdidRdSta){
						iTE_MsgE((" IT6662_EdidComse Edid Block Read Fail \n"));
						stIt6662->u8TxEdidRdFail |= g_u8TxShift;
						IT6662_DeviceSelect(u8TmpDev);
						stIt6662->u8EdidCheck |= g_u8TxShift;
						IT6662_DeviceSelect(IT6662_C);
						return;
					}

					Edid_Rb(8, 4, u8EdidDtd);
					for(u8Cnt2 = 0; u8Cnt2 < 4; u8Cnt2++){
						u8CheckSum += u8EdidDtd[u8Cnt2] - u8EdidFHD[u8Cnt2+8];
					}
					Edid_Wb(8, 4, &u8EdidFHD[8]);

					u8TmpDtd = stTmpEdidInfo->u8PNameAdr;
					if(u8TmpDtd){
						u8TmpDtd += 5;
						Edid_Rb(u8TmpDtd, 13, u8EdidDtd);
						for(u8Cnt2 = 0; u8Cnt2 < 13; u8Cnt2++){
							u8CheckSum += u8EdidDtd[u8Cnt2] - u8DefDisplayDescriptorPName[u8Cnt2];
						}
						Edid_Wb(u8TmpDtd, 13, u8DefDisplayDescriptorPName);
					}
	// Blk0 Dtd
					for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
						u8TmpDtd = stTmpEdidInfo->u8Blk0Dtd[u8Cnt1];
						if(u8TmpDtd){
							if(g_stComEdidInfo.pstVdbInfo.u8VicList[u8TmpDtd>>3] & (0x01 << (u8TmpDtd & 0x07))){
								if(u8TmpDtd >=  93){
									u8Dtd4k2k = 1;
								}
							}else{
//								g_stEdidInfo[u8Temp].u8Blk0Dtd[u8Cnt1] = 0;
								u8NoSupDtd |= (1<<u8Cnt1);
							}
						}
					}

					u8NoSupDtd |= stTmpEdidInfo->u8UnSupDtd;



					if(u8NoSupDtd){
						while(u8NoSupDtd){
							for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
								if(u8NoSupDtd & (1 << u8Cnt1)){
									u8NoSupDtd &= ~(1<<u8Cnt1);
									Edid_Rb(0x36+(0x12*u8Cnt1), 0x12, u8EdidDtd);
									for(u8Cnt2 = 0; u8Cnt2 < 0x12; u8Cnt2++){
										u8CheckSum += u8EdidDtd[u8Cnt2];
									}
									if((u8Dtd4k2k == 0) && ((g_stComEdidInfo.pstVdbInfo.u8VicList[11] & 0xE0) || g_stComEdidInfo.pstVdbInfo.u8VicList[12] || (g_stComEdidInfo.pstVdbInfo.u8VicList[13] & 0x0F))){
										for(u8Cnt2 = 93; u8Cnt2 < 108; u8Cnt2++){
											if(g_stComEdidInfo.pstVdbInfo.u8VicList[u8Cnt2 >> 3] & (0x01 << (u8Cnt2 & 0x07))){
												u8Temp = Edid_GetDtdDes(u8Cnt2, u8EdidDtd);
												u8Dtd4k2k = 1;
												break;
											}
										}
										if(u8Cnt2 >= 108){
											iTE_MsgE(("Can't found 4k2k support VIC\n"));
											u8Temp = Edid_GetDtdDes(g_stComEdidInfo.pstVdbInfo.u8NatList[0] & 0x7F, u8EdidDtd);
										}
									}else{
										u8Temp = Edid_GetDtdDes(g_stComEdidInfo.pstVdbInfo.u8NatList[0] & 0x7F, u8EdidDtd);
									}
									u8CheckSum -= u8Temp;
									Edid_Wb(0x36+(0x12*u8Cnt1), 0x12, u8EdidDtd);
								}
							}
						}
					}else{
						Edid_Rb(0x36, 0x12, u8EdidDtd);
					}

					u8CheckSum += Edid_R(0x7F);
					Edid_W(0x7F, u8CheckSum);

	// Blk1 Dtd
					u8Cnt2 = u8Edid[2];
					u8Cnt1 = 0;
					u8Temp = 0;
					do{
						u8Temp += u8EdidDtd[u8Cnt1];
						u8Edid[u8Cnt2++] = u8EdidDtd[u8Cnt1++];
					}while(u8Cnt1 < 0x12);
					u8Edid[0x7F] -= u8Temp;
					break;
				}
			}

			Edid_FixTabBlkLoad(u8Edid, 1);
		}else {
			IT6662_EdidLoadDefault(EDID_DEFAULT);
		}
	}
}
//****************************************************************************
void IT6662_EdidCompose_Min(EdidInfo*	pstSrcEdidInfo)
{
	iTE_u8	u8Cnt1, u8Cnt2;
	iTE_u16	u163dSbS, u163dTaB, u163dFP;
	iTE_u8	u8TmpVic, u8FreePos;

	g_stComEdidInfo.u8CeaB3 = g_stComEdidInfo.u8CeaB3 & pstSrcEdidInfo->u8CeaB3 & 0xF0;						// CEA B3

	for(u8Cnt1 = 0; u8Cnt1 < 16; u8Cnt1++){ 																	// VDB VicList
		g_stComEdidInfo.pstVdbInfo.u8VicList[u8Cnt1] &= pstSrcEdidInfo->pstVdbInfo.u8VicList[u8Cnt1];
	}

//	g_stComEdidInfo.pstVsdbInfo.u83dMulti &= pstSrcEdidInfo->pstVsdbInfo.u83dMulti & 0x80;

	u163dSbS = g_stComEdidInfo.pstVsdbInfo.u163dSbS;
	u163dTaB = g_stComEdidInfo.pstVsdbInfo.u163dTaB;
	u163dFP = g_stComEdidInfo.pstVsdbInfo.u163dFP;
	g_stComEdidInfo.pstVsdbInfo.u163dSbS = 0;
	g_stComEdidInfo.pstVsdbInfo.u163dTaB = 0;
	g_stComEdidInfo.pstVsdbInfo.u163dFP = 0;
	u8FreePos = 0;

	for(u8Cnt1=0; u8Cnt1 < EDID_3D_SUPP_MAX; u8Cnt1++){														// VSDB 3D List
#if 0
		for(u8Cnt2=0; u8Cnt2 < EDID_3D_SUPP_MAX; u8Cnt2++){
#if 1
			if(g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1] == pstSrcEdidInfo->pstVdbInfo.u8Vic3dList[u8Cnt2]){
				break;
			}
#else
			u8TmpVic = g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1];
			if(u8TmpVic == pstSrcEdidInfo->pstVdbInfo.u8Vic3dList[u8Cnt2]){
				if(u8TmpVicList[u8TmpVic >> 3] & (1<< (u8TmpVic & 0x07))){
					u8Cnt2 = EDID_3D_SUPP_MAX;
				}
				u8TmpVicList[u8TmpVic >> 3] |= (1<< (u8TmpVic & 0x07));
				break;
			}
#endif
			if(pstSrcEdidInfo->pstVdbInfo.u8Vic3dList[u8Cnt2] ==0){
				u8Cnt2 = EDID_3D_SUPP_MAX;
				break;
			}
		}

		if(u8Cnt2 == EDID_3D_SUPP_MAX){
			g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1] = 0x00;
		}else{
			if(u8FreePos != u8Cnt1){
				g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8FreePos]  = g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1];
				g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1] = 0x00;
			}

			if(g_stComEdidInfo.pstVsdbInfo.u83dMulti){
				if((u163dSbS & (1 << u8Cnt1)) && (pstSrcEdidInfo->pstVsdbInfo.u163dSbS & (1 << u8Cnt2))){
					g_stComEdidInfo.pstVsdbInfo.u163dSbS |= (1 << u8FreePos);
				}
				if((u163dTaB & (1 << u8Cnt1)) && (pstSrcEdidInfo->pstVsdbInfo.u163dTaB & (1 << u8Cnt2))){
					g_stComEdidInfo.pstVsdbInfo.u163dTaB |= (1 << u8FreePos);
				}
				if((u163dFP & (1 << u8Cnt1)) && (pstSrcEdidInfo->pstVsdbInfo.u163dFP & (1 << u8Cnt2))){
					g_stComEdidInfo.pstVsdbInfo.u163dFP |= (1 << u8FreePos);
				}
			}
			u8FreePos++;
		}
#else
		u8TmpVic = g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1];
		if(u8TmpVic){
			iTE_u8	u8Cnt3 = 0;

			for(u8Cnt2 = 0; u8Cnt2 < EDID_3D_SUPP_MAX; u8Cnt2++){
				if(u8TmpVic == pstSrcEdidInfo->pstVdbInfo.u8Vic3dList[u8Cnt2]){
					if(u8FreePos != u8Cnt1){
						g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8FreePos]  = u8TmpVic;
						g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1] = 0x00;
					}
					if(g_stComEdidInfo.pstVsdbInfo.u83dMulti){
						if((u163dSbS & (1 << u8Cnt1)) && (pstSrcEdidInfo->pstVsdbInfo.u163dSbS & (1 << u8Cnt2))){
							g_stComEdidInfo.pstVsdbInfo.u163dSbS |= (1 << u8FreePos);
						}
						if((u163dTaB & (1 << u8Cnt1)) && (pstSrcEdidInfo->pstVsdbInfo.u163dTaB & (1 << u8Cnt2))){
							g_stComEdidInfo.pstVsdbInfo.u163dTaB |= (1 << u8FreePos);
						}
						if((u163dFP & (1 << u8Cnt1)) && (pstSrcEdidInfo->pstVsdbInfo.u163dFP & (1 << u8Cnt2))){
							g_stComEdidInfo.pstVsdbInfo.u163dFP |= (1 << u8FreePos);
						}
					}
					u8Cnt3++;
				}else if(pstSrcEdidInfo->pstVdbInfo.u8Vic3dList[u8Cnt2] == 0){
					break;
				}
			}
			if(u8Cnt3){
				u8FreePos++;
			}else{
				g_stComEdidInfo.pstVdbInfo.u8Vic3dList[u8Cnt1] = 0x00;
			}
		}else{
			break;
		}
#endif
	}

	g_stComEdidInfo.pstVsdbInfo.u8B6 &= pstSrcEdidInfo->pstVsdbInfo.u8B6;								// VSDB B6
	if(g_stComEdidInfo.pstVsdbInfo.u8B7 > pstSrcEdidInfo->pstVsdbInfo.u8B7)								// VSDB B7
		g_stComEdidInfo.pstVsdbInfo.u8B7 = pstSrcEdidInfo->pstVsdbInfo.u8B7;

	if(0){
		g_stComEdidInfo.pstVsdbInfo.u8ExtVic &= pstSrcEdidInfo->pstVsdbInfo.u8ExtVic;							// VSDB 4k2k
	}else{
		g_stComEdidInfo.pstVsdbInfo.u8ExtVic = 0;
		for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
			if(g_stComEdidInfo.pstVdbInfo.u8VicList[u8ExtVic[u8Cnt1] >> 3] & (1 << (u8ExtVic[u8Cnt1] & 0x07))){
				g_stComEdidInfo.pstVsdbInfo.u8ExtVic |= (1 << (u8Cnt1+1));
			}
		}
	}

	g_stComEdidInfo.u8SpkAlloc &= pstSrcEdidInfo->u8SpkAlloc;											// SADB


	u8FreePos = 0;
	for(u8Cnt1 = 0; u8Cnt1 < g_stComEdidInfo.pstAdbInfo.u8AdoCnt; u8Cnt1++){									// ADB
		iTE_Msg(("u8Cnt1[0] = 0x%02X\n", g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][0]));
		for(u8Cnt2 = 0; u8Cnt2 < pstSrcEdidInfo->pstAdbInfo.u8AdoCnt; u8Cnt2++){
		iTE_Msg(("u8Cnt2[0] = 0x%02X\n", pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][0]));
			if((g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][0] & 0x78) == (pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][0] & 0x78)){
				if((g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][0] & 0x07) > (pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][0] & 0x07)){
					g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][0] = pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][0];
				}
				g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][1] &= pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][1];

				if((g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][0] & 0x78) < 0x10){								// ADB FMT 0,1
					g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][2] &= pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][2];
				}else if((g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][0] & 0x78) < 0x48 ){							// ADB FMT 2~8
					if(g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][2] > pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][2]){
						g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][2] = pstSrcEdidInfo->pstAdbInfo.u8AdoDes[u8Cnt2][2];
					}
				}else{																						// ADB FMT 9~
					g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][2] = 0;
				}

				if(u8FreePos != u8Cnt1){
					g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8FreePos][0] = g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][0];
					g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8FreePos][1] = g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][1];
					g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8FreePos][2] = g_stComEdidInfo.pstAdbInfo.u8AdoDes[u8Cnt1][2];
				}
				u8FreePos++;
				break;
			}
		}
	}
	g_stComEdidInfo.pstAdbInfo.u8AdoCnt = u8FreePos;

	u8FreePos = 0;
	for(u8Cnt1 = 0; u8Cnt1 < EDID_NATIVE_MAX; u8Cnt1++){												// Native VIC
		u8TmpVic = g_stComEdidInfo.pstVdbInfo.u8NatList[u8Cnt1];
		if(u8TmpVic){
			for(u8Cnt2 = 0; u8Cnt2 < EDID_NATIVE_MAX; u8Cnt2++){
				if(u8TmpVic == pstSrcEdidInfo->pstVdbInfo.u8NatList[u8Cnt2]){
					if(u8FreePos != u8Cnt1){
						g_stComEdidInfo.pstVdbInfo.u8NatList[u8FreePos] = u8TmpVic;
						g_stComEdidInfo.pstVdbInfo.u8NatList[u8Cnt1] = 0;
					}
					u8FreePos++;
					break;
				}else if(pstSrcEdidInfo->pstVdbInfo.u8NatList[u8Cnt2] == 0){
					u8Cnt2 = EDID_NATIVE_MAX;
					break;
				}
			}
		}else{
			break;
		}
		if(u8Cnt2 == EDID_NATIVE_MAX){
			g_stComEdidInfo.pstVdbInfo.u8NatList[u8Cnt1] = 0;
		}
	}

#if 0
	if(g_stComEdidInfo.pstVdbInfo.u8NatList[0] == 0){
		if(g_stComEdidInfo.pstVdbInfo.u8Vic3dList[0]){
			g_stComEdidInfo.pstVdbInfo.u8NatList[0] = g_stComEdidInfo.pstVdbInfo.u8Vic3dList[0]|0x80;
		}
	}
#endif
}
//****************************************************************************
iTE_u1 IT6662_EdidGet(iTE_u8 u8EdidMode)
{
	iTE_Msg(("IT6662_Edid_Get %d\n", u8EdidMode));
	stCurEdidInfo =&g_stComEdidInfo;
	switch(u8EdidMode){
		case	EDID_DEFAULT_FHD:
		case	EDID_DEFAULT_4K2K:
				IT6662_EdidLoadDefault(u8EdidMode);
			break;
		case	EDID_COPY:
				HdmiTx_Select(EDID_COPY_PORT & 0x03);
				if(stIt6662->u8TxHpdStatus & (0x01 << (EDID_COPY_PORT & 0x03))){
					IT6662_EdidCopy(EDID_COPY_PORT & 0x03);
				}else{
					IT6662_EdidLoadDefault(EDID_DEFAULT);
				}
			break;
		case	EDID_COMPOSE_MIN:
				IT6662_EdidCompose(EDID_COMPOSE_MIN);
			break;
		case	EDID_COMPOSE_J:
				IT6662_EdidCompose(EDID_COMPOSE_J);
			break;
	}
	IT6662_DeviceSelect(IT6662_ROOT);
	if(stIt6662->u8TxEdidRdFail) {		return	iTE_FALSE;	}

	Edid_InfoShow(stCurEdidInfo);
//	g_stCurEdidInfo = &g_stEdidInfo[EDID_COPY_PORT];
#if (USING_EDID_CHG_HPD == iTE_TRUE)
{
	iTE_u1	bStatus = iTE_FALSE;
	iTE_pu8	pu8TmpEdidInfo = (iTE_pu8)&g_stTmpEdidInfo;
	iTE_pu8	pu8CurEdidInfo = (iTE_pu8)stCurEdidInfo;
	iTE_u16	u16Temp = sizeof(EdidInfo);
	while(u16Temp--){
		if(pu8TmpEdidInfo[u16Temp] != pu8CurEdidInfo[u16Temp]){
			pu8TmpEdidInfo[u16Temp] = pu8CurEdidInfo[u16Temp];
			bStatus = iTE_TRUE;
		}
	}

	iTE_Msg(("EDID_CHG = %d\n", bStatus));
	return bStatus;
}
#else
	return iTE_TRUE;
#endif
}
iTE_u8	u8EdidChkPA = 0;
iTE_u8	u8EdidChkPB = 0;
//****************************************************************************
void IT6662_SysEdidCheck(void)
{
#if (USING_CASCADE == iTE_TRUE)

#else
	iTE_u8	u8TxHpdPA, u8TxHpdPB;

	IT6662_DeviceSelect(IT6662_A);
	if(stIt6662->u8TxDoHdcp) {
		iTE_Msg(("A TxDoHdcp\n"));
		return;
	}
	stIt6662->u8TxEdidRdFail = 0;
	u8TxHpdPA = stIt6662->u8TxHpdStatus;
	u8EdidChkPA |= stIt6662->u8EdidCheck;
	if(stIt6662->u8EdidCheck){
	iTE_Msg(("IT6662_SysEdidCheck A %x\n", stIt6662->u8EdidCheck));
		Edid_ExtCheck(HDMI_TX_A);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}
		Edid_ExtCheck(HDMI_TX_B);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}
		Edid_ExtCheck(HDMI_TX_C);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}
		Edid_ExtCheck(HDMI_TX_D);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}

		if(stIt6662->u8TxPortEn){
			if(HdmiTx_R(0x22) & u8TxHpdPA){
				HdmiTx_Set(0x0A, 0x40, 0x00);
			}else{
				HdmiTx_Set(0x0A, 0x40, 0x40);			// disable Tx Packet FIFO error INT when all sink are not HDMI
			}
		}
		stIt6662->u8EdidCheck = 0;

		IT6662_DeviceSelect(IT6662_C);
		HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
		return;
	}

	IT6662_DeviceSelect(IT6662_B);
	if(stIt6662->u8TxDoHdcp){
		iTE_Msg(("B TxDoHdcp\n"));
		return;
	}
	stIt6662->u8TxEdidRdFail = 0;
	u8TxHpdPB = stIt6662->u8TxHpdStatus;
	u8EdidChkPB |= stIt6662->u8EdidCheck;
	if(stIt6662->u8EdidCheck){
	iTE_Msg(("IT6662_SysEdidCheck B %x\n", stIt6662->u8EdidCheck));
		Edid_ExtCheck(HDMI_TX_A);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}
		Edid_ExtCheck(HDMI_TX_B);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}
		Edid_ExtCheck(HDMI_TX_C);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}
		Edid_ExtCheck(HDMI_TX_D);
		if(stIt6662->u8TxEdidRdFail) {	IT6662_DeviceSelect(IT6662_C);		HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}

		if(stIt6662->u8TxPortEn){
			if(HdmiTx_R(0x22) & u8TxHpdPB){
				HdmiTx_Set(0x0A, 0x40, 0x00);
			}else{
				HdmiTx_Set(0x0A, 0x40, 0x40);			// disable Tx Packet FIFO error INT when all sink are not HDMI
			}
		}
		stIt6662->u8EdidCheck = 0;

		IT6662_DeviceSelect(IT6662_C);
		HdmiRx_W(0x84, 0x80 | TIME_BASE);		// Enable Timer 100ms
		return;
	}

#if (USING_CEC == iTE_TRUE)
	g_u8TxCecInitA |=u8EdidChkPA & u8TxHpdPA;
	g_u8TxCecInitB |=u8EdidChkPB & u8TxHpdPB;
#endif

	IT6662_DeviceSelect(IT6662_C);
	stIt6662->u8TxEdidRdFail = 0;

	if(u8EdidChkPA || u8EdidChkPB ){
		iTE_Msg(("IT6662_SysEdidCheck C\n"));
		if(g_u8EdidMode == EDID_COPY){
			iTE_u8	u8Temp;
			switch(EDID_COPY_PORT & 0x30){
				case	0x00:
				case	0x10:
					IT6662_DeviceSelect(IT6662_B);
					u8Temp = u8EdidChkPB;
					break;
				case	0x20:
				case	0x30:
				default:
					IT6662_DeviceSelect(IT6662_A);
					u8Temp = u8EdidChkPA;
					break;
			}
			if(u8Temp & (1<<(EDID_COPY_PORT&0x03))){
				if((EdidUpdateTxChange_Copy) || (stIt6662->u8TxHpdStatus & (1<<(EDID_COPY_PORT&0x03)))){
					if(IT6662_EdidGet(EDID_COPY)){
						IT6662_DeviceSelect(IT6662_C);
						stIt6662->u8TxTrgRxHpd |= 0x80;
					}
				}
			}
		}else if((g_u8EdidMode == EDID_COMPOSE_MIN) ||(g_u8EdidMode == EDID_COMPOSE_J)){
			iTE_Msg(("****IT6662 %x, %x, %x, %x\n",u8TxHpdPA, u8EdidChkPA,  u8TxHpdPB, u8EdidChkPB));
			if((EdidUpdateTxChange_Compose) || (u8TxHpdPA & u8EdidChkPA) || (u8TxHpdPB & u8EdidChkPB)){
				if(IT6662_EdidGet(g_u8EdidMode)){		//ToDo
					IT6662_DeviceSelect(IT6662_C);
					stIt6662->u8TxTrgRxHpd |= 0x80;
				}
			}
		}

		IT6662_DeviceSelect(IT6662_C);
		if(stIt6662->u8TxEdidRdFail) {	HdmiRx_W(0x84, 0x80 | TIME_BASE);		return;			}

		if(((stIt6662->u8TxTrgRxHpd & 0x80) ||(u8TxHpdPA & u8EdidChkPA) || (u8TxHpdPB & u8EdidChkPB))){
			iTE_u8	u8VsdbB6Adr = (stCurEdidInfo->pstVsdbInfo.u8PaAdr + 2) | 0x80;
			if(u8VsdbB6Adr < Edid_R(0x82)){
				iTE_u8	u8TmpB6 = 0xFF;

				if(u8TxHpdPA & u8EdidChkPA){
					IT6662_DeviceSelect(IT6662_A);
					u8TmpB6 &= Edid_ColorDepthCheck();
				}
				if(u8TxHpdPB & u8EdidChkPB){
					IT6662_DeviceSelect(IT6662_B);
					u8TmpB6 &= Edid_ColorDepthCheck();
				}
				if(u8TmpB6 != 0xFF){
					IT6662_DeviceSelect(IT6662_C);
					if(Edid_ColorDepthChange(u8TmpB6, u8VsdbB6Adr)){
						stIt6662->u8TxTrgRxHpd |= 0x80;
					}
				}
			}
		}
	}

	if(u8EdidChkPA || g_bCscChk){
		iTE_u8	u8AVMuteState;

		IT6662_DeviceSelect(IT6662_A);
		u8AVMuteState = HdmiTx_R(0x23);
		HdmiRx_CscOutputSet(HdmiTx_CscOutputSet());
		HdmiTx_W(0x23, u8AVMuteState);
		u8EdidChkPA = 0;
	}

	if(u8EdidChkPB || g_bCscChk){
		iTE_u8	u8AVMuteState;

		IT6662_DeviceSelect(IT6662_B);
		u8AVMuteState = HdmiTx_R(0x23);
		HdmiRx_CscOutputSet(HdmiTx_CscOutputSet());
		HdmiTx_W(0x23, u8AVMuteState);
		u8EdidChkPB = 0;
	}

	g_bCscChk = 0;
#endif
}
//****************************************************************************
void IT6662_SysRxHpdTrg(void)
{
#if (USING_CASCADE == iTE_TRUE)
	IT6662_DeviceSelect(IT6662_A);
	IT6662_RxHpdTrg();
	IT6662_DeviceSelect(IT6662_B);
	IT6662_RxHpdTrg();
	IT6662_DeviceSelect(IT6662_C);
	IT6662_RxHpdTrg();
#else
	iTE_u8	u8TmpTxTrgRxHpd = 0;
	iTE_u8	u8TmpTxHpdStatus = 0;

	if(u8EdidChkPA || u8EdidChkPB)
		return;
	IT6662_DeviceSelect(IT6662_A);
	u8TmpTxHpdStatus |= stIt6662->u8TxHpdStatus;
	if(stIt6662->u8TxTrgRxHpd){
		u8TmpTxTrgRxHpd |= 1;
		IT6662_ClrHpdCnt();
	}

#if 1	// move HdmiTx_PreSpenHdcp from HdmiTx_PortEnable to here
	if(stIt6662->u8TxSpenHdcp){
//		iTE_Sleep_ms(10);
		iTE_u8	u8Tmp;
		for(u8Tmp = 0; u8Tmp < 0x04; u8Tmp++){
			if(stIt6662->u8TxSpenHdcp & (1<<u8Tmp)){
				HdmiTx_Select(u8Tmp);
//				stIt6662->u8HDCPFireCnt[g_u8TxSel]=0;
				HdmiTx_PreSpenHdcp(iTE_TRUE);
			}
		}
	}
#endif

	IT6662_DeviceSelect(IT6662_B);
	u8TmpTxHpdStatus |= stIt6662->u8TxHpdStatus;
	if(stIt6662->u8TxTrgRxHpd){
		u8TmpTxTrgRxHpd |= 2;
		IT6662_ClrHpdCnt();
	}
#if 1	// move HdmiTx_PreSpenHdcp from HdmiTx_PortEnable to here
	if(stIt6662->u8TxSpenHdcp){
//		iTE_Sleep_ms(10);
		iTE_u8	u8Tmp;
		for(u8Tmp = 0; u8Tmp < 0x04; u8Tmp++){
			if(stIt6662->u8TxSpenHdcp & (1<<u8Tmp)){
				HdmiTx_Select(u8Tmp);
//				stIt6662->u8HDCPFireCnt[g_u8TxSel]=0;
				HdmiTx_PreSpenHdcp(iTE_TRUE);
			}
		}
	}
#endif
	IT6662_DeviceSelect(IT6662_C);

	if(stIt6662->u8TxTrgRxHpd || u8TmpTxTrgRxHpd){		// if EDID change or Tx port inserted
		iTE_Msg(("IT6662_RxHpdTrg_8 %02X %02X\n", stIt6662->u8TxTrgRxHpd, u8TmpTxTrgRxHpd));
		if(stIt6662->bRxHpdStatus){
#if (USING_REPEATER == iTE_TRUE)
			HdmiRx_TrgHpd(10);
#else
			if(stIt6662->u8TxTrgRxHpd & 0x80){
				HdmiRx_TrgHpd(10);
			}
#endif
		}else{
			HdmiRx_TrgHpd(1);
		}
	}else{
		if(stIt6662->bRxHpdStatus && (u8TmpTxHpdStatus == 0)){
			HdmiRx_HpdClr(0);
		}
	}
	IT6662_ClrHpdCnt();
#endif
}
//****************************************************************************
void IT6662_SysSetEdidMode(iTE_u8 u8EdidMode)
{
	g_u8EdidMode = u8EdidMode;
}
//****************************************************************************
void IT6662_SysSetLED(void)
{
	iTE_u16	u16Temp, u16LedSet;

	IT6662_DeviceSelect(IT6662_A);
	u16LedSet =  (IT6662_GetReg(HDMI_TX_ADR, 0x19) & 0x0F) << 4;
	IT6662_DeviceSelect(IT6662_B);
	u16LedSet |=  (IT6662_GetReg(HDMI_TX_ADR, 0x19) & 0x0F);
	IT6662_DeviceSelect(IT6662_C);
	u16Temp = (IT6662_GetReg(HDMI_RX_ADR, 0x0A) & 0x80);
	u16LedSet |= u16Temp << 1;
	if(u16Temp){
		if((IT6662_GetReg(HDMI_RX_ADR, 0x93) & 0x01) && ((IT6662_GetReg(HDMI_RX_ADR, 0xA8) & 0x02) == 0)){
			u16LedSet |= 0x0200;
		}
	}

	iTE_GPO_Set(u16LedSet);
//	printf("LED = 0x%04X\n", u16LedSet);
}
//****************************************************************************
void IT6662_SysEdidModeSelect(void)
{
#if (defined _MCU_IT6350_)
	iTE_u8	u8Temp;

	u8Temp = GPDRG & 0x80;
	u8Temp |= GPDRC & 0x20;

	switch(u8Temp & 0xA0){
		case	KEY_EDID_MODE_FHD:
				IT6662_SysSetEdidMode(EDID_DEFAULT_FHD);
			break;
		case	KEY_EDID_MODE_COPY:
				IT6662_SysSetEdidMode(EDID_COPY);
			break;
		case	KEY_EDID_MODE_COMPOSE:
				IT6662_SysSetEdidMode(EDID_COMPOSE_J);
			break;
		case	KEY_EDID_MODE_UHD:
				IT6662_SysSetEdidMode(EDID_DEFAULT_4K2K);
			break;
	}
#else
	IT6662_SysSetEdidMode(EDID_COMPOSE_J);
#endif
}
//****************************************************************************
iTE_u1 IT6662_SysInit(void)
{
	iTE_Msg(("1 to 8 splitter\n"));

	IT6662_DeviceSelect(IT6662_A);
	if(IT6662_Init()){
		IT6662_SetReg(HDMI_RX_ADR, 0x27, DefaultEQA);
		IT6662_DeviceSelect(IT6662_B);
		if(IT6662_Init()){
			IT6662_SetReg(HDMI_RX_ADR, 0x27, DefaultEQB);
			IT6662_DeviceSelect(IT6662_C);
			if(IT6662_Init()){
#if (USING_CASCADE == iTE_FALSE)
				HdmiTx_Select(HDMI_TX_B);
				HdmiTx_PortEnable(1);
				HdmiTx_Select(HDMI_TX_C);
				HdmiTx_PortEnable(1);
#endif
				IT6662_SysEdidModeSelect();
				if(iTE_TRUE == EnIntEDID){
					IT6662_EdidGet(	g_u8EdidMode);
				}
				MhlRx_Enable();
				return iTE_TRUE;
			}
		}
	}
	return iTE_FALSE;

}
//****************************************************************************
#if (USING_CEC == iTE_TRUE)
void IT6662_CecChangeParameter(void)
{
	extern	stCecVar *g_stCecCur;
	iTE_u8	u8Follower;

	u8Follower = Cec_R(0x10) & 0x0F;
	Cec_W(0x10,  u8Follower | g_stCecCur->u8MyLogAdr << 4);

	switch(Cec_R(0x11)){
		case	eActiveSource:
			Cec_W(0x12, g_stCecCur->u8PaL);
			Cec_W(0x13, g_stCecCur->u8PaH);
			break;
	}
}
//****************************************************************************
iTE_u1 IT6662_DevTxCecHandler(iTE_u8 u8DevSel)
{
	iTE_u8	u8Temp;
	extern	stCecVar *g_stCecCur;

	IT6662_DeviceSelect(u8DevSel);
	Cec_BlockSel(CEC_TX_SEL);
	if(g_stCecCur->bTxCecDone){
		g_stCecCur->u8TxCecInitDone &= stIt6662->u8TxHpdStatus;
		g_stCecCur->u8TxCecFire &= stIt6662->u8TxHpdStatus;
		if(g_stCecCur->u8TxCecFire){
			for(u8Temp = 0; u8Temp < 4; u8Temp++){
				if(g_stCecCur->u8TxCecFire & (0x01 << u8Temp)){
					g_stCecCur->u8TxCecFire &= ~(0x01 << u8Temp);
					Cec_TxSel(u8Temp);
					IT6662_CecChangeParameter();
					Cec_TxFire();
					return iTE_FALSE;
				}
			}
		}
		if(g_u8TxCecInitA){
			for(u8Temp = 0; u8Temp < 4; u8Temp++){
				if(g_u8TxCecInitA & (0x01 << u8Temp)){
					g_u8TxCecInitA &= ~(0x01 << u8Temp);
					Cec_TxSel(u8Temp);
					//Cec_TxPolling(IT6662_TX_LA);
					Cec_TxPolling(g_stCecCur->u8MyLogAdr);
					return iTE_FALSE;
				}
			}
		}
		Cec_TxSel(IT6662_DEFAULT_CEC);
		return iTE_TRUE;
	}
	return iTE_FALSE;
}
void IT6662_DevTxCecHandlerCmd(iTE_u8 u8DevSel, iTE_pu8 pu8Reg10)
{
	iTE_u8	u8Temp;
	extern	stCecVar *g_stCecCur;

	IT6662_DeviceSelect(u8DevSel);
	Cec_Wb(0x10, 20, pu8Reg10);
	g_stCecCur->u8TxCecFire = g_stCecCur->u8TxCecInitDone;
	for(u8Temp = 0; u8Temp < 4; u8Temp++){
		if(g_stCecCur->u8TxCecFire & (0x01 << u8Temp)){
			g_stCecCur->u8TxCecFire &= ~(0x01 << u8Temp);
			Cec_TxSel(u8Temp);
			IT6662_CecChangeParameter();
			Cec_TxFire();
			return;
		}
	}
}
void IT6662_TxCecHandler(void)
{
	iTE_u1	bStatus;

	bStatus = IT6662_DevTxCecHandler(IT6662_A);
	bStatus &= IT6662_DevTxCecHandler(IT6662_B);

	if(bStatus){
		IT6662_DeviceSelect(IT6662_C);
		if(Cec_TxCmdPull()){
			iTE_u8	u8CecReg10[20];
			Cec_Rb(0x10, 20, u8CecReg10);

			IT6662_DevTxCecHandlerCmd(IT6662_A, u8CecReg10);
			IT6662_DevTxCecHandlerCmd(IT6662_B, u8CecReg10);
		}
	}
}
#endif
//****************************************************************************
void IT6662_SysIrq(void)
{
	IT6662_DeviceSelect(IT6662_C);
	IT6662_Irq();
	IT6662_DeviceSelect(IT6662_B);
	IT6662_Irq();
	IT6662_DeviceSelect(IT6662_A);
	IT6662_Irq();

	IT6662_SysEdidCheck();
	IT6662_SysRxHpdTrg();
#if (USING_CEC == iTE_TRUE)
	IT6662_TxCecHandler();
#endif
	IT6662_SysSetLED();
#if (defined _MCU_IT6350_)
//	iTE_Msg(("1msTick = %d \n", ui1msTick));
#endif
	iTE_ExtIntEn();
}
#endif
