#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "uart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "SysConfig.h"
#include "menuData.h"
#include "userPref.h"
#include "ledcontrol.h"
#include "I2C.h"
#include "FPGA.h"

#include "uart.h"
#include "UartDebug.h"


#if ENABLE_TEST_GRID_CONTROL
BYTE bGridFirst;
BYTE GridSetIndex = 0;
BYTE testGrid[4];
BYTE gridLineMode[4];
BYTE gridLineColor[4];	
int gridHSpace[4];	
int gridVSpace[4];	
BYTE gridBackgroud[4];
BYTE gridDotMode[4];	
BYTE gridDotSpeed[4];
BYTE gridDotPosX[4];
BYTE gridDotPosY[4];
BYTE gridDotColor[4];
BYTE gridDotColorR[4];	
BYTE gridDotColorG[4];	
BYTE gridDotColorB[4];

BYTE gridDotChar[4];
int gridDotCharPos_X[4];
int gridDotCharPos_Y[4];



int gridHPos[4];
int gridVPos[4];
int gridHWidth[4];
int gridVHeigh[4];



// 下标对应的是菜单中的值
BYTE testGripTbl[] = {FPGA_GRID_OFF,FPGA_GRID_ON};
BYTE gripBgTbl[] = {FPGA_GRID_BACKGROUD_NULL,FPGA_GRID_BACKGROUD_VIDEO};
BYTE gripLineModeTbl[] = {FPGA_GRID_LINE_HV,FPGA_GRID_LINE_H,FPGA_GRID_LINE_V,FPGA_GRID_LINE_BORDERS};
BYTE gripLineColorTbl[] = {FPGA_GRID_LINE_COLOR_RED,FPGA_GRID_LINE_COLOR_GREEN,FPGA_GRID_LINE_COLOR_YELLOW};
BYTE gripDotModeTbl[] = {FPGA_TEST_DOT_MOVE,FPGA_TEST_DOT_STILL,FPGA_TEST_DOT_HIDE}; 

#endif

fpga_dev_info fpga_dev;



void FPGA_Program_Start(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// reset pin
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;		
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_Pin = FPGA_RESET_PIN;
	GPIO_Init(FPGA_RESET_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = FPGA_NCE_PIN;
	GPIO_Init(FPGA_NCE_PORT, &GPIO_InitStruct);
	
	RESET_FPGA_RST();
	RESET_FPGA_NCE();
}




void FPGA_Program_Done(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// reset pin
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;		
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	
	GPIO_InitStruct.GPIO_Pin = FPGA_RESET_PIN;
	GPIO_Init(FPGA_RESET_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = FPGA_NCE_PIN;
	GPIO_Init(FPGA_NCE_PORT, &GPIO_InitStruct);
	
	//SET_FPGA_CONFIG();
	//SET_FPGA_NCE();
}

void FPGA_Reset(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_InitStruct.GPIO_Pin = FPGA_RESET_PIN; // PD11         FPGA RESET#
    GPIO_Init(FPGA_RESET_PORT, &GPIO_InitStruct);
    GPIO_SetBits(FPGA_RESET_PORT, FPGA_RESET_PIN);
    GPIO_ResetBits(FPGA_RESET_PORT, FPGA_RESET_PIN);
    GPIO_SetBits(FPGA_RESET_PORT, FPGA_RESET_PIN);	
}


void FPGA_set(u8 address,  u8 value)
{
	i2cWriteByte(FPGA_I2C_PORT, FPGA_ADDR, address, value);
}


u8 FPGA_get(u8 address)
{
	return i2cReadByte(FPGA_I2C_PORT, FPGA_ADDR, address);
}

void FPGA_init(void)
{
#if ENABLE_TEST_GRID_CONTROL
	BYTE i;
	bGridFirst = 0;

	GridSetIndex = 0;
	for(i = 0;i < 4;i++)
	{
		testGrid[i] = 0;
		gridLineMode[i] = 0;
		gridLineColor[i] = 0; 
		gridHSpace[i] = 40;	
		gridVSpace[i] = 40;	
		gridBackgroud[i] = 1;
		gridDotMode[i] = 0;	
		gridDotSpeed[i] = 30;
		gridDotPosX[i] = 1;
		gridDotPosY[i] = 1;
		gridDotColorR[i] = 255; 
		gridDotColorG[i] = 255; 
		gridDotColorB[i] = 255; 

		gridHPos[i] = 800;
		gridVPos[i] = 600;
		gridHWidth[i] = 400;
		gridVHeigh[i] = 400;		
	}
#endif
    FPGA_set(0x10,0x00);
	FPGA_set(0x11,0x04);
    FPGA_set(0x31,0x04);
    FPGA_set(0x51,0x04);
    FPGA_set(0x71,0x04);

	FPGA_ReadSoftVersion();	
}


void FPGA_I2C_Test()
{
	BYTE vaule;
	i2c_Init(FPGA_I2C_PORT);
    i2cWriteByte(FPGA_I2C_PORT, FPGA_ADDR, FPGA_REG_DVI_CTL, 0x30);	// TTL_timming_a  even-b odd-a
    DebugMessageFpga("reg 0x10 write 0x30\r\n");
	vaule = i2cReadByte(FPGA_I2C_PORT, FPGA_ADDR, FPGA_REG_DVI_CTL);
	DebugMessageFpga("reg 0x10 is 0x%x\r\n",vaule);
}

#if ENABLE_TEST_GRID_CONTROL
void FPGA_Set_TestGrid_index(BYTE index)
{
	if(index < 4)
	{
		GridSetIndex = index;
	}
}

BYTE FPGA_Get_TestGrid_index(void)
{
	return GridSetIndex;
}
void FPGA_TestGrid_Ctrl(int index)
{

}

void FPGA_TestGrid_RefleshByPort(void)
{


}

void init_testGridData(void)
{
  
}

void FPGA_TestGrid_test(void)
{   

}

#endif

int FPGA_ReadSoftVersion(void)
{
	u8 ver_msb;
	u8 ver_lsb;
	ver_msb  = FPGA_get(FPGA_REG_0X00);	
	ver_lsb = FPGA_get(FPGA_REG_0X01);	
	
	fpga_dev.ver_msb = ver_msb;
	fpga_dev.ver_lsb = ver_lsb;
	
	return (ver_msb);
}


int FPGA_GetSoftVersion(u8 * msb,u8 * lsb)
{
	if(msb != NULL)
	{
		(*msb) = fpga_dev.ver_msb;
	}

	if(lsb != NULL)
	{
		(*lsb) = fpga_dev.ver_lsb;	
	}
	return (FPGA_get(FPGA_REG_VersionNum));
}


u8 Fpga_setPreviewOutEnable(u8 enable)
{
#if 1
	u8 value = FPGA_get(FPGA_REG_DVI_CTL);
    value &= 0xCF;      // Bit4-5
	if(enable == 2)
	{
		value |= 0x10;
	}    
	else if(enable == 1)
	{
		value |= 0x20;
	}
	else
	{
		value |= 0x30;
	}
	FPGA_set(FPGA_REG_DVI_CTL,value & 0xFE);
#endif
}






void FPGASetFadeTime(u8 value)
{
    FPGA_set(FPGA_REG_FADE_TIME,value);
}


void FPGASetFreeze(u8 value)
{
	//FPGA_REG_Freeze
    FPGA_set(FPGA_REG_Freeze,value);
}



void FPGASetOutputBlack(char enable)
{
    u8 value = FPGA_get(FPGA_REG_BLACK);
	value &= 0xfe;    //Bit7-6
	if(enable == TRUE) 
	{
		value |= 0x01; //black
	}
	else            
	{
		value &= 0xfe; //live
	}
	
	FPGA_set(FPGA_REG_BLACK,value);
}

BYTE FPGAGetOutputBlack(void)
{
	u8 value,i;
	value = FPGA_get(FPGA_REG_BLACK);
	if((value & 0x01) == 0x01) 
	{
		return TRUE;
	}

    return FALSE;
}

void FPGA_setOutputBlack(char enable)
{
    u8 value = FPGA_get(FPGA_REG_DVI_CTL);
	value &= 0x3F;    //Bit7-6
	if(enable == TRUE) 
	{
		value |= 0x80; //black
	}
	else            
	{
		value |= 0x40; //live
	}
	FPGA_set(FPGA_REG_DVI_CTL,value);
}

BYTE FPGA_getOutputBlack(void)
{
    u8 value = FPGA_get(FPGA_REG_DVI_CTL);
	if((value & 0x80) == 0x80) 
	{
		return TRUE;
	}

    return FALSE;
}

#define FPGA_DEBUG

#ifdef FPGA_DEBUG

extern uartRxSt uartxRxStatus;

const char *const debugCmd[] = 
{
	"fpga r : "
	"fpga w : "
};

u8 AscToInt8(u8 *buf)
{

	u8 l4b = 0,m4b = 0;
	if(*buf - '0' <= 9)
		l4b = *buf - '0';
	else
		l4b = (*buf - 'a') + 10;
	
	if(*(buf+1) - '0' <= 9)
		m4b = *(buf+1) - '0';
	else
		m4b = (*(buf+1) - 'a') + 10;
	return (l4b << 4) + m4b;
	//d = (d << 4) + (*(buf + 1) - '0');
}


u16 AscToInt16(u8 *buf)
{

	u16 d = 0,d1;
	d =	((*buf - '0') <= 9)?((*buf - '0')):((*buf - 'a') + 10);
	d1 = d;
	d = ((*(buf+1) - '0') <= 9)?((*(buf+1) - '0')):((*(buf+1) - 'a') + 10);
	d1 = (d1 << 4) | d;
	d = ((*(buf+2) - '0') <= 9)?((*(buf+2) - '0')):((*(buf+2) - 'a') + 10);
	d1 = (d1 << 4) | d;
	d = ((*(buf+3) - '0') <= 9)?((*(buf+3) - '0')):((*(buf+3) - 'a') + 10);
	d1 = (d1 << 4) | d;
	return d1;
}


char FpgaDebug(u8 *buf)
{
	u8 cmpBuf[64];
	u16 addr,data;
	//char *offset;
	char r = 'r';
	
	if(GetUartDataTranFinishFlag() == true)
	{
		SetUartDataTranFinishFlag(false);
		
		strcpy((char*)cmpBuf,(const char*)buf);
		UARTSendBuffer(2,buf,GetUartDataTranFinishLen());
		SysDelay1ms(100);
		UARTSendBuffer(1,uartxRxStatus.uart2RxBuf,GetUart2DataTranFinishLen());
		memset(uartxRxStatus.uart2RxBuf,0,sizeof(uartxRxStatus.uart2RxBuf));
		ClrUart2DataTranFinishLen();
		//offset = strstr((char*)cmpBuf,"R");
		
		if(strstr((char*)cmpBuf,"r") != NULL)
		{
			
			addr = AscToInt8(buf+(GetUartDataTranFinishLen()-2));
			data = FPGA_get(addr);
			DebugMessageFpga(" fpga read addr %x,resoult = %x\r\n",addr,data);
		}
		else if(strstr((char*)cmpBuf,"w") != NULL)
		{
			addr = AscToInt8(buf+(GetUartDataTranFinishLen()-7));
			data = AscToInt16(buf+(GetUartDataTranFinishLen()-4));
			FPGA_set(addr,(u8)(data & 0xff));
			FPGA_set(addr+1,(u8)(data >> 8));
			//data = FPGA_get(addr);
			DebugMessageFpga(" fpga write addr %x,data = %x\r\n",addr,data);
			
			data = FPGA_get(addr + 1);
			data = (data << 8) + FPGA_get(addr);
			DebugMessageFpga(" fpga read addr %x,resoult = %x\r\n",addr,data);
		}
		ClrUartDataTranFinishLen();
	}
	
}




void FpgaDebugTask(void)
{
	FpgaDebug(uartxRxStatus.uart1RxBuf);
}



void FpgaOutputPortSel(u8 port0,u8 port1,u8 port2,u8 port3)
{
	u8 outputSel = 0;
	outputSel = port3;
	outputSel = (outputSel << 2) | port2;
	outputSel = (outputSel << 2) | port1;
	outputSel = (outputSel << 2) | port0;
	
	FPGA_set(FPGA_REG_OutputPortSel,outputSel);

	DebugMessageFpga("FpgaOutputPortSel = %x\r\n",FPGA_get(FPGA_REG_OutputPortSel));
}


#endif




