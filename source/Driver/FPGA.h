#ifndef _FPGA_H_
#define _FPGA_H_
#include "define.h"

#define USE_FPGA_MODULE

#ifdef USE_FPGA_MODULE


// FPGA_CE -- PE10
// FPGA_CONFIG -- PE11	
// RESETn_IN	-- PE12
//#define FPGA_CONFIG_PIN			GPIO_Pin_11	//OUT
//#define FPGA_CONFIG_PORT		GPIOE

#define FPGA_NCE_PIN			GPIO_Pin_6	//OUT
#define FPGA_NCE_PORT			GPIOF
#define FPGA_RESET_PORT			GPIOF
#define FPGA_RESET_PIN			GPIO_Pin_7

#define SET_FPGA_RST() 		GPIO_SetBits(FPGA_RESET_PORT,FPGA_RESET_PIN)
#define RESET_FPGA_RST() 	GPIO_ResetBits(FPGA_RESET_PORT,FPGA_RESET_PIN)

#define SET_FPGA_NCE() 			GPIO_SetBits(FPGA_NCE_PORT,FPGA_NCE_PIN)
#define RESET_FPGA_NCE() 		GPIO_ResetBits(FPGA_NCE_PORT,FPGA_NCE_PIN)

#define FPGA_I2C_PORT           I2C_A
//const I2CPort_TypeDef fpga_i2c_port = I2C_I;





#define DVI0_OUTPUT	00
#define DVI1_OUTPUT 01
#define DVI2_OUTPUT 02
#define DVI3_OUTPUT 03




typedef enum
{
	FPGA_REG_VersionNum = 0x00,		//
	FPGA_REG_DeviceNum,
	FPGA_REG_GolobalRst,
	FPGA_REG_OutputPortSel = 0x05,

	FPGA_REG_DisplayMode = 0x06,	//��ʾģʽ�Ĵ���
									//����ģʽѡ��
									//8λֵ0: ��Ե�����ƴ��ģʽ
									//8λֵ1: ������ƴ��ģʽ
									//8λֵ3: 2��4K������ʾģʽ
									//8λֵ4: 2796�������Ԥ��ģʽ
									//8λֵ6: 2796�������Ԥ��ģʽ
									
	FPGA_REG_Freeze = 0x07,//���λ��1���ử��

	FPGA_REG_V_PaneStart0Pos_LSB = 0x08,	//��ֱ������ʼλ��0
	FPGA_REG_V_PaneStart0Pos_MSB,
	FPGA_REG_V_PaneStart1Pos_LSB,			//��ֱ������ʼλ��0
	FPGA_REG_V_PaneStart1Pos_MSB,
	FPGA_REG_V_PaneEnd0Pos_LSB,				//��ֱ�������λ��1		
	FPGA_REG_V_PaneEnd0Pos_MSB,
	FPGA_REG_V_PaneEnd1Pos_LSB,				//��ֱ�������λ��1
	FPGA_REG_V_PaneEnd1Pos_MSB,


	//######################################################################################################
	//�����0->DVI0
	FPGA_REG_h_Output0Layer_A_StartPos_LSB = 0X10,		//�����0ͼ��Aˮƽ������ʼλ�õ��ֽ�
	FPGA_REG_h_Output0Layer_A_StartPos_MSB,				//�����0ͼ��Aˮƽ������ʼλ�ø��ֽ�
	FPGA_REG_v_Output0Layer_A_StartPos_LSB,				//�����0ͼ��A��ֱ������ʼλ�õ��ֽ�
	FPGA_REG_v_Output0Layer_A_StartPos_MSB,				//�����0ͼ��A��ֱ������ʼλ�ø��ֽ�
	FPGA_REG_h_Output0Layer_A_WidthPos_LSB,				//�����0ͼ��Aˮƽ�����ȵ��ֽ�
	FPGA_REG_h_Output0Layer_A_WidthPos_MSB,				//�����0ͼ��Aˮƽ�����ȸ��ֽ�
	FPGA_REG_v_Output0Layer_A_HeightPos_LSB,			//�����0ͼ��A��ֱ����߶ȵ��ֽ�
	FPGA_REG_v_Output0Layer_A_HeightPos_MSB,			//�����0ͼ��A��ֱ����߶ȸ��ֽ�
	
	FPGA_REG_h_Output0Layer_B_StartPos_LSB,				//�����0ͼ��Bˮƽ������ʼλ�õ��ֽ�
	FPGA_REG_h_Output0Layer_B_StartPos_MSB,				//�����0ͼ��Bˮƽ������ʼλ�ø��ֽ�
	FPGA_REG_v_Output0Layer_B_StartPos_LSB,				//�����0ͼ��B��ֱ������ʼλ�õ��ֽ�
	FPGA_REG_v_Output0Layer_B_StartPos_MSB,				//�����0ͼ��B��ֱ������ʼλ�ø��ֽ�
	FPGA_REG_h_Output0Layer_B_WidthPos_LSB,				//�����0ͼ��Bˮƽ�����ȵ��ֽ�
	FPGA_REG_h_Output0Layer_B_WidthPos_MSB,				//�����0ͼ��Bˮƽ�����ȸ��ֽ�
	FPGA_REG_v_Output0Layer_B_HeightPos_LSB,			//�����0ͼ��B��ֱ����߶ȵ��ֽ�
	FPGA_REG_v_Output0Layer_B_HeightPos_MSB,			//�����0ͼ��B��ֱ����߶ȸ��ֽ�
	FPGA_REG_Output0layer_A_Alpha = 0X20,				//�����0ͼ��A  alphaֵ
	FPGA_REG_Output0layer_B_Alpha,						//�����0ͼ��B  alphaֵ
	
	FPGA_REG_h_Output0SplitActiveStartPos_LSB,			//�����0ˮƽƴ����Ч��ʼλ�õ��ֽ�
	FPGA_REG_h_Output0SplitActiveStartPos_MSB,			//�����0ˮƽƴ����Ч��ʼλ�ø��ֽ�
	FPGA_REG_h_Output0SplitActiveEndPos_LSB,			//�����0ˮƽƴ����Ч����λ�õ��ֽ�
	FPGA_REG_h_Output0SplitActiveEndPos_MSB,			//�����0ˮƽƴ����Ч����λ�ø��ֽ�
	FPGA_REG_h_Output0CropStartPos_LSB,					//�����0ˮƽƴ�Ӳü���ʼλ�õ��ֽ�
	FPGA_REG_h_Output0CropStartPos_MSB,					//�����0ˮƽƴ�Ӳü���ʼλ�ø��ֽ�
	FPGA_REG_h_Output0CropEndPos_LSB,					//�����0ˮƽƴ�Ӳü�����λ�õ��ֽ�
	FPGA_REG_h_Output0CropEndPos_MSB,					//�����0ˮƽƴ�Ӳü�����λ�ø��ֽ�



	//######################################################################################################
	//�����1->DVI1
	FPGA_REG_h_Output1Layer_A_StartPos_LSB = 0X30,
	FPGA_REG_h_Output1Layer_A_StartPos_MSB,
	FPGA_REG_v_Output1Layer_A_StartPos_LSB,
	FPGA_REG_v_Output1Layer_A_StartPos_MSB,
	FPGA_REG_h_Output1Layer_A_WidthPos_LSB,
	FPGA_REG_h_Output1Layer_A_WidthPos_MSB,
	FPGA_REG_v_Output1Layer_A_HeightPos_LSB,
	FPGA_REG_v_Output1Layer_A_HeightPos_MSB,
	
	FPGA_REG_h_Output1Layer_B_StartPos_LSB,
	FPGA_REG_h_Output1Layer_B_StartPos_MSB,
	FPGA_REG_v_Output1Layer_B_StartPos_LSB,
	FPGA_REG_v_Output1Layer_B_StartPos_MSB,
	FPGA_REG_h_Output1Layer_B_WidthPos_LSB,
	FPGA_REG_h_Output1Layer_B_WidthPos_MSB,
	FPGA_REG_v_Output1Layer_B_HeightPos_LSB,
	FPGA_REG_v_Output1Layer_B_HeightPos_MSB,
	
	FPGA_REG_Output1layer_A_Alpha = 0X40,
	FPGA_REG_Output1layer_B_Alpha,
	
	FPGA_REG_h_Output1SplitActiveStartPos_LSB,
	FPGA_REG_h_Output1SplitActiveStartPos_MSB,
	FPGA_REG_h_Output1SplitActiveEndPos_LSB,
	FPGA_REG_h_Output1SplitActiveEndPos_MSB,
	FPGA_REG_h_Output1CropStartPos_LSB,
	FPGA_REG_h_Output1CropStartPos_MSB,
	FPGA_REG_h_Output1CropEndPos_LSB,
	FPGA_REG_h_Output1CropEndPos_MSB,



	//######################################################################################################
	//�����2->DVI2
	FPGA_REG_h_Output2Layer_A_StartPos_LSB = 0X50,
	FPGA_REG_h_Output2Layer_A_StartPos_MSB,
	FPGA_REG_v_Output2Layer_A_StartPos_LSB,
	FPGA_REG_v_Output2Layer_A_StartPos_MSB,
	FPGA_REG_h_Output2Layer_A_WidthPos_LSB,
	FPGA_REG_h_Output2Layer_A_WidthPos_MSB,
	FPGA_REG_v_Output2Layer_A_HeightPos_LSB,
	FPGA_REG_v_Output2Layer_A_HeightPos_MSB,
	
	
	FPGA_REG_h_Output2Layer_B_StartPos_LSB,
	FPGA_REG_h_Output2Layer_B_StartPos_MSB,
	FPGA_REG_v_Output2Layer_B_StartPos_LSB,
	FPGA_REG_v_Output2Layer_B_StartPos_MSB,
	FPGA_REG_h_Output2Layer_B_WidthPos_LSB,
	FPGA_REG_h_Output2Layer_B_WidthPos_MSB,
	FPGA_REG_v_Output2Layer_B_HeightPos_LSB,
	FPGA_REG_v_Output2Layer_B_HeightPos_MSB,
	
	FPGA_REG_Output2layer_A_Alpha = 0X60,
	FPGA_REG_Output2layer_B_Alpha,
	
	FPGA_REG_h_Output2SplitActiveStartPos_LSB,
	FPGA_REG_h_Output2SplitActiveStartPos_MSB,
	FPGA_REG_h_Output2SplitActiveEndPos_LSB,
	FPGA_REG_h_Output2SplitActiveEndPos_MSB,
	FPGA_REG_h_Output2CropStartPos_LSB,
	FPGA_REG_h_Output2CropStartPos_MSB,
	FPGA_REG_h_Output2CropEndPos_LSB,
	FPGA_REG_h_Output2CropEndPos_MSB,


	//######################################################################################################
	//�����3->DVI3
	FPGA_REG_h_Output3Layer_A_StartPos_LSB = 0X70,
	FPGA_REG_h_Output3Layer_A_StartPos_MSB,
	FPGA_REG_v_Output3Layer_A_StartPos_LSB,
	FPGA_REG_v_Output3Layer_A_StartPos_MSB,
	FPGA_REG_h_Output3Layer_A_WidthPos_LSB,
	FPGA_REG_h_Output3Layer_A_WidthPos_MSB,
	FPGA_REG_v_Output3Layer_A_HeightPos_LSB,
	FPGA_REG_v_Output3Layer_A_HeightPos_MSB,
	
	FPGA_REG_h_Output3Layer_B_StartPos_LSB,
	FPGA_REG_h_Output3Layer_B_StartPos_MSB,
	FPGA_REG_v_Output3Layer_B_StartPos_LSB,
	FPGA_REG_v_Output3Layer_B_StartPos_MSB,
	FPGA_REG_h_Output3Layer_B_WidthPos_LSB,
	FPGA_REG_h_Output3Layer_B_WidthPos_MSB,
	FPGA_REG_v_Output3Layer_B_HeightPos_LSB,
	FPGA_REG_v_Output3Layer_B_HeightPos_MSB,
	
	FPGA_REG_Output3layer_A_Alpha = 0X80,
	FPGA_REG_Output3layer_B_Alpha,
	
	FPGA_REG_h_Output3SplitActiveStartPos_LSB,
	FPGA_REG_h_Output3SplitActiveStartPos_MSB,
	FPGA_REG_h_Output3SplitActiveEndPos_LSB,
	FPGA_REG_h_Output3SplitActiveEndPos_MSB,
	FPGA_REG_h_Output3CropStartPos_LSB,
	FPGA_REG_h_Output3CropStartPos_MSB,
	FPGA_REG_h_Output3CropEndPos_LSB,
	FPGA_REG_h_Output3CropEndPos_MSB,

	//####################################################################################
	//����ֱ��ʼĴ���
	FPGA_REG_Format_h_Active_LSB = 0x90,
	FPGA_REG_Format_h_Active_MSB,
	FPGA_REG_Format_h_Total_LSB,
	FPGA_REG_Format_h_Total_MSB,
	FPGA_REG_Format_h_SyncStart_LSB,
	FPGA_REG_Format_h_SyncStart_MSB,
	FPGA_REG_Format_h_SyncEnd_LSB,
	FPGA_REG_Format_h_SyncEnd_MSB,

	FPGA_REG_Format_v_Active_LSB,
	FPGA_REG_Format_v_Active_MSB,
	FPGA_REG_Format_v_Total_LSB,
	FPGA_REG_Format_v_Total_MSB,
	FPGA_REG_Format_v_SyncStart_LSB,
	FPGA_REG_Format_v_SyncStart_MSB,
	FPGA_REG_Format_v_SyncEnd_LSB,
	FPGA_REG_Format_v_SyncEnd_MSB,

	//####################################################################################
	//scale�Ĵ���
	FPGA_REG_h_Dvi0_Zoom_LSB = 0xa0,	//�����ˮƽ�Ŵ�ϵ�� = ����������8λ����������,			
	FPGA_REG_h_Dvi0_Zoom_MSB,			//������ �� ƴ�ӽ��� �� ƴ�ӿ�ʼ(active_end - active_start)
	FPGA_REG_v_Dvi0_Zoom_LSB,			//�����ˮƽ�Ŵ�ϵ��������߶�����8λ��������߶�,					
	FPGA_REG_v_Dvi0_Zoom_MSB,			//����߶� �� ��ֱ���� �� ��ֱ�е�(high_end - high_mid)

	FPGA_REG_h_Dvi1_Zoom_LSB,			//�����ˮƽ�Ŵ�ϵ�� = ����������8λ����������,			
	FPGA_REG_h_Dvi1_Zoom_MSB,			//������ �� ƴ�ӽ��� �� ƴ�ӿ�ʼ(active_end - active_start)
	FPGA_REG_v_Dvi1_Zoom_LSB,			//�����ˮƽ�Ŵ�ϵ��������߶�����8λ��������߶�,					
	FPGA_REG_v_Dvi1_Zoom_MSB,			//����߶� �� ��ֱ���� �� ��ֱ�е�(high_end - high_mid)

	FPGA_REG_h_Dvi2_Zoom_LSB,			//�����ˮƽ�Ŵ�ϵ�� = ����������8λ����������,			
	FPGA_REG_h_Dvi2_Zoom_MSB,			//������ �� ƴ�ӽ��� �� ƴ�ӿ�ʼ(active_end - active_start)
	FPGA_REG_v_Dvi2_Zoom_LSB,			//�����ˮƽ�Ŵ�ϵ��������߶�����8λ��������߶�,					
	FPGA_REG_v_Dvi2_Zoom_MSB,			//����߶� �� ��ֱ���� �� ��ֱ�е�(high_end - high_mid)

	FPGA_REG_h_Dvi3_Zoom_LSB,			//�����ˮƽ�Ŵ�ϵ�� = ����������8λ����������,			
	FPGA_REG_h_Dvi3_Zoom_MSB,			//������ �� ƴ�ӽ��� �� ƴ�ӿ�ʼ(active_end - active_start)
	FPGA_REG_v_Dvi3_Zoom_LSB,			//�����ˮƽ�Ŵ�ϵ��������߶�����8λ��������߶�,					
	FPGA_REG_v_Dvi3_Zoom_MSB,			//����߶� �� ��ֱ���� �� ��ֱ�е�(high_end - high_mid)
	

	FPGA_REG_MaxSize,
	
}FpgaRegIndex;



#define FPGA_ADDR               0x80

// �豸��
#define FPGA_REG_0X00           0x00
// �汾��
#define FPGA_REG_0X01           0x01

//DVI2 DUAL link 7172    BIT0: DVI1 timming select (0-a 1-b) BIT1: DVI2 timing select  
//BIT5,BIT4: DVI1/2 data select  00: even-a odd-a  01:even-b odd-b  10: even-a odd-b  
//BIT6  split screen 00:enable 11:disable
#define FPGA_REG_DVI_CTL         0x10
//Reg 0x10 Bit [7:6]
#define FPGA_DVI0_9320			0x00
#define FPGA_DVI0_BUILDIN		0x80
#define FPGA_DVI0_PREVIEW		0xC0

//Reg 0x10 Bit [5:4]
#define FPGA_DVI1_9320			0x40
//#define FPGA_DVI1_PREVIEW		0x30

#define FPGA_REG_GRID_CTL       0x11
// Reg 0x11 Bit [7]
#define FPGA_GRID_ON			0x80
#define FPGA_GRID_OFF			0x00

// Reg 0x11 Bit [6:5]
#define FPGA_GRID_LINE_MASK		0x60
#define FPGA_GRID_LINE_HV		0x60
#define FPGA_GRID_LINE_H		0x20
#define FPGA_GRID_LINE_V		0x40
#define FPGA_GRID_LINE_BORDERS	0x00

// Reg 0x11 Bit [4:3]
#define FPGA_TEST_DOT_MASK		0x18
#define FPGA_TEST_DOT_MOVE		0x10
#define FPGA_TEST_DOT_STILL		0x08
#define FPGA_TEST_DOT_HIDE		0x00

// Reg 0x11 Bit [2]
#define FPGA_GRID_BACKGROUD_MASK	0x104
#define FPGA_GRID_BACKGROUD_VIDEO	0x104
#define FPGA_GRID_BACKGROUD_NULL	0x00

// Reg 0x11 Bit [1:0]
#define FPGA_GRID_LINE_COLOR_MASK	0x03
#define FPGA_GRID_LINE_COLOR_GREEN	0x01
#define FPGA_GRID_LINE_COLOR_RED	0x00
#define FPGA_GRID_LINE_COLOR_YELLOW	0x03

#define FPGA_TESTDOT_SPEED		0x12

#define FPGA_GRID_HSPACE_LSB		0x28
#define FPGA_GRID_HSPACE_MSB		0x29

#define FPGA_GRID_VSPACE_LSB		0x2A
#define FPGA_GRID_VSPACE_MSB		0x2B


#define FPGA_TESTDOT_STILL_X	0x15
#define FPGA_TESTDOT_STILL_Y	0x16

#define FPGA_TESTDOT_R			0x17
#define FPGA_TESTDOT_G			0x18
#define FPGA_TESTDOT_B			0x19

#define FPGA_REG_0X12           0x12		
#define FPGA_REG_HACTIVE_LSB	0x13
#define FPGA_REG_HACTIVE_MSB	0x14
		
#define FPGA_REG_H_POS_LSB		0x20
#define FPGA_REG_H_POS_MSB		0x21

#define FPGA_REG_V_POS_LSB		0x22
#define FPGA_REG_V_POS_MSB		0x23

#define FPGA_REG_H_WIDTH_LSB	0x24
#define FPGA_REG_H_WIDTH_MSB	0x25

#define FPGA_REG_V_HEIGHT_LSB	0x26
#define FPGA_REG_V_HEIGHT_MSB	0x27

//###########################################################################################
//����ģ����ؼĴ���

#define FPGA_REG_PGM_PRV		0x10	//�����/Ԥ������Ĵ���
#define FPGA_REG_BLACK			0x90	//�ڳ�/���뵭��
#define FPGA_REG_TESTPATTERN	0x91	//FPGA�������ڲ�����ģ��

#define FPGA_REG_H_GRID_LSB		0x92	//�����ȵ�8λ
#define FPGA_REG_H_GRID_MSB		0x93	//�����ȸ�8λ

#define FPGA_REG_V_GRID_LSB		0x94	//����߶ȵ�8λ
#define FPGA_REG_V_GRID_MSB		0x95	//����߶ȸ�8λ

#define FPGA_REG_CHAR			0X96	//FPGA�����ַ�
#define FPGA_REG_CHAR_ZOOM		0x97	//FPGA�����Ŵ��������ַ�

#define FPGA_REG_CHARA_X_LSB	0x98	//�ַ�A��Xλ�õ�8λ
#define FPGA_REG_CHARA_X_MSB	0x99	//�ַ�A��Xλ�ø�8λ
#define FPGA_REG_CHARA_Y_LSB	0x9A	//�ַ�A��Yλ�õ�8λ
#define FPGA_REG_CHARA_Y_MSB	0x9B	//�ַ�A��Yλ�ø�8λ

#define FPGA_REG_CHARB_X_LSB	0x9C	//�ַ�B��Xλ�õ�8λ
#define FPGA_REG_CHARB_X_MSB	0x9D	//�ַ�B��Xλ�ø�8λ
#define FPGA_REG_CHARB_Y_LSB	0x9E	//�ַ�B��Yλ�õ�8λ
#define FPGA_REG_CHARB_Y_MSB	0x9F	//�ַ�B��Yλ�ø�8λ

#define FPGA_REG_CHARC_X_LSB	0xA0	//�ַ�C��Xλ�õ�8λ
#define FPGA_REG_CHARC_X_MSB	0xA1	//�ַ�C��Xλ�ø�8λ
#define FPGA_REG_CHARC_Y_LSB	0xA2	//�ַ�C��Yλ�õ�8λ
#define FPGA_REG_CHARC_Y_MSB	0xA3	//�ַ�C��Yλ�ø�8λ

#define FPGA_REG_CHARD_X_LSB	0xA4	//�ַ�D��Xλ�õ�8λ
#define FPGA_REG_CHARD_X_MSB	0xA5	//�ַ�D��Xλ�ø�8λ
#define FPGA_REG_CHARD_Y_LSB	0xA6	//�ַ�D��Yλ�õ�8λ
#define FPGA_REG_CHARD_Y_MSB	0xA7	//�ַ�D��Yλ�ø�8λ

#define FPGA_REG_CHARE_X_LSB	0xA8	//�ַ�E��Xλ�õ�8λ
#define FPGA_REG_CHARE_X_MSB	0xA9	//�ַ�E��Xλ�ø�8λ
#define FPGA_REG_CHARE_Y_LSB	0xAA	//�ַ�E��Yλ�õ�8λ
#define FPGA_REG_CHARE_Y_MSB	0xAB	//�ַ�E��Yλ�ø�8λ

#define FPGA_REG_CHARF_X_LSB	0xAC	//�ַ�F��Xλ�õ�8λ
#define FPGA_REG_CHARF_X_MSB	0xAD	//�ַ�F��Xλ�ø�8λ
#define FPGA_REG_CHARF_Y_LSB	0xAE	//�ַ�F��Yλ�õ�8λ
#define FPGA_REG_CHARF_Y_MSB	0xAF	//�ַ�F��Yλ�ø�8λ

#define FPGA_REG_CHARG_X_LSB	0xB0	//�ַ�G��Xλ�õ�8λ
#define FPGA_REG_CHARG_X_MSB	0xB1	//�ַ�G��Xλ�ø�8λ
#define FPGA_REG_CHARG_Y_LSB	0xB2	//�ַ�G��Yλ�õ�8λ
#define FPGA_REG_CHARG_Y_MSB	0xB3	//�ַ�G��Yλ�ø�8λ

#define FPGA_REG_CHARH_X_LSB	0xB4	//�ַ�H��Xλ�õ�8λ
#define FPGA_REG_CHARH_X_MSB	0xB5	//�ַ�H��Xλ�ø�8λ
#define FPGA_REG_CHARH_Y_LSB	0xB6	//�ַ�H��Yλ�õ�8λ
#define FPGA_REG_CHARH_Y_MSB	0xB7	//�ַ�H��Yλ�ø�8λ

#define FPGA_REG_FADE_TIME		0xB8	//���뵭��ʱ��Ĵ���


#define CHAR_WIDTH				156		//�ַ���
#define CHAR_HIGH				205		//�ַ���

#define BIT_CHAR_WIDTH				312		//�ַ���
#define BIT_CHAR_HIGH				410		//�ַ���





#define PGM_TEST_PATTERN_ON			~(3 << 6)
//#define PGM_TEST_PATTERN_OFF		 (3 << 6)

#define PRV_TEST_PATTERN_ON		 	 (3 << 4)
//#define PRV_TEST_PATTERN_OFF		 (3 << 4)

















#if ENABLE_TEST_GRID_CONTROL
extern BYTE GridSetIndex;
extern BYTE testGrid[4];
extern BYTE gridLineMode[4];
extern BYTE gridLineColor[4];	
extern int gridHSpace[4];	
extern int gridVSpace[4];	
extern BYTE gridBackgroud[4];
extern BYTE gridDotMode[4];	
extern BYTE gridDotSpeed[4];
extern BYTE gridDotPosX[4];
extern BYTE gridDotPosY[4];
extern BYTE gridDotColor[4];
extern BYTE gridDotColorR[4];	
extern BYTE gridDotColorG[4];	
extern BYTE gridDotColorB[4];		
#endif

typedef struct
{
	u8 ver_msb;
	u8 ver_lsb;
	unsigned char port;
	GPIO_TypeDef * reset_gpiox;
	uint32_t reset_Pin;
	unsigned char slave_addr;

	
}fpga_dev_info;



void FPGA_Program_Start(void);
void FPGA_Program_Done(void);
int FPGA_ReadSoftVersion(void);
int FPGA_GetSoftVersion(u8 * msb,u8 * lsb);

void FPGA_init(void);

void FPGA_set(u8 address,  u8 value);
u8 FPGA_get(u8 address);
void FPGA_I2C_Test(void);
#if ENABLE_TEST_GRID_CONTROL
void FPGA_Set_TestGrid_index(BYTE index);
BYTE FPGA_Get_TestGrid_index(void);
void FPGA_TestGrid_Ctrl(int index);
void FPGA_TestGrid_RefleshByPort(void);
void init_testGridData(void);

#endif
u8 Fpga_setPreviewOutEnable(u8 enable);
void FPGA_setOutputBlack(char enable);
BYTE FPGA_getOutputBlack(void);

void FPGASetOutputBlack(char enable);
BYTE FPGAGetOutputBlack(void);
void FPGASetFadeTime(u8 value);
void FPGASetFreeze(u8 value);
void FpgaOutputPortSel(u8 port0,u8 port1,u8 port2,u8 port3);
//void FpgaOutputPortSel(u8 port0,u8 port1,u8 port2,u8 port3);








#endif		// #ifdef USE_FPGA_MODULE

#endif		// _FPGA_H_
