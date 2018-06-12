///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CecTx_Handler.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CecTx_Handler.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#include "it6662/include/Cec.h"

#if (USING_CEC == iTE_TRUE)
extern stCecVar *g_stCecCur;
extern iTE_u8	g_u8Initiator;
extern iTE_u8 g_u8CecTxSel;
extern iTE_u8 g_u8TxSel, g_u8TxShift;
extern IT6662_Var	*stIt6662;

extern void HdmiTx_Select(iTE_u8 u8HdmiTxSel);
extern void HdmiTx_PortDisable(void);

//****************************************************************************
void CecTx_CdcFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case	eFeatureAbort:
			return;
		case	eAbort:
				Cec_TxFeatureAbort(eAbort, eRefused);
			break;
	}
}
//****************************************************************************
void CecTx_OtpFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case	eImageViewOn:
				// ToDo
			break;
		case	eTextViewOn:
				// ToDo
			break;
		case	eActiveSource:
				// ToDo
			break;
	}
}
//****************************************************************************
void CecTx_RoutingCtlFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case	eRoutingChange:
				// ToDo
			break;
		case	eRoutingInformation:
				// ToDo
			break;
		case	eInactiveSource:
				if(g_stCecCur->u8MyLogAdr == DEVICE_ID_TV){
					// ToDo
				}
			break;
		case	eRequestActiveSource:	// <DB> <0x85> , need to reply <active source> by CEC active device
				if((g_stCecCur->u8MyLogAdr != DEVICE_ID_TV) && (g_stCecCur->u8MyLogAdr != DEVICE_ID_BROADCAST)){
					if(stIt6662->u8TxPortEn & (1<<g_u8CecTxSel)){	// Tx Active
						g_stCecCur->stCecTxCmd.ucOperand1 = g_stCecCur->u8PaL;
						g_stCecCur->stCecTxCmd.ucOperand2 = g_stCecCur->u8PaH;
						Cec_TxCmdPush(DEVICE_ID_BROADCAST, eActiveSource, 4);
					}
				}
			break;
		case	eSetStreamPath:		// <DB> <0x86> <Physical Address> , example:[0x0F 0x86 AB CD] TV request a streaming path from the specified Physical Address
				// ToDo
			break;
	}
}
//****************************************************************************
void CecTx_StandbyFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case	eStandBy:		// <MS> <0X36> or <DB> <0x36> ,
				if((g_stCecCur->u8MyLogAdr != DEVICE_ID_TV) && (g_stCecCur->u8MyLogAdr != DEVICE_ID_BROADCAST)){
					iTE_u8	u8CurTx = g_u8TxSel;
					HdmiTx_Select(g_u8CecTxSel);
					HdmiTx_PortDisable();
					stIt6662->u8TxHpdStatus &= ~g_u8TxShift;
					HdmiTx_Select(u8CurTx);
				}
			break;
	}
}
//****************************************************************************
void CecTx_SysInfoFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case	eCECVersioin:				//<MS> <0x9E> <CEC version>
				iTE_Cec_Msg(("\t CEC Version =%bX!!!\n",g_stCecCur->stCecRxCmd.ucOperand1));
			break;
		case	eGetCECVersion :   		//<MS> <0x9F> , example: [0x05 0x9F] TV request AVR to reply CEC version
				g_stCecCur->stCecTxCmd.ucOperand1 = 0x04;
				Cec_TxCmdPush(g_u8Initiator, eCECVersioin, 3);
			break;
		case	eGivePhysicalAddress:		//<MS> <0x83> , example:[0x05 0x83] TV request AVR to return Physical address
				Cec_TxReportPA();
			break;
		case	eReportPhysicalAddress:	// <DB> <0x84> <Physicall Adress> <Device type>, example:[0x5F 0x84 AB CD 0]
				iTE_Cec_Msg((" PhyAdr = %02X%02X, DEV_TYP = %d \n", g_stCecCur->stCecRxCmd.ucOperand1, g_stCecCur->stCecRxCmd.ucOperand2, g_stCecCur->stCecRxCmd.ucOperand3));
				// ToDo
			break;
		case eGetMenuLanguage :	//Get Menu Language
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eGetMenuLanguage, eRefused);
				}
			break;
		case eSetMenuLanguage :	//Set Menu Language
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eSetMenuLanguage, eRefused);
				}
			break;
	}
}
//****************************************************************************
void CecTx_VenSpecCmdFeature(iTE_u8 u8CmdSize)
{
	iTE_u8	u8Temp;

	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case eVendorCommandWithID :	//Vendor Command With ID			// Vendor Specific
				if(u8CmdSize > 16){
					iTE_Cec_Msg(("Cmd Size exceed 11 Bytes\n"));
				}else{
					iTE_Cec_Msg(("Vendor ID: %02X %02X %02X\n", g_stCecCur->stCecRxCmd.ucOperand1, g_stCecCur->stCecRxCmd.ucOperand2, g_stCecCur->stCecRxCmd.ucOperand3));
					for(u8Temp=5; u8Temp<u8CmdSize; u8Temp++){
						iTE_Cec_Msg(("%02X\n",*((&g_stCecCur->stCecRxCmd)+u8Temp)));
					}
					// ToDo
				}
			break;
		case eDeviceVendorID :   	//<DB> <0x87> <Vendor ID>
				iTE_Cec_Msg(("Vendor ID: %02X %02X %02X\n", g_stCecCur->stCecRxCmd.ucOperand1, g_stCecCur->stCecRxCmd.ucOperand2, g_stCecCur->stCecRxCmd.ucOperand3));
			break;
		case eGiveDeviceVendorID :	//<MS> <0x8C>	//need to return <Device Vendor ID>
				g_stCecCur->stCecTxCmd.ucOperand1 = CEC_DEV_VENDOR_ID_0;
				g_stCecCur->stCecTxCmd.ucOperand2 = CEC_DEV_VENDOR_ID_1;
				g_stCecCur->stCecTxCmd.ucOperand3 = CEC_DEV_VENDOR_ID_2;
				Cec_TxCmdPush(DEVICE_ID_BROADCAST, eDeviceVendorID, 5);
			break;
		case eVendorCommand :    	//Vendor Command				// Vendor Specific
				if(u8CmdSize > 16){
					iTE_Cec_Msg(("Cmd Size exceed 14 Bytes\n"));
				}else{
					for(u8Temp=2; u8Temp<u8CmdSize; u8Temp++){
						iTE_Cec_Msg(("%02X\n",*((&g_stCecCur->stCecRxCmd)+u8Temp)));
					}
				}
				// ToDo
			break;
		case eRemoteButtonDown :    	//Vendor Remote Button Down		// Vendor Specific
				if(u8CmdSize > 16){
					iTE_Cec_Msg(("Cmd Size exceed 14 Bytes\n"));
				}else{
					for(u8Temp=2; u8Temp<u8CmdSize; u8Temp++){
						iTE_Cec_Msg(("%02X\n",*((&g_stCecCur->stCecRxCmd)+u8Temp)));
					}
				}
				// ToDo
			break;
		case eRemoteButtonUp :    	//Vendor Remote Button Up		// Vendor Specific
			break;
	}
}
//****************************************************************************
void CecTx_DeckCtlFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case ePlay :	//Play							 //<MS> <featuer about> <op code> <abort reason>
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(ePlay, eRefused);
				}
			break;
		case eDeckControl :	//Deck control				//<MS> <featuer about> <op code> <abort reason>
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eDeckControl, eRefused);
				}
			break;
		case eGiveDeckStatus :	//Give Deck Status
				iTE_Cec_Msg(("\t[%02X]\n", g_stCecCur->stCecRxCmd.ucOperand1));
				if(0){
					// ToDo
				}else{
					g_stCecCur->stCecTxCmd.ucOperand1 = 0x1F;	// Other status
					Cec_TxCmdPush(g_u8Initiator, eDeckStatus, 3);
				}
			break;
		case eDeckStatus :	//Deck Status				//<MS> <featuer about> <op code> <abort reason>
				iTE_Cec_Msg(("\t[%02X]\n", g_stCecCur->stCecRxCmd.ucOperand1));
			break;
	}
}
//****************************************************************************
void CecTx_RemoteCtlPassthroughFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case eUserPressed :	//<MS> <0x44> <UI Command>
				// ToDo
			break;
		case eUserReleased :	//<MS> <0X45>
				// ToDo
			break;
	}
}
//****************************************************************************
void CecTx_PwrStatusFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case eGiveDevicePowerStatus:	// <MS> <0x8F Give Device Power Status> --> <MS> <0x90 Report Power Status> < 0~3 >
				if(0){
					// ToDo
				}else{
					g_stCecCur->stCecTxCmd.ucOperand1 = 0x00;			// 0: on , 1: standby, 2: in transition standby to on , 3: in transition on to standby
				}
				Cec_TxCmdPush(g_u8Initiator, eReportPowerStatus, 3);
			break;
		case eReportPowerStatus:
				iTE_Cec_Msg(("%02X\n", g_stCecCur->stCecRxCmd.ucOperand1));
			break;
	}
}
//****************************************************************************
void CecTx_SysAudioCtlFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
#if 1
		case eAudioModeRequest :		//<MS> <0x70 Audio Mode Request > <Phy Adr> ,example:[0x05 0x70 00 00] TV send Audio mode request ON to AVR
		case eGiveAudioStatus:			//<MS> <0x71>
		case eSetSystemAudioMode :		//<MS> <0X72> <Audio on/off>
		case eReportAudioStatus:			//<MS> <0x7A> <Volume and Mute Status>
		case eReportAudioDescriptor:
		case eGiveSystemAudIoModeStatus :	//<MS> <0x7D >  --> <MS> <0x7E > <Audio On / Off>
		case eSystemAudIoModeStatus :		//<MS> <0x7E> <System Audio Status>
		case eRequestAudioDescriptor :		//<MS> <0xA4> <Audio Format ID and Code>
				iTE_Cec_Msg(("\t SysAudioCtlFeature !!!\n"));
				Cec_TxFeatureAbort(g_stCecCur->stCecRxCmd.ucOpcode, eRefused);
			break;
#else
		case eAudioModeRequest :
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eAudioModeRequest, eRefused);
				}
			break;
		case eGiveAudioStatus:			//<MS> <0x71>
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eGiveAudioStatus, eRefused);
				}
			break;
		case eSetSystemAudioMode :		//<MS> <0X72> <Audio on/off>
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eSetSystemAudioMode, eRefused);
				}
			break;
		case eReportAudioStatus:
				iTE_Cec_Msg(("\t[0x%02X]\n", g_stCecCur->stCecRxCmd.ucOperand1));
				// ToDo
			break;
		case eGiveSystemAudIoModeStatus :	//<MS> <0x7D >  --> <MS> <0x7E > <Audio On / Off>
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eGiveSystemAudIoModeStatus, eRefused);
				}
			break;
		case eSystemAudIoModeStatus :		//<MS> <0x7E> <System Audio Status>
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eSystemAudIoModeStatus, eRefused);
				}
			break;
		case eRequestAudioDescriptor :		//<MS> <0xA4> <Audio Format ID and Code>
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eRequestAudioDescriptor, eRefused);
				}
			break;
#endif
	}
}
//****************************************************************************
void CecTx_ARCFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
#if 1
		case eInitiateARC :
		case eReportARCInitiated :
		case eReportARCTerminated :
		case eRequestARCInitiation :
		case eRequestARCTermination :    //Request ARC Termination
		case eTerminateARC :    //Termiate ARC
				Cec_TxFeatureAbort(g_stCecCur->stCecRxCmd.ucOpcode, eRefused);
			break;
#else
		case eInitiateARC :
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eInitiateARC, eRefused);
				}
			break;
		case eReportARCInitiated :
				// ToDo
			break;
		case eReportARCTerminated :
				// ToDo
			break;
		case eRequestARCInitiation :
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eRequestARCInitiation, eRefused);
				}
			break;
		case eRequestARCTermination :    //Request ARC Termination
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eRequestARCTermination, eRefused);
				}
			break;
		case eTerminateARC :    //Termiate ARC
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eTerminateARC, eRefused);
				}
			break;
#endif
	}
}
//****************************************************************************
void CecTx_HECFeature(void)
{
	switch(g_stCecCur->stCecRxCmd.ucOpcode){
		case eCDC:
#if 1
				if(0){
					// ToDo
				}else{		// Not supported
					Cec_TxFeatureAbort(eCDC, eRefused);
				}
#else
			switch( g_stCecCmd.ucOperand3){
				case eHEC_InquirState:
						iTE_Cec_Msg(("\nReceive !!!CEC_HEC_InquireState !!!\n"));
//						SwitchHECState(sHEC_InquirState);
					break;
				case eHEC_ReportState:
						iTE_Cec_Msg(("\nReceive !!!CEC_HEC_Report_State !!!\n"));
						SwitchHECState(sHEC_ReportState);
					break;
				case eHEC_SetStateAdjacent:
						iTE_Cec_Msg(("\nReceive !!!CEC_HEC_SetStateAdjacent !!!\n"));
						SwitchHECState(sHEC_SetStateAdjacent);
					break;
				case eHEC_SetState:
						iTE_Cec_Msg(("\nReceive !!!CDC_HEC_SetState !!!\n"));
						SwitchHECState(sHEC_SetState);
					break;
				case eHEC_RequestDeactivation:
						iTE_Cec_Msg(("\nReceive !!!CEC_HEC_RequestDeactivation !!!\n"));
						SwitchHECState(sHEC_RequestDeactivation);
					break;
				case eHEC_NotifyAlive:
						iTE_Cec_Msg(("\nReceive !!!CEC_HEC_NotifyAlive !!!\n"));
						SwitchHECState(sHEC_NotifyAlive);
					break;
				case eHEC_Discover:
						iTE_Cec_Msg(("\nReceive !!!CEC_HEC_Discover !!!\n"));
						SwitchHECState(sHEC_Discover);
					break;
				case eHEC_HPDSetState:
						iTE_Cec_Msg(("\nReceive !!!CDC_HPD_SetState !!!\n"));
						SwitchHECState(sHEC_HPDSetState);
					break;
				case eHEC_HPDReportState:
						iTE_Cec_Msg(("\nReceive CEC_HDP_ReportState !!!\n"));
						SwitchHECState(sHEC_HPDReportState);
					break;
				default :
						iTE_Cec_Msg(("\nReceive CDC UnRecongnized OpCode !!!\n"));
					break;
			}
#endif
			break;
	}
}

#endif
//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
