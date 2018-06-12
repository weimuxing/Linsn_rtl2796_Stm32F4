#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "uart.h"
#include "SysConfig.h"
#include "control.h"
#include "i2c.h"
#include "VideoFormatTable.h"
#include "Input.h"
#include "IT6604.h"
#include "UartDebug.h"




const char it6604_i2c_list[] =
{
#if 0
				// I2C port, I2C Slave Address
/*IT6604_PORT_1*/	I2C_B,		IT6604_I2C_ADDRESS_1,
/*IT6604_PORT_2*/	I2C_B,		IT6604_I2C_ADDRESS_2,
/*IT6604_PORT_3*/	I2C_C,		IT6604_I2C_ADDRESS_1,
/*IT6604_PORT_4*/	I2C_C,		IT6604_I2C_ADDRESS_2,
/*IT6604_PORT_5*/	I2C_D,		IT6604_I2C_ADDRESS_1,
/*IT6604_PORT_6*/	I2C_D,		IT6604_I2C_ADDRESS_2,
/*IT6604_PORT_7*/	I2C_A,		IT6604_I2C_ADDRESS_1,
/*IT6604_PORT_8*/	I2C_A,		IT6604_I2C_ADDRESS_2,
#else
				// I2C port, I2C Slave Address
/*IT6604_PORT_1*/	I2C_A,		IT6604_I2C_ADDRESS_2,
#endif



};

BYTE bCSCOffset_16_235[] =
{
    0x00,0x80,0x00
};

BYTE bCSCOffset_0_255[]=
{
	0x10,0x80,0x10
};

BYTE bCSCMtx_RGB2YUV_ITU601_16_235[] =
{
	0xB2,0x04,0x64,0x02,0xE9,0x00,
	0x93,0x3C,0x18,0x04,0x56,0x3F,
	0x49,0x3D,0x9F,0x3E,0x18,0x04
} ;

BYTE  bCSCMtx_RGB2YUV_ITU601_0_255[] =
{
	0x09,0x04,0x0E,0x02,0xC8,0x00,
	0x0E,0x3D,0x84,0x03,0x6E,0x3F,
	0xAC,0x3D,0xD0,0x3E,0x84,0x03
} ;

BYTE  bCSCMtx_RGB2YUV_ITU709_16_235[] =
{
	0xB8,0x05,0xB4,0x01,0x93,0x00,
	0x49,0x3C,0x18,0x04,0x9F,0x3F,
	0xD9,0x3C,0x10,0x3F,0x18,0x04
} ;

BYTE  bCSCMtx_RGB2YUV_ITU709_0_255[] =
{
	0xE5,0x04,0x78,0x01,0x81,0x00,
	0xCE,0x3C,0x84,0x03,0xAE,0x3F,
	0x49,0x3D,0x33,0x3F,0x84,0x03
} ;

BYTE bCSCMtx_YUV2RGB_ITU601_16_235[] =
{
    0x00,0x08,0x6A,0x3A,0x4F,0x3D,
    0x00,0x08,0xF7,0x0A,0x00,0x00,
    0x00,0x08,0x00,0x00,0xDB,0x0D
} ;

BYTE bCSCMtx_YUV2RGB_ITU601_0_255[] =
{
    0x4F,0x09,0x81,0x39,0xDF,0x3C,
    0x4F,0x09,0xC2,0x0C,0x00,0x00,
    0x4F,0x09,0x00,0x00,0x1E,0x10
} ;

BYTE bCSCMtx_YUV2RGB_ITU709_16_235[] =
{
    0x00,0x08,0x53,0x3C,0x89,0x3E,
    0x00,0x08,0x51,0x0C,0x00,0x00,
    0x00,0x08,0x00,0x00,0x87,0x0E
} ;

BYTE bCSCMtx_YUV2RGB_ITU709_0_255[] =
{
    0x4F,0x09,0xBA,0x3B,0x4B,0x3E,
    0x4F,0x09,0x56,0x0E,0x00,0x00,
    0x4F,0x09,0x00,0x00,0xE7,0x10
} ;


static int IT6604_InputFormat = MD_NOINPUT;
static int IT6604_FormatLast = 0xff;
static int IT6604_InterlaceInput = 0xff; //   1----I制,采用YUV色空间      0-----其他,采用RGB色空间

//VideoFormat_TypeDef getInputFormat(IT6604_PortTypeDef IT6604_PORT_x);



static void WriteRegisterIT6604(IT6604_PortTypeDef IT6604_PORT_x, int address, int value)
{
	i2cWriteByte((I2CPort_TypeDef)it6604_i2c_list[IT6604_PORT_x*2], it6604_i2c_list[IT6604_PORT_x*2+1], address, value);	
}
static void WriteRegisterIT6604Buffer(IT6604_PortTypeDef IT6604_PORT_x, int address,BYTE len, BYTE *value)
{
	i2cBurstWriteBytes((I2CPort_TypeDef)it6604_i2c_list[IT6604_PORT_x*2], it6604_i2c_list[IT6604_PORT_x*2+1], address, value,len);	
}


static int ReadRegisterIT6604(IT6604_PortTypeDef IT6604_PORT_x, int address)
{
	return i2cReadByte((I2CPort_TypeDef)it6604_i2c_list[IT6604_PORT_x*2], it6604_i2c_list[IT6604_PORT_x*2+1], address);	
	
}

void ResetIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	#if 0 //复位6604由主板103完成
	GPIO_InitTypeDef GPIO_InitStruct;

	static bool isReset = FALSE;

	if(!isReset)
	{
		isReset = TRUE;

		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
		GPIO_Init(GPIOC, &GPIO_InitStruct);
		GPIO_ResetBits(GPIOC,GPIO_InitStruct.GPIO_Pin);
		SysDelay1ms(10);
		GPIO_SetBits(GPIOC,GPIO_InitStruct.GPIO_Pin);
		SysDelay1ms(10);
	}
	#endif
}

void InitIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	DebugMessageIt6604("%s\r\n",__func__);

	ResetIT6604(IT6604_PORT_x);

	#if 1
	WriteRegisterIT6604(IT6604_PORT_x,REG_RX_RST_CTRL,B_REGRST);
	//WriteRegisterIT6604(IT6604_PORT_x,REG_RX_VIDEO_MAP,B_SWAP_OUTRB);	 //会导致有时候出现红蓝反色哟 //20150325
	#else
	WriteRegisterIT6604(IT6604_PORT_x, 0x05, 0xff );
	SysDelay1ms(10);

	WriteRegisterIT6604(IT6604_PORT_x, 0x00, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x01, 0xf0 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x02, 0x23 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x03, 0x60 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x04, 0xa1 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x05, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x06, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x07, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x08, 0x9b );
	WriteRegisterIT6604(IT6604_PORT_x, 0x09, 0x99 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x0A, 0x2a );
	WriteRegisterIT6604(IT6604_PORT_x, 0x0B, 0xa5 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x0C, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x0D, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x0E, 0x04 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x0F, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x10, 0xb7 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x11, 0x89 );  // 解决HDCP的问题
	WriteRegisterIT6604(IT6604_PORT_x, 0x12, 0x45 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x13, 0x35 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x14, 0x5c );
	WriteRegisterIT6604(IT6604_PORT_x, 0x15, 0x03 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x16, 0x3f );
	WriteRegisterIT6604(IT6604_PORT_x, 0x17, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x18, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x19, 0x30 );
	
	WriteRegisterIT6604(IT6604_PORT_x, 0x1A, 0x14 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x1B, 0x00 );
//	WriteRegisterIT6604(IT6604_PORT_x, 0x1A, 0x14 );
//	WriteRegisterIT6604(IT6604_PORT_x, 0x1B, 0x40 );
	
	WriteRegisterIT6604(IT6604_PORT_x, 0x1C, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x1D, 0x20 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x1E, 0xda );
	WriteRegisterIT6604(IT6604_PORT_x, 0x1F, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x20, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x21, 0x10 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x22, 0x80 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x23, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x24, 0xb2 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x25, 0x04 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x26, 0x64 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x27, 0x02 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x28, 0xe9 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x29, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x2A, 0x93 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x2B, 0x1c );
	WriteRegisterIT6604(IT6604_PORT_x, 0x2C, 0x16 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x2D, 0x04 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x2E, 0x56 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x2F, 0x1f );
	WriteRegisterIT6604(IT6604_PORT_x, 0x30, 0x49 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x31, 0x1d );
	WriteRegisterIT6604(IT6604_PORT_x, 0x32, 0x9f );
	WriteRegisterIT6604(IT6604_PORT_x, 0x33, 0x1e );
	WriteRegisterIT6604(IT6604_PORT_x, 0x34, 0x16 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x35, 0x04 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x36, 0x0e );
	WriteRegisterIT6604(IT6604_PORT_x, 0x37, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x38, 0x5f );
	WriteRegisterIT6604(IT6604_PORT_x, 0x39, 0x06 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x3A, 0x80 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x3B, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x3C, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x3D, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x3E, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x3F, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x40, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x41, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x42, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x43, 0x89 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x44, 0x90 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x45, 0x35 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x46, 0x23 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x47, 0x30 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x48, 0x20 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x49, 0xff );
	WriteRegisterIT6604(IT6604_PORT_x, 0x4A, 0xf7 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x4B, 0x7f );
	WriteRegisterIT6604(IT6604_PORT_x, 0x4C, 0x59 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x4D, 0xf3 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x4E, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x4F, 0x4d );
	WriteRegisterIT6604(IT6604_PORT_x, 0x50, 0xc0 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x51, 0x20 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x52, 0x0c );
	WriteRegisterIT6604(IT6604_PORT_x, 0x53, 0x52 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x54, 0x10 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x55, 0x10 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x56, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x57, 0x08 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x58, 0x09 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x59, 0x98 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x5A, 0x78 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x5B, 0x80 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x5C, 0x2c );
	WriteRegisterIT6604(IT6604_PORT_x, 0x5D, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x5E, 0x58 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x5F, 0x65 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x60, 0x44 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x61, 0x38 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x62, 0x29 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x63, 0x04 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x64, 0x18 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x65, 0x03 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x66, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x67, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x68, 0x04 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x69, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x6A, 0xa8 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x6B, 0x10 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x6C, 0x03 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x6D, 0x64 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x6E, 0x0c );
	WriteRegisterIT6604(IT6604_PORT_x, 0x6F, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x70, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x71, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x72, 0x22 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x73, 0x31 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x74, 0xc1 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x75, 0x60 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x76, 0xe4 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x77, 0x20 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x78, 0xc1 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x79, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x7A, 0x18 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x7B, 0x80 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x7C, 0x97 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x7D, 0x06 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x7E, 0x02 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x7F, 0x80 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x80, 0x2d );
	WriteRegisterIT6604(IT6604_PORT_x, 0x81, 0x90 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x82, 0x97 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x83, 0x06 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x84, 0x62 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x85, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x86, 0x20 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x87, 0x08 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x88, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x89, 0x80 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x8A, 0x81 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x8B, 0x0c );
	WriteRegisterIT6604(IT6604_PORT_x, 0x8C, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x8D, 0xbf );
	WriteRegisterIT6604(IT6604_PORT_x, 0x8E, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x8F, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x90, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x91, 0x08 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x92, 0x09 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x93, 0xf8 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x94, 0xbc );
	WriteRegisterIT6604(IT6604_PORT_x, 0x95, 0x8d );
	WriteRegisterIT6604(IT6604_PORT_x, 0x96, 0xff );
	WriteRegisterIT6604(IT6604_PORT_x, 0x97, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x98, 0x8d );
	WriteRegisterIT6604(IT6604_PORT_x, 0x99, 0x2e );
	WriteRegisterIT6604(IT6604_PORT_x, 0x9A, 0x9e );
	WriteRegisterIT6604(IT6604_PORT_x, 0x9B, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x9C, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x9D, 0xf9 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x9E, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0x9F, 0x20 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA0, 0x07 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA1, 0x14 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA2, 0x5e );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA3, 0x62 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA4, 0xc6 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA5, 0xcf );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA6, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA7, 0x0a );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA8, 0x83 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xA9, 0x01 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xAA, 0x08 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xAB, 0x0d );
	WriteRegisterIT6604(IT6604_PORT_x, 0xAC, 0x02 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xAD, 0xf7 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xAE, 0x40 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xAF, 0x28 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB0, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB1, 0x10 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB2, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB3, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB4, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB5, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB6, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB7, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB8, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xB9, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xBA, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xBB, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xBC, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xBD, 0x83 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xBE, 0x01 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xBF, 0x19 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC0, 0x61 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC1, 0x53 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC2, 0x43 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC3, 0x45 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC4, 0x49 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC5, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC6, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC7, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC8, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xC9, 0x50 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xCA, 0x53 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xCB, 0x33 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xCC, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xCD, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xCE, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xCF, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD0, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD1, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD2, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD3, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD4, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD5, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD6, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD7, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD8, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xD9, 0x08 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xDA, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xDB, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xDC, 0x01 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xDD, 0x70 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xDE, 0x01 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xDF, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE0, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE1, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE2, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE3, 0x0a );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE4, 0x07 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE5, 0x07 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE6, 0xc9 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE7, 0x0f );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE8, 0xf4 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xE9, 0x5d );
	WriteRegisterIT6604(IT6604_PORT_x, 0xEA, 0xef );
	WriteRegisterIT6604(IT6604_PORT_x, 0xEB, 0x2b );
	WriteRegisterIT6604(IT6604_PORT_x, 0xEC, 0x04 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xED, 0xb4 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xEE, 0xd0 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xEF, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF0, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF1, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF2, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF3, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF4, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF5, 0x00 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF6, 0x67 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF7, 0x57 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF8, 0x0f );
	WriteRegisterIT6604(IT6604_PORT_x, 0xF9, 0x3c );
	WriteRegisterIT6604(IT6604_PORT_x, 0xFA, 0xe3 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xFB, 0xdd );
	WriteRegisterIT6604(IT6604_PORT_x, 0xFC, 0x86 );
	WriteRegisterIT6604(IT6604_PORT_x, 0xFD, 0x5b );
	WriteRegisterIT6604(IT6604_PORT_x, 0xFE, 0xcd );
	WriteRegisterIT6604(IT6604_PORT_x, 0xFF, 0x04 );
	#endif
}

BYTE IT6604Signal = 0;
void TaskIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	
	static u32 IT6604_systime = 0;
	int hde, vde;
	
	if(!isSysTickTimeout(IT6604_systime, 1100))
	{
		return;
	}
	IT6604_systime = GetSysTick();
	
	if(!HasSignalIT6604(IT6604_PORT_x))
	{
		DebugMessageIt6604("%s no signal!!\r\n",__func__);
		if(IT6604Signal != 1)
		{
		    //if((GetUserSetting()->previewSwitchValue) == 0)
            {      
			   // Fpga_setPreviewOutEnable(0);
            }
			IT6604Signal = 1;
		}
		return;
	}
	if(IT6604Signal != 2)
	{
	    //if((GetUserSetting()->previewSwitchValue) == 0)
        {   
		   // Fpga_setPreviewOutEnable(1);
        }
		IT6604Signal = 2;
	}	
	
	// 解决声音错误没有声音的问题
	if(ReadRegisterIT6604(IT6604_PORT_x,0x15) & 0x18)
	{
		WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x04);
		SysDelay1ms(10);
		WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x00);
	}

	#if 0
//	IT6604_InputFormat = getInputFormat(IT6604_PORT_x);//GetInputFormat();
	#else
	//printf("TaskIT6604()_mb103_getSwitch(0) = %d,",mb103_getSwitch(0));
	#if 0    //test 
	if((mb103_getSwitch(0) == 1)||(mb103_getSwitch(0) == 2)) //外扩板输入的信号,默认分辨率
	{
		IT6604_InputFormat = getInputSetting()->extInput[mb103_getSwitch(0) - 1].outputFormat;//MD_1920x1080_60;//MD_1680x1050_60;
		//printf("ext7/8_format = %d\r\n",IT6604_InputFormat);
	}
	else 
		#endif
	{
//		IT6604_InputFormat = getInputFormat(IT6604_PORT_x);//GetInputFormat();
		//printf("intput%d_format = %d\r\n",mb103_getSwitch(0),IT6604_InputFormat);
	}
	#endif
	
	if(IT6604_FormatLast == IT6604_InputFormat)
	{
		return;
	}
	DebugMessageIt6604("TTL: receive new format_%d-->%d\r\n",IT6604_InputFormat,GetSysTick());
	IT6604_FormatLast = IT6604_InputFormat;
	
	// 判断视频逻辑单元是否出错，出错要复位
	hde = GetHActiveIT6604(IT6604_PORT_x);
	vde = GetVActiveIT6604(IT6604_PORT_x);
	if((hde < 480) || (vde<200))
	{
		printf("TTL error ,reset-->%d\r\n",GetSysTick());
		SoftResetVideoLogicIT6604(IT6604_PORT_x);
	}

	#if 1 //统一采用444的色空间
//	rgb_SetInputColorSpace_TTL(0);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
	IT6604_YUV2RGB(IT6604_PORT_x);
	//IT6604_RGB2YUV(IT6604_PORT_x);
	#else
	if((IsInterlaceSignalIT6604(IT6604_PORT_x))||(isInput576P(IT6604_InputFormat))||(isInput1080P30Or25(IT6604_InputFormat)) )//I制和576P用YUV444的色空间
	//if((IsInterlaceSignalIT6604(IT6604_PORT_x)) )//I制用YUV444的色空间
	{
		{
			if(IT6604_InterlaceInput != 1)
			{
				printf("HDMI in use,and input interlace signal first time\r\n");
				IT6604_InterlaceInput = 1;		
			}
		}
		IT6604_RGB2YUV(IT6604_PORT_x);
		#if 1
		if(rgb_GetInputColorSpace_TTL() != 1)
		{
			setMB103_outputDisable(0);
//			rgb_SetInputColorSpace_TTL(1);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
			SysDelay1ms(200);
			setMB103_outputEnable(0);
		}
		#endif
	}
	else                                   //其他情况用RGB的色空间
	{
		//if((GetUserSetting()->inputSource[GetUserSetting()->currentWindow] == INPUT_DVI)
		//  ||((GetUserSetting()->inputSource[GetUserSetting()->currentWindow?0:1]) == INPUT_DVI)) //主通道或子通道正在输入HDMI
		{
			if(IT6604_InterlaceInput != 0)
			{
				printf("HDMI in use,and input progreaaive signal first time\r\n");
				IT6604_InterlaceInput = 0;		
				
				//rgb_SetInputColorSpace_TTL(0);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
				//rgb_updateColorSpace();
			}
		}
		IT6604_YUV2RGB(IT6604_PORT_x);
		#if 1
		if(rgb_GetInputColorSpace_TTL() != 0)
		{
			setMB103_outputDisable(0);
//			rgb_SetInputColorSpace_TTL(0);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
			SysDelay1ms(100);
			setMB103_outputEnable(0);
		}
		#endif
	}
	#endif
}

void SoftResetVideoLogicIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x02);
	SysDelay1ms(10);
	WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x00);
}

void SoftResetT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	//mb_resetIT6604();
	SysDelay1ms(100);//(300);
	WriteRegisterIT6604(IT6604_PORT_x,REG_RX_RST_CTRL,B_REGRST);
	//WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x02);
	//SysDelay1ms(10);
	//WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x00);
	IT6604_FormatLast = 0xff;
	IT6604_InterlaceInput = 0xff;
}

void SetIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	static u32 IT6604_systime = 0;
	int hde, vde;
		
	//if(!HasSignalIT6604(IT6604_PORT_x))
	//{
	//	//printf("IT6604: no signal-->%d\r\n",GetSysTick());
	//	return;
	//}
	// 解决声音错误没有声音的问题
	if(ReadRegisterIT6604(IT6604_PORT_x,0x15) & 0x18)
	{
		WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x04);
		SysDelay1ms(10);
		WriteRegisterIT6604(IT6604_PORT_x,0x05, 0x00);
	}

	#if 0
//	IT6604_InputFormat = getInputFormat(IT6604_PORT_x);//GetInputFormat();
	#else
	#if 0
	//printf("TaskIT6604()_mb103_getSwitch(0) = %d,",mb103_getSwitch(0));
	if((mb103_getSwitch(0) == 1)||(mb103_getSwitch(0) == 2)) //外扩板输入的信号,默认分辨率
	{
		IT6604_InputFormat = getInputSetting()->extInput[mb103_getSwitch(0) - 1].outputFormat;//MD_1920x1080_60;//MD_1680x1050_60;
		//printf("ext7/8_format = %d\r\n",IT6604_InputFormat);
	}
	else 
		#endif
	{
//		IT6604_InputFormat = getInputFormat(IT6604_PORT_x);//GetInputFormat();
		//printf("intput%d_format = %d\r\n",mb103_getSwitch(0),IT6604_InputFormat);
	}
	#endif
	
	if(IT6604_FormatLast == IT6604_InputFormat)
	{
		return;
	}
	DebugMessageIt6604("SetTTL: receive new format_%d-->%d\r\n",IT6604_InputFormat,GetSysTick());
	IT6604_FormatLast = IT6604_InputFormat;
	
	// 判断视频逻辑单元是否出错，出错要复位
	hde = GetHActiveIT6604(IT6604_PORT_x);
	vde = GetVActiveIT6604(IT6604_PORT_x);
	if((hde < 480) || (vde<200))
	{
		printf("SetTTL error ,reset-->%d\r\n",GetSysTick());
		SoftResetVideoLogicIT6604(IT6604_PORT_x);
	}
	
	#if 0 //统一采用444的色空间
//	rgb_SetInputColorSpace_TTL(0);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
	IT6604_YUV2RGB(IT6604_PORT_x);
	//IT6604_RGB2YUV(IT6604_PORT_x);
	#else
	if((IsInterlaceSignalIT6604(IT6604_PORT_x))||(isInput576P(IT6604_InputFormat))||(isInput1080P30Or25(IT6604_InputFormat)) )//I制和576P用YUV444的色空间
	//if((IsInterlaceSignalIT6604(IT6604_PORT_x)) )//I制用YUV444的色空间
	{
		{
			if(IT6604_InterlaceInput != 1)
			{
				printf("SetTTL:HDMI in use,and input interlace signal first time\r\n");
				IT6604_InterlaceInput = 1;		
			}
		}
		IT6604_RGB2YUV(IT6604_PORT_x);
		#if 1
		//if(rgb_GetInputColorSpace_TTL() != 1)
		{
		//	setMB103_outputDisable(0);
//			rgb_SetInputColorSpace_TTL(1);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
		//	SysDelay1ms(100);
		//	setMB103_outputEnable(0);
		}
		#endif
	}
	else                                   //其他情况用RGB的色空间
	{
		//if((GetUserSetting()->inputSource[GetUserSetting()->currentWindow] == INPUT_DVI)
		//  ||((GetUserSetting()->inputSource[GetUserSetting()->currentWindow?0:1]) == INPUT_DVI)) //主通道或子通道正在输入HDMI
		{
			if(IT6604_InterlaceInput != 0)
			{
				printf("SetTTL:HDMI in use,and input progreaaive signal first time\r\n");
				IT6604_InterlaceInput = 0;				
			}
		}
		IT6604_YUV2RGB(IT6604_PORT_x);
		#if 1
		//if(rgb_GetInputColorSpace_TTL() != 0)
		{
		//	setMB103_outputDisable(0);
//			rgb_SetInputColorSpace_TTL(0);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
		//	SysDelay1ms(100);
		//	setMB103_outputEnable(0);
		}
		
		//rgb_SetInputColorSpace_TTL(0);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
		//rgb_updateColorSpace();
		#endif
	}
	#endif
}

void IT6604SetOutPolarity(IT6604_PortTypeDef IT6604_PORT_x, int value)
{
	if(value)
	{
		WriteRegisterIT6604((IT6604_PortTypeDef) IT6604_PORT_x, 0x1A, 0x14+0x03 );
		WriteRegisterIT6604((IT6604_PortTypeDef) IT6604_PORT_x, 0x1B, 0x40 );
	}
	else
	{
		WriteRegisterIT6604((IT6604_PortTypeDef) IT6604_PORT_x, 0x1A, 0x14);
		WriteRegisterIT6604((IT6604_PortTypeDef) IT6604_PORT_x, 0x1B, 0x00 );
	}	
}

bool HasSignalIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	if(ReadRegisterIT6604(IT6604_PORT_x, 0x58 ) & 0x08)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int GethStartIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	int result;
	int Htotal,Hactive;

	Htotal = GetHTotalIT6604(IT6604_PORT_x);
	Hactive = GetHActiveIT6604(IT6604_PORT_x);
	result = Htotal - Hactive - GetHsyncFrontPorch(IT6604_PORT_x);
	return result; // HsyncBackPorch
}
int GetvStartIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	int result;

	//result = GetVsyncBackPorch(IT6604_PORT_x);

	result = GetvSyncWidthIT6604(IT6604_PORT_x) + GetVsyncBackPorch(IT6604_PORT_x);
	return result; // VsynBackPorch
}

int GetHsyncFrontPorch(IT6604_PortTypeDef IT6604_PORT_x)
{
	int result;
	
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x5E);
	result += ((ReadRegisterIT6604(IT6604_PORT_x, 0x5d ) & 0xf0)<<4);
	return result;
}

int GetVsyncFrontPorch(IT6604_PortTypeDef IT6604_PORT_x)
{
	int result;
	
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x63);
	return result;
}
int GetVsyncBackPorch(IT6604_PortTypeDef IT6604_PORT_x)
{
	int result;
	
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x62);
	return result;
}

int GethSyncWidthIT6604(IT6604_PortTypeDef IT6604_PORT_x) // OK
{
	int result;
	
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x5c );
	result += ((ReadRegisterIT6604(IT6604_PORT_x, 0x5d ) & 0x0f)<<8);
	return result;
}

int GetvSyncWidthIT6604(IT6604_PortTypeDef IT6604_PORT_x) //
{
	int result;
	int VsyncFrontPorch,VsynBackPorch;
	int Vtotal,Vactive;
	
	VsyncFrontPorch = GetVsyncFrontPorch(IT6604_PORT_x);
	VsynBackPorch = GetVsyncBackPorch(IT6604_PORT_x);
	Vtotal = GetVTotalIT6604(IT6604_PORT_x);
	Vactive = GetVActiveIT6604(IT6604_PORT_x);
	result = Vtotal - Vactive - VsyncFrontPorch - VsynBackPorch;

//	printf("VsyncFrontPorch=%d\r\n",VsyncFrontPorch);
//	printf("VsynBackPorch=%d\r\n",VsynBackPorch);
//	printf("Vtotal=%d\r\n",Vtotal);
//	printf("Vactive=%d\r\n",Vactive);

	return result;
}

int GethvSyncPolarityIT6604(IT6604_PortTypeDef IT6604_PORT_x)// OK
{
	int result;
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x65); 
	result &= 0x03;

//	printf("IT6604 %d input polarity = %d --- before\r\n", IT6604_PORT_x, result);
	
	result = ~result; //与程序相反: IT6604: bit0 -- horizontal sync  bit1 -- vertical sync,  0 -- 负, 1 -- 正
	result &= 0x03;

//	printf("IT6604 %d input polarity = %d --- after\r\n", IT6604_PORT_x, result);

	return result;
}

int GetOutputhvSyncPolarityIT6604(IT6604_PortTypeDef IT6604_PORT_x)// OK
{
	int result;
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x1A); 
	result &= 0x03;

	//printf("IT6604 %d output polarity = %d\r\n", IT6604_PORT_x, result);

	return result;
}


int GetHTotalIT6604(IT6604_PortTypeDef IT6604_PORT_x) // OK
{
	int result;
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x59 );
	result += ((ReadRegisterIT6604(IT6604_PORT_x, 0x5a ) & 0x0f)<<8);
	return result;
}
int GetVTotalIT6604(IT6604_PortTypeDef IT6604_PORT_x) // OK
{
	int result;
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x5f );
	result += ((ReadRegisterIT6604(IT6604_PORT_x, 0x60 ) & 0x0f)<<8);
	return result;
}
int GetHActiveIT6604(IT6604_PortTypeDef IT6604_PORT_x) // OK
{
	int result;
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x5b );
	result += ((ReadRegisterIT6604(IT6604_PORT_x, 0x5a ) & 0xf0)<<4);
	return result;
}
int GetVActiveIT6604(IT6604_PortTypeDef IT6604_PORT_x) // OK
{
	int result;
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x61 );
	result += ((ReadRegisterIT6604(IT6604_PORT_x, 0x60 ) & 0xf0)<<4);
	return result;
}
int GetVFreqIT6604(IT6604_PortTypeDef IT6604_PORT_x) // OK
{
	int result;
	unsigned int clk;
	
	result = ReadRegisterIT6604(IT6604_PORT_x, 0x64 );
	clk = (unsigned int) 128*IT6604_CRYSTAL_FREQ/result;
	result = clk/(GetHTotalIT6604(IT6604_PORT_x)*GetVTotalIT6604(IT6604_PORT_x));
	return result;
}

// 20121002  get input color space by port
// return: 0 -- RGB, 1 -- YUV 4:4:4, 2 -- YUV 4:2:2, 3 -- BT656
int GetColorSpaceIT6604(IT6604_PortTypeDef IT6604_PORT_x) // OK
{
	int temp;

	temp = ReadRegisterIT6604(IT6604_PORT_x, 0xAE);
	temp = temp >> 5;
	temp = temp & 0x03; // 00: RGB, 01:YUV 4:2:2, 10: YUV 4:4:4, 11:BT656

	if(ReadRegisterIT6604(IT6604_PORT_x, 0x10) & 0x10) // HDMI mode
	{


		if(temp == 1) {return 2;}   // YUV 4:2:2
		else if(temp == 2) {return 1;}  // YUV 4:4:4
		return temp;
	}
	else   // DVI mode
	{
		return 0;
	}
}
	
VideoFormat_TypeDef getInputFormat(IT6604_PortTypeDef IT6604_PORT_x)
{
	static VideoFormat_TypeDef result = MD_NOINPUT;
	if(HasSignalIT6604(IT6604_PORT_x))
	{
		int hde, vde, htotal, vtotal, vfreq;
		hde = GetHActiveIT6604(IT6604_PORT_x);
		vde = GetVActiveIT6604(IT6604_PORT_x);
		htotal = GetHTotalIT6604(IT6604_PORT_x);
		vtotal = GetVTotalIT6604(IT6604_PORT_x); 	
		vfreq = GetVFreqIT6604(IT6604_PORT_x);

		//printf("H act: %d,V act: %d,H Total: %d,V Total: %d,V Freg: %d\r\n",hde,vde,htotal, vtotal, vfreq);
		//result = (VideoFormat_TypeDef) getVideoFormatByTotal(hde,vde,htotal, vtotal, vfreq);
		result = (VideoFormat_TypeDef) getVideoFormatByActive(hde,vde,htotal, vtotal, vfreq);
		return result;
	}
	else
	{
		return MD_NOINPUT;
	}
}

bool IsInterlaceSignalIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	if(ReadRegisterIT6604(IT6604_PORT_x,0x58 ) & 0x02)
	{
		//printf("IsInterlaceSignalIT6604(%d) = 1\r\n",IT6604_PORT_x);
		return TRUE;
	}
	else
	{
		//printf("IsInterlaceSignalIT6604(%d) = 0\r\n",IT6604_PORT_x);
		return FALSE;
	}
}

bool HasHDMIInIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	if(ReadRegisterIT6604(IT6604_PORT_x,0x10 ) & 0x10)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


void IT6604_RGB2YUV(IT6604_PortTypeDef IT6604_PORT_x)
{
	BYTE  Input_Colour[1];
	BYTE  buffer[1];

	//printf("IT6604_YUV2RGB\r\n");
	buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x10);
	if(buffer[0] & 0x10) //HDMI
	{
		DebugMessageIt6604("%s HDMI input!!\r\n",__func__);
		Input_Colour[0] = ReadRegisterIT6604(IT6604_PORT_x,REG_RX_AVI_DB1);
		Input_Colour[0] >>= 5;
		Input_Colour[0] &= 0x03;
		//printf("COLOR SPACE: %d\r\n",Input_Colour[0]);
	    if((Input_Colour[0] == YUV422_COLOUR) || (Input_Colour[0] == YUV444_COLOUR))
	    {
			//printf("HDMI_no csc,color: %d\r\n",Input_Colour[0]);
	    	buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x20);
			buffer[0] &= 0xFC;
			WriteRegisterIT6604Buffer(IT6604_PORT_x,0x20,1, buffer);
	    }
	    else
		{
			buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x20);
			buffer[0] |= 0x02;
			buffer[0] &= 0xFE;
			WriteRegisterIT6604Buffer(IT6604_PORT_x,0x20,1, buffer);
			#if 0
			if(isInputHD())  //ITU709
			#else
			if(isInputHD(IT6604_InputFormat))
			#endif
	        {
				//printf("HDMI_HD input\r\n");
				#if 1
	            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_YOFF, 3, bCSCOffset_16_235);
	            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU709_16_235);
				#else
	            //IT6604_WriteRegs(REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
	            //IT6604_WriteRegs(REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU709_0_255);
	            #endif
	        }
	        else //if(isInputSD()) //ITU601
	        {
				//printf("HDMI_SD input\r\n");
				#if 1
	            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_YOFF, 3, bCSCOffset_16_235);
	            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU601_16_235);
				#else
	            IT6604_WriteRegs(REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
	            IT6604_WriteRegs(REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU601_0_255);
				#endif
	        }
	    }
    }
    else  //DVI
    {
		DebugMessageIt6604("%s DVI input!!\r\n",__func__);
    	buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x20);
		buffer[0] |= 0x02;
		buffer[0] &= 0xFE;
		WriteRegisterIT6604Buffer(IT6604_PORT_x,0x20,1, buffer);
		#if 0
		if(isInputHD())  //ITU709
		#else
		if(isInputHD(IT6604_InputFormat))
		#endif
	    {
	        WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
	        WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU709_0_255);
	    }
	    else //if(isInputSD())  //ITU601
	    {
	        WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
	        WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU601_0_255);
	    }
    }
}



void IT6604_YUV2RGB(IT6604_PortTypeDef IT6604_PORT_x)
{
	BYTE  Input_Colour[1];
	BYTE  buffer[1];

	//printf("IT6604_YUV2RGB\r\n");
	buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x10);
	if(buffer[0] & 0x10) //HDMI
	{
		DebugMessageIt6604("%s HDMI input!!\r\n",__func__);
		Input_Colour[0] = ReadRegisterIT6604(IT6604_PORT_x,REG_RX_AVI_DB1);
		Input_Colour[0] >>= 5;
		Input_Colour[0] &= 0x03;
		//printf("COLOR SPACE: %d\r\n",Input_Colour[0]);
	    if(Input_Colour[0] == RGB_COLOUR)
	    {
	    	#if 1
	    	buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x20);
			buffer[0] &= 0xFC;
			WriteRegisterIT6604Buffer(IT6604_PORT_x,0x20,1, buffer);
			#else
			if(IsInterlaceSignalIT6604())//(!isInputInterlace())
			{
				buffer[0] = IT6604_ReadReg(0x20);
				buffer[0] &= 0xFC;
				IT6604_WriteRegs(0x20,1, buffer);
			}
			else
			{
				printf("Interlace input\r\n");
				buffer[0] = IT6604_ReadReg(0x20);
				buffer[0] &= 0xFC;
				buffer[0] |= 0x02;
				IT6604_WriteRegs(0x20,1, buffer);
				if(isInputHD())  //ITU709
		        {
					printf("HD input\r\n");
		            IT6604_WriteRegs(REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
		            IT6604_WriteRegs(REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU709_0_255);
		        }
				else// if(isInputSD()) //ITU601
		        {
					printf("SD input\r\n");
		            IT6604_WriteRegs(REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
		            IT6604_WriteRegs(REG_RX_CSC_MTX11_L, 18, bCSCMtx_RGB2YUV_ITU601_0_255);
		        }				
			}
			#endif			
	    }
	    else
		{
			#if 0
			if(isInputP60Hz())
			{
				buffer[0] = IT6604_ReadReg(0x20);
				buffer[0] |= 0x03;
				//buffer[0] &= 0xFF;
				IT6604_WriteRegs(0x20,1, buffer);
				if(isInputHD())  //ITU709
		        {
					HDMIRX_DEBUG_PRINTF("HD input\r\n");
		            IT6604_WriteRegs(REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
		            IT6604_WriteRegs(REG_RX_CSC_MTX11_L, 18, bCSCMtx_YUV2RGB_ITU709_0_255);
		        }
				else// if(isInputSD()) //ITU601
		        {
					HDMIRX_DEBUG_PRINTF("SD input\r\n");
		            IT6604_WriteRegs(REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
		            IT6604_WriteRegs(REG_RX_CSC_MTX11_L, 18, bCSCMtx_YUV2RGB_ITU601_0_255);
		        }
			}
			else
			{
		    	buffer[0] = IT6604_ReadReg(0x20);
				buffer[0] &= 0xFC;
				IT6604_WriteRegs(0x20,1, buffer);
			}
			#else
			//if(IsInterlaceSignalIT6604(IT6604_PORT_x))//(isInputInterlace())
			//{
			//	buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x20);
			//	buffer[0] &= 0xFC;
			//	WriteRegisterIT6604Buffer(IT6604_PORT_x,0x20,1, buffer);
			//}
			//else
			{
				buffer[0] = ReadRegisterIT6604(IT6604_PORT_x,0x20);
				buffer[0] |= 0x03;
				WriteRegisterIT6604Buffer(IT6604_PORT_x,0x20,1, buffer);
				#if 0
				if(isInputHD())  //ITU709
				#else
				if(isInputHD(IT6604_InputFormat))
				#endif
		        {
					//printf("HD input\r\n");
		            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
		            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_MTX11_L, 18, bCSCMtx_YUV2RGB_ITU709_0_255);
		        }
				else// if(isInputSD()) //ITU601
		        {
					//printf("SD input\r\n");
		            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_YOFF, 3, bCSCOffset_0_255);
		            WriteRegisterIT6604Buffer(IT6604_PORT_x,REG_RX_CSC_MTX11_L, 18, bCSCMtx_YUV2RGB_ITU601_0_255);
		        }
			}
			#endif	        
	    }
    }
    else  //DVI
    {
		DebugMessageIt6604("%s DVI input!!\r\n",__func__);
    	buffer[0] = 0x00;//IT6604_ReadReg(0x20);
		WriteRegisterIT6604Buffer(IT6604_PORT_x,0x20,1, buffer);
    }
}

char CheckIT6604(IT6604_PortTypeDef IT6604_PORT_x)
{
	char res,i,checkValue[5] = {0xB2,0x04,0x64,0x02,0xE9},readValue;
	
	for(i = 0; i < 5; i++)
	{
		WriteRegisterIT6604(IT6604_PORT_x,REG_RX_CSC_MTX11_L,checkValue[i]);
		readValue = ReadRegisterIT6604(IT6604_PORT_x,REG_RX_CSC_MTX11_L);
		if(readValue != checkValue[i])
		{
			res = FALSE;
			return res;
		}
	}
	res = TRUE;
	return res;
}

void resetIT6604_Manual(void) 
{
	SysDelay1ms(200);//(300);
	DebugMessageIt6604("%s-->%d",__func__,GetSysTick());
	SoftResetT6604((IT6604_PortTypeDef)0);
	SysDelay1ms(100);//(300);
	SetIT6604((IT6604_PortTypeDef)0);
}



