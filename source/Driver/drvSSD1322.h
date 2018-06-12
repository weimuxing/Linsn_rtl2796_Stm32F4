/*************************************************
 *  Copyright (C), 2011-2012, RGBlink (Xiamen) Corporation Ltd.
 *  All rights reserved.
 *
 *  File Name:     drvSSD1322.h
 *
 *  Description:    用于OLED 驱动芯片SSD1322的驱动
 *
 *  Others:        驱动程序
 *
 *  History:       
 *  1.  Date: 2012.07.18      Version:   1.0     Author:  anve
 *       Modification:  first release
 *  2. ...
*************************************************/


#ifndef DRV_SSD132_H_
#define DRV_SSD132_H_

#define PIC_FROM_FLASH   0// 1 
#define USE_LM6069DFW_LCD

#include "define.h"

typedef enum
{
	SD1322_CMD_ENABLE_GRAY_SCALE_TABLE    = 0x00,
	SD1322_CMD_SET_COLUMN_ADDRESS         = 0x15,
	SD1322_CMD_WRITE_RAM_COMMAND          = 0x5C,
	SD1322_CMD_SET_ROW_ADDRESS            = 0x75,
	SD1322_CMD_SET_COMMAND_LOCK           = 0xFD,
	SD1322_CMD_SET_COMMAND_DISPLAYOFF          = 0xAE,
	SD1322_CMD_SET_COMMAND_OSCILLATAR_FREQUANCY           = 0xB3,
	SD1322_CMD_SET_COMMAND_multiplex_ratio           = 0xCA,
	SD1322_CMD_SET_COMMAND_set_offset           = 0xA2,
	SD1322_CMD_SET_COMMAND_start_line           = 0xA1,
	SD1322_CMD_SET_COMMAND_set_remap           = 0xA0,
	SD1322_CMD_SET_COMMAND_funtion_selection           = 0xAB,
	SD1322_CMD_SET_COMMAND_set_contrast_current           = 0xC1,
	SD1322_CMD_SET_COMMAND_master_contrast_current_control           = 0xC7,
	SD1322_CMD_SET_COMMAND_SET_PHASE_LENGTH           = 0xB1,
	SD1322_CMD_SET_COMMAND_VOLTAGE           = 0xBB,
	SD1322_CMD_SET_COMMAND_SECOND_PERIOD           = 0xB6,
	SD1322_CMD_SET_COMMAND_Vcomh           = 0xBE,
	SD1322_CMD_SET_COMMAND_normal_display          = 0xA6,
    SD1322_CMD_SET_COMMAND_display_ON           = 0xAF,
	
	
} SD1322_Cmd_TypeDef;


#if 1

typedef enum
{
	PIC_H_2_1 = 0,   	//水平一分二未选中
	PIC_V_2_1,   		//水平一分二未选中
	PIC_V_4_1, 			//水平一分二未选中
	PIC_H_4_1, 			//水平一分二未选中
	PIC_FILD, 			//水平一分二未选中
	PIC_4P,				//四画面未选中
	PIC_UN_LOCK, 		//水平一分二未选中

	PIC_H_2_1_F, 		//水平一分二未选中
	PIC_V_2_1_F, 		//水平一分二未选中
	PIC_H_4_1_F,		//水平一分二未选中
	PIC_V_4_1_F,		//水平一分二未选中
	PIC_FILD_F,			//水平一分二未选中
	PIC_4P_F,			//四画面选中
	PIC_LOCK, 			//水平一分二未选中

	PIC_PIP_OFF,
	PIC_PIP_LT,
	PIC_PIP_RT,
	PIC_PIP_LB,
	PIC_PIP_RB,
	PIC_PIP_CNTER,
	//PIC_PBP_LR,
	//PIC_PBP_TB,
	
	CUSTOM_LOGO,
	NumOfPic,
}PIC_FLASH_INDEX;

#else

#endif

void init_OLED(void);
void init_SSD1322_rst(void);
void init_SSD1322(void);

static void writeByte_SSD1322(BYTE value);

void writeCmd_SSD1322(BYTE cmd);
void writeData_SSD1322(BYTE value);

void write4Bit_SSD1322(BYTE value); // write high 4bit only
void Set_Column_Address(unsigned char a, unsigned char b) ;
void Set_Row_Address(unsigned char a, unsigned char b) ;


void writeStr_SSD1322(int row, int column, const char *str);
void writeStrF_SSD1322(int row, int column, const char *str);
void writeLine1_SSD1322(const char *str);
void writeLine2_SSD1322(const char *str);
void writeLine3_SSD1322(const char *str);							   
void writeLine4_SSD1322(const char *str);
void writeLine_SSD1322(int lineNumber, const char *str);
void writeLine_SSD1322_fb(int lineNumber, const char *str);

void write_SSD1322(WORD column, WORD row, const char *str);
void write_SSD1322_fb(WORD column, WORD row, const char *str);
void GetHzMat(const char *code,unsigned char *mat,u8 size);


void clear_SSD1322(void);
void brightness_SSD1322(char liangdu);
void LcdFillLink(WORD column0, WORD column1, WORD row0, WORD row1) ;

void LcdDrawAlphaBar(BYTE Dir,u32 Speed);
void LcdDrawAlphaManualBar(BYTE Grade);
void LcdDrawAlphaLinkBar(void);
//modify by elisen 20120905
void LcdDrawAlphaHorizontalBar(WORD column0, WORD column1,WORD row0,WORD row1);
//void LcdDispBMP(WORD column0, WORD row0,const char *pic);


void LcdDispEnCnStr(WORD x, WORD y, const char *pString);
void LcdDispEnCnStr_fb(WORD x, WORD y, const char *pString);
void LcdDispEnStr(WORD column, WORD row, BYTE str, BYTE fb);
void LcdDispCnStr(WORD column, WORD row, const char *str, BYTE fb);
void LcdFillBlock(WORD column0, WORD column1, WORD row0, WORD row1);
void LcdFillBlock_clear(WORD column0, WORD column1, WORD row0, WORD row1);
void LcdFillBlock2(WORD column0, WORD column1, WORD row0, WORD row1);

void LcdDrawStartFrame(BYTE percent) ;
void LcdDrawStartBar(BYTE percent);
void LcdDrawItemBar(BYTE TotalItem, BYTE CurItem, BYTE CurDispLine);
void LcdDrawTpItemBar(BYTE TotalItem, BYTE CurItem, BYTE CurDispLine);
void LcdDrawAlphaBar(BYTE Dir,u32 Speed);
void LcdDrawAlphaManualBar(BYTE Grade);
void LcdDrawAlphaLinkBar(void);
void LcdDrawAlphaHorizontalBar(WORD column0, WORD column1,WORD row0,WORD row1);
void LcdDispLogo(const char *pic);
void LcdDispLogoFlash(const char *pic);
void LcdDispLogo_Custom(void);
void LcdDispBMP(WORD column0, WORD row0,PIC_FLASH_INDEX pic);

void LcdDrawStartBarUpdata(BYTE index ,BYTE percent);
void UseTimeInfo(int time);
void LcdDrawLine(WORD column0, WORD column1, WORD row0, WORD row1);
 
void dispMenuPosBar(int current,int max);

BYTE getOLEDLum(void);
void setUserOLEDLum(BYTE value);
void writeStr_SSD1322_fb(int row, int column, const char *str);
//void setDispLineEn(void);

void writeLine1_SSD1322_L(const char *str);
void writeLine2_SSD1322_L(const char *str);
void writeLine3_SSD1322_L(const char *str);
void writeLine4_SSD1322_L(const char *str);


void init_OLED_1(void);

void LcdDispBMP_1(WORD column0, WORD row0,const char *pic);

void LcdDrawPoint(WORD x, WORD y);  //宽度1个点

void LcdDrowHorizontalLine(WORD x, WORD y, WORD len);
void LcdDrowVerticallLine(WORD x, WORD y, WORD len,u8 pos);

#endif



