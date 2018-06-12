#include "VideoFormatTable.h"
#include "menu.h"
#include <stdio.h>
#include "langStr.h"

#define _INPUT_FORMAT MD_1024x768_60

Resolution_Struct  tModeResolution[RES_INDEX_SIZE] = 
{
	{720, 480}, 	//RES_720X480,
	{720, 576}, 	//RES_720X576,
	{1280, 720}, 	//RES_1280X720,
	{1920, 1080}, 	//RES_1920X1080,
	{1920, 540},    //RES_1920X540,
	{640, 480}, 	//RES_640X480,
	{800, 600}, 	//RES_800X600,
	{1024, 768}, 	//RES_1024X768,
	{1280, 1024},	//RES_1280X1024,
	{1600, 1200}, 	//RES_1600X1200,
	{1280, 768},	//RES_1280X768, 	
	{1440, 864},	//RES_1440X864,	
	{1440, 900},	//RES_1440X900,
	{1400, 1050},	//RES_1400X1050,	
	{1280, 960},	//RES_1280X960,
	{1152, 864},	//RES_1152X864,
	{1920, 1200},	//RES_1920X1200,
	{1360, 768},	//RES_1360X768,
	{1366, 768},	//RES_1366X768,
	{1680, 1050},	//RES_1680X1050,
	{2048, 1152},	//RES_2048X1152,
	{2176, 544},	//RES_2176X544,
	{1152, 1152},	//RES_1152X1152,
	{1280, 1280},	//RES_1280X1280,
	{2560, 812},	//RES_2560X812,
	{2560, 816},	//RES_2560X816,
	{1280, 800},    //RES_1280X800,
	
	{1440, 240},//RES_1440X240,
	{1440, 288},//RES_1440X288,
	
	{2048, 1080},	//RES_2048x1080,
	{2304, 1152},	//RES_2304x1152,
	{1024, 1280},	//RES_1024x1280,
	{2560, 1024},	//RES_2560x1024, 
	{2560, 1600},	//RES_2560x1600, 
	{2560, 1440},	//RES_2560x1440,
	{3840, 1080},	//RES_3840x1080,
	{3840, 2160},	//RES_3840x2160
	{3400, 1000},	//RES_3400x1000,
	{3072, 1536},	//RES_3072x1536, 
	{1900, 1000},	//RES_1900x1000,
	{1536, 1536},	//RES_1536x1536,	
	{3800, 1000},	//RES_3800x1000,
	{1920, 2160},	//RES_1920x2160,	
	
	{7680, 1080},	//RES_7680x1080,
	{4096, 1152},	//RES_4096x1152,
	{2048, 2304},	//RES_2048x2304
	{2048, 1200},	//RES_2048x1200

};


VideoDefine_Struct tVideoDefineCustom;

const u8 FormatIndex[] = 
{
	0,3,4,6,7,9,10,11,12,13,15
#if 0
	RES_1024X768,
	RES_1280X720,
	RES_1280X1024,
	RES_1400X1050,
	RES_1536x1536,
	RES_1600X1200,
	RES_1920X1080,
	RES_1920X1200,
	RES_2048X1152,
	RES_2560X816,
	RES_3840x540,
	RES_3840x1080,
	MD_1080x1920_60
#endif
};

const VideoDefine_Struct tVideoDefine[MD_INDEX_SIZE] =
{

//#################################################################################################################################

//MD_1024x768_60,
		{
			RES_1024X768, //Resolution_Struct res;
			1024,
			768,
			1344,		//WORD htotal;
			806,		//WORD vtotal;
			296,		//WORD hstart;
			35, 		//WORD vstart;
			65000000,	//WORD pClk;	//M
			60, 		//BYTE vfreq;	// Hz
			136,		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			3,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
			
		},


		//MD_1024x1280_60
		{
			RES_1024x1280, //Resolution_Struct res;
			1024,
			1280,
			1376,		//WORD htotal;
			1327,		//WORD vtotal;
			104+176,	//WORD hstart;
			10+34,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			104,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_1152x1152_60 = 57,
		{
			RES_1152X1152, //Resolution_Struct res;
			1152,
			1152,
			1552,		//WORD htotal;
			1195,		//WORD vtotal;
			120+200,	//WORD hstart;
			10+30,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			120,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_1280x720_60,
		{
			RES_1280X720, //Resolution_Struct res;
			1280,
			720,
			1650,		//WORD htotal;
			750,		//WORD vtotal;
			40+110, 	//WORD hstart;
			5+5,		//WORD vstart;
			74250000,	//WORD pClk;	//M
			60, 		//BYTE vfreq;	// Hz
			40, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

//MD_1280x1024_60,
		{
			RES_1280X1024, //Resolution_Struct res;
			1280,
			1024,
			1688,		//WORD htotal;
			1066,		//WORD vtotal;
			112+248,	//WORD hstart;
			3+38,		//WORD vstart;
			10800000,	//WORD pClk;	//M
			60, 		//BYTE vfreq;	// Hz
			40,			//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


		//MD_1280x1280_60  = 58,
		{
			RES_1280X1280, //Resolution_Struct res;
			1280,
			1280,
			1728,		//WORD htotal;
			1327,		//WORD vtotal;
			136+224,	//WORD hstart;
			10+34,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			136,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_1400x1050_60, (VESA)
		{
			RES_1400X1050, //Resolution_Struct res;
			1400,
			1050,
			1864,		//WORD htotal;
			1089,		//WORD vtotal;
			144+232,	//WORD hstart;
			4+32,		//WORD vstart;
			121750000,	//WORD pClk;	59.948//M
			60, 		//BYTE vfreq;	// Hz
			144,		//WORD hSyncWidth;
			4,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

//MD_1536x1536_60R
		{
			RES_1536x1536, //Resolution_Struct res;
			1536,
			1536,
			1680,		//WORD htotal;
			1550,		//WORD vtotal;
			32+80,		//WORD hstart;
			10+31,		//WORD vstart;
			156240000,	//WORD pClk;	59.948//M
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},



//MD_2560x1600_60
		{
			RES_2560x1600, //Resolution_Struct res;
			2560,
			1600,
			3504,		//WORD htotal;
			1658,		//WORD vtotal;
			280+472,	//WORD hstart;
			6+49,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			280,		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_1600x1200_60_reduce = 51,
		{
			RES_1600X1200, //Resolution_Struct res;
			1600,
			1200,
			1760,		//WORD htotal;
			1235,		//WORD vtotal;
			32+80,		//WORD hstart;
			4+28,		//WORD vstart;
			130250000,	//WORD pClk;	130.25//M
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_1920x1080_60_reduce = 52,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2200,		//WORD htotal;
			1125,		//WORD vtotal;
			88+44,		//WORD hstart;
			3+5,		//WORD vstart;
			148500000,	//WORD pClk;	138.5//M
			60, 		//BYTE vfreq;	// Hz
			44, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_1920x1200_60, (Reduced Blanking)
		{
			RES_1920X1200, //Resolution_Struct res;
			1920,
			1200,
			2080,		//WORD htotal;
			1235,		//WORD vtotal;
			32+80,		//WORD hstart;
			6+26,		//WORD vstart;
			154000000,	//WORD pClk;	154//M
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_2048x1152_60, (Reduced Blanking)
		{
			RES_2048X1152, //Resolution_Struct res;
			2048,
			1152,
			2208,		//WORD htotal;
			1185,		//WORD vtotal;
			32+80,		//WORD hstart;
			5+25,		//WORD vstart;
			156750000,	//WORD pClk;	
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负		
		},

		//MD_2560x960_60			= 60,
		{
			RES_2560X816, //Resolution_Struct res;
			2560,
			960,
			2700,		//WORD htotal;
			972,		//WORD vtotal;
			32+80,		//WORD hstart;
			5+5,		//WORD vstart;
			157464000,	//WORD pClk;	136.25//M
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			5, 			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


		//MD_2560x1024_60
		{
			RES_2560x1024, //Resolution_Struct res;
			2560,
			1024,
			3424,		//WORD htotal;
			1063,		//WORD vtotal;
			272+432,	//WORD hstart;
			10+26,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			272,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


		//MD_3840x540_60_R
		{
			RES_3840x540, //Resolution_Struct res;
			3840,
			540,
			4000,		//WORD htotal;
			559,		//WORD vtotal;
			32+80,		//WORD hstart;
			10+6,		//WORD vstart;
			134000000,	//WORD pClk;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},



		//MD_3840x1080_30_DP
		{
			RES_3840x1080, //Resolution_Struct res;
			3840,
			1080,
			4000,		//WORD htotal;
			1099,		//WORD vtotal;
			32+80, 		//WORD hstart;
			10+6, 		//WORD vstart;
			131750000,	//WORD pClk;
			30, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

		//MD_1080x1920_60_DP
		{
			MD_1080x1920_60, //Resolution_Struct res;
			1080,
			1920,
			1240,		//WORD htotal;
			1975,		//WORD vtotal;
			32+80, 		//WORD hstart;
			10+42, 		//WORD vstart;
			146750000,	//WORD pClk;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},






//#################################################################################################################################

		//MD_800x600_60,
		{
			RES_800X600, //Resolution_Struct res;
			800,
			600,
			1056,		//WORD htotal;
			628,		//WORD vtotal;
			128+88, 	//WORD hstart;
			4+23, 		//WORD vstart;
			40000000,
			60, 		//BYTE vfreq;	// Hz
			128,		//WORD hSyncWidth;
			4,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_1280x720_50,
		{
			RES_1280X720, 	//Resolution_Struct res;
			1280,
			720,
			1980,			//WORD htotal;
			750,			//WORD vtotal;
			260,			//WORD hstart;
			25, 			//WORD vstart;
			60500000,		//WORD pClk;	//M
			50, 			//BYTE vfreq;	// Hz
			40, 			//WORD hSyncWidth;
			5,				// WORD vSyncWidth;
			0,				//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
//MD_1280x768_60, //DMT-20
		{
			RES_1280X768, //Resolution_Struct res;
			1280,
			768,
			1664,		//WORD htotal;
			798,		//WORD vtotal;
			128+192,	//WORD hstart;
			7+20,		//WORD vstart;
			79500000,	//68.25,		//WORD pClk;	//M
			60, 		//BYTE vfreq;	// Hz
			128,		//WORD hSyncWidth;
			7,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
//MD_1280x800_60			= 61,
		{
			RES_1280X800, //Resolution_Struct res;
			1280,
			800,
			1680,		//WORD htotal;
			831,		//WORD vtotal;
			128+200,	//WORD hstart;
			6+22,		//WORD vstart;
			83500000,	//WORD pClk;	//M
			60, 		//BYTE vfreq;	// Hz
			128,		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

//MD_1360x768_60,
		{
			RES_1360X768, //Resolution_Struct res;
			1360,
			768,
			1792,		//WORD htotal;
			795,		//WORD vtotal;
			112+256,	//WORD hstart;
			6+18,		//WORD vstart;
			85500000,	//WORD pClk;	//M
			60, 		//BYTE vfreq;	// Hz
			112,		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
//MD_1366x768_60,
		{
			RES_1366X768, //Resolution_Struct res;
			1366,
			768,
			1792,		//WORD htotal;
			798,		//WORD vtotal;
			143+213,	//WORD hstart;
			3+24,		//WORD vstart;
			85750000,	//WORD pClk;	//M
			60, 		//BYTE vfreq;	// Hz
			143,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},	

		//MD_1440x900_60,
		{
			RES_1440X900, //Resolution_Struct res;
			1400,
			900,
			1904,		//WORD htotal;
			934,		//WORD vtotal;
			152+232,	//WORD hstart;
			6+25,		//WORD vstart;
			106500000,	//WORD pClk;	86.5//M
			60, 		//BYTE vfreq;	// Hz
			152,		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

//MD_1680x1050_60,
		{
			RES_1680X1050, //Resolution_Struct res;
			1680,
			1050,
			2240,		//WORD htotal;
			1089,		//WORD vtotal;
			176+280,	//WORD hstart;
			6+30,		//WORD vstart;
			146250000,	//WORD pClk;	119//M
			60, 		//BYTE vfreq;	// Hz
			176,		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

//MD_1920x1080_50i,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,		//WORD htotal;
			1125,		//WORD vtotal;
			44+148, 	//WORD hstart;
			5+15,		//WORD vstart;
			141500000,	//WORD pClk;	115//M
			50,			//60,		//BYTE vfreq;	// Hz
			44, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


//MD_2560x812_60			= 59,
		{
			RES_2560X812, //Resolution_Struct res;
			2560,
			812,
			2720,		//WORD htotal;
			836,		//WORD vtotal;
			32+80,		//WORD hstart;
			10+11,		//WORD vstart;
			138750000,	//WORD pClk;	136.25//M
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

//MD_3840x1080_60R
		{
			RES_3840x1080, //Resolution_Struct res;
			3840,
			1080,
			4000,		//WORD htotal;
			1111,		//WORD vtotal;
			32+80,		//WORD hstart;
			10+18,		//WORD vstart;
			3460000,	//WORD pClk;	131.75//M
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
//MD_3840x2160_60
		{
			RES_3840x2160, //Resolution_Struct res;
			3840,
			2160,
			5136,		//WORD htotal;
			2200,		//WORD vtotal;
			408+648,	//WORD hstart;
			5+32,		//WORD vstart;
			712750000,	//WORD pClk;	533.25/M
			30, 		//BYTE vfreq;	// Hz
			408,		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		

			
		//MD_720x480_60
		{
			RES_720X480, //Resolution_Struct res;
			720,
			480,
			858,		//WORD htotal;
			525,		//WORD vtotal;
			62+60,		//WORD hstart;
			6+30,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			62, 		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			3,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_720x576_50,
		{
			RES_720X576, //Resolution_Struct res;
			720,
			576,
			864,		//WORD htotal;
			625,		//WORD vtotal;
			64+68,		//WORD hstart;
			5+39,		//WORD vstart;
			50, 		//BYTE vfreq;	// Hz
			64, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			3,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1280x720_60,
		{
			RES_1280X720, //Resolution_Struct res;
			1280,
			720,
			1650,		//WORD htotal;
			750,		//WORD vtotal;
			40+220, 	//WORD hstart;
			5+20,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			40, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		
		
		//MD_1280x720_30,
		{
			RES_1280X720, //Resolution_Struct res;
			1280,
			720,
			3300,	//WORD htotal;
			750,	//WORD vtotal;
			260,	//WORD hstart;
			25, 	//WORD vstart;
			30, 	//BYTE vfreq;	// Hz
			40, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1280x720_29_97,
		{
			RES_1280X720, //Resolution_Struct res;
			1280,
			720,
			3300,	//WORD htotal;
			750,	//WORD vtotal;
			260,	//WORD hstart;
			25, 	//WORD vstart;
			30, 	//BYTE vfreq;	// Hz
			40, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1280x720_25,
		{
			RES_1280X720, //Resolution_Struct res;
			1280,
			720,
			3960,	//WORD htotal;
			750,	//WORD vtotal;
			260,	//WORD hstart;
			25, 	//WORD vstart;
			25, 	//BYTE vfreq;	// Hz
			40, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1280x720_24,
		{
			RES_1280X720, //Resolution_Struct res;
			1280,
			720,
			4125,	//WORD htotal;
			750,	//WORD vtotal;
			260,	//WORD hstart;
			25, 	//WORD vstart;
			24, 	//BYTE vfreq;	// Hz
			40, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1280x720_23_98,
		{
			RES_1280X720, //Resolution_Struct res;
			1280,
			720,
			4125,	//WORD htotal;
			750,	//WORD vtotal;
			260,	//WORD hstart;
			25, 	//WORD vstart;
			24, 	//BYTE vfreq;	// Hz
			40, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_60,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2200,		//WORD htotal;
			1125,		//WORD vtotal;
			44+148, 	//WORD hstart;
			5+36,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			44, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_59_94,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2200,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			60, 	//BYTE vfreq;	// Hz
			40, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_50,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			50, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_30,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			30, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_30psf,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			30, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_29_97,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			30, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_29_97psf,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			30, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_25,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			25, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_25psf,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			25, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_24,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2750,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			24, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth; 
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_24psf,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2750,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			24, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_23_98,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2750,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			24, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth; 
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_23_98psf,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2750,	//WORD htotal;
			1125,	//WORD vtotal;
			192,	//WORD hstart;
			41, 	//WORD vstart;
			24, 	//BYTE vfreq;	// Hz
			44, 	//WORD hSyncWidth;
			5,		// WORD vSyncWidth; 
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_640x480_60,
		{
			RES_640X480, //Resolution_Struct res;
			640,
			480,
			800,	//WORD htotal;
			525,	//WORD vtotal;
			136,	//WORD hstart;
			27, 	//WORD vstart;
			60, 	//BYTE vfreq;	// Hz
			96, 	//WORD hSyncWidth;
			2,		// WORD vSyncWidth;
			3,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_640x480_75,
		{
			RES_640X480, //Resolution_Struct res;
			640,
			480,
			840,	//WORD htotal;
			500,	//WORD vtotal;
			184,	//WORD hstart;
			19, 	//WORD vstart;
			75, 	//BYTE vfreq;	// Hz
			64, 	//WORD hSyncWidth;
			3,		// WORD vSyncWidth;
			3,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_640x480_85,
		{
			RES_640X480, //Resolution_Struct res;
			640,
			480,
			832,	//WORD htotal;
			509,	//WORD vtotal;
			136,	//WORD hstart;
			28, 	//WORD vstart;
			85, 	//BYTE vfreq;	// Hz
			56, 	//WORD hSyncWidth;
			3,		// WORD vSyncWidth;
			3,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},

		//MD_800x600_75,
		{
			RES_800X600, //Resolution_Struct res;
			800,
			600,
			1056,	//WORD htotal;
			625,	//WORD vtotal;
			240,	//WORD hstart;
			24, 	//WORD vstart;
			75, 	//BYTE vfreq;	// Hz
			80, 	//WORD hSyncWidth;
			3,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_800x600_85,
		{
			RES_800X600, //Resolution_Struct res;
			800,
			600,
			1048,	//WORD htotal;
			631,	//WORD vtotal;
			216,	//WORD hstart;
			30, 	//WORD vstart;
			85, 	//BYTE vfreq;	// Hz
			64, 	//WORD hSyncWidth;
			3,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		
		//MD_1024x768_75,
		{
			RES_1024X768, //Resolution_Struct res;
			1024,
			768,
			1312,	//WORD htotal;
			800,	//WORD vtotal;
			272,	//WORD hstart;
			31, 	//WORD vstart;
			75, 	//BYTE vfreq;	// Hz
			96, 	//WORD hSyncWidth;
			3,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1024x768_85,
		{
			RES_1024X768, //Resolution_Struct res;
			1024,
			768,
			1376,	//WORD htotal;
			808,	//WORD vtotal;
			304,	//WORD hstart;
			39, 	//WORD vstart;
			85, 	//BYTE vfreq;	// Hz
			96, 	//WORD hSyncWidth;
			3,		// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		
		//MD_1280x1024_75,
		{
			RES_1280X1024, //Resolution_Struct res;
			1280,
			1024,
			1688,		//WORD htotal;
			1066,		//WORD vtotal;
			144+248,	//WORD hstart;
			41, 		//WORD vstart;
			75, 		//BYTE vfreq;	// Hz
			144,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1280x1024_85,
		{
			RES_1280X1024, //Resolution_Struct res;
			1280,
			1024,
			1728,		//WORD htotal;
			1072,		//WORD vtotal;
			384,		//WORD hstart;
			47, 		//WORD vstart;
			85, 		//BYTE vfreq;	// Hz
			160,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1600x1200_60,	// 以上是GS4911 支持的格式	
		{
			RES_1600X1200, //Resolution_Struct res;
			1600,
			1200,
			2160,		//WORD htotal;
			1250,		//WORD vtotal;
			496,		//WORD hstart;
			49, 		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			192,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		// 以下为新增格式
		//MD_720x480_60i,
		{
			RES_720X480, //Resolution_Struct res;
			720,
			480,
			1716,		//WORD htotal;
			525,		//WORD vtotal;
			124+114,	//WORD hstart;
			3+15,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			124,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			3,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_720x576_50i,
		{
			RES_720X576, //Resolution_Struct res;
			720,
			576,
			1728,		//WORD htotal;
			625,		//WORD vtotal;
			128+138,	//WORD hstart;
			3+19,		//WORD vstart;
			50,			//60,		//BYTE vfreq;	// Hz
			128,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			3,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//
		//MD_1920x1080_60i,
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2200,		//WORD htotal;
			1125,		//WORD vtotal;
			44+148, 	//WORD hstart;
			5+15,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			44, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		
		//
		//MD_1152x864_75,
		{
			RES_1152X864, //Resolution_Struct res;
			1152,
			864,
			1600,		//WORD htotal;
			900,		//WORD vtotal;
			128+256,	//WORD hstart;
			3+32,		//WORD vstart;
			75, 		//BYTE vfreq;	// Hz
			128,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		
		//MD_1280x960_60,
		{
			RES_1280X960, //Resolution_Struct res;
			1280,
			960,
			1800,		//WORD htotal;
			1000,		//WORD vtotal;
			112+312,	//WORD hstart;
			3+36,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			112,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1280x960_85,
		{
			RES_1280X960, //Resolution_Struct res;
			1280,
			960,
			1728,		//WORD htotal;
			1011,		//WORD vtotal;
			160+224,	//WORD hstart;
			3+47,		//WORD vstart;
			85, 		//BYTE vfreq;	// Hz
			160,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1440x900_60, (Reduced Blanking)
		{
			RES_1440X900, //Resolution_Struct res;
			1440,
			900,
			1600,		//WORD htotal;
			926,		//WORD vtotal;
			32+80,		//WORD hstart;
			6+17,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负		
		},
		//MD_1680x1050_60_reduce   = 54,
		{
			RES_1680X1050, //Resolution_Struct res;
			1680,
			1050,
			1840,		//WORD htotal;
			1080,		//WORD vtotal;
			32+80,		//WORD hstart;
			6+21,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			6,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1400x1050_60, (Reduced Blanking)
		{
			RES_1400X1050, //Resolution_Struct res;
			1400,
			1050,
			1560,		//WORD htotal;
			1080,		//WORD vtotal;
			32+80,		//WORD hstart;
			4+23,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			4,			// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_2176x544_60,(Reduced Blanking) NOT CVT STANDARD
		{
			RES_2176X544, //Resolution_Struct res;
			2176,
			544,
			2336,		//WORD htotal;
			563,		//WORD vtotal;
			32+80,		//WORD hstart;
			10+6,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		
		
		//MD_1024x768_59_94,
		{
			RES_1024X768, //Resolution_Struct res;
			1024,
			768,
			1344,	//WORD htotal;
			806,	//WORD vtotal;
			296,	//WORD hstart;
			35, 	//WORD vstart;
			60, 	//BYTE vfreq;	// Hz
			136,	//WORD hSyncWidth;
			6,		// WORD vSyncWidth;
			3,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1920x1080_59_94i,
		{
			RES_1920X540, //Resolution_Struct res;
			1920,
			540,
			2200,		//WORD htotal;
			1125,		//WORD vtotal;
			44+148, 	//WORD hstart;
			5+15,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			44, 		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			0,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1440X480_60i,
		{
			RES_1440X240, //Resolution_Struct res;
			1440,
			240,
			1716,		//WORD htotal;
			525,		//WORD vtotal;
			124+114,	//WORD hstart;
			3+15,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			124,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			3,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1440X576_50i,
		{
			RES_1440X288, //Resolution_Struct res;
			1440,
			288,
			1728,		//WORD htotal;
			625,		//WORD vtotal;
			128+138,	//WORD hstart;
			3+19,		//WORD vstart;
			50,			//60,		//BYTE vfreq;	// Hz
			128,		//WORD hSyncWidth;
			3,			// WORD vSyncWidth;
			3,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		// MD_2048x1080_60R,
		{
			RES_2048x1080, //Resolution_Struct res;
			2048,
			1080,
			2208,		//WORD htotal;
			1111,		//WORD vtotal;
			32+80,		//WORD hstart;
			10+18,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
	
		//MD_2304x1152_60R,
		{
			RES_2304x1152, //Resolution_Struct res;
			2304,
			1152,
			2464,		//WORD htotal;
			1185,		//WORD vtotal;
			32+80,		//WORD hstart;
			10+20,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			32, 		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		


		
		//MD_2560x1440_60
		{
			RES_2560x1440, //Resolution_Struct res;
			2560,
			1440,
			3488,		//WORD htotal;
			1493,		//WORD vtotal;
			272+464,	//WORD hstart;
			5+45,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			272,		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},	
		// MD_1920x1080_120
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2656,		//WORD htotal;
			1160,		//WORD vtotal;
			208+368,	//WORD hstart;
			5+72,		//WORD vstart;
			120,		//BYTE vfreq;	// Hz
			208,		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
			
		//MD_1920x1080_100
		{
			RES_1920X1080, //Resolution_Struct res;
			1920,
			1080,
			2640,		//WORD htotal;
			1147,		//WORD vtotal;
			208+360,	//WORD hstart;
			5+59,		//WORD vstart;
			100,		//BYTE vfreq;	// Hz
			208,		//WORD hSyncWidth;
			5,			// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},


	
		//MD_3400x1000_60
		{
			RES_3400x1000, //Resolution_Struct res;
			3400,
			1000,
			4536,		//WORD htotal;
			1038,		//WORD vtotal;
			360+568,	//WORD hstart;
			10+25,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			360,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1900x1000_60
		{
			RES_1900x1000, //Resolution_Struct res;
			1900,
			1000,
			2520,		//WORD htotal;
			1038,		//WORD vtotal;
			200+312,	//WORD hstart;
			10+25,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			200,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
			//MD_3072x1536_60
		{
			RES_3072x1536, //Resolution_Struct res;
			3072,
			1536,
			4192,		//WORD htotal;
			1592,		//WORD vtotal;
			328+560,	//WORD hstart;
			10+43,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			328,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			2,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_1536x1536_60
		{
			RES_1536x1536, //Resolution_Struct res;
			1536,
			1536,
			2096,		//WORD htotal;
			1592,		//WORD vtotal;
			160+280,	//WORD hstart;
			10+43,		//WORD vstart;
			60, 		//BYTE vfreq;	// Hz
			160,		//WORD hSyncWidth;
			10, 		// WORD vSyncWidth;
			1,			//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		
	#if 1
		//这些分辨率需要2颗9320共同完成,只是占着分辨率列表,参数不一定正确
		//MD_1920x2160_60			
		{
			RES_1920x2160, //Resolution_Struct res;
			1920,
			2160,
			2080,	//WORD htotal;
			2222,	//WORD vtotal;
			112,	//WORD hstart;
			59, 	//WORD vstart;
			60, 	//BYTE vfreq;	// Hz
			32, 	//WORD hSyncWidth;
			4,		// WORD vSyncWidth;
			1,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},	
	
	#if 0
	#else
		//MD_2048x1200_60 = 84,
		{
			RES_2048x1200,
			2048,
			1200,
			2200,	//WORD htotal;
			1250,	//WORD vtotal;
			32+3, 	//WORD hstart;
			48+26,	//WORD vstart;
			60, 	//BYTE vfreq;	// Hz
			32, 	//WORD hSyncWidth;
			48, 	// WORD vSyncWidth;
			0,		//BYTE hvSyncPolarity;	// bit0 -- horizontal sync	bit1 -- vertical sync,	0 -- 正, 1 -- 负
		},
		//MD_3840x2160_24 = 85,
		{
			RES_3840x2160,
			3840,
			2160,
			4090,	
			2250,
			48+8,
			80+10,
			24,
			48, 	
			80,
			0,
		},	
	#endif
	#endif
		
};

#if 0

#else


//char * tVideoFormatStr[MD_INDEX_SIZE+3] = 
const char *const tVideoFormatStr[MD_INDEX_SIZE+5] =
{
	"720x480@60", 		//1MD_720x480_60,
	"720x576@50", 		//1MD_720x576_50,
	"1280x720@60", 		//1MD_1280x720_60,
	"1280x720@59.94", 	//MD_1280x720_59_94,
	"1280x720@50", 		//MD_1280x720_50,
	"1280x720@30", 		//MD_1280x720_30,
	"1280x720@29.97", 	//MD_1280x720_29_97,
	"1280x720@25", 		//MD_1280x720_25,
	"1280x720@24", 		//MD_1280x720_24,
	"1280x720@23.98", 	//MD_1280x720_23_98,
	"1920x1080@60", 	//MD_1920x1080_60,
	"1920x1080@59.94", 	//MD_1920x1080_59_94,
	"1920x1080@50", 	//MD_1920x1080_50,
	"1920x1080@30", 	//MD_1920x1080_30,
	"1920x1080@30s", 	//MD_1920x1080_30psf,
	"1920x1080@29.97", 	//MD_1920x1080_29_97,
	"1920x1080@29.9s", //MD_1920x1080_29_97psf,
	"1920x1080@25", 	//MD_1920x1080_25,
	"1920x1080@25s", 	//MD_1920x1080_25psf,
	"1920x1080@24", 	//MD_1920x1080_24,
	"1920x1080@24s", 	//MD_1920x1080_24psf,
	"1920x1080@23.98", 	//MD_1920x1080_23_98,
	"1920x1080@23.9s", //MD_1920x1080_23_98psf,
	"640x480@60", 		//MD_640x480_60,
	"640x480@75", 		//MD_640x480_75,
	"640x480@85", 		//MD_640x480_85,
	"800x600@60", 		//MD_800x600_60,
	"800x600@75", 		//MD_800x600_75,
	"800x600@85", 		//MD_800x600_85,
	"1024x768@60", 		//MD_1024x768_60,
	"1024x768@75", 		//MD_1024x768_75,
	"1024x768@85", 		//MD_1024x768_85,
	"1280x1024@60", 	//MD_1280x1024_60,
	"1280x1024@75", 	//MD_1280x1024_75,
	"1280x1024@85", 	//MD_1280x1024_85,
	"1600x1200@60", 	//MD_1600x1200_60,
	// 以下为新增格式
	"720x480i@60", 		//MD_720x480_60i,
	"720x576i@50", 		//MD_720x576_50i,
	//
	"1920x1080i@60", 	//MD_1920x1080_60i,
	"1920x1080i@50", 	//MD_1920x1080_50i,
	//
	"1152x864@75", 		//MD_1152x864_75,
	"1280x768@60", 		//MD_1280x768_60,
	"1280x960@60", 		//MD_1280x960_60,
	"1280x960@85", 		//MD_1280x960_85,
	"1360x768@60", 		//MD_1360x768_60,
	"1366x768@60", 		//MD_1366x768_60,
	"1400x1050@60", 	//MD_1400x1050_60,
	"1440x900@60", 		//MD_1440x900_60,
	"2048x1152@60", 	//MD_2048x1152_60,
	"1680x1050@60", 	//MD_1680x1050_60,
	"1920x1200@60", 	//MD_1920x1200_60,
	
	"1600x1200@60_r",		//MD_1600x1200_60_reduce = 51,
	"1920x1080@60_r",		//MD_1920x1080_60_reduce = 52,
	"1440x900@60_r",		//MD_1440x900_60_no_reduce = 53,
	"1680x1050@60_r",		//MD_1680x1050_60_reduce   = 54,
	"1400x1050@60_r",		//MD_1400x1050_60_reduce   = 55,
	"2176x544@60_r",		//MD_2176x544_60_reduce	= 56,
	"1152x1152@60",		//MD_1152x1152_60_reduce  = 57,
	"1280x1280@60",		//MD_1280x1280_60_reduce  = 58,
	"2560x812@60",		//MD_2560x812_60			= 59,
	"2560x960@60",		//MD_2560x816_60			= 60,
	"1280x800@60",		//MD_1280x800_60			= 61,
	"1024x768@59.94",		//MD_1024x768_59_94       = 62, //added for 8028 v0.40
	"1920x1080i@59.94",		//MD_1920x1080_59_94i     = 63, //added for 8028 v0.40

	"720x480i@60",		//MD_720x480_60i_HDMI		= 64,
	"720x576i@50",		//MD_720x576_50i_HDMI		= 65,	

	"2048x1080@60",		//MD_2048x1080_60			= 66,					
	"2304x1152i@60",		//MD_2304x1152_60			= 67,
	"1536x1536@60_r",		//MD_1536x1536_60R		= 68,
	"1024x1280@60",		//MD_1024x1280_60			= 69,

	"2560x1024@60",		//MD_2560x1024_60			= 70,	
	"2560x1600@60",		//MD_2560x1600_60			= 71,				
	"2560x1440@60",		//MD_2560x1440_60			= 72,
	//MD_1080x3840_60,			
	"3840x1080@60",		//MD_3840x1080_60			= 73,
	"1920x1080@120",		//MD_1920x1080_120		= 74,				
	"1920x1080@100",		//MD_1920x1080_100		= 75,		
	
	"3840x2160@30",		//MD_3840x2160_30			= 76,
	"3840x2160@30_r",		//MD_3840x2160_30R		= 77,
	"3840x2160@30_dp",		//MD_3840x2160_30DP		= 78,

	"3400x1000@60",		//MD_3400x1000_60			= 79,
	"1900x1000@60",		//MD_1900x1000_60			= 80,
	"3072x1535@60",		//MD_3072x1536_60			= 81,
	"1536x1536@60",		//MD_1536x1536_60			= 82,
	"1920x2160@60",		//MD_1920x2160_60			= 83,
	"2048x1200@60",		//MD_2048x1200_60			= 84,
	"3840x2160@24",		//MD_3840x2160_24			= 85,	


	"3840x540@60",		//MD_3840x540_60			= 100,
	"3840x1080@30",		//MD_3840x1080_30			= 101,	
	"1080x1920@60", 	//MD_INDEX_SIZE,	
	//"1920x1080@60",     //MD_CUSTOM	
	"UNDEFINE  ", 		//MD_UNDEFINE = 126,
	"NO INPUT  ", 		//MD_NOINPUT = 127,
	"INPUT FORMAT", 	//MD_INPUT_FORMAT = 125,
};
#endif

u8 tGS4911_VID_STD[] =
{
	9,		//MD_720x480_60,
	10,		//MD_720x576_50,
	11,		//MD_1280x720_60,
	12,		//MD_1280x720_59_94,
	13,		//MD_1280x720_50,
	14,		//MD_1280x720_30,
	15,		//MD_1280x720_29_97,
	16,		//MD_1280x720_25,
	17,		//MD_1280x720_24,
	18,		//MD_1280x720_23_98,
	21,		//MD_1920x1080_60,
	22,		//MD_1920x1080_59_94,
	23,		//MD_1920x1080_50,
	29,		//MD_1920x1080_30,
	30,		//MD_1920x1080_30psf,
	31,		//MD_1920x1080_29_97,
	32,		//MD_1920x1080_29_97psf,
	33,		//MD_1920x1080_25,
	34,		//MD_1920x1080_25psf,
	35,		//MD_1920x1080_24,
	36,		//MD_1920x1080_24psf,
	37,		//MD_1920x1080_23_98,
	38,		//MD_1920x1080_23_98psf,
	39,		//MD_640x480_60,
	40,		//MD_640x480_75,
	41,		//MD_640x480_85,
	42,		//MD_800x600_60,
	43,		//MD_800x600_75,
	44,		//MD_800x600_85,
	45,		//MD_1024x768_60,
	46,		//MD_1024x768_75,
	47,		//MD_1024x768_85,
	48,		//MD_1280x1024_60,
	49,		//MD_1280x1024_75,
	50,		//MD_1280x1024_85,
	51,		//MD_1600x1200_60,
	MD_INDEX_SIZE,		
};

static const char * unknowFormat = "Unknown";
const char * GetStrByVideoFormat(BYTE format)
{
	if(format < MD_INDEX_SIZE)
	{
		/*
		if(format == MD_CUSTOM)
		{
			char buffer[30];
			const char *p;
			sprintf(buffer,"%dx%dx%d",getCustomDisplayWidth(),getCustomDisplayHeight(),getCustomVFreq());
			printf(buffer);
			p = buffer;
			return p;
		}
		*/
		return tVideoFormatStr[format];
	}
	else if(format == MD_UNDEFINE)
	{
		return tVideoFormatStr[MD_INDEX_SIZE+1];
	}
	else if(format == MD_NOINPUT)
	{
		return langGetStr(noInputStr);//tVideoFormatStr[MD_INDEX_SIZE+2];
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return tVideoFormatStr[MD_INDEX_SIZE+3];
	}
	else 
	{
		return unknowFormat;
	}
}


Resolution_Struct GetResByVideoFormat(BYTE format)
{
	if(format < MD_INDEX_SIZE)
	{
	
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if(format >= MD_3840x2160_24)
		{
			Resolution_Struct res;
			res.DisplayWidth = getCustomDisplayWidth();
			res.DisplayHeight= getCustomDisplayHeight();	
			return res;
		}
		return tModeResolution[tVideoDefine[format].res];
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetResByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		return tModeResolution[tVideoDefine[MD_1024x768_60].res];
	}
}

u8 GetGS4911ByVideoFormat(BYTE format)
{
	if(format <= MD_1600x1200_60)
	{
		return tGS4911_VID_STD[format];
	}
	else
	{
		return tGS4911_VID_STD[MD_1024x768_60];
	}
}

WORD GetHDEByVideoFormat(BYTE format)
{
	return GetResByVideoFormat(format).DisplayWidth;
}
WORD GetVDEByVideoFormat(BYTE format)
{
	return GetResByVideoFormat(format).DisplayHeight;
}

WORD GetHTotalByVideoFormat(BYTE format)
{
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))
		{
//			return getCustomHTotal();
		}
		return tVideoDefine[format].htotal;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetHTotalByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		return tVideoDefine[MD_1024x768_60].htotal;
	}
}
WORD GetVTotalByVideoFormat(BYTE format)
{
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))
		{
			return getCustomVTotal();
		}
		return tVideoDefine[format].vtotal;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetVTotalByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		return tVideoDefine[MD_1024x768_60].vtotal;
	}
}


WORD GetHStartByVideoFormat(BYTE format)
{
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))
		{
//			return getCustomHStart();
		}
		return tVideoDefine[format].hstart;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetHStartByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		return tVideoDefine[MD_1024x768_60].hstart;
	}
}
WORD GetVStartByVideoFormat(BYTE format)
{
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))	
		{
			return getCustomVStart();
		}
		return tVideoDefine[format].vstart;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetVStartByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		return tVideoDefine[MD_1024x768_60].vstart;
	}
}

WORD GetHSyncWidthByVideoFormat(BYTE format)
{
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))	
		{
//			return getCustomHSyncWidth();
		}
		return tVideoDefine[format].hSyncWidth;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetHSyncWidthByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		return tVideoDefine[MD_1024x768_60].hSyncWidth;
	}
}
WORD GetVSyncWidthByVideoFormat(BYTE format)
{
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))
		{
			return getCustomVSyncWidth();
		}
		return tVideoDefine[format].vSyncWidth;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetVSyncWidthByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		return tVideoDefine[MD_1024x768_60].vSyncWidth;
	}
}

WORD GetHSyncPolarityByVideoFormat(BYTE format)  // return 0 -- 正, return 1 -- 负
{
	BYTE result;
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))
		{
//			return getCustomHVSyncPolarity();
		}
		result =  tVideoDefine[format].hvSyncPolarity;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetHSyncPolarityByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		result =  tVideoDefine[MD_1024x768_60].hvSyncPolarity;
	}
	if(result & 0x01)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
WORD GetVSyncPolarityByVideoFormat(BYTE format)  // return 0 -- 正, return 1 -- 负
{
	BYTE result;
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))
		{
//			return getCustomHVSyncPolarity();
		}
		result =  tVideoDefine[format].hvSyncPolarity;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetVSyncPolarityByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		result =  tVideoDefine[MD_1024x768_60].hvSyncPolarity;
	}
	if(result & 0x02)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

WORD GetVFreqByVideoFormat(BYTE format)
{
	WORD result;
	if(format <= MD_INDEX_SIZE)
	{
		//if((format >= MD_CUSTOM)&&(format <= MD_1536x1536x60_r))
		if((format >= MD_CUSTOM))
		{
			return getCustomVFreq();
		}
		result = tVideoDefine[format].vfreq;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetVFreqByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		result =  tVideoDefine[MD_1024x768_60].vfreq;
	}
	return result;
}


int GetDlckByVideoFormat(BYTE format)
{
	int result;
	if(format <= MD_INDEX_SIZE)
	{
		result =  tVideoDefine[format].htotal*tVideoDefine[format].vtotal*tVideoDefine[format].vfreq;
	}
	else if(format == MD_INPUT_FORMAT)
	{
		return GetDlckByVideoFormat(_INPUT_FORMAT);
	}
	else
	{
		result =  tVideoDefine[MD_1024x768_60].htotal*tVideoDefine[MD_1024x768_60].vtotal*tVideoDefine[MD_1024x768_60].vfreq;;
	}
	return result;
}
#ifndef _ABS
#define _ABS(a) (((a)>0)?(a):(-(a)))
#endif

int GetVideoFormat(int hde, int vde, int htotal, int vtotal, int vfreq)
{
	int i = 0;
	for(i=0; i<MD_INDEX_SIZE; i++)
	{
		Resolution_Struct res = GetResByVideoFormat(i);
		if((_ABS((int)res.DisplayHeight - vde)<2) && (_ABS((int)res.DisplayWidth - hde)<2))
		{
			if((_ABS((int)GetVFreqByVideoFormat(i)-vfreq)<2))
			{
				if((_ABS((int)GetHTotalByVideoFormat(i)-htotal)<5)
					&&(_ABS((int)GetVTotalByVideoFormat(i)-vtotal)<5))
				{
					break;
				}
			}
		}
	}
	if(i < MD_INDEX_SIZE)
	{
		return i;
	}
	else
	{
		return MD_UNDEFINE;
	}
}
BYTE isInputHD(int format )
{
	if((format == MD_1920x1080_50)||(format == MD_1920x1080_60)||(format == MD_1920x1080_59_94))
	{
		return TRUE;
	}
	return FALSE;
}
int isInput576P(int format)
{
	if(format == MD_720x576_50p)
	{
		return TRUE;
	}
	return FALSE;
}
int isInput1080P30Or25(int format)
{
	if((format == MD_1920x1080_30)||(format == MD_1920x1080_25))
	//if((format == MD_1920x1080_30)||(format == MD_1920x1080_25)||(format == MD_UNDEFINE))
	{
		return TRUE;
	}
	return FALSE;
}

int isInterlaceFormatByFormat(int format)
{
	if((format == MD_720x480_60i)||(format == MD_720x576_50i)
	// ||(format == MD_1440X240)||(format == MD_1440X288)
	 ||(format == MD_1920x1080_50i)||(format == MD_1920x1080_59_94i)||(format == MD_1920x1080_60i))
	{
		return TRUE;
	}
	return FALSE;
}

int  isInterlaceFormat(int  hde, int  vde, int  htotal, int  vtotal, int  vfreq)
{
	int temp;

	temp = hde;
	hde = vde;
	vde = temp;
		
	if((_ABS(1920 - vde)<2) && (_ABS(540 - hde)<2)
	//==============20130604
	//	&&(ABS(121 - vfreq)<5)
		&&(_ABS(60 - vfreq)<5)
	//==============20130604 END
		&&(_ABS(2200-htotal)<5)&&(_ABS(562-vtotal)<5))
	{
		return (MD_1920x1080_60i);
	}
	if((_ABS(1920 - vde)<2) && (_ABS(540 - hde)<2)
	//==============20130604
	//	&&(ABS(121 - vfreq)<5)
		&&(_ABS(59 - vfreq)<5)
	//==============20130604 END
		&&(_ABS(2200-htotal)<5)&&(_ABS(562-vtotal)<5))
	{
		return (MD_1920x1080_59_94i); // test
	}
	else if((_ABS(1920 - vde)<2) && (_ABS(540 - hde)<2)
	//==============20130604
	//	&&(ABS(101 - vfreq)<5)
		&&(_ABS(50 - vfreq)<5)
	//==============20130604 END
		&&(_ABS(2640-htotal)<5)&&(_ABS(562-vtotal)<5))
	{
		return (MD_1920x1080_50i);
	}
	else if((_ABS(1440 - vde)<2) && (_ABS(240 - hde)<2)
		&&(_ABS(59 - vfreq)<5)
		&&(_ABS(1716-htotal)<5)&&(_ABS(263-vtotal)<5))
	{
		return (MD_720x480_60i);
	}
	else if((_ABS(1440 - vde)<2) && (_ABS(288 - hde)<2)
		&&(_ABS(49 - vfreq)<5)
		&&(_ABS(1728-htotal)<5)&&(_ABS(313-vtotal)<5))
	{
		return (MD_720x576_50i);
	}
	//==============20130604

	else if((_ABS(720 - vde)<2) && (_ABS(240 - hde)<2)
		&&(_ABS(59 - vfreq)<5)
		&&(_ABS(858-htotal)<5)&&(_ABS(263-vtotal)<5))
	{
		return (MD_720x480_60i);
	}
	else if((_ABS(720 - vde)<2) && (_ABS(288 - hde)<2)
		&&(_ABS(49 - vfreq)<5)
		&&(_ABS(864-htotal)<5)&&(_ABS(313-vtotal)<5))
	{
		return (MD_720x576_50i);
	}
	//==============20130604 END
	else
		{return (MD_UNDEFINE);}

}

int getVideoFormatByTotal(int hde, int vde, int htotal, int vtotal, int vfreq)
{
	int i = 0;
	for(i=0; i<MD_INDEX_SIZE; i++)
	{
		Resolution_Struct res;
		res = GetResByVideoFormat(i);
		if((_ABS(res.DisplayHeight - vde)<2) && (_ABS(res.DisplayWidth - hde)<2))
		{
			if((_ABS(GetVFreqByVideoFormat(i)-vfreq)<5))
			{
				if((_ABS(GetHTotalByVideoFormat(i)-htotal)<5)
					&&(_ABS(GetVTotalByVideoFormat(i)-vtotal)<5))
				{
					break;
				}
			}
		}
	}
	if(i < MD_INDEX_SIZE)
	{
		return i;
	}
	else
	{
		return (isInterlaceFormat(hde, vde, htotal, vtotal, vfreq));
		//return MD_UNDEFINE;
	}
}

int getVideoFormatByActive(int hde, int vde, int htotal, int vtotal, int vfreq)
{
	int i = 0;
	for(i=0; i<MD_INDEX_SIZE; i++)
	{
		Resolution_Struct res;
		res = GetResByVideoFormat(i);
		if((_ABS(res.DisplayHeight - vde)<2) && (_ABS(res.DisplayWidth - hde)<2))
		{
			if((_ABS(GetVFreqByVideoFormat(i)-vfreq)<5))
			{
				{
					break;
				}
			}
		}
	}
	if(i < MD_INDEX_SIZE)
	{
		return i;
	}
	else
	{
		return (isInterlaceFormat(hde, vde, htotal, vtotal, vfreq));
		//return MD_UNDEFINE;
	}
}

int GetFormatByHdeVde(int hde, int vde, int vfreq)
{
	int i = 0;
	for(i=0; i<MD_INDEX_SIZE; i++)
	{
		Resolution_Struct res = GetResByVideoFormat(i);
		if((res.DisplayHeight == vde) && (res.DisplayWidth == hde))
		{
			if((_ABS((int)GetVFreqByVideoFormat(i)-vfreq)<2))
			{
				
				break;
			}
		}
	}
	if(i < MD_INDEX_SIZE)
	{
		return i;
	}
	else
	{
		return MD_UNDEFINE;
	}
}

/*
int GetFormatByLinesAndPixels(int line, int clock)
{
	char i;
	for(i=0;i<MD_CUSTOM;i++)
	{
		if((line>(tVideoDefine[i].vtotal - 2))
			&&(line<(tVideoDefine[i].vtotal + 2)) )
		{
				return i;
		}
	}
	return MD_INDEX_SIZE;
}
*/


// output format
const u8 outFormatList[41] = 
{
	//MD_800x600_60,

	MD_1024x768_60,
	MD_1024x768_75,
	MD_1024x768_85,
	MD_1152x864_75,	
	MD_1280x720_50,
	MD_1280x720_59_94,	
	MD_1280x720_60,
//	MD_1280x720_30, // 不支持，显示器不显示
//	MD_1280x720_29_97, // 不支持，显示器不显示
//	MD_1280x720_25, // 不支持，显示器不显示
//	MD_1280x720_24, // 不支持，显示器不显示
//	MD_1280x720_23_98, // 不支持，显示器不显示
	MD_1280x768_60,
	MD_1280x960_60, 
	MD_1280x960_85,	// 10
	MD_1280x1024_60,
	MD_1280x1024_75,
	MD_1280x1024_85,
	MD_1280x1280_60,
	MD_1360x768_60,
	MD_1366x768_60,
	MD_1400x1050_60_reduce,
	MD_1400x1050_60,
	MD_1440x900_60, 
	MD_1440x900_60_reduce, // 20
	MD_1600x1200_60,
	MD_1600x1200_60_reduce,
	MD_1680x1050_60,
	MD_1680x1050_60_reduce,
	MD_1920x1080_23_98,
	MD_1920x1080_23_98psf,
	MD_1920x1080_24,
	MD_1920x1080_24psf, 
	MD_1920x1080_25,  
	MD_1920x1080_25psf,	//30
	MD_1920x1080_29_97,
	MD_1920x1080_29_97psf,
	MD_1920x1080_30,
	MD_1920x1080_30psf,	
	MD_1920x1080_59_94,
	MD_1920x1080_50,
	MD_1920x1080_60,
	MD_1920x1080_60_reduce,
	MD_1920x1200_60, 
	MD_2048x1152_60, //40
	MD_INPUT_FORMAT, //41
};



