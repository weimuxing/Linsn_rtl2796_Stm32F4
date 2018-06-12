///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Edid.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Edid.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _EDID_H_
#define _EDID_H_
#include "it6662/include/IT6662_IO.h"

#define EDID_NATIVE_MAX	(4)
#define EDID_3D_SUPP_MAX	(16)
#define EDID_BLK0_DTD_MAX	(4)
#define EDID_BLK1_DTD_MAX	(6)

#define PARSE_CEA_VDB		(iTE_TRUE)
#define PARSE_CEA_ADB		(iTE_TRUE)
#define SAVE_VIC_LIST		(iTE_TRUE)

#define EDID_BLK0_DTD_MAX	(4)
#define EDID_BLK1_DTD_MAX	(6)
#define EDID_VIC_LIST_MAX	(16)
#define DEFAULT_3D_CNT	(5)

#define EDID_READ_MAX	(32)
#define EDID_READ_MIN	(4)
#if (EDID_READ_MAX < EDID_READ_MIN)
	#pragma message("****ERROR****: EDID read size must bigger than EDID_READ_MIN")
#endif

typedef enum _EDID_STAT{
	EDID_READ_ERR,
	EDID_HEADER_ERR,
	EDID_VSDB_ERR,
	EDID_CEA_ERR,
	EDID_NO_ERR
}EDID_STAT;

typedef enum _CEA_TAG{
	CEA_TAG_NONE 		= 0x00,
	CEA_TAG_ADB		= 0x20,
	CEA_TAG_VDB		= 0x40,
	CEA_TAG_VSDB		= 0x60,
		CEA_VSDB_ID		= CEA_TAG_VSDB,
		CEA_VSDB_PA		= CEA_TAG_VSDB + 1,
		CEA_VSDB_CLR_DEP	= CEA_TAG_VSDB + 2,
		CEA_VSDB_TMDS_CLK = CEA_TAG_VSDB + 3,
		CEA_VSDB_HDMI_PRE = CEA_TAG_VSDB + 4,
		CEA_VSDB_LATENCY	= CEA_TAG_VSDB + 5,
		CEA_VSDB_3D		= CEA_TAG_VSDB + 6,
		CEA_VSDB_4K2K		= CEA_TAG_VSDB + 7,
		CEA_VSDB_STRUCT	= CEA_TAG_VSDB + 8,
		CEA_VSDB_3D_VIC	= CEA_TAG_VSDB + 9,
		CEA_VSDB_DONE		= CEA_TAG_VSDB +0x1F,
	CEA_MODE_SPK		= 0x80,
	CEA_MODE_DTC		= 0xA0,
	CEA_MODE_REV		= 0xC0,
	CEA_MODE_EXT		= 0xE0
}CeaTag;

typedef struct _VSDB_INFO{
	iTE_u8	u8PaAdr;									// Position of HDMI physical address
	iTE_u8	u8VsdbPaL, u8VsdbPaH;						// HDMI physical address
	iTE_u8	u8B6, u8B7;									// VSDB Byte6, Byte7
	iTE_u8	u83dMulti;									// VSDB 3D present Byte
	iTE_u8	u83dLen;
	iTE_u8	u8ExtVic;									// Extended Timing
	iTE_u16	u163dSbS, u163dTaB, u163dFP;				// 3D information
}VsdbInfo;

typedef struct _VDB_INFO{
#if (SAVE_VIC_LIST == iTE_TRUE)
	iTE_u8	u8NatList[EDID_NATIVE_MAX];					// VDB native VIC number list
	iTE_u8	u8VicList[EDID_VIC_LIST_MAX];								// VDB VIC supported index list
	iTE_u8	u8Vic3dList[EDID_3D_SUPP_MAX];				// VDB first 16 VIC number List
#endif
	iTE_u8	u8VicCnt;									// VIC count
	iTE_u8	u8NatCnt;
//	iTE_u8	u8VicStart;
}VdbInfo;

typedef struct _ADB_INFO{
	iTE_u8	u8AdoCnt;									// Short Audio Descriptors count
//	iTE_u8	u8SupFmt;
	iTE_u8	u8AdoDes[15][3];								// Short Audio Descriptors
}AdbInfo;

typedef struct _EDID_PARSE_STR{
	iTE_u8	u8RdPtr;
//	iTE_u8	u8RdStart;
	iTE_s8	s8Length;
	CeaTag	eTag;
	iTE_pu8	pu8RdBuf;
}EdidParStr;

typedef struct _EDID_INFOR_STR{
	iTE_u8	u8ExtBlkCnt;									// EDID extern block count, EDID[0x7E]
	iTE_u8	u8Blk0Dtd[EDID_BLK0_DTD_MAX];				// Blcok0's DTD data
	iTE_u8	u8UnSupDtd;
	iTE_u8	u8CeaB3;									// Byte3 of EDID CEA Block
	VdbInfo	pstVdbInfo;									// VDB information
#if (PARSE_CEA_ADB == iTE_TRUE)
	AdbInfo	pstAdbInfo;									// ADB information
	iTE_u8	u8SpkAlloc;									// SADB information
#endif
	VsdbInfo	pstVsdbInfo;									// VSDB informtaion
	iTE_u8	u8Blk1Dtd[EDID_BLK1_DTD_MAX];				// DTD data of EDID CEA Block
	iTE_u8	u8Default3DVic[DEFAULT_3D_CNT];
	iTE_u8	u8PNameAdr;
}EdidInfo;

typedef struct _MHL_3D_INFO_STR{
	iTE_u8	u8VicCnt;
	iTE_u16	u163dSbS, u163dTaB, u163dFP;
	iTE_u8	u8Default3DVic[DEFAULT_3D_CNT];
	iTE_u8	u83dMulti;
}Mhl3dInfo;

#define EDID_INFO_STR_SIZE	(sizeof(EdidInfo))

void Edid_ParseInit(EdidInfo *pstEdidInfo);
void Edid_FixTabLoad(iTE_pu8 pu8EdidTab, EdidInfo *pstEdidInfo );
void Edid_FixTabBlkLoad(iTE_pu8 pu8EdidTab, iTE_u8 u8BlkNo);
EDID_STAT Edid_FixTabCeaParse(iTE_u8 *u8EdidBuf, EdidInfo *pstEdidInfo, iTE_u1 bPDefault3DVic);
EDID_STAT Edid_ExtGetBlock(iTE_u8 u8EdidBlk);
EDID_STAT Edid_ExtSaveBlock(iTE_u8 u8EdidBlk);
void Edid_ExtCheck(iTE_u8 u8TxSel);
EDID_STAT Edid_ExtParse(EdidInfo *pstEdidInfo);
EDID_STAT Edid_ExtGet(iTE_u8 u8HdmiTxPort);
EDID_STAT Edid_ExtBlk0Parse(EdidInfo *pstEdidInfo);
EDID_STAT Edid_ExtCeaParse(iTE_u8 u8EdidBlk, EdidInfo *pstEdidInfo, iTE_u1 bPDefault3DVic);
iTE_u8 Edid_ColorDepthCheck(void);
iTE_u1 Edid_ColorDepthChange(iTE_u8 u8VsdbB6, iTE_u8 u8VsdbB6Adr);
void Edid_InfoShow(EdidInfo *pstEdidInfo);
iTE_u8 Edid_GetDtdDes(iTE_u8 u8Vic, iTE_pu8 pu8DtdDes);
EDID_STAT Edid_DtdParse(iTE_u8 u8RdStart, EdidInfo *pstEdidInfo,  iTE_u16 u16EdidBlk, iTE_pu8 pu8EdidBuf);
void Edid_CeaPrase(EdidInfo *pstEdidInfo, EdidParStr *pstEdidParPtr, iTE_pu8 u8EdidBuf, iTE_u1 bPDefault3DVic);
void Edid_Dtd2Vic(EdidInfo *pstEdidInfo);
void Edid_VicConvert(iTE_u8 u8Vic, VdbInfo *pstVdbInfo);
void Edid_ParseVSDB(EdidParStr *pstEdidParPtr, VsdbInfo *pstVsdbInfo);
void Edid_GetDefault3DVic(EdidInfo *pstEdidInfo, iTE_u8 u8Vic, iTE_u8 u8VicIndex);
iTE_u8 Edid_GetBitCnt(iTE_u16 u16Word);
void Edid_CalVicCnt(VdbInfo *pstVdbInfo);
iTE_u16 Edid_Analyze3dType(iTE_pu16 pu16SbS, iTE_pu16 pu16TaB, iTE_pu16 pu16FP, iTE_pu8 pu8Type, iTE_pu8 pu8Len);
void Edid_RegenBlk1(EdidInfo *pstEdidInfo, iTE_pu8 u8Edid, iTE_u1 bPDefault3DVic);

#define Edid_Wb(u8Offset, u8Count, pu8Data)	iTE_I2C_WriteBurst(EDID_ADR, u8Offset, u8Count, pu8Data)
#define Edid_Rb(u8Offset, u8Count, pu8Data)	iTE_I2C_ReadBurst(EDID_ADR, u8Offset, u8Count, pu8Data)
#define Edid_Set(u8Offset, u8InvMask, u8Data)	iTE_I2C_SetByte(EDID_ADR, u8Offset, u8InvMask, u8Data)
#define Edid_W(u8Offset, u8Data)				iTE_I2C_WriteByte(EDID_ADR, u8Offset, u8Data)
#define Edid_R(u8Offset)						iTE_I2C_ReadByte(EDID_ADR, u8Offset)
#endif
