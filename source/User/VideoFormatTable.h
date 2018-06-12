#ifndef VIDEO_FORMAT_TABLE_H
#define VIDEO_FORMAT_TABLE_H

#include "rs232command.h"


typedef enum _VIDEO_FORMAT
{
	MD_720x480_60p 		= 0,
	MD_720x576_50p 		= 1,
	MD_1280x720_60 		= 2,
	MD_1280x720_59_94 	= 3,
	MD_1280x720_50 		= 4,
	MD_1280x720_30 		= 5,
	MD_1280x720_29_97 	= 6,
	MD_1280x720_25 		= 7,
	MD_1280x720_24 		= 8,
	MD_1280x720_23_98 	= 9,	
	MD_1920x1080_60 	= 10,
	MD_1920x1080_59_94 	= 11,
	MD_1920x1080_50 	= 12,
	MD_1920x1080_30 	= 13,
	MD_1920x1080_30psf 	= 14,
	MD_1920x1080_29_97 	= 15,
	MD_1920x1080_29_97psf = 16,
	MD_1920x1080_25 	= 17,
	MD_1920x1080_25psf 	= 18,
	MD_1920x1080_24 	= 19,
	MD_1920x1080_24psf 	= 20,
	MD_1920x1080_23_98 	= 21,
	MD_1920x1080_23_98psf= 22,
	
	MD_640x480_60 		= 23,
	MD_640x480_75 		= 24,
	MD_640x480_85 		= 25,
	MD_800x600_60 		= 26,
	MD_800x600_75 		= 27,
	MD_800x600_85 		= 28,
	MD_1024x768_60 		= 29,
	MD_1024x768_75 		= 30,
	MD_1024x768_85 		= 31,	
	MD_1280x1024_60 	= 32,
	MD_1280x1024_75 	= 33,
	MD_1280x1024_85 	= 34,
	MD_1600x1200_60 	= 35,  
	// 以上是GS4911 支持的格式
	// 以下为新增格式
	MD_720x480_60i 		= 36,
	MD_720x576_50i 		= 37,
	//
	MD_1920x1080_60i 	= 38,
	MD_1920x1080_50i 	= 39,
	//
	MD_1152x864_75 		= 40,
	MD_1280x768_60 		= 41,
	MD_1280x960_60 		= 42,
	MD_1280x960_85 		= 43,
	MD_1360x768_60 		= 44,
	MD_1366x768_60 		= 45,
	MD_1400x1050_60 	= 46,
	MD_1440x900_60 		= 47,
	MD_2048x1152_60 	= 48,
	MD_1680x1050_60 	= 49,
	MD_1920x1200_60 	= 50,

	MD_1600x1200_60_reduce 	= 51,
	MD_1920x1080_60_reduce	= 52,
	MD_1440x900_60_reduce 	= 53,
	MD_1680x1050_60_reduce  = 54,
	MD_1400x1050_60_reduce  = 55,
	MD_2176x544_60_reduce	= 56,  // 有问题
	MD_1152x1152_60         = 57,  // 有问题
	MD_1280x1280_60         = 58,
	MD_2560x812_60			= 59,
	MD_2560x816_60			= 60,
	MD_1280x800_60			= 61,
	MD_1024x768_59_94       = 62, //added for 8028 v0.40
	MD_1920x1080_59_94i     = 63, //added for 8028 v0.40

	MD_720x480_60i_HDMI		= 64,
	MD_720x576_50i_HDMI		= 65,	

	MD_2048x1080_60			= 66,					
	MD_2304x1152_60			= 67,
	MD_1536x1536_60R		= 68,
	MD_1024x1280_60			= 69,

	MD_2560x1024_60			= 70,	
	MD_2560x1600_60			= 71,				
	MD_2560x1440_60			= 72,
	//MD_1080x3840_60,			
	MD_3840x1080_60			= 73,
	MD_1920x1080_120		= 74,				
	MD_1920x1080_100		= 75,		
	
	MD_3840x2160_30			= 76,
	MD_3840x2160_30R		= 77,
	MD_3840x2160_30DP		= 78,

	MD_3400x1000_60			= 79,
	MD_1900x1000_60			= 80,
	MD_3072x1536_60			= 81,
	MD_1536x1536_60			= 82,
	MD_1920x2160_60			= 83,
	MD_2048x1200_60			= 84,
	MD_3840x2160_24			= 85,	

	MD_3840x540_60			= 86,
	MD_3840x1080_30			= 87,
	MD_1080x1920_60			= 88,	
	
	//MD_1560x1600_60        = 100,
	//MD_3840x540_60         = 101,
	//MD_3840x1080_30         = 102,

	MD_INDEX_SIZE,
	MD_CUSTOM		= 124,
	MD_INPUT_FORMAT = 125,
	MD_UNDEFINE = 126,
	MD_NOINPUT = 127,
	MD_1600x768_60,		//for sendcard
	MD_1920x640_60,		//for sendcard
} VideoFormat_TypeDef;

typedef enum
{
	RES_720X480,
	RES_720X576,
	RES_1280X720,
	RES_1920X1080,
	RES_1920X540,
	RES_640X480,
	RES_800X600,
	RES_1024X768,
	RES_1280X1024,
	RES_1600X1200,
	RES_1280X768,
	RES_1440X864,
	RES_1440X900,
	RES_1400X1050,
	RES_1280X960,
	RES_1152X864,
	RES_1920X1200,
	RES_1360X768,
	RES_1366X768,
	RES_1680X1050,
	RES_2048X1152,
	RES_2176X544,
	RES_1152X1152,
	RES_1280X1280,
	RES_2560X812,
	RES_2560X816,
	RES_1280X800,
	
	RES_1440X240,
	RES_1440X288,

	RES_2048x1080,
	RES_2304x1152,
	RES_1024x1280,
	RES_2560x1024, 
	RES_2560x1600, 
	RES_2560x1440,
	RES_3840x1080,
	
	RES_3840x2160,
	RES_3400x1000,
	RES_3072x1536, 
	RES_1900x1000,
	RES_1536x1536,
	RES_3800x1000,
	RES_1920x2160,

	RES_7680x1080,
	RES_4096x1152,
	RES_2048x2304,

	RES_2048x1200,

	RES_3840x540,
	//MD_1080x1920_60,
	
	RES_CUSTOM,
	RES_INDEX_SIZE,		
} Resolution_TypeDef;
#pragma pack(1) //让编译器对这个结构作1字节对齐

typedef struct
{
	WORD DisplayWidth;
	WORD DisplayHeight;
} Resolution_Struct;



typedef struct
{
	Resolution_TypeDef res;
	WORD hActive;
	WORD vActive;
	WORD htotal;
	WORD vtotal;
	WORD hstart;
	WORD vstart;	
	u64 pClk;	//M
	BYTE vfreq;   // Hz	
	BYTE hSyncWidth;
	BYTE vSyncWidth;
	BYTE hvSyncPolarity;  // bit0 -- horizontal sync  bit1 -- vertical sync,  0 -- 正, 1 -- 负
	//WORD hBackP;
	//WORD vBackP;
} VideoDefine_Struct;
#pragma pack() //让编译器对这个结构作1字节对齐

extern VideoDefine_Struct tVideoDefineCustom;

const char * GetStrByVideoFormat(BYTE format);
Resolution_Struct GetResByVideoFormat(BYTE format);

u8 GetGS4911ByVideoFormat(BYTE format);

WORD GetHDEByVideoFormat(BYTE format);
WORD GetVDEByVideoFormat(BYTE format);
WORD GetHTotalByVideoFormat(BYTE format);
WORD GetVTotalByVideoFormat(BYTE format);

WORD GetHStartByVideoFormat(BYTE format);
WORD GetVStartByVideoFormat(BYTE format);
WORD GetHSyncWidthByVideoFormat(BYTE format);
WORD GetVSyncWidthByVideoFormat(BYTE format);
WORD GetHSyncPolarityByVideoFormat(BYTE format);
WORD GetVSyncPolarityByVideoFormat(BYTE format);
WORD GetVFreqByVideoFormat(BYTE format);

 
int GetDlckByVideoFormat(BYTE format);
int GetVideoFormat(int hde, int vde, int htotal, int vtotal, int vfreq);

int GetFormatByHdeVde(int hde, int vde, int vfreq);

//int GetFormatByLinesAndPixels(int line, int clock);
int getVideoFormatByTotal(int hde, int vde, int htotal, int vtotal, int vfreq);
int getVideoFormatByActive(int hde, int vde, int htotal, int vtotal, int vfreq);

int isInput1080P30Or25(int format);
int isInterlaceFormatByFormat(int format);
BYTE isInputHD(int format );
int isInput576P(int format);

#endif // VIDEO_FORMAT_TABLE_H

