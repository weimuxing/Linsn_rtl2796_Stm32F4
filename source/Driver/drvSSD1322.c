
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

#include "drvSSD1322.h"
#include "fonts.h"
#include "stdio.h"
#include "userPref.h"
#include "string.h"
#include "spi_flash.h"
#include "picture.h"
#include "SysTick.h"
#include "Spi_flash.h"
#include "soft_spi.h"



#ifdef VIP_43S

#define SSD1322_SCLK_PIN GPIO_Pin_5
#define SSD1322_SCLK_PORT GPIOG
// ok 
#define SSD1322_CS_PIN GPIO_Pin_4
#define SSD1322_CS_PORT GPIOG
// OK 
#define SSD1322_SID_PIN GPIO_Pin_3
#define SSD1322_SID_PORT GPIOG
// ok
#define SSD1322_RST_PIN GPIO_Pin_2
#define SSD1322_RST_PORT GPIOG

#define SSD1322_DC_PIN GPIO_Pin_4
#define SSD1322_DC_PORT GPIOG

#define SET_SSD1322_CS() while(0)// GPIO_SetBits(SSD1322_CS_PORT,SSD1322_CS_PIN)
#define RESET_SSD1322_CS() while(0)//GPIO_ResetBits(SSD1322_CS_PORT,SSD1322_CS_PIN)
#define SET_SSD1322_SCLK() GPIO_SetBits(SSD1322_SCLK_PORT,SSD1322_SCLK_PIN)
#define RESET_SSD1322_SCLK() GPIO_ResetBits(SSD1322_SCLK_PORT,SSD1322_SCLK_PIN)
#define SET_SSD1322_SID()  GPIO_SetBits(SSD1322_SID_PORT,SSD1322_SID_PIN)
#define RESET_SSD1322_SID() GPIO_ResetBits(SSD1322_SID_PORT,SSD1322_SID_PIN)	
#define SET_SSD1322_RST() GPIO_SetBits(SSD1322_RST_PORT,SSD1322_RST_PIN)
#define RESET_SSD1322_RST() GPIO_ResetBits(SSD1322_RST_PORT,SSD1322_RST_PIN)
#define SET_SSD1322_DC() GPIO_SetBits(SSD1322_DC_PORT,SSD1322_DC_PIN)
#define RESET_SSD1322_DC() GPIO_ResetBits(SSD1322_DC_PORT,SSD1322_DC_PIN)

#else
#define SSD1322_SCLK_PIN GPIO_Pin_9
#define SSD1322_SCLK_PORT GPIOC
#define SSD1322_CS_PIN GPIO_Pin_10
#define SSD1322_CS_PORT GPIOC
#define SSD1322_SID_PIN GPIO_Pin_12
#define SSD1322_SID_PORT GPIOC
#define SSD1322_RST_PIN GPIO_Pin_11
#define SSD1322_RST_PORT GPIOC

#define SSD1322_DC_PIN GPIO_Pin_10
#define SSD1322_DC_PORT GPIOC

#define SSD1322_AO_PIN GPIO_Pin_8
#define SSD1322_AO_PORT GPIOB

#define SET_SSD1322_CS() while(0)// GPIO_SetBits(SSD1322_CS_PORT,SSD1322_CS_PIN)
#define RESET_SSD1322_CS() while(0)//GPIO_ResetBits(SSD1322_CS_PORT,SSD1322_CS_PIN)
#define SET_SSD1322_SCLK() GPIO_SetBits(SSD1322_SCLK_PORT,SSD1322_SCLK_PIN)
#define RESET_SSD1322_SCLK() GPIO_ResetBits(SSD1322_SCLK_PORT,SSD1322_SCLK_PIN)
#define SET_SSD1322_SID()  GPIO_SetBits(SSD1322_SID_PORT,SSD1322_SID_PIN)
#define RESET_SSD1322_SID() GPIO_ResetBits(SSD1322_SID_PORT,SSD1322_SID_PIN)	
#define SET_SSD1322_RST() GPIO_SetBits(SSD1322_RST_PORT,SSD1322_RST_PIN)
#define RESET_SSD1322_RST() GPIO_ResetBits(SSD1322_RST_PORT,SSD1322_RST_PIN)
#define SET_SSD1322_DC() GPIO_SetBits(SSD1322_DC_PORT,SSD1322_DC_PIN)
#define RESET_SSD1322_DC() GPIO_ResetBits(SSD1322_DC_PORT,SSD1322_DC_PIN)
#define SET_SSD1322_AO() GPIO_SetBits(SSD1322_AO_PORT,SSD1322_AO_PIN)
#define RESET_SSD1322_AO() GPIO_ResetBits(SSD1322_AO_PORT,SSD1322_AO_PIN)

#endif

#define SSD1322_DELAY() //delay_SSD1322()
#define MAX_LINE_NUM 30//32
#define MAX_LINE_NUM_L 32


BYTE OLEDLum = 12;

#define FONT16ADDR   0x80


const u32 sizeOfPictures[] = 
{
	2048, //LOGO,
	48, //PIC_SCREEN, //屏参
	48, //PIC_SCREEN, //屏参
	48, //PIC_SINGLE, //单画面
	48, //PIC_PIP_TOP_LEFT, //画中画左上
	48, //PIC_PIP_TOP_RIGHT, //画中画右上
	48, //PIC_PIP_BOTTOM_LEFT,//画中画左下
	48, //PIC_PIP_BOTTOM_RIGHT,//画中画右下
	48, //PIC_PIP_CENT,     //画中画居中
	48, //PIC_PBP_LF,       //左右双画面
	48, //PIC_PBP_TB,       //上下双画面
	48, //PIC_TRANS_FADE,       //淡入淡出
	48, //PIC_TRANS_CUT,        //硬切
	48, //PIC_TRANS_WIPE_SQUARE_IN,       //从中心划变
	48, //PIC_TRANS_WIPE_SQUARE,			//向中心划变
	48, //PIC_TRANS_WIPE_TOP_LEFT_IN,  //从左上划变
	48, //PIC_TRANS_WIPE_TOP_LEFT,     //向左上划变
	48, //PIC_TRANS_WIPE_TOP_RIGHT_IN, //从右上划变
	48, //PIC_TRANS_WIPE_TOP_RIGHT,    //向右上划变
	48, //PIC_TRANS_WIPE_BOTTOM_LEFT_IN,  //从左下划变
	48, //PIC_TRANS_WIPE_BOTTOM_LEFT,     //向左下划变
	48, //PIC_TRANS_WIPE_BOTTOM_RIGHT_IN, //从右下划变
	48, //PIC_TRANS_WIPE_BOTTOM_RIGHT,    //向右下划变
	48, //PIC_TRANS_WIPE_LEFT_IN,         //从左划变
	48, //PIC_TRANS_WIPE_LEFT,			//向左划变
	48, //PIC_TRANS_WIPE_RIGHT_IN,		//从右划变
	48, //PIC_TRANS_WIPE_RIGHT,           //向右划变
	48, //PIC_TRANS_WIPE_UP_IN,			//从上划变
	48, //PIC_TRANS_WIPE_UP, 				//向上划变
	48, //PIC_TRANS_WIPE_DOWN_IN,			//从下划变
	48, //PIC_TRANS_WIPE_DOWN,			//向下划变
	48, //PIC_DISP_BLACK,                 //黑场
	48, //PIC_DISP_LIVE,					//视频
	48, //PIC_DISP_FREEZE,				//冻结
	48, //PIC_DISP_PURE,					//纯色
	48, //PIC_DISP_TP,					//测试图像
	48, //PIC_UNLOCK,						//解锁
	48, //PIC_LOCK, 						//锁定
	48, //PIC_NETWORK_SINGLE,				//单机
	48, //PIC_NETWORK_INTERNET,           //网络
	48, //PIC_SPLIT,                      //拼接
	48, //PIC_RECALL_1,                   //调保存1
	48, //PIC_RECALL_2,                   //调保存2
	48, //PIC_RECALL_3,                   //调保存3
	48, //PIC_RECALL_4,                   //调保存4
	48, //PIC_RECALL_5,                   //调保存5
	48, //PIC_RECALL_6,                   //调保存6
	48, //PIC_RECALL_7,                   //调保存7
	48, //PIC_RECALL_8,                   //调保存8
	48, //PIC_RECALL_9,                   //调保存9
	48, //PIC_RECALL_10,                  //调保存10
	

	2048, //CUSTOM_LOGO,
};

const unsigned char liangdu_L[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};   
const unsigned char liangdu_H[]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};


u32 getPictureOffset(PIC_FLASH_INDEX pic)
{
	u8 i = 0;
	u32 picFlashAddr = 0;
	if(pic >= NumOfPic)
	{
		return 0;
	}
	while(pic--)
	{
		picFlashAddr += picFlashAddr + sizeOfPictures[i++]; 
	}
	return picFlashAddr;
}

extern int lang_GetLang(void);

void init_SSD1322_rst(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	
	/* config RST pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_RST_PIN;
	GPIO_Init(SSD1322_RST_PORT, &GPIO_InitStruct);
	RESET_SSD1322_RST();
}


#ifdef VIP_43S
void init_SSD1322(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	
	/* config RST pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_RST_PIN;
	GPIO_Init(SSD1322_RST_PORT, &GPIO_InitStruct);
	RESET_SSD1322_RST();
	/* config SCLK pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_SCLK_PIN;
	GPIO_Init(SSD1322_SCLK_PORT, &GPIO_InitStruct);
	RESET_SSD1322_SCLK();
	GPIO_InitStruct.GPIO_Pin = SSD1322_SID_PIN;
	GPIO_Init(SSD1322_SID_PORT, &GPIO_InitStruct);
	RESET_SSD1322_SID();
	/* config DC pin  */
	GPIO_InitStruct.GPIO_Pin = SSD1322_DC_PIN;
	GPIO_Init(SSD1322_DC_PORT, &GPIO_InitStruct);
	RESET_SSD1322_DC();

	SysDelay1ms(200);	
	
	/* reset SSD1322 */
	SET_SSD1322_RST();
}

#else
void init_SSD1322(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;


	/* config AO pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_AO_PIN;
	GPIO_Init(SSD1322_AO_PORT, &GPIO_InitStruct);
	RESET_SSD1322_AO();
	
	/* config RST pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_RST_PIN;
	GPIO_Init(SSD1322_RST_PORT, &GPIO_InitStruct);
	RESET_SSD1322_RST();
	/* config SCLK pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_SCLK_PIN;
	GPIO_Init(SSD1322_SCLK_PORT, &GPIO_InitStruct);
	RESET_SSD1322_SCLK();
	/* config CS pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_CS_PIN;
	GPIO_Init(SSD1322_CS_PORT, &GPIO_InitStruct); 
	SET_SSD1322_CS();
	/* config SID pin */
	GPIO_InitStruct.GPIO_Pin = SSD1322_SID_PIN;
	GPIO_Init(SSD1322_SID_PORT, &GPIO_InitStruct);
	RESET_SSD1322_SID();
	/* config DC pin  */
	GPIO_InitStruct.GPIO_Pin = SSD1322_DC_PIN;
	GPIO_Init(SSD1322_DC_PORT, &GPIO_InitStruct);
	RESET_SSD1322_DC();
	
	/* reset SSD1322 */
	SET_SSD1322_RST();
}

#endif
/******************************************/	
static void writeByte_SSD1322(BYTE value)
{
	int i;
	RESET_SSD1322_SCLK();
	for(i=0; i<8; i++)
	{
		RESET_SSD1322_SCLK();
		if(value & 0x80)
		{
			SET_SSD1322_SID();
		}
		else
		{
			RESET_SSD1322_SID();
		}
		value <<= 1;
		SET_SSD1322_SCLK();
		SSD1322_DELAY();
	}
	RESET_SSD1322_SID();
}



void writeCmd_SSD1322(BYTE cmd)
{
	RESET_SSD1322_DC();
	RESET_SSD1322_CS();
	writeByte_SSD1322(cmd);	
	SET_SSD1322_CS();	
}

void writeData_SSD1322(BYTE value)
{
	SET_SSD1322_DC();
	RESET_SSD1322_CS();
	writeByte_SSD1322(value);	
	SET_SSD1322_CS();	
}

void write4Bit_SSD1322(BYTE value) // write high 4bit only
{
	BYTE i;
	
	SET_SSD1322_DC();
	RESET_SSD1322_CS();
	
	RESET_SSD1322_SCLK();
	value <<= 4;	
	for(i=0; i<4; i++)
	{
		RESET_SSD1322_SCLK();
		if(value & 0x80)
		{
			SET_SSD1322_SID();
		}
		else
		{
			RESET_SSD1322_SID();
		}
		value <<= 1;
		SET_SSD1322_SCLK();
		SSD1322_DELAY();
	}
	RESET_SSD1322_SID();
	
	SET_SSD1322_CS();	
}

void Set_Column_Address(unsigned char a, unsigned char b) 
{ 
	 writeCmd_SSD1322(SD1322_CMD_SET_COLUMN_ADDRESS); 
	 writeData_SSD1322(a);    /* Default => 0x00*/ 
	 writeData_SSD1322(b);    /* Default => 0x77*/ 
}  
   
void Set_Row_Address(unsigned char a, unsigned char b) 
{ 
 writeCmd_SSD1322(SD1322_CMD_SET_ROW_ADDRESS); 
 writeData_SSD1322(a);    /* Default => 0x00 */ 
 writeData_SSD1322(b);    /* Default => 0x7F */ 
} 


void writeStr_SSD1322(int row, int column, const char *str)		 //4行32个字符
{
	LcdDispEnCnStr(column, row, str);
}

void writeStr_SSD1322_fb(int row, int column, const char *str)		 //4行32个字符
{

	LcdDispEnCnStr_fb(column, row, str);
}

const char lineBlank[]   = "                           ";
const char lineBlank_L[] = "                                ";

/************************************************************************/
void writeLine1_SSD1322(const char *str)
{

	LcdDispEnCnStr(0, 0, lineBlank);
	LcdDispEnCnStr(0, 0, str);

}								   				   	  
void writeLine1_SSD1322_L(const char *str)
{

	LcdDispEnCnStr(0, 0, lineBlank_L);
	LcdDispEnCnStr(0, 0, str);
}		

/***********************************************************************/
void writeLine2_SSD1322(const char *str)
{

	LcdDispEnCnStr(0, 1, lineBlank);
	LcdDispEnCnStr(0, 1, str);

}								   				

void writeLine2_SSD1322_L(const char *str)
{
	LcdDispEnCnStr(0, 1, lineBlank_L);
	LcdDispEnCnStr(0, 1, str);
}								   				

/********************************************************************/
void writeLine3_SSD1322(const char *str)
{
	LcdDispEnCnStr(0, 2, lineBlank);
	LcdDispEnCnStr(0, 2, str);

}
void writeLine3_SSD1322_L(const char *str)
{

	LcdDispEnCnStr(0, 2, lineBlank_L);
	LcdDispEnCnStr(0, 2, str);
}								   		

/*******************************************************************/
void writeLine4_SSD1322(const char *str)
{

	LcdDispEnCnStr(0, 3, lineBlank);
	LcdDispEnCnStr(0, 3, str);
}								   		
void writeLine4_SSD1322_L(const char *str)
{
	LcdDispEnCnStr(0, 3, lineBlank_L);
	LcdDispEnCnStr(0, 3, str);
}								   		

/********************************************************************/
void writeLine_SSD1322(int lineNumber, const char *str)
{
	switch(lineNumber)
	{
		case 0: writeLine1_SSD1322(str); break;
		case 1: writeLine2_SSD1322(str); break;
		case 2: writeLine3_SSD1322(str); break;
		case 3: writeLine4_SSD1322(str); break;
	}
}

void writeLine_SSD1322_fb(int lineNumber, const char *str)
{

	LcdDispEnCnStr_fb(0, lineNumber, lineBlank);
	LcdDispEnCnStr_fb(0, lineNumber, str);
}

void write_SSD1322(WORD column, WORD row, const char *str)
{
	LcdDispEnCnStr(column, row, str);
}

void write_SSD1322_fb(WORD column, WORD row, const char *str)
{
	LcdDispEnCnStr_fb(column, row, str);
}



/****************************************************************/
void clear_SSD1322() 
{ 
	unsigned char x,y; 
	writeCmd_SSD1322(SD1322_CMD_SET_COLUMN_ADDRESS); 
	writeData_SSD1322(0x00); 
	writeData_SSD1322(0x77);    
    writeCmd_SSD1322(SD1322_CMD_SET_ROW_ADDRESS); 
	writeData_SSD1322(0x00); 
	writeData_SSD1322(0x7f); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 
	for(y=0;y<128;y++) 
	{ 
		for(x=0;x<120;x++) 
		{ 
			writeData_SSD1322(0x00); 
			writeData_SSD1322(0x00);   
		} 
	}               
} 

/**************************************************************/
void init_OLED()
{
	init_SSD1322();
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_LOCK);            
	writeData_SSD1322(0X12);              /* UNLOCK */ 					
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_DISPLAYOFF);                       
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_OSCILLATAR_FREQUANCY);
	writeData_SSD1322(0X91); 		//divide by 2
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_multiplex_ratio);
	writeData_SSD1322(0X3F); 		/*duty = 1/64*/ 
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_set_offset );
	writeData_SSD1322(0X00); 
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_start_line);
	writeData_SSD1322(0X00);		// from 0th line
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_set_remap);
	writeData_SSD1322(0X14);		//Enable Nibble Re-map & COM[N-1] to COM0
	writeData_SSD1322(0X11);		//Enable Dual COM mode (MUX  ≤  63) 

	//added 20140712
	writeCmd_SSD1322(0xB5);        //
	writeData_SSD1322(0x00);
	
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_funtion_selection );
	writeData_SSD1322(0X01);		/* selection external vdd */

	//added 20140712
	writeCmd_SSD1322(0xB4);        //
	writeData_SSD1322(0xA0);
	writeData_SSD1322(0xFD);	
	
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_set_contrast_current);
	writeData_SSD1322(0X9F);		//0x00~0xff 
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_master_contrast_current_control );
	writeData_SSD1322(0X0F);		//0x00~0x0f full color
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_SET_PHASE_LENGTH);
	writeData_SSD1322(0XE2);		//reset phase length 5 DCLKs &first pre-charge phase length 14 DCLKs

	//added 20140712
	writeCmd_SSD1322(0xD1);        //
	writeData_SSD1322(0x82);
	writeData_SSD1322(0x20);	
	writeCmd_SSD1322(0xBB);        //
	writeData_SSD1322(0x1F);
	writeCmd_SSD1322(0xB6);        //
	writeData_SSD1322(0x08);
	writeCmd_SSD1322(0xBE);        //
	writeData_SSD1322(0x07);
	
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_normal_display);
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_display_ON);

	clear_SSD1322();
}

void init_OLED_1(void)
{
	init_SSD1322();	
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_LOCK);            
	writeData_SSD1322(0X12);              /* UNLOCK */  
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_DISPLAYOFF);                       
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_OSCILLATAR_FREQUANCY);
	writeData_SSD1322(0X91);  //divide by 2
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_multiplex_ratio);
	writeData_SSD1322(0X3F);  /*duty = 1/64*/ 
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_set_offset );
	writeData_SSD1322(0X00); 
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_start_line);
	writeData_SSD1322(0X00); // from 0th line
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_set_remap);
	writeData_SSD1322(0X14); //Enable Nibble Re-map & COM[N-1] to COM0
	writeData_SSD1322(0X11); //Enable Dual COM mode (MUX  ≤  63) 
	
	//added 20140712
	writeCmd_SSD1322(0xB5);        //
	writeData_SSD1322(0x00);
	
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_funtion_selection );
	writeData_SSD1322(0X01); /* selection external vdd */
	
	//added 20140712
	writeCmd_SSD1322(0xB4);        //
	writeData_SSD1322(0xA0);
	writeData_SSD1322(0xFD);	
	
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_set_contrast_current);
	writeData_SSD1322(0X9F); //0x00~0xff 
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_master_contrast_current_control );
	writeData_SSD1322(0X0F); //0x00~0x0f full color
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_SET_PHASE_LENGTH);
	writeData_SSD1322(0XE2); //reset phase length 5 DCLKs &first pre-charge phase length 14 DCLKs

	//added 20140712
	writeCmd_SSD1322(0xD1);        //
	writeData_SSD1322(0x82);
	writeData_SSD1322(0x20);	
	writeCmd_SSD1322(0xBB);        //
	writeData_SSD1322(0x1F);
	writeCmd_SSD1322(0xB6);        //
	writeData_SSD1322(0x08);
	writeCmd_SSD1322(0xBE);        //
	writeData_SSD1322(0x07);
	
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_normal_display);
	writeCmd_SSD1322(SD1322_CMD_SET_COMMAND_display_ON);
}




void LcdDrowHorizontalLine(WORD x, WORD y, WORD len)
{
	WORD k,j,num;
	unsigned char liangdu;
	liangdu = OLEDLum;
	
	if(liangdu>15) 
		liangdu = 15;
	len = len/4;
	
	Set_Column_Address(0x1C+x,0x1C+len-1); 
	Set_Row_Address(y,y); 				 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);
				
    for(k=0;k<len;k++)
	{
		unsigned char temp = 0xff;//ASCII8x16_Table[num+k];
		unsigned char shift = 0x80;
		unsigned char data=0x00,data2=0x00;
		
		if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
		else data &=liangdu_L[liangdu];
		shift >>= 1;			  
		if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
		else data &=liangdu_H[liangdu];
		shift >>= 1;
		if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
		else data2 &=liangdu_L[liangdu];
		shift >>= 1;
		if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
		else data2 &=liangdu_H[liangdu];	
		shift >>= 1;
		writeData_SSD1322(data);
		writeData_SSD1322(data2);
	}
}



void LcdDrowVerticallLine(WORD x, WORD y, WORD len,u8 pos)
{
	WORD k,j,num;
	unsigned char liangdu;
	liangdu = OLEDLum;
	unsigned char temp;//ASCII8x16_Table[num+k];
	unsigned char shift = 0x80;
	unsigned char data=0x00,data2=0x00;
	
	if(liangdu>15) 
		liangdu = 15;
	len = len/4;
	
	Set_Column_Address(0x1C+x,0x1C+x); 
	Set_Row_Address(y,len); 				 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);

	switch(pos)
	{
		case 0:
			data |= liangdu_H[liangdu];
			data2 = 0;//liangdu_L[liangdu];
		break;
		case 1:
			data = 0;//liangdu_H[liangdu];
			data2 = 0x0c;//liangdu_L[liangdu];
		break;
		default:
		break;
	}
				
    for(k=0;k<len;k++)
	{
		writeData_SSD1322(data);
		writeData_SSD1322(data2);	
	}
}


/**************************************************************/

void GetHzMat(const char *code,unsigned char *mat,u8 size) 
{ 
    unsigned char qh,ql; 
    unsigned long foffset;  
    qh=*code; 
    ql=*(++code); 
	ql -= 0xa1; 
	qh -= 0xa1; 
	foffset = ((unsigned long)94*qh + ql)*(size);
	SoftSpi_UsePort(SPI_PORT_C);
	SPI_FLASH_BufferRead(mat,foffset+0,size);
	SoftSpi_UnUsePort(SPI_PORT_C);
}

void LcdDispEnStr(WORD column, WORD row, BYTE str, BYTE fb)
{
	WORD k,j,num;
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) liangdu = 15;
	
	Set_Column_Address(0x1C+0x00+0x02*column,0x1C+0x01+0x02*column); 
	Set_Row_Address(0x00+row*16,0x0f+row*16); 				 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);

	num=(str-32)*16;				
    for(k=0;k<16;k++)
	{
		unsigned char temp=ASCII8x16_Table[num+k];
		unsigned char shift = 0x80;
		if(fb) {temp = ~temp;}
		
		for(j=0;j<2;j++)										  
		{
			unsigned char data=0x00,data2=0x00;
			if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
			else data &=liangdu_L[liangdu];
			shift >>= 1;			  
			if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
			else data &=liangdu_H[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
			else data2 &=liangdu_L[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
			else data2 &=liangdu_H[liangdu];	
			shift >>= 1;
			writeData_SSD1322(data);
			writeData_SSD1322(data2);
		}   	
	}
}
void LcdDispCnStr(WORD column, WORD row, const char *str, BYTE fb)
{
	WORD k,j;
	BYTE HZbuf[32] = {	
						0x3F,0xF0,0x20,0x20,0x3F,0xF0,0x00,0x00,0x7F,0xFF,0x42,0x08,0x7D,0xF0,0x00,0x00,
						0x4F,0xF0,0x48,0x20,0x4F,0xF2,0x40,0x01,0x7F,0xFE,0x40,0x00,0x00,0x00,0x00,0x00
					};/*"?",0*/
	unsigned char liangdu;	
	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;
	
	Set_Column_Address(0x1C+0x00+0x02*column,0x1C+0x03+0x02*column); 
	Set_Row_Address(0x00+row*16,0x0f+row*16); 				 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);

	GetHzMat(str, HZbuf, 32);	
    for(k=0;k<32;k++)
	{
		unsigned char temp= HZbuf[k];								  
		unsigned char shift = 0x80;

		if(fb) {temp = ~temp;}

		for(j=0;j<2;j++)										  
		{
			unsigned char data=0x00,data2=0x00;
			if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
			else data &=liangdu_L[liangdu];
			shift >>= 1;			  
			if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
			else data &=liangdu_H[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
			else data2 &=liangdu_L[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
			else data2 &=liangdu_H[liangdu];	
			shift >>= 1;	  
			writeData_SSD1322(data);
			writeData_SSD1322(data2);
		}   	
	}

}

void Drv_DisplayEnCharacter(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh);
void Drv_DisplayEnCharacterReverse(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh);
void Drv_DisplayCnCharacter(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh);
void Drv_DisplayCnCharacterReverse(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh);

void LcdDispEnCnStr(WORD column, WORD row, const char *pString)
{
	const char *pstr;

#if defined(USE_LM6069DFW_LCD)
	if(column>=28 || row>=8)          
		return;
	pstr = pString;
	while(*pstr != '\0')
	{
		if(*pstr <= 0x80)
		{
			//if()
			Drv_DisplayEnCharacter(8,row*2, column*8, (uint8*)pstr);
			column += 1;
			pstr += 1;
		}
		else
		{
			//LcdDispCnStr(column, row, pstr, 0);
			Drv_DisplayCnCharacter(16,row*2, column*8, (uint8*)pstr);
			column += 2;
			pstr += 2;
		}
	}
#else
	if(column>=64 || row>=4)          
		return;
	pstr = pString;
	while(*pstr != '\0')
	{
		if(*pstr <= 0x80)
		{
			//if()
			LcdDispEnStr(column, row, *pstr, 0);
			column += 1;
			pstr += 1;
		}
		else
		{
			LcdDispCnStr(column, row, pstr, 0);
			column += 2;
			pstr += 2;
		}
	}
#endif
}


// 反白显示
void LcdDispEnCnStr_fb(WORD column, WORD row, const char *pString)
{
	const char *pstr;
#if defined(USE_LM6069DFW_LCD)
	if(column>=28 || row>=8)          
		return;
	pstr = pString;
	while(*pstr != '\0')
	{
		if(*pstr <= 0x80)
		{
			//if()
			Drv_DisplayEnCharacterReverse(8,row*2, column*8, (uint8*)pstr);
			column += 1;
			pstr += 1;
		}
		else
		{
			//LcdDispCnStr(column, row, pstr, 0);
			Drv_DisplayCnCharacterReverse(16,row*2, column*8, (uint8*)pstr);
			column += 2;
			pstr += 2;
		}
	}

#else
	if(column>=64 || row>=16)
		return;	
	pstr = pString;
	while(*pstr != '\0')
	{
	    if(column > 0x5b)
            printf("this is error column\r\n");
		//if((x+nByte)>=16)
		//	break;	
		if(*pstr <= 0x80)
		{
			//Drv_DisplayEnCharacterReverse(8,row*2, column, (uint8*)pstr);
			LcdDispEnStr(column, row, *pstr, 1);
			column += 1;
			pstr += 1;
		}
		else
		{
			LcdDispCnStr(column, row, pstr, 1);
			column += 2;
			pstr += 2;
		}
	}
#endif
}

void LcdFillBlock(WORD column0, WORD column1, WORD row0, WORD row1) 
{ 
	unsigned char i,j; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;

	liangdu = liangdu_H[OLEDLum]|liangdu_L[OLEDLum];
	Set_Column_Address(0x1C+column0, 0x1C+column1); 
	Set_Row_Address(row0, row1); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 
	for(i=0;i<(row1-row0+1);i++) 
	{ 
		for(j=0;j<(column1-column0+1);j++) 
		{ 
			writeData_SSD1322(liangdu); //上半屏
			writeData_SSD1322(liangdu); //下半屏
		} 
	} 
}

void LcdFillBlock_clear(WORD column0, WORD column1, WORD row0, WORD row1) 
{
	int temp;

	temp = OLEDLum;
	OLEDLum = 0;
	LcdFillBlock(column0, column1, row0, row1);
	OLEDLum = temp;
}

void LcdFillBlock2(WORD column0, WORD column1, WORD row0, WORD row1) 
{ 
	unsigned char i; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;
	
	Set_Column_Address(0x1C+column0, 0x1C+column1); 
	Set_Row_Address(row0, row1); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 

	for(i=0;i<(row1-row0+1);i++) 
	{ 
		writeData_SSD1322(0x00);	
		writeData_SSD1322(liangdu_L[liangdu]); 
		writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]); 
		writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]); 
	} 
}

void LcdFillLink(WORD column0, WORD column1, WORD row0, WORD row1)  //虚线
{ 
	unsigned char i; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	
	if(liangdu>15)
		liangdu = 15;
	
	Set_Column_Address(0x1C+column0, 0x1C+column1); 
	Set_Row_Address(row0, row1); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 

	for(i=0;i<(row1-row0+1);i++) 
	{ 
		writeData_SSD1322(0x00); 
		if((i%2) == 0) {writeData_SSD1322(0x00);}
		else {writeData_SSD1322(liangdu_L[liangdu]);}
	} 
}


void LcdDrawPoint(WORD x, WORD y)  //宽度1个点
{  
	int i,j; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;
	Set_Column_Address(0x1C+x, 0x1C+x); 
	Set_Row_Address(y, y);
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 

	writeData_SSD1322(0x00); 
	if((x+1) % 2)
		writeData_SSD1322(liangdu_L[liangdu]);
	else
		writeData_SSD1322(liangdu_H[liangdu]);
}

void LcdDrawLineW1(WORD column0, WORD column1, WORD row0, WORD row1)  //实线 宽度1个点
{  
	int i,j; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;
	Set_Column_Address(0x1C+column0, 0x1C+column1); 
	Set_Row_Address(row0, row1);
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 
	for(j = 0;j < column1-column0+1;j++)
	{
		for(i=0;i<(row1-row0+1);i++) 
		{ 
			writeData_SSD1322(0x00); 
			writeData_SSD1322(liangdu_L[liangdu]);
			writeData_SSD1322(liangdu_H[liangdu]);
		} 
	}
}


void LcdDrawLineW2(WORD column0, WORD column1, WORD row0, WORD row1)  //实线 宽度2个点
{  
	// x: 0~128 (256/4)
	// y: 0~64
	unsigned char i; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;
	
	liangdu = liangdu_H[OLEDLum]|liangdu_L[OLEDLum];

	Set_Column_Address(0x1C+column0, 0x1C+column1); 
	Set_Row_Address(row0, row1); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 

	for(i=0;i<(row1-row0+1);i++) 
	{ 
		writeData_SSD1322(0x00); 
		writeData_SSD1322(liangdu);
	} 
}




void LcdDrawSnow(WORD x0, WORD y0, BYTE width, BYTE height) 
{ 
	BYTE x, y;
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) liangdu = 15;
	
	Set_Column_Address(0x1C+x0, 0x1C+x0+width); 
	Set_Row_Address(y0, y0+ height); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 

	for(y=0;y<(height/2+1);y++) 
	{ 
		for(x=0;x<(width+1);x++) 
		{ 
		  writeData_SSD1322(liangdu_H[OLEDLum]);//(0xf0); 
		  writeData_SSD1322(liangdu_H[OLEDLum]);//(0xf0);   
		} 
		for(x=0;x<(width+1);x++) 
		{ 
			writeData_SSD1322(liangdu_L[OLEDLum]);
			writeData_SSD1322(liangdu_L[OLEDLum]); 
		} 
	}  
}

void LcdDrawStartFrame(BYTE percent) 
{ 
    unsigned char x,y; 

	unsigned char xStart,xEnd,width,height;

	xStart = 1;
	xEnd = 62;
	width = xEnd - xStart;
	height = 14;
	
	Set_Column_Address(0x1C+0, 0x1C+0+width); 
	Set_Row_Address(0, height); 	
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 
	
	for(x=xStart;x<width;x++) 
	{ 
		writeData_SSD1322(0xFF); 
	} 
	for(y=0;y<(height-2);y++) 
	{   
		writeData_SSD1322(0xf0); 
		writeData_SSD1322(0x00); 
		for(x=xStart;x<(width-2);x++) 
		{     
			writeData_SSD1322(0x00);   
			writeData_SSD1322(0x00);  
		}  
		writeData_SSD1322(0x00); 
		writeData_SSD1322(0x0f); 
	}        
	for(x=xStart;x<width;x++) 
	{ 
		writeData_SSD1322(0xFF); 
	} 
}

void LcdDrawStartBar(BYTE percent)
{
	#define SYSINIT_TIME 140
	char buf[40];
	static u8 percent_old = 2;

	if(percent > SYSINIT_TIME) 
		return;
	

	if(lang_GetLang() == 0)
		sprintf(buf, " System Initializing %d%c", (percent*100)/SYSINIT_TIME, '%');
	else
		sprintf(buf, "   系统启动中.... %d%c", (percent*100)/SYSINIT_TIME, '%');

	LcdDispEnCnStr(3, 2, buf);	
	percent = ((percent*100)/SYSINIT_TIME)/2;
	LcdFillBlock(percent_old, 10+percent, 55, 63);
	percent_old = 10+percent; 
}


void LcdDrawStartBarUpdata(BYTE index ,BYTE percent)
{

	char buf[40];
	static u8 percent_old = 2;

	if(percent > 100) 
	{
		percent_old = 2;
		return;
	}
	sprintf(buf, " Upgrade Please Waiting... ");
	if (index == 1)
	{
		sprintf(buf, "Kernel Firmware Upgrade %d%c", percent, '%');
		
	}
	else
	{
		sprintf(buf, "   Chinese Font Upgrade %d%c", percent, '%');
		
	}

	LcdDispEnCnStr(1, 2, buf);			
	percent = percent/2;
	LcdFillBlock(percent_old, 10+percent, 55, 63);
	percent_old = 10+percent; 
}


void UseTimeInfo(int time)
{
	char buf[40];
	if(lang_GetLang() == 0)
		sprintf(buf, "Upgrade Use Time = %d Seconds",time);
	else
		sprintf(buf, "升级使用时间 = %d 秒",time);
	LcdDispEnCnStr(0, 3, buf);

}

void LcdDrawItemBar(BYTE TotalItem, BYTE CurItem, BYTE CurDispLine)
{
//	char buf[30];
	int DispX,DispY,DispH;
	BYTE temp;

 	TotalItem --;// remove Menu Lable,
 	CurItem --; // >=1
	
	DispX = 62;
	DispH = 63/TotalItem;	
	DispY = (63)*CurItem/TotalItem;
	if(DispH<1) DispH = 1;

	temp = OLEDLum;
	OLEDLum = 0;
	LcdFillBlock(60, 60, 0, 63);
	LcdFillBlock(62, 63, 0, 63);	
	OLEDLum = temp;	
	LcdFillLink(61, 61, 0, 63);
	LcdFillBlock2(DispX, DispX+1, DispY, DispY+DispH);
}


void LcdDrawTpItemBar(BYTE TotalItem, BYTE CurItem, BYTE CurDispLine)
{
	int DispX,DispY,DispH;
	BYTE temp;
 	TotalItem --;// remove Menu Lable,
 	CurItem --; // >=1
	DispX = 62;
	DispH = 63/TotalItem;	
	DispY = (63)*CurItem/TotalItem;
	if(DispH<1) 
		DispH = 1;
	temp = OLEDLum;
	OLEDLum = 0;
	LcdFillBlock(62, 63, 0, 63);	
	OLEDLum = temp;	
	LcdFillLink(61, 61, 0, 63);
	LcdFillBlock2(DispX, DispX+1, DispY, DispY+DispH);
}


void LcdDrawAlphaBar(BYTE Dir,u32 Speed)
{
	int i;
	BYTE MAX_ITEM = 48-4;//42 grade

	
	if (Dir)
	{
		for(i=0;i<=MAX_ITEM;i++) 
		{
			LcdDrawAlphaManualBar(i);
			SysDelay1ms(Speed);
		}
		return;
	}
	else
	{
		for(i=MAX_ITEM-1;i>=0;i--)
		{
			LcdDrawAlphaManualBar(i);
			SysDelay1ms(Speed);
		}
		return;
	}
	
}


//modify by elisen 20120904
//0~42 Grade
void LcdDrawAlphaManualBar(BYTE Grade)
{
	int DispX,DispY,DispH,DispW;
	BYTE temp;
	int i;
	unsigned char j,l; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;

	DispX = 59;
	DispW = DispX+4;
	DispH = 4;	//black size
	temp = OLEDLum;
	OLEDLum = 0;
	LcdFillBlock(DispX, DispW, 0, 48);	
	OLEDLum = temp;
	 	
	if (Grade <= 0)	 		
		i = 0;		
	else if (Grade >=  44)
		i = 44;		
	else
		i = Grade;
		
	DispY = i;
	Set_Column_Address(0x1C+DispX, 0x1C+DispW); 
	Set_Row_Address(DispY, DispY+DispH); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 
	
	for(j=0;j<(DispH+1);j++) 
		for(l=DispX;l<=DispW;l++)
			{ 
				writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]);	
				writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]);
				writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]);	
				writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]);			
			} 
	
	LcdDrawAlphaLinkBar();
	return; 
}

void LcdDrawAlphaLinkBar(void)
{

		// x: 0~128 (256/4)
		// y: 0~64
		unsigned char i; 
		//unsigned char liangdu_L[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};   
		//unsigned char liangdu_H[]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
		unsigned char liangdu;	
		WORD column0 = 61;
		WORD column1 = 61;
		WORD row0	 = 0;
		WORD row1 	 = 48;
		
		liangdu = OLEDLum;
		if(liangdu>15) liangdu = 15;
		
		Set_Column_Address(0x1C+column0, 0x1C+column1); 
		Set_Row_Address(row0, row1); 
		writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 

		for(i=0;i<(row1-row0+1);i++) 
		{
			writeData_SSD1322(liangdu_L[liangdu]); 
			writeData_SSD1322(liangdu_H[liangdu] );
		}



}


//modify by elisen 20120905
void LcdDrawAlphaHorizontalBar(WORD column0, WORD column1,WORD row0,WORD row1)
{

	BYTE temp;
	int i;
	unsigned char j; 
	unsigned char liangdu;	
	liangdu = OLEDLum;
	if(liangdu>15) 
		liangdu = 15;

	temp = OLEDLum;
	OLEDLum = 0;
	LcdFillBlock(column0, column1, row0, row1);	
	OLEDLum = temp;
	
	Set_Column_Address(0x1C+column0, 0x1C+column1); 
	Set_Row_Address(row0+6, row1-6); 
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND); 

	
	for(j=row0+6;j<=row1-6;j++) //height
		for (i=column0;i<=column1;i++)
	{ 
		writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]);	
		writeData_SSD1322(liangdu_H[liangdu]|liangdu_L[liangdu]); 
	} 
	
	return; 
}

void Lcd_write_data(char temp)
{
	WORD j;
	unsigned char shift;
		
	//unsigned char liangdu_L[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};   
	//unsigned char liangdu_H[]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
	unsigned char liangdu=OLEDLum;	
	
	shift = 0x80;
	for(j=0;j<2;j++)	//8									  
	{
		unsigned char data=0x00,data2=0x00;
		if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
		else data &=liangdu_L[liangdu];
		shift >>= 1;			  
		if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
		else data &=liangdu_H[liangdu];
		shift >>= 1;
		if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
		else data2 &=liangdu_L[liangdu];
		shift >>= 1;
		if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
		else data2 &=liangdu_H[liangdu];	
		shift >>= 1;	  
		writeData_SSD1322(data);
		writeData_SSD1322(data2);
	}   	

}



#if 1
void LcdDispFlashBMP(WORD column0,WORD column1, WORD row0, WORD row1,PIC_FLASH_INDEX pic)
{
	#if 1
	u32 addr = 0,byteToRead = 0;
	u8 pBuffer = 0;
	
	byteToRead = sizeOfPictures[pic];

	if(pic == CUSTOM_LOGO)
	{
		addr = SPI_FLASH_LOGO_OFFSET;
	}
	else
	{
		addr = SPI_FLASH_PIC_OFFSET + getPictureOffset(pic);
	}	
	
	//printf("addr: 0x%x,len: %d\r\n",addr,byteToRead);
	
	Set_Column_Address(0x1C+column0,0x1C+column1); 
	//printf("column: %d,%d\r\n",column0,column1);
	Set_Row_Address(row0,row1); 	
	//printf("row: %d,%d\r\n",row0,row1);
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);
	
	SoftSpi_UsePort(SPI_PORT_A);
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "Read from Memory " instruction */
	SPI_FLASH_SendByte(CMD_READ);

	/* Send ReadAddr high nibble address byte to read from */
	SPI_FLASH_SendByte((addr & 0xFF0000) >> 16);
	/* Send ReadAddr medium nibble address byte to read from */
	SPI_FLASH_SendByte((addr& 0xFF00) >> 8);
	/* Send ReadAddr low nibble address byte to read from */
	SPI_FLASH_SendByte(addr & 0xFF);

	while (byteToRead--) /* while there is data to be read */
	{
	/* Read a byte from the FLASH */
		pBuffer = SPI_FLASH_SendByte(DUMMY_BYTE);
		Lcd_write_data(pBuffer);
	}

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();	
	SoftSpi_UnUsePort(SPI_PORT_A);
	#endif
}


#endif
#if(!PIC_FROM_FLASH)
void LcdDispRomBMP(WORD column0, WORD row0,const char *pic)
{
	WORD k,j;
	unsigned char temp;
	unsigned char shift;
	WORD DISP_W = 5;
	WORD DISP_H = 16;
	
	//unsigned char liangdu_L[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};   
	//unsigned char liangdu_H[]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
	unsigned char liangdu=OLEDLum;	
	
	Set_Column_Address(0x1C+column0,0x1C+DISP_W+column0); 
	Set_Row_Address(row0,DISP_H+row0); 	
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);
	//3=24/8
    for(k=0;k<(3*16);k=k+1)
	{
		temp = pic[k];
		
		shift = 0x80;
		for(j=0;j<2;j++)	//8									  
		{
			unsigned char data=0x00,data2=0x00;
			if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
			else data &=liangdu_L[liangdu];
			shift >>= 1;			  
			if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
			else data &=liangdu_H[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
			else data2 &=liangdu_L[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
			else data2 &=liangdu_H[liangdu];	
			shift >>= 1;	  
			writeData_SSD1322(data);
			writeData_SSD1322(data2);
		}   	
	}
}
#endif

void LcdDispLogo_Custom(void)
{
	LcdDispFlashBMP(0,0x3F,0,0x3F,CUSTOM_LOGO);
}

void LcdDispLogo(const char *pic)
{
	WORD k,j;
	unsigned char temp;
	unsigned char shift;
	
	unsigned char liangdu_L[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};   
	unsigned char liangdu_H[]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
	unsigned char liangdu=OLEDLum;	

	if(pic == NULL)
		return ;
	Set_Column_Address(0x1C+0x00,0x1C+0x3F); 
	Set_Row_Address(0x00,0x3F); 	
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);

    for(k=0;k<(128*16);k=k+1)
	{
		temp = pic[k];
		
		shift = 0x80;
		for(j=0;j<2;j++)										  
		{
			unsigned char data=0x00,data2=0x00;
			if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
			else data &=liangdu_L[liangdu];
			shift >>= 1;			  
			if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
			else data &=liangdu_H[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
			else data2 &=liangdu_L[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
			else data2 &=liangdu_H[liangdu];	
			shift >>= 1;	  
			writeData_SSD1322(data);
			writeData_SSD1322(data2);
		}   	
	}
}

void LcdDispLogoFlash(const char *pic)
{
	WORD k,j;
	unsigned char temp;
	unsigned char shift;
	
	unsigned char liangdu_L[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};   
	unsigned char liangdu_H[]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
	unsigned char liangdu=OLEDLum;	

	if(pic == NULL)
		return ;
	Set_Column_Address(0x1C+0x00,0x1C+0x3F); 
	Set_Row_Address(0x00,0x3F); 	
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);

    for(k=0;k<(128*16);k=k+1)
	{
		temp = pic[k];
		
		shift = 0x80;
		for(j=0;j<2;j++)										  
		{
			unsigned char data=0x00,data2=0x00;
			if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
			else data &=liangdu_L[liangdu];
			shift >>= 1;			  
			if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
			else data &=liangdu_H[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
			else data2 &=liangdu_L[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
			else data2 &=liangdu_H[liangdu];	
			shift >>= 1;	  
			writeData_SSD1322(data);
			writeData_SSD1322(data2);
		}   	
	}
}


void LcdDispBMP(WORD column0, WORD row0,PIC_FLASH_INDEX pic)
{
	#if(PIC_FROM_FLASH)
	WORD DISP_W = 5;
	WORD DISP_H = 16;
	LcdDispFlashBMP(column0,DISP_W+column0,row0,DISP_H+row0,pic);
	#else
	const char *p;
	p = (const char *)ICONS_24_16 + (pic - PIC_H_2_1)*48;
	LcdDispRomBMP(column0,row0,p);
	#endif
}
#if 0
static BYTE dispLineEn;
void setDispLineEn(void)
{
	dispLineEn = 1;
}
#endif
void dispMenuPosBar(int current,int max)
{
	char len = 0,start = 0;//,i;
	len = 64/max;//MAX_LINE_NUM
	start = (current-1)*64/max;//MAX_LINE_NUM
	#if 0
	//printf("current: %d,max:%d,len: %d,start: %d\r\n",current,max,len,start);
	LcdFillBlock_clear(0x3F,0x3F,0,64);
	LcdDrawLineW2(0x3F,0x3F,start,start+len);
	//LcdDrawLineW2(0x3F,0x3F,7,7);
	#else
	LcdFillBlock_clear(60, 60, 0, 63);
	LcdFillBlock_clear(62, 63, 0, 63);	

	LcdFillLink(61, 61, 0, 63);
	LcdFillBlock2(0x3E,0x3F,start,start+len);
	#endif

}

BYTE getOLEDLum(void)
{
	return OLEDLum;
}
void setUserOLEDLum(BYTE value)
{
	 OLEDLum = value;
	// printf("OLEDLum = %d\r\n", OLEDLum);
	 if((OLEDLum < 1)||(OLEDLum > 15)) OLEDLum = 12;
}

void LcdDispBMP_1(WORD column0, WORD row0,const char *pic)
{
	WORD k,j;
	unsigned char temp;
	unsigned char shift;
	WORD DISP_W = 5;
	WORD DISP_H = 16;
	unsigned char liangdu=OLEDLum;	
	
	Set_Column_Address(0x1C+column0,0x1C+DISP_W+column0); 
	Set_Row_Address(row0,DISP_H+row0); 	
	writeCmd_SSD1322(SD1322_CMD_WRITE_RAM_COMMAND);
	//3=24/8
    for(k=0;k<(3*16);k=k+1)
	{
		temp = pic[k];
		
		shift = 0x80;
		for(j=0;j<2;j++)	//8									  
		{
			unsigned char data=0x00,data2=0x00;
			if(temp&shift) data |=liangdu_H[liangdu]; // HIGH 4bit
			else data &=liangdu_L[liangdu];
			shift >>= 1;			  
			if(temp&shift) data |=liangdu_L[liangdu]; // low 4bit
			else data &=liangdu_H[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_H[liangdu]; // high 4bit
			else data2 &=liangdu_L[liangdu];
			shift >>= 1;
			if(temp&shift) data2 |=liangdu_L[liangdu]; // low 4bit
			else data2 &=liangdu_H[liangdu];	
			shift >>= 1;	  
			writeData_SSD1322(data);
			writeData_SSD1322(data2);
		}   	
	}
}

/*******************************************************/

void ssd1322Test()
{
    init_OLED_1();
}



