///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_8.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_8.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#define _EDID_TAB_
#include "it6662/include/IT6662_config.h"
#include "it6662/include/IT6662_IO.h"
#include "it6662/include/IT6662.h"

#include "it6662/include/Hdmi_Rx.h"
#include "it6662/include/Hdmi_Tx.h"
#include "it6662/include/Mhl_Rx.h"
#include "it6662/include/ITE_Edid.h"

#include "it6662/include/IT6662_DefaultEdid.h"

extern iTE_u1 bEnHDCP;
extern iTE_u8 u8HdmiTxSpenHdcpTrg;
extern iTE_u8 u8TxTrgRxHpd;
extern iTE_u1 bRxTrgRxHdcp;
extern iTE_u8 u8TxTrgTxHdcp;
extern iTE_u8 g_u8EdidCheck, g_u8TxHpdStatus;
extern iTE_u8 u8HDCPFireCnt[4];
extern iTE_u8 g_u8TxHpdCnt[4];
extern iTE_u8 g_u8TxSel, g_u8TxOffset, g_u8TxBank, g_u8TxShift;

extern iTE_u8	g_u8EdidMode;
EdidInfo	g_stEdidInfoAry[4];
iTE_u8	g_u8EdidArySta = 0;

void IT6662_Edid_Get_8(iTE_u8 u8EdidMode)
{
	EDID_STAT eEdidRdSta;

	iTE_Msg(("IT6662_Edid_Get %d\n", u8EdidMode));

	switch(u8EdidMode){
		case	EDID_DEFAULT_FHD:
				Edid_FixTabLoad(u8EdidFHD, &g_stEdidInfoAry[EDID_COPY_PORT]);
			break;
		case	EDID_DEFAULT_4K2K:
				Edid_FixTabLoad(u8Edid4K2K, &g_stEdidInfoAry[EDID_COPY_PORT]);
			break;
		case	EDID_COPY:
				HdmiTx_Select(EDID_COPY_PORT);
				if(g_u8TxHpdStatus & (0x01 <<EDID_COPY_PORT)){
					HdmiTx_Bank(g_u8TxBank);
					eEdidRdSta = Edid_Get(&g_stEdidInfoAry[EDID_COPY_PORT]);
					HdmiTx_Bank(0);
				}else{
					iTE_Msg((" EDID_DEFAULT = %d\n", EDID_DEFAULT));
					if(EDID_DEFAULT == EDID_DEFAULT_FHD){
						Edid_FixTabLoad(u8EdidFHD, &g_stEdidInfoAry[EDID_COPY_PORT]);
					}else if(EDID_DEFAULT == EDID_DEFAULT_4K2K){
						Edid_FixTabLoad(u8Edid4K2K, &g_stEdidInfoAry[EDID_COPY_PORT]);
					}
				}

			break;
		//case	EDID_COMPOSITE:
		//	break;
	}
	Edid_InfoShow(&g_stEdidInfoAry[EDID_COPY_PORT]);
}

void IT6662_Edid_Check_8(iTE_u8 u8TxSel)
{
	if(g_u8EdidCheck & (0x01 << u8TxSel)){
		HdmiTx_Select(u8TxSel);
		if(g_u8TxHpdStatus & g_u8TxShift){
			HdmiTx_Bank(g_u8TxBank);
			Edid_BlkParse(&g_stEdidInfoAry[u8TxSel]);
			HdmiTx_Bank(0);
			Edid_InfoShow(&g_stEdidInfoAry[u8TxSel]);
			g_u8EdidArySta |= g_u8TxShift;
		}else{
			g_u8EdidArySta &= ~g_u8TxShift;
		}
	}
}

iTE_u1 IT6662_Detect_8(void)
{
	iTE_u8	ucVenID[2], ucDevID[2], ucRevID;

	HdmiRx_Rb(0x00, 2, ucVenID);
	HdmiRx_Rb(0x02, 2, ucDevID);

	HdmiRx_Rb(0x00, 2, ucVenID);
	HdmiRx_Rb(0x02, 2, ucDevID);

	ucRevID = HdmiRx_R(0x04);

	iTE_Msg(("###############################################\r\n"));
	iTE_Msg(("#           			IT6662 Detect           					#\r\n"));
	iTE_Msg(("###############################################\r\n"));

	iTE_Msg(("Current VenID=%02X-%02X\r\n", (int)ucVenID[1], (int)ucVenID[0]));
	iTE_Msg(("Current DevID=%02X-%02X\r\n", (int)ucDevID[1], (int)ucDevID[0]));
	iTE_Msg(("Current RevID=%02X\r\n", (int)ucRevID));

	if( ucVenID[0]!=0x54 || ucVenID[1]!=0x49 || ucDevID[0]!=0x62 || ucDevID[1]!=0x66){// || (ucRevID!=0xB0 && ucRevID!=0xB1)) {	//V1.06_1
		iTE_Msg(("\nERROR: Can not find iTE Device !!!\r\n"));
		return iTE_FALSE;
     }

	return iTE_TRUE;
}

iTE_u1 IT6662_Init_8(void)
{
	iTE_Msg(("\nIT6662_Init S\r\n"));
	if(iTE_TRUE == IT6662_Detect()){
		HdmiRx_Init();
		MhlRx_Init();
		HdmiTx_Init();

		if(iTE_TRUE == EnIntEDID){
//			Edid_FixTabLoad((iTE_pu8)EDID_DEFAULT);
			IT6662_Edid_Get(g_u8EdidMode);
//			//Edid_Init();
		}

		MhlRx_Set(0x2A, 0x01, 0x01);
		MhlRx_Set(0x0F, 0x10, 0x00);

		iTE_Msg(("IT6662_Init E\r\n"));
		return iTE_TRUE;
	}

	return iTE_FALSE;
}

void IT6662_IRQ_8(void)
{
	iTE_u8	u8IntSta;

//	iTE_Msg(("IT6662_IRQ S\r\n"));
#if 1
	u8IntSta = HdmiRx_R(0xE0);

	u8IntSta = 0x70;
#endif
	if(u8IntSta & 0x40) {
		Mhl_CBusIrq();
		Mhl_MscIrq();
	}
	if(u8IntSta & 0x20) {
		HdmiRx_Irq();
	}

	if(u8IntSta & 0x10) {	//
		HdmiTx_Irq();
	}

//	HdmiRx_VideoFSM();


#if 1	//ming

	if(g_u8EdidCheck){
#if 1
		IT6662_Edid_Check(HDMI_TX_A);
		IT6662_Edid_Check(HDMI_TX_B);
		IT6662_Edid_Check(HDMI_TX_C);
		IT6662_Edid_Check(HDMI_TX_D);
#else
		if(g_u8EdidCheck & (0x01 << HDMI_TX_A)){
			HdmiTx_Select(HDMI_TX_A);
			if(g_u8TxHpdStatus & (0x01 << HDMI_TX_A)){
				HdmiTx_Bank(g_u8TxBank);
				Edid_BlkParse(&g_stEdidInfoAry[HDMI_TX_A]);
				HdmiTx_Bank(0);
				g_u8EdidArySta |= (0x01 << HDMI_TX_A);
			}else{
				g_u8EdidArySta &= ~(0x01 << HDMI_TX_A);
			}
		}

		if(g_u8EdidCheck & (0x01 << HDMI_TX_B)){
			HdmiTx_Select(HDMI_TX_B);
			if(g_u8TxHpdStatus & (0x01 << HDMI_TX_B)){
				HdmiTx_Bank(g_u8TxBank);
				Edid_BlkParse(&g_stEdidInfoAry[HDMI_TX_B]);
				HdmiTx_Bank(0);
				g_u8EdidArySta |= (0x01 << HDMI_TX_B);
			}else{
				g_u8EdidArySta &= ~(0x01 << HDMI_TX_B);
			}
		}
		if(g_u8EdidCheck & 0x04){
			HdmiTx_Select(HDMI_TX_C);
			if(g_u8TxHpdStatus & (0x01 << HDMI_TX_C)){
				HdmiTx_Bank(g_u8TxBank);
				Edid_BlkParse(&g_stEdidInfoAry[HDMI_TX_C]);
				HdmiTx_Bank(0);
				g_u8EdidArySta |= (0x01 << HDMI_TX_C);
			}else{
				g_u8EdidArySta &= ~(0x01 << HDMI_TX_C);
			}
		}
		if(g_u8EdidCheck & 0x08){
			HdmiTx_Select(HDMI_TX_D);
			if(g_u8TxHpdStatus & (0x01 << HDMI_TX_D)){
				HdmiTx_Bank(g_u8TxBank);
				Edid_BlkParse(&g_stEdidInfoAry[HDMI_TX_D]);
				HdmiTx_Bank(0);
				g_u8EdidArySta |= (0x01 << HDMI_TX_D);
			}else{
				g_u8EdidArySta &= ~(0x01 << HDMI_TX_D);
			}
		}
#endif

		if((g_u8EdidMode == EDID_COPY) && (g_u8EdidCheck & (1<<EDID_COPY_PORT))){
			IT6662_Edid_Get(EDID_COPY);
			u8TxTrgRxHpd = 1;
		}
		g_u8EdidCheck = 0;
	}
	if(u8TxTrgRxHpd) {
			MSC_CMD TxMscCmd;
			iTE_Msg(("*****TxTrgRxHPD*****\n"));

		if(HdmiRx_R(0x0A) & 0x40){
			while(Msc_Fire(MSC_PKT_CLR_HPD, &TxMscCmd)!=MSC_SUCCESS);
		}else{
			HdmiRx_Bank(1);
			HdmiRx_Set(0xB0, 0x03, 0x01);		// Clear HPD
			HdmiRx_Bank(0);
		}
		iTE_Sleep_ms(200);

		if(HdmiRx_R(0x0A) & 0x40){
			while(Msc_Fire(MSC_PKT_SET_HPD, &TxMscCmd)!=MSC_SUCCESS);
		}else{
			HdmiRx_Bank(1);
			HdmiRx_Set(0xB0, 0x03, 0x03);		// Set HPD
			HdmiRx_Bank(0);
		}
		u8TxTrgRxHpd = 0;
		u8HDCPFireCnt[0]=0;
		u8HDCPFireCnt[1]=0;
		u8HDCPFireCnt[2]=0;
		u8HDCPFireCnt[3]=0;
		g_u8TxHpdCnt[0]  = 0;
		g_u8TxHpdCnt[1]  = 0;
		g_u8TxHpdCnt[2]  = 0;
		g_u8TxHpdCnt[3]  = 0;
	 }

#endif

//	iTE_Msg(("IT6662_IRQ E\r\n"));
}

iTE_u8	IT6662_GetReg_8(iTE_u8	u8I2cAdr, iTE_u16 u16Reg)
{
	#if 0
	iTE_u8	u8RegValue;
	iTE_u8	u8Temp = u16Reg >> 8;
	if(u8Temp){
		iTE_i2c_write_byte(u8I2cAdr, 0x0F, 1, &u8Temp, iTE_I2C_Dev);
	}
	iTE_i2c_read_byte(u8I2cAdr, u16Reg & 0xFF, 1, &u8RegValue, iTE_I2C_Dev);
	if(u8Temp){
		u8Temp = 0;
		iTE_i2c_write_byte(u8I2cAdr, 0x0F, 1, &u8Temp, iTE_I2C_Dev);
	}
	return u8RegValue;
	#endif
}

