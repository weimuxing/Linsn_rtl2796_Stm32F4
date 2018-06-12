///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE_typedef.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE_typedef.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _ITE_TYPEDEF_H_
#define _ITE_TYPEDEF_H_

//////////////////////////////////////////////////
// data type
//////////////////////////////////////////////////
#define iTE_FALSE	0
#define iTE_TRUE		1

#define _MCU_STM32_ 1


#ifdef _MCU_8051_
	typedef bit iTE_u1 ;
	#define _CODE code
#elif _MCU_STM32_
	typedef int iTE_u1 ;
	#define _CODE 
#else
#ifdef WIN32
	typedef int iTE_u1 ;
	#define _CODE
#else
	typedef int iTE_u1 ;
//	#define _CODE
	#define _CODE __attribute__ ((section ("._OEM_BU1_RODATA ")))
#endif // _MCU_8051_
#endif


typedef enum _iTE_STATUS {
    iTE_SUCCESS = 0,
    iTE_FAIL
//    SYS_RESERVED
} iTE_STATUS;

#ifndef NULL
	#define NULL ((void *) 0)
#endif

typedef char iTE_s8, *iTE_ps8;
typedef unsigned char iTE_u8, *iTE_pu8;
typedef short iTE_s16, *iTE_ps16;
typedef unsigned short iTE_u16, *iTE_pu16;


#ifndef _MCU_8051_				// IT6350
typedef unsigned int iTE_u32, *iTE_pu32;
typedef int iTE_s32, *iTE_ps32;
#else
typedef unsigned long iTE_u32, *iTE_pu32;
typedef long iTE_s32, *iTE_ps32;
#endif

#define CD8BIT			4
#define CD10BIT	 		5
#define CD12BIT			6
#define CD16BIT			7

#define OUT8B           		0
#define OUT10B          		1
#define OUT12B          		2

#define RGB444			0
#define YCbCr422			1
#define YCbCr444			2

#define RGB444_SDR		0
#define YUV444_SDR		1
#define RGB444_DDR		2
#define YUV444_DDR		3
#define YUV422_EMB_SYNC_SDR	4
#define YUV422_EMB_SYNC_DDR	5
#define YUV422_SEP_SYNC_SDR	6
#define YUV422_SEP_SYNC_DDR	7
#define CCIR656_EMB_SYNC_SDR	8
#define CCIR656_EMB_SYNC_DDR	9
#define CCIR656_SEP_SYNC_SDR	10
#define CCIR656_SEP_SYNC_DDR	11
#define RGB444_HALF_BUS		12
#define YUV444_HALF_BUS		13
#define BTA1004_SDR				14
#define BTA1004_DDR				15


typedef struct _IT6662_Var{
//HDMI_TX
	iTE_u8 u8HDCPFireCnt[4];				// Counter for HDCP reAuthentication
	iTE_u32 u8HDCPRiChkCnt[4];			// Counter for HDCP Ri check.
	iTE_u8 u8TxHpdChkTimer[4];			// Counter for Check the Low status period of HPD
	iTE_u8 u8TxHpdCnt[4];				//
	iTE_u8 u8EdidCheck;					// The flag for HPD status changed, EDID may need to check
	iTE_u8 u8TxPortEn;					// The flag for record Tx port enable status, HPD == 1 && RxSen == 1
	iTE_u8 u8TxHpdStatus;				// The flag for record Tx HPD status
	iTE_u16 u16DevCnt;					// HDCP device count for HDCP repeater use
	iTE_u8 u8MaxDepth;					// The Max HDCP device depth
	iTE_u8 u8TxDoHdcp;					// Tx doing HDCP
	iTE_u8 u8TxTrgRxHpd;				// [3:0] The flag for record Tx HPD status changed from Low to High, [7] EDID changed.
	iTE_u8 u8TxSyncDetCnt[4];			// Counter for HDCP Sync Detect.
	iTE_u8 u8TxSpenHdcp;

	iTE_u8 u8TxRptRdyCnt[4];				// Counter for Check HDCP Ready bit in repeater mode.
	iTE_u8 u8TxEdidRdFail;

//HDMI_RX
	iTE_u1 bForceCsc;					// Force Change to RGB444 mode
	iTE_u1 bRxTrgTxHdcp;				// The flag for record Rx trigger
	iTE_u1 bRxHpdStatus;					// The flag for record Rx HPD status
	iTE_u8 u8EccErrCnt;					// ECC error counter
	iTE_u8 u8EqFailCnt;					// EQ fail counter
	iTE_u8 u8DeskewFailCnt;				// DeSkew Fail counter
	iTE_u8 u8HpdTimerCnt;				//
//	iTE_u8 u8OutClr;
//MHL_RX
	iTE_u32 u32RefClk;					// IT6662 reference clock
	iTE_u16 u16WakeFailCnt;				// CBus wakeup fail counter
	iTE_u8 u8DisvFailCnt;					// CBus descover fail counter
	iTE_u8 u8WakeFailSleepCnt;			// Sleep counter for wakeup fail interrupt
	iTE_u1 bCbusDiscov;					// CBus Discover down.
	//iTE_u8 u8EccErrSleepCnt;
	//iTE_u8 u8ClkVaildSleepCnt;
	//iTE_u8 u8ClkRdySleepCnt;
	//iTE_u8 u8ClkStabSleepCnt;
//CEC_TX
}IT6662_Var;


#define GP00				(0)
#define GPB3				(1)
#define GPB4				(2)
#define GPF6				(3)
#define GPF7				(4)
#define GPA0				(5)
#define GPA1				(6)
#define GPG3				(7)
#define GPG4				(8)
#define GPG6				(9)
#define GPA6				(10)
#define GPI0				(11)
#define GPI1				(12)
#define GPI2				(13)
#define GPI3				(14)
#define GPI4				(15)
#define GPI5				(16)
#define GPG5				(17)
enum{
	EDID_DEFAULT_FHD 	= 0x01,
	EDID_DEFAULT_4K2K = 0x02,
	EDID_COPY			= 0x03,
	EDID_COMPOSE_MIN	= 0x04,
	EDID_COMPOSE_J	= 0x05,
};
enum{
	HDMI_TX_A = 0x00,
	HDMI_TX_B = 0x01,
	HDMI_TX_C = 0x02,
	HDMI_TX_D = 0x03,
};

#define IT6662_HPD_H				(0x03)
#define IT6662_HPD_L				(0x01)
#define IT6662_HPD_AUTO			(0x00)

#define IT6662_CEC_A				(0x80 | (HDMI_TX_A << 4))
#define IT6662_CEC_B				(0x80 | (HDMI_TX_B << 4))
#define IT6662_CEC_C				(0x80 | (HDMI_TX_C << 4))
#define IT6662_CEC_D				(0x80 | (HDMI_TX_D << 4))
#endif
