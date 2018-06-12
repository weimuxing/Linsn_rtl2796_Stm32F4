///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CEC_typedef.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CEC_typedef.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _CEC_TYPEDEF_H_
#define _CEC_TYPEDEF_H_

#include "it6662/include/iTE_typedef.h"
#include "it6662/include/IT6662_config.h"

typedef struct
{
	iTE_u8	ucSize;
	iTE_u8	ucHeader;
	iTE_u8	ucOpcode;
	iTE_u8	ucOperand1;
	iTE_u8	ucOperand2;
	iTE_u8	ucOperand3;
	iTE_u8	ucOperand4;
	iTE_u8	ucOperand5;
	iTE_u8	ucOperand6;
	iTE_u8	ucOperand7;
	iTE_u8	ucOperand8;
	iTE_u8	ucOperand9;
	iTE_u8	ucOperand10;
	iTE_u8	ucOperand11;
	iTE_u8	ucOperand12;
	iTE_u8	ucOperand13;
	iTE_u8	ucOperand14;
	iTE_u8	ucOperand15;
	iTE_u8	ucOperand16;
}CEC_FRAME, *P_CEC_FRAME;

#define	Q_SIZE		(10)
typedef struct {
	iTE_u8	ucRptr;
	iTE_u8	ucWptr;
	CEC_FRAME	QBuf[Q_SIZE];
}CEC_QBuf, *pCEC_QBuf;

typedef enum _CEC_TX_StateType
{
	sCECOff=0,
	sCECcmdGet,
	sCECTransfer,
	sCECCheckResult,
	sCECReceiverFail,
	sCECReceiverAck,
	sCECReceiverNack,
	sCECOk,
	sCECNone
} CEC_TX_StateType;


typedef enum _ARC_StateType
{
	sRequestARCInitiation=0,
	sInitiateARC,
	sReportARCInitiated,

	sRequestARCTermination,
	sTerminateARC,
	sReportARCTermination,

	sARCNone
} ARC_StateType;

typedef enum _HEC_StateType
{
	sHEC_InquirState			=0x00,
	sHEC_ReportState			=0x01,
	sHEC_SetStateAdjacent		=0x02,
	sHEC_SetState				=0x03,
	sHEC_RequestDeactivation	=0x04,
	sHEC_NotifyAlive			=0x05,
	sHEC_Discover				=0x06,
	sHEC_HPDSetState			=0x10,
	sHEC_HPDReportState		=0x11,
	sHEC_None
} HEC_StateType;


typedef enum _SystemInfoFeature_StateType
{
	sCECVersioin			=0x9E,
	sGetCECVersion			=0x9F,
	sGivePhysicalAddress	=0x83,	// follower:All
	sGetMenuLanguage		=0x91,	// follower:TV
	sReportPhysicalAddress	=0x84,	// follower:TV
	sSetMenuLanguage		=0x32,	// follower:All,		Initiator:TV
	sSIFnone
} SystemInfoFeature_StateType;


typedef enum _SystemAudioControl_StateType
{

	sComesOutOfStandby=0,
	sSetSystemAudioModeToTV,
	sWaitingFeatureAbort,
	sSetSystemAudioModeToAll,
	sSACnone
} SystemAudioControl_StateType;




typedef enum _OneTouchPlay_StateType
{
	sActiveSource,
	sImageViewOn,
	sTextViewOn,
	sOTNone
} OneTouchPlay_StateType;

typedef enum _LogicAddressing_StateType
{
	sReportPhysicalAddressTransfer,
	sReportPhysicalAddressReceived,
	sPollingMessage,
	sPollingResult,
	sLANone
} LogicAddressing_StateType;


typedef enum _PollingMessage_StateType
{
	sPollingMessage1,
	sPollingMessage2,
	sPollingMessage3,
	sReportPhyAdr,
	sReportDeviceVendorID,
	sPMnone
} PollingMessage_StateType;

typedef enum _CEC_Logic_DEVICE_ID_t
{
	DEVICE_ID_TV=0,
	DEVICE_ID_RECORDING1,		// 1
	DEVICE_ID_RECORDING2,		// 2
	DEVICE_ID_TUNER1,			// 3
	DEVICE_ID_PLAYBACK1,		// 4
	DEVICE_ID_AUDIO,			// 5
	DEVICE_ID_TUNER2,			// 6
	DEVICE_ID_TUNER3,			// 7
	DEVICE_ID_PLAYBACK2,		// 8
	DEVICE_ID_RECORDING3,		// 9
	DEVICE_ID_TUNER4,			// 10
	DEVICE_ID_PLAYBACK3,		// 11
	DEVICE_ID_RESERVED1,		// 12
	DEVICE_ID_RESERVED2,		// 13
	DEVICE_ID_FREEUSE,			// 14
	DEVICE_ID_BROADCAST,		// 15
	DEVICE_ID_MAX
} CEC_Logic_DEVICE_ID_t;

typedef enum _CEC_DEVICE_TYPE
{
	CEC_DEV_TYPE_TV=0,
	CEC_DEV_TYPE_RECORDING_DEV=1,
	CEC_DEV_TYPE_RESERVED=2,
	CEC_DEV_TYPE_TUNER=3,
	CEC_DEV_TYPE_PLAYBACK_DEV=4,
	CEC_DEV_TYPE_AUDIO_SYS=5,
	CEC_DEV_TYPE_PURE_CEC_SWITCH=6,
	CEC_DEV_TYPE_VIDEO_PROCESSOR=7
} CEC_DEVICE_TYPE;

 typedef struct {
// 	CEC_Logic_DEVICE_ID_t	LogicDevice;
	iTE_u8						PhyicalAddr1;
	iTE_u8						PhyicalAddr2;
	iTE_u8						Active;
 }CECDevice, *pCECDevice;


typedef struct {
//	iTE_u8 	HDMIInput		: 14;	//bit 0~13	 , HDMI Input 1 as bit13, HDMI Input 2 as bit12
	iTE_u8 	HECActivation	: 1;		//bit 14	, '0' as HEC Inactive  , '1' as HEC Active
	iTE_u8		Reserved		: 1;		//bit 15
}HECActivationField, *pHECActivationField;

typedef struct {
//	iTE_u8 	HDMIInput		: 14;	//bit 0~13	 , HDMI Input 1 as bit13, HDMI Input 2 as bit12
	iTE_u8 	HECSupport		: 1;		//bit 14	, '0' as HEC Not Support  , '1' as HEC Support
	iTE_u8		Reserved		: 1;		//bit 15
}HECSupportField, *pHECSupportField;

typedef struct {
	iTE_u8 	CDCErrorCode	: 2;		//bit 0~1	, HDMI Input 1 as bit13, HDMI Input 2 as bit12
	iTE_u8 	ENCState		: 2;		//bit 2~3	, '00' Ext Con Not Support  , '01' Ext Con Inactive ,'10' Ext Con Active
	iTE_u8		HostState		: 2;		//bit 4~5	, '00' Host Not Support  , '01' Host Inactive ,'10' Host Active
	iTE_u8		HECState		: 2;		//bit 6~7	, '00' HEC Not Support  , '01' HEC Inactive ,'10' HEC Active
}HECStateField, *pHECStateField;

typedef enum {
	eCPEDID_disable=0,
	eCPEDID_ENABLE=1,
	eCPEDID_DISABLE_ENABLE=2,
	eEDID_DISABLE=3,
	eEDID_ENABLE=4,
	eEDID_DISABLE_ENABLE=5
}HPDStateType, *pHPDStateType;

typedef enum {
	eNoError=0,
	eNoRequestedCapability=1,
	eNoCapableToCarryOut=2,
	eOtherError=3,
	eNoErrorNoVideoStream=4
}HPDErrorCodeType, *pHPDErrorCodeType;

typedef enum{
	eUnrecognizedOpcode = 0,
	eNotIncorrectModeToRespond = 1,
	eCannotProvideSource = 2,
	eInvalidOperand = 3,
	eRefused = 4,
	eUnableToDetermine = 5
}CecAbortReson;

typedef enum _CEC_COMMAND
{
	eFeatureAbort			=0x00,

// One Touch Play Feature
	eActiveSource			=0x82,	// follower:TV, switch	--> Broadcst , Directly address
	eImageViewOn			=0x04,	// follower:TV, switch	--> Broadcst
	eTextViewOn			=0x0D,	// follower:TV

// Routing Control Feature
	eRoutingChange			=0x80,	// follower:TV, switch	--> Broadcst
	eRoutingInformation		=0x81,	// follower:switch		--> Broadcst
	eRequestActiveSource	=0x85,	// follower:switch		--> Broadcst 	, AVR to request active source when Audio ocntrol feature is active
	eSetStreamPath			=0x86,	// follower:switch		--> Broadcst
	eInactiveSource			=0x9D,	//
	eStandBy				=0x36,	// follower:All			--> Broadcst

// System Information Feature
	eCECVersioin			=0x9E,
	eGetCECVersion			=0x9F,
	eGivePhysicalAddress	=0x83,	// follower:All
	eGetMenuLanguage		=0x91,	// follower:TV
	eReportPhysicalAddress	=0x84,	// follower:TV
	eSetMenuLanguage		=0x32,	// follower:All,		Initiator:TV

// Vendor Specific Commands Feature
	eDeviceVendorID		=0x87,
	eGiveDeviceVendorID	=0x8C,
	eVendorCommand		=0x89,
	eGiveDevicePowerStatus	=0x8F,
	eReportPowerStatus		=0x90,
	eVendorCommandWithID =0xA0,
	eRemoteButtonDown		=0x8A,
	eRemoteButtonUp		=0x8B,

// Deck Control Feature
	ePlay					=0x41,
	eDeckControl			=0x42,
	eDeckStatus				=0x1B,
	eGiveDeckStatus			=0x1A,
// other
	eUserPressed			=0x44,
	eUserReleased			=0x45,
	eAudioModeRequest		=0x70,
	eGiveAudioStatus		=0x71,
	eSetSystemAudioMode	=0x72,
	eReportAudioStatus		=0x7A,
	eGiveSystemAudIoModeStatus	=0x7D,
	eSystemAudIoModeStatus		=0x7E,
	eReportAudioDescriptor 	=0xA3,
	eRequestAudioDescriptor =0xA4,

// ACR
	eInitiateARC				=0xC0,
	eReportARCInitiated		=0xC1,
	eReportARCTerminated	=0xC2,
	eRequestARCInitiation	=0xC3,
	eRequestARCTermination	=0xC4,
	eTerminateARC			=0xC5,

// HEC
	eCDC					=0xF8,

// Polling Mesage
	ePollingMessage,

	eAbort					=0xff,

	eCECNone

} eCEC_CMD;

typedef enum _CDC_COMMAND
{
	eHEC_InquirState			=0x00,
	eHEC_ReportState			=0x01,
	eHEC_SetStateAdjacent		=0x02,
	eHEC_SetState				=0x03,
	eHEC_RequestDeactivation	=0x04,
	eHEC_NotifyAlive			=0x05,
	eHEC_Discover				=0x06,
	eHEC_HPDSetState			=0x10,
	eHEC_HPDReportState		=0x11
}eCDC_CMD;

typedef union {
	struct
	{
		iTE_u8 	AudioVolumeStatus	:7;	//bit 0~6	Volume status
		iTE_u8	AudioMuteStatus		:1;	//bit 7 	Mute status
	}Map;

	iTE_u8 AStatus;
}CECAudioStatus;

typedef union {
	struct
	{
		iTE_u8 	AudioFormatCode	:6;	//bit 0~5	Audio Format Code
		iTE_u8	AudioFormatID		:2;	//bit 6~7 	Audio Format ID
	}Map;

	iTE_u8 AStatus;
}CECAudioFormatID;

typedef enum {
	eDirectly=0,
	eBroadcast=1,
	eBoth=2
}HeaderType;

typedef enum{
	eCDCFeature = 0,
	eOTPFeature,
	eRoutingCtlFeature,
	eStandByFeature,
	eSysInfoFeature,
	eVenSpecCmdFeature,
	eRemoteCtlPassthroughFeature,
	ePowerStatusFeature,
	eSysAudioCtlFeature,
	eACRFeature,
	eHECFeature,
	eDeckCtlFeature,
	eNoneFeature
}CECOpCodeType;

typedef struct {
	eCEC_CMD cmd;
	HeaderType header;
	iTE_u8	size;
	CECOpCodeType eType;
	iTE_ps8	CmdString;
} CECcmdFormat;


// for check Invalid CEC Cmd
#ifdef _CEC_LOOKUP_TAB_
#if (USING_CEC == iTE_TRUE)
static _CODE  CECcmdFormat  LookUpTable_CEC[]=
//static _CODE  LookUpTable_CEC[][3]=
 {

	{eFeatureAbort,			eDirectly,		4,	eCDCFeature, "Feature Abort"},			// [Header] + [0x00] + [Feature OpCode] + [Abort Reason]
	{eAbort,					eDirectly,		2,	eCDCFeature, "Abort"},			// [Header] + [0xFF]

	// One Touch Play Feature
	{eActiveSource,			eBroadcast,	4,	eOTPFeature, "Active Source"},			// Header + [0x82] + [Physical Address]
	{eImageViewOn,			eDirectly,		2,	eOTPFeature, "Image View On"},			// Header + [0x04]
	{eTextViewOn,			eDirectly,		2,	eOTPFeature, "Text View On"},			// Header + [0x0D]

	// Routing Control Feature
	{eInactiveSource,			eBroadcast,	2,	eRoutingCtlFeature, "Inactive Source"},			// Header + [0x9D]
	{eRequestActiveSource,		eBroadcast,	2,	eRoutingCtlFeature, "Request Active Source"},			// Header + [0x85]
	{eRoutingChange,			eBroadcast,	6,	eRoutingCtlFeature, "Routing Change"},			// Header + [0x80] + [Org Address] + [New Address]
	{eRoutingInformation,		eBroadcast,	4,	eRoutingCtlFeature, "Routing Information"},			// Header + [0x80] + [Org Address] + [New Address]
	{eSetStreamPath,			eBroadcast,	4,	eRoutingCtlFeature, "Set Stream Path"},			// Header + [0x86] + [Physical Address]

	{eStandBy,				eBoth,		2,	eStandByFeature, "Standby"},			// Header + [0x36]

	// System Information Feature
	{eCECVersioin,			eDirectly,		3,	eSysInfoFeature, "CEC Version"},			// Header + [0x82] + [CEC Version]
	{eGetCECVersion,			eDirectly,		2,	eSysInfoFeature, "Get CEC Version"},			// Header + [0x9F]
	{eGivePhysicalAddress,		eDirectly,		2,	eSysInfoFeature, "Give Physical Address"},			// Header + [0x83]
	{eGetMenuLanguage,		eDirectly,		2,	eSysInfoFeature, "Get Menu Language"},			// Header + [0x91]
	{eReportPhysicalAddress,	eBroadcast,	5,	eSysInfoFeature, "Report Physical Address"},			// Header + [0x84] + [Physical Address]+ [Device Type]
	{eSetMenuLanguage,		eBroadcast,	3,	eSysInfoFeature, "Set Menu Language"},			// Header + [0x32] + [Language]

	// Vendor Specific Commands Feature
	{eDeviceVendorID,		eBroadcast,	5,	eVenSpecCmdFeature, "Device Vendor ID"},			// Header + [0x87] + [Vendor ID]
	{eGiveDeviceVendorID,		eDirectly,		2,	eVenSpecCmdFeature, "Give Device Vendor ID"},			// Header + [0x8C]
	{eVendorCommand,		eDirectly,		3,	eVenSpecCmdFeature, "Vendor Command"},			// Header + [0x89] + [Vendor Specific Data]
	{eVendorCommandWithID,	eBoth,		6,	eVenSpecCmdFeature, "Vendor Command With ID"},			// Header + [0xA0] + [Vendor ID]+ [Vendor Specific Data]
	{eRemoteButtonDown,		eBoth,		3,	eVenSpecCmdFeature, "Vendor Remote Button Down"},			// Header + [0x8A] + [Vendor Specific RC code]
	{eRemoteButtonUp,		eBoth,		3,	eVenSpecCmdFeature, "Vendor Remote Button Up"},			// Header + [0x8B] + [Vendor Specific RC code]

	// Remote Control PassThrough Feature
	{eUserPressed,			eDirectly,		3,	eRemoteCtlPassthroughFeature, "User Control Pressed"},			// Header + [0x44] + [UI command]
	{eUserReleased,			eDirectly,		2,	eRemoteCtlPassthroughFeature, "User Control Released"},			// Header + [0x45]

	// Power Status Feature
	{eGiveDevicePowerStatus,	eDirectly,		2,	ePowerStatusFeature, "Give Device Power Status"},			// Header + [0x8F]
	{eReportPowerStatus,		eDirectly,		3,	ePowerStatusFeature, "Report Power Status"},			// Header + [0x90] + [Power Status]

	// System Audio Control Feature
	{eGiveAudioStatus,		eDirectly,		2,	eSysAudioCtlFeature, "Give Audio Status"},			// Header + [0x71]
	{eGiveSystemAudIoModeStatus,eDirectly,	2,	eSysAudioCtlFeature, "Give System Audio Mode Status"},			// Header + [0x7D]
	{eReportAudioStatus,		eDirectly,		3,	eSysAudioCtlFeature, "Report Audio Status"},			// Header + [0x7A] + [Audio Status]
	{eReportAudioDescriptor,	eDirectly,		4,	eSysAudioCtlFeature, "Report Audio Descriptor"},			// Header + [0xA3] + [Audio Descriptor]
	{eRequestAudioDescriptor,	eDirectly,		3,	eSysAudioCtlFeature, "Request Short Audio Descriptor"},			// Header + [0xA4] + [Audio Format ID and Code]
	{eSetSystemAudioMode	,	eBoth,		3,	eSysAudioCtlFeature, "Set System Audio Mode"},			// Header + [0x72] + [System Audio Status]
	{eAudioModeRequest,		eDirectly,		4,	eSysAudioCtlFeature, "System Audio Mode Request"},			// Header + [0x70] + [Physical Address]
	{eSystemAudIoModeStatus,	eDirectly,		3,	eSysAudioCtlFeature, "System Audio Mode Status"},			// Header + [0x7E] + [System Audio Status]

	// ACR
	{eInitiateARC,				eDirectly,		2,	eACRFeature, "Initiate ARC"},			// Header + [0xC0]
	{eReportARCInitiated,		eDirectly,		2,	eACRFeature, "Report ARC Initiated"},			// Header + [0xC1]
	{eReportARCTerminated,	eDirectly,		2,	eACRFeature, "Report ARC Terminated"},			// Header + [0xC2]
	{eRequestARCInitiation,		eDirectly,		2,	eACRFeature, "Request ARC Initiation"},			// Header + [0xC3]
	{eRequestARCTermination,	eDirectly,		2,	eACRFeature, "Request ARC Termination"},			// Header + [0xC4]
	{eTerminateARC,			eDirectly,		2,	eACRFeature, "Termiate ARC"},			// Header + [0xC5]

	// HEC
	{eCDC,					eDirectly,		3,	eHECFeature, "CDC"},			// Header + [0xF8]

	// Deck Control Feature
	{ePlay,					eDirectly,		3,	eDeckCtlFeature, "Play"},			// Header + [0x41] + [Play Mode]
	{eDeckControl,			eDirectly,		3,	eDeckCtlFeature, "Deck Control"},			// Header + [0x41] + [Play Mode]
	{eDeckStatus,				eDirectly,		3,	eDeckCtlFeature, "Deck Status"},			// Header + [0x41] + [Play Mode]
	{eGiveDeckStatus,			eDirectly,		3,	eDeckCtlFeature, "Give Deck Status"},			// Header + [0x41] + [Play Mode]

	// Polling Mesage
	//ePollingMessage,
//	{eCDC+1,				eBoth,		0,	eNoneFeature},			//end of 	lookup table !!!
 };

#define     SizeOfLookUpTable_CEC    (sizeof(LookUpTable_CEC)/sizeof(CECcmdFormat))
#endif
#endif
#endif
