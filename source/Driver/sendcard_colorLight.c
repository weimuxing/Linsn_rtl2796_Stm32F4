
#include "uart.h"
#include "pipe.h"
#include "rs232command.h"
#include "systick.h"
//#include "vsp320.h"
#include "userpref.h"
#include "control.h"
#include "videoFormatTable.h"
#include "sendcard_colorLight.h"
#include <stdio.h>
#include "uart.h"
#include <math.h>
#include <string.h>


//typedef unsigned int WORD ;
typedef uint32_t DWORD;


#define LOWORD(x)           ((WORD)((DWORD)(x) & 0xffff))
#define HIWORD(x)           ((WORD)((DWORD)(x) >> 16))



#define Send_card_Time   3

extern int UART_PORT;


extern u8 cardTypeCheckOK;
extern u8 isCheckCardtype;
extern BYTE  UART1Buffer_Receive[];
extern unsigned int ReceiveCount;
extern u8 GetSendCardUsbPlusState(void);


u8 command_kalaite[262]=
{
	0xaa,0x44,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xee

};


static BYTE SendcardCheckSum(BYTE *buffer, int size)
{
	u32 sum = 0;
	int i;
	for(i=0; i<size; i++)
	{
		sum += buffer[i];
	}
	return (BYTE)sum;
}

static u32 SendcardCheckSum_u32(BYTE *buffer, int size)
{
	u32 sum = 0;
	int i;
	for(i=0; i<size; i++)
	{
		sum += buffer[i];
	}
	return sum;
}

bool readsendcard(void)
{
	u32 startTime;
	ReceiveCount = 0;
	UARTSendBuffer(UART_PORT,(char *)command_kalaite,262);
	startTime = GetSysTick();
	while(ReceiveCount!=256)
	{
		if(isSysTickTimeout(startTime,3000))
		{
			printf("can't receive cmd from receive card01 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card01 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card01 = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card01 = %x\r\n", UART1Buffer_Receive[3]);
			//ReceiveCount = 0;
			return FALSE;
		}
	}
	SysDelay1ms(100);
	
	getsendcard()->sendcardinfo = (UART1Buffer_Receive[2]<<8)+UART1Buffer_Receive[3];
	
    getsendcard()->brightness = (UART1Buffer_Receive[38])/16;
    //getsendcard()->gamma = UART1Buffer_Receive[102];
	getsendcard()->posx[0] = UART1Buffer_Receive[8]+(UART1Buffer_Receive[9]<<8);
	getsendcard()->sizex[0] = UART1Buffer_Receive[10]+(UART1Buffer_Receive[11]<<8);
    getsendcard()->posy[0] = UART1Buffer_Receive[4]+(UART1Buffer_Receive[5]<<8);
	getsendcard()->sizey[0] = UART1Buffer_Receive[6]+(UART1Buffer_Receive[7]<<8);

	getsendcard()->posx[1] = UART1Buffer_Receive[16]+(UART1Buffer_Receive[17]<<8);
	getsendcard()->sizex[1] = UART1Buffer_Receive[18]+(UART1Buffer_Receive[19]<<8);
    getsendcard()->posy[1] = UART1Buffer_Receive[12]+(UART1Buffer_Receive[13]<<8);
	getsendcard()->sizey[1] = UART1Buffer_Receive[14]+(UART1Buffer_Receive[15]<<8);
	savesendcard();
	//ReceiveCount = 0;
	return TRUE;
	
}


bool erase_flash(u32 adress)
{
	BYTE buffer[262];
	int i=0;
//	u32 startTime;
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x23;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;
	for(i=0;i<256;i++)
	{
		buffer[i+5] = 0;
	}
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);
	/*startTime = GetSysTick();
	while(UART1Buffer_Receive[1]!=3)
	{
		if(isSysTickTimeout(startTime,10000))
		{
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[3]);
			return FALSE;
		}
	}*/
	SysDelay1ms(1000);        
	return TRUE;
	
}


bool Sendcard_Brightness_Command_temporary(BYTE value)
{
	BYTE buffer[14];
//	int i=0;
	//u32 startTime;
	ReceiveCount = 0;
	buffer[0] = 0x80;
	buffer[1] = 0x11;
	buffer[2] = 0x22;
	buffer[3] = 0x33;
	buffer[4] = 0x44;
	buffer[5] = value;
	buffer[6] = 0xff;
	buffer[7] = 0xff;
	buffer[8] = 0xff;
	buffer[9] = 0xff;
	buffer[10] = 0x19;
	buffer[11] = 0x64;
	buffer[12] = 0xff;
	buffer[13] = 0x00;
	
	UARTSendBuffer(UART_PORT,(char *)buffer,14);
	SysDelay1ms(20);  
	return TRUE;
}

bool Sendcard_Brightness_Command(BYTE value)
{
	BYTE buffer[262];
	int i=0;
//	u32 startTime;
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x85;
	buffer[2] = 0xf3;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	for(i=0;i<256;i++)
	{
		buffer[i+5] = 0;
	}
    buffer[5] = value;
	buffer[6] = 0xff;
	buffer[7] = 0xff;
	buffer[8] = 0xff;
	buffer[9] = 0xff;
	
	/*for(i=0;i<0x10;i++)
	{
		buffer[i+5] = value;
	}*/
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);

	/*startTime = GetSysTick();
	while(UART1Buffer_Receive[1]!=3)
	{
		if(isSysTickTimeout(startTime,3000))
		{
		    printf("can't receive cmd from receive card11 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card11 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card11 = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card11 = %x\r\n", UART1Buffer_Receive[3]);
			return FALSE;
		}
	}*/
	SysDelay1ms(200);  
	return TRUE;
}



bool Sendcard_position_Command(u32 adress, u16 x,u16 y, u16 xsize, u16 ysize)
{
		BYTE buffer[262];
		int i=0;
		u32 startTime;
		ReceiveCount = 0;
		buffer[0] = 0xaa;
		buffer[1] = 0x85;
		buffer[2] = (adress&0x0ffffff)>>16;
		buffer[3] = (adress&0x0ffff)>>8;
		buffer[4] = adress&0x0ff;
		for(i=0;i<256;i++)
		{
			buffer[i+5] = 0;
		}
		buffer[5] = 0x06;
		buffer[6] = 0x33;
		buffer[7] = 0x00;
		buffer[8] = 0x00;
		buffer[9] = 0x00;//包间隙0小，32中，64大
	
	#if 0	//@cc20141227
		buffer[0x10] = y&0xff;
		buffer[0x11] = y>>8;
		buffer[0x12] = ysize&0xff;
		buffer[0x13] = ysize>>8;
		buffer[0x14] = x&0xff;
		buffer[0x15] = x>>8;
		buffer[0x16] = xsize&0xff;
		buffer[0x17] = xsize>>8;
	#else
		buffer[0x15] = y&0xff;
		buffer[0x16] = y>>8;
		buffer[0x17] = ysize&0xff;
		buffer[0x18] = ysize>>8;
		buffer[0x19] = x&0xff;
		buffer[0x1a] = x>>8;
		buffer[0x1b] = xsize&0xff;
		buffer[0x1c] = xsize>>8;
	#endif
		
		buffer[261] = SendcardCheckSum(buffer,261);
		UARTSendBuffer(UART_PORT,(char *)buffer,262);
	
		startTime = GetSysTick();
		while(UART1Buffer_Receive[1]!=3)
		{
			if(isSysTickTimeout(startTime,3000))
			{
				printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[0]);
				printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[1]);
				printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[2]);
				printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[3]);
				return FALSE;
			}
		}
		SysDelay1ms(100);
		return TRUE;
}


bool Sendcard_position_Command1(u32 adress, u16 x,u16 y, u16 xsize, u16 ysize)
{
	BYTE buffer[262];
	int i=0;
	u32 startTime;
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x85;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;
	for(i=0;i<256;i++)
	{
		buffer[i+5] = 0;
	}
	buffer[0x5] = y&0xff;
	buffer[0x6] = y>>8;
	buffer[0x7] = ysize&0xff;
	buffer[0x8] = ysize>>8;
    buffer[0x9] = x&0xff;
	buffer[0xa] = x>>8;
	buffer[0xb] = xsize&0xff;
	buffer[0xc] = xsize>>8;
	
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);

	startTime = GetSysTick();
	while(UART1Buffer_Receive[1]!=3)
	{
		if(isSysTickTimeout(startTime,3000))
		{
		    printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card1 = %x\r\n", UART1Buffer_Receive[3]);
			return FALSE;
		}
	}
	SysDelay1ms(100);
	return TRUE;
}

bool Sendcard_jizai_Command(u32 adress)
{
	BYTE buffer[262];
	int i=0;
	u32 startTime;
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x44;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;
	for(i=0;i<256;i++)
	{
		buffer[i+5] = 0;
	}
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);
	startTime = GetSysTick();
	while(UART1Buffer_Receive[1]!=3)
	{
		if(isSysTickTimeout(startTime,3000))
		{
			printf("can't receive cmd from receive card2 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card2 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card2 = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card2 = %x\r\n", UART1Buffer_Receive[3]);
			return FALSE;
		}
	}
	SysDelay1ms(20);  
	return TRUE;
}

//19 00 00 ff ff 85 00 00 00 0d 00 00 00 02 xx 02 
bool Sendcard_gamma_Command(u8 value)
{
	BYTE buffer[136];
	int i=0;
	u32 startTime;
//	BYTE value1 = 0;
	ReceiveCount = 0;
	buffer[0] = 0xcc;
	buffer[1] = 0xf0;
	buffer[2] = 0x00;
	buffer[3] = 0x88;
	
	buffer[4] = 0x19;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0xff;
	buffer[8] = 0xff;
	buffer[9] = 0x85;
	buffer[10] = 0x00;

	buffer[11] = 0x00;
	buffer[12] = 0x00;
	buffer[13] = 0x0d;
	buffer[14] = 0x00;
	buffer[15] = 0x00;
	buffer[16] = 0x00;
    buffer[17] = 0x02;
	buffer[18] = value;
	buffer[19] = 0x02;
	
	for(i=0;i<116;i++)
	{
		buffer[i+20] = 0;
	}
	UARTSendBuffer(UART_PORT,(char *)buffer,136);
	startTime = GetSysTick();
	while(ReceiveCount!=6)
	{
		if(isSysTickTimeout(startTime,3000))
		{
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[3]);
			return FALSE;
		}
	}
	SysDelay1ms(20);  
	return TRUE;
}

bool Sendcard_EraseFlash_Gamma(void)
{
	BYTE buffer[274];
	u32 checksum =0;
	int i=0;
	ReceiveCount = 0;
	buffer[0] = 0xcc;
	buffer[1] = 0xf0;
	buffer[2] = 0x01;
	buffer[3] = 0x12;
	
	buffer[4] = 0x26;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0xff;
	buffer[8] = 0xff;
	buffer[9] = 0x23;
	buffer[10] = 0x00;
	buffer[11] = 0x09;
	buffer[12] = 0x00;
	buffer[13] = 0x00;
	for(i=0;i<256;i++)
	{
		buffer[i+14] = 0;
	}
	checksum = SendcardCheckSum_u32(buffer,270);
	buffer[270] = (checksum >> 24);
	buffer[271] = (checksum& 0x0ffffff) >> 16;
	buffer[272] = (checksum& 0x0ffff) >> 8;
	buffer[273] = (checksum& 0x0ff) ;
	
	UARTSendBuffer(UART_PORT,(char *)buffer,274);
	
	SysDelay1ms(3200);        
	return TRUE;
}

//gamma表的计算（gamma 是Gamma值）for Colorlight Sender
BYTE gamaTableBuf[1152];
BYTE gammaTableHead[16]={0x28,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

bool Sendcard_gamma_Command_New(u32 address)
{
	BYTE buffer[274];
	int i=0;
	u32 startTime;
	u32 checksum =0;
	ReceiveCount = 0;
	buffer[0] = 0xcc;
	buffer[1] = 0xf0;
	buffer[2] = 0x01;
	buffer[3] = 0x12;
	
	buffer[4] = 0x26;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0xff;
	buffer[8] = 0xff;
	
	buffer[9] = 0x85;
	buffer[10] = 0x00;
	buffer[11] = (address&0x0ffffff)>>16;
	buffer[12] = (address&0x0ffff)>>8;
	buffer[13] = (address&0x0ff);

	if(buffer[12] ==0x00)
	{
		for(i =0;i<16;i++)
		{
			buffer[i+14] = gammaTableHead[i];
		}
		for(i=0;i<240;i++)	//256-16
		{
			buffer[i+30] = gamaTableBuf[i];
		}
	}
	else if(buffer[12] ==0x01)
	{
		for(i =0;i<256;i++)
		{
			buffer[i+14] = gamaTableBuf[240+i];
		}
	}
	else if(buffer[12] ==0x02)
	{
		for(i =0;i<256;i++)
		{
			buffer[i+14] = gamaTableBuf[240+256+i];
		}
	}
	else if(buffer[12] ==0x03)
	{
		for(i =0;i<256;i++)
		{
			buffer[i+14] = gamaTableBuf[240+256+256+i];
		}
	}
	else if(buffer[12] ==0x04)
	{
		for(i =0;i<144;i++)	//1152-(254+256*3)
		{
			buffer[i+14] = gamaTableBuf[240+256+256+256+i];
		}
		for(i=0;i<112;i++)
		{
			buffer[i+14+144] =0;	
		}
	}
	
	//buffer[270] = SendcardCheckSum(buffer,270);
	checksum = SendcardCheckSum_u32(buffer,270);
	buffer[270] = (checksum >> 24);
	buffer[271] = (checksum& 0x0ffffff) >> 16;
	buffer[272] = (checksum& 0x0ffff) >> 8;
	buffer[273] = (checksum& 0x0ff) ;
	UARTSendBuffer(UART_PORT,(char *)buffer,274);
	startTime = GetSysTick();
	while(ReceiveCount!=6)
	{
		if(isSysTickTimeout(startTime,1000))
		{
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card2g = %x\r\n", UART1Buffer_Receive[3]);
			return FALSE;
		}
	}
	SysDelay1ms(200);  
	return TRUE;
}

//圆整
DWORD Round(double value)	
{
	if(value>=0)
		return (DWORD)(value+0.5);
	else
		return (DWORD)(value-0.5);
}

 double GammaFunEx(double x, double gamma, double k, double base)
{
	double x0;
	double a0;
	double gamma_x0;
	double ratio;
	
	if(k>0.5)	k = 0.5;
	if(k<0)		k = 0.0;

	if(x<0.00001)
		return 0.0;

	if(base>0.00001)
	{
		double fvalue = 0.0;
		if(gamma<1.1)
			fvalue = pow(x, gamma);
		else
		{
			 x0		= pow(k/gamma, 1.0/(gamma-1));
			 a0		= x0*k;
			 gamma_x0	= pow(x0, gamma);
			 ratio		= (1.0-a0)/(1-gamma_x0);

			if(x<x0)
			{
				fvalue = x*k;
			}
			else
			{
				fvalue = a0 + ratio*(pow(x, gamma) - gamma_x0);
			}
		}

		return fvalue * (1-base) + base;
	}
	else
	{
		if(gamma<1.1)
			return pow(x, gamma) * (1-base) + base;
		else
		{
			 x0		= pow(k/gamma, 1.0/(gamma-1));
			 a0		= x0*k;
			 gamma_x0	= pow(x0, gamma);
			 ratio		= (1.0-a0)/(1-gamma_x0);

			if(x<x0)
			{
				return x*k;
			}
			else
			{
				return a0 + ratio*(pow(x, gamma) - gamma_x0);
			}
		}
	}
}


void sendcard_Creat_GammaTable(double gamma)
{
	int i=0;
	DWORD dwValue	= 0;
	WORD lwValue	= 0;
	WORD hwValue	= 0;
	double	k		= 0.0;
	double	base	= 0.0;
	double 	fValue	= 0;
	double	maxValue = 65536.0*256-1.0;


	//gamma表
	//ZeroMemory(gamaTableBuf,sizeof(gamaTableBuf));
	for(i=0;i<1152;i++)
	{
		gamaTableBuf[i] =0x0;
	}
	k=0.0625;
	base =0.0;
	for(i=0; i<256; i++)
	{
		fValue	= GammaFunEx(i/255.0, gamma, k, base) * maxValue;
		dwValue	= Round(fValue);

		if (dwValue>0xfffffe)	
			dwValue = 0xfffffe;

		hwValue = HIWORD(dwValue);
		lwValue = LOWORD(dwValue);

		gamaTableBuf[i*3]		= LOBYTE(hwValue);
		gamaTableBuf[i*3+1]	= HIBYTE(lwValue);
		gamaTableBuf[i*3+2]	= LOBYTE(lwValue);
	}

	memcpy(gamaTableBuf+768, gamaTableBuf, 192);
	memcpy(gamaTableBuf+768+192, gamaTableBuf, 192);

}

bool sendcard_Set_Gamma(u8 gammaval)
{
	double gamma;

	gammaTableHead[0]= gammaval;
	gamma = (gammaval/ 16)*1.0+(gammaval%16)*0.1;
	sendcard_Creat_GammaTable(gamma);
	Sendcard_gamma_Command_New(0x090000);
	Sendcard_gamma_Command_New(0x090100);
	Sendcard_gamma_Command_New(0x090200);
	Sendcard_gamma_Command_New(0x090300);
	Sendcard_gamma_Command_New(0x090400);
	
	return TRUE;
}


bool Sendcard_Command_jiazai(void)
{
		BYTE buffer[136];
		int i=0;
		u32 startTime;
		ReceiveCount = 0;
		buffer[0] = 0xcc;
		//buffer[1] = 0xf0;
	
	#if 1	//@cc20141227
		if(getsendcard()->lxynetwork ==0)
		{
			buffer[1] = 0xf0;	
		}
		else
		{
			buffer[1] = 0xf1;
		}
	#else
		buffer[2] = 0x00;
	#endif
		buffer[3] = 0x88;
		
		buffer[4] = 0x06;
		buffer[5] = 0x00;
		buffer[6] = 0x00;
		buffer[7] = 0xff;
		buffer[8] = 0xff;
		buffer[9] = 0x77;
		buffer[10] = 0x00;
	
		buffer[11] = 0x00;
		buffer[12] = 0x00;
		buffer[13] = 0x00;
		buffer[14] = 0x00;
		buffer[15] = 0x00;
		buffer[16] = 0x00;
		buffer[17] = 0x00;
		buffer[18] = 0x00;
		buffer[19] = 0x00;
		
		for(i=0;i<116;i++)
		{
			buffer[i+20] = 0;
		}
		UARTSendBuffer(UART_PORT,(char *)buffer,136);
		startTime = GetSysTick();
		while(ReceiveCount!=6)
		{
			if(isSysTickTimeout(startTime,3000))
			{
				printf("can't receive cmd from receive card2j = %x\r\n", UART1Buffer_Receive[0]);
				printf("can't receive cmd from receive card2j = %x\r\n", UART1Buffer_Receive[1]);
				printf("can't receive cmd from receive card2j = %x\r\n", UART1Buffer_Receive[2]);
				printf("can't receive cmd from receive card2j = %x\r\n", UART1Buffer_Receive[3]);
				return FALSE;
			}
		}
		SysDelay1ms(20);  
		return TRUE;
}


//cc 00 00 88 19 00 00 00 00 85 00 00 00 00 00 00 00 0a 00 20 00 20 00 98 00 98
bool Sendcard_area_Command(u16 cardnumber,u16 x, u16 y, u16 endx, u16 endy)
{
		BYTE buffer[136];
		int i=0;
		u32 startTime;
	//	BYTE value1 = 0;
	
		//printf("Sendcard_area_Command cardnumber = %d\n",cardnumber);
		//printf("Sendcard_area_Command x = %d\n",x);
		//printf("Sendcard_area_Command y = %d\n",y);
		//printf("Sendcard_area_Command endx = %d\n",endx);
		//printf("Sendcard_area_Command endy = %d\n",endy);
		ReceiveCount = 0;
		buffer[0] = 0xcc;
	#if 1	//@cc20141227
		if(getsendcard()->lxynetwork ==0)
		{
			buffer[1] = 0x00;	
		}
		else
		{
			buffer[1] = 0x01;
		}
	#else
		buffer[1] = 0x00;
	#endif
		buffer[2] = 0x00;
		buffer[3] = 0x88;
		
		buffer[4] = 0x19;
		buffer[5] = 0x00;
		buffer[6] = 0x00;
		buffer[7] = cardnumber>>8;
		buffer[8] = cardnumber&0x0ff;
		buffer[9] = 0x85;
		buffer[10] = 0x00;
	
		buffer[11] = 0x00;
		buffer[12] = 0x00;
		#if 0 //@lv 20151201
		buffer[13] = 0x00; 
		#else
		buffer[13] = 0x02;
		#endif
		buffer[14] = 0x00;
		buffer[15] = 0x00;
		buffer[16] = 0x00;
		buffer[17] = 0x0a;
	#if 0 //@lv 20150925
    #if 1
		buffer[18] = 0x00;
		buffer[19] = 0x00;
		
		buffer[20] = x>>8;
		buffer[21] = x&0x0ff;
	
		buffer[22] = y>>8;
		buffer[23] = y&0x0ff;
	
		buffer[24] = endx>>8;
		buffer[25] = endx&0x0ff;
	
		buffer[26] = endy>>8;
		buffer[27] = endy&0x0ff;
		
		for(i=0;i<108;i++)
		{
			buffer[i+28] = 0;
		}
    #else
		buffer[18] = x>>8;
		buffer[19] = x&0x0ff;
	
		buffer[20] = y>>8;
		buffer[21] = y&0x0ff;
	
		buffer[22] = endx>>8;
		buffer[23] = endx&0x0ff;
	
		buffer[24] = endy>>8;
		buffer[25] = endy&0x0ff;
		
		for(i=0;i<110;i++)
		{
			buffer[i+26] = 0;
		}
	#endif
	#else
	buffer[18] = x>>8;
	buffer[19] = x&0x0ff;

	buffer[20] = y>>8;
	buffer[21] = y&0x0ff;

	buffer[22] = endx>>8;
	buffer[23] = endx&0x0ff;

	buffer[24] = endy>>8;
	buffer[25] = endy&0x0ff;
	
	for(i=0;i<106;i++)
	{
		buffer[i+26] = 0;
	}
	#endif
		UARTSendBuffer(UART_PORT,(char *)buffer,136);
		startTime = GetSysTick();
		while(ReceiveCount!=6)
		{
			if(isSysTickTimeout(startTime,3000))
			{
				printf("can't receive cmd from receive card2a = %x\r\n", UART1Buffer_Receive[0]);
				printf("can't receive cmd from receive card2a = %x\r\n", UART1Buffer_Receive[1]);
				printf("can't receive cmd from receive card2a = %x\r\n", UART1Buffer_Receive[2]);
				printf("can't receive cmd from receive card2a = %x\r\n", UART1Buffer_Receive[3]);
				return FALSE;
			}
		}
		SysDelay1ms(20);  
		return TRUE;
}


extern BYTE m_edidRaw[];
bool SendCard_Colorlight_Write_Edid(int width,int height)
{
	u32 startTime;
	BYTE buffer[262];
	int i=0;
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x85;
	buffer[2] = 7;
	buffer[3] = 0;
	buffer[4] = 0;

	buffer[5] = 1;
	buffer[6] = width>>8;
	buffer[7] = width&0xff;
	buffer[8] = height>>8;
	buffer[9] = height&0xff;
	
	for(i=0;i<123;i++)
	{
		buffer[i+10] = 0;
	}

	for(i =0;i<128;i++)
	{
		buffer[i+133] = m_edidRaw[i];
	}
	UART1Buffer_Receive[1] =0;
	buffer[261] = SendcardCheckSum(buffer,261);
	
	UARTSendBuffer(UART_PORT,(char *)buffer,262);
	startTime = GetSysTick();
	
	while(UART1Buffer_Receive[1]!=0x03)
	{
		if(isSysTickTimeout(startTime,2000))
		{
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[2]);
			printf("can't receive cmd from receive card10 = %x\r\n", UART1Buffer_Receive[3]);
			return FALSE;
		}
	}
	SysDelay1ms(1000);        
	return TRUE;
}


bool Clt_SendCard_Erase_Flash(u32 adress)
{
	BYTE buffer[262];
	int i=0;
    u32 startTime;
    
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x23;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;
	for(i = 0;i < 256;i++)
	{
		buffer[i+5] = 0;
	}
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);
	startTime = GetSysTick();
	while(UART1Buffer_Receive[1] != 0x03)
	{
		if(isSysTickTimeout(startTime,10000))
		{
			printf("<Erase Send Card> send to sendcard(%x,%x,%x,%x)Fail\r\n", 
                    UART1Buffer_Receive[0],
        			UART1Buffer_Receive[1],
        			UART1Buffer_Receive[2],
        			UART1Buffer_Receive[3]);
			return FALSE;
		}
	}
	SysDelay1ms(30); 
    
	return TRUE;	
}

bool Clt_Sendcard_Port1_Area_Write(u32 adress, u16 x,u16 y, u16 xsize, u16 ysize)
{
	BYTE buffer[262];
	int i=0;
	u32 startTime;
    
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x85;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;
	for(i = 0;i < 256;i++)
	{
		buffer[i+5] = 0;
	}
	buffer[5] = 0x06;
	buffer[6] = 0x33;
	buffer[7] = 0x00;
	buffer[8] = 0x00;
	buffer[9] = 0x00;//包间隙0小，32中，64大
	buffer[10] = 0x00;      // 大包标志,Bitr 0:0,不是大包，1是大包
	
	buffer[0x15] = y&0xff;
	buffer[0x16] = y>>8;
	buffer[0x17] = ysize&0xff;
	buffer[0x18] = ysize>>8;
	buffer[0x19] = x&0xff;
	buffer[0x1a] = x>>8;
	buffer[0x1b] = xsize&0xff;
	buffer[0x1c] = xsize>>8;

	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);

	startTime = GetSysTick();
	while(UART1Buffer_Receive[1] != 0x03)
	{
		if(isSysTickTimeout(startTime,2000))
		{
			printf("<Port1_Area> send to sendcard(%x,%x,%x,%x)Fail\r\n", 
                    UART1Buffer_Receive[0],
        			UART1Buffer_Receive[1],
        			UART1Buffer_Receive[2],
        			UART1Buffer_Receive[3]);
			return FALSE;
		}
	}    
	SysDelay1ms(100);
	return TRUE;
}

bool Clt_Sendcard_Port2_Area_Write(u32 adress, u16 x,u16 y, u16 xsize, u16 ysize)
{
	BYTE buffer[262];
	int i=0;
	u32 startTime;
    
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x85;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;
	for(i = 0;i < 256;i++)
	{
		buffer[i+5] = 0;
	}
	buffer[0x5] = y&0xff;
	buffer[0x6] = y>>8;
	buffer[0x7] = ysize&0xff;
	buffer[0x8] = ysize>>8;
    buffer[0x9] = x&0xff;
	buffer[0xa] = x>>8;
	buffer[0xb] = xsize&0xff;
	buffer[0xc] = xsize>>8;
	
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);

	startTime = GetSysTick();
	while(UART1Buffer_Receive[1] != 0x03)
	{
		if(isSysTickTimeout(startTime,2000))
		{
			printf("<Port2_Area> send to sendcard(%x,%x,%x,%x)Fail\r\n", 
                    UART1Buffer_Receive[0],
        			UART1Buffer_Receive[1],
        			UART1Buffer_Receive[2],
        			UART1Buffer_Receive[3]);
			return FALSE;
		}
	}    

	SysDelay1ms(100);
	return TRUE;
}


bool Clt_Sendcard_jiazai_Command(u32 adress)
{
	BYTE buffer[262];
	int i=0;
	u32 startTime;
    
	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x44;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;
	for(i=0;i<256;i++)
	{
		buffer[i+5] = 0;
	}
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);
	startTime = GetSysTick();
	while(UART1Buffer_Receive[1] != 0x03)
	{
		if(isSysTickTimeout(startTime,2000))
		{
			printf("<sendCard JiaZai> send to sendcard(%x,%x,%x,%x)Fail\r\n", 
                    UART1Buffer_Receive[0],
        			UART1Buffer_Receive[1],
        			UART1Buffer_Receive[2],
        			UART1Buffer_Receive[3]);
			return FALSE;
		}
	}  

	SysDelay1ms(20);  
	return TRUE;
}

bool Clt_Sendcard_Brightness_Temporary(BYTE value)
{
	BYTE buffer[14];
    //int i=0;
	//u32 startTime;
	ReceiveCount = 0;
	buffer[0] = 0x80;
	buffer[1] = 0x11;
	buffer[2] = 0x22;
	buffer[3] = 0x33;
	buffer[4] = 0x44;
	buffer[5] = value;
 #if 0  
	buffer[6] = 0xff;
	buffer[7] = 0xff;
	buffer[8] = 0xff;
	buffer[9] = 0xff;
	buffer[10] = 0x19;
	buffer[11] = 0x64;
	buffer[12] = 0xff;
	buffer[13] = 0x00;
#else
    buffer[6] = 0x00;
    buffer[7] = 0x00;
    buffer[8] = 0x00;
    buffer[9] = 0x00;
    buffer[10] = 0x00;
    buffer[11] = 0x00;
    buffer[12] = 0x00;
    buffer[13] = 0x00;
#endif
	
	UARTSendBuffer(UART_PORT,(char *)buffer,14);
	SysDelay1ms(20);  
	return TRUE;
}

bool Clt_Sendcard_OnOffPanel_Temporary(BYTE value)
{
	BYTE buffer[8];
    //int i=0;
	//u32 startTime;
	ReceiveCount = 0;
	buffer[0] = 0x81;
	buffer[1] = 0x11;
	buffer[2] = 0x22;
	buffer[3] = 0x33;
	buffer[4] = 0x44;
	buffer[5] = value;
    buffer[6] = 0x00;
    buffer[7] = 0x00;
	
	UARTSendBuffer(UART_PORT,(char *)buffer,8);
	SysDelay1ms(20); 
    
	return TRUE;
}


bool Clt_Sendcard_Brightness_Save(u32 adress,BYTE value)
{
	BYTE buffer[262];
	int i=0;
    u32 startTime;

	ReceiveCount = 0;
	buffer[0] = 0xaa;
	buffer[1] = 0x85;
	buffer[2] = (adress&0x0ffffff)>>16;
	buffer[3] = (adress&0x0ffff)>>8;
	buffer[4] = adress&0x0ff;

	for(i = 0;i< 16;i++)
	{
		buffer[i+5] = value;
	} 
	for(i=16;i<256;i++)
	{
		buffer[i+5] = 0;
	}    
	
	buffer[261] = SendcardCheckSum(buffer,261);
	UARTSendBuffer(UART_PORT,(char *)buffer,262);
   
	startTime = GetSysTick();
	while(UART1Buffer_Receive[1] != 0x03)
	{
		if(isSysTickTimeout(startTime,2000))
		{
			printf("<Bright Save> send to sendcard(%x,%x,%x,%x)Fail\r\n", 
                    UART1Buffer_Receive[0],
        			UART1Buffer_Receive[1],
        			UART1Buffer_Receive[2],
        			UART1Buffer_Receive[3]);
			return FALSE;
		}
	} 

	SysDelay1ms(200);  
	return TRUE;
}




