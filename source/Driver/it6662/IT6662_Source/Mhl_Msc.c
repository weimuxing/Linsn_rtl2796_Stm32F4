///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Msc.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Mhl_Msc.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#define _MHL_MSC_INIT_TAB_
#include "it6662/include/Mhl_Msc.h"
#include "it6662/include/Mhl_RcpUcpHandler.h"
#include "it6662/include/IT6662_DefaultEdid.h"
#include "it6662/include/ITE_Edid.h"
PARSE3D_STR	st3DParse = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

iTE_u8 u8MscFireCnt = 0;
MSC_WB_INFO stTxWbInfo;

extern MSC_CMD TxMscCmd;
EdidInfo *stCurEdidInfo = NULL;
iTE_u8 _CODE u8Default3DVicFmt[DEFAULT_3D_CNT] = {0x03, 0x04, 0x03, 0x04, 0x03};
iTE_u8	_CODE MscFire[17][2] = {		{0x50, 0x80},		{0x50, 0x80},		{0x50, 0x40},		{0x50, 0x01},
	{0x50, 0x02},		{0x50, 0x04},		{0x50, 0x08},		{0x51, 0x02},		{0x50, 0x10},		{0x50, 0x20},
	{0x51, 0x01},		{0x51, 0x80},		{0x51, 0x80},		{0x51, 0x80},		{0x51, 0x80},		{0x51, 0x80},
	{0x51, 0x80},
};

iTE_ps8 _CODE pucRcpString[0x80]={
	"Select", 			"Up",	 		"Down", 				"Left", 			"Right", 			"Right-Up", 			"Right-Down",		"Left-Up",
	"Left-Down", 		"Root Menu",	 	"Setup Menu", 		"Contents Menu", 	"Favorite Menu", 	"Exit",				NULL,			NULL,
	NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
	NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
	"Numeric 0",		"Numeric 1",		"Numeric 2",			"Numeric 3",		"Numeric 4",		"Numeric 5",			"Numeric 6",		"Numeric 7",
	"Numeric 8",		"Numeric 9",		"Dot",				"Enter",			"Clear",			NULL,				NULL,			NULL,
	"Channel Up",		"Channel Down",	"Previous Channel",	"Sound Select",	"Input Select", 	"Show Information",	"Help",			"Page Up",
	"Page Down",		NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
	NULL,			"Volume Up",		"Volume Down",		"Mute",			"Play",			"Stop",				"Pause",			"Record",
	"Rewind",			"Fast Forward",	"Eject",				"Forward",		"Backward",		NULL,				NULL,			NULL,
	"Angle",			"Subpicture",		NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
	NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
	"Play Function",	"Pause Play Function",	"Record Function",	"Pause Record Function",	"Stop",	"Mute",				"Restore Volume Funation",	"Tune Function",
	"Select Media Function", NULL,		NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
	NULL,			"F1 (Blue)",		"F2 (Red)",			"F3 (Green)",		"F4 (Yellow)",		"F5",					NULL,			NULL,
	NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				"Vendor Specific",	NULL
};

MSC_STATUS MHL_MscReqFailCheck(iTE_u8 *ucMHL18)
{
	MSC_STATUS eMscResult=MSC_FAIL;

	iTE_MRx_Msg(("MSC Req Fail Interrupt (Unexpected) ... \n"));
	if(ucMHL18[0]&0x01 ) {	iTE_MRx_Msg(("ERROR: Incomplete Packet !!!\n"));	             }
	if(ucMHL18[0]&0x02 ) {	iTE_MRx_Msg(("ERROR: MSC 100ms TimeOut !!!\n"));	             }
	if(ucMHL18[0]&0x04 ) {	iTE_MRx_Msg(("ERROR: Protocol Error !!!\n"));		             }
	if(ucMHL18[0]&0x08 ) {	iTE_MRx_Msg(("ERROR: Retry > 32 times !!!\n"));		             }
	if(ucMHL18[0]&0x10 ) {
		iTE_MRx_Msg(("ERROR: Receive ABORT Packet !!!\n"));
		iTE_Sleep_ms(WAITABORTNUM);
		eMscResult = MSC_RCVABORT;
	}
	if(ucMHL18[0]&0x20 ) {
		iTE_MRx_Msg(("MSC_MSG Requester Receive NACK Packet !!! \n"));
		eMscResult = MSC_RCVNACK;
	}
	if(ucMHL18[0]&0x40 ) {
		iTE_MRx_Msg(("***************************************************************\n"));
		iTE_MRx_Msg(("Disable HW Retry and MSC Requester Arbitration Lose at 1st Packet !!! \n"));
		eMscResult = MSC_ARBLOSE;
//		MHL_Reset();
	}
	if(ucMHL18[0]&0x80 ) {
		iTE_MRx_Msg(("***************************************************************\n"));
		iTE_MRx_Msg(("Disable HW Retry and MSC Requester Arbitration Lose before 1st Packet !!!\n"));
		eMscResult = MSC_ARBLOSE;
//		MHL_Reset();
	}
	if(ucMHL18[1]&0x01 ) {
		iTE_MRx_Msg(("ERROR: TX FW Fail in the middle of the command sequence !!!\n"));
		eMscResult = MSC_FWTXFAIL;
	}
	if(ucMHL18[1]&0x02 ) {
		iTE_MRx_Msg(("ERROR: TX Fail because FW mode RxPktFIFO not empty !!!\n"));
		eMscResult = MSC_FWRXPKT;
	}

	return eMscResult;
}
//****************************************************************************
void Msc_Read_DevCap(void)
{
	iTE_u8	ucOffset, ucTemp;

	for(ucOffset=0; ucOffset<0x10; ucOffset++){
		TxMscCmd.ucOffset = ucOffset;

		if(MSC_SUCCESS == Msc_Fire(MSC_PKT_READ_DEVCAP, &TxMscCmd)){
			ucTemp = MhlRx_R(0x56);
			if((3 == ucOffset) || (4 == ucOffset)){
				MhlRx_W(0x5B+ucOffset, ucTemp);
			}
#if iTE_MhlRx_Msg_Print
			Msc_ParseDevCap(ucOffset, ucTemp);
#endif
		}
	}
}
//****************************************************************************
#if iTE_MhlRx_Msg_Print
void Msc_ParseDevCap(iTE_u8 ucOffset, iTE_u8 ucData)
{
	switch(ucOffset){
		case	0:
				iTE_MRx_Msg(("DEV_STATE=0x%x\n", (int)ucData));
			break;
		case	1:
				iTE_MRx_Msg(("MHL_Version=0x%x\n",(int)ucData));
			break;
		case	2:
				switch(ucData & 0xF){
					case	0:	iTE_MRx_Msg(("ERROR: DEV_TYPE at least one bit must be set !!!\n"));	break;
					case	1:	iTE_MRx_Msg(("DEV_TYPE = Sink, "));								break;
					case	2:	iTE_MRx_Msg(("DEV_TYPE = Source, "));								break;
					case	3:	iTE_MRx_Msg(("DEV_TYPE = Dongle, "));								break;
					default:		iTE_MRx_Msg(("ERROR: Reserved for future use !!! "));				break;
				}
				iTE_MRx_Msg(("POW=%d, PLIM=%d \n", (int)((ucData&0x10)>>4), (int)((ucData&0x60)>>5)));
			break;
		case	3:
				iTE_MRx_Msg(("ADOPTER_ID_H=0x%X, ", (int)ucData));
			break;
		case	4:
				iTE_MRx_Msg(("ADOPTER_ID_L=0x%X \n",(int)ucData));
			break;
		case	5:
				iTE_MRx_Msg(("\nVID_LINK_MODE:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	SUPP_RGB444\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	SUPP_YCBCR444\n"));
				if(ucData&0x04)	iTE_MRx_Msg(("	SUPP_YCBCR422\n"));
				if(ucData&0x08)	iTE_MRx_Msg(("	SUPP_PPIXEL\n"));
				if(ucData&0x10)	iTE_MRx_Msg(("	SUPP_ISLANDS\n"));
				if(ucData&0x20)	iTE_MRx_Msg(("	SUPP_VGA\n"));
			break;
		case	6:
				iTE_MRx_Msg(("\nAUD_LINK_MODE:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	SUPP_2CH\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	SUPP_8CH\n"));
			break;
		case	7:
				if(ucData & 0x80){
					iTE_MRx_Msg(("\nVIDEO_TYPE:\n"));
					if(ucData&0x01)	iTE_MRx_Msg(("	VT_GRAPHICS\n"));
					if(ucData&0x02)	iTE_MRx_Msg(("	VT_PHOTO\n"));
					if(ucData&0x04)	iTE_MRx_Msg(("	VT_CINEMA\n"));
					if(ucData&0x08)	iTE_MRx_Msg(("	VT_GAME\n"));
				}else{
					iTE_MRx_Msg(("Not Support VIDEO_TYPE\n"));
				}
			break;
		case	8:
				iTE_MRx_Msg(("\nLOG_DEV_MAP:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	LD_DISPLAY\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	LD_VIDEO\n"));
				if(ucData&0x04)	iTE_MRx_Msg(("	LD_AUDIO\n"));
				if(ucData&0x08)	iTE_MRx_Msg(("	LD_MEDIA\n"));
				if(ucData&0x10)	iTE_MRx_Msg(("	LD_TUNER\n"));
				if(ucData&0x20)	iTE_MRx_Msg(("	LD_RECORD\n"));
				if(ucData&0x40)	iTE_MRx_Msg(("	LD_SPEAKER\n"));
				if(ucData&0x80)	iTE_MRx_Msg(("	LD_GUI\n"));
			break;
		case	9:
				iTE_MRx_Msg(("\nBANDWIDTH= %dMHz\n", (int)ucData*5));
			break;
		case	10:
				iTE_MRx_Msg(("FEATURE_FLAG:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	RCP_SUPPORT\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	RAP_SUPPORT\n"));
				if(ucData&0x04)	iTE_MRx_Msg(("	SP_SUPPORT\n"));
				if(ucData&0x08)	iTE_MRx_Msg(("	UCP_SEND_SUPPORT\n"));
				if(ucData&0x10)	iTE_MRx_Msg(("	UCP_RECV_SUPPORT\n"));
			break;
		case	11:
				iTE_MRx_Msg(("DEVICE_ID_H=0x%x, ", (int)ucData));
			break;
		case	12:
				iTE_MRx_Msg(("DEVICE_ID_L=0x%x\n", (int)ucData));
			break;
		case	13:
				iTE_MRx_Msg(("SCRATCHPAD_SIZE=%d Bytes\n", (int)ucData));
			break;
		case	14:
				iTE_MRx_Msg(("INT_SIZE=%d\n", (int)(ucData&0xf)+1));
				iTE_MRx_Msg(("STAT_SIZE=%d\n", (int)(ucData>>4)+1));
			break;
	}
}
#endif
//****************************************************************************
void mhl_CBusReDiscovery(void)
{
	iTE_MRx_Msg(("*******************mhl_CBusReDiscovery \n"));

	MhlRx_Set(0x28, 0x02, 0x02);
	iTE_Sleep_ms(100);
	MhlRx_Set(0x28, 0x02, 0x00);

}
//****************************************************************************
MSC_STATUS Msc_MscMsgFire(MSC_CMD *MscCmd)
{
	return Msc_MscFire(MSC_PKT_MSC_MSG, MscCmd);
}
//****************************************************************************
MSC_STATUS Msc_MscFire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd)
{
	MSC_STATUS eMscResult;
	iTE_u8	ucTxBakBuf[2];
	iTE_u1 bRetryFlag=iTE_FALSE;
	iTE_u8	ucRetryCnt=0;

	if(++u8MscFireCnt > CBUSRTYMAX<<2){
		iTE_MRx_Msg(("Msc_MscFire recursive too many times\n"));
		u8MscFireCnt = 0;
		return MSC_FAIL;
	}

	do{
		eMscResult = Msc_Fire(eMscCtlPkt, MscCmd);

		if( eMscResult == MSC_RCVABORT){
			if(Msc_Fire(MSC_PKT_GET_MSC_ERRORCODE, &TxMscCmd)==MSC_SUCCESS){
				iTE_MRx_Msg(("GET_MSC_ERRORCODE=0x%2X\n",(int)MhlRx_R(0x56)));
			}
		}

		if (eMscResult != MSC_SUCCESS){
			ucRetryCnt++;

			if(eMscResult != MSC_FWRXPKT){		//should always TRUE
				if(MhlRx_R(0x04) & 0x10){
					iTE_MRx_Msg(("Check MSC_MSG interrupt to prevent deadlock ...\n"));
					MhlRx_W(0x04, 0x10);
					iTE_MRx_Msg(("MSC Rx MSC_MSG Interrupt ...\n"));
					MhlRx_Rb(0x54, 0x02, ucTxBakBuf );
					Msc_MsgHandler();
					MhlRx_Wb(0x54, 0x02, ucTxBakBuf);
				}else{
					iTE_Sleep_ms(200);
				}
			}
			if(eMscCtlPkt == MSC_PKT_WRITE_BURST){
				MhlRx_Set(0x5C, 0x01, 0x01);	//TxPktFIFO clear for Retry
				Msc_WriteBurstDataFill(stTxWbInfo.ucOffset, stTxWbInfo.ucCnt, stTxWbInfo.pucTxScrpad);
				iTE_MRx_Msg(("Retry WRITE_BURST fire command ...\n"));
			}
			if(EnCBusReDisv && ucRetryCnt>= CBUSRTYMAX && bRetryFlag==iTE_FALSE){
				iTE_MRx_Msg(("ERROR: CBUS Link Layer Error ==> Retry CBUS Discovery Process !!!\n"));
				MhlRx_Set(0x0f, 0x10, 0x10);
				MhlRx_W(0x08, 0xff);
				MhlRx_W(0x09, 0xff);
				MhlRx_Set(0x0f, 0x10, 0x00);
				bRetryFlag = iTE_TRUE;
				ucRetryCnt = 0;
			}
			iTE_MRx_Msg(("Retry this command again ... ==> %dth retry\n", ucRetryCnt));
		}
	}while((eMscResult != MSC_SUCCESS) && (ucRetryCnt < CBUSRTYMAX));

	u8MscFireCnt = 0;
	if(eMscResult != MSC_SUCCESS){
		iTE_MRx_Msg(("*************************************************************\n"));
		iTE_MRx_Msg(("ERROR: MSC Request Maximum Retry Fail\n"));
		return iTE_FALSE;
	}else{
		return iTE_TRUE;
	}

}
//****************************************************************************
iTE_u1 Msc_WriteBurstDataFill(iTE_u8 ucOffset, iTE_u8 ucByteNo, iTE_u8 *pucData)
{
	iTE_u8	ucTemp;

	stTxWbInfo.pucTxScrpad = pucData;
	stTxWbInfo.ucCnt = ucByteNo;
	stTxWbInfo.ucOffset = ucOffset;

	if(ucByteNo<=2 || (ucOffset+ucByteNo)>16){
		iTE_Msg(("ERROR: Set Burst Write Data Fail\n"));
		return iTE_FALSE;
	}else{
		MhlRx_Wb(0x5E, 2, pucData);
		MhlRx_W(0x54, 0x40+ucOffset);
		if(MSCBurstWrID)
			ucTemp = 2;
		else
			ucTemp = 0;

		iTE_MRx_Msg(("Msc_WriteBurstDataFill ="));
		for(; ucTemp < ucByteNo;){
			iTE_MRx_Msg((" 0x%x\n ", (int)pucData[ucTemp]));
			MhlRx_W(0x59, pucData[ucTemp++]);
		}
		iTE_MRx_Msg(("\n"));
		return iTE_TRUE;
	}
}
//****************************************************************************
MSC_STATUS Msc_Fire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd)
{
	#define CBUSWAITNUM     	(100)
	iTE_u8	ucCBusWaitCnt;

	switch(eMscCtlPkt){
		case	MSC_PKT_WRITE_STAT:
		case	MSC_PKT_SET_INT:
		case	MSC_PKT_READ_DEVCAP:
		case	MSC_PKT_GET_STATE:
		case	MSC_PKT_GET_VENDOR_ID:
		case	MSC_PKT_SET_HPD:
		case	MSC_PKT_CLR_HPD:
		case	MSC_PKT_MSC_MSG:

		case	MSC_PKT_GET_DDC_ERRORCODE:
		case	MSC_PKT_GET_MSC_ERRORCODE:
		case	MSC_PKT_WRITE_BURST:

				MhlRx_Wb(0x54, 0x02, &MscCmd->ucOffset);
				MhlRx_W(MscFire[eMscCtlPkt][0], MscFire[eMscCtlPkt][1]);
				iTE_MRx_Msg(("MSC_FIRE  Reg0x%x=0x%x\n",(int)MscFire[eMscCtlPkt][0], (int)MscFire[eMscCtlPkt][1]));
			break;
#if 0
//		case		MSC_PKT_GET_SC3_ERRORCODE:
//		case		MSC_PKT_GET_SC1_ERRORCODE:
//		case	MSC_PKT_EOF:
		case	MSC_PKT_ACK:
				ucTxData[0]=0x05;
				ucTxData[1]=Msc_CtlPktTab[MSC_PKT_ACK];
				ucTxData[2]=0x04;
				ucTxData[3]=MscCmd->ucValue;
				MhlRx_Wb(0x58, 4, ucTxData);
				MhlRx_W(0x51, 0x80);
			break;
//		case	MSC_PKT_NACK:
		case	MSC_PKT_ABORT:
				ucTxData[0]=0x05;
				ucTxData[1]=Msc_CtlPktTab[MSC_PKT_ABORT];
				MhlRx_Wb(0x58, 0x02, ucTxData);
				MhlRx_W(0x51, 0x80);
			break;

#endif
		default:
			return MSC_FAIL;
	}

	ucCBusWaitCnt = 0;
	while((MhlRx_R(0x1c)&0x02) && (ucCBusWaitCnt<CBUSWAITNUM*10)){
		iTE_Sleep_ms(1);
		ucCBusWaitCnt++;
	}
	if(MhlRx_R(0x05) & 0x02){
		iTE_u8 pucMHL18[2];
		MhlRx_Rb(0x18, 0x02, pucMHL18);
		MhlRx_Wb(0x18, 0x02, pucMHL18);
		MhlRx_W(0x05, 0x02);
		return MHL_MscReqFailCheck(pucMHL18);
	}else if (ucCBusWaitCnt < CBUSWAITNUM){
		return MSC_SUCCESS;
	}else{
		iTE_MRx_Msg(("ERROR: Unknown Issue!!!\n"));
		return MSC_FAIL;
	}
}
//****************************************************************************
void Msc_MsgHandler(void)
{
	#define RCPBUSYNUM      (50)
	#define RAPBUSYNUM      (50)
	iTE_u8	ucMRxReg60[2];
	MSC_CMD MscTxCmd;

	MhlRx_Rb(0x60, 2, ucMRxReg60);

	iTE_MRx_Msg(("Msc_MsgHandler cmd=0x%x, data=0x%x\n", (int)ucMRxReg60[0], (int)ucMRxReg60[1]));
	switch(ucMRxReg60[0]){
		case	MSC_MSG_MSGE :
#if iTE_MhlRx_Msg_Print
				Msc_MsgeHandler((MSC_STATUS_CODE)ucMRxReg60[1]);
#endif
			break;
		case	MSC_MSG_RAP:
				MscTxCmd.ucOffset = MSC_MSG_RAPK;
				iTE_MRx_Msg(("stMscMsg.unMsgData.eRapAction = 0x%x\n",(int)ucMRxReg60[1]));

				if(SuppRAPCode[ucMRxReg60[1] & 0x1F] ) {
					if(iTE_TRUE == Msc_RapHandler((RAP_ACTION_CODE)ucMRxReg60[1])){
						MscTxCmd.ucValue = RAP_NO_ERROR;
					}
				}else{
					MscTxCmd.ucValue = RAP_UNSUPPORT_ACTION_CODE;
				}
				Msc_MscMsgFire(&MscTxCmd);
			break;
		case MSC_MSG_RAPK :
#if iTE_MhlRx_Msg_Print
				Msc_Rapk_Handler((RAP_ERROR)ucMRxReg60[1]);
#endif
			break;
		case	MSC_MSG_RCP:
				if(SuppRCPCode[ucMRxReg60[1] & 0x7F]){
					if(iTE_FALSE == Msc_RcpHandler(ucMRxReg60[1])){
						MscTxCmd.ucOffset = MSC_MSG_RCPE;
						MscTxCmd.ucValue = RCP_RESPONDER_BUSY;
						Msc_MscMsgFire(&MscTxCmd);
					}
				}else{
					iTE_MRx_Msg(("Rx RCP = 0x%x not supported \n",(int)ucMRxReg60[1]));
					MscTxCmd.ucOffset = MSC_MSG_RCPE;
					MscTxCmd.ucValue = RCP_INEFFECTIVE_KEY;
					Msc_MscMsgFire(&MscTxCmd);
				}

				MscTxCmd.ucOffset = MSC_MSG_RCPK;
				MscTxCmd.ucValue = ucMRxReg60[1];
				Msc_MscMsgFire(&MscTxCmd);
			break;
		case	MSC_MSG_RCPK :
#if iTE_MhlRx_Msg_Print
				Msc_RcpkHandler(ucMRxReg60[1]);
#endif
			break;


		case	MSC_MSG_RCPE :
#if iTE_MhlRx_Msg_Print
				Msc_RcpeHandler((RCP_ERROR)ucMRxReg60[1]);
#endif
			break;

		case	MSC_MSG_UCP  :
				if(iTE_FALSE == Msc_UcpHandler(ucMRxReg60[1])){
					MscTxCmd.ucOffset = MSC_MSG_UCPE;
					MscTxCmd.ucValue = UCP_INEFFECTIVE_KEY;
					Msc_MscMsgFire(&MscTxCmd);
				}

				MscTxCmd.ucOffset = MSC_MSG_UCPK;
				MscTxCmd.ucValue = ucMRxReg60[1];
				Msc_MscMsgFire(&MscTxCmd);
			break;

		case	MSC_MSG_UCPK :
#if iTE_MhlRx_Msg_Print
				Msc_UcpkHandler(ucMRxReg60[1]);
#endif
			break;

		case	MSC_MSG_UCPE :
#if iTE_MhlRx_Msg_Print
				Msc_UcpeHandler((UCP_ERROR)ucMRxReg60[1]);
#endif
			break;

		default :
				iTE_MRx_Msg(("ERROR: Unknown MSC_MSG sub-command code 0x%02X !!!\n", (int)ucMRxReg60[1]));
				MscTxCmd.ucOffset = MSC_MSG_MSGE;
				MscTxCmd.ucValue = MSC_INVALID_CODE;
				Msc_MscMsgFire(&MscTxCmd);
			break;
	}
}
//****************************************************************************
#if iTE_MhlRx_Msg_Print
void	Msc_MsgeHandler(MSC_STATUS_CODE eMsgStatus)
{
	iTE_Msg(("Rx MSGE => 0x%x \n",(int)eMsgStatus));
	switch(eMsgStatus){
		case MSC_NO_ERROR:
				iTE_MRx_Msg(("No Error\n"));
			break;
		case MSC_INVALID_CODE:
				iTE_MRx_Msg(("ERROR: Invalid sub-command code !!!\n"));
			break;
		default  :
				iTE_MRx_Msg(("ERROR: Unknown MSC_MSG status code 0x%02X !!!\n", (int)eMsgStatus));
			break;
	}
}
//****************************************************************************
void Msc_Rapk_Handler(RAP_ERROR eRapError)
{
	iTE_Msg(("RX RAPK => "));
	switch(eRapError) {
		case RAP_NO_ERROR:
				iTE_MRx_Msg(("No Error\n"));
			break;
		case RAP_UNRECOGN_ACTION_CODE:
				iTE_MRx_Msg(("ERROR: Unrecognized Action Code !!!\n"));
			break;
		case RAP_UNSUPPORT_ACTION_CODE:
				iTE_MRx_Msg(("ERROR: Unsupported Action Code !!!\n"));
			break;
		case RAP_RESPONDER_BUSY:
				iTE_MRx_Msg(("Responder Busy ...\n"));
			break;
		default  :
				iTE_MRx_Msg(("ERROR: Unknown RAP status code 0x%02X !!!\n", eRapError));
			break;
	}
}
//****************************************************************************
void Msc_RcpkHandler(iTE_u8 ucRcpKey)
{
	iTE_Msg(("RX RCPK => "));
	Msc_RcpKeyParse( ucRcpKey);
}
//****************************************************************************
void Msc_UcpkHandler(iTE_u8 ucUcpkCode)
{
	iTE_Msg(("RX UCPK [0x%x] =%c\n", (int)ucUcpkCode, ucUcpkCode));
}
//****************************************************************************
void Msc_RcpeHandler(RCP_ERROR eRcpError)
{
	iTE_Msg(("RX RCPE => "));
	switch(eRcpError){
		case	RCP_NO_ERROR:
				iTE_MRx_Msg(("No Error\n"));
			break;
		case	RCP_INEFFECTIVE_KEY:
				iTE_MRx_Msg(("ERROR: Ineffective RCP Key Code !!!\n"));
			break;
		case	RCP_RESPONDER_BUSY:
				iTE_MRx_Msg(("Responder Busy ...\n"));
			break;
		default:
				iTE_MRx_Msg(("ERROR: Unknown RCP status code !!!\n"));
			break;
	}
}
//****************************************************************************
void Msc_UcpeHandler(UCP_ERROR eUcpeStatus)
{
	iTE_MRx_Msg(("RX UCPE  => "));
	switch(eUcpeStatus){
		case	UCP_NO_ERROR:
				iTE_MRx_Msg((" No Error\n"));
			break;
		case	UCP_INEFFECTIVE_KEY:
				iTE_MRx_Msg((" ERROR: Ineffective UCP Key Code !!!\n"));
			break;
		default:
				iTE_MRx_Msg(("ERROR: Unknown UCP status code 0x%02X !!!\n", (int)eUcpeStatus));
			break;
	}
}
#endif
//****************************************************************************
void Msc_RcpKeyParse( iTE_u8 ucRcpKey )
{
	if(pucRcpString[ucRcpKey&0x7f] != NULL){
     		iTE_MRx_Msg(("KeyCode=0x%02X ==> [%s]\n ", (int)ucRcpKey, pucRcpString[ucRcpKey&0x7f]));
	}else{
		iTE_Msg(("ERROR: Reserved RCP sub-command code [0x%x]!!!\n", (int)ucRcpKey));
		return;
	}

	if(ucRcpKey & 0x80){
		iTE_MRx_Msg((" Key Release\n"));
	}else{
		iTE_MRx_Msg((" Key Press\n"));
	}
}
//****************************************************************************
void Msc_3DProcess(MHL3D_STATE *e3DReqState)
{
	iTE_u8	ucTemp;

	iTE_Msg(("***Msc_3DProcess***\n"));
	switch(*e3DReqState){
		case	MHL3D_REQ_DONE:
				iTE_MRx_Msg(("***Msc_3DProcess*** MHL3D_REQ_DONE\n"));
			break;
		case	MHL3D_REQ_START:
				iTE_MRx_Msg(("***Msc_3DProcess*** MHL3D_REQ_START\n"));
				if(st3DParse.uc3DInfor == 0){
#if (USING_DEFAULT_3D_VIC == iTE_TRUE)
					ucTemp = sizeof(uc3DVic);
					if((ucTemp==1) && (uc3DVic[0] == 0))
						ucTemp = 0;
					st3DParse.ucVicCnt = ucTemp;
					st3DParse.uc3DInfor = uc3DVic;
#else
	#if (USING_CALLOC == iTE_TRUE)
					st3DParse.uc3DInfor = (iTE_u8*)calloc(VIC_3D_MAX , sizeof(iTE_u8));
	#else
					st3DParse.uc3DInfor = uc3DInfor;
	#endif
					st3DParse.ucVicCnt = stCurEdidInfo->pstVdbInfo.u8VicCnt;
					if(st3DParse.ucVicCnt < VIC_3D_MAX){
						ucTemp = st3DParse.ucVicCnt;
					}else{
						ucTemp = VIC_3D_MAX;
					}
					if( stCurEdidInfo->pstVsdbInfo.u83dMulti & 0x80) {
						iTE_u8	u83DCnt = 0;
						while(ucTemp--){
							st3DParse.uc3DInfor[ucTemp] = 0;
							if( stCurEdidInfo->pstVsdbInfo.u163dFP & (0x01 << ucTemp)){
								st3DParse.uc3DInfor[ucTemp] |= 0x01;
								u83DCnt = 1;
							}
							if( stCurEdidInfo->pstVsdbInfo.u163dTaB & (0x01 << ucTemp)){
								st3DParse.uc3DInfor[ucTemp] |= 0x02;
								u83DCnt = 1;
							}
							if( stCurEdidInfo->pstVsdbInfo.u163dSbS & (0x01 << ucTemp)){
								st3DParse.uc3DInfor[ucTemp] |= 0x04;
								u83DCnt = 1;
							}
						}

						for(ucTemp = 0; ucTemp < DEFAULT_3D_CNT; ucTemp++){
							if( stCurEdidInfo->u8Default3DVic[ucTemp] < VIC_3D_MAX ){
								st3DParse.uc3DInfor[ stCurEdidInfo->u8Default3DVic[ucTemp]] |= u8Default3DVicFmt[ucTemp];
								u83DCnt = 1;
							}
						}
						if(u83DCnt == 0){
							st3DParse.ucVicCnt = 0;
						}
					}else{
						st3DParse.ucVicCnt = 0;
					}

#endif
					ucTemp = sizeof(uc3DDtd);
					if((ucTemp == 1) && (uc3DDtd[0]==0))
						ucTemp = 0;
					st3DParse.ucDtdCnt = ucTemp;
					st3DParse.uc3DTempCnt = 0x80;
				}

				if(st3DParse.uc3DInfor){
					TxMscCmd.ucOffset = 0x20;
					TxMscCmd.ucValue = MHL_INT_REQ_WRT;
					Msc_Fire(MSC_PKT_SET_INT, &TxMscCmd);
					*e3DReqState = MHL3D_REQ_WRT;

#if (USING_CALLOC == iTE_TRUE)
				}else{
					iTE_MsgE(("st3DParse.uc3DInfor mem alloc fail \n" ));
					Msc_Fire(MSC_PKT_ABORT, &TxMscCmd);
					*e3DReqState = MHL3D_REQ_DONE;
					st3DParse.uc3DTempCnt = 0x80;
#endif
				}
			break;
		case	MHL3D_REQ_WRT:
					iTE_MRx_Msg(("***Msc_3DProcess*** MHL3D_REQ_WRT\n"));
					*e3DReqState = MHL3D_GNT_WRT;
			break;
		case	MHL3D_GNT_WRT:
				iTE_MRx_Msg(("***Msc_3DProcess*** MHL3D_GNT_WRT\n"));
				if(st3DParse.uc3DTempCnt & 0x80){
					*e3DReqState = MSC_3DInforSend(iTE_TRUE);
					iTE_MRx_Msg(("***Msc_3DProcess*** MSC_3DInforSend(iTE_TRUE)\n"));
				}else{
					*e3DReqState = MSC_3DInforSend(iTE_FALSE);
					iTE_MRx_Msg(("***Msc_3DProcess*** MSC_3DInforSend(iTE_FALSE)\n"));
				}
				if(*e3DReqState == MHL3D_REQ_DONE){
#if (USING_CALLOC == iTE_TRUE)
					free(st3DParse.uc3DInfor);
#endif
					st3DParse.uc3DInfor = 0x00;
					st3DParse.uc3DTempCnt = 0x80;
					iTE_MRx_Msg(("***Msc_3DProcess*** MHL3D_REQ_DONE\n"));
				}else{
					TxMscCmd.ucOffset = 0x20;
					TxMscCmd.ucValue = MHL_INT_REQ_WRT;
					Msc_Fire(MSC_PKT_SET_INT, &TxMscCmd);
					*e3DReqState = MHL3D_REQ_WRT;
					iTE_MRx_Msg(("***Msc_3DProcess*** MHL3D_REQ_WRT\n"));
				}
			break;
	default:
		break;

	}

}
//****************************************************************************
MHL3D_STATE MSC_3DInforSend(iTE_u1 b3dDtd)
{
	iTE_u8	ucTemp, uc3DInforLen;
	iTE_u8	ucWBData[16];
	iTE_u8	uc3DTempCnt;
	MHL3D_STATE eRet3dState;

	uc3DTempCnt = st3DParse.uc3DTempCnt & 0x7F;

	if(b3dDtd){
		ucWBData[0] = MSC_3D_DTD >> 8;
		ucWBData[1] = MSC_3D_DTD & 0xff;
		ucWBData[3] = st3DParse.ucDtdCnt;
	}else{
		ucWBData[0] = MSC_3D_VIC >> 8;
		ucWBData[1] = MSC_3D_VIC & 0xff;
		ucWBData[3] = st3DParse.ucVicCnt;
	}
	ucWBData[2] = 0;
	ucWBData[4] = (uc3DTempCnt / 5) + 1;

	ucWBData[5] = ucWBData[3] - uc3DTempCnt;
	if(ucWBData[5] > 5){
		ucWBData[5] = 5;
		eRet3dState = MHL3D_REQ_WRT;
		st3DParse.uc3DTempCnt += 5;
		iTE_Msg(("*** MSC_3DInforSend MHL3D_REQ_WRT ***\n"));
	}else{
		if(b3dDtd){
			st3DParse.uc3DTempCnt = 0;
			eRet3dState = MHL3D_REQ_WRT;
			iTE_Msg(("*** MSC_3DInforSend DTD Done ***\n"));
		}else{
			st3DParse.uc3DTempCnt = 0x80;
			eRet3dState = MHL3D_REQ_DONE;
			iTE_Msg(("*** MSC_3DInforSend VIC Done ***\n"));
		}
	}
	uc3DInforLen = 6 + (ucWBData[5] * 2);
	for(ucTemp = 6; ucTemp < uc3DInforLen; ){
		ucWBData[ucTemp++] = 0x00;
		if(b3dDtd){
			ucWBData[ucTemp++] = uc3DDtd[uc3DTempCnt++];
		}else{
			ucWBData[ucTemp++] = st3DParse.uc3DInfor[uc3DTempCnt++];
		}
	}
	do{
		if(--ucTemp != 2){
			ucWBData[2] -= ucWBData[ucTemp];
		}
	}while(ucTemp);


	if(iTE_TRUE == Msc_WriteBurstDataFill(0x00, uc3DInforLen, ucWBData)){
		TxMscCmd.ucOffset = 0x40;
		if(Msc_Fire(MSC_PKT_WRITE_BURST, &TxMscCmd)==MSC_SUCCESS){
			TxMscCmd.ucOffset = 0x20;
			TxMscCmd.ucValue = MHL_INT_DSCR_CHG;
			Msc_Fire(MSC_PKT_SET_INT, &TxMscCmd);
		}
	}
	iTE_Msg(("*** MSC_3DInforSend eRet3dState = 0x%x ***\n", (int)eRet3dState));
	return eRet3dState;
}

