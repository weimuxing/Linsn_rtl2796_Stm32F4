///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Cec.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Cec.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
//****************************************************************************
#define _CEC_LOOKUP_TAB_
#include "it6662/include/Cec.h"
//#include "Cec_Reg.h"
#include "it6662/include/Cec_Typedef.h"
#include "it6662/include/CecRx_Handler.h"
#include "it6662/include/CecTx_Handler.h"
#include "it6662/include/ITE_Edid.h"
//#include "../INC/CEC_typedef.h"
//****************************************************************************
#if (USING_CEC == iTE_TRUE)

iTE_u8	u8CecTxReSendCnt=0;				//20121220 cec resend max 10 times
iTE_u8	g_u8CecSel = CEC_RX_SEL;
iTE_u8	g_u8CecTxSel;
extern IT6662_Var	*stIt6662;
extern EdidInfo*	g_stEdidInfo;
#if (USING_1to8 == iTE_TRUE)
stCecVar g_stCecRx[3], g_stCecTx[3];
#else
stCecVar g_stCecRx[1], g_stCecTx[1];
#endif
stCecVar *g_stCecCur;
iTE_u8	g_u8Initiator;


//****************************************************************************
void Cec_VarInit(iTE_u8 u8PaH, iTE_u8 u8PaL, iTE_u8 u8MyLogAdr)
{
	g_stCecCur->u8CecLA[0] = IT6662_TX_LA;
	g_stCecCur->u8CecLA[1] = IT6662_TX_LA;
	g_stCecCur->u8CecLA[2] = IT6662_TX_LA;
	g_stCecCur->u8CecLA[3] = IT6662_TX_LA;
	g_stCecCur->u8MyLogAdr = u8MyLogAdr;
	g_stCecCur->u8PaH = u8PaH;
	g_stCecCur->u8PaL = u8PaL;

	g_stCecCur->stRxQ.u8Rptr = g_stCecCur->stRxQ.u8Wptr = 0;
	g_stCecCur->stTxQ.u8Rptr = g_stCecCur->stTxQ.u8Wptr = 0;
	g_stCecCur->u8RxTmpHeader[0] = 0;
	g_stCecCur->bTxQFull = 0;
	g_stCecCur->bTxCecDone = 1;
	g_stCecCur->u8TxCecFire = 0x00;
	g_stCecCur->u8TxCecInitDone = 0x00;
//	g_stCecCur->u8RxTmpHeader[1] = 0;
//	g_stCecCur->u8RxTmpHeader[2] = 0;
}
//****************************************************************************
void Cec_Init(void)
{
	iTE_u32 ulClkCnt = 0;
	iTE_Cec_Msg((" cec_init_S \n"));
	Cec_W(0x08, 0x4C);
	Cec_W(0x08, 0x48);

    	Cec_W(0x22, 0x0F);//ucMyselfAddr); 	//REG_Logical_addr[3:0]

	if(g_u8CecSel == CEC_RX_SEL){
		Cec_VarInit(0x00, 0x00, IT6662_RX_LA);		//
	}else{
		Cec_VarInit(0x00, 0x10, IT6662_TX_LA);		//
	}

	ulClkCnt = stIt6662->u32RefClk/160;
	iTE_Cec_Msg(("ulCLKCnt=0x%x\n", (int)(ulClkCnt&0xff)));

	Cec_W(0x09,  0x40 | (CEC_RX_SELF_DIS<< 5));

	Cec_W(0x0B, 0x14); 		//REG_Data_Min[7:0]

    	Cec_W(0x0C, ulClkCnt); 	//REG_Timer_unit[7:0]

	Cec_Set(0x08, 0x04, CEC_RST << 2);

	Cec_Set(0x09, 0x02, 0x00);

//	Cec_W(0x4C, 0xFF);

	Cec_W(0x06, 0x00);
	Cec_Set(0x08, 0x01, 0x01);

	Cec_RxFifoReset();
//	Cec_W(0x0A, 0x20);				// Disable CEC HW retry
	Cec_W(0x0A, 0x03);
	Cec_Set(0x0A, 0x40, 0x40);			// Enable CEC

	iTE_Cec_Msg((" cec_init_E \n"));
}

//****************************************************************************
void Cec_Irq(void)
{
	iTE_u1	bDoTx=1;
	iTE_u8	u8CecStatus = Cec_R(0x4C);

	iTE_Cec_Msg(("Cec_Irq %02X\n", u8CecStatus));
	Cec_W(0x4C, u8CecStatus);
//	iTE_Cec_Msg(("[0x%02X, 0x%02X]\n", Cec_R(0x44), Cec_R(0x54)));
	if(u8CecStatus & 0x28 /*0x01*/){
		//iTE_Cec_Msg(("Cec_Tx Int\n"));
		if(u8CecStatus & 0x20){
			g_stCecCur->bTxCecDone = 1;
			if(Cec_FireStatus(0)==iTE_FALSE){
				iTE_Cec_Msg(("*************************************Cec_Tx Fail\n"));
				if(++u8CecTxReSendCnt < CEC_RESEND_MAX){
					Cec_TxFire();
					bDoTx = 0;
				}else{
					u8CecTxReSendCnt = 0;
				}
			}else{
				iTE_u8	u8Temp = (g_stCecCur->u8MyLogAdr << 4) | g_stCecCur->u8MyLogAdr;
				u8CecTxReSendCnt = 0;
				if(Cec_R(0x10) == u8Temp){
//					g_stCecCur->u8CecLA[g_u8CecTxSel] = g_stCecCur->u8MyLogAdr;
					Cec_W(0x22, g_stCecCur->u8MyLogAdr);
					Cec_TxReportPA();
					g_stCecCur->u8TxCecInitDone |= (0x01 << g_u8CecTxSel);
					bDoTx = 0;
				}
			}
		}
		if(u8CecStatus & 0x08){
			u8CecTxReSendCnt = 0;
			g_stCecCur->bTxCecDone = 1;
			if(Cec_FireStatus(1)==iTE_FALSE){
				iTE_u8	u8Temp = (g_stCecCur->u8MyLogAdr << 4) | g_stCecCur->u8MyLogAdr;
				iTE_Cec_Msg(("*************************************Cec_Tx Fail\n"));
				if(Cec_R(0x10) ==u8Temp){
					if(g_stCecCur->u8MyLogAdr != 0x0F){
						g_stCecCur->u8MyLogAdr =  Cec_SwitchLA(g_stCecCur->u8MyLogAdr);
						if(g_stCecCur->u8MyLogAdr != 0x0F) {
							Cec_TxPolling(g_stCecCur->u8MyLogAdr);
							bDoTx = 0;
						}else{																// all of LA are inactived.
//							g_stCecCur->u8CecLA[g_u8CecTxSel] = g_stCecCur->u8MyLogAdr;
							Cec_W(0x22, g_stCecCur->u8MyLogAdr);
							Cec_TxReportPA();
							g_stCecCur->u8TxCecInitDone |= (0x01 << g_u8CecTxSel);			//????
							bDoTx = 0;
						}
					}
				}
			}
		}
	}

	if(u8CecStatus & 0xD4/*0x02*/){
//		iTE_Cec_Msg(("Cec_Rx Int\n"));
		if(u8CecStatus & 0x04){
			iTE_u8	u8Temp[3];
			iTE_Cec_Msg(("****************************************************Cec_Rx Fail\n"));
			Cec_RxCmdGet();
			Cec_Rb(0x51, 0x03, u8Temp);
			iTE_Cec_Msg(("****0x%02X, 0x%02X, 0x%02X\n", u8Temp[0], u8Temp[1], u8Temp[2]));
		}
		if(u8CecStatus & 0xC0){
			iTE_u8	u8Temp[3];
			Cec_Rb(0x51, 0x03, u8Temp);
			iTE_Cec_Msg(("****0x%02X, 0x%02X, 0x%02X\n", u8Temp[0], u8Temp[1], u8Temp[2]));
			iTE_Cec_Msg(("***************************************************Cec_Rx Cmd/Data FIFO overflow  [%02X]\n", u8CecStatus));
			Cec_RxFifoReset();
		}
		if(u8CecStatus & 0x10){
			iTE_u8	u8Temp[3];
			iTE_Cec_Msg(("Cec_Rx Done\n"));
			Cec_RxCmdGet();
			Cec_Rb(0x51, 0x03, u8Temp);
			iTE_Cec_Msg(("****0x%02X, 0x%02X, 0x%02X\n", u8Temp[0], u8Temp[1], u8Temp[2]));
		}
	}

	Cec_RxCmdPull();
	if(bDoTx){
//		Cec_TxSel(IT6662_DEFAULT_CEC);
		if(g_stCecCur->bTxQFull){
			Cec_TxCmdPush(0, 0, 0);
		}
		if(g_u8CecSel == CEC_RX_SEL){
			Cec_TxCmdPull();
		}
	}
}
//****************************************************************************
iTE_u1 Cec_FireStatus(iTE_u1 bTxDone)
{
	iTE_u8	u8Initiator, u8Follower;
	iTE_u8	u8Sta2 = Cec_R(0x54);
	iTE_u8	u8Sta1 = (Cec_R(0x44) & 0x0C) >> 2;
	iTE_ps8	sTxStatus[4] = {"ACK", "NACK", "RETRY", "FAIL"};

	iTE_Cec_Msg(("OpCode[0x%02X] %s\n", Cec_R(0x11), sTxStatus[u8Sta1]));
	iTE_Cec_Msg((" 0x%02X, 0x%02X\n", u8Sta1, u8Sta2));

	u8Initiator = Cec_R(0x10);
	u8Follower = u8Initiator & 0x0F;
	u8Initiator >>= 4;
#if 1
	if(u8Initiator == u8Follower){
		u8Sta2 &= 0x03;
		if(bTxDone){
			if(u8Sta2  == 0x01)	{	//	ACK
				return iTE_FALSE;
			}
		}else{
			if(u8Sta2 != 0x02)		{	//	NACK
				return iTE_FALSE;
			}
		}
	}
	iTE_Cec_Msg(("Cec_Tx Done\n"));
	return iTE_TRUE;
#else
	if(bTxDone){
		if(/*(u8Follower == 0x0F) ||*/(u8Initiator == u8Follower)){
			if((u8Sta2 & 0x03) == 0x01){	// ACK
				iTE_Cec_Msg(("Cec_Tx Fail\n"));
				return iTE_FALSE;
			}
		}
		iTE_Cec_Msg(("Cec_Tx Done\n"));
		return iTE_TRUE;
	}else{
		if(/*(u8Follower == 0x0F) ||*/(u8Initiator == u8Follower)){
			if((u8Sta2 & 0x03) == 0x02){	// NACK
				iTE_Cec_Msg(("Cec_Tx Done\n"));
				return iTE_TRUE;
			}
		}
		iTE_Cec_Msg(("Cec_Tx Fail\n"));
		return iTE_FALSE;
	}
#endif
}
//****************************************************************************
iTE_u8 Cec_SwitchLA(iTE_u8 u8MyLA)
{
	iTE_u8	pu8MyLA[0x10] = {	DEVICE_ID_FREEUSE,						// DEVICE_ID_TV
								DEVICE_ID_RECORDING2,					// DEVICE_ID_RECORDING1
								DEVICE_ID_RECORDING3,					// DEVICE_ID_RECORDING2
								DEVICE_ID_TUNER2,						// DEVICE_ID_TUNER1
								DEVICE_ID_PLAYBACK2,					// DEVICE_ID_PLAYBACK1
								DEVICE_ID_FREEUSE,						// DEVICE_ID_AUDIO
								DEVICE_ID_TUNER3,						// DEVICE_ID_TUNER2
								DEVICE_ID_TUNER4,						// DEVICE_ID_TUNER3
								DEVICE_ID_PLAYBACK3,					// DEVICE_ID_PLAYBACK2
								DEVICE_ID_FREEUSE,						// DEVICE_ID_RECORDING3
								DEVICE_ID_FREEUSE,						// DEVICE_ID_TUNER4
								DEVICE_ID_FREEUSE,						// DEVICE_ID_PLAYBACK3
								DEVICE_ID_BROADCAST,					// DEVICE_ID_RESERVED1
								DEVICE_ID_BROADCAST,					// DEVICE_ID_RESERVED2
								DEVICE_ID_BROADCAST,					// DEVICE_ID_FREEUSE
								DEVICE_ID_BROADCAST					// DEVICE_ID_BROADCAST
							};

	return pu8MyLA[u8MyLA];
}
//****************************************************************************
void Cec_RxFifoReset(void)
{
	Cec_W(0x52, 0x60);
	Cec_W(0x52, 0xE0);
	Cec_W(0x52, 0x80);
	g_stCecCur->u8RxTmpHeader[0] = 0;
//	g_stCecCur->u8RxTmpHeader[1] = 0;
//	g_stCecCur->u8RxTmpHeader[2] = 0;
}
//****************************************************************************
iTE_u1 Cec_RxCmdGet(void)
{
	iTE_u8	pu8Reg51[3];
	iTE_pu8	pu8RxHeader = g_stCecCur->u8RxTmpHeader;



	Cec_Rb(0x51, 0x03, pu8Reg51);
	iTE_Cec_Msg(("Cec_RxCmdGet [%02X, %02X, %02X]\n", pu8Reg51[0], pu8Reg51[1], pu8Reg51[2]));
	pu8Reg51[1] &= 0x0F;
	do{
		if(pu8Reg51[0] & 0xCC){
			iTE_Cec_Msg(("**************************************CecRx_FIFORESET\n"));
			Cec_RxFifoReset();
			return iTE_FALSE;
		}else{
			if(pu8RxHeader[0]){
				if(iTE_FALSE == Cec_RxCmdPush(pu8RxHeader)){
					return iTE_FALSE;
				}
			}
			while(pu8Reg51[1]--){
				Cec_Rb(0x4D, 0x03, pu8RxHeader);
				iTE_Cec_Msg(("0x%02X, %02X, %02X\n", pu8RxHeader[0], pu8RxHeader[1], pu8RxHeader[2]));
				if(iTE_FALSE == Cec_RxCmdPush(pu8RxHeader)){
					return iTE_FALSE;
				}
			}
			Cec_Rb(0x51, 0x02, pu8Reg51);
			pu8Reg51[1] &= 0x0F;
		}
	}while(pu8Reg51[1]);

	return iTE_TRUE;
}
//****************************************************************************
iTE_u1 Cec_RxCmdPush(iTE_pu8 pu8Header)
{
	iTE_pu8	pu8RxQ = g_stCecCur->stRxQ.pu8Q;
	iTE_u8	u8Rptr = g_stCecCur->stRxQ.u8Rptr;
	iTE_u8	u8Wptr = g_stCecCur->stRxQ.u8Wptr;
	iTE_u8	u8CecSize = (pu8Header[2] & 0x1F) + 1;
	iTE_u8	u8FreeSize;

	if(u8Rptr > u8Wptr){
		u8FreeSize = u8Rptr - u8Wptr;
	}else{
		u8FreeSize = CEC_RX_QUEUE_SIZE + u8Rptr - u8Wptr;
	}
	if(u8CecSize < 3){
//		u8CecSize = 3;
		if(pu8Header[2] & 0x60){
			iTE_Cec_Msg(("--------------CEC Rx ERROR: %x\n", pu8Header[2] & 0x60));
		}
		iTE_Cec_Msg(("-----------------------------------------------------------------------CEC Rx Fail Command-------------\n"));
		iTE_Cec_Msg(("-----------------------------------------------------------------------ERROR-------------\n"));
		iTE_Cec_Msg(("-----------------------------------------------------------------------ERROR-------------\n"));
		return iTE_TRUE;
	}
	if(u8FreeSize < u8CecSize){
		return iTE_FALSE;
	}else{
		iTE_Cec_Msg(("iRxPush W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
		if(CEC_RX_QUEUE_SIZE - u8Wptr >= 3){
			pu8RxQ[u8Wptr++] = u8CecSize;
			pu8RxQ[u8Wptr++] = pu8Header[0];
			pu8RxQ[u8Wptr++] = pu8Header[1];
		iTE_Cec_Msg(("AiRxPush W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
		}else{
			pu8RxQ[u8Wptr++ & CEC_RX_QUEUE_MAX] = u8CecSize;
			pu8RxQ[u8Wptr++ & CEC_RX_QUEUE_MAX] = pu8Header[0];
			pu8RxQ[u8Wptr++ & CEC_RX_QUEUE_MAX] = pu8Header[1];
		iTE_Cec_Msg(("BiRxPush W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
		}
		u8Wptr &= CEC_RX_QUEUE_MAX;
		pu8Header[0] = 0;
//		pu8Header[1] = 0;
//		pu8Header[2] = 0;

		u8CecSize -= 3;

		if(u8CecSize){
			if(CEC_RX_QUEUE_SIZE - u8Wptr >= u8CecSize){
				Cec_Rb(0x50, u8CecSize, &pu8RxQ[u8Wptr]);
				u8Wptr += u8CecSize;
			}else{
				iTE_u8	u8Temp;
				for(u8Temp = 0; u8Temp < u8CecSize; u8Temp++){
					pu8RxQ[u8Wptr++ & CEC_RX_QUEUE_MAX] = Cec_R(0x50);
				}
			}
			u8Wptr &= CEC_RX_QUEUE_MAX;
		}

		g_stCecCur->stRxQ.u8Wptr = u8Wptr;
		iTE_Cec_Msg(("oRxPush W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
	}
	return iTE_TRUE;
}
//****************************************************************************
iTE_u1 Cec_RxCmdPull(void)
{
	iTE_u8	u8Temp, u8FillSize;
	iTE_u8	u8CmdSize;
	iTE_pu8	pu8CmdBuf = (iTE_pu8)&(g_stCecCur->stCecRxCmd.ucHeader);

	iTE_u8	u8Follower;
	iTE_u1	bVaildCmd;
	iTE_pu8	pu8RxQ = g_stCecCur->stRxQ.pu8Q;
	iTE_u8	u8Rptr = g_stCecCur->stRxQ.u8Rptr;
	iTE_u8	u8Wptr = g_stCecCur->stRxQ.u8Wptr;

	if(u8Wptr >= u8Rptr){
		u8FillSize = u8Wptr - u8Rptr;
	}else{
		u8FillSize = CEC_RX_QUEUE_SIZE + u8Wptr - u8Rptr;
	}

	while((g_stCecCur->bTxQFull==iTE_FALSE) && (u8FillSize >= 3)){
		iTE_Cec_Msg(("iRxPull W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
		u8CmdSize = pu8RxQ[u8Rptr++];

		if(u8CmdSize < 3){
			iTE_Cec_Msg(("**************************************ERROR: Rx Cmd size < 3\n"));
		}
		if(u8FillSize < u8CmdSize){
			u8CmdSize = u8FillSize;
			iTE_Cec_Msg(("**************************************ERROR: Rx Cmd Buffer ERROR\n"));
		}

		u8FillSize -= u8CmdSize;
		u8CmdSize--;

		iTE_Cec_Msg(("CEC_RX_CMD => \n"));
		if(CEC_RX_QUEUE_SIZE - u8Rptr > u8CmdSize){
			for(u8Temp = 0; u8Temp<u8CmdSize; u8Temp++){
				pu8CmdBuf[u8Temp] = pu8RxQ[u8Rptr++];
				iTE_Cec_Msg(("\t[%02d]=0x%02X\n", u8Temp, pu8CmdBuf[u8Temp]));
			}
		}else{
			for(u8Temp = 0; u8Temp < u8CmdSize; u8Temp++){
				pu8CmdBuf[u8Temp] = pu8RxQ[u8Rptr++ & CEC_RX_QUEUE_MAX];
				iTE_Cec_Msg(("\t[%02d]=0x%02X\n", u8Temp, pu8CmdBuf[u8Temp]));
			}
			u8Rptr &= CEC_RX_QUEUE_MAX;
		}
		g_stCecCur->stRxQ.u8Rptr = u8Rptr;

		g_u8Initiator = g_stCecCur->stCecRxCmd.ucHeader >> 4;
		u8Follower = g_stCecCur->stCecRxCmd.ucHeader & 0x0F;
		iTE_Cec_Msg(("CEC Initiator = %d, Follower = %d\n", g_u8Initiator, u8Follower));

#if 0	// why need these codes
		if(u8CmdSize == 1){
			if(ucMyselfAddr == u8Follower){
				Cec_SwitchPollingMsg(sReportPhyAdr);
			}
			break;
		}
#endif

		for(u8Temp=0; u8Temp < SizeOfLookUpTable_CEC; u8Temp++){
			if(g_stCecCur->stCecRxCmd.ucOpcode == LookUpTable_CEC[u8Temp].cmd){
				break;
			}
		}
		if(u8Temp < SizeOfLookUpTable_CEC){
			if(LookUpTable_CEC[u8Temp].size != u8CmdSize){
				iTE_Cec_Msg(("**************************************ERROR: Invalid Cmd Size\n"));
			}else{
				bVaildCmd = iTE_TRUE;
				switch(LookUpTable_CEC[u8Temp].header){
					case	eDirectly:
						if(u8Follower == DEVICE_ID_BROADCAST){
							bVaildCmd = iTE_FALSE;
						}
						break;
					case	eBroadcast:
						if(u8Follower != DEVICE_ID_BROADCAST){
							bVaildCmd = iTE_FALSE;
						}
						break;
					default:
						break;
				}
				if(bVaildCmd == iTE_TRUE){
					iTE_Cec_Msg(("\n CEC Receive Cmd => \t %s !!!\n", LookUpTable_CEC[u8Temp].CmdString));
					if( g_u8CecSel == CEC_RX_SEL){
#if 0	// for test, HDMI Tx 	send same command as HDMI Rx Received
						CecRx_PushCmd2CecTx(g_stCecCur->stCecRxCmd.ucHeader & 0x0F, g_stCecCur->stCecRxCmd.ucOpcode, u8CmdSize);
#endif
						switch(LookUpTable_CEC[u8Temp].eType){
#if 1
							case	eCDCFeature:
									CecRx_CdcFeature();
								break;
							case	eOTPFeature:
									CecRx_OtpFeature();
								break;
							case	eRoutingCtlFeature:
									CecRx_RoutingCtlFeature();
								break;
							case	eStandByFeature:
									CecRx_StandbyFeature();
								break;
							case	eSysInfoFeature:
									CecRx_SysInfoFeature();
								break;
							case	eVenSpecCmdFeature:
									CecRx_VenSpecCmdFeature(u8CmdSize);
								break;
							case	eDeckCtlFeature:
									CecRx_DeckCtlFeature();
								break;
							case	eRemoteCtlPassthroughFeature:
									CecRx_RemoteCtlPassthroughFeature();
								break;
							case	ePowerStatusFeature:
									CecRx_PwrStatusFeature();
								break;
							case	eSysAudioCtlFeature:
									CecRx_SysAudioCtlFeature();
								break;
							case	eACRFeature:
									CecRx_ARCFeature();
								break;
							case	eHECFeature:
									CecRx_HECFeature();
								break;
#endif
							case	eNoneFeature:
							default:
								break;
						}
					}else{
						switch(LookUpTable_CEC[u8Temp].eType){
#if 1
							case	eCDCFeature:
									CecTx_CdcFeature();
								break;
							case	eOTPFeature:
									CecTx_OtpFeature();
								break;
							case	eRoutingCtlFeature:
									CecTx_RoutingCtlFeature();
								break;
							case	eStandByFeature:
									CecTx_StandbyFeature();
								break;
							case	eSysInfoFeature:
									CecTx_SysInfoFeature();
								break;
							case	eVenSpecCmdFeature:
									CecTx_VenSpecCmdFeature(u8CmdSize);
								break;
							case	eDeckCtlFeature:
									CecTx_DeckCtlFeature();
								break;
							case	eRemoteCtlPassthroughFeature:
									CecTx_RemoteCtlPassthroughFeature();
								break;
							case	ePowerStatusFeature:
									CecTx_PwrStatusFeature();
								break;
							case	eSysAudioCtlFeature:
									CecTx_SysAudioCtlFeature();
								break;
							case	eACRFeature:
									CecTx_ARCFeature();
								break;
							case	eHECFeature:
									CecTx_HECFeature();
								break;
#endif
							case	eNoneFeature:
							default:
								break;
						}
					}
					iTE_Cec_Msg(("\n"));
				}
			}
		}else{
			iTE_Cec_Msg(("**************************************WARING: UnSupport OpCode : 0x%02X\n", g_stCecCur->stCecRxCmd.ucOpcode));
		}
		iTE_Cec_Msg(("oRxPull W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
	}

	if((g_stCecCur->bTxQFull==iTE_FALSE)  && u8FillSize){
		iTE_Cec_Msg(("**************************************ERROR: Rx Cmd Buffer ERROR, Reset Buffer\n"));
		g_stCecCur->stRxQ.u8Wptr = g_stCecCur->stRxQ.u8Rptr = 0;
		return iTE_FALSE;
	}else{
		return iTE_TRUE;
	}
}
//****************************************************************************
void Cec_TxFire(void)
{
#if 0	// for Disable retry for polling message
	iTE_u8	u8Temp0, u8Temp1;

	u8Temp0 = Cec_R(0x10);
	u8Temp1 = u8Temp0 >> 4;
	if((u8Temp0 & 0x0F) == u8Temp1){
		u8Temp0 = 0x20;				// disable Cec Fail Retry
	}else{
		u8Temp0 = 0x00;				// enable Cec fail Retry
	}
	Cec_Set(0x0A, 0x20, u8Temp0);
#endif
	iTE_u8	u8CecCmd[20];
	Cec_Rb(0x10, 20, u8CecCmd);
	iTE_Cec_Msg(("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X \n%02X %02X\n",
		u8CecCmd[0], u8CecCmd[1], u8CecCmd[2], u8CecCmd[3], u8CecCmd[4], u8CecCmd[5], u8CecCmd[6], u8CecCmd[7], u8CecCmd[8], u8CecCmd[9],
		u8CecCmd[10], u8CecCmd[11], u8CecCmd[12], u8CecCmd[13], u8CecCmd[14], u8CecCmd[15], u8CecCmd[16], u8CecCmd[17], u8CecCmd[18], u8CecCmd[19]));
	Cec_Set(0x08, 0x88, 0x08);         	//FireCmd
	Cec_Set(0x08, 0x88, 0x88);         	//FireCmd
	g_stCecCur->bTxCecDone = 0;
}
//****************************************************************************
void Cec_TxPolling(iTE_u8 u8LogicalAdr)
{
	Cec_W(0x10, u8LogicalAdr<<4 | u8LogicalAdr);
	Cec_W(0x23, 0x01);
	Cec_TxFire();
}
//****************************************************************************
void Cec_TxFeatureAbort(iTE_u8 CecRxCmd, CecAbortReson eAbortReson)
{
	g_stCecCur->stCecTxCmd.ucOperand1 = CecRxCmd;
	g_stCecCur->stCecTxCmd.ucOperand2 = eAbortReson;
	Cec_TxCmdPush(g_u8Initiator, eFeatureAbort, 4);
}
//****************************************************************************

//****************************************************************************
void Cec_TxReportPA(void)
{
#if 0
	g_stCecCur->stCecTxCmd.ucOperand1 = g_stCecCur->u8PaL;
	g_stCecCur->stCecTxCmd.ucOperand2 = g_stCecCur->u8PaH;
	if(g_u8CecSel == CEC_RX_SEL){
		g_stCecCur->stCecTxCmd.ucOperand3 = IT6662_RX_TYPE;
	}else{
		g_stCecCur->stCecTxCmd.ucOperand3 = IT6662_TX_TYPE;
	}
	Cec_TxCmdPush(DEVICE_ID_BROADCAST, eReportPhysicalAddress, 5);
#else
	iTE_u8	u8ReportPA[5];
	u8ReportPA[0] = (g_stCecCur->u8MyLogAdr << 4) | DEVICE_ID_BROADCAST;
	u8ReportPA[1] = eReportPhysicalAddress;
	u8ReportPA[2] = g_stCecCur->u8PaL;
	u8ReportPA[3] = g_stCecCur->u8PaH;
	if(g_u8CecSel == CEC_RX_SEL){
		u8ReportPA[4] = IT6662_RX_TYPE;
	}else{
		u8ReportPA[4] = IT6662_TX_TYPE;
	}
	Cec_Wb(0x10, 0x05, u8ReportPA);
	Cec_W(0x23, 0x05);

	Cec_TxFire();
		iTE_Cec_Msg(("********************************************************************************************\n"));
#endif
}
//****************************************************************************
void Cec_TxCmdPush(iTE_u8 ucFollower, eCEC_CMD TxCmd, iTE_u8 u8CecSize)
{
	iTE_pu8	pu8TxQ = g_stCecCur->stTxQ.pu8Q;
	iTE_u8	u8Rptr = g_stCecCur->stTxQ.u8Rptr;
	iTE_u8	u8Wptr = g_stCecCur->stTxQ.u8Wptr;
	iTE_u8	u8FreeSize;
	iTE_pu8	pu8Src = (iTE_pu8)(&g_stCecCur->stCecTxCmd.ucSize);

	if((g_stCecCur->bTxQFull == iTE_TRUE) && u8CecSize){
		iTE_Cec_Msg(("**************************************ERROR: CEC Tx Buffer OverFlow, 0x%02X, 0x%02x, 0x%02X\n", pu8Src[0], pu8Src[1], pu8Src[2]));
		iTE_Cec_Msg(("**************************************ERROR: CEC Tx Buffer OverFlow, 0x%02X, 0x%02x, 0x%02X\n", pu8Src[0], pu8Src[1], pu8Src[2]));
		iTE_Cec_Msg(("**************************************ERROR: CEC Tx Buffer OverFlow, 0x%02X, 0x%02x, 0x%02X\n", pu8Src[0], pu8Src[1], pu8Src[2]));
		iTE_Cec_Msg(("**************************************ERROR: CEC Tx Buffer OverFlow, 0x%02X, 0x%02x, 0x%02X\n", pu8Src[0], pu8Src[1], pu8Src[2]));
	}

	if(u8CecSize){
		g_stCecCur->stCecTxCmd.ucSize = ++u8CecSize;
		g_stCecCur->stCecTxCmd.ucHeader = ucFollower; // + (g_stCecCur->u8MyLogAdr<<4);
		g_stCecCur->stCecTxCmd.ucOpcode = TxCmd;
	}
	iTE_Cec_Msg(("iTxPush W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));

	if(u8Rptr > u8Wptr){
		u8FreeSize = u8Rptr - u8Wptr;
	}else{
		u8FreeSize = CEC_TX_QUEUE_SIZE + u8Rptr - u8Wptr;
	}

	if(u8FreeSize < u8CecSize){
		g_stCecCur->bTxQFull = iTE_TRUE;
	}else{
		iTE_u8	u8Temp;
		g_stCecCur->bTxQFull = iTE_FALSE;
		if(CEC_TX_QUEUE_SIZE - u8Wptr >= u8CecSize){
			for(u8Temp=0; u8Temp < u8CecSize; u8Temp++){
				pu8TxQ[u8Wptr++] = pu8Src[u8Temp];
			}
		}else{
			for(u8Temp=0; u8Temp < u8CecSize; u8Temp++){
				pu8TxQ[u8Wptr++ & CEC_TX_QUEUE_MAX] = pu8Src[u8Temp];
			}
		}
		u8Wptr &= CEC_TX_QUEUE_MAX;
	}

	g_stCecCur->stTxQ.u8Wptr = u8Wptr;
	iTE_Cec_Msg(("oTxPush W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
#if 0
	if(Cec_R(0x44) & 0x01){
		if(Cec_R(0x4C) & ~(Cec_R(0x06))){
			Cec_TxCmdPull();
		}
	}
#endif
}
//****************************************************************************
iTE_u1 Cec_TxCmdPull(void)
{
	iTE_u8	u8Temp, u8FillSize;
	iTE_u8	u8CmdSize;
//	iTE_u8	u8CurCecSel = g_u8CecTxSel;

	iTE_pu8	pu8TxQ = g_stCecCur->stTxQ.pu8Q;
	iTE_u8	u8Rptr = g_stCecCur->stTxQ.u8Rptr;
	iTE_u8	u8Wptr = g_stCecCur->stTxQ.u8Wptr;

	if(u8Wptr >= u8Rptr){
		u8FillSize = u8Wptr - u8Rptr;
	}else{
		u8FillSize = CEC_TX_QUEUE_SIZE + u8Wptr - u8Rptr;
	}

	if(u8FillSize >= 3){
		iTE_Cec_Msg(("iTxPull W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
		u8CmdSize = pu8TxQ[u8Rptr++];
		if(u8CmdSize < 3){
			iTE_Cec_Msg(("**************************************ERROR: Tx Cmd size < 3\n"));
		}
		if(u8FillSize < u8CmdSize){
			iTE_Cec_Msg(("**************************************ERROR: Tx Buffer ERROR\n"));
			u8CmdSize = u8FillSize;
		}
		u8FillSize -= u8CmdSize;
		u8CmdSize--;
		u8Rptr &= CEC_RX_QUEUE_MAX;
#if 0
		if(pu8TxQ[u8Rptr] & 0x80){
			Cec_TxSel((pu8TxQ[u8Rptr] & 0x30) >> 4);
		}
#endif
		pu8TxQ[u8Rptr] |= (g_stCecCur->u8MyLogAdr<<4);

		iTE_Cec_Msg(("CEC_TX_CMD => \n"));
		if(CEC_TX_QUEUE_SIZE - u8Rptr > u8CmdSize){
			Cec_Wb(0x10, u8CmdSize, &pu8TxQ[u8Rptr]);
			for(u8Temp = 0; u8Temp < u8CmdSize; u8Temp++){
				iTE_Cec_Msg(("\t[%02d]=0x%02X\n", u8Temp, pu8TxQ[u8Rptr+u8Temp]));
			}
			u8Rptr += u8CmdSize;
		}else{
			for(u8Temp = 0; u8Temp < u8CmdSize; u8Temp++){
				Cec_W(0x10+u8Temp,  pu8TxQ[u8Rptr++& CEC_RX_QUEUE_MAX]);
				iTE_Cec_Msg(("\t[%02d]=0x%02X\n", u8Temp, Cec_R(0x10+u8Temp)));
			}
			u8Rptr &= CEC_RX_QUEUE_MAX;
		}
		Cec_W(0x23, u8CmdSize);
		g_stCecCur->stTxQ.u8Rptr = u8Rptr;
		if(g_u8CecSel == CEC_RX_SEL){
			Cec_TxFire();
		}
		iTE_Cec_Msg(("oTxPull W=0x%02X, R=0x%02X\n", u8Wptr, u8Rptr));
		return iTE_TRUE;
	}else{
		if(u8FillSize){
			iTE_Cec_Msg(("**************************************ERROR: Tx Buffer ERROR\n => Reset Buffer"));
			g_stCecCur->stTxQ.u8Wptr = g_stCecCur->stTxQ.u8Rptr = 0;
		}
		return iTE_FALSE;
	}

}
//****************************************************************************
void Cec_TxSel(iTE_u8 u8TxSel)
{
#if 0
	iTE_u16	u16PhysicalAddr;
	iTE_u8	u8Temp;
#endif
	if(g_u8CecSel == CEC_RX_SEL)
		return;

	g_stCecCur->u8MyLogAdr = g_stCecCur->u8CecLA[u8TxSel];
	Cec_W(0x22, g_stCecCur->u8MyLogAdr);
	Cec_Set(0x07, 0x03, u8TxSel);
	g_u8CecTxSel = u8TxSel;

#if 0
	u16PhysicalAddr = (g_stEdidInfo[u8TxSel].pstVsdbInfo.u8VsdbPaL<<8) | g_stEdidInfo[u8TxSel].pstVsdbInfo.u8VsdbPaH;

	for(u8Temp=0; u8Temp<16; u8Temp+=4){
		if(u16PhysicalAddr & (0x0F << u8Temp)){
			break;
		}
	}
	if(u8Temp){
		u8Temp -= 4;
		u16PhysicalAddr |= (0x01 << u8Temp);
		g_stCecCur->u8PaH = u16PhysicalAddr & 0xFF;
		g_stCecCur->u8PaL = u16PhysicalAddr >> 8;
	}else{
		g_stCecCur->u8PaH = 0xFF;
		g_stCecCur->u8PaL = 0xFF;
	}
#else
		g_stCecCur->u8PaL = g_stEdidInfo[u8TxSel].pstVsdbInfo.u8VsdbPaL;
		g_stCecCur->u8PaH = g_stEdidInfo[u8TxSel].pstVsdbInfo.u8VsdbPaH;
#endif
}
//****************************************************************************
void Cec_BlockSel(iTE_u8 u8CecSel)
{

	iTE_u8	u8Temp = 0;
#if (USING_1to8 == iTE_TRUE)
	switch(g_u8CurDev){
		case	IT6662_B:
				u8Temp = 1;
			break;
		case	IT6662_C:
				u8Temp = 2;
			break;
		case	IT6662_A:
		default:
				u8Temp = 0;
			break;
	}
#endif

	g_u8CecSel = u8CecSel;
	if(g_u8CecSel == CEC_RX_SEL){
		g_stCecCur = &g_stCecRx[u8Temp];
	}else{
		g_stCecCur = &g_stCecTx[u8Temp];
	}

}

#endif
