
#include "uart.h"
#include "pipe.h"
#include "rs232command.h"
#include "systick.h"
//#include "vsp320.h"
#include "userpref.h"
#include "control.h"
#include "videoFormatTable.h"
#include "sendcard_nowa.h"
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


u8 cardTypeCheckOK =0;
u8 isCheckCardtype =0;
u8 cmdid = 0;
extern BYTE  UART1Buffer_Receive[];
extern unsigned int ReceiveCount;
extern u8 GetSendCardUsbPlusState(void);

void init_sendcard(void)
{
    if(getCommHardVersion() > 0)
    {
        UART_PORT = 4;
    }
    else
    {
        UART_PORT = 6;
    }
	UARTInit(UART_PORT,115200);
}


BYTE SendcardCheckSum(BYTE *buffer, int size)
{
	u32 sum = 0;
	int i;
	for(i=0; i<size; i++)
	{
		sum += buffer[i];
	}
	return (BYTE)sum;
}

u32 SendcardCheckSum_u32(BYTE *buffer, int size)
{
	u32 sum = 0;
	int i;
	for(i=0; i<size; i++)
	{
		sum += buffer[i];
	}
	return sum;
}



//static u8 NW_Read_Brightness_Command[20] = {0x55 ,0xAA ,0x00 ,0x14 ,0xFE ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x02 ,0x05 ,0x00 ,0x70 ,0x56};
static u8 NW_Write_Brightness_Command[21] ={0x55 ,0xAA ,0x00 ,0x15 ,0xFE ,0x00 ,0x01 ,0xFF ,0xFF ,0xFF ,0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x02 ,0x01 ,0x00 ,0x80 ,0xEB ,0x59};
static u8 NW_Save_Brightness_Command[21] = {0x55 ,0xAA ,0x00 ,0x15 ,0xFE ,0x00 ,0x01 ,0xFF ,0xFF ,0xFF ,0x01 ,0x00 ,0x11 ,0x00 ,0x00 ,0x01 ,0x01 ,0x00 ,0x00 ,0x7A ,0x59};
u8 Serial_Number = 1;


bool NW_Brightness_Command_Write(u8 Brightness)
{
    u8 i;
	u16 Check_SUM = 0;
	u32 startTime;
	
	NW_Write_Brightness_Command[3] = Serial_Number ++;
	NW_Write_Brightness_Command[7] = 0XFF;
	NW_Write_Brightness_Command[8] = 0XFF;
	NW_Write_Brightness_Command[9] = 0XFF;
	NW_Write_Brightness_Command[18] = Brightness;
	Check_SUM = 0;
	for(i = 2;i < 19;i ++)
	{
		Check_SUM += NW_Write_Brightness_Command[i];
	}
	Check_SUM += 0x5555;
	NW_Write_Brightness_Command[19] = (u8)Check_SUM;
	NW_Write_Brightness_Command[20] = (u8)(Check_SUM >> 8);
	UARTSendBuffer(UART_PORT,(char *)NW_Write_Brightness_Command,21);
	//SysDelay1ms(1000);
	startTime = GetSysTick();
	while(ReceiveCount!=20)
	{
		if(isSysTickTimeout(startTime,1000))
		{
			printf("can't receive cmd from receive card3 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card3 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive 1 ReceiveCount = %d\r\n", ReceiveCount);
			return FALSE;
		}
	}
	//printf("can't receive cmd from receive card6 = %x\r\n", UART1Buffer_Receive[0]);
	//printf("can't receive cmd from receive card6 = %x\r\n", UART1Buffer_Receive[1]);
	//printf("can't receive startTime = %d\r\n", (GetSysTick()-startTime));
	SysDelay1ms(100);
	return TRUE;
}

bool NW_Brightness_Command_Save(void)
{
    u8 i;
	u16 Check_SUM = 0;
	u32 startTime;
	
	NW_Save_Brightness_Command[3] = Serial_Number ++;
	Check_SUM = 0;
	for(i = 2;i < 19;i ++)
	{
		Check_SUM += NW_Save_Brightness_Command[i];
	}
	Check_SUM += 0x5555;
	NW_Save_Brightness_Command[19] = (u8)Check_SUM;
	NW_Save_Brightness_Command[20] = (u8)(Check_SUM >> 8);
	UARTSendBuffer(UART_PORT,(char *)NW_Save_Brightness_Command,21);
	//SysDelay1ms(500);

	startTime = GetSysTick();
	while(ReceiveCount!=20)
	{
		if(isSysTickTimeout(startTime,1000))
		{
			printf("can't receive cmd from receive card4 = %x\r\n", UART1Buffer_Receive[0]);
			printf("can't receive cmd from receive card4 = %x\r\n", UART1Buffer_Receive[1]);
			printf("can't receive cmd from receive 2 ReceiveCount = %d\r\n", ReceiveCount);
			return FALSE;
		}
	}
	//printf("can't receive cmd from receive card7 = %x\r\n", UART1Buffer_Receive[0]);
	//printf("can't receive cmd from receive card7 = %x\r\n", UART1Buffer_Receive[1]);
	SysDelay1ms(100);
	return TRUE;
} 



