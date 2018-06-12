//
//		GS2971A
//     Output H, V, F or CEA 861 Timing Signals
//
//
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "uart.h"

#include "SysConfig.h"

#include "control.h"
#include "SiI9134.h"
#include "videoFormatTable.h"

#include "GS2971A.h"

#ifdef DEBUG_PRINT_ENABLE
//#define GS2971_DBG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"/n", __LINE__, ##__VA_ARGS__)
#define GS2971_DBG(format,...) printf("[gs2971]"format,  ##__VA_ARGS__)

#else
#define GS2971_DBG(format,...)	do{}while(0)
#endif

u8 GS2971A_SPI_MISO_READ(GS2971A_Port_TypeDef GS2971A_X)
{
	if(GS2971A_X == GS2971A_A)
	{
	  return GPIO_ReadInputDataBit(GS2971A_MISO_PORT_A,GS2971A_MISO_PIN_A);
	}

	return Bit_SET;
}

void GS2971A_SPI_MOSI_LOW(GS2971A_Port_TypeDef GS2971A_X)
{
	if(GS2971A_X == GS2971A_A)
	{
      	GPIO_ResetBits(GS2971A_MOSI_PORT_A,GS2971A_MOSI_PIN_A);
	}
}

void GS2971A_SPI_MOSI_HIGH(GS2971A_Port_TypeDef GS2971A_X)
{
	if(GS2971A_X == GS2971A_A)
	{
      	GPIO_SetBits(GS2971A_MOSI_PORT_A,GS2971A_MOSI_PIN_A);
	}
}

void GS2971A_SPI_CLK_LOW(GS2971A_Port_TypeDef GS2971A_X)
{
	if(GS2971A_X == GS2971A_A)
	{
      	GPIO_ResetBits(GS2971A_CLK_PORT_A,GS2971A_CLK_PIN_A);
	}
}

void GS2971A_SPI_CLK_HIGH(GS2971A_Port_TypeDef GS2971A_X)
{
	if(GS2971A_X == GS2971A_A)
	{
      	GPIO_SetBits(GS2971A_CLK_PORT_A,GS2971A_CLK_PIN_A);
	}
}

void GS2971A_SPI_CS_LOW(GS2971A_Port_TypeDef GS2971A_X)
{
	if(GS2971A_X == GS2971A_A)
	{
     	 GPIO_ResetBits(GS2971A_CS_PORT_A,GS2971A_CS_PIN_A);
	}
}

void GS2971A_SPI_CS_HIGH(GS2971A_Port_TypeDef GS2971A_X)
{
	if(GS2971A_X == GS2971A_A)
	{
     	 	GPIO_SetBits(GS2971A_CS_PORT_A,GS2971A_CS_PIN_A);
	}
}

void GS2971A_SPI_Delay(BYTE uiDelay)
{
	BYTE i;
 	for(i = 0;i<uiDelay;i++){}
}

static BYTE GS2971A_SPI_Read(GS2971A_Port_TypeDef GS2971A_X,WORD W_Address, WORD *
	rpBuffer, WORD W_Len)
{
	BYTE i;
	WORD W_Data;
	unsigned  short jj;

	if((rpBuffer == 0)||(W_Len == 0)||(GS2971A_X > GS2971A_MAX))
	{
		return FALSE;
	}

	//GS2971_Init(GS2971_X);

	W_Address |= GS2971A_READ_FLAG;
	if(W_Len > 1)
	{
		W_Address |= GS2971A_AUTOINC_FLAG;
	}

	//SCLK must be low when CS changes from high to low
	GS2971A_SPI_CS_LOW(GS2971A_X);//SPI_CS = 0;
	GS2971A_SPI_CLK_LOW(GS2971A_X);//SPI_CLK = 0;

	//T0:min 1.5ns since CS low to the first SCLK rising edge
	//GS2971A_SPI_Delay(10);
	jj=20;
	while(jj--);

	//i = 16;
	//while(i--)
	for(i = 0; i < 16; i++)
	{
		//Send out the 16bit command word Start from MSB
		if(W_Address & 0x8000)
		{
			GS2971A_SPI_MOSI_HIGH(GS2971A_X);//SPI_DATA_OUT = 1;
		}
		else
		{
			GS2971A_SPI_MOSI_LOW(GS2971A_X);//SPI_DATA_OUT = 0;
		}

		jj=10;
		while(jj--);
		//_nop_();  
		GS2971A_SPI_CLK_HIGH(GS2971A_X);//SPI_CLK = 1;
		
		jj=10;
		while(jj--);
		//_nop_();
		
		W_Address <<= 1;

		GS2971A_SPI_CLK_LOW(GS2971A_X);//SPI_CLK = 0;
	}

	//T5-delay between the last command bit and the first data bit min 150ns
	//SPIWait(10);
	jj=20;
	while(jj--);

	while(W_Len--)
	{
		W_Data = 0;
		
		//i = 16;
		//while(i--)
		for(i = 0; i < 16; i++)
		{
			W_Data <<= 1;
			//_nop_();
			jj=10;
			while(jj--);

			GS2971A_SPI_CLK_HIGH(GS2971A_X);  //SPI_CLK = 1; 
			
			if(GS2971A_SPI_MISO_READ(GS2971A_X))//if(SPI_DATA_IN)
			{
				W_Data |= 0x0001;
			}

			jj=10;
			while(jj--);
			//_nop_();

			GS2971A_SPI_CLK_LOW(GS2971A_X);//SPI_CLK = 0;
		}

		*rpBuffer = W_Data;
		rpBuffer++;
	}

	//T7:the delay that CS goes high after the last SCLK
	//SPIWait(10);
	jj=20;
	while(jj--);
	GS2971A_SPI_CS_HIGH(GS2971A_X);//SPI_CS = 1;

	//GS2971A_Port_DeInit(GS2971A_X);
	
	return TRUE;
}

void GS2971A_Reset(GS2971A_Port_TypeDef GS2971A_X)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef *GPIOx;	

	GS2971_DBG("%s portx=%d\r\n",__func__,GS2971A_X);	

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	if(GS2971A_X == GS2971A_A)
	{
		/* Enable GPIO clock */
		RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GS2971A_RESET_PIN_A;
		GPIOx = GS2971A_RESET_PORT_A;
	}

	GPIO_Init(GPIOx, &GPIO_InitStruct);		
	GPIO_ResetBits(GPIOx,GPIO_InitStruct.GPIO_Pin);
	SysDelay1ms(100);
	GPIO_SetBits(GPIOx,GPIO_InitStruct.GPIO_Pin);
	SysDelay1ms(2);		
}


void GS2971A_Port_Init(GS2971A_Port_TypeDef GS2971A_X)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if(GS2971A_X == GS2971A_A)
	{
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

		// MISO
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_InitStruct.GPIO_Pin = GS2971A_MISO_PIN_A;
		GPIO_Init(GS2971A_MISO_PORT_A, &GPIO_InitStruct);

		// MOSI
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Pin = GS2971A_MOSI_PIN_A;
		GPIO_Init(GS2971A_MOSI_PORT_A, &GPIO_InitStruct);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = GS2971A_CLK_PIN_A;
		GPIO_Init(GS2971A_CLK_PORT_A, &GPIO_InitStruct);
		GPIO_ResetBits(GS2971A_CLK_PORT_A,GS2971A_CLK_PIN_A);
		// CS
		GPIO_InitStruct.GPIO_Pin = GS2971A_CS_PIN_A;
		GPIO_Init(GS2971A_CS_PORT_A, &GPIO_InitStruct);
		GPIO_SetBits(GS2971A_CS_PORT_A,GS2971A_CS_PIN_A);
	
	}
}

void GS2971A_Port_DeInit(GS2971A_Port_TypeDef GS2971A_X)
{		
}

void GS2971A_ReadReg(GS2971A_Port_TypeDef GS2971A_X,WORD W_Address, WORD *rpBuffer, WORD W_Len)
{
	GS2971A_SPI_Read(GS2971A_X,W_Address,rpBuffer,W_Len);
}

int GS2971A_ReadInputFormat(GS2971A_Port_TypeDef GS2971A_X)
{
	WORD format;
	WORD lock;
	//WORD readRegValue[8];	

#if 0
	//WORD HTotal,VTotal,HAtive,VActive,interlaceFlag;
	WORD readRegValue[8];

	GS2971A_Port_Init(GS2971A_X);
	GS2971A_ReadReg(GS2971A_X,0x1F, readRegValue, 8);	
	//GS2971_DeInit(GS2971_X);
	
	#if 0	
	HAtive = readRegValue[0];
	HTotal = readRegValue[1];
	VTotal = readRegValue[2];
	VActive = readRegValue[3]&0x07FF;
	interlaceFlag = (readRegValue[3]>>11)&0x0001;
	lock = readRegValue[3];
	format = readRegValue[6];
	printf("GS2971_%d:HAtive = %d,HTotal = %d,VActive = %d,VTotal = %d,
		interlaceFlag = %d\r\n",GS2971_X,readRegValue[0],readRegValue[1],VActive,
		readRegValue[2],interlaceFlag);
	printf("0x22 = %x,0x23 = %x,0x24 = %x,0x25 = %x,0x26 = %x\r\n",readRegValue[3],
		readRegValue[4],readRegValue[5],readRegValue[6],readRegValue[7]);
	#endif
	
	//GS2971_Init(GS2971_X);
	//GS2971_ReadReg(GS2971_X,0x22, &lock, 1);
	//GS2971_ReadReg(GS2971_X,0x25, &format, 1);
	//GS2971_DeInit(GS2971_X);
	lock = readRegValue[3];
	format = readRegValue[6];
	//printf("GS2971_%d:lock = %x,format = %d\r\n",GS2971_X,lock,format);

	// RASTER_STRUC_4
	// bit 12:Video standard lock.
	if((lock & (0x01<<12)) == 0)
	{
		return MD_NOINPUT;
	}
#else		// by hibernate
	GS2971A_Port_Init(GS2971A_X);

	//GS2971A_ReadReg(GS2971A_X,0x1F, readRegValue, 8);
	//lock = readRegValue[3];
	//format = readRegValue[6];


	GS2971A_ReadReg(GS2971A_X,0x25, &format, 1);
#endif
	
	#if 0
	format = GetFormatByTotal(HAtive,VActive,HTotal,VTotal,interlaceFlag);
	return format;
	#else
	if(format & BIT6)
	{
		return MD_NOINPUT;
	}
	
	switch(format & 0x3f)
	{
		case 6:
		case 7:
			return MD_720x480_60i;
		case 21:
		case 22:
			return MD_720x576_50i;
		case 4:
			return MD_1280x720_60;
		case 19:
			return MD_1280x720_50;
		case 5:
			return MD_1920x1080_60i;
		case 20:
			return MD_1920x1080_50i;
		case 34:
			return MD_1920x1080_30;
		case 33:
			return MD_1920x1080_25;
		case 32:
			return MD_1920x1080_24;
		case 16:
			return MD_1920x1080_60;
		case 31:
			return MD_1920x1080_50;
		default:
			return MD_UNDEFINE;
	}
	#endif
}

static WORD SDIFormatHistory = MD_NOINPUT;
char getSDIFormat(void)//port 0-2
{
	return SDIFormatHistory;
}
void clearSDIFormat(void)
{
	SDIFormatHistory = MD_NOINPUT;
}

void taskInputSDI(void)
{
	static u32 taskInputSDI_sysTime = 0;
	static char checkPort0And1Enable = 0;
	WORD format_SDI;
	if(!isSysTickTimeout(taskInputSDI_sysTime,1100))
	{
		return;
	}
	taskInputSDI_sysTime = GetSysTick();

	//test
	//GS2971_ReadReg(GS2971_A,0x22, &formatA, 1);
	//GS2971_ReadReg(GS2971_A,0x25, &formatB, 1);
	//printf("GS2971_A:formatA = %x,formatB = %x\r\n",formatA,formatB);

	if(checkPort0And1Enable == 0)
	{
		if(isSysTickTimeout(0,14000))
		{
			checkPort0And1Enable = 1;
		}
	}
	if(checkPort0And1Enable == 1)
	{
		format_SDI = GS2971A_ReadInputFormat(GS2971A_A);
		if(format_SDI != SDIFormatHistory)
		{
			SDIFormatHistory = format_SDI;
			GS2971_DBG("format_SDI = %x\r\n",format_SDI);
//			SiI9134_Set_SdiOutputFormat(SiI9134_PORT_3,format_SDI);
		}
	}
}


