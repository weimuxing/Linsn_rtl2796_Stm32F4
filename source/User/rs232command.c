/*
 * rs232command.c
 *
 *  Created on: 2009-5-14
 *      Author: Administrator
 */
#define _RS232COMMAND_C_

#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "SysConfig.h"
#include "I2C.h"
#include "spi_flash.h"
#include "pipe.h"
#include "systick.h"
#include "userpref.h"
#include "control.h"
#include "mb103.h"
#if defined(USE_STDP9320)
#include "stdp9320.h"
#elif defined(USE_STDP8028)
#include "mst6m48.h"
#else
error!
#endif
#include "rgblinkProtocol.h"
#include "SiI9134.h"
#include "lcd.h"
#include "keyscan.h"
#include "menuData.h"
#include "menu.h"
#include "videoFormatTable.h"
#include "timecontrol.h"
#include "oldTimeControl.h"
#include "Input.h"
#include "FPGA.h"
#include "rs232command.h"
#include "processKey.h"
#include "UartDebug.h"






#define ENABLE_WRITE_EDID 0

/*
 * 外部变量
 */
/* at_System.c */
//extern	LOCALM localm[];
#define LocM   localm[NETIF_ETH]

/*
 * 定义常量
 */
 
//static char  CommandToLEDHeader[] = ":TOVC";
//static char  CommandFromLEDHeader[] = ":FRVC06";
//static char  CommandEnd[] = "\r\n";

//static char  CommandToLEDHeaderNew[] = "<F";
//static char  CommandEndNew[] = ">";
	
#define CMD_TYPE_OLD 0
#define CMD_TYPE_NEW 1
	
//static u8 cmdType = CMD_TYPE_NEW ;
#define cmdType  CMD_TYPE_NEW 


#define STR_COMMAND_LENGTH 150//(  sizeof(CommandToLEDHeader)-1+2*sizeof(Rs232CommandToLED)+sizeof(CommandEnd)-1  )

typedef struct _ReceiveCommandType
{
	char Buffer[STR_COMMAND_LENGTH];
	BYTE Index;
} ReceiveCommandType;

/*
 * 定义局部变量
 */

//static Rs232CommandToLED commandToLED;			// 存储接收的命令结构体
static Rs232CommandToLEDNew commandToLEDNew;			// 存储接收的命令结构体
//static char strCommandFromLED[sizeof(CommandFromLEDHeader)-1+2*sizeof(Rs232CommandFromLED)+sizeof(CommandEnd)-1+1];	// 存储要发送的命令str
//static Rs232CommandFromLED commandFromLED;		// 存储要发送的命令结构体
//static ReceiveCommandType g_ReceiveCommand;
//static u8 g_ReceiveDetected;
static u32 delayTimeCount = 0;
static u32 delayTimeStart;
static u32 delayUpdateTimeStart = 0;
static u32 delayUpdateTimeCount = 0;
//static char commandFromType = COMMAND_FROM_UART;
static char commMedium = COMMAND_FROM_UART;

//static int receive129ByteFromPC(u8 *rxBuffer);

// define for FPGA program saved in SPI flash
u8 FPGA_GUID[] =  {0x62,0x05,0x7F,0x97,0x28,0x63,0x45,0xEC,0x8A,0x7B,0xD2,0xB3,0x8D,0x90,0x14,0xEC};
u8 FPGA_END_SIGN[] = {0x19,0x81,0x02,0x14};

//static u32 fpgaFileSize = 0;
//static u32 fpgaFileChecksum = 1;

//static void SendAckOrNack(BYTE value);
void selfCheckkTask(void);
void WrLEDControlTask(void);  //0x9c
//static int receiveBufferFromPC(u8 *rxBuffer, int size);
extern char getFrontType(void);





stControlProtocal st_ControlProtocal;


void setCommandToLEDNew(char from, char address,char sn,char cmd,char dat1,char dat2, char dat3, char dat4)
{
//	commandFromType = from;
//	commandToLEDNew.address = address;
//	commandToLEDNew.sn = sn;
//	commandToLEDNew.command = cmd;
//	commandToLEDNew.data_1 = dat1;
//	commandToLEDNew.data_2 = dat2;
//	commandToLEDNew.data_3 = dat3;
//	commandToLEDNew.data_4 = dat4;
//	
//	commandToLED.address = address;
//	commandToLED.command = cmd;
//	commandToLED.data_1 = dat1;
//	commandToLED.data_2 = dat2;
//	commandToLED.data_3 = dat3;
//	commandToLED.data_4 = dat4;
}

char getCommMedium(void)
{
	return commMedium;
}

void asynRs232Delay(u32 nTime)  // delay nTime ms
{
	delayTimeStart = GetSysTick();
	delayTimeCount = nTime;
}

void asynRs232DisableDelay(void)  // disable delay
{
	delayTimeCount = 0;
}
bool asynRs232Timeout(void)
{
	if(delayTimeCount == 0)
	{
		return FALSE;
	}
	return isSysTickTimeout(delayTimeStart, delayTimeCount);
}

static void asynUpdateRs232Delay(u32 nTime)  // delay nTime ms
{
	delayUpdateTimeStart = GetSysTick();
	delayUpdateTimeCount = nTime;
}

static bool asynUpdateRs232Timeout(void)
{
	if(delayUpdateTimeCount == 0)
	{
		return TRUE;
	}
	if(isSysTickTimeout(delayUpdateTimeStart, delayUpdateTimeCount))
	{
		delayUpdateTimeCount = 0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//#include <stdio.h>

#define RS232_COMMAND_BUFFER_SIZE (UPDATE_BlOCK_SIZE+256)

static BYTE command_buffer[RS232_COMMAND_BUFFER_SIZE];

static Pipe_t command_pipe;

//static u8 ReceiveCommand(BYTE value);

u8 CheckSum(Rs232CommandToLED *rsCom)
{
	BYTE RSchecksum = 0;
	int cnt;	
	BYTE *rs;
	if(cmdType == CMD_TYPE_OLD)
	{
		rs = (BYTE *) rsCom;
		for(cnt=2; cnt<sizeof(Rs232CommandToLED)-1; cnt++)
		{
			rs[cnt] -= (*rsCom).offset;
			RSchecksum += rs[cnt];
		}
		(*rsCom).checksum -= (*rsCom).offset;

		if((*rsCom).checksum != RSchecksum)
		{
			#if DEBUG_RS232
			printf("checksum=%x\r\n", RSchecksum);
			#endif
			return FALSE;
		}
	}
	else if(cmdType == CMD_TYPE_NEW)
	{	
		RSchecksum = commandToLEDNew.address+commandToLEDNew.sn+commandToLEDNew.command+
		commandToLEDNew.data_1+commandToLEDNew.data_2+commandToLEDNew.data_3+commandToLEDNew.data_4;
	
		if(commandToLEDNew.checksum != RSchecksum)
		{
			#if DEBUG_RS232
			printf("checksum=%x\r\n", RSchecksum);
			#endif
			return FALSE;			
		}
	}
	return TRUE;
}

// 读取ASCII命令串，转换为Rs232CommandToLED结构
// 要求ASCII命令串结构是正确的。
// ASCII命令串  :  "<T协议数据>"
void StrToRs232Command(char *strCom, Rs232CommandToLED *rsCom)
{
	BYTE i;
	BYTE * rs;

	if(cmdType == CMD_TYPE_OLD)
	{
		strCom += 5;
		rs = (BYTE *) rsCom;

		for(i=0; i<sizeof(Rs232CommandToLED); i++)
		{
			*rs = HexCharToByte(strCom);
			rs++;
			strCom += 2;
		}
		commandToLEDNew.command = rsCom->command;
		commandToLEDNew.data_1 = rsCom->data_1;
		commandToLEDNew.data_2 = rsCom->data_2;
		commandToLEDNew.data_3 = rsCom->data_3;
		commandToLEDNew.data_4 = rsCom->data_4;
	}
	else if(cmdType == CMD_TYPE_NEW)
	{
		strCom += 2;
		rs = (BYTE *) &commandToLEDNew;
		for(i=0; i<sizeof(Rs232CommandToLEDNew); i++)
		{
			*rs = HexCharToByte(strCom);
			rs++;
			strCom += 2;
		}
		rsCom->command = commandToLEDNew.command;
		rsCom->data_1 = commandToLEDNew.data_1;
		rsCom->data_2 = commandToLEDNew.data_2;
		rsCom->data_3 = commandToLEDNew.data_3;
		rsCom->data_4 = commandToLEDNew.data_4;
	}
}

/*
 * Function: Rs232CommandToStr
 * Usage: Rs232CommandToStr(rsFromLED, str);
 * In: rsFromLED
 * Out: str
 * ---------------------------------------------
 * 1. 计数checksum
 * 2. 把Rs232CommandFromLED结构体打包为字符串
 * ASCII命令串  :  ":FRVC协议数据\r\n"
 * 要求事先要把协议数据以外的内容填入。
 */
void Rs232CommandToStr(Rs232CommandFromLED *rsFromLED, char *str)
{
//	BYTE i;
//	BYTE * rsArray;
//	str += sizeof(CommandFromLEDHeader)-1;
//	rsArray = (BYTE *) rsFromLED;
////	SetCheckSum(rsFromLED);
//	for(i=0; i<sizeof(Rs232CommandFromLED); i++)
//	{
//		ByteToHexChar(*rsArray, str);
//		rsArray++;
//		str += 2;
//	}
}
/*
 * Function: Rs232CommandToStr1
 * Usage: Rs232CommandToStr1(rsFromLED, str);
 * In: rsFromLED
 * Out: str
 * ---------------------------------------------
 * 1. 计数checksum
 * 2. 把Rs232CommandFromLED结构体打包为字符串
 * ASCII命令串  :  ":FRVC06协议数据的data部分\r\n"
 * 要求事先要把协议数据以外的内容填入。
 */
void Rs232CommandToStr1(Rs232CommandFromLED *rsFromLED, char *str)
{
//	BYTE i;
//	BYTE * rsArray;
//	str += sizeof(CommandFromLEDHeader) -1;
//	rsArray = (BYTE *) rsFromLED;
//	rsArray += 3; 
//	for(i=0; i<4; i++)
//	{
//		ByteToHexChar(*rsArray, str);
//		rsArray++;
//		str += 2;
//	}
//	*str = '\r';
//	str++;
//	*str = '\n';
//	str++;
//	*str = '\0';
}


// char2由两个char组成 把两个ASCII码转换为一个BYTE
BYTE HexCharToByte(char *char2)
{
	BYTE value = 0;
	BYTE i = 0;

	for(i=0; i<2; i++)
	{
		value *= 16;
		if(('0' <= *char2) && (*char2 <= '9'))
		{
			value += *char2 - '0';
		}
		else if(('a' <= *char2) && (*char2 <= 'f'))
		{
			value += *char2 - 'a' + 10;
		}
		else if(('A' <= *char2) && (*char2 <= 'F'))
		{
			value += *char2 - 'A' + 10;
		}

		char2++;

	}
	return value;
}
char * ByteToHexChar(BYTE value, char *char2)
{
	BYTE value2 = value >> 4;
	if(value2 <= 9)
	{
		char2[0] = '0' + value2;
	}
	else
	{
		char2[0] = 'a' + (value2 - 10);
	}

	value2 = value & 0x0F;
	if(value2 <= 9)
	{
		char2[1] = '0' + value2;
	}
	else
	{
		char2[1] = 'a' + (value2 - 10);
	}
	return char2;

}



/*
 * Function: InitRs232CommandVariable
 * Usage: InitRs232CommandVariable();
 * ----------------------------------------
 * 初始化全局变量
 */
void InitRs232CommandVariable(void)
{
//	BYTE i;	

	// Initial strCommandFromLED
//	strCommandFromLED[sizeof(strCommandFromLED)-1] = '\0';
//	for(i=0; i<sizeof(CommandFromLEDHeader)-1; i++)
//	{
//		strCommandFromLED[i] = CommandFromLEDHeader[i];
//	}
//	for(i=0; i<sizeof(CommandEnd)-1; i++)
//	{
//		strCommandFromLED[sizeof(strCommandFromLED)-sizeof(CommandEnd)+i] = CommandEnd[i];
//	}

//	g_ReceiveDetected = FALSE;

	pipe_init(&command_pipe, command_buffer, sizeof(command_buffer));
}

void ProcessPcCmd(stControlProtocal *pControl)
{
	switch(pControl->pCmd)
	{
		case 0x13:	//DVI
			SetVideoInputSource((u8)(pControl->pAddr),pControl->pFunc1,pControl->pFunc2,pControl->pFunc3,pControl->pFunc4);	
		break;
		case 0x23:	//HDMI1
			LoadSave(pControl->pFunc1);
		break;
		case 0xa2:	//HDMI2
		break;
		case 0xa3:	//DP
		break;
		default:
		break;
	}
}


char TaskPcControl(u8 *buf)
{
	//u8 cmpBuf[64];
	//u16 addr,data;
	//char *offset;
	//char r = 'r';
	
	if(GetUartDataTranFinishFlag() == true)
	{
		SetUartDataTranFinishFlag(false);
		
		//strcpy((char*)cmpBuf,(const char*)buf);
		
		memcpy((u8*)&st_ControlProtocal,uartxRxStatus.uart1RxBuf,sizeof(st_ControlProtocal));

		if(st_ControlProtocal.pBegin != 0x53 || st_ControlProtocal.pEnd != 0x4e)	//检查头尾
		{
			ClrUartDataTranFinishLen();
			return 0;
		}
		ProcessPcCmd(&st_ControlProtocal);
		ClrUartDataTranFinishLen();
	}
	
}



void PcDebugTask(void)
{
	TaskPcControl(uartxRxStatus.uart1RxBuf);
}

#undef _RS232COMMAND_C_


