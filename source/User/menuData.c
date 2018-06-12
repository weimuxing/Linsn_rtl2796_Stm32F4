#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "SysConfig.h"
#include "menuData.h"
#include "update_from_fatfs.h"
#include "VideoFormatTable.h"
#include "control.h"
#include "langStr.h"
#include "rs232command.h"
#include "userPref.h"
#include "menu.h"
#include "drvSSD1322.h"
#include "LCD.h"
#include "Input.h"
#include "processKey.h"
#include "ledcontrol.h"
#include "FPGA.h"
#include "tempSensor.h"
#include "RTD2796.h"
#include "SiI9134.h"



#define MENU_DEBUG	1

u8 IpModifyFlah;	//iP设置标志
u8 IpType;

static int valueList[4]; // for test
static char buffer[64];//[40];
unsigned char step = 1;
static char saveTo = 0;
static char usbSaveTo = 0;
static char loadFrom = 0;
static char usbLoadFrom = 0;

u8 sendcard_type = 0;
u8 SendTimeoutFlg =FALSE;



#define MD_OLED_LUM_MIN  1
#define MD_OLED_LUM_MAX  15

extern int dispValue[3];
extern BYTE OLEDLum;
extern u8 sendcardset;
extern const VideoDefine_Struct tVideoDefine[MD_INDEX_SIZE];
extern LAYER_PARAM_Typedef layerParam;
extern LAYER_PARAM_Typedef layerParamBackUp;
extern OUTPUT_TYPE_PARAM_Typedef stOutputTypeParam;

#ifdef USER_SECOND_BOOTLOADER	

#define USER_START_SECTOR 2
#define MAX_USER_SECTOR 27

#define CCLK 60000 					/* 60,000 KHz for IAP call */

#define FLASH_BUF_SIZE 256
#define BOOT_FLASH_START sector_start_map[0]
#define USER_FLASH_START sector_start_map[USER_START_SECTOR]
#define USER_FLASH_END	 sector_end_map[MAX_USER_SECTOR]
#define USER_FLASH_SIZE  ((USER_FLASH_END - USER_FLASH_START) + 1)
#define MAX_FLASH_SECTOR 32

/* Define start address of each Flash sector */
#define SECTOR_0_START      0x00000000
#define SECTOR_1_START      0x00001000
#define SECTOR_2_START      0x00002000
#define SECTOR_3_START      0x00003000
#define SECTOR_4_START      0x00004000
#define SECTOR_5_START      0x00005000
#define SECTOR_6_START      0x00006000
#define SECTOR_7_START      0x00007000
#define SECTOR_8_START      0x00008000
#define SECTOR_9_START      0x00010000
#define SECTOR_10_START     0x00018000
#define SECTOR_11_START     0x00020000
#define SECTOR_12_START     0x00028000
#define SECTOR_13_START     0x00030000
#define SECTOR_14_START     0x00038000
#define SECTOR_15_START     0x00040000
#define SECTOR_16_START     0x00048000
#define SECTOR_17_START     0x00050000
#define SECTOR_18_START     0x00058000
#define SECTOR_19_START     0x00060000
#define SECTOR_20_START     0x00068000
#define SECTOR_21_START     0x00070000
#define SECTOR_22_START     0x00078000
#define SECTOR_23_START     0x00079000
#define SECTOR_24_START     0x0007A000
#define SECTOR_25_START     0x0007B000
#define SECTOR_26_START     0x0007C000
#define SECTOR_27_START     0x0007D000
#define SECTOR_28_START     0xFFFFFFFF
#define SECTOR_29_START     0xFFFFFFFF
#define SECTOR_30_START     0xFFFFFFFF
#define SECTOR_31_START     0xFFFFFFFF

/* Define end address of each Flash sector */
#define SECTOR_0_END        0x00000FFF
#define SECTOR_1_END        0x00001FFF
#define SECTOR_2_END        0x00002FFF
#define SECTOR_3_END        0x00003FFF
#define SECTOR_4_END        0x00004FFF
#define SECTOR_5_END        0x00005FFF
#define SECTOR_6_END        0x00006FFF
#define SECTOR_7_END        0x00007FFF
#define SECTOR_8_END        0x0000FFFF
#define SECTOR_9_END        0x00017FFF
#define SECTOR_10_END       0x0001FFFF
#define SECTOR_11_END       0x00027FFF
#define SECTOR_12_END       0x0002FFFF
#define SECTOR_13_END       0x00037FFF
#define SECTOR_14_END       0x0003FFFF
#define SECTOR_15_END       0x00047FFF
#define SECTOR_16_END       0x0004FFFF
#define SECTOR_17_END       0x00057FFF
#define SECTOR_18_END       0x0005FFFF
#define SECTOR_19_END       0x00067FFF
#define SECTOR_20_END       0x0006FFFF
#define SECTOR_21_END       0x00077FFF
#define SECTOR_22_END       0x00078FFF
#define SECTOR_23_END       0x00079FFF
#define SECTOR_24_END       0x0007AFFF
#define SECTOR_25_END       0x0007BFFF
#define SECTOR_26_END       0x0007CFFF
#define SECTOR_27_END       0x0007DFFF
#define SECTOR_28_END       0xFFFFFFFF
#define SECTOR_29_END       0xFFFFFFFF
#define SECTOR_30_END       0xFFFFFFFF
#define SECTOR_31_END       0xFFFFFFFF
#define HOT_BACKUP_ON		1
 
void write_data(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count);
void find_erase_prepare_sector(unsigned cclk, unsigned flash_address);
void erase_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
void prepare_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
void iap_entry(unsigned param_tab[],unsigned result_tab[]);
void enable_interrupts(unsigned interrupts);
void disable_interrupts(unsigned interrupts);
void watch_dog_reset(void);
void write_usb_fg(void);
void dispFileName(char *source,char *dest,int source_len,int dest_len);

typedef enum
{
	PREPARE_SECTOR_FOR_WRITE = 50,
	COPY_RAM_TO_FLASH = 51,
	ERASE_SECTOR = 52,
	BLANK_CHECK_SECTOR = 53,
	READ_PART_ID = 54,
	READ_BOOT_VER = 55,
	COMPARE = 56,
	REINVOKE_ISP = 57
}IAP_Command_Code;



#define CMD_SUCCESS 0
#define IAP_ADDRESS 0x7FFFFFF1

const unsigned sector_start_map[MAX_FLASH_SECTOR] = 
{
	SECTOR_0_START,             \
	SECTOR_1_START,SECTOR_2_START,SECTOR_3_START,SECTOR_4_START,SECTOR_5_START,      \
	SECTOR_6_START,SECTOR_7_START,SECTOR_8_START,SECTOR_9_START,SECTOR_10_START,     \
	SECTOR_11_START,SECTOR_12_START,SECTOR_13_START,SECTOR_14_START,SECTOR_15_START, \
	SECTOR_16_START,SECTOR_17_START,SECTOR_18_START,SECTOR_19_START,SECTOR_20_START, \
	SECTOR_21_START,SECTOR_22_START,SECTOR_23_START,SECTOR_24_START,SECTOR_25_START, \
	SECTOR_26_START,SECTOR_27_START,SECTOR_28_START,SECTOR_29_START,SECTOR_30_START, \
	SECTOR_31_START
};



const unsigned sector_end_map[MAX_FLASH_SECTOR] = 
{
	SECTOR_0_END,SECTOR_1_END,    \
	SECTOR_2_END,SECTOR_3_END,SECTOR_4_END,SECTOR_5_END,SECTOR_6_END,SECTOR_7_END,   \
	SECTOR_8_END,SECTOR_9_END,SECTOR_10_END,SECTOR_11_END,SECTOR_12_END,             \
	SECTOR_13_END,SECTOR_14_END,SECTOR_15_END,SECTOR_16_END,SECTOR_17_END,           \
	SECTOR_18_END,SECTOR_19_END,SECTOR_20_END,SECTOR_21_END,SECTOR_22_END,           \
	SECTOR_23_END,SECTOR_24_END,SECTOR_25_END,SECTOR_26_END,                         \
	SECTOR_27_END,SECTOR_28_END,SECTOR_29_END,SECTOR_30_END,SECTOR_31_END
};


#define vector_size  64
unsigned param_table[5];
unsigned result_table[5];
unsigned flash_data_table[256]={0};
unsigned vector_buff[vector_size]={0};
unsigned cclk = CCLK;
#endif


static const u8 outFormatList[] =
{

	MD_1024x768_60,
	MD_1024x1280_60,
	MD_1152x1152_60,
	MD_1280x720_60,
	MD_1280x1024_60,
	MD_1280x1280_60,
	MD_1400x1050_60,
	MD_1536x1536_60,
	MD_2560x1600_60,
	MD_1600x1200_60,
	MD_1920x1080_60,
	MD_1920x1200_60,
	MD_2048x1152_60,
	MD_2560x816_60,
	MD_2560x1024_60,
	MD_3840x540_60,
	MD_3840x1080_30,
	MD_1080x1920_60,
};

static const u8 tpStepColorList[] =
{
	1,
	2,
	4,
	7,
};

unsigned char gImage[] = 
{
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XE0,0X00,0X00,0X20,0X00,
    0X00,0X20,0X00,0X00,0X20,0X07,0XFF,0XE0,0X04,0X00,0X00,0X04,0X00,0X00,0X04,0X00,
    0X40,0X07,0XFF,0XE0,0X00,0X00,0X40,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XE0,0X04,0X00,0X00,0X04,
    0X00,0X00,0X04,0X00,0X00,0X07,0XFF,0XE0,0X00,0X00,0X20,0X00,0X00,0X20,0X02,0X00,
    0X20,0X07,0XFF,0XE0,0X02,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X40,0X07,0XFF,0XE0,0X04,
    0X00,0X40,0X04,0X00,0X00,0X04,0X00,0X00,0X07,0XFF,0XE0,0X00,0X00,0X20,0X00,0X00,
    0X20,0X00,0X00,0X20,0X07,0XFF,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0X00,0X00,0X07,0XFF,0XE0,0X02,
    0X00,0X20,0X00,0X00,0X20,0X00,0X00,0X20,0X07,0XFF,0XE0,0X04,0X00,0X00,0X04,0X00,
    0X00,0X04,0X00,0X00,0X07,0XFF,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,

	0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X1F,0XC0,0X04,0X10,0X40,0X04,0X10,0X40,0X04,
    0X10,0X40,0X04,0X10,0X40,0X04,0X10,0X40,0X04,0X10,0X40,0X04,0X10,0X40,0X04,0X10,
    0X40,0X04,0X10,0X40,0X04,0X10,0XE0,0X07,0XF0,0X40,0X00,0X00,0X00,0X00,0X00,0X00,

	0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XF0,0X40,0X04,0X10,0XE0,0X04,0X10,0X40,0X04,
    0X10,0X40,0X04,0X10,0X40,0X04,0X10,0X40,0X04,0X10,0X40,0X04,0X10,0X40,0X04,0X10,
    0X40,0X04,0X10,0X40,0X04,0X10,0X40,0X04,0X1F,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,

	0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XF8,0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X02,
    0X08,0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X02,0X08,
    0X20,0X02,0X08,0X20,0X07,0X08,0X20,0X02,0X0F,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,

	0X00,0X00,0X00,0X00,0X00,0X00,0X02,0X0F,0XE0,0X07,0X08,0X20,0X02,0X08,0X20,0X02,
    0X08,0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X02,0X08,
    0X20,0X02,0X08,0X20,0X02,0X08,0X20,0X03,0XF8,0X20,0X00,0X00,0X00,0X00,0X00,0X00,

};



const unsigned char gImage_1[] = {
	
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X1F,0XFF,0XFF,0XDF,0XFF,
	0XFF,0XDF,0XFF,0XFF,0XDF,0XF8,0X00,0X1F,0XFB,0XFF,0XFF,0XFB,0XFF,0XFF,0XFB,0XFF,
	0XBF,0XF8,0X00,0X1F,0XFF,0XFF,0XBF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,

	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X1F,0XFB,0XFF,0XFF,0XFB,
	0XFF,0XFF,0XFB,0XFF,0XFF,0XF8,0X00,0X1F,0XFF,0XFF,0XDF,0XFF,0XFF,0XDF,0XFD,0XFF,
	0XDF,0XF8,0X00,0X1F,0XFD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,

	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBF,0XF8,0X00,0X1F,0XFB,
	0XFF,0XBF,0XFB,0XFF,0XFF,0XFB,0XFF,0XFF,0XF8,0X00,0X1F,0XFF,0XFF,0XDF,0XFF,0XFF,
	0XDF,0XFF,0XFF,0XDF,0XF8,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,

	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XFF,0XFF,0XF8,0X00,0X1F,0XFD,
	0XFF,0XDF,0XFF,0XFF,0XDF,0XFF,0XFF,0XDF,0XF8,0X00,0X1F,0XFB,0XFF,0XFF,0XFB,0XFF,
	0XFF,0XFB,0XFF,0XFF,0XF8,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,

	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFB,0XE0,0X3F,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,
	0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,
	0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0X1F,0XF8,0X0F,0XBF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,

	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0X0F,0XBF,0XFB,0XEF,0X1F,0XFB,0XEF,0XBF,0XFB,
	0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,
	0XBF,0XFB,0XEF,0XBF,0XFB,0XEF,0XBF,0XFB,0XE0,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,

	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X07,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,
	0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,
	0XDF,0XFD,0XF7,0XDF,0XF8,0XF7,0XDF,0XFD,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,

	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XF0,0X1F,0XF8,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,
	0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,
	0XDF,0XFD,0XF7,0XDF,0XFD,0XF7,0XDF,0XFC,0X07,0XDF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};



void drawLEDConnectIcon(int column, int row,char index, char fb)
{
	if(fb == 1)
	{
		LcdDispBMP_1(column,row,(const char*)&gImage_1[index*48]);  
	}
	else
	{
		LcdDispBMP_1(column,row,(const char*)&gImage[index*48]);  
	}
}



void displayLEDConnectIcon(int focus)
{
	BYTE i;
	
	for(i = 0;i < 4;i++)
	{
		LcdDispBMP_1(16 + i*8,32, (const char*)&gImage[i*48]);
		LcdDispBMP_1(16 + i*8,48, (const char*)&gImage[(i+4)*48]); 
	}
	
	if(focus < 4)
	{
		LcdDispBMP_1(16 + (focus) * 8,32, (const char*)&gImage_1[focus * 48]); 
	}
	else
	{
		LcdDispBMP_1(16 +(focus - 4) * 8,48, (const char*)&gImage_1[focus * 48]);
	}
}




const char * inputSourceStrList[] =
{	
	
	"DP",				//	VIPDRIVE_INPUT_DP, 							
	"HDMI1.4",			//	VIPDRIVE_INPUT_HDMI,	
	"HDMI2.0",			//	VIPDRIVE_INPUT_HDMI,	
	"DVI",				//	VIPDRIVE_INPUT_DVI,	
	//"M1",				//	VIPDRIVE_INPUT_M1,						
	//"M2",				//	VIPDRIVE_INPUT_M2,			
	//"M3",				//	VIPDRIVE_INPUT_M3,				
	//"M4",				//	VIPDRIVE_INPUT_M4,			
};


const char * inputStrList[] =
{	
	"DP",				//	VIPDRIVE_INPUT_DP, 							
	"HDMI1.4",			//	VIPDRIVE_INPUT_HDMI,	
	"HDMI2.0",			//	VIPDRIVE_INPUT_HDMI,	
	"DVI",				//	VIPDRIVE_INPUT_DVI,	
	//"M1",				//	VIPDRIVE_INPUT_M1,						
	//"M2",				//	VIPDRIVE_INPUT_M2,			
	//"M3",				//	VIPDRIVE_INPUT_M3,				
	//"M4",				//	VIPDRIVE_INPUT_M4,					
};




const char * edidSourceFileStrList[] = 
{
	"YDSX DVI",   		//公司DVI EDID
	"YDSX HDMI",     	//公司HDMI EDID
	"YDRX VGA",   		//公司VGA EDID
	"YDRX DP",			//公司DP EDID
	"PREVIEW DVI",	    //输出1	
	"OUT_DVI2",	    	//  输出2
	"OUT_VGA", 			//	输出VGA
	"FOLLOW",       	//EDID跟随
	"CUSTOM",			//custom edid
};




const char * const gammaListStr[] =
{
	"0",
	"1.8",
	"2.0",
	"2.2",
	"2.4",
};




const char * const extInputConfigListStr[] =
{
	"AUDIO",
	"HDMI",
};



const char * const sendcardgammaListStr[51] =
{
	"1.0",
	"1.1",
	"1.2",
	"1.3",
	"1.4",
	"1.5",
	"1.6",
	"1.7",
	"1.8",
	"1.9",
	"2.0",
	"2.1",
	"2.2",
	"2.3",
	"2.4",
	"2.5",
	"2.6",
	"2.7",
	"2.8",
	"2.9",
	"3.0",
	"3.1",
	"3.2",
	"3.3",
	"3.4",
	"3.5",
	"3.6",
	"3.7",
	"3.8",
	"3.9",
	"4.0",
	"4.1",
	"4.2",
	"4.3",
	"4.4",
	"4.5",
	"4.6",
	"4.7",
	"4.8",
	"4.9",
	"5.0",
	"5.1",
	"5.2",
	"5.3",
	"5.4",
	"5.5",
	"5.6",
	"5.7",
	"5.8",
	"5.9",
	"6.0",
};




const char const *sendcard12chooseStr_en[2] =
{
	" NO.1",
	" NO.2",
};



const char const *sendcard12chooseStr_ch[2] =
{
	" 1号卡",
	" 2号卡",
};



const char const *outputTypeStr[2] =
{
	"DVI",
	"HDMI",
};



const char const *bitWidthStr[3] =
{
	"8 bit",
	"10 bit",
	"12 bit",
};



const char const *outputRangeStr[2] =
{
	"RGB",
	"YUV",
};
  
const char const *coloreffectStr[][2] =
{
	{"Standard","标准"},
	{"Game",	"游戏"},
	{"Movie",	"电影"},
	{"Photo",	"图片"},
	{"Vivid",	"艳丽"},
};

const char const *colorTemperStr[] =
{
	{"CT_9300"},
	{"CT_7500"},
	{"CT_6500"},
	{"CT_5800"},
	{"CT_SRGB"},
	{"Default"},
};



const char hotBackupListEnable_reset[] = 
{
	1,    //VIPDRIVE_INPUT_VGA,				
	1,    //VIPDRIVE_INPUT_DP,				
	1,    //VIPDRIVE_INPUT_DVI,	
	1,    //VIPDRIVE_INPUT_HDMI,	
	//1,	  //VIPDRIVE_INPUT_SDI, 
	//1,    //VIPDRIVE_INPUT_EXT_7,		
	//1,    //VIPDRIVE_INPUT_EXT_8,			
	//1,    //VIPDRIVE_INPUT_EXT_9,			
};
static char hotBackupListEnable[] = 
{
	1,    //VIPDRIVE_INPUT_VGA,	
	1,    //VIPDRIVE_INPUT_DP,					 
	1,    //VIPDRIVE_INPUT_DVI,	
	1,    //VIPDRIVE_INPUT_HDMI,	
	//1,    //VIPDRIVE_INPUT_SDI,	
	//1,    //VIPDRIVE_INPUT_EXT_7,				
	//1,    //VIPDRIVE_INPUT_EXT_8,			
	//1,    //VIPDRIVE_INPUT_EXT_9,				
};
const VipDrive_Source_TypeDef hotBackupList_1u_new[] = 
{
	VIPDRIVE_INPUT_DP,					 // 1
	VIPDRIVE_INPUT_HDMI,				// 3	
	VIPDRIVE_INPUT_HDMI2,				// 3	
	VIPDRIVE_INPUT_DVI,					 // 2
	//VIPDRIVE_INPUT_M1,					// 4
	//VIPDRIVE_INPUT_M2,				// 5
	//VIPDRIVE_INPUT_M3,				// 6
	//VIPDRIVE_INPUT_M4,				// 7	
};


const VipDrive_Source_TypeDef hotBackupList_VipDrive[] = 
{
	VIPDRIVE_INPUT_DP,					 // 1
	VIPDRIVE_INPUT_HDMI,				// 3	
	VIPDRIVE_INPUT_HDMI2,				// 3	
	VIPDRIVE_INPUT_DVI,					 // 2
	//VIPDRIVE_INPUT_M1,					// 4
	//VIPDRIVE_INPUT_M2,				// 5
	//VIPDRIVE_INPUT_M3,				// 6
	//VIPDRIVE_INPUT_M4,				// 7		
};

void delayms(int ms)
{
	int i; 
	while(ms>=0)
		{
		ms--;
	for(i = 0;i < 1000; i++)
		;
		}
}

void refleshHotBackupEnable(void)
{
	int i;//,index;
	//index = 0;
	for(i = 0; i < sizeof(hotBackupListEnable);i++)
	{
		int j;
		{
			hotBackupListEnable[i] = hotBackupListEnable_reset[i];
		}
		for(j = 0; j < 3; j++)
		{
			if(((hotBackupList_VipDrive[i] == getHotBackupDetials(j))&&(getHotBackupDetials(j + 1) != getHotBackupDetials(j)))
			  ||(hotBackupList_VipDrive[i] == getHotBackupDetials(0)))
			{
				hotBackupListEnable[i] = 0;
				break;
			}
		}
	}
}

int refleshHotBackupList(VipDrive_Source_TypeDef *list)  //返回可用信号个数
{
	int i,index;
	index = 0;
	for(i = 0; i < sizeof(hotBackupListEnable);i++)
	{
		if(hotBackupListEnable[i] == 1)
		{
			*(list + index) = hotBackupList_VipDrive[i];
			index++;
		}
	}
	return index;
}


void resetHotBackupEnable(void)
{
	int i;
	for(i = 0; i < sizeof(hotBackupListEnable);i++)
	{
		#if 0
		hotBackupListEnable[i] = 1;
		#else
		hotBackupListEnable[i] = hotBackupListEnable_reset[i];
		#endif
	}
}


void setHotBackupDisable(int source)
{
	int i;
	for(i = 0; i < sizeof(hotBackupListEnable);i++)
	{
		if(hotBackupList_VipDrive[i] == source)
		{
			hotBackupListEnable[i] = 0;
			break;
		}
	}
}

int getNextBackupCh(int cnt,int source)
{
	int index,len;
	VipDrive_Source_TypeDef enableList[10];
	if(cnt == 0)
	{
		resetHotBackupEnable();
	}
	len = refleshHotBackupList(enableList);
	
	printf("refleshHotBackupList()_len: %d	   :\r\n",len);
	for(index = 0; index < len; index++)
	{
		printf("  %s  ",valueToString(inputSourceValue,enableList[index]));
	}
	
	for(index = 0; index < len; index++)
	{
		if(enableList[index] == source)
		{
			break;
		}
	}
	//printf("index = %d\r\n",index);
	index++;
	if(index >= len)
	{
		index = 0;
	}
	return enableList[index];
	
}

int getPreBackupCh(int cnt,int source)
{
	int index,len;
	VipDrive_Source_TypeDef enableList[9];
	if(cnt == 0)
	{
		resetHotBackupEnable();
	}
	len = refleshHotBackupList(enableList);
	printf("refleshHotBackupList()_len: %d     :\r\n",len);
	for(index = 0; index < len; index++)
	{
		printf("  %s  ",valueToString(inputSourceValue,enableList[index]));
	}
	for(index = 0; index < len; index++)
	{
		if(enableList[index] == source)
		{
			break;
		}
	}
	//printf("index = %d\r\n",index);
	index--;
	if(index < 0)
	{
		index = len - 1;
	}
	return enableList[index];
	
}


//edid control
typedef struct
{
	char len;
	char edidDetinationPort[6];
}EDID_DETINALTION_PORT_TYPESEF;
static EDID_DETINALTION_PORT_TYPESEF EDIDDetinationPort = 
{
	3,
	{EDID_DES_PORT_DVI,EDID_DES_PORT_HDMI,EDID_DES_PORT_HDMI2}
};

const EDID_DesPort_Typedef edidDetinationPortList_1u[] = 
{
	EDID_DES_PORT_DVI, //= 1,
	EDID_DES_PORT_HDMI,  //= 2,
};
static char edidDetinationPortListEnable_1u[] = 
{
	0,//VIPDRIVE_INPUT_VGA,				
	1,//VIPDRIVE_INPUT_DVI,				
	1,//VIPDRIVE_INPUT_HDMI,				
	0,//VIPDRIVE_INPUT_EXT_7,			
	0,//VIPDRIVE_INPUT_EXT_8,			
	0,//VIPDRIVE_INPUT_EXT_9,				
};

void checkEDIDDetinationList(void)
{
	char i,index = 0;
	switch(getFrontType())
	{	
		case 0x00:
			edidDetinationPortListEnable_1u[EDID_DES_PORT_HDMI2]  = 0;
			edidDetinationPortListEnable_1u[EDID_DES_PORT_DVI]  = 1;
			edidDetinationPortListEnable_1u[EDID_DES_PORT_HDMI] = 0;
			break;
		case 0x01:
			edidDetinationPortListEnable_1u[EDID_DES_PORT_HDMI2]	= 1;
			edidDetinationPortListEnable_1u[EDID_DES_PORT_DVI]	= 1;
			edidDetinationPortListEnable_1u[EDID_DES_PORT_HDMI] = 0;
			break;
		case 0x02:
			edidDetinationPortListEnable_1u[EDID_DES_PORT_HDMI2]	= 1;
			edidDetinationPortListEnable_1u[EDID_DES_PORT_DVI]	= 1;
			edidDetinationPortListEnable_1u[EDID_DES_PORT_HDMI] = 1;
			break;
	}
	for(i = EDID_DES_PORT_DVI; i < sizeof(edidDetinationPortListEnable_1u); i++)
	{
		edidDetinationPortListEnable_1u[i] = 1;
	}
	for(i = 0; i < sizeof(edidDetinationPortListEnable_1u);i++)
	{
		if(edidDetinationPortListEnable_1u[i] == 1)
		{
			EDIDDetinationPort.edidDetinationPort[index] = edidDetinationPortList_1u[i];
			index++;
			if(index == 1)
			{
//				GetEDIDManageSetting()->desPort = edidDetinationPortList_1u[i];
			}
		}
	}
	EDIDDetinationPort.len = index;

}

char getNextEDIDDetinationPort(char port)
{
	char index;
	for(index = 0; index < EDIDDetinationPort.len; index++)
	{
		if(EDIDDetinationPort.edidDetinationPort[index] == port)
		{
			break;
		}
	}
	//printf("current detination port = %d,",port);
	index++;
	if(index >= EDIDDetinationPort.len)
	{
		index = EDIDDetinationPort.len - 1;
	}
	//printf("next port = %d",EDIDDetinationPort.edidDetinationPort[index]);
	return EDIDDetinationPort.edidDetinationPort[index];
}
char getPreEDIDDetinationPort(char port)
{
	int index;
	for(index = 0; index < EDIDDetinationPort.len; index++)
	{
		if(EDIDDetinationPort.edidDetinationPort[index] == port)
		{
			break;
		}
	}
	//printf("current detination port = %d,",port);
	index--;
	if(index < 0)
	{
		index = 0;//edidSourcePort.len - 1;
	}
	//printf("pre port = %d",EDIDDetinationPort.edidDetinationPort[index]);
	return EDIDDetinationPort.edidDetinationPort[index];
}

#if 1           //只针对外扩主板V1.3及以上
const char * InputAudioTypeStr[] = 
{
	"AUDIO_1",
	"AUDIO_2",
	"AUDIO_3",
	"AUDIO_MAX_SIZE",
};

const char * InputSourceList_1u_newStr[] = 
{
	"CV",					 // 0
	"VGA",					 // 3
	"DP",					     // 5
	"DVI",					 // 6
	"EXT_7",					 // 7
	"EXT_8",					 // 8
	"EXT_9",					 // 9
};

const VipDrive_Source_TypeDef InputSourceList_1u_new[] = 
{				
	VIPDRIVE_INPUT_DVI,					 // 2
	VIPDRIVE_INPUT_DP,					 // 1
	VIPDRIVE_INPUT_HDMI,				// 3	
	VIPDRIVE_INPUT_HDMI2,				// 3	
	VIPDRIVE_INPUT_M1,					// 4
	VIPDRIVE_INPUT_M2,				// 5
	VIPDRIVE_INPUT_M3,				// 6
	VIPDRIVE_INPUT_M4,				// 7				
};

static INPUT_AUDIO_TYPE AudioInputConfig_1u[] =         //只针对V1.3版本的外扩主板和对应音频板
{	
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_CV1,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_CV3,					
	INPUT_AUDIO_3,          //	VIPDRIVE_INPUT_VGA,					
	INPUT_AUDIO_1,          //	VIPDRIVE_INPUT_DP, 				
	INPUT_AUDIO_2,          //	VIPDRIVE_INPUT_DVI,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_EXT_7,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_EXT_8,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_EXT_9,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_SIZE,
};
static INPUT_AUDIO_TYPE AudioInputListEnable_1u[] =         //只针对V1.3版本的外扩主板和对应音频板
{	
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_CV1,				
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_VGA,				
	INPUT_AUDIO_MAX_SIZE,   //  VIPDRIVE_INPUT_DP, 				
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_DVI,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_EXT_7,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_EXT_8,					
	INPUT_AUDIO_MAX_SIZE,   //	VIPDRIVE_INPUT_EXT_9,					
};

INPUT_AUDIO_TYPE getAudioInputConfig(char source)
{
	printf("get %s__%s\r\n",inputSourceStrList[source],InputAudioTypeStr[AudioInputConfig_1u[source]]);
	return AudioInputConfig_1u[source];
}


void refleshAudioInputEnable(char index)
{
	int i;
	for(i = 0; i < sizeof(AudioInputListEnable_1u);i++)
	{
        AudioInputListEnable_1u[i] = INPUT_AUDIO_MAX_SIZE;
	}
}

void refleshAudioInputEnable_total(void)
{
	int i;
	for(i = 0; i < sizeof(AudioInputListEnable_1u);i++)
	{
		AudioInputListEnable_1u[i] = INPUT_AUDIO_MAX_SIZE;
		AudioInputConfig_1u[InputSourceList_1u_new[i]] = INPUT_AUDIO_MAX_SIZE;
	}
	#if(DISP_TEST_INFO)
	//printf("AudioInputDetail:%d,%d,%d,",getAudioInputDetails(0),getAudioInputDetails(1),getAudioInputDetails(2));
	printf("refleshAudioInputEnable_total():%d,%d,%d,%d,%d,%d,%d\r\n",
		AudioInputListEnable_1u[0],AudioInputListEnable_1u[1],AudioInputListEnable_1u[2],AudioInputListEnable_1u[3],
		AudioInputListEnable_1u[4],AudioInputListEnable_1u[5],AudioInputListEnable_1u[6]);
	printf("AudioInputConfig_1u:%s_%s,%s_%s,%s_%s,%s_%s,%s_%s,%s_%s,%s_%s\r\n",
		InputSourceList_1u_newStr[0],InputAudioTypeStr[AudioInputConfig_1u[InputSourceList_1u_new[0]]],
		InputSourceList_1u_newStr[1],InputAudioTypeStr[AudioInputConfig_1u[InputSourceList_1u_new[1]]],
		InputSourceList_1u_newStr[2],InputAudioTypeStr[AudioInputConfig_1u[InputSourceList_1u_new[2]]],
		InputSourceList_1u_newStr[3],InputAudioTypeStr[AudioInputConfig_1u[InputSourceList_1u_new[3]]],
		InputSourceList_1u_newStr[4],InputAudioTypeStr[AudioInputConfig_1u[InputSourceList_1u_new[4]]],
		InputSourceList_1u_newStr[5],InputAudioTypeStr[AudioInputConfig_1u[InputSourceList_1u_new[5]]],
		InputSourceList_1u_newStr[6],InputAudioTypeStr[AudioInputConfig_1u[InputSourceList_1u_new[6]]]);
	#endif
}

int getNextAudioInputCh(int cnt,int source)
{

	return source;
	
}

int getPreAudioInputCh(int cnt,int source)
{

	return source;
	
}
#endif

const char * lxyOutputFormatStr_List[]=
{
	"640x480@60", 	// 0
	"800x600@60", 	// 1
	"1024x768@60",	// 2 
	"1280x768@60",	// 3 
	"1280x800@60",	// 4 
	"1280x1024@60",	// 5
	"1366x768@60",	// 6
	"1440x900@60",	// 7 
	"1440x1080@60",	// 8
	"1600x576@60",	// 9
	"1600x900@60",	// 10
	"1600x1000@60",	// 11
	"1600x1200@60",	// 12
	"1680x1050@60",	// 13 
	"1728x1296@60",	// 14
	"1920x1080@60", //15
	"1920x1200@60",	// 16
	"2048x1152@60",	// 17
	"2048x1280@60",	// 18
	"2560x1440@60",	// 19
	"2560x1600@60",	// 20
	
};

const char * colorlightOutputFormatStr_List[]=
{
	"1024x768@60",
	"1280x720@60",
	"1280x1024@60",
	"1600x768@60",
	"1920x640@60",
	"1920x1080@60",
};

const char * inputSourceStr_List[] =
{
	"DP",
	"HDMI1.4",
	"HDMI2.0",
	"DVI",
#ifdef _VGA_INPUT_ENABLE
	"VGA",
#endif
};

const char * inputSourceStr_List1[] =
{
	"DP ",
	"HD1",
	"HD2",
	"DVI",
	"VGA",
};

const char * outputPortStr_List[]=
{
	"DVI1",
	"DVI2",
	"DVI3",
	"DVI4",
};

#ifdef USER_SECOND_BOOTLOADER	
/*************************************************
  Function:void write_usb_fg(void)
  Description:write enter boot flag to the flash
写进入BOOTLOATER程序标志到FLASH
*************************************************/

void write_usb_fg(void)
{
}
#endif

static u8 nextItem(u8 currentValue, const u8 *list, int size)
{
    int i;
    for(i=0; i<size; ++i)
    {
        if(currentValue == list[i])
        {
            break;
        }
    }
    ++i;
    if(i<size)
    {
        return list[i];
    }
    else
    {
        return list[size-1];
    }
}
static u8 prevItem(u8 currentValue, const u8 *list, int size)
{
    int i;
    for(i=0; i<size; ++i)
    {
        if(currentValue == list[i])
        {
            break;
        }
    }
    --i;
    if(i >= 0 )
    {
        return list[i];
    }
    else
    {
        return list[0];
    }
}

void clearStep(void)
{
	step = 0;
}

const char * dispInputFormatByCh(int ch,int format)
{
	if(format == MD_UNDEFINE) 
	{
		if((getValue(inputHActiveValue) < getValue(inputHTotalValue))&&(getValue(inputVActiveValue) < getValue(inputVTotalValue)))
		{
			sprintf(buffer,"C: %dx%dx%d",getValue(inputHActiveValue),getValue(inputVActiveValue),getValue(inputFreqValue));
		}
		else
		{
			return "UNDEFINE";
		}
	}
	else
	{
		return GetStrByVideoFormat(format);
	}
	return buffer;
}

extern char USB_FileNames[10][128]; //最多一次存10个文件名    getUSBControlSetting()->fileName里面只存现在正播放的文件名

const char * valueToString(int index, int value)
{
//	BYTE curport =0;
//	BYTE i;
//	char buffer2[40];

	//curport = getsendcard()->lxynetwork;	//20150330
	//sprintf(buffer, "%s", "INVALID");
    switch(index)
    {
    case naValue:
		return langGetStr(naStr);
    case dvi1OutputValue:
		return langGetStr(outputNormalStr+value);
	case dvi1InputValue:
		return langGetStr(dviStr+value);
    case autoTpIimeValue:
		if(value == 0)
		{
			return langGetStr(offStr);
		}
		else
		{
			sprintf(buffer, "%ds", value);
		}
		break;
    case tpStepColorValue:
		if(value == 1)
		{
			return langGetStr(redStr);
		}
		else if(value == 2)
		{
			return langGetStr(greenStr);
		}
		else if(value == 4)
		{
			return langGetStr(blueStr);
		}
		else if(value == 7)
		{
			return langGetStr(whiteStr);
		}
			
		break;
    case licensePasswordValue:
		sprintf(buffer, "%06x", value);
		break;
	case lockPanelPasswordValue:
		sprintf(buffer, "%04x", value);
		break;		
	case licenseStateValue:
	case keyLockValue:
		return langGetStr(offStr+value);
	case temperatureModeValue:
		return langGetStr(celsiusStr + value);
	case temperatureValue:
		break;
	case licenseTimeValue:

		break;
    case canUseTimeValue:

		break;
    case delayCallSaveValue:
		sprintf(buffer, "%ds", value);
		break;
    case workTimeValue:
		{
			u32 temp = value;
			temp = temp/60;  // minute
			//printf("work time: %d,temp: %d,day:%d,hour:%d,min:%d\r\n",value,temp,(temp/60)/24, (temp/60)%24,  temp%60);
			if(GetLanguage() == LANG_ENGLISH)
			{
				sprintf(buffer, "%dd :%02dh :%02dm", temp/60/24, temp/60%24,  temp%60);
			}
			else
			{
				sprintf(buffer, "%4d天%02d小时%02d分", temp/60/24, temp/60%24,  temp%60);
			}
    	}
		break;
	case totalWorkTimeValue:
	case lastWorkTimeValue:
		{
			u32 temp = value;
			//printf("total time: %d,day:%d,hour:%d,min:%d\r\n",value,(temp/60)/24, (temp/60)%24,  temp%60);
			if(GetLanguage() == LANG_ENGLISH)
			{
				sprintf(buffer, "%dd :%02dh :%02dm", (temp/60)/24, (temp/60)%24,  temp%60);
			}
			else
			{
				sprintf(buffer, "%4d天%02d小时%02d分", (temp/60)/24, (temp/60)%24,  temp%60);
			}
    	}
		break;
    case dateValue:
//		{
//			Time_TypeDef tempTime;
//			if(!GetTime_PCF8563(&tempTime))
//			{
//				init_PCF8563();
//				GetTime_PCF8563(&tempTime);				
//			}
//			sprintf(buffer, "%4x-%02x-%02x", tempTime.year,tempTime.month, tempTime.day);
//    	}
		break;
	case timeValue:
//		{
//			Time_TypeDef tempTime;
//			if(!GetTime_PCF8563(&tempTime))
//			{
//				init_PCF8563();
//				GetTime_PCF8563(&tempTime);				
//			}
//			sprintf(buffer, "%02x:%02x:%02x", tempTime.hour,tempTime.minute, tempTime.second);
//    	}
		break;
    case ipAddress3Value:
		{
		#ifdef _useArtNet
			u8 IpAdr[4];
			GetLocalIpAddress(IpAdr);
			sprintf(buffer,"%d.%d.%d.%d", IpAdr[0], IpAdr[1], IpAdr[2],value);
		#endif
    	}
		break;

	case RemoteIpAddressValue:
	case LocalIpAddressValue:
	case subnetMaskValue:
	case gatewayAddressValue:
		{
			u8 IpAdr[4];
			IpAdr[0] = value >> 24;
			IpAdr[1] = value >> 16;
			IpAdr[2] = value >> 8;
			IpAdr[3] = value;
			sprintf(buffer,"%03d.%03d.%03d.%03d", IpAdr[0], IpAdr[1], IpAdr[2],IpAdr[3]);
    	}
		break;
    case snValue:
		sprintf(buffer, "%04d", value);
		break;
	case fpgaVersionValue:
		if((value == 0)||(value == 0xff))
		{
			return langGetStr(noExtTipStr);
		}
		else
		{
			sprintf(buffer, "%d.%02d", value>>4, (value&0xf));
		}
	break;
	case mcuVersionValue:
	case videoVersionValue:
	case ext1MainBoardSoftVersionValue:
	case ext2MainBoardSoftVersionValue:
	case mainBoardVersionValue:
		if((value == 0)||(value == 0xff))
		{
			return langGetStr(noExtTipStr);
		}
		else
		{
			sprintf(buffer, "%d.%02d", value>>8, (value&0xff));
		}
		break;
	case mainBoardHardVersionValue:
	case ext1BoardHardVersionValue:
	case ext2BoardHardVersionValue:
	case extInputHardVersionValue:
	case commBoardHardVersionValue:
		if(value == 0xff)
		{
			return langGetStr(noExtTipStr);
		}
		else
		{
			sprintf(buffer, "1.%02d", value);
		}
		break;
		
    case displayModeValue:
        return langGetStr(ratio_4_3_str+value);
    case windowRatioValue:
		//return langGetStr(ratio_4_3_str+value);
        if(value == 0)
            return langGetStr(offStr);
        return langGetStr(onStr);
    case languageValue:
		return langGetStr(englishStr+value);
	case languageLongValue:
		if(value == 0)
		{
			return langGetStr(englishLongStr);
		}
		else 
		{
			return langGetStr(chineseStr);
		}
		
    case freezeValue:
		return langGetStr(liveStr+value);
    case outputFormatValue:
		if(value == MD_CUSTOM)
		{
			if(lang_GetLang() == LANG_ENGLISH)
			{
				sprintf(buffer,"CUSTOM                      >>");
			}
			else
			{
				sprintf(buffer,"自定义                      >>");
			}
			break;
		}
		else
		{
	        return GetStrByVideoFormat(value);
		}
	case AnotherFormatValue:
		if(value == MD_UNDEFINE)// || value == MD_CUSTOM) 
		{
			if((getValue(inputHActiveValue) < getValue(inputHTotalValue))&&(getValue(inputVActiveValue) < getValue(inputVTotalValue)))
			{
				sprintf(buffer,"C:%dx%dx%d",getValue(inputHActiveValue),getValue(inputVActiveValue),getValue(inputFreqValue)/10);
				break;
			}
			else
			{
				return "UNDEFINE";
			}
		}
		else
		{
	        return GetStrByVideoFormat(value);
		}
    case inputFormatValue:
		//printf("disp inputFormat = %d\r\n",value);
		if(value == MD_UNDEFINE)// || value == MD_CUSTOM) 
		{
			if((getValue(inputHActiveValue) < getValue(inputHTotalValue))&&(getValue(inputVActiveValue) < getValue(inputVTotalValue)))
			{
				sprintf(buffer,"C:%dx%dx%d",getValue(inputHActiveValue),getValue(inputVActiveValue),getValue(inputFreqValue)/10);
				break;
			}
			else
			{
				return "UNDEFINE";
			}
		}
		else
		{
	        return GetStrByVideoFormat(value);
		}
	case standardFormatValue:
		if(value == MD_CUSTOM) 
		{
			sprintf(buffer,"C: %dx%dx%d",getValue(customHValue),getValue(customVValue),getValue(customFValue));
			break;
		}
		else
		{
			return GetStrByVideoFormat(value);
		}
	case LEDoutputFormatValue:		

	case inputSourceValue:
        if((value !=  VIPDRIVE_INPUT_DVI) && (value !=  VIPDRIVE_INPUT_HDMI) && (value !=  VIPDRIVE_INPUT_DP))
        {
            value = VIPDRIVE_INPUT_DVI;
        }
		{
			return inputSourceStrList[value];
		}
		//break;
	case autoFitValue:
		return langGetStr(noStr + value);

	case scaleModeValue:
		sprintf(buffer, "Global Scale");
		break;
	case screenModeValue:
		#if 1
		return langGetStr(value + FullSizeStr);
		#else
		{
			char buffer_1[30];
			sprintf(buffer_1, "%s(%d,%d,%d,%d)", langGetStr(value + FullSizeStr), getValue(scalePosxValue), getValue(scalePosyValue), getValue(scaleWidthValue), getValue(scaleHeightValue));
			sprintf(buffer, "%22s", buffer_1);
		}
		break;
		#endif
	case dviSelectValue:
		sprintf(buffer, "DVI%d", value?1:2);
		break;
	case vgaSelectValue:
		sprintf(buffer, "VGA%d", value?1:2);
		break;
	case deOnOffValue:
	case muteOnOffValue:
	case networkEnableValue:
	case dhcpEnableValue:
	case timingControlValue:
	case textEnableValue:
	case interlaceSwitchValue:
	case imageEnhanceValue:
	case autoSpeedValue:
	case realTimeSaveValue:
	case screenFollowValue:
	case hdmiColorSpaceAdjustValue:
    case tpSwitchValue:    
		return langGetStr(offStr+value);
	case dviHdmiValue:
		return langGetStr(dviStr+value);
	case dataRangeValue:
		return langGetStr(imageStr+value);
	case gammaValue:
		if((0 <= value) && (value < (sizeof(gammaListStr)/sizeof(gammaListStr[0]))))
		{
			return gammaListStr[value];
		}
	break;
	case colorEffectValue:
		if(value > 4)
			value = 4;
		sprintf(buffer, "%s", coloreffectStr[value][GetLanguage()]);
	break;
	case vga1KeyModeValue:
	case vga2KeyModeValue:
		return langGetStr(vgaStr+value);
	case blackKeyTypeValue:
		return langGetStr(blackDisplayModeStr + value);
	case scaleKeyTypeValue:
		if(value == 0)
		{
			return langGetStr(scalePauseStr);
		}
		else if(value == 1)
		{
			return langGetStr(blackZeroStr);
		}
		else  if(value == 2)
		{
			return langGetStr(freezeDisplayModeStr);
		}

		return langGetStr(logoNaModeStr);
        
	case mapKeyTypeValue:
		if(value == 0)
		{
			return langGetStr(mapKeyStr);
		}
		else if(value == 1)
		{
			return langGetStr(blackZeroStr);
		}
		else if(value == 2)
		{
			return langGetStr(freezeDisplayModeStr);
		}

		return langGetStr(logoNaModeStr);
	
	case switchModeValue:
//        if(GetUserSetting()->imageEnhance == 1)
//        {
//            sprintf(buffer, "Disable");
//        }
//        else
//            return langGetStr(fadeStr+value);
        break;
	case fadeTimeValue:       
		    sprintf(buffer, "%d.%ds", GetSystemParam()->SysFadeTime/10, GetSystemParam()->SysFadeTime%10);
	break;
	case switchTypeValue:
		return langGetStr(simpleVideoStr+value);

	case previewDblTapSwitchValue:
		return langGetStr(offStr + value);
	case previewSwitchValue:
		if(value == 0)
		{
			return langGetStr(preStr);
		}
		else if(value == 1)
		{
			return langGetStr(postStr);
		}
		break;
    case previewDVISwitchValue:
 		if(value == 0)
		{
			return langGetStr(previewStr1);
		}
		else if(value == 1)
		{
			return langGetStr(programStr1);
		}
		break;       

	case saveToValue:
	case loadFromValue:
		sprintf(buffer, "Save%d", value+1);
        break;
	case usbSingleLoadValue:
		sprintf(buffer, "Save%d", value+1);
        break;		
	case usbAllLoadValue:
	case usbAllSaveValue:
		sprintf(buffer, "All");
		break;
//	case pipModeValue:
	case splitOnOffValue:
		return langGetStr(offStr+value);
	case swapMainPipValue:
		return langGetStr(offStr+value);	
	case layoutValue:
		if(value > (LAYOUT_SIZE-1))
		{
			value = (LAYOUT_SIZE-1);
		}        
		return langGetStr(pipLTStr+value);		
	case currentWindowValue:
		return langGetStr(imageAStr+value);
	case audioModeValue:
		return langGetStr(imageAStr+value);
	case hdmiAudioValue:
	case dviAndExtAudioValue:
	case audioTypeValue:
	case hdmiAndExtAudioValue:
		return langGetStr(internalStr+value);
	case audioInput1Value:
	case audioInput2Value:
	case audioInput3Value:		
		return inputSourceStrList[value];
	case lockModeValue:
		return langGetStr(freeRunStr+value);
	case lockSourceValue:
		return langGetStr(imageAStr+value);
	case textAboveValue:
		return langGetStr(belowStr+value);
	case textAndOrValue:
		return langGetStr(orStr+value);
	case textModeValue:
	case splitSyncModeValue:
		sprintf(buffer,"%s%d ", langGetStr(modeStr), value+1);
		break;
	case textPresetValue:
		return langGetStr(userStr);//langGetStr(userStr+value);
	case outputColorSpaceValue:
		return langGetStr(rgb444Str+value);
	case customFormatValue:
	case customEDIDValue:
		{
			int Custom[3];
			Custom[0] = getDispCustomValue(0);//getValue(customHValue);
			Custom[1] = getDispCustomValue(1);//getValue(customVValue);
			Custom[2] = getDispCustomValue(2);//getValue(customFValue);
			if(Custom[0] == 0)
			{
				sprintf(buffer,"            ");
			}
			else
			{
				if(Custom[1] == 0)
				{
					sprintf(buffer,"%dx", Custom[0]);
				}
				else 
				{
					if(Custom[2] == 0)
					{
						sprintf(buffer,"%dx%d@", Custom[0], Custom[1]);
					}
					else
					{
						sprintf(buffer,"%dx%d@%d", Custom[0], Custom[1], Custom[2]);
					}
				}
			}
    	}
		break;

	case zoomWidthValue:
	case zoomHeightValue:
	case zoomPosxValue:
	case zoomPosyValue:
        sprintf(buffer, "%3d", value);
		buffer[3] = '%';
		buffer[4] = 0;
        break;		
		
	case zoomUpValue:
	case zoomDownValue:
	case zoomUpDownValue:
	case zoomLeftValue:
	case zoomRightValue:
	case zoomLeftRightValue:
	case zoomCenterValue:
		//printf("read value: %d,",value);
		value = 10000/(100-value);
		//printf("disp value: %d\r\n",value);
        sprintf(buffer, "%4d", value);
		buffer[4] = '%';
		buffer[5] = 0;
		break;

	case cropUpValue:
	case cropDownValue:
	case cropUpDownValue:
	case cropLeftValue:
	case cropRightValue:
	case cropLeftRightValue:
	case cropCenterValue:
		
	case cropInputWidthValue:
	case cropInputHeightValue:
	case cropInputPosxValue:
	case cropInputPosyValue:
        sprintf(buffer, "%d", value);
        break;

	case saleNumberValue:
		if(getDeviceInfo()->logoDisMode & 0x80)
		{
			return langGetStr(salesNumberStr);
		}
		else
		{
			return getDeviceInfo()->saleNumStr;
		}
		//break;
	case customNumberValue:
		if(getDeviceInfo()->logoDisMode & 0x80)
		{
			return langGetStr(afterSaleNumberStr);
		}
		else
		{
			return getDeviceInfo()->customNumStr;
		}
		//break;
	case emailValue:
		if(getDeviceInfo()->logoDisMode & 0x80)
		{
			return langGetStr(emailStr);
		}
		else
		{
			return getDeviceInfo()->emailStr;
		}
		//break;
	case webSiteValue:
		if(getDeviceInfo()->logoDisMode & 0x80)
		{
			return langGetStr(webSiteStr);
		}
		else
		{
			return getDeviceInfo()->websiteStr;
		}
		
	case deHPalarityValue:
	case deVPalarityValue:
		return langGetStr(positiveStr+(value&0x01));

	case bitDepthValue:
		return langGetStr(bit_8Str + value);
		
	case ttlInterlaveModeValue:
		sprintf(buffer,"STEP_%d",value);
		break;
		
	case hotBackupEnableValue:
		return langGetStr(offStr+value);
	case hotBackupClass2Value:	
	case hotBackupClass3Value:	
	case hotBackupClass4Value:	
	case hotBackupClass5Value:	
		if(getValue(index) == getValue(index - 1))
		{
			return langGetStr(offStr);
		}
	case hotBackupClass1Value:	
		return inputSourceStrList[value];
	case edidSourcePortValue:
		return edidSourceFileStrList[value];
	case edidDetinationPortValue:
		switch(value)
		{
			case EDID_DES_PORT_M1:
			case EDID_DES_PORT_M2:
			case EDID_DES_PORT_M3:
				break;
			case EDID_DES_PORT_DVI:
				printf("edidDetinationPortValue: EDID_DES_PORT_DVI\r\n");
				return inputSourceStrList[VIPDRIVE_INPUT_DVI];
			case EDID_DES_PORT_HDMI:
				return inputSourceStrList[VIPDRIVE_INPUT_HDMI];
			case EDID_DES_PORT_HDMI2:
				return inputSourceStrList[VIPDRIVE_INPUT_HDMI2];
			default:
			break;
		}
		break;

	case currentSpeedValue:
		if(value == 0)
		{
			return langGetStr(offStr);
		}
		else
		{
			if(lang_GetLang() == 0)
			{
				sprintf(buffer, "%d00RPM", value);
			}
			else
			{
				sprintf(buffer, "%d00转", value);
			}
		}
		break;

	case currentTempValue:
		sprintf(buffer, "%d℃", value);
		break;
        
    case colorTempValue:
		if(value > 5)
			value = 5;
		sprintf(buffer,"%s",colorTemperStr[value]);
	break;
	case videoStatusValue:
	case mainBoardStatusValue:
		return langGetStr(unnormalStr+value);
		
	case splitFixedTypeValue:
		return langGetStr(Horizontal2Str+value);
	case splitFixedIndexValue:
		//sprintf(buffer, "%d", value+1);
		//break;
	case splitFixedHNumberValue:
	case splitFixedVNumberValue:
	case splitFixedHSizeValue:
	case splitFixedVSizeValue:
		sprintf(buffer, "%d", value);
		break;

	case extInputComfigValue:
		if((value == 0)||(value == 1))
		{
			return extInputConfigListStr[value];
		}
		break;
		
	case sendCardTypeValue:
		if(value>2)
		{
			value = 0;
		}
		return langGetStr(sendCardType_Colorlight+value);

    case sendCardPortSetWidthValue:
    case sendCardPort1WidthValue:
    case sendCardPort2WidthValue:
    case sendCardPort3WidthValue:
    case sendCardPort4WidthValue:
		if(value>1)
		{
			value = 0;
		}
        return langGetStr(sendCardAutoStr+value);
	case sendcardchoosevalue:		//1发送卡类型
	case sendcardVersionValue:
    case networkchoosevalue:
	case sendCardPortSelValue:
	break;
    case sendCardWallConectValue:
    break;

	case hdmi20PortHdmi20EdidValue:
	case hdmi20PortHdmi20VideoEdidValue:
	case hdmi20PortHdmi14EdidValue:
	case hdmi20PortHdmi13EdidValue:
	case hdmi14PortHdmi14EdidValue:
	case hdmi14PortHdmi13EdidValue:
	case dpPortDpEdidValue:
	case dviPortDualLinkDviEdidValue:
	case dviPortDviEdidValue:
	case dviPortHdmi13EdidValue:
	case hdmi20PortCustomEdidValue:
	case hdmi14PortCustomEdidValue:
	case hdmi20PortHdmi20NoHfvsdbNo420: 		
	case hdmi20PortHdmi20NoHfvsdbWith420: 	
	case hdmi20PortHhdmi20WithHfvsdbNo420: 	
	case hdmi20PortHdmi20WithHfvsdbNo420Hdr: 
	case hdmi20PortHdmi20WithHfvsdbWith420: 
	case dpPortCustomEdidValue:
	case dpPort4k2k60Value:
	case dpPort4k2k30Value:
	case dviPortCustomEdidValue:
	case outputCustomValue:
	case _4pLTSendValue:
	case _4pLBSendValue:
	case _4pRTSendValue:
	case _4pRBSendValue:
		
	case connectvalue:
	case sendCardPanelConectValue:
	case sendCardSendToPortValue:
    case sendCardSendAllPortValue:    
    case sendCardSendToAutoPortValue:
	case FieldSplitParamSendValue:
	case Horizontal2SplitParamSendValue:
	case Vertical2SplitParamSendValue:
	case two4kTakeModeSendValue:
	case Horl2SplitParamSendValue:
	case Verl2SplitParamSendValue:
	case Horizontal3SplitParamSendValue:
	case Horizontal4SplitParamSendValue:
	case Vertical4SplitParamSendValue:
   	case sendCardSetWidthValue:
	case layerAResetValue:
	case layerBResetValue:
	case layerCResetValue:
	case layerDResetValue:
	case outPutDvi1Value:
	case outPutDvi2Value:
	case outPutDvi3Value:
	case outPutDvi4Value:
			sprintf(buffer, " ");
	break;
	case _4pLTSourceValue:
	case _4pLBSourceValue:
	case _4pRTSourceValue:
	case _4pRBSourceValue:
	case pgmSourceValue:
	case pipSourceValue:
	case prvSourceValue:
		sprintf(buffer, inputSourceStr_List[GET_INPUT_SOURCE_INDEX(value)]);
	break;

	case mode4PSplitParamSendValue:
	case pipModeValue:
		return (value & 0x01)?(langGetStr(onStr)):(langGetStr(offStr));
	case outputFormatIndex:
		sprintf(buffer,"%dx%d@%d",tVideoDefineCustom.hActive,tVideoDefineCustom.vActive,tVideoDefineCustom.vfreq);
	break;
   	case output800x600x60Value:
	case output1024x768x60Value:
	case output1280x720x60Value:
	case output1280x720x50Value:
	case output1280x768x60Value:
	case output1280x800x60Value:
	case output1280x1024x60Value:
	case output1360x768x60Value:
	case output1366x768x60Value:
	case output1400x1050x60Value:
	case output1440x900x60Value:
	case output1600x1200x60Value:
	case output1680x1050x60Value:
	case output1920x1080x60Value:
	case output1920x1080x50Value:
	case output1920x1200x60Value:
	case output2048x1152x60Value:
	case output2560x812x60Value:
		sprintf(buffer, "HZ");
	break;
	
	case cascadevalue:
		if(value >1)
		{
			value =0;
		}
		return langGetStr(offStr + value);
		
	case brightness_sendcard_value:
	break;
	case cardoutputFormatValue:					//1发送卡输出分辨率
	break;
	case outputPort0Value:
		sprintf(buffer,outputPortStr_List[getValue(outputPort0Value) & 0x03]);
	break;
	case outputPort1Value:
		sprintf(buffer,outputPortStr_List[getValue(outputPort1Value) & 0x03]);
	break;
	case outputPort2Value:	
		sprintf(buffer,outputPortStr_List[getValue(outputPort2Value) & 0x03]);
	break;
	case outputPort3Value:
		sprintf(buffer,outputPortStr_List[getValue(outputPort3Value) & 0x03]);
	break;

#if ENABLE_TEST_GRID_CONTROL
    case GridPortValue:
        return langGetStr(port1Str + value);
	case GridTestValue:
	case gridBackgroudVaule:
		return langGetStr(offStr + value);
	case gridModeValue:
		return langGetStr(lineModeCrossStr + value);
	case gridColorVaule:
		if(0 == value)
			return langGetStr(redStr);
		else if(1 == value)
			return langGetStr(greenStr);
		else 
			return langGetStr(yellowStr);
    case gridDotColorValue:
        if(0 == value)
            return langGetStr(whiteStr);
		else if(1 == value)
			return langGetStr(redStr);
		else if(2 == value)
			return langGetStr(greenStr);
		else if(3 == value) 
			return langGetStr(blueStr);
        else if(4 == value) 
            return langGetStr(customStr);
        
	case gridDotValue:
        if(value > 1)
        {
           value = 0; 
        }
        if(0 == value)
            return langGetStr(gridDotMoveStr);
		else if(1 == value)
			return langGetStr(gridDotManualStr);
        else
        {
        }
	break;
#endif

	case videoPanelFWSaveValue:
		return langGetStr(naStr);

	case artNetStartAddrValue:
		sprintf(buffer, "%3d", value);
		break;
	case artNetUniverseValue:
		 sprintf(buffer, "%d", value);
		 break;

	case outputPort0TpyeValue:
	case outputPort1TpyeValue:
	case outputPort2TpyeValue:
	case outputPort3TpyeValue:
		sprintf(buffer, "%s", outputTypeStr[value & 0x01]);
	break;
	
	case outputPort0BitWidthValue:
	case outputPort1BitWidthValue:
	case outputPort2BitWidthValue:
	case outputPort3BitWidthValue:
		sprintf(buffer, "%s", bitWidthStr[value & 0x03]);
	break;
	
	case outputPort0RangeValue:
	case outputPort1RangeValue:
	case outputPort2RangeValue:
	case outputPort3RangeValue:
		sprintf(buffer, "%s", outputRangeStr[value & 0x01]);
	break;
    default:
        sprintf(buffer, "%d", value);
    break;
		
    }
    return buffer;
}

void dispFileName(char *source,char *dest,int source_len,int dest_len)
{
	int i,dot,logo;
	for(i = 0; i < source_len; i++)  //找到后缀的'.'的位置
	{
		if(*(source + i) == '.')
		{
			dot = i;
		}
	}
	printf("dot = %d\r\n",dot);
	for(i =0; i < 10; i+=2)
	{
		if((*(source + dot + i) == 0x00)&&(*(source + dot + i + 1) == 0x00))
		{
			break;
		}
		
	}
	logo = i;
	printf("len of logo = %d\r\n",i);
	for(i = 0; i < (dest_len - logo); i++)
	{
		*(dest+i) = *(source+i);
		printf(" %02x   ",*(dest+i));
	}
	for(i = 0; i < logo; i++)
	{
		*(dest+i+dest_len-logo) = *(source + dot + i);
		printf(" %02x   ",*(dest+i+dest_len-logo));
	}
	printf("\r\n");
}




int getValue(int index)
{
	int result = 0;
//	BYTE curCard = 0;
//	BYTE curport = 0;
	
    switch(index)
	{
		case naValue:
			return 0;
		case dvi1OutputValue:
		case dvi1InputValue:
			return 0;//getMB103_HDMISwitch(MB103_HDMISWITCH_PORT_INPUT);
		case snValue:
			return getDeviceSN();
		case mcuVersionValue:
			return (SOFT_VERSION_HIGH<<8)+SOFT_VERSION_LOW;
		case stepValue:
			return step;
		case videoVersionValue:	
			return GetRtd2796Ver();//(SOFT_VERSION_HIGH<<8)+SOFT_VERSION_LOW;
		case fpgaVersionValue:
			return FPGA_GetSoftVersion(NULL,NULL);
		case ext1MainBoardSoftVersionValue:
			return (EXT1_VERSION_HIGH<<8)+EXT1_VERSION_LOW;
		case ext2MainBoardSoftVersionValue:
			return (EXT2_VERSION_HIGH<<8)+EXT2_VERSION_LOW;
		case mainBoardVersionValue:
		break;
		case commBoardHardVersionValue:
			return getCommHardVersion();
		
		case mainBoardHardVersionValue:
			return ((HARD_VERSION_HIGH << 8) + HARD_VERSION_LOW);
		case ext1BoardHardVersionValue:
			return ((EXT1_HARD_VERSION_HIGH << 8) + EXT1_HARD_VERSION_LOW);
		case ext2BoardHardVersionValue:
			return ((EXT2_HARD_VERSION_HIGH << 8) + EXT2_HARD_VERSION_LOW);
		case extInputHardVersionValue:
			if(getDeviceInfo()->extType == 0)
			{
				return 0xff;
			}
		case factoryResetValue:
			return 0;
		case saveIpResetValue:
			return 0;
		case volumeValue:
		case hdmiAudioValue:
		case dviAndExtAudioValue:
		case audioTypeValue:
		case hdmiAndExtAudioValue:
		case audioModeValue:
		case muteOnOffValue:
		case audioInput1Value:
		case audioInput2Value:
		case audioInput3Value:	
		case displayModeValue:			
		break;
		case languageValue:
			return GetLanguage();
		case outputFormatValue:
		case standardFormatValue:	
		case inputSourceValue:
		case zoomPosxValue:
		case zoomPosyValue:
		case zoomWidthValue:
		case zoomHeightValue:
		case cropUpValue:
		case cropDownValue:
		case cropUpDownValue:
		case cropLeftValue:
		case cropRightValue:
		case cropLeftRightValue:
		case cropCenterValue:	
		case cropInputWidthValue:
		case cropInputHeightValue:
		case cropInputPosxValue:
		case cropInputPosyValue:
    	case colorTempValue:
			return GetSystemParam()->colorTemper;
		case colorTempRedValue:
			return GetSystemParam()->colorTemperRed;
		case colorTempGreenValue:
			return GetSystemParam()->colorTemperGreen;
		case colorTempBlueValue:
			return GetSystemParam()->colorTemperBlue;
		case brightnessValue:
			return GetSystemParam()->brightness;
		case contrastValue:
			return GetSystemParam()->cnntrast;
		case colorEffectValue:
			return GetSystemParam()->colorEffect;
		case sharpnessValue:
		case saturationValue:
		case vgaPosxValue:
		break;
		case vgaPosyValue:
		break;
		case vgaClockValue:
		break;
		case vgaPhaseValue:
		break;
		case screenWidthValue:
		case screenHeightValue:
		case screenPosxValue:
		case screenPosyValue:
		case autoFitValue:
		case scaleModeValue:
		case screenModeValue:
		case screenFollowValue:
		case dviSelectValue:
		case vgaSelectValue:
		case keyLockValue:
			return getPadLock()->keyLock;
		case dePosxValue:
		case dePosyValue:
		case deWidthValue:
		case deHeightValue:
		case deHPalarityValue:
		case deVPalarityValue:
		case deOnOffValue:
		case bitDepthValue:
		case dataRangeValue:
		case dviHdmiValue:
		case gammaValue:
			return GetSystemParam()->gamma;
		case vga1KeyModeValue:
		case blackKeyTypeValue:
		case scaleKeyTypeValue:
		case mapKeyTypeValue:
		case fadeTimeValue:
			return GetSystemParam()->SysFadeTime;
		case previewDblTapSwitchValue:	
		case previewSwitchValue:	
    	case previewDVISwitchValue:  
		case alphaValue:
		break;
		case saveToValue:
			return saveTo;
		case loadFromValue:
			return loadFrom;
		case usbAllLoadValue:		
		case usbSingleLoadValue:
			return usbLoadFrom;		
		case usbAllSaveValue:
			return usbSaveTo;		
		case freezeValue:
		case swapMainPipValue:
		case layoutValue:
			return GetSystemParam()->PipLayout;
		case currentWindowValue:
			return GetSystemParam()->CurrentWindow;
		case windowRatioValue:
		case flatRedValue:
		case flatGreenValue:
		case flatBlueValue:
		case lockModeValue:
		case lockSourceValue:
    	case tpSwitchValue:
		case testPatternModeValue:
		case tpColorRedValue:
		case tpColorGreenValue:
		case tpColorBlueValue:
		case tpHorStepValue:
		case tpVerStepValue:
		case tpStepColorValue:
		case inputHStartValue:
		case inputVStartValue:
		case inputHActValue:
		case inputVActValue:
		case textEnableValue:
		case textAlphaValue:
		case textAboveValue:
		case textAndOrValue:
		case textModeValue:
		case textRedValue:
		case textGreenValue:
		case textBlueValue:
		case textPresetValue:
		case splitOnOffValue:
		case splitSyncModeValue:
		case splitHTotalValue:
		case splitHSizeValue:
		case splitHPosValue:
		case splitVTotalValue:
		case splitVSizeValue:
		case splitVPosValue:
		case totalWorkTimeValue:
		case bootTimesValue:
		case workTimeValue:
		case lastWorkTimeValue:
		case interlaceSwitchValue:
		case imageEnhanceValue:		
		case hdmiColorSpaceAdjustValue:	
		case vLockLineValue:
		break;
		case customHValue:
			return GetPanleAndSplitParamRam()->stCustomFormat.hActive;
		case customVValue:
			return GetPanleAndSplitParamRam()->stCustomFormat.vActive;
		case customFValue:
			return GetPanleAndSplitParamRam()->stCustomFormat.vfreq;
		case customEDIDHValue:
		case customEDIDVValue:
		case customEDIDFValue:
		break;
		case luminanceValue:
			return OLEDLum ;//getOLEDLuminance();
		case displayAlphaValue:
			return GetPanleAndSplitParamRam()->alpha;
		case temperatureModeValue:
		case temperatureValue:	
		case ttlInterlaveModeValue:
		case hotBackupEnableValue:
			return getHotBackupEnable();
		case hotBackupClass1Value:	
		case hotBackupClass2Value:	
		case hotBackupClass3Value:	
		case hotBackupClass4Value:	
		case hotBackupClass5Value:	
			return getHotBackupDetials(index - hotBackupClass1Value);
		case edidSourcePortValue:
			return getEDIDSourcePort();
		case edidDetinationPortValue:
			return getEDIDDesPort();
		case inputHActiveValue:		
		case inputVActiveValue:
		case inputFreqValue:
		case inputHTotalValue:
		case inputVTotalValue:
		case autoSpeedValue:
		case currentSpeedValue:
		case currentTempValue:
		case videoStatusValue:
		case mainBoardStatusValue:
		case realTimeSaveValue:
		case splitFixedTypeValue:
		case splitFixedIndexValue:
		case splitFixedHNumberValue:
		case splitFixedVNumberValue:
		case splitFixedHSizeValue:
		case splitFixedVSizeValue:
		case extInputComfigValue:
		case networkchoosevalue:
		break;
		case sendCardPortSelValue:
		break;
		case receivecard_number_value:
		case sendCardPanelNoValue:
		break;
		case xpos_receive_value:
		case sendCardPanelHOffsetValue:
		break;
		case ypos_receive_value:
		case sendCardPanelVOffsetValue:
		break;
		case RedColorTemSet_Value:
		break;
		case GreenColorTemSet_Value:
		break;
		case BlueColorTemSet_Value:
		break;
		case vlinevalue:
		case sendCardVPanelsValue:
		break;
		case vlinevalue11:
		break;
		case vlinevalue12:
		break;
    	case sendCardWallVPanelsValue:
		break;          
		case hlinevalue:
		case sendCardHPanelsValue:
		break;
		case hlinevalue11:
		break;
		case hlinevalue12:
		break;
    	case sendCardWallHPanelsValue:
		break;         
		case vsizevalue1:
		case vsizevalue2:
		case vsizevalue:
		case sendCardVLedsValue:
		break;
		case sendCardWallHLedsValue:
        break;
		case sendCardWallVLedsValue: 
        break;
		case hsizevalue1:
		case hsizevalue2:
		case hsizevalue:
		case sendCardHLedsValue:
		break;
	 	case xoffsetvalue:
	 	case sendCardPortHPosValue:
		break;
	 	case yoffsetvalue:
	 	case sendCardPortVPosValue:
		break;
	 	case widthValue:
		break;
	 	case heightValue:
		break;
	 	case connectvalue:
	 	case sendCardPanelConectValue:
		break;
    	case sendCardWallConectValue:
        break;
		case cascadevalue:
		break;
		case sendcardchoosevalue:
		break;		
		case gamma_sendcard_Value:
		break;		 
		case cardoutputFormatValue:		//1获取发送卡输出分辨率
		break;			
		case brightness_sendcard_value:
		break;		
		case videoPanelFWSaveValue:
		case usbUpdateConfirmValue:
		getUsbUpdateFlag();
		break;
		case WriteSnValue:
		return getDeviceInfo()->sn;
//		break;
	//########################################################################
	//输出分辨率
		case outputFormatIndex:
		case output800x600x60Value:
		case output1024x768x60Value:
		case output1280x720x60Value:
		case output1280x720x50Value:
		case output1280x768x60Value:
		case output1280x800x60Value:
		case output1280x1024x60Value:
		case output1360x768x60Value:
		case output1366x768x60Value:
		case output1400x1050x60Value:
		case output1440x900x60Value:
		case output1600x1200x60Value:
		case output1680x1050x60Value:
		case output1920x1080x60Value:
		case output1920x1080x50Value:
		case output1920x1200x60Value:
		case output2048x1152x60Value:
		case output2560x812x60Value:
		case outputCustomValue:
			return GetOutputFormatIndex();
		case mode4PSplitParamSendValue:
			return GetSystemParam()->Osd4pSwitch;
		case pipModeValue:
			return GetSystemParam()->PipModeSwitch;
		break;
		case _4pLTSourceValue:
		case pgmSourceValue:
			return (InputSourceConv(GetPanleAndSplitParamRam()->inputSource1));
		case _4pRTSourceValue:
//		case pipModeValue:
		case pipSourceValue:
		case prvSourceValue:
			return (InputSourceConv(GetPanleAndSplitParamRam()->inputSource2));
		case _4pLBSourceValue:
			return (InputSourceConv(GetPanleAndSplitParamRam()->inputSource3));
		case _4pRBSourceValue:
			return (InputSourceConv(GetPanleAndSplitParamRam()->inputSource4));
		case totalWidthValue:
			if(GetPanleAndSplitParamRam()->fieldSplit_UserTotalDisplayWidth > tVideoDefineCustom.hActive*2)
				GetPanleAndSplitParamRam()->fieldSplit_UserTotalDisplayWidth = tVideoDefineCustom.hActive*2;
			return (GetPanleAndSplitParamRam()->fieldSplit_UserTotalDisplayWidth);
		case totalHeighValue:
			if(GetPanleAndSplitParamRam()->fieldSplit_UserTotalDisplayHeight > tVideoDefineCustom.vActive*2)
				GetPanleAndSplitParamRam()->fieldSplit_UserTotalDisplayHeight = tVideoDefineCustom.vActive*2;
			return (GetPanleAndSplitParamRam()->fieldSplit_UserTotalDisplayHeight);
		case firstDisWidthValue:
			if(GetPanleAndSplitParamRam()->fieldSplit_FirstDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->fieldSplit_FirstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->fieldSplit_FirstDisplayWidth);
		case firstDisHeighValue:
			if(GetPanleAndSplitParamRam()->fieldSplit_FirstDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->fieldSplit_FirstDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->fieldSplit_FirstDisplayHeight);
		case secondDisWidthValue:
			return (GetPanleAndSplitParamRam()->fieldSplit_SecondDisplayWidth);
		case secondDisHeighValue:
			return (GetPanleAndSplitParamRam()->fieldSplit_SecondDisplayHeight);
		case thirdDisWidthValue:
			return (GetPanleAndSplitParamRam()->fieldSplit_ThirdDisplayWidth);
		case thirdDisHeighValue:
			return (GetPanleAndSplitParamRam()->fieldSplit_ThirdDisplayHeight);
		case fourDisWidthValue:
			return (GetPanleAndSplitParamRam()->fieldSplit_FourthDisplayWidth);
		case fourDisHeighValue:
			return (GetPanleAndSplitParamRam()->fieldSplit_FourthDisplayHeight);

		case h12TotalWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayWidth > tVideoDefineCustom.hActive*2)
				GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayWidth = tVideoDefineCustom.hActive*2;
			return (GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayWidth);
		case h12TotalHeighValue:
			if(GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayHeight);
		case h12FirstDisWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight12_FirstDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->leftRight12_FirstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->leftRight12_FirstDisplayWidth);
		case h12FirstDisHeighValue:
			if(GetPanleAndSplitParamRam()->leftRight12_FirstDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->leftRight12_FirstDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->leftRight12_FirstDisplayHeight);
		case h12SecondDisWidthValue:
		case h12SecondDisHeighValue:
		case h12ThirdDisWidthValue:
		case h12ThirdDisHeighValue:
		case h12FourDisWidthValue:
		case h12FourDisHeighValue:
		case v12TotalWidthValue:
			if(GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayWidth);
		case v12TotalHeighValue:
			if(GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayHeight > tVideoDefineCustom.vActive*2)
				GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayHeight = tVideoDefineCustom.vActive*2;
			return (GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayHeight);
		case v12FirstDisWidthValue:
			if(GetPanleAndSplitParamRam()->topBotton12_firstDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->topBotton12_firstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->topBotton12_firstDisplayWidth);
		case v12FirstDisHeighValue:
			if(GetPanleAndSplitParamRam()->topBotton12_firstDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->topBotton12_firstDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->topBotton12_firstDisplayHeight);
		case v12SecondDisWidthValue:
		case v12SecondDisHeighValue:
		case v12ThirdDisWidthValue:
		case v12ThirdDisHeighValue:
		case v12FourDisWidthValue:
		case v12FourDisHeighValue:

		case h14TotalWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayWidth > tVideoDefineCustom.hActive*4)
				GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayWidth = tVideoDefineCustom.hActive*4;
			return (GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayWidth);
		case h14TotalHeighValue:
			if(GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayHeight);
		case h14FirstDisWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight14_FirstDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->leftRight14_FirstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->leftRight14_FirstDisplayWidth);
		
		case h14SecondDisWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight14_SecongDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->leftRight14_SecongDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->leftRight14_SecongDisplayWidth);
		case h14ThirdDisWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight14_ThirdDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->leftRight14_ThirdDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->leftRight14_ThirdDisplayWidth);
			
		case h14FirstDisHeighValue:
		case h14SecondDisHeighValue:
		case h14ThirdDisHeighValue:
		case h14FourDisWidthValue:
		case h14FourDisHeighValue:
		break;

		case hor12TotalWidthValue:
			if(GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayWidth > tVideoDefineCustom.hActive*2)
				GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayWidth = tVideoDefineCustom.hActive*2;
			return (GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayWidth);
		case hor12TotalHeighValue:
			if(GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayHeight);
		case hor12FirstDisWidthValue:
			if(GetPanleAndSplitParamRam()->FourPort12_FirstDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->FourPort12_FirstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->FourPort12_FirstDisplayWidth);
		case hor12FirstDisHeighValue:
		case hor12SecondDisWidthValue:
		case hor12SecondDisHeighValue:
		case hor12ThirdDisWidthValue:
		case hor12ThirdDisHeighValue:
		case hor12FourDisWidthValue:
		case hor12FourDisHeighValue:
		break;

		case ver12TotalWidthValue:
			if(GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayWidth);
		case ver12TotalHeighValue:
			if(GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayHeight > tVideoDefineCustom.vActive*2)
				GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayHeight = tVideoDefineCustom.vActive*2;
			return (GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayHeight);
		
		case ver12FirstDisHeighValue:
			if(GetPanleAndSplitParamRam()->FourPortVer12_FirstDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->FourPortVer12_FirstDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->FourPortVer12_FirstDisplayHeight);
		case ver12FirstDisWidthValue:
		case ver12SecondDisWidthValue:
		case ver12SecondDisHeighValue:
		case ver12ThirdDisWidthValue:
		case ver12ThirdDisHeighValue:
		case ver12FourDisWidthValue:
		case ver12FourDisHeighValue:
		break;
		case h13TotalWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayWidth > tVideoDefineCustom.hActive*3)
				GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayWidth = tVideoDefineCustom.hActive*3;
			return (GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayWidth);
		case h13TotalHeighValue:
			if(GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayHeight > tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayHeight);
		case h13FirstDisWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight13_FirstDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->leftRight13_FirstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->leftRight13_FirstDisplayWidth);
		case h13SecondDisWidthValue:
			if(GetPanleAndSplitParamRam()->leftRight13_SecongDisplayWidth > tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->leftRight13_SecongDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->leftRight13_SecongDisplayWidth);
		case h13FirstDisHeighValue:
		case h13SecondDisHeighValue:
		case h13ThirdDisWidthValue:
		case h13ThirdDisHeighValue:
		case h13FourDisWidthValue:
		case h13FourDisHeighValue:
		break;

		case mode4PTotalWidthValue:
		break;
		case mode4PTotalHeighValue:
		break;
		case mode4PFirstDisWidthValue:
			if(GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth);
		case mode4PFirstDisHeighValue:
			if(GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight);
		case mode4PSecondDisWidthValue:
			if(GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth);
		case mode4PSecondDisHeighValue:
			if(GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight);
		case mode4PThirdDisWidthValue:
			if(GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth);
		case mode4PThirdDisHeighValue:
			if(GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight);
		case mode4PFourDisWidthValue:
			if(GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth);
		case mode4PFourDisHeighValue:
			if(GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight);
		case userAlphaValue:
			return (GetPanleAndSplitParamRam()->alpha);
		case outputPort0Value:
			return (GetPanleAndSplitParamRam()->outputPort1Map);
		case outputPort1Value:
			return (GetPanleAndSplitParamRam()->outputPort2Map);
		case outputPort2Value:	
			return (GetPanleAndSplitParamRam()->outputPort3Map);
		case outputPort3Value:
			return (GetPanleAndSplitParamRam()->outputPort4Map);

		case _4pLTWValue:
			if(GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth);
		case _4pLTHValue:
			if(GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight);

		case _4pLBWValue:
			if(GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth);
		case _4pLBHValue:
			if(GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight);

		case _4pRTWValue:
			if(GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth);
		case _4pRTHValue:
			if(GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight);

		case _4pRBWValue:
			if(GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth >= tVideoDefineCustom.hActive)
				GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth = tVideoDefineCustom.hActive;
			return (GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth);
		case _4pRBHValue:
			if(GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight >= tVideoDefineCustom.vActive)
				GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight = tVideoDefineCustom.vActive;
			return (GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight);
			
		case layerAHSizeValue:
			return layerParam.layerAWidth;
		case layerAVSizeValue:
			return layerParam.layerAHeight;
		case layerAHPosValue:
			return layerParam.layerAXstart;
		case layerAVPosValue:
			return layerParam.layerAVstart;
		case layerASplitStartValue:
			return layerParam.layerASplitStart;
		case layerACropStartValue:
			return layerParam.layerASplitWidth;
		case layerBHSizeValue:
			return layerParam.layerBWidth;
		case layerBVSizeValue:
			return layerParam.layerBHeight;
		case layerBHPosValue:
			return layerParam.layerBXstart;
		case layerBVPosValue:
			return layerParam.layerBVstart;
		case layerBSplitStartValue:
			return layerParam.layerBSplitStart;
		case layerBCropStartValue:
			return layerParam.layerBSplitWidth;
		case layerBFineValue:
			return layerParam.layerACropStart;

		case layerCHSizeValue:
			return layerParam.layerCWidth;
		case layerCVSizeValue:
			return layerParam.layerCHeight;
		case layerCHPosValue:
			return layerParam.layerCXstart;
		case layerCVPosValue:
			return layerParam.layerCVstart;
		case layerCSplitStartValue:
			return layerParam.layerCSplitStart;
		case layerCCropStartValue:
			return layerParam.layerCSplitWidth;

		case layerDHSizeValue:
			return layerParam.layerDWidth;
		case layerDVSizeValue:
			return layerParam.layerDHeight;
		case layerDHPosValue:
			return layerParam.layerDXstart;
		case layerDVPosValue:
			return layerParam.layerDVstart;
		case layerDSplitStartValue:
			return layerParam.layerDSplitStart;
		case layerDCropStartValue:
			return layerParam.layerDSplitWidth;
		case layerDFineValue:
			return layerParam.layerBCropStart;

		case outputPort0TpyeValue:
			return GetOutputTypeParamRam()->output0Type;
		case outputPort1TpyeValue:
			return GetOutputTypeParamRam()->output1Type;
		case outputPort2TpyeValue:
			return GetOutputTypeParamRam()->output2Type;
		case outputPort3TpyeValue:
			return GetOutputTypeParamRam()->output3Type;
		
		case outputPort0BitWidthValue:
			return GetOutputTypeParamRam()->output0BitWidth;
		case outputPort1BitWidthValue:
			return GetOutputTypeParamRam()->output1BitWidth;
		case outputPort2BitWidthValue:
			return GetOutputTypeParamRam()->output2BitWidth;
		case outputPort3BitWidthValue:
			return GetOutputTypeParamRam()->output3BitWidth;

		case outputPort0RangeValue:
			return GetOutputTypeParamRam()->output0Range;
		case outputPort1RangeValue:
			return GetOutputTypeParamRam()->output1Range;
		case outputPort2RangeValue:
			return GetOutputTypeParamRam()->output2Range;
		case outputPort3RangeValue:
			return GetOutputTypeParamRam()->output3Range;
		case scalePosxValue:
		case scalePosyValue:
		case scaleWidthValue:
		case scaleHeightValue:
			return GetScalerPosSize(index);
		default:
			if((0 <= index) && (index < maxSizeValue))
			{
				return valueList[index%4];
			}
			else
			{
				return 0;
			}
    }

	return result;
}

void SetCurrentIsEnterNumberFlag(char flag);

int setValue(int index, int value)
{	
	SetCurrentIsEnterNumberFlag(0);
    switch(index)
	{
		case naValue:
			value = 0;
	    break;
		case dvi1OutputValue:
		break;
		case dvi1InputValue:
		break;
		case autoTpIimeValue:
		break;
		case dateValue:
		break;
		case timeValue:
		break;
		case licenseTimeValue:
		break;
		case licenseStateValue:
		break;
		case licensePasswordValue:
		break;
		case lockPanelPasswordValue:
			getPadLock()->lockPanelPassword = value;
			saveUserPadLock();
		break;
		case keyLockValue:
			getPadLock()->keyLock = value;
			saveUserPadLock();
			SysDelay1ms(10);
			loadPadLock();
		break;		
		case delayCallSaveValue:
		break;
		case adcAutoAdjustValue:
		break;
		case resetAdcSettingValue:
		break;
		case networkEnableValue:
		break;
		case dhcpEnableValue:
		break;
		case artNetStartAddrValue:
		break;
		case artNetUniverseValue:
		break;
		case ipAddress3Value:
		break;
		case RemoteIpAddressValue:
		break;		
		case LocalIpAddressValue:
		break;
		case subnetMaskValue:
		break;
		case gatewayAddressValue:
		break;
		case factoryResetValue:
			if(value)
			{
				dispFactoryReset();
			}
		break;
		case saveIpResetValue:
		break;
		case muteOnOffValue:
		break;
		case audioModeValue:
		break;
		case volumeValue:
		break;
		case hdmiAudioValue:
		break;
		case dviAndExtAudioValue:
		break;
		case audioTypeValue:
		break;
		case hdmiAndExtAudioValue:
		break;	
		case audioInput1Value:
		case audioInput2Value:
		case audioInput3Value:		
		case flatRedValue:
		case flatGreenValue:
		case flatBlueValue:
		break;
		case displayModeValue:
		break;
		case windowRatioValue:
		break;
		case languageValue:
			SetLanguage(value);
		break;
		case outputFormatValue:
		case standardFormatValue:
			SetOutputFormat(value);
		break;
		case inputSourceValue:
		break;
		case resetScaleValue:
		break;
		case resetSdiInputValue:
		break;
		case zoomWidthValue:
		break;
		case zoomHeightValue:
		break;
		case zoomPosxValue:
		break;
		case zoomPosyValue:
		break;
		case resetZoomValue:
		break;
		case cropUpValue:
		break;
		case cropDownValue:
		break;
		case cropUpDownValue:
		break;
		case cropLeftValue:
		break;
		case cropRightValue:
		break;
		case cropLeftRightValue:
		break;
		case cropCenterValue:
		break;
		case resetCropValue:
		break;
		case cropInputWidthValue:
		case cropInputHeightValue:
		case cropInputPosxValue:
		case cropInputPosyValue:
		break;
		case resetCropInputValue:
		break;
	    case colorTempValue:
			GetSystemParam()->colorTemper = value;
			SetVideocolorTempType(GetSystemParam()->colorTemper);
	    break;
		case colorTempBlueValue:
			if(value >= 255)
				GetSystemParam()->colorTemperBlue = 255;	
			else
				GetSystemParam()->colorTemperBlue = value;
			SetVideocolorTemp(0xa2,GetSystemParam()->colorTemperBlue);
		break;
		case colorTempGreenValue:
			if(value >= 255)
				GetSystemParam()->colorTemperGreen = 255;
			else
				GetSystemParam()->colorTemperGreen = value;
			SetVideocolorTemp(0xa1,GetSystemParam()->colorTemperGreen);
		break;
		case colorTempRedValue:
			if(value >= 255)
				GetSystemParam()->colorTemperRed = 255;
			else
				GetSystemParam()->colorTemperRed = value;
			SetVideocolorTemp(0xa0,GetSystemParam()->colorTemperRed);
		break;
		case resetColorTempValue:
		break;
		case contrastValue:
			if(value >= 4096)
				GetSystemParam()->cnntrast = 4096;
			else
				GetSystemParam()->cnntrast = value;
			SetVideoContrast(GetSystemParam()->cnntrast);
		break;
		case brightnessValue:
			if(value >= 1023)
				GetSystemParam()->brightness = 1023;
			else
				GetSystemParam()->brightness = value;
			SetVideoBrightness(GetSystemParam()->brightness);
		break;
		case colorEffectValue:
			GetSystemParam()->colorEffect = value;
			SetVideoColorEffect(GetSystemParam()->colorEffect);
		break;
		case sharpnessValue:
		break;
		case saturationValue:
		break;
		case vgaPosxValue:
		break;
		case vgaPosyValue:
		break;
		case vgaPhaseValue:
		break;
		case vgaClockValue:
		break;
		case vgaAutoAdjustValue:		
		break;	
		case resetInputConfigValue:
		break;
		case resetPictureValue:
			ResetVideoColorEffect(1);
		break;
		case screenWidthValue:
		break;
		case screenHeightValue:
		break;
		case screenPosxValue:
		break;
		case screenPosyValue:
		break;
		case autoFitValue:
		break;	
		case scaleModeValue:
		return 0;		
		case screenModeValue:
		break;
		case screenFollowValue:
		break;
		case resetScreenValue:
		break;
		case dviSelectValue:
		break;
		case vgaSelectValue:   
		break;
		case dataRangeValue:
		break;
		case dviHdmiValue:
		break;	
		case deOnOffValue:
		break;
		case bitDepthValue:
		break;
		case dePosxValue:
		break;
		case dePosyValue:
		break;
		case deWidthValue:
		break;
		case deHeightValue:
		break;
		case deHPalarityValue:
		break;
		case deVPalarityValue:
		break;
		case resetDviValue:
		break;
		case gammaValue:
			GetSystemParam()->gamma = value;
			SetVideoGamma(GetSystemParam()->gamma);
		break;
		case vga1KeyModeValue:
		break;
		case blackKeyTypeValue:
		break;
		case scaleKeyTypeValue:
		break;
		case mapKeyTypeValue:
		break;
		case switchModeValue:
		break;
		case fadeTimeValue:
			GetSystemParam()->SysFadeTime = value;
			SaveSystemParam();
		break;	
		case switchTypeValue:
		break;
		case previewDblTapSwitchValue:
		break;		
		case previewSwitchValue:	
		break;
	    case previewDVISwitchValue: 
	    break;
		case alphaValue:
		break;
		case loadFromValue:
	        LoadSave(value);
			loadFrom = value;
		break;
		case saveToValue:
	        SaveUserSetting((UserSaveIndex_TypeDef) (value));
			saveTo = value;
		break;
		case usbAllLoadValue:		
		break;
		case usbSingleLoadValue:
		break;
		case usbAllSaveValue:
		break;
		case stepValue:
			step = value;
		break;	
		case freezeValue:
		break;	
		case swapMainPipValue:
		break;	
		case layoutValue:
			SetPipLayout(value);
		break;
		case currentWindowValue:
			#if 0
			#if(ENABLE_SAVE_REAL_TIME)
			UserSettingSaveTo(USER_SAVE_1); 
			#else
			if(getRealTimeSaveEnable())
			{
				UserSettingSaveTo(USER_SAVE_1); 
			}
			#endif
			#else
			GetSystemParam()->CurrentWindow = value & 1;;
			#endif
		break;
		case lockModeValue:
		break;
		case lockSourceValue:
		break;
	    case tpSwitchValue:
	    break;
		case testPatternModeValue:
		break;
		case tpColorRedValue:
		break;
		case tpColorGreenValue:
		break;
		case tpColorBlueValue:
		break;
		case tpHorStepValue:
		break;
		case tpVerStepValue:
		break;
		case tpStepColorValue:
		break;
		case inputHStartValue:
		break;
		case inputVStartValue:
		break;
		case inputHActValue:
		break;
		case inputVActValue:
		break;
		case textAboveValue:
		break;
		case textAndOrValue:
		break;
		case textEnableValue:
		break;
		case textModeValue:
		break;
		case textAlphaValue:
		break;
		case textRedValue:
		break;
		case textGreenValue:
		break;		
		case textBlueValue:
		break;
		case textPresetValue:
		break;
		case splitOnOffValue:
		break;
		case splitSyncModeValue:
		break;
		case splitHTotalValue:
		break;
		case splitHSizeValue:
		break;
		case splitHPosValue:
		break;
		case splitVTotalValue:
		break;
		case splitVSizeValue:
		break;
		case splitVPosValue:
		break;
		case resetSplitValue:
		break;
		case interlaceSwitchValue:
		break;
		case imageEnhanceValue:
		break;
		case hdmiColorSpaceAdjustValue:
		break;	
		case imageEnhanceVerifyValue:	
		break;
		case vLockLineValue:
		break;
		case customFormatValue:
			VideoPanelSetting(dispValue[0],dispValue[1],dispValue[2]);
		break;
		case customHValue:
			setCustomDisplayWidth(value);
		break;
		case customVValue:
			setCustomDisplayHeight(value);
		break;
		case customFValue:
			setCustomVFreq(value);
		break;
		case replaceRecoedValue:
		break;

		case luminanceValue:
			OLEDLum = value;
		break;
		case displayAlphaValue:
			if(value >= 255)
			{
				GetUserSettingParam()->userSplitParam.alpha = 255;
				return 0;
			}
			else
				GetUserSettingParam()->userSplitParam.alpha = value;
			
			SetDisplayAlpha(GetUserSettingParam()->userSplitParam.alpha);
			SavePanleAndSplitParam();
			//SysDelay1ms(10);
			//LoadPanleAndSplitParam();
		break;
		case temperatureModeValue:
			GetUserPref()->temperatureMode = value;
			saveUserPref();
		break;
		case temperatureValue:
		break;
		case bootloaderValue:
		break;
		case ttlInterlaveModeValue:
		break;
		case hotBackupEnableValue:
			setHotBackupEnable(value);
			defaultMenuPageProcess(MENU_EVENT_ENTER,0); 
		break;
		case hotBackupClass1Value:	
		case hotBackupClass2Value:	
		case hotBackupClass3Value:	
		case hotBackupClass4Value:	
		case hotBackupClass5Value:	
			setHotBackupDetials((index - hotBackupClass1Value),value);
		break;
		case edidSourcePortValue:
			setEDIDSourcePort(value);		
		break;
		case edidDetinationPortValue:
			setEDIDDesPort(value);
		break;
		case autoSpeedValue:
		break;
		case currentSpeedValue:
		break;
		case customEDIDValue:
		break;
		case customEDIDHValue:
		break;
		case customEDIDVValue:
		break;
		case customEDIDFValue:
		break;
		case realTimeSaveValue:
		break;	
		case splitFixedTypeValue:
		break;
		case splitFixedIndexValue:
		break;
		case splitFixedHNumberValue:
		break;
		case splitFixedVNumberValue:
		break;
		case splitFixedHSizeValue:		
		break;
		case splitFixedVSizeValue:		
		break; 	  
		case brightness_sendcard_value:
		break;
		case usbUpdateConfirmValue:
			setUsbUpdateFlag(value);
		break;	
		case WriteSnValue:
			getDeviceInfo()->sn = value;
			saveDeviceInfo();
			SysDelay1ms(100);
			loadDeviceInfo();
		break;
		case output800x600x60Value:
		case output1024x768x60Value:
		case output1280x720x60Value:
		case output1280x720x50Value:
		case output1280x768x60Value:
		case output1280x800x60Value:
		case output1280x1024x60Value:
		case output1360x768x60Value:
		case output1366x768x60Value:
		case output1400x1050x60Value:
		case output1440x900x60Value:
		case output1600x1200x60Value:
		case output1680x1050x60Value:
		case output1920x1080x60Value:
		case output1920x1080x50Value:
		case output1920x1200x60Value:
		case output2048x1152x60Value:
		case output2560x812x60Value:
			GetUserSettingParam()->userSplitParam.outputFormatIndex = index - output800x600x60Value;
			memcpy((u8*)&tVideoDefineCustom,(u8*)&tVideoDefine[index - output800x600x60Value],sizeof(VideoDefine_Struct));
			SetOutputFormat(GetUserSettingParam()->userSplitParam.outputFormatIndex);
			SavePanleAndSplitParam();
			SetFpgaOutputFormat(GetUserSettingParam()->userSplitParam.outputFormatIndex,0);
		break;
		case outputCustomValue:
			GetUserSettingParam()->userSplitParam.outputFormatIndex = MD_CUSTOM;
			SetOutputFormatCustom();
		break;
		case dpPortDpEdidValue:
		case hdmi20PortHdmi20EdidValue:
		case hdmi20PortHdmi20VideoEdidValue:
		case hdmi20PortHdmi14EdidValue:
		case hdmi20PortHdmi13EdidValue:
		case hdmi14PortHdmi14EdidValue:
		case hdmi14PortHdmi13EdidValue:
		case dviPortDualLinkDviEdidValue:
		case dviPortDviEdidValue:
		case dviPortHdmi13EdidValue:
		case dpPort4k2k60Value:
		case dpPort4k2k30Value:
		case hdmi20PortHdmi20NoHfvsdbNo420: 		
		case hdmi20PortHdmi20NoHfvsdbWith420: 	
		case hdmi20PortHhdmi20WithHfvsdbNo420: 	
		case hdmi20PortHdmi20WithHfvsdbNo420Hdr: 
		case hdmi20PortHdmi20WithHfvsdbWith420: 	
			WriteEdid(index);
		break;
		case dpPortCustomEdidValue:
		case hdmi20PortCustomEdidValue:
		case hdmi14PortCustomEdidValue:
		case dviPortCustomEdidValue:
			setEDIDSourcePort(8);	
		break;

	//#################################################################################
	//
		case Horizontal2SplitParamSendValue:
			if(value != -1)
			{
				menuSetBusy(1,9);
			}
			
			TwoPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_TB);

			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(1,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_H_1_SUB_2);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case Vertical2SplitParamSendValue:
			if(value != -1)
			{
				menuSetBusy(1,9);
			}
			TwoPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_LR);
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(1,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_V_1_SUB_2);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case two4kTakeModeSendValue:
			if(value != -1)
			{
				menuSetBusy(2,9);
			}
			Two4kTakeModeSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_LR);
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(2,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_TWO_4K_TAKE);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case FieldSplitParamSendValue:
			if(value != -1)
			{
				menuSetBusy(1,9);
			}
			FieldSplitSetting(&(GetUserSettingParam()->userSplitParam));
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(1,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_FIELD_SPLIT);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case Horl2SplitParamSendValue:
			if(value != -1)
			{
				menuSetBusy(0,9);
			}
			Horizontal2SplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_TOW_PORT_TB);
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(0,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_HORIZONTAL1_2_SPLIT);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case Verl2SplitParamSendValue:
			if(value != -1)
			{
				menuSetBusy(0,9);
			}
			Vertical2SplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_TOW_PORT_LR);
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(0,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_VERTICAL1_2_SPLIT);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case Horizontal3SplitParamSendValue:
			if(value != -1)
			{
				menuSetBusy(1,9);
			}
			ThirdPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_3P_SACLE_TB);
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(1,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_HORIZONTAL1_3_SPLIT);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case Horizontal4SplitParamSendValue:
			if(value != -1)
			{
				menuSetBusy(2,9);
			}
			FourPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_SACLE_TB);
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(2,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_HORIZONTAL1_4_SPLIT);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case Vertical4SplitParamSendValue:

			#if 1
			if(value != -1)
			{
				menuSetBusy(2,9);
			}
			FourPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_SACLE_LR);
			if(value != -1)
			{
				menuDisableBusy();
				LCD_WriteStr_fb2(2,18, "   OK  ");
				SysDelay1ms(1000);
				menuEnterPage(MENU_INDEX_VERTICAL1_4_SPLIT);
			}
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
			#endif
		break;
		case pipModeValue:
			PipModeSetting(value & 0x01);
		break;
		case mode4PSplitParamSendValue:
			GetSystemParam()->Osd4pSwitch = value & 0x01;
			if(GetSystemParam()->Osd4pSwitch == 1)
			{
				if(value != -1)
				{	
					menuEnterPage(MENU_INDEX_INDEPENDENT);
					menuSetBusy(0,8);
				}
				IndependentOutputSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_4P);
				if(value != -1)
				{
					menuDisableBusy();
					LCD_WriteStr_fb2(0,16, "   OK  ");
					SysDelay1ms(1000);
				}
				GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
				SaveCurrentUserSettingParam();
				SysDelay1ms(100);
				LoadCurrentUserSettingParam();
			}
			else if(GetSystemParam()->Osd4pSwitch == 0)
			{
				FPGASetFreeze(1);
				RTD2796Reset();
				menuSetBusy(0,8);
				menuEnterPage(MENU_INDEX_INDEPENDENT);
				SysDelay1ms(3000);
				FieldSplitSetting(&(GetUserSettingParam()->userSplitParam));
				GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
				SaveCurrentUserSettingParam();
				SysDelay1ms(100);
				LoadCurrentUserSettingParam();
				SysDelay1ms(4000);
				menuDisableBusy();
				LCD_WriteStr_fb2(0,16, "   OK  ");
				FPGASetFreeze(0);
			}
		break;
		case _4pLTSendValue:
			SetVideoRegionParam(	_OSD_SR_4P_LT_INSIDE,
									0,
									0,
									GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth,
									GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight
							   );
			SavePanleAndSplitParam();
		break;
		case _4pLBSendValue:
			SetVideoRegionParam(	_OSD_SR_4P_LB_INSIDE,
									0,
									0,
									GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth,
									GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight
							   );
			SavePanleAndSplitParam();
		break;
		case _4pRTSendValue:
			SetVideoRegionParam(	_OSD_SR_4P_RT_INSIDE,
									0,
									0,
									GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth,
									GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight
							   );
			SavePanleAndSplitParam();
		break;
		case _4pRBSendValue:
			SetVideoRegionParam(	_OSD_SR_4P_RB_INSIDE,
									0,
									0,
									GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth,
									GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight
							   );
			SavePanleAndSplitParam();
		break;
		case totalWidthValue:
			if(value >= tVideoDefineCustom.hActive*2)
				value = tVideoDefineCustom.hActive*2;
			GetUserSettingParam()->userSplitParam.fieldSplit_UserTotalDisplayWidth = value;
		break;
		case totalHeighValue:

			if(value >= tVideoDefineCustom.vActive*2)
				value = tVideoDefineCustom.vActive*2;
			GetUserSettingParam()->userSplitParam.fieldSplit_UserTotalDisplayHeight = value;
		break;
		case firstDisWidthValue:

			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetUserSettingParam()->userSplitParam.fieldSplit_FirstDisplayWidth = value;
		break;
		case firstDisHeighValue:

			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetUserSettingParam()->userSplitParam.fieldSplit_FirstDisplayHeight = value;
		break;
		case h12TotalWidthValue:
			if(value >= tVideoDefineCustom.hActive*2)
				value = tVideoDefineCustom.hActive*2;
			GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayWidth = value;
		break;
		case h12TotalHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->leftRight12_UserTotalDisplayHeight = value;
		break;
		case h12FirstDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->leftRight12_FirstDisplayWidth = value;
		break;
		case h12FirstDisHeighValue:
			GetPanleAndSplitParamRam()->leftRight12_FirstDisplayHeight = value;
		break;
		
		case h12SecondDisWidthValue:
		case h12SecondDisHeighValue:
		case h12ThirdDisWidthValue:
		case h12ThirdDisHeighValue:
		case h12FourDisWidthValue:
		case h12FourDisHeighValue:
		break;
		case v12TotalWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayWidth = value;
		break;
		case v12TotalHeighValue:
			if(value >= tVideoDefineCustom.vActive*2)
				value = tVideoDefineCustom.vActive*2;
			GetPanleAndSplitParamRam()->topBotton12_UserTotalDisplayHeight = value;
		break;
		case v12FirstDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->topBotton12_firstDisplayWidth = value;
		break;
		case v12FirstDisHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->topBotton12_firstDisplayHeight = value;
		break;
		case v12SecondDisWidthValue:
		case v12SecondDisHeighValue:
		case v12ThirdDisWidthValue:
		case v12ThirdDisHeighValue:
		case v12FourDisWidthValue:
		case v12FourDisHeighValue:
		break;

		case h14TotalWidthValue:
			if(value >= tVideoDefineCustom.hActive*4)
				value = tVideoDefineCustom.hActive*4;
			GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayWidth = value;
		break;
		case h14TotalHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->leftRight14_UserTotalDisplayHeight = value;
		break;
		case h14FirstDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->leftRight14_FirstDisplayWidth = value;
		break;
		case h14FirstDisHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->leftRight14_FirstDisplayHeight = value;
		break;
		
		case h14SecondDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->leftRight14_SecongDisplayWidth = value;
		break;
		case h14ThirdDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->leftRight14_ThirdDisplayWidth = value;
		break;
		case h14SecondDisHeighValue:
		case h14ThirdDisHeighValue:
		case h14FourDisWidthValue:
		case h14FourDisHeighValue:
		break;

		case hor12TotalWidthValue:
			if(value >= tVideoDefineCustom.hActive*2)
				value = tVideoDefineCustom.hActive*2;
			GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayWidth = value;
		break;
		case hor12TotalHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->FourPort12_UserTotalDisplayHeight = value;
		break;
		case hor12FirstDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->FourPort12_FirstDisplayWidth = value;
		break;
		case hor12FirstDisHeighValue:
		case hor12SecondDisWidthValue:
		case hor12SecondDisHeighValue:
		case hor12ThirdDisWidthValue:
		case hor12ThirdDisHeighValue:
		case hor12FourDisWidthValue:
		case hor12FourDisHeighValue:
		break;

		case ver12TotalWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayWidth = value;
		break;
		case ver12TotalHeighValue:
			if(value >= tVideoDefineCustom.vActive*2)
				value = tVideoDefineCustom.vActive*2;
			GetPanleAndSplitParamRam()->FourPortVer12_UserTotalDisplayHeight = value;
		break;
		case ver12FirstDisHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->FourPortVer12_FirstDisplayHeight = value;
		break;
		case ver12FirstDisWidthValue:
		case ver12SecondDisWidthValue:
		case ver12SecondDisHeighValue:
		case ver12ThirdDisWidthValue:
		case ver12ThirdDisHeighValue:
		case ver12FourDisWidthValue:
		case ver12FourDisHeighValue:
		break;
		
		case h13TotalWidthValue:
			if(value >= tVideoDefineCustom.hActive*3)
				value = tVideoDefineCustom.hActive*3;
			GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayWidth = value;
		break;
		case h13TotalHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->leftRight13_UserTotalDisplayHeight = value;
		break;
		case h13FirstDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->leftRight13_FirstDisplayWidth = value;
		break;
		case h13SecondDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->leftRight13_SecongDisplayWidth = value;
		break;
		case h13FirstDisHeighValue:
		case h13SecondDisHeighValue:
		case h13ThirdDisWidthValue:
		case h13ThirdDisHeighValue:
		case h13FourDisWidthValue:
		case h13FourDisHeighValue:
		break;	

		case mode4PTotalWidthValue:
		break;
		case mode4PTotalHeighValue:
		break;
		case mode4PFirstDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth = value;
		break;
		case mode4PFirstDisHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight = value;
		break;
		break;
		case mode4PSecondDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth = value;
		break;
		case mode4PSecondDisHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight = value;
		break;
		case mode4PThirdDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth = value;
		break;
		case mode4PThirdDisHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight = value;
		break;
		case mode4PFourDisWidthValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth = value;
		break;
		case mode4PFourDisHeighValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight = value;
		break;
		case userAlphaValue:
			if(value >= 255)
				value = 255;
			GetUserSettingParam()->userSplitParam.alpha = value;
		break;
		
		case pgmSourceValue:
			GetPanleAndSplitParamRam()->inputSource1 = InputSourceConv1(GET_INPUT_SOURCE_INDEX(value));
		break;
		case prvSourceValue:
			GetPanleAndSplitParamRam()->inputSource2 = InputSourceConv1(GET_INPUT_SOURCE_INDEX(value));
//		case pipModeValue:
		case pipSourceValue:
			GetPanleAndSplitParamRam()->inputSource2 = InputSourceConv1(GET_INPUT_SOURCE_INDEX(value));
			SetVideoInputSource(_DISPLAY_RGN_SUB,GetPanleAndSplitParamRam()->inputSource1,GetPanleAndSplitParamRam()->inputSource2,0,0);
		break;
		break;
		
		case _4pLTSourceValue:
			GetPanleAndSplitParamRam()->inputSource1 = InputSourceConv1(GET_INPUT_SOURCE_INDEX(value));
			Set4PRegionSource(_DISPLAY_RGN_LT,GetPanleAndSplitParamRam()->inputSource1);
		break;
		case _4pRTSourceValue:
			GetPanleAndSplitParamRam()->inputSource2 = InputSourceConv1(GET_INPUT_SOURCE_INDEX(value));
			Set4PRegionSource(_DISPLAY_RGN_RT,GetPanleAndSplitParamRam()->inputSource2);
		break;
		case _4pLBSourceValue:
			GetPanleAndSplitParamRam()->inputSource3 = InputSourceConv1(GET_INPUT_SOURCE_INDEX(value));
			Set4PRegionSource(_DISPLAY_RGN_LB,GetPanleAndSplitParamRam()->inputSource3);
		break;
		case _4pRBSourceValue:	
			GetPanleAndSplitParamRam()->inputSource4 = InputSourceConv1(GET_INPUT_SOURCE_INDEX(value));
			Set4PRegionSource(_DISPLAY_RGN_RB,GetPanleAndSplitParamRam()->inputSource4);
		break;
		case outputPort0Value:
			GetPanleAndSplitParamRam()->outputPort1Map = value;
			u8 OutputPortSel = FPGA_get(FPGA_REG_OutputPortSel);
			
			FpgaOutputPortSel(((OutputPortSel & 0x00) | (value  & 0x03)),((OutputPortSel & 0x0c) >> 2),((OutputPortSel & 0x30) >> 4),((OutputPortSel & 0xc0) >> 6));
			SavePanleAndSplitParam();
		break;
		case outputPort1Value:
			GetPanleAndSplitParamRam()->outputPort2Map = value;
			u8 OutputPortSel2 = FPGA_get(FPGA_REG_OutputPortSel);
			
			FpgaOutputPortSel(((OutputPortSel2 & 0x03)),(value  & 0x03),((OutputPortSel2 & 0x30) >> 4),((OutputPortSel2 & 0xc0) >> 6));
			SavePanleAndSplitParam();
		break;
		case outputPort2Value:
			GetPanleAndSplitParamRam()->outputPort3Map = value;
			u8 OutputPortSel3 = FPGA_get(FPGA_REG_OutputPortSel);
			
			FpgaOutputPortSel(((OutputPortSel3 & 0x03)),((OutputPortSel3 & 0x0c) >> 2),(value  & 0x03),((OutputPortSel3 & 0xc0) >> 6));
			SavePanleAndSplitParam();
		break;
		case outputPort3Value:
			GetPanleAndSplitParamRam()->outputPort4Map = value;
			u8 OutputPortSel4 = FPGA_get(FPGA_REG_OutputPortSel);
			
			FpgaOutputPortSel(((OutputPortSel4 & 0x03)),((OutputPortSel4 & 0x0c) >> 2),((OutputPortSel4 & 0x30) >> 4),(value  & 0x03));
			SavePanleAndSplitParam();
		break;

		case _4pLTWValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			(GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth) = value;
		break;
		case _4pLTHValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			(GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight) = value;
		break;

		case _4pLBWValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			(GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth) = value;
		break;
		case _4pLBHValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			(GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight) = value;
		break;

		case _4pRTWValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			(GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth) = value;
		break;
		case _4pRTHValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			(GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight) = value;
		break;

		case _4pRBWValue:
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
			(GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth) = value;
		break;
		case _4pRBHValue:
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
			(GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight) = value;
		break;
			

	case layerAHSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerAWidth = value;
			if(GetPanleAndSplitParamRam()->_4P_FirstDisplayWidth <= 1920)
			{
				//SetVideoRegionWidth(_OSD_SR_4P_LT_INSIDE,value);
				FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			else
			{
				//SysDelay1ms(100);
				FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerAVSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerAHeight = value;
			if(GetPanleAndSplitParamRam()->_4P_FirstDisplayHeight <= 1080)
			{
				//SetVideoRegionHeight(_OSD_SR_4P_LT_INSIDE,value);
				FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_MSB,(u8)(value >> 8));
			}
			else
			{
				//SysDelay1ms(100);
				FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerAHPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerAXstart = value;
			FPGA_set(FPGA_REG_h_Output0Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_h_Output0Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerAVPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerAVstart = value;
			FPGA_set(FPGA_REG_v_Output0Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_v_Output0Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerAResetValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_LSB,(u8)(layerParamBackUp.layerAWidth));
			FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_MSB,(u8)((layerParamBackUp.layerAWidth) >> 8));
			FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_LSB,(u8)(layerParamBackUp.layerAHeight));
			FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_MSB,(u8)((layerParamBackUp.layerAHeight) >> 8));
			FPGA_set(FPGA_REG_h_Output0Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerAXstart));
			FPGA_set(FPGA_REG_h_Output0Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerAXstart) >> 8));
			FPGA_set(FPGA_REG_v_Output0Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerAVstart));
			FPGA_set(FPGA_REG_v_Output0Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerAVstart) >> 8));
			memcpy((u8*)&layerParam,(u8*)&layerParamBackUp,14);
			LCD_WriteStr_fb2(0,22, "   OK  ");
			SysDelay1ms(1000);
		}
	break;

	case layerBHSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerBWidth = value;
			if(GetPanleAndSplitParamRam()->_4P_SecongDisplayWidth <= 1920)
			{
				//SetVideoRegionWidth(_OSD_SR_4P_RT_INSIDE,value);
				FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			else
			{
				//SysDelay1ms(100);
				FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerBVSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerBHeight = value;
			if(GetPanleAndSplitParamRam()->_4P_SecongDisplayHeight <= 1080)
			{
//				SetVideoRegionHeight(_OSD_SR_4P_RT_INSIDE,value);
				FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_MSB,(u8)(value >> 8));
			}
			else
			{
				//SysDelay1ms(100);
				FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerBHPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerBXstart = value;
			FPGA_set(FPGA_REG_h_Output1Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_h_Output1Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerBVPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerBVstart = value;
			FPGA_set(FPGA_REG_v_Output1Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_v_Output1Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerBFineValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerACropStart = value;
			FPGA_set(FPGA_REG_h_Output0CropStartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_h_Output0CropStartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerBResetValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_LSB,(u8)(layerParamBackUp.layerBWidth));
			FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_MSB,(u8)((layerParamBackUp.layerBWidth) >> 8));
			FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_LSB,(u8)(layerParamBackUp.layerBHeight));
			FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_MSB,(u8)((layerParamBackUp.layerBHeight) >> 8));
			FPGA_set(FPGA_REG_h_Output1Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerBXstart));
			FPGA_set(FPGA_REG_h_Output1Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerBXstart) >> 8));
			FPGA_set(FPGA_REG_v_Output1Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerBVstart));
			FPGA_set(FPGA_REG_v_Output1Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerBVstart) >> 8));
			FPGA_set(FPGA_REG_h_Output0CropStartPos_LSB,(u8)(layerParamBackUp.layerACropStart));
			FPGA_set(FPGA_REG_h_Output0CropStartPos_MSB,(u8)((layerParamBackUp.layerACropStart) >> 8));
			memcpy((u8*)&layerParam+14,(u8*)&layerParamBackUp+14,14);

			layerParam.layerACropStart = layerParamBackUp.layerACropStart;
			LCD_WriteStr_fb2(1,22, "   OK  ");
			SysDelay1ms(1000);
		}
	break;

	

	case layerCHSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerCWidth = value;
			if(GetPanleAndSplitParamRam()->_4P_ThirdDisplayWidth <= 1920)
			{
				FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			else
			{
				FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerCVSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerCHeight = value;
			if(GetPanleAndSplitParamRam()->_4P_ThirdDisplayHeight <= 1080)
			{
			}
			else
			{
				FPGA_set(FPGA_REG_v_Output2Layer_A_HeightPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_v_Output2Layer_A_HeightPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerCHPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerCXstart = value;
			FPGA_set(FPGA_REG_h_Output2Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_h_Output2Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerCVPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerCVstart = value;
			FPGA_set(FPGA_REG_v_Output2Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_v_Output2Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerCResetValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_LSB,(u8)(layerParamBackUp.layerCWidth));
			FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_MSB,(u8)((layerParamBackUp.layerCWidth) >> 8));
			FPGA_set(FPGA_REG_v_Output2Layer_A_HeightPos_LSB,(u8)(layerParamBackUp.layerCHeight));
			FPGA_set(FPGA_REG_v_Output2Layer_A_HeightPos_MSB,(u8)((layerParamBackUp.layerCHeight) >> 8));
			FPGA_set(FPGA_REG_h_Output2Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerCXstart));
			FPGA_set(FPGA_REG_h_Output2Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerCXstart) >> 8));
			FPGA_set(FPGA_REG_v_Output2Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerCVstart));
			FPGA_set(FPGA_REG_v_Output2Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerCVstart) >> 8));
			memcpy((u8*)&layerParam+28,(u8*)&layerParamBackUp+28,14);
			LCD_WriteStr_fb2(0,22, "   OK  ");
			SysDelay1ms(1000);
		}
	break;

	case layerDHSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerDWidth = value;
			if(GetPanleAndSplitParamRam()->_4P_FourthDisplayWidth <= 1920)
			{
				FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			else
			{
				FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerDVSizeValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerDHeight = value;
			if(GetPanleAndSplitParamRam()->_4P_FourthDisplayHeight <= 1080)
			{
			}
			else
			{
				FPGA_set(FPGA_REG_v_Output3Layer_A_HeightPos_LSB,(u8)value);
				FPGA_set(FPGA_REG_v_Output3Layer_A_HeightPos_MSB,(u8)(value >> 8));
			}
			
		}
	break;
	case layerDHPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerDXstart = value;
			FPGA_set(FPGA_REG_h_Output3Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_h_Output3Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerDVPosValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerDVstart = value;
			FPGA_set(FPGA_REG_v_Output3Layer_A_StartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_v_Output3Layer_A_StartPos_MSB,(u8)(value >> 8));
		}
	break;

	case layerDFineValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			layerParam.layerBCropStart = value;
			FPGA_set(FPGA_REG_h_Output1CropStartPos_LSB,(u8)value);
			FPGA_set(FPGA_REG_h_Output1CropStartPos_MSB,(u8)(value >> 8));
		}
	break;
	case layerDResetValue:
		if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB ||
			GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
			GetPgmCurrentDisplayMode() == _OSD_DM_4P
		  )
		{
			FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_LSB,(u8)(layerParamBackUp.layerDWidth));
			FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_MSB,(u8)((layerParamBackUp.layerDWidth) >> 8));
			FPGA_set(FPGA_REG_v_Output3Layer_A_HeightPos_LSB,(u8)(layerParamBackUp.layerDHeight));
			FPGA_set(FPGA_REG_v_Output3Layer_A_HeightPos_MSB,(u8)((layerParamBackUp.layerDHeight) >> 8));
			FPGA_set(FPGA_REG_h_Output3Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerDXstart));
			FPGA_set(FPGA_REG_h_Output3Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerDXstart) >> 8));
			FPGA_set(FPGA_REG_v_Output3Layer_A_StartPos_LSB,(u8)(layerParamBackUp.layerDVstart));
			FPGA_set(FPGA_REG_v_Output3Layer_A_StartPos_MSB,(u8)((layerParamBackUp.layerDVstart) >> 8));
			FPGA_set(FPGA_REG_h_Output1CropStartPos_LSB,(u8)(layerParamBackUp.layerBCropStart));
			FPGA_set(FPGA_REG_h_Output1CropStartPos_MSB,(u8)((layerParamBackUp.layerBCropStart) >> 8));
			memcpy((u8*)&layerParam + 42,(u8*)&layerParamBackUp + 42,14);
			layerParam.layerBCropStart = layerParamBackUp.layerBCropStart;
			LCD_WriteStr_fb2(1,22, "   OK  ");
			SysDelay1ms(1000);
		}
		
	break;
	case outputPort0TpyeValue:
		GetOutputTypeParamRam()->output0Type = value;
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_3,value);
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_4,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort1TpyeValue:
		GetOutputTypeParamRam()->output1Type = value;
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_1,value);
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_2,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort2TpyeValue:
		GetOutputTypeParamRam()->output2Type = value;
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_5,value);
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_6,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort3TpyeValue:
		GetOutputTypeParamRam()->output3Type = value;
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_7,value);
		SiI9134_SetOutputIsHDMI(SiI9134_PORT_8,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	
	case outputPort0BitWidthValue:
		GetOutputTypeParamRam()->output0BitWidth = value;
		SiI9134_SetBitDepth(SiI9134_PORT_3,value);
		SiI9134_SetBitDepth(SiI9134_PORT_4,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort1BitWidthValue:
		GetOutputTypeParamRam()->output1BitWidth = value;
		SiI9134_SetBitDepth(SiI9134_PORT_1,value);
		SiI9134_SetBitDepth(SiI9134_PORT_2,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort2BitWidthValue:
		GetOutputTypeParamRam()->output2BitWidth = value;
		SiI9134_SetBitDepth(SiI9134_PORT_5,value);
		SiI9134_SetBitDepth(SiI9134_PORT_6,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort3BitWidthValue:
		GetOutputTypeParamRam()->output3BitWidth = value;
		SiI9134_SetBitDepth(SiI9134_PORT_7,value);
		SiI9134_SetBitDepth(SiI9134_PORT_8,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	
	case outputPort0RangeValue:
		GetOutputTypeParamRam()->output0Range = value;
		SiI9134_SetDataRange(SiI9134_PORT_3,value);
		SiI9134_SetDataRange(SiI9134_PORT_4,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort1RangeValue:
		GetOutputTypeParamRam()->output1Range = value;
		SiI9134_SetDataRange(SiI9134_PORT_1,value);
		SiI9134_SetDataRange(SiI9134_PORT_2,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort2RangeValue:
		GetOutputTypeParamRam()->output2Range = value;
		SiI9134_SetDataRange(SiI9134_PORT_5,value);
		SiI9134_SetDataRange(SiI9134_PORT_6,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case outputPort3RangeValue:
		GetOutputTypeParamRam()->output3Range = value;
		SiI9134_SetDataRange(SiI9134_PORT_7,value);
		SiI9134_SetDataRange(SiI9134_PORT_8,value);
		SaveOutputTypeParam();
		SysDelay1ms(100);
		LoadOutputTypeParam();
	break;
	case scalePosxValue:
		SetScalerPosSize(index,value);
		SetPipPosAndSize(_DISPLAY_RGN_MAIN,W_PIP_SUB_POS_X,GetScalerPosSize(index) + GetFirstRegionWidthStart());
	break;
	case scalePosyValue:
		SetScalerPosSize(index,value);
		SetPipPosAndSize(_DISPLAY_RGN_MAIN,W_PIP_SUB_POS_Y,GetScalerPosSize(index) + GetFirstRegionHeightStart());
	break;
	case scaleWidthValue:
		SetScalerPosSize(index,value);
		SetPipPosAndSize(_DISPLAY_RGN_MAIN,W_PIP_SUB_SIZE_X,GetScalerPosSize(index));
	break;
	case scaleHeightValue:
		SetScalerPosSize(index,value);
		SetPipPosAndSize(_DISPLAY_RGN_MAIN,W_PIP_SUB_SIZE_Y,GetScalerPosSize(index));
	break;
	default:
		if((0 <= index) && (index < maxSizeValue))
		{
			valueList[index%4] = value;
		}
	break;
    }
	return value;
}


int nextValue(int index, int value)
{

	switch(index)
	{
		case naValue:
		case scaleModeValue:			
		value = 0;  
		break;  
		case licenseStateValue:
		case autoFitValue:
		case screenModeValue:
		case screenFollowValue:
		case dviSelectValue:
		case vgaSelectValue:
		case deOnOffValue:
		case dataRangeValue:	
		case freezeValue:
		case swapMainPipValue:
//		case pipModeValue:	
#if(CURRENT_DEVICE == _VIP_SMART)
		case languageValue:
			value++;
			if(value == 2)
				value = 0;
		break;
#endif
		case currentWindowValue:
		case muteOnOffValue:
		case hdmiAudioValue:
		case dviAndExtAudioValue:
		case hdmiAndExtAudioValue:
		case audioTypeValue:
		case audioModeValue:
		case keyLockValue:
		case networkEnableValue:
		case dhcpEnableValue:
		case adcAutoAdjustValue:
		case dviHdmiValue:
		case lockSourceValue:
		case timingControlValue:
		case textModeValue:
		case textAboveValue:
		case textAndOrValue:
		case textEnableValue:
		case splitOnOffValue:
		case splitSyncModeValue:
		case interlaceSwitchValue:
		case vga1KeyModeValue:
		case vga2KeyModeValue:
		case deHPalarityValue:
		case deVPalarityValue:
		case imageEnhanceValue:
		case hotBackupEnableValue:
		case mode4PSplitParamSendValue:
			if(value)
			{
				value = 0;
			}
			else
			{
				value = 1;
			}
		break;
		case autoSpeedValue:
		case realTimeSaveValue:
		case switchTypeValue:
		case previewDblTapSwitchValue:	
		case previewSwitchValue:	
	    case previewDVISwitchValue:
		case extInputComfigValue:
		case dvi1OutputValue:
		case dvi1InputValue:
		case choosecard12value:
		case hdmiColorSpaceAdjustValue:
		case temperatureModeValue: 
		case sendCardPortSetWidthValue:
		case sendCardPort1WidthValue:       
		case sendCardPort2WidthValue:      
		case sendCardPort3WidthValue:       
		case sendCardPort4WidthValue:       
		break;
		case temperatureValue:
		break;
		case scaleKeyTypeValue:		
		case mapKeyTypeValue:
		break;
		case artNetStartAddrValue:
		break;
		case artNetUniverseValue:
		break;			
#if !(CURRENT_DEVICE == _VIP_SMART)
		case languageValue:
			value++;
			if(value == 2)
				value = 0;
		break;
#endif	
		case sendCardTypeValue:
		break;
		case blackKeyTypeValue:
		break;
		case bitDepthValue:
			if(getValue(dviHdmiValue))
			{
				value++;
				if(value > 2)
				{
					value = 2;
				}
			}
		break;
			
		case switchModeValue:       
		break;
		case gammaValue:
			value++;
	        if(value > (sizeof(gammaListStr)/sizeof(gammaListStr[0]) - 1))
	        {
	            value = (sizeof(gammaListStr)/sizeof(gammaListStr[0]) - 1);
	        }
	    break;
		case lockModeValue:
		break;	
	    case inputFormatValue:
	    break;
	    case outputFormatValue:
	        {
	    	}
    	break;
		case standardFormatValue:
	        {
	    	}
    	break;
		case LEDoutputFormatValue:		
		break;
		case screenPosxValue:
		break;
		case screenPosyValue:
		break;
	    case sendCardSetWidthHValue:    
		case dePosxValue:
		case deWidthValue:
		break;
	    case sendCardSetWidthVValue:
//		case scaleHeightValue:
		case screenHeightValue:
		case dePosyValue:
		case deHeightValue:
		break;		
		case delayCallSaveValue:
		break;
		case zoomWidthValue:
		case zoomHeightValue:
		case zoomPosxValue:
		case zoomPosyValue:
		break;
		case cropInputWidthValue:
		break;
		case cropInputHeightValue:
		break;
		case cropInputPosxValue:
		break;
		case cropInputPosyValue:
		break;	
		case zoomUpValue:
		case zoomDownValue:
		break;
		case zoomLeftValue:
		case zoomRightValue:
		break;
		case zoomUpDownValue:
		case zoomLeftRightValue:
		case zoomCenterValue:
		break;
		case cropUpValue:
		break;		
		case cropDownValue:
		break;		
		case cropUpDownValue:
		case cropCenterValue:
		break;		
		case cropLeftValue:
		break;
		case cropRightValue:
		break;
		case cropLeftRightValue:
		break;			
		case volumeValue:
		break;
		case gamma_sendcard_Value:		//1设置GAMMA
		break;
		case colorTempBlueValue:
		case colorTempGreenValue:
		case colorTempRedValue:
			value += step;
			if(value > 0xff)
				value = 0;
		break; 
		case colorTempValue:
			value += 1;
			if(value > 5)
			{
				value = 0;
			}
		break;
		case colorEffectValue:
			value += 1;
			if(value > 4)
			{
				value = 0;
			}
		break;
		case contrastValue:
			value += step;
			if(value > 4096)
				value = 0;
		break;
		case saturationValue:
		case brightnessValue:
			value += step;
			if(value > 1023)
				value = 0;
		break;
		case sharpnessValue:	   
		break;
		case fadeTimeValue:
			value += 5;
			if(value > 30)
			{
				value = 0;
			}
		break;
		case alphaValue:
			value += 1;
			if(value > 16)
			{
				value = 16;
			}
		break;
		case loadFromValue:
		case saveToValue:
		case usbSingleLoadValue:	
		break;
		case usbAllLoadValue:
		case usbAllSaveValue:
		break;
		case layoutValue:	
			value += 1;
			if(value > (LAYOUT_SIZE-1))
			{
				value = (LAYOUT_SIZE-1);
			}
		break;	
		case windowRatioValue:
		break;
		case displayModeValue:
		break;
		case flatBlueValue:
		case flatGreenValue:
		case flatRedValue:
		case ipAddress3Value:
		case tpColorBlueValue:
		case tpColorGreenValue:
		case tpColorRedValue:
        break;
		case licenseTimeValue:
		break;
    	case tpSwitchValue:
		break;       
		case testPatternModeValue:        
		break;
		case tpVerStepValue:
		break;
		case tpHorStepValue:
		break;
		case tpStepColorValue:
		break;
		case textAlphaValue:
		break;
		case textRedValue:
		case textBlueValue:
		break;
		case textGreenValue:
		break;
		case textPresetValue:
		break;
		case outputColorSpaceValue:
		break;
		case autoTpIimeValue:
		break;
		case customHValue:
		case customVValue:
			value += step;
			if(value > 3840)
			{
				value = 3840;
			}
		break;
		case customFValue:
			value += step;
			if(value > 75)
			{
				value = 75;
			}
		break;
		case luminanceValue:
			value++;
			if(value > MD_OLED_LUM_MAX)
			{
				value = MD_OLED_LUM_MAX;
			}
		break;
		case displayAlphaValue:
			value += step;
			if(value > 255)
			{
				value = 255;
			}
		break;
		case splitHTotalValue:
		case splitVTotalValue:
		case splitHPosValue:
		case splitVPosValue:
        break;
		case splitHSizeValue:
		case splitFixedHSizeValue:
        break;
		case splitVSizeValue:
		case splitFixedVSizeValue:
        break;	
		case ttlInterlaveModeValue:
		break;
		case hotBackupClass1Value:	
		case hotBackupClass2Value:	
		case hotBackupClass3Value:	
		case hotBackupClass4Value:	
		case hotBackupClass5Value:	
	//#ifdef HOT_BACKUP_ON
			//if(!getHotBackupEnable())
			//{
			//	break;
			//}
			value = getNextBackupCh((index - hotBackupClass1Value),getValue(index));
	//#endif
		break;	
		case edidSourcePortValue:
		break;
		case edidDetinationPortValue:
		break;
		case currentSpeedValue:
			value += 10;
			if(value > 90)
			{
				value = 90;
			}
		break;
		case splitFixedTypeValue:
		break;
		case splitFixedIndexValue:
		break;
		case splitFixedHNumberValue:
		case splitFixedVNumberValue:
		break;
		case audioInput1Value:
		case audioInput2Value:
		case audioInput3Value:		
		break;
		case xposvalue: 	//chagned by @cc20140726
		break;
		case yposvalue: //chagned by @cc20140726
		break;
		case vlinevalue:
		case sendCardVPanelsValue:
		case hlinevalue:
		case sendCardHPanelsValue:	
		case hlinevalue11:
		case hlinevalue12:
		case vlinevalue11:
		case vlinevalue12:
		break;
		case sendCardWallHPanelsValue:				
		case sendCardWallVPanelsValue:        
		case vsizevalue1:
		case vsizevalue2:
		case hsizevalue1:
		case hsizevalue2:
		case vsizevalue:
		case sendCardVLedsValue:	
		case hsizevalue:
		case sendCardHLedsValue:
		case sendCardWallHLedsValue:
		case sendCardWallVLedsValue:        
		break;   
	 	case networkchoosevalue:
		break;
		case sendCardPortSelValue:
		break;		
		case receivecard_number_value:
		case sendCardPanelNoValue:
		break;
		case xpos_receive_value:
		case sendCardPanelHOffsetValue:
		case ypos_receive_value:
		case sendCardPanelVOffsetValue:
		break;
		case RedColorTemSet_Value:
		case GreenColorTemSet_Value:
		case BlueColorTemSet_Value:
		break;
		case xoffsetvalue:
		case sendCardPortHPosValue:
		case yoffsetvalue:
		case sendCardPortVPosValue:
		break;
	 	case widthValue:
		break;
	 	case heightValue:
		break;
	 	case connectvalue:
	 	case sendCardPanelConectValue:
		break;
   	 	case sendCardWallConectValue:
        break;           
		case cascadevalue:
		break;	
		case brightness_sendcard_value:
		break;
		case sendcardchoosevalue:
		break;
		case cardoutputFormatValue:
		break;
#if ENABLE_TEST_GRID_CONTROL
	    case GridPortValue:       
	    break;
		case GridTestValue:
		case gridBackgroudVaule:
	 	case gridDotValue:	       
		break;		
		case gridModeValue:
		break;
		case gridColorVaule:
		break;
    	case gridDotColorValue:
		break;        	
		case gridHSpaceVaule:
		case gridVSpaceVaule:
		case gridDotSpeedValue:
		case gridDotColorRValue:
		case gridDotColorGValue:
		case gridDotColorBValue:	        
        break;
		case gridDotPosXValue:  
        break;
		case gridDotPosYValue:	 
		break;
#endif	
		case videoPanelFWSaveValue:	
		break;
		case usbUpdateConfirmValue:
		break;	
		case WriteSnValue:
			value += step;
			if(value > 9999)
	        {
	            value = 0;
	        }
		break;
		
		case _4pLTSourceValue:	
		case _4pLBSourceValue:	
		case _4pRTSourceValue:	
		case _4pRBSourceValue:	
		case pgmSourceValue:
		case prvSourceValue:
		case pipSourceValue:
			value += 1;
	        if(value > INPUT_SOURCE_MAX)
	        {
	            value = 0;
	        }
		break;

		case totalWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive*2)
	        {
	            value = tVideoDefineCustom.hActive*2;
	        }
		break;
		case totalHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive*2)
	        {
	            value = tVideoDefineCustom.vActive*2;
	        }
		break;
		case firstDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case firstDisHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case secondDisHeighValue:
		case thirdDisWidthValue:
		case thirdDisHeighValue:
		case fourDisWidthValue:
		case fourDisHeighValue:
		break;

		case h12TotalWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive*2)
		    {
		        value = tVideoDefineCustom.hActive*2;
		    }
		break;
		case h12TotalHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case h12FirstDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case h12FirstDisHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case h12SecondDisWidthValue:
		case h12SecondDisHeighValue:
		case h12ThirdDisWidthValue:
		case h12ThirdDisHeighValue:
		case h12FourDisWidthValue:
		case h12FourDisHeighValue:
		break;

		case v12TotalWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case v12TotalHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive*2)
		    {
		        value = tVideoDefineCustom.vActive*2;
		    }
		break;
		case v12FirstDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case v12FirstDisHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case v12SecondDisWidthValue:
		case v12SecondDisHeighValue:
		case v12ThirdDisWidthValue:
		case v12ThirdDisHeighValue:
		case v12FourDisWidthValue:
		case v12FourDisHeighValue:
		break;

		case h14TotalWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive*4)
		    {
		        value = tVideoDefineCustom.hActive*4;
		    }
		break;
		case h14TotalHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case h14FirstDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case h14SecondDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case h14ThirdDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case h14SecondDisHeighValue:
		case h14FirstDisHeighValue:
		case h14ThirdDisHeighValue:
		case h14FourDisWidthValue:
		case h14FourDisHeighValue:
		break;

		case hor12TotalWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive*2)
		    {
		        value = tVideoDefineCustom.hActive*2;
		    }
		break;
		case hor12TotalHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case hor12FirstDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case hor12FirstDisHeighValue:
		case hor12SecondDisWidthValue:
		case hor12SecondDisHeighValue:
		case hor12ThirdDisWidthValue:
		case hor12ThirdDisHeighValue:
		case hor12FourDisWidthValue:
		case hor12FourDisHeighValue:
		break;

		case ver12TotalWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case ver12TotalHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive*2)
		    {
		        value = tVideoDefineCustom.vActive*2;
		    }
		break;
		case ver12FirstDisHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case ver12FirstDisWidthValue:
		case ver12SecondDisWidthValue:
		case ver12SecondDisHeighValue:
		case ver12ThirdDisWidthValue:
		case ver12ThirdDisHeighValue:
		case ver12FourDisWidthValue:
		case ver12FourDisHeighValue:
		break;

		case h13TotalWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive*3)
		    {
		        value = tVideoDefineCustom.hActive*3;
		    }
		break;
		case h13TotalHeighValue:
			value += step;
			if(value > tVideoDefineCustom.vActive)
		    {
		        value = tVideoDefineCustom.vActive;
		    }
		break;
		case h13FirstDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case h13SecondDisWidthValue:
			value += step;
			if(value > tVideoDefineCustom.hActive)
		    {
		        value = tVideoDefineCustom.hActive;
		    }
		break;
		case h13FirstDisHeighValue:
		case h13SecondDisHeighValue:
		case h13ThirdDisWidthValue:
		case h13ThirdDisHeighValue:
		case h13FourDisWidthValue:
		case h13FourDisHeighValue:
		break;	


		case mode4PTotalWidthValue:
		break;
		case mode4PTotalHeighValue:
		break;
		case mode4PFirstDisWidthValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case mode4PFirstDisHeighValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;
		case mode4PSecondDisWidthValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case mode4PSecondDisHeighValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;
		case mode4PThirdDisWidthValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case mode4PThirdDisHeighValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;
		case mode4PFourDisWidthValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case mode4PFourDisHeighValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;
	
		case userAlphaValue:
			value += step;
	        if(value > 255)
	        {
	            value = 255;
	        }
		break;
		case outputPort0Value:
		case outputPort1Value:
		case outputPort2Value:	
		case outputPort3Value:
			value++;//= step;
	        if(value > 3)
	        {
	            value = 0;
	        }
		break;

		case _4pLTWValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case _4pLTHValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;

		case _4pLBWValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case _4pLBHValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;

		case _4pRTWValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case _4pRTHValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;

		case _4pRBWValue:
			value += step;
			if(value >= tVideoDefineCustom.hActive)
				value = tVideoDefineCustom.hActive;
		break;
		case _4pRBHValue:
			value += step;
			if(value >= tVideoDefineCustom.vActive)
				value = tVideoDefineCustom.vActive;
		break;

		
		case layerAHSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.hActive - layerParam.layerAXstart))
				value = (tVideoDefineCustom.hActive - layerParam.layerAXstart);
		break;
		case layerAVSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.vActive - layerParam.layerAVstart))
				value = (tVideoDefineCustom.vActive - layerParam.layerAVstart);
		break;
		case layerAHPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.hActive - layerParam.layerAWidth))
				value = (tVideoDefineCustom.hActive - layerParam.layerAWidth);
		break;
		case layerAVPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.vActive - layerParam.layerAHeight))
				value = (tVideoDefineCustom.vActive - layerParam.layerAHeight);
		break;
		case layerASplitStartValue:
			value += 2;
		break;
		case layerACropStartValue:
			value += 2;
		break;
		case layerBHSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.hActive - layerParam.layerBXstart))
				value = (tVideoDefineCustom.hActive - layerParam.layerBXstart);
		break;
		case layerBVSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.vActive - layerParam.layerBVstart))
				value = (tVideoDefineCustom.vActive - layerParam.layerBVstart);
		break;
		case layerBHPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.hActive - layerParam.layerBWidth))
				value = (tVideoDefineCustom.hActive - layerParam.layerBWidth);
		break;
		case layerBVPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.vActive - layerParam.layerBHeight))
				value = (tVideoDefineCustom.vActive - layerParam.layerBHeight);
		break;
		case layerBSplitStartValue:
			value += 2;
		break;
		case layerBCropStartValue:
			value += 2;
		break;
		case layerBFineValue:
			value += 2;
		break;
		case layerCHSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.hActive - layerParam.layerCXstart))
				value = (tVideoDefineCustom.hActive - layerParam.layerCXstart);
		break;
		case layerCVSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.vActive - layerParam.layerCVstart))
				value = (tVideoDefineCustom.vActive - layerParam.layerCVstart);
		break;
		case layerCHPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.hActive - layerParam.layerCWidth))
				value = (tVideoDefineCustom.hActive - layerParam.layerCWidth);
		break;
		case layerCVPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.vActive - layerParam.layerCHeight))
				value = (tVideoDefineCustom.vActive - layerParam.layerCHeight);
		break;
		case layerCSplitStartValue:
			value += 2;
		break;
		case layerCCropStartValue:
			value += 2;
		break;
		case layerDHSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.hActive - layerParam.layerDXstart))
				value = (tVideoDefineCustom.hActive - layerParam.layerDXstart);
		break;
		case layerDVSizeValue:
			value += step;
			if(value > (tVideoDefineCustom.vActive - layerParam.layerDVstart))
				value = (tVideoDefineCustom.vActive - layerParam.layerDVstart);
		break;
		case layerDHPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.hActive - layerParam.layerDWidth))
				value = (tVideoDefineCustom.hActive - layerParam.layerDWidth);
		break;
		case layerDVPosValue:
			value += step;
			if(value >= (tVideoDefineCustom.vActive - layerParam.layerDHeight))
				value = (tVideoDefineCustom.vActive - layerParam.layerDHeight);
		break;
		case layerDSplitStartValue:
			value += 2;
		break;
		case layerDCropStartValue:
			value += 2;
		break;
		case layerDFineValue:
			value += 2;
		break;
		case outputPort0TpyeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		case outputPort1TpyeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		case outputPort2TpyeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		case outputPort3TpyeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		
		case outputPort0BitWidthValue:
			value++;
			if(value > 2)
				value = 0;
		break;
		case outputPort1BitWidthValue:
			value++;
			if(value > 2)
				value = 0;
		break;
		case outputPort2BitWidthValue:
			value++;
			if(value > 2)
				value = 0;
		break;
		case outputPort3BitWidthValue:
			value++;
			if(value > 2)
				value = 0;
		break;
		
		case outputPort0RangeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		case outputPort1RangeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		case outputPort2RangeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		case outputPort3RangeValue:
			value++;
			if(value > 1)
				value = 0;
		break;
		case scalePosxValue:
			gInputFlag = 1;
			value += step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value >= (GetPipParam()->gPipMainWinSizeX - GetPipParam()->gPipSubWinSizeX))
					value = 0;
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= GET_PBP_LEFT_SIZE_X())
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						//if(value >= (GET_PBP_RIGHT_SIZE_X() + GET_PBP_LEFT_SIZE_X()))
						if(value >= ((1920 - GET_PBP_RIGHT_SIZE_X()) + 1920))
							value = GET_PBP_LEFT_SIZE_X();
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= GET_PBP_TOP_SIZE_X())
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value >= GET_PBP_BOTTOM_SIZE_X())
							value = 0;
					}
				}
			}
		break;
		case scalePosyValue:
			gInputFlag = 1;
			value += step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value >= (GetPipParam()->gPipMainWinSizeY - GetPipParam()->gPipSubWinSizeY))
					value = 0;
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= (2160 - GET_PBP_LEFT_SIZE_Y()))
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value >= GET_PBP_RIGHT_SIZE_Y())
							value = GET_PBP_LEFT_SIZE_Y();
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= GET_PBP_TOP_SIZE_Y())
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value >= GET_PBP_BOTTOM_SIZE_Y())
							value = GET_PBP_TOP_SIZE_Y();
					}
				}
			}
		break;
		case scaleWidthValue:
			gInputFlag = 1;
			value += step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value >= 2048)
					value = 0;
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= stPipWinSizeBackUp.gPbpLeftWinSizeX)
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value >= 2048)
							value = 0;
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= stPipWinSizeBackUp.gPbpTopWinSizeX)
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value >= stPipWinSizeBackUp.gPbpBottomWinSizeX)
							value = 0;
					}
				}
			}
		break;
		case scaleHeightValue:
			gInputFlag = 1;
			value += step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value >= (GetPipParam()->gPipMainWinSizeY - GetPipParam()->gPipSubWinPosY))
					value = 0;
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= stPipWinSizeBackUp.gPbpLeftWinSizeY)
							value = stPipWinSizeBackUp.gPbpLeftWinSizeY/2;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value >= stPipWinSizeBackUp.gPbpRightWinSizeY)
							value = stPipWinSizeBackUp.gPbpRightWinSizeY/2;
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= stPipWinSizeBackUp.gPbpTopWinSizeY)
							value = stPipWinSizeBackUp.gPbpTopWinSizeY/2;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value >= stPipWinSizeBackUp.gPbpBottomWinSizeY)
							value = stPipWinSizeBackUp.gPbpBottomWinSizeY/2;
					}
				}
			}
		break;
	    default:
	        value += step;
	        if(value > 4096)
	        {
	            value = 4096;
	        }
	    break;
    }
    return value;
}

int prevValue(int index, int value)
{

    switch(index)
    {
 		case naValue:
		case scaleModeValue:			
			value = 0;  
		break;
	    case licenseStateValue:
		case autoFitValue:
	    case screenModeValue:
		case screenFollowValue:
		case dviSelectValue:
		case vgaSelectValue:
		case deOnOffValue:
		case dataRangeValue:	
		case freezeValue:
		case currentWindowValue:
		case swapMainPipValue:
//		case pipModeValue:
		case muteOnOffValue:
		case hdmiAudioValue:
		case dviAndExtAudioValue:
		case hdmiAndExtAudioValue:
		case audioTypeValue:
		case audioModeValue:
		case keyLockValue:	
		case networkEnableValue:
		case dhcpEnableValue:
		case adcAutoAdjustValue:
		case dviHdmiValue:
		case lockSourceValue:
		case timingControlValue:
		case textEnableValue:
		case textAboveValue:
		case textAndOrValue:
		case textModeValue:
		case splitOnOffValue:
		case splitSyncModeValue:
		case interlaceSwitchValue:
		case vga1KeyModeValue:
		case vga2KeyModeValue:
		case deHPalarityValue:
		case deVPalarityValue:
		case imageEnhanceValue:
		case hotBackupEnableValue:
		case mode4PSplitParamSendValue:
			if(value)
			{
				value = 0;
			}
			else
			{
				value = 1;
			}
		break;	
		case autoSpeedValue:
		case realTimeSaveValue:
		case switchTypeValue:
		case previewDblTapSwitchValue:	
		case previewSwitchValue:
        case previewDVISwitchValue:    
		case extInputComfigValue:
		case dvi1OutputValue:
		case dvi1InputValue:
		case choosecard12value:
		case hdmiColorSpaceAdjustValue:
		case temperatureModeValue: 
        case sendCardPortSetWidthValue:
     	case sendCardPort1WidthValue:       
    	case sendCardPort2WidthValue:      
    	case sendCardPort3WidthValue:       
    	case sendCardPort4WidthValue:              
		break;	
		case temperatureValue:
		break;		
		case artNetStartAddrValue:        
		break;
		case artNetUniverseValue:
		break;				
		case scaleKeyTypeValue:
		case mapKeyTypeValue:			
		break;
	#if (CURRENT_DEVICE == _VIP_SMART)
		case languageValue:
			value--;
			if(value < 0)
				value = 1;
		break;
	#endif		
		case inputFormatValue:
		case gammaValue:
		case alphaValue:
		case lockModeValue:
		case switchModeValue:
		case bitDepthValue:
		case blackKeyTypeValue:
		case sendcardchoosevalue:
		case sendCardTypeValue:
			value--;
			if(value < 0)
			{
				value = 0;
			}
	    break;
		case edidSourcePortValue:			
			value--;
			if(value < 0)
			{
				value = 0;
			}			
			if(value == EDID_SOURCE_OUTPUTVGA)
			{
				value = EDID_SOURCE_OUTPUT1;
			}
	    break;
		case edidDetinationPortValue:
		break;
		case ttlInterlaveModeValue:
		break;
    	case outputFormatValue:
		{
			
    	}
        break;
		case standardFormatValue:
		{
    	}
        break;
		case cardoutputFormatValue:
		case LEDoutputFormatValue:		
			value -= 1;
			if(value < 0)
			{
				value = 0;
			}
		break;

		case screenPosxValue:
		case screenPosyValue:
		case dePosxValue:
		case dePosyValue:
		break;
	    case sendCardSetWidthHValue:     
		case screenWidthValue:
		case deWidthValue:
		break;	
	    case sendCardSetWidthVValue:
		case screenHeightValue:
		case deHeightValue:
		break;
		case zoomWidthValue:
		case zoomUpValue:
		case zoomDownValue:
		case zoomUpDownValue:
		case zoomLeftValue:
		case zoomRightValue:
		case zoomLeftRightValue:
		case zoomCenterValue:	
		case cropInputWidthValue:
		case cropInputHeightValue:
		case cropInputPosxValue:
		case cropInputPosyValue:
		break;
		case zoomHeightValue:
		break;
		case zoomPosxValue:
		case zoomPosyValue:
		break;
		case cropUpValue:
		case cropDownValue:
		case cropUpDownValue:
		case cropLeftValue:
		case cropRightValue:
		case cropLeftRightValue:
		case cropCenterValue:
		break;
		case colorTempValue:
			value -= 1;
			if(value < 0)//100
			{
				value = 5;
			}        
	    break;
		case colorTempBlueValue:
		case colorTempGreenValue:
		case colorTempRedValue:
			value -= step;
			if(value < 0)
			{
				value = 0xff;
			} 
	    break;
		case colorEffectValue:
			value --;
			if(value < 0)
			{
				value = 4;
			}
		break;
		case contrastValue:	
			value -= step;
			if(value < 0)
			{
				value = 4096;
			}
		break;
		case saturationValue:
		case brightnessValue:
			value -= step;
			if(value < 0)
			{
				value = 1023;
			}
		case volumeValue:
		case delayCallSaveValue:
		break;
		case licenseTimeValue:
		break;
		case sharpnessValue:
			if(step > 10)
			{
				step = 10;
			}
			value -= step;
			if(value < 0)
			{
				value = 0;
			}    
		break;
		case vgaClockValue:
		case vgaPosxValue:
		case vgaPosyValue:
		case vgaPhaseValue:
		break;
		case fadeTimeValue:
			value -= 5;
			if(value < 0)
			{
				value = 30;
			}
		break;
  		case windowRatioValue:     
        break;
		case layoutValue:
			value -= 1;
			if(value < 0)
	        {
	            value = 0;
	        }
		case displayModeValue:
		break;
		case tpVerStepValue:
		break;
		case tpHorStepValue:
		break;
		case tpStepColorValue:
		break;
    	case tpSwitchValue:
		break;        
		case testPatternModeValue:
		break;
		case autoTpIimeValue:
		case textAlphaValue:
		case textPresetValue:
		break;
		case textRedValue:
		case textBlueValue:
		break;
		case textGreenValue:
		break;
		case outputColorSpaceValue:
			value -= 1;
			if(value < 0)
			{
				value = 0;
			}
		break;
		case customHValue:
		case customVValue:
		case customFValue:
			value -= step;
			if(value < 0)
			{
				value = 0;
			}
		break;	
		case luminanceValue:
			value--;
			if(value < MD_OLED_LUM_MIN)
			{
				value = MD_OLED_LUM_MIN;
			}
		break;
		case displayAlphaValue:
			value -= step;
			if(value <= 0)
			{
				value = 0;
			}
		break;
		case hotBackupClass1Value:	
		case hotBackupClass2Value:	
		case hotBackupClass3Value:	
		case hotBackupClass4Value:	
		case hotBackupClass5Value:	
			value = getPreBackupCh((index-hotBackupClass1Value),getValue(index));
		break;		
		case currentSpeedValue:
			value -= 10;
			if(value <= 0)
			{
				value = 0;
			}
		break;
		case splitFixedIndexValue:
		case splitFixedHNumberValue:
		case splitFixedVNumberValue:	
		break;
		case audioInput1Value:
		case audioInput2Value:
		case audioInput3Value:		
			value = getPreAudioInputCh((index - audioInput1Value),getValue(index));
		break;
		case xposvalue:
		case xsizevalue:
		case yposvalue:
		case ysizevalue:
		case xposvalue1:
		case xsizevalue1:
		case yposvalue1:
		case ysizevalue1:
			value -= step;
			if(value < 0)
			{
				value = 0;
			}
		break;
		case networkchoosevalue:
			if(value)
			{
				value = 0;
			}
			else
			{
				value = 1;
			}
		break;
		case sendCardPortSelValue:
			value--;
			if(value < 0)
			{
				value = 0;
			}
		break;			
		case receivecard_number_value:
		case sendCardPanelNoValue:
		break;
		case RedColorTemSet_Value:
		case GreenColorTemSet_Value:
		case BlueColorTemSet_Value:
		case xpos_receive_value:
		case sendCardPanelHOffsetValue:
		case ypos_receive_value:
		case sendCardPanelVOffsetValue:
		break;
		case hlinevalue12:
		case hlinevalue11:
		case vlinevalue11:
		case vlinevalue12:
		case vsizevalue1:
		case vsizevalue2:
		case hsizevalue1:
		case hsizevalue2:
		case vlinevalue:
		case sendCardVPanelsValue:	
		case hlinevalue:
		case sendCardHPanelsValue:
		case vsizevalue:
		case sendCardVLedsValue:		
		case hsizevalue:
		case sendCardHLedsValue:
	 	case sendCardWallHLedsValue:
		case sendCardWallVLedsValue:  
	 	case sendCardWallHPanelsValue:				
		case sendCardWallVPanelsValue:         
		break;
		case heightValue:
		case widthValue:
		case xoffsetvalue:
		case sendCardPortHPosValue:
		case yoffsetvalue:
		case sendCardPortVPosValue:	
		break;
		case connectvalue:
		case sendCardPanelConectValue:
	    case sendCardWallConectValue:
		break;
		case cascadevalue:
		case gamma_sendcard_Value:		//1设置GAMMA
		break;
		case brightness_sendcard_value:
		break;	
    	case GridPortValue:     
        break;
		case GridTestValue:
		case gridBackgroudVaule:
		case gridDotValue:        
		break;	
		case gridModeValue:
		case gridColorVaule:
		break;
		case gridHSpaceVaule:
		case gridVSpaceVaule:
		case gridDotSpeedValue:
	    case gridDotColorValue:    
		case gridDotColorRValue:
		case gridDotColorGValue:
		case gridDotColorBValue:
		case gridDotPosXValue:
		case gridDotPosYValue:	
		break;		
		case videoPanelFWSaveValue:	
		break;
		case usbUpdateConfirmValue:
		break;	
		case usbSingleLoadValue:	
		case usbAllLoadValue:
		case usbAllSaveValue:
		break;	
		case _4pLTSourceValue:	
		case _4pLBSourceValue:	
		case _4pRTSourceValue:	
		case _4pRBSourceValue:	
		case pgmSourceValue:
		case prvSourceValue:
		case pipSourceValue:
			value -= 1;
	        if(value < 0)
	        {
	            value = INPUT_SOURCE_MAX;
	        }
		break;
		case outputPort0Value:
		case outputPort1Value:
		case outputPort2Value:	
		case outputPort3Value:
			value --;
	        if(value < 0)
	        {
	            value = 3;
	        }
		break;
		case layerAHSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerAVSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerAHPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerAVPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerASplitStartValue:
			value-=2;
			if(value < 0)
				value = 0;
		break;
		case layerACropStartValue:
			value -= 2;
			if(value < 0)
				value = 0;
		break;
		case layerBHSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerBVSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerBHPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerBVPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerBSplitStartValue:
			value-=2;
			if(value < 0)
				value = 0;
		break;
		case layerBCropStartValue:
			value -= 2;
			if(value < 0)
				value = 0;
		break;
		case layerBFineValue:
			value -= 2;
			if(value < 0)
				value = 0;
		break;


		case layerCHSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerCVSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerCHPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerCVPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerCSplitStartValue:
			value-=2;
			if(value < 0)
				value = 0;
		break;
		case layerCCropStartValue:
			value -= 2;
			if(value < 0)
				value = 0;
		break;


		case layerDHSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerDVSizeValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerDHPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerDVPosValue:
			value-=step;
			if(value < 0)
				value = 0;
		break;
		case layerDSplitStartValue:
			value-=2;
			if(value < 0)
				value = 0;
		break;
		case layerDCropStartValue:
			value -= 2;
			if(value < 0)
				value = 0;
		break;
		case layerDFineValue:
			value -= 2;
			if(value < 0)
				value = 0;
		break;


		case outputPort0TpyeValue:
			value--;
			if(value < 0)
				value = 1;
		break;
		case outputPort1TpyeValue:
			value--;
			if(value < 0)
				value = 1;
		break;
		case outputPort2TpyeValue:
			value--;
			if(value < 0)
				value = 1;
		break;
		case outputPort3TpyeValue:
			value--;
			if(value < 0)
				value = 1;
		break;
		
		case outputPort0BitWidthValue:
			value--;
			if(value < 0)
				value = 2;
		break;
		case outputPort1BitWidthValue:
			value--;
			if(value < 0)
				value = 0;
		break;
		case outputPort2BitWidthValue:
			value--;
			if(value < 0)
				value = 2;
		break;
		case outputPort3BitWidthValue:
			value--;
			if(value < 0)
				value = 2;
		break;
		
		case outputPort0RangeValue:
			value--;
			if(value < 0)
				value = 1;
		break;
		case outputPort1RangeValue:
			value--;
			if(value < 0)
				value = 1;
		break;
		case outputPort2RangeValue:
			value--;
			if(value < 0)
				value = 1;
		break;
		case outputPort3RangeValue:
			value--;
			if(value < 0)
				value = 1;
		break;

		case WriteSnValue:
			value -= step;
			if(value < 0)
				value = 0;
		break;
		case scalePosxValue:
			gInputFlag = 1;
			value -= step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value < 0)
					value = (GetPipParam()->gPipMainWinSizeX - GetPipParam()->gPipSubWinSizeX);
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < GET_PBP_LEFT_SIZE_X())
							value = GET_PBP_LEFT_SIZE_X();
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < 0)
							value = 0;
					}
				}
			}
		break;
		case scalePosyValue:
			gInputFlag = 1;
			value -= step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value < 0)
					value = (GetPipParam()->gPipMainWinSizeY - GetPipParam()->gPipSubWinSizeY);
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < 0)
							value = (2160 - GET_PBP_LEFT_SIZE_Y());
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = 0;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < GET_PBP_TOP_SIZE_X())
							value = GET_PBP_TOP_SIZE_X();
					}
				}
			}
		break;
		case scaleWidthValue:
			gInputFlag = 1;
			value -= step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value < 0)
					value = (GetPipParam()->gPipMainWinSizeX - GetPipParam()->gPipSubWinPosX);
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpLeftWinSizeX;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpRightWinSizeX;
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpTopWinSizeX;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpBottomWinSizeX;
					}
				}
			}
		break;
		case scaleHeightValue:
			gInputFlag = 1;
			value -= step;
			if(GetSystemParam()->PipLayout < LAYOUT_PBP_LR)
			{
				if(value < 0)
					value = (GetPipParam()->gPipMainWinSizeY - GetPipParam()->gPipSubWinPosY);
			}
			else
			{
				if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpLeftWinSizeY;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpRightWinSizeY;
					}
				}
				else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
				{
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpTopWinSizeY;
					}
					else if(GetSystemParam()->CurrentWindow == IMAGE_B)
					{
						if(value < 0)
							value = stPipWinSizeBackUp.gPbpBottomWinSizeY;
					}
				}
			}
		break;
		
	    default:
	        value -= step;
	        if(value < 0)
	        {
	            value = 0;
	        }
		break;
    }
    return value;

}
int getUpLimit(int index)
{
	int result;
	
	switch(index)
	{
		case licenseStateValue:
		case autoFitValue:
	    case screenModeValue:
		case dviSelectValue:
		case vgaSelectValue:
		case deOnOffValue:
		case dataRangeValue:	
		case freezeValue:
		case currentWindowValue:
		case swapMainPipValue:
//		case pipModeValue:
		case languageValue:
		case muteOnOffValue:
		case hdmiAudioValue:
		case audioModeValue:
		case keyLockValue:
		case networkEnableValue:
		case adcAutoAdjustValue:
		case dviHdmiValue:
		case lockSourceValue:
		case timingControlValue:
		case textModeValue:
		case textAboveValue:
		case textAndOrValue:
		case textEnableValue:
		case splitOnOffValue:
		case splitSyncModeValue:
		case interlaceSwitchValue:
		case vga1KeyModeValue:
		case vga2KeyModeValue:
		case imageEnhanceValue:
			result = 1;
			break;
			
		case switchModeValue:
			result =  (int)(SWITCH_MODE_CUT);//(SWITCH_MODE_SIZE - 1);
			break;
				
		case gammaValue:
			result =  (int)(sizeof(gammaListStr)/sizeof(gammaListStr[0]) - 1);
			break;
			
		case lockModeValue:
			result = 2;
			break;
			
	    case inputFormatValue:
	        	result =  (int)MD_INDEX_SIZE;
			break;
	    case outputFormatValue:
			result =  (int)( outFormatList[sizeof(outFormatList)-1]);
			break;
			
		case standardFormatValue:
			result =  (int)( outFormatList[sizeof(outFormatList)-2]);
			break;
				
		case screenPosxValue:
			break;
		case screenPosyValue:
			break;
        case sendCardSetWidthHValue:   
		case screenWidthValue:
		case dePosxValue:
		case deWidthValue:
		break;
        case sendCardSetWidthVValue:
		case screenHeightValue:
		case dePosyValue:
		case deHeightValue:
		case delayCallSaveValue:
			result =  (int) 255;
		break;
		
		case zoomWidthValue:
		case zoomHeightValue:
		case zoomPosxValue:
		case zoomPosyValue:
		case zoomUpValue:
		case zoomDownValue:
		case zoomUpDownValue:
		case zoomLeftValue:
		case zoomRightValue:
		case zoomLeftRightValue:
		case zoomCenterValue:
			result =  (int)  200;
		break;
		case volumeValue:
			result =  (int)  100;
		break;
		case colorTempValue:
            result =  (int) 4;
        break;            
		case colorTempBlueValue:
		case colorTempGreenValue:
		case colorTempRedValue:
            result =  (int) 255;
        break;
		case contrastValue:
		case saturationValue:
		case brightnessValue:
			result =  (int) 100;
		break;
		case sharpnessValue:		
			result =  (int) 100;
		break;

		case fadeTimeValue:
			result =  (int) 30;
		break;

		case alphaValue:
			result =  (int) 16;
		break;
		case usbSingleLoadValue:	
		case usbAllLoadValue:
		case usbAllSaveValue:
		case loadFromValue:
		case saveToValue:
			result =  (int) (USER_SAVE_MAX_SIZE -1);
		break;

		case layoutValue:		
			result =  (int) (LAYOUT_SIZE -1);
		break;
		case pipSourceValue:
			result = (int)6;
		break;

		case windowRatioValue:
			result =  (int) 1;//2;
		break;

		case displayModeValue:
			result =  (int) 4;
		break;

		case flatBlueValue:
		case flatGreenValue:
		case flatRedValue:
		case ipAddress3Value:
		case tpColorBlueValue:
		case tpColorGreenValue:
		case tpColorRedValue:
			result =  (int)  255;
		break;

		case licenseTimeValue:
			result =  (int) (30*24*60);
		break;

		case testPatternModeValue:
            result =  (int) 20;
		break;

		case tpVerStepValue:
			result =  (int) 64;
		break;

		case tpHorStepValue:
			result =  (int) 64;
		break;

		case tpStepColorValue:
			result =  (int) ( tpStepColorList[sizeof(tpStepColorList) - 1]);
		break;

		case textAlphaValue:
			result =  (int)  15;
		break;

		case textRedValue:
		case textBlueValue:
			result =  (int) ( 255&0xf8);
		break;

		case textGreenValue:
			result =  (int)  (255>>2<<2);
		break;

		case textPresetValue:
			result =  (int) 12;
		break;

		case outputColorSpaceValue:
			result =  (int) 2;

		case autoTpIimeValue:
			result =  (int) 10;

		case customHValue:
		case customVValue:
			result =  (int) 5000;
		break;

		case customFValue:
			result =  (int) 99;
		break;

		case luminanceValue:
			result =  (int) (MD_OLED_LUM_MAX);
		break;

	#if ENABLE_TEST_GRID_CONTROL
		case gridBackgroudVaule:
		case GridTestValue:
 		case gridDotValue:           
			result =  (int) 1;
		break;
		case gridModeValue:
			result =  (int) 3;
		break;
		case gridColorVaule:
			result =  (int) 2;
		break;
   		case gridDotColorValue:
			result =  (int) 4;
		break;         
		case gridHSpaceVaule:
		case gridVSpaceVaule:
		case gridDotSpeedValue:
		case gridDotColorRValue:
		case gridDotColorGValue:
		case gridDotColorBValue:
		case gridDotPosXValue:
		case gridDotPosYValue:	
			result =  (int) 255;
		break;
	#endif	

	
	case videoPanelFWSaveValue:
		result =  (int) 20;
		break;
		
	    default:
	        result = 4096;
	}

	return result;
}
