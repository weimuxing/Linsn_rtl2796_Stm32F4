#include "SiI9134.h"
#include "i2c.h"
#include "systick.h"
#include "videoFormatTable.h"
#include "UartDebug.h"


u8 format_avi[SiI9134_PORT_MAX_SIZE] = 
{ 
	MD_1920x1080_60,
	MD_1920x1080_60,
	MD_1920x1080_60,
	MD_1920x1080_60,
	MD_1920x1080_60,
};

/* Private functions ---------------------------------------------------------*/
void SiI9134WriteVideoReg(SiI9134PortTypeDef portx, BYTE ucSubAdr, BYTE ucVal)
{
	switch(portx)
	{
	case SiI9134_PORT_1:
		i2cWriteByte(I2C_A, SiI9134_VIDEO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_2:
		i2cWriteByte(I2C_A, SiI9134_VIDEO_ADDRESS+4,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_3:
		i2cWriteByte(I2C_B, SiI9134_VIDEO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_4:
		i2cWriteByte(I2C_B, SiI9134_VIDEO_ADDRESS+4,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_5:
		i2cWriteByte(I2C_C, SiI9134_VIDEO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_6:
		i2cWriteByte(I2C_C, SiI9134_VIDEO_ADDRESS+4,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_7:
		i2cWriteByte(I2C_D, SiI9134_VIDEO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_8:
		i2cWriteByte(I2C_D, SiI9134_VIDEO_ADDRESS+4,ucSubAdr,ucVal);
		break;
	}
}
BYTE SiI9134ReadVideoReg(SiI9134PortTypeDef portx, BYTE ucSubAdr)
{
	BYTE result = 0xFF;
	switch(portx)
	{
	case SiI9134_PORT_1:
		result = i2cReadByte(I2C_A, SiI9134_VIDEO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_2:
		result = i2cReadByte(I2C_A, SiI9134_VIDEO_ADDRESS+4,ucSubAdr);
		break;
	case SiI9134_PORT_3:
		result = i2cReadByte(I2C_B, SiI9134_VIDEO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_4:
		result = i2cReadByte(I2C_B, SiI9134_VIDEO_ADDRESS+4,ucSubAdr);
		break;
	case SiI9134_PORT_5:
		result = i2cReadByte(I2C_C, SiI9134_VIDEO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_6:
		result = i2cReadByte(I2C_C, SiI9134_VIDEO_ADDRESS+4,ucSubAdr);
		break;
	case SiI9134_PORT_7:
		result = i2cReadByte(I2C_D, SiI9134_VIDEO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_8:
		result = i2cReadByte(I2C_D, SiI9134_VIDEO_ADDRESS+4,ucSubAdr);
		break;
	}
	return result;
}

void SiI9134WriteAudioReg(SiI9134PortTypeDef portx, BYTE ucSubAdr, BYTE ucVal)
{
	switch(portx)
	{
	case SiI9134_PORT_1:
		i2cWriteByte(I2C_A, SiI9134_AUDIO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_2:
		i2cWriteByte(I2C_A, SiI9134_AUDIO_ADDRESS+4,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_3:
		i2cWriteByte(I2C_B, SiI9134_AUDIO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_4:
		i2cWriteByte(I2C_B, SiI9134_AUDIO_ADDRESS+4,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_5:
		i2cWriteByte(I2C_C, SiI9134_AUDIO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_6:
		i2cWriteByte(I2C_C, SiI9134_AUDIO_ADDRESS+4,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_7:
		i2cWriteByte(I2C_D, SiI9134_AUDIO_ADDRESS,ucSubAdr,ucVal);
		break;
	case SiI9134_PORT_8:
		i2cWriteByte(I2C_D, SiI9134_AUDIO_ADDRESS+4,ucSubAdr,ucVal);
	}
}

static BYTE SiI9134ReadAudioReg(SiI9134PortTypeDef portx, BYTE ucSubAdr)
{
	BYTE result = 0xFF;
	switch(portx)
	{
	case SiI9134_PORT_1:
		result = i2cReadByte(I2C_A, SiI9134_AUDIO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_2:
		result = i2cReadByte(I2C_A, SiI9134_AUDIO_ADDRESS+4,ucSubAdr);
		break;
	case SiI9134_PORT_3:
		result = i2cReadByte(I2C_B, SiI9134_AUDIO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_4:
		result = i2cReadByte(I2C_B, SiI9134_AUDIO_ADDRESS+4,ucSubAdr);
		break;
	case SiI9134_PORT_5:
		result = i2cReadByte(I2C_C, SiI9134_AUDIO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_6:
		result = i2cReadByte(I2C_C, SiI9134_AUDIO_ADDRESS+4,ucSubAdr);
		break;
	case SiI9134_PORT_7:
		result = i2cReadByte(I2C_D, SiI9134_AUDIO_ADDRESS,ucSubAdr);
		break;
	case SiI9134_PORT_8:
		result = i2cReadByte(I2C_D, SiI9134_AUDIO_ADDRESS+4,ucSubAdr);
		break;
	}
	return result;
}




static void DelaySil9134Rst595(void)
{
	#if 0
	char i;
	for(i = 0; i < 20; i++)
	{
		;
	}
	#endif
}

//###########################################################################################
//
void InitSil9134Rst595(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* configure SH_CLK pin */
	GPIO_InitStruct.GPIO_Pin = _9134_HC595_SH_CLK_PIN;
	GPIO_Init(_9134_HC595_SH_CLK_PORT,&GPIO_InitStruct);
	
	/* configure ST_CLK pin */
	GPIO_InitStruct.GPIO_Pin = _9134_HC595_ST_CLK_PIN;
	GPIO_Init(_9134_HC595_ST_CLK_PORT,&GPIO_InitStruct);

	/* configure SERIN pin */
	GPIO_InitStruct.GPIO_Pin = _9134_HC595_SERIN_PIN;
	GPIO_Init(_9134_HC595_SERIN_PORT,&GPIO_InitStruct);

}


void WriteSil9134Rst595(u8 value)
{
	int i;
	RESET_9134_HC595_ST_CLK();
	DelaySil9134Rst595();
	RESET_9134_HC595_SH_CLK();
	DelaySil9134Rst595();
	for(i=0; i<8; i++)
	{
		if(value & 0x80)
		{
			SET_9134_HC595_SERIN();
		}
		else
		{
			RESET_9134_HC595_SERIN();
		}
		value <<= 1;
		DelaySil9134Rst595();
		SET_9134_HC595_SH_CLK();
		DelaySil9134Rst595();
		RESET_9134_HC595_SH_CLK();
		DelaySil9134Rst595();
	}

	SET_9134_HC595_ST_CLK();
	DelaySil9134Rst595();
	RESET_9134_HC595_ST_CLK();
	DelaySil9134Rst595();
	RESET_9134_HC595_SH_CLK();
	DelaySil9134Rst595();
	RESET_9134_HC595_SERIN();
	DelaySil9134Rst595();
}




void SiI9134_Reset(void)
{
	#if 0 //由主板来执行复位
	GPIO_InitTypeDef GPIO_InitStruct;

	

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_ResetBits(GPIOC,GPIO_InitStruct.GPIO_Pin);
	SysDelay1ms(100);
	GPIO_SetBits(GPIOE,GPIO_InitStruct.GPIO_Pin);
	SysDelay1ms(10);  // 要延时一段时间才能设置寄存器
	#endif
	
	InitSil9134Rst595();

	WriteSil9134Rst595(0x00);
	SysDelay1ms(100);
	WriteSil9134Rst595(0xff);
	SysDelay1ms(10);  // 要延时一段时间才能设置寄存器
}



/* Exported functions ------------------------------------------------------- */
void SiI9134_PowerDown(SiI9134PortTypeDef portx)
{
	BYTE SYS_CTRL1;
	SYS_CTRL1 = SiI9134ReadVideoReg(portx, 0x08);
	SYS_CTRL1 &= (~0x01);
	SiI9134WriteVideoReg(portx,0x08,SYS_CTRL1);
}

void SiI9134_Init(SiI9134PortTypeDef portx)
{
	
	BYTE SYS_CTRL1;
	BYTE SYS_RES = 0;
	bool isDeEnable;
	
	SYS_CTRL1 = SiI9134ReadVideoReg(portx, 0x08);
	SYS_CTRL1 &= (~0x01);
	SiI9134WriteVideoReg(portx,0x08,SYS_CTRL1);
	SysDelay1ms(10);
	SYS_CTRL1 |= 0x01;
	SiI9134WriteVideoReg(portx,0x08,SYS_CTRL1);
	SYS_CTRL1 = 0;
	SYS_CTRL1 = SiI9134ReadVideoReg(portx, 0x08);

	SiI9134WriteVideoReg(portx,0x4a,0x00);  // disable data range

	//SiI9134WriteVideoReg(portx,0x82,0x24); //增加输出驱动力20140419
	SiI9134_SetAudioFormat(portx, 0);
#if 1
    SiI9134WriteAudioReg(portx,0x04,0x18);
	//SiI9134WriteAudioReg(portx,0x14,0x00); // no audio input
	SiI9134WriteAudioReg(portx,0x14,0x11);  // I2S input channel 0
    SiI9134WriteAudioReg(portx,0x15,0x00);
	SiI9134WriteAudioReg(portx,0x19,0x09);
	SiI9134WriteAudioReg(portx,0x1a,0x64);
	SiI9134WriteAudioReg(portx,0x1b,0x08);
	
	SiI9134WriteAudioReg(portx,0x1c,0xE4);
	SiI9134WriteAudioReg(portx,0x1d,0x40);
	SiI9134WriteAudioReg(portx,0x1f,0x19);
	SiI9134WriteAudioReg(portx,0x20,0x20);
	SiI9134WriteAudioReg(portx,0x21,0x02);
	
	SiI9134WriteAudioReg(portx,0x22,0xDB);
	SiI9134WriteAudioReg(portx,0x23,0x10);
	SiI9134WriteAudioReg(portx,0x24,0x9B);
	SiI9134WriteAudioReg(portx,0x3E,0x33);
	SiI9134WriteAudioReg(portx,0x3F,0x3C);

	SiI9134WriteAudioReg(portx,0x80,0x84);
	SiI9134WriteAudioReg(portx,0x81,0x01);
	SiI9134WriteAudioReg(portx,0x82,0x0A);
	SiI9134WriteAudioReg(portx,0x83,0x70);
	SiI9134WriteAudioReg(portx,0x84,0x01);
    SiI9134WriteAudioReg(portx,0xDF,0x10);
	
#endif
	//SiI9134_SetVideoFormat(portx, OutputMode_xga);

	isDeEnable = SiI9134_GetEnableDE(portx);
	
}


#if !defined SiI9134_ENABLE_DE_GENERATE
#define SiI9134_ENABLE_DE_GENERATE 1
#endif
void SiI9134_SetVideoFormat(SiI9134PortTypeDef portx, int format)
{   
	int i;
 	switch(format)
 	{
 	case OutputMode_480p:
		{
			// P480_60
			
		 	u8 RegValue[][2] = 
		 	#if !SiI9134_ENABLE_DE_GENERATE
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x00},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 	};
			#else
			{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0x44},
		 		{0x34, 0x20},  
		 		{0x36, 0xd0},
		 		{0x37, 0x02},
		 		{0x38, 0xe0},
		 		{0x39, 0x01},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			#endif
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
 		}
		break;
	case OutputMode_576p:
		{
			 // P576_50
		 	u8 RegValue[][2] = 
		 	#if !SiI9134_ENABLE_DE_GENERATE
		 	{     //address,    value
		 		{0x08, 0x37},
		 		{0x33, 0x00},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 	};
			#else
			{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0x46},
		 		{0x34, 0x20},  
		 		{0x36, 0xd0},
		 		{0x37, 0x02},
		 		{0x38, 0x40},
		 		{0x39, 0x02},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			#endif
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	case OutputMode_720p50a:
	case OutputMode_720p:
		{
			// P720_60
		 	u8 RegValue[][2] = 
		 	#if !SiI9134_ENABLE_DE_GENERATE
		 	{     //address,    value
		 		{0x08, 0x37},
		 		{0x33, 0x00},
		 		{0x48, 0x18},
				{0x4a, 0x38},
		 	};
			#else
			{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0xE4},
		 		{0x34, 0x17},  
		 		{0x36, 0x00},
		 		{0x37, 0x05},
		 		{0x38, 0xD0},
		 		{0x39, 0x02},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			#endif
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	case OutputMode_576i100:
	case OutputMode_576i75:
		{
			// i576_50
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},
		 		{0x33, 0x00},
		 		{0x48, 0x20},
				{0x4a, 0x3e},
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	#if SiI9134_ENABLE_DE_GENERATE
	case OutputMode_1080i:
		{
			
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0x96},  // Hstart
		 		{0x34, 0x06},  // Vstart
		 		{0x36, 0x80},  // 1920
		 		{0x37, 0x07},
		 		{0x38, 0x1c}, // 540
		 		{0x39, 0x02},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	case OutputMode_1080i50:
		{
			
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0x8E},  // Hstart
		 		{0x34, 0x0D},  // Vstart
		 		{0x36, 0x80},  // 1920
		 		{0x37, 0x07},
		 		{0x38, 0x1c}, // 540
		 		{0x39, 0x02},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
		
	#else
	case OutputMode_1080i:
	case OutputMode_1080i50:
	#endif
	case OutputMode_576i50:
		{
			// i1080_60 i1080_50
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},
		 		{0x33, 0x00},
		 		{0x48, 0x18},
				{0x4a, 0x38},
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	
	case OutputMode_480i60:
		{
			// i1080_60 i1080_50
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},
		 		{0x33, 0x00},
		 		{0x48, 0x18},
				{0x4a, 0x38},
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	
	#if SiI9134_ENABLE_DE_GENERATE
	case OutputMode_xga:		
		{
			// i576_50
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0xAA},  // Hstart
		 		{0x34, 0x17},  // Vstart
		 		{0x36, 0x00},  // 1024
		 		{0x37, 0x04},
		 		{0x38, 0x00}, // 768
		 		{0x39, 0x03},
		 		{0x48, 0x00},
				{0x4a, 0x00},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	case OutputMode_800X600:
		{
			// 800X600
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0x4A},  // Hstart
		 		{0x34, 0x17},  // Vstart
		 		{0x36, 0x20},  // 800
		 		{0x37, 0x03},
		 		{0x38, 0x58}, // 600
		 		{0x39, 0x02},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	case OutputMode_1280X768:
		{
			
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0xA4},  // Hstart
		 		{0x34, 0x14},  // Vstart
		 		{0x36, 0x00},  // 1280
		 		{0x37, 0x05},
		 		{0x38, 0x00}, // 768
		 		{0x39, 0x03},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	case OutputMode_1280X1024:
		{			
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0xED},  // Hstart
		 		{0x34, 0x26},  // Vstart
		 		{0x36, 0x00},  // 1280
		 		{0x37, 0x05},
		 		{0x38, 0x00}, // 1024
		 		{0x39, 0x04},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	case OutputMode_1080p60:
	case OutputMode_1080p50:
		{			
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},		 		
		 		{0x33, 0x70}, // enable generate de
		 		{0x32, 0x90},  // Hstart
		 		{0x34, 0x28},  // Vstart
		 		{0x36, 0x80},  // 1920
		 		{0x37, 0x07},
		 		{0x38, 0x38}, // 1080
		 		{0x39, 0x04},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 		{0x41, 0x00},
		 		{0x40, 0x10},
		 		{0x45, 0x00},
		 		{0x44, 0x3E},
		 		{0x46, 0x09},
		 		{0x47, 0x06},	
		 			
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	
		
	#else
	case OutputMode_xga:
	case OutputMode_800X600:
	case OutputMode_1280X768:
	case OutputMode_1280X1024:
	case OutputMode_1080p60:
	case OutputMode_1080p50:
		{
			 // P576_50
		 	u8 RegValue[][2] = 
		 	{     //address,    value
		 		{0x08, 0x37},
		 		{0x33, 0x00},
		 		{0x48, 0x00},
				{0x4a, 0x38},
		 	};
			int count = sizeof(RegValue) / 2;
			for(i=0; i<count; i++)
			{
				SiI9134WriteVideoReg(portx, RegValue[i][0], RegValue[i][1]);
			}
		}
		break;
	#endif
	
	
	
	
	
		
		
	
	default:
		break;
		
 	}
}



void SiI9134_SetAudioFormat(SiI9134PortTypeDef portx, int format)

{
	
	u8 VolRegValue[][2] = 
	{		
		//address, value
		{0x03, 0x00},
		{0x04, 0x30},
		{0x05, 0x00},
		{0x21, 0x02},
		{0x22, 0x21},
		{0x1D, 0x40},
		{0x14, 0x11},
		//{0x2F, 0x01},  // hdmi
		{0x2F, 0x00},  // dvi
		{0x0D, 0x00},
	};
	int i;

	format = format;
	
	for(i=0; i<(sizeof(VolRegValue) / 2); i++)
	{
		SiI9134WriteAudioReg(portx, VolRegValue[i][0], VolRegValue[i][1]);
	}
}

void SiI9134_SetOutputIsHDMI(SiI9134PortTypeDef portx, bool isHdmi)
{
	int result = SiI9134ReadAudioReg(portx, 0x2f);
	//printf("SiI9134_SetOutputIsHDMI(%d,%d)",portx,isHdmi);

	if(isHdmi)
	{
		result |= 0x01;
		SiI9134WriteAudioReg(portx, 0x2F, result);
		SiI9134WriteAudioReg(portx, 0x3E, 0x33);
		SiI9134WriteAudioReg(portx, 0x3F, 0x3C);
		SiI9134_Set_AVI_Info(portx,format_avi[portx]);
	}
	else
	{
		result &= ~(0x01);
		SiI9134WriteAudioReg(portx, 0x2F, result);
	}
}

bool SiI9134_GetOutputIsHDMI(SiI9134PortTypeDef portx)
{
	int result;
	result = SiI9134ReadAudioReg(portx, 0x2f);
	if(result & 0x01)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// DE adjust
void SiI9134_SetEnableDE(SiI9134PortTypeDef portx, bool enable)
{
	BYTE value = SiI9134ReadVideoReg(portx,0x33);
	if(enable)
	{
		value |= (BIT6);
	}
	else
	{
		value &= (~BIT6);
	}
	SiI9134WriteVideoReg(portx,0x33,value);
}
bool SiI9134_GetEnableDE(SiI9134PortTypeDef portx)
{
	BYTE value = SiI9134ReadVideoReg(portx,0x33);
	if(value & BIT6)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
void SiI9134_SetDE_TOP(SiI9134PortTypeDef portx, int DE_TOP)
{
	SiI9134WriteVideoReg(portx,0x34,(BYTE)DE_TOP);
}
int SiI9134_GetDE_TOP(SiI9134PortTypeDef portx)
{
	return SiI9134ReadVideoReg(portx,0x34);
}
void SiI9134_SetDE_DLY(SiI9134PortTypeDef portx, int DE_DLY)
{
	BYTE value = SiI9134ReadVideoReg(portx,0x33);
	SiI9134WriteVideoReg(portx,0x32,(BYTE)DE_DLY);
	value &= (~(0x0F));
	value |= ((DE_DLY>>8) & 0x0F);
	SiI9134WriteVideoReg(portx,0x33,value);
}
int SiI9134_GetDE_DLY(SiI9134PortTypeDef portx)
{
	int result = SiI9134ReadVideoReg(portx,0x32);
	result += ((SiI9134ReadVideoReg(portx,0x33)&0x0F)<<8);
	return result;
}
void SiI9134_SetDE_WIDTH(SiI9134PortTypeDef portx, int DE_WIDTH)
{
	BYTE value = SiI9134ReadVideoReg(portx,0x37);
	SiI9134WriteVideoReg(portx,0x36,(BYTE)DE_WIDTH);
	value &= (~(0x0f));
	value |= ((DE_WIDTH>>8) & 0x0f);
	SiI9134WriteVideoReg(portx,0x37,value);
}
int SiI9134_GetDE_WIDTH(SiI9134PortTypeDef portx)
{
	int result = SiI9134ReadVideoReg(portx,0x36);
	result += ((SiI9134ReadVideoReg(portx,0x37)&0x0f)<<8);
	return result;
}
void SiI9134_SetDE_HEIGHT(SiI9134PortTypeDef portx, int DE_HEIGHT)
{
	BYTE value = SiI9134ReadVideoReg(portx,0x39);
	SiI9134WriteVideoReg(portx,0x38,(BYTE)DE_HEIGHT);
	value &= (~(0x07));
	value |= ((DE_HEIGHT>>8) & 0x07);
	SiI9134WriteVideoReg(portx,0x39,value);
}
int SiI9134_GetDE_HEIGHT(SiI9134PortTypeDef portx)
{
	int result = SiI9134ReadVideoReg(portx,0x38);
	result += ((SiI9134ReadVideoReg(portx,0x39)&0x07)<<8);
	return result;
}
/*
 *
 *  HsVs -- BIT0 - Hs,  BIT1 - Vs
 */
void SiI9134_SetHsVs(SiI9134PortTypeDef portx, int HsVs)
{
	BYTE value = SiI9134ReadVideoReg(portx,0x33);
	HsVs &= 0x03;
	HsVs <<= 4;
	value &= (~ 0x30);
	value |= HsVs;
	
	SiI9134WriteVideoReg(portx,0x33, value);
}
/*
 *
 *  Return:  BIT0 - Hs,  BIT1 - Vs
 */

int SiI9134_GetHsVs(SiI9134PortTypeDef portx)
{
	int result = SiI9134ReadVideoReg(portx,0x33);
	result >>= 4;
	result &= 0x03;
	return result;
}

void SiI9134_SetDataRange(SiI9134PortTypeDef portx, int enable) // Data Range 16-235 To 0-255
{
	BYTE value = SiI9134ReadVideoReg(portx,0x4a);
	if(enable)
	{
		value |= 0x10;
	}
	else
	{
		value &= (~0x10);
	}	
	SiI9134WriteVideoReg(portx,0x4a, value);
}


void SiI9134_SoftReset(SiI9134PortTypeDef portx)
{
	BYTE SYS_CTRL1;
	SYS_CTRL1 = SiI9134ReadVideoReg(portx, 0x08);
	SYS_CTRL1 &= (~0x01);
	SiI9134WriteVideoReg(portx,0x08,SYS_CTRL1);
	SysDelay1ms(10);
	SYS_CTRL1 |= 0x01;
	SiI9134WriteVideoReg(portx,0x08,SYS_CTRL1);
}
#if 1
void SiI9134_Set_AVI_Info(SiI9134PortTypeDef portx,u8 Format)
{

	u8 CheckSum_Val;
	int i;
	u8 temp;
	u8 temp1 = 0;

	format_avi[portx] = Format;
	
	CheckSum_Val = 0;
	temp = 0x82;
	SiI9134WriteAudioReg(portx,0x40,temp);	
	CheckSum_Val += temp;
	
	temp = 0x02;
	SiI9134WriteAudioReg(portx,0x41,temp);
	CheckSum_Val += temp;
	
	temp = 0x0D;
	SiI9134WriteAudioReg(portx,0x42,temp);
	CheckSum_Val += temp;
	

	temp = 0x00;
	SiI9134WriteAudioReg(portx,0x44,temp);
	CheckSum_Val += temp;

	
	
	temp = 0x00;
	SiI9134WriteAudioReg(portx,0x46,temp);
	
	CheckSum_Val += temp;


	switch(Format)
	{
		case MD_720x480_60i: //"487i @ 60", // 3
//		case MD_1440X240:
			temp = 0x58;
			temp1 = 6;
			break;
		case MD_720x576_50i:	//"576i @ 50", // 4
//		case MD_1440X288:
			temp = 0x58;
			temp1 = 21;
			break;
		case MD_720x480_60p:	//"483P @ 60", // 9
			temp = 0x58;
			temp1 = 2;
			break;
		case MD_720x576_50p://	"576P @ 50", // 10
			temp = 0x58;
			temp1 = 17;
			break;
		case MD_1280x720_23_98://	"720P @ 23.98", // 18
			temp = 0xA8;
			temp1 = 60;
			break;
		case MD_1280x720_24://"720P @ 24", // 17
			temp = 0xA8;
			temp1 = 60;
			break;
		case MD_1280x720_25://	"720P @ 25", // 16
			temp = 0xA8;
			temp1 = 61;
			break;
		case MD_1280x720_29_97://	"720P @ 29.97", // 15
			temp = 0xA8;
			temp1 = 62;
			break;
		case MD_1280x720_30://	"720P @ 30", // 14
			temp = 0xA8;
			temp1 = 62;	
			break;
		case MD_1280x720_50://	"720P @ 50", // 13
			temp = 0xA8;
			temp1 = 19;	
			break;
		case MD_1280x720_59_94://	"720P @ 59.94", // 12			
		case MD_1280x720_60://	"720P @ 60", // 11
			temp = 0xA8;
			temp1 = 4;
			break;
		case MD_1920x1080_50i://	"1080i @ 50", // 27
			temp = 0xA8;
			temp1 = 20;
			break;
		case MD_1920x1080_59_94i://	"1080i @ 59.94", // 26
			temp = 0xA8;
			temp1 = 5;
			break;
		case MD_1920x1080_60i://	"1080i @ 60", // 25
			temp = 0xA8;
			temp1 = 5;
			break;
		case MD_1920x1080_23_98://	"1080P @ 23.98", // 37 
			temp = 0xA8;
			temp1 = 32;
			break;
		case MD_1920x1080_24://	"1080P @ 24", // 35
			temp = 0xA8;
			temp1 = 32;
			break;
		case MD_1920x1080_25://	"1080P @ 25", // 33
			temp = 0xA8;
			temp1 = 33;
			break;
		case MD_1920x1080_29_97://	"1080P @ 29.97", // 31
			temp = 0xA8;
			temp1 = 34;
			break;
		case MD_1920x1080_30://	"1080P @ 30", // 29
			temp = 0xA8;
			temp1 = 34;
			break;
		case MD_1920x1080_50://	"1080P @ 50", // 23
			temp = 0xA8;
			temp1 = 31;
			break;
		case MD_1920x1080_59_94://	"1080P @ 59.94", // 22
			temp = 0xA8;
			temp1 = 16;
			break;
		case MD_1920x1080_60://	"1080P @ 60", // 21
			temp = 0xA8;
			temp1 = 16;
			break;
		default:
			break;
		
		
	}
	SiI9134WriteAudioReg(portx,0x45,temp);
	CheckSum_Val += temp;
	SiI9134WriteAudioReg(portx,0x47,temp1);
	CheckSum_Val += temp1;
	if((Format == MD_720x480_60i)||(Format == MD_720x576_50i))
	{
		SiI9134WriteAudioReg(portx,0x48,1); // Pixel Repeated
		CheckSum_Val += 1;
	}
	else
	{
		SiI9134WriteAudioReg(portx,0x48,0); // Pixel Repeated
	}
	for(i=0; i<8; i++)
	{
		CheckSum_Val += SiI9134ReadAudioReg(portx,0x49+i);
	}
	CheckSum_Val = 0 - CheckSum_Val;
	SiI9134WriteAudioReg(portx,0x43,CheckSum_Val);
}

void SiI9134_SetOutput8Bits(SiI9134PortTypeDef portx)
{
	int result;

	//SiI9134_PowerDown(portx);

	result = SiI9134ReadVideoReg(portx,0x49);
	
	result &= ~0xC0;

	SiI9134WriteVideoReg(portx,0x49,result);

	SiI9134WriteVideoReg(portx,0x83,0x1B);
	
	SiI9134WriteAudioReg(portx,0x2F,0x60);//SiI9134WriteAudioReg(portx,0x2F,0x61);

	result = SiI9134ReadVideoReg(portx,0x49);

	result &= ~0xC0;

	SiI9134WriteVideoReg(portx,0x49,result);

	
	result = SiI9134ReadVideoReg(portx,0x4A);	
	
	result &= ~0xE0;

	SiI9134WriteVideoReg(portx,0x4A,result);

	SiI9134WriteVideoReg(portx,0x05,0x01); // software reset
	
	SysDelay1ms(10);
	SiI9134WriteVideoReg(portx,0x05,0x00); // software reset
	SysDelay1ms(10);
	SiI9134_Init(portx);
	SiI9134_Set_AVI_Info( portx,format_avi[portx]);
	SiI9134WriteAudioReg(portx,0x2F,0x60);//SiI9134WriteAudioReg(portx,0x2F,0x61);
	

	//SiI9134_SoftReset((SiI9134PortTypeDef) portx);
}


void SiI9134_SetOutput10Bits(SiI9134PortTypeDef portx)
{
	int result;
	
	SiI9134WriteAudioReg(portx,0x2F,0x69);

	result = SiI9134ReadVideoReg(portx,0x49);

	result |= 0x40;
	result &= ~0x80;

	SiI9134WriteVideoReg(portx,0x49,result);

	result = SiI9134ReadVideoReg(portx,0x4A);

	result |= 0x60;
	result &= ~0x80;

	SiI9134WriteVideoReg(portx,0x4A,result);	

	SiI9134WriteVideoReg(portx,0x83,0x23);	
	
}

void SiI9134_SetOutput12Bits(SiI9134PortTypeDef portx)
{
	int result;
	
	SiI9134WriteAudioReg(portx,0x2F,0x71);

	result = SiI9134ReadVideoReg(portx,0x49);

	result &= ~0x40;
	result |= 0x80;

	SiI9134WriteVideoReg(portx,0x49,result);

	result = SiI9134ReadVideoReg(portx,0x4A);

	result |= 0xA0;
	result &= ~0x40;

	SiI9134WriteVideoReg(portx,0x4A,result);

	

	SiI9134WriteVideoReg(portx,0x83,0x2B);

	
	
}

	

void SiI9134_SetBitDepth(SiI9134PortTypeDef portx, int bitWidth) // bitWidth: 0 -- 8bit, 1 -- 10bit, 2 -- 12bit
{
	//if((SiI9134_PORT_1<=portx) && (portx <= SiI9134_PORT_5))
	if(portx <= SiI9134_PORT_MAX_SIZE)
	{
		if(bitWidth == 0)
		{
			SiI9134_SetOutput8Bits( portx);
		}
		else if(bitWidth == 1)
		{
			SiI9134_SetOutput10Bits( portx);
		}
		else if(bitWidth == 2)
		{
			SiI9134_SetOutput12Bits( portx);
		}
	}
}	
#endif

char seleCheck_Si9134(SiI9134PortTypeDef portx)
{
	char res,writeReg,readReg;
	res = TRUE;
	{
		char regValue[5] = {0x01,0x19,0xc8,0x27,0x5a},index;
		for(index = 0; index < 5; index++)
		{
			writeReg = regValue[index];
			SiI9134WriteVideoReg(portx,0x4a,writeReg);
			readReg = SiI9134ReadVideoReg(portx,0x4a);
			if((readReg == 0xff)||(readReg != writeReg))
			{
				res = FALSE;
				return res;
			}					
		}
	}
	if(res == TRUE)
	{
		char regValue[5] = {0x00,0x02,0x03,0x05,0x07},index;
		for(index = 0; index < 5; index++)
		{
			writeReg = regValue[index];
			SiI9134WriteAudioReg(portx,0x02,writeReg);
			readReg = SiI9134ReadAudioReg(portx,0x02);
			if((readReg == 0xff)||(readReg != writeReg))
			{
				res = FALSE;
				return res;
			}					
		}
	}		
	return res;
}



void OutputTypeInit(OUTPUT_TYPE_PARAM_Typedef *stOutputType)
{
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_1,stOutputType->output1Type);
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_2,stOutputType->output1Type);
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_3,stOutputType->output0Type);
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_4,stOutputType->output0Type);
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_5,stOutputType->output2Type);
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_6,stOutputType->output2Type);
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_7,stOutputType->output3Type);
	SiI9134_SetOutputIsHDMI(SiI9134_PORT_8,stOutputType->output3Type);

	SiI9134_SetBitDepth(SiI9134_PORT_1,stOutputType->output1BitWidth);
	SiI9134_SetBitDepth(SiI9134_PORT_2,stOutputType->output1BitWidth);
	SiI9134_SetBitDepth(SiI9134_PORT_3,stOutputType->output0BitWidth);
	SiI9134_SetBitDepth(SiI9134_PORT_4,stOutputType->output0BitWidth);
	SiI9134_SetBitDepth(SiI9134_PORT_5,stOutputType->output2BitWidth);
	SiI9134_SetBitDepth(SiI9134_PORT_6,stOutputType->output2BitWidth);
	SiI9134_SetBitDepth(SiI9134_PORT_7,stOutputType->output3BitWidth);
	SiI9134_SetBitDepth(SiI9134_PORT_8,stOutputType->output3BitWidth);

	SiI9134_SetDataRange(SiI9134_PORT_1,stOutputType->output1Range);
	SiI9134_SetDataRange(SiI9134_PORT_2,stOutputType->output1Range);
	SiI9134_SetDataRange(SiI9134_PORT_3,stOutputType->output0Range);
	SiI9134_SetDataRange(SiI9134_PORT_4,stOutputType->output0Range);
	SiI9134_SetDataRange(SiI9134_PORT_5,stOutputType->output2Range);
	SiI9134_SetDataRange(SiI9134_PORT_6,stOutputType->output2Range);
	SiI9134_SetDataRange(SiI9134_PORT_7,stOutputType->output3Range);
	SiI9134_SetDataRange(SiI9134_PORT_8,stOutputType->output3Range);
	
}






