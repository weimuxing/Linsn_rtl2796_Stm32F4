#include "UART.h"
#include "SysTick.h"
#include "SysConfig.h"
#include "control.h"
#include "userpref.h"
#include "videoFormatTable.h"
#include "it6604.h"
#include "AD4604.h"
#include "langStr.h"
#include "SiI9134.h"
#include "rotaryEncoder.h"
#include "rs232command.h"
#include <stdio.h>

#include "THS8200.h"
#if defined(USE_STDP9320)
#include "stdp9320.h"
#elif defined(USE_STDP8028)
#include "mst6m48.h"
#else
error!
#endif
#include "processKey.h"
#include "edid.h"
#include "lcd.h"
#include "pipe.h"
#include "Input.h"
#include "keyScan.h"
//#include <Net_Config.h>
#include "menu.h"
#include "menuData.h"
#include "mb103.h"


//++++++++++++++++++++++++++++com to mainBoard+++++++++++++++++++++++++++++++++++++++++++++++++
#define MB103_UART_PORT  6
//u8 receive_mb103_buffer[1024];
Pipe_t receive_mb103_pipe;
u8 receive_mb103_sn = 0;
u8 send_mb103_sn = 0;
static u8 deviceIsConnet_mb103 = 1;
const char mb103_cmd_header[] = {'<','F'};
const char mb103_cmd_end[] = {'>'};
//MainBoard_Setting_TypeDef mb103_Setting;

static Command_TypeDef cmd_from_mb103;
static InterpretCmdState_TypeDef state_mb103 = interpret_cmd_init;
static u32 startMB103Time;

#define debugMB103EDID(x)     //x

MainBoard_Setting_TypeDef * getMb103Setting(void)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	
	return &mb103_Setting;
#endif

}


void init_mb103(void)
{
#if 0
	pipe_init(&receive_mb103_pipe,receive_mb103_buffer,sizeof(receive_mb103_buffer));
	UARTInit(6,115200); //main board 103

	mb103_Setting.outputEnable[0] = 0;         //默认打开输出
	mb103_Setting.outputEnable[1] = 0;         //默认打开输出

	mb103_Setting.switchStatus[0] = 0;          //开机默认切换主板DVI
	mb103_Setting.switchStatus[1] = 0;          //开机默认切换主板DVI
	
	mb103_Setting.ver_lsb = 0x01;
	mb103_Setting.ver_msb = 0x00;

	mb103_Setting.hardVer = 0;

	mb103_Setting.fansDutyRatio = 0;
	mb103_Setting.autoDuty = 1;
	mb103_Setting.temp = 0;

	mb103_Setting.hdmiSwitch[0] = 0;
	mb103_Setting.hdmiSwitch[1] = 0;

	#if 0  //刚开机的时候不等回码
	mb103_send(0x6A, 0x03, 0x00, 0x00, 0x00);	// read soft version
	mb103_send(0x6A, 0x05, 0x00, 0x00, 0x00);	// read hard version
	#else  
	mb103_sendAndWait(0x6A, 0x03, 0x00, 0x00, 0x00);	// read soft version
	mb103_sendAndWait(0x6A, 0x05, 0x00, 0x00, 0x00);	// read hard version
	#endif
#endif	
}

void SwitchCmdToPc_mb103(Command_TypeDef *cmd)
{
#if 0
	#if 0
	char buffer[30];
	Command_TypeDef cmdToPc;
	cmdToPc = *cmd;
	cmdToPc.checksum = cmdToPc.address+cmdToPc.sn+cmdToPc.command+cmdToPc.data_1+cmdToPc.data_2+cmdToPc.data_3+cmdToPc.data_4;
	sprintf(buffer, "<F%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", cmdToPc.address,cmdToPc.sn,cmdToPc.command,
			cmdToPc.data_1,cmdToPc.data_2,cmdToPc.data_3,cmdToPc.data_4,cmdToPc.checksum);
	SerialPutString(buffer);
	#else
	cmd->address = 0x01;
	cmd->checksum += 1;
	printf("<F%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", cmd->address,cmd->sn,cmd->command,
			cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,cmd->checksum);
	#endif
#endif
}

int ReceiveBuffer_mb103(char *buffer, int size, int timeout)
{
	u32 startTime;
	int i = 0;
#if 0		// No use in VipSmart,Cancel by hibernate	
	startTime = GetSysTick();
	
	while(1)
	{
		BYTE value;
		if(pipe_read(&receive_mb103_pipe, &value))
		{
			buffer[i++] = value;	
			if(i >= size)
			{
				break;
			}
		}
		if(isSysTickTimeout(startTime, timeout))
		{
			break;
		}
	}
#endif
	return i;
}

void SendBuffer_mb103(u8 * buffer, int size)
{
#if !(CURRENT_DEVICE == _VIP_SMART)		// No use in VipSmart,Cancel by hibernate	
	UARTSendBuffer(6,(char *)buffer,size);
#endif
}

int mb103_readBuffer( char *buffer, int size, int timeout)
{
#if (CURRENT_DEVICE == _VIP_SMART)		// No use in VipSmart,Cancel by hibernate	
	return 0;
#else
	return ReceiveBuffer_mb103(buffer,size,timeout);
#endif
}

static bool readFromMCUFlag = 0;

static void process_mb103_cmd(Command_TypeDef * cmd)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	

	deviceIsConnet_mb103 = 1;
	#if 0
	if(cmd->checksum != getCmdChecksum(cmd))
	{
		return;
	}
	#endif

	if((cmd->command == 0x6a)&&(readFromMCUFlag == 0))
	{
		SwitchCmdToPc_mb103( cmd);
	}
	
	//printf("process_mb103_cmd---->%d\r\n",GetSysTick());
	
	switch(cmd->command)
	{
		case 0x6a:
			switch(cmd->data_1)
			{
				case 0x03:
					if(readFromMCUFlag == 1)
					{
						readFromMCUFlag = 0;
					}
					mb103_Setting.ver_msb = cmd->data_2;
					mb103_Setting.ver_lsb = cmd->data_3;
					break;	
				case 0x05:
					if(readFromMCUFlag == 1)
					{
						readFromMCUFlag = 0;
					}
					mb103_Setting.hardVer = cmd->data_3;
					if(mb103_Setting.hardVer == 0)
					{
						printf("old front panel\r\n");
						setFrontType(0);
					}
					else if(mb103_Setting.hardVer == 2)
					{
						printf("new front panel\r\n");
						setFrontType(1);
					}
					else if(mb103_Setting.hardVer == 3)
					{
						printf("dvi dp board\r\n");
						setFrontType(2);
					}
					break;
				default:
					break;
			}
			break;	
		case 0x69:
			switch(cmd->data_1)
			{
				case 0x00:
				case 0x01: 
					mb103_Setting.regValue = cmd->data_4;
					break;
				case 0x30:
				case 0x31:
					mb103_Setting.fansDutyRatio = cmd->data_2;
					GetUserPref()->fanControl.fanDutyRatio = cmd->data_2;
					break;
				case 0x32:
				case 0x33:
					mb103_Setting.autoDuty = cmd->data_2;
					GetUserPref()->fanControl.autoDuty = cmd->data_2;
					break;
				case 0x41:
					mb103_Setting.temp = cmd->data_2;
					break;
				case 0x50: 
				case 0x51: 
					//printf("331_%d is %d \r\n",cmd->data_2,cmd->data_3);
					mb103_Setting.hdmiSwitch[cmd->data_2] = cmd->data_3;
					break;
				case 0x52:
				case 0x53:
					mb103_Setting.hdmiSwitchEQ[cmd->data_2][cmd->data_3] = cmd->data_4;
					break;
				case 0x54:
				case 0x55:
					mb103_Setting.hdmiSwitchDrv[cmd->data_2] = cmd->data_3;
					break;
				default:
					break;
			}
			break;
		case 0x73:
			switch(cmd->data_1)
			{
				case 0x00:
				case 0x01:
				case 0x04:
				case 0x05:
					mb103_Setting.switchStatus[cmd->data_3] = cmd->data_4;
					GetUserSetting()->switchStatusMB[cmd->data_3] = cmd->data_4;
					break;

				case 0x02:
				case 0x03:
					mb103_Setting.outputEnable[cmd->data_3] = cmd->data_4;
					break;

				default:
					break;
					
			}
			break;
		case 0x74:
			switch(cmd->data_1)
			{
				case 0x00:
				case 0x01:
					mb103_Setting.hdmiPortAudioEnble = cmd->data_3;
					break;
				default:
					break;
			}
			break;
		case 0xF1: 
			switch(cmd->data_1)
			{
				case 0x0c:    //read edid
					if(cmd->data_2 >= MB_EDID_PORT_SIZE)
					{
						return;
					}
					{
						int index,len;
						char buffer[257]; //256 + 1
						len = cmd->data_3 + (cmd->data_4<<8);
						if(len == 0)
						{
							getMb103Setting()->edidLen[cmd->data_2] = 0;
							//printf("<F:getMb103Setting()->edidLen[%d] = 0>\r\n", cmd->data_2);
							return;
						}
						index = mb103_readBuffer(buffer, len,200);
						if(index != len)
						{								
							//printf("<F:receive %d data from mainboard fail>\r\n", index);
							return;
						}
						getMb103Setting()->edidLen[cmd->data_2] = len;
						//printf receive file
						debugMB103EDID(printf("read port %d edid,len %d ,= \r\n",cmd->data_2,getMb103Setting()->edidLen[cmd->data_2]);)
						for(index = 0; index < len; index++)
						{
							getMb103Setting()->edidDetail[cmd->data_2][index] = buffer[index];
							debugMB103EDID(printf(" %02x  ",getMb103Setting()->edidDetail[cmd->data_2][index]);)//buffer[index]
						}
					}
					break;
			}
			break;
			
		case 0xff:
			mb103_Setting.regValue = cmd->data_4;
			break;
			
	}
	//pipe_init(&receive_mb103_pipe,receive_mb103_buffer,sizeof(receive_mb103_buffer));
#endif
}



static void interpret_mb103_char(u8 value)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	
	static u8 index = 0;
	static char hexCharBuffer[2];
	
	if(value == '<')
	{
		state_mb103 = interpret_cmd_init;
	}
	
	switch(state_mb103)
	{
	case interpret_cmd_init:
		if(value == '<')
		{
			index = 1;
			state_mb103 = interpret_cmd_start;
		}
		break;
	case interpret_cmd_start:		
		if(value == mb103_cmd_header[index++])
		{
			if(index >= sizeof(mb103_cmd_header))
			{
				state_mb103 = interpret_cmd_data;
				index = 0;
			}
		}
		else
		{
			state_mb103 = interpret_cmd_init;
			//DebugPutString("Preamp Start Header error\r\n");
		}
		break;
	case interpret_cmd_data:
		if((index & 0x01) == 0)
		{
			hexCharBuffer[0] = value;
			++index;
			break;
		}
		switch(index)
		{
		case 1:
			hexCharBuffer[1] = value;
			cmd_from_mb103.address= HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 3:
			hexCharBuffer[1] = value;
			cmd_from_mb103.sn = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 5:
			hexCharBuffer[1] = value;
			cmd_from_mb103.command = HexCharToByte(hexCharBuffer);
			++index;
			break;			
		case 7:
			hexCharBuffer[1] = value;
			cmd_from_mb103.data_1 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 9:
			hexCharBuffer[1] = value;
			cmd_from_mb103.data_2 = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 11:
			hexCharBuffer[1] = value;
			cmd_from_mb103.data_3 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 13:
			hexCharBuffer[1] = value;
			cmd_from_mb103.data_4 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 15:
			// get checksum no use
			hexCharBuffer[1] = value;
			cmd_from_mb103.checksum = HexCharToByte(hexCharBuffer);
			state_mb103 = interpret_cmd_end;
			index = 0;
			break;
		default:
			state_mb103 = interpret_cmd_init;
			break;
		}
		break;
	case interpret_cmd_end:
		if(value == mb103_cmd_end[index])
		{
			++index;
			if(index >= sizeof(mb103_cmd_end))
			{
				state_mb103 = interpret_cmd_init;
				// active cmd
				receive_mb103_sn = cmd_from_mb103.sn;
				//DebugPrintf("active=%d\r\n", receive_preamp_count);
				process_mb103_cmd(&cmd_from_mb103);
				index = 0;
			}
		}
		else
		{
			state_mb103 = interpret_cmd_init;
			//DebugPutString("preamp end header error\r\n");
			//DebugPrintf("end char = %x\r\n", value);
			//DebugPrintf("index = %d\r\n", index);
			//DebugPrintf("sizeof = %d\r\n", sizeof(preamp_cmd_end));
			
		}
		break;
	default:
		state_mb103 = interpret_cmd_init;
	}
#endif	
}

void task_mb103(void)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	
	u8 value;
	#if 1
	while(pipe_read(&receive_mb103_pipe,&value))
	{
		//USART_SendData(USART1, value);  // for test
		interpret_mb103_char(value);
		if(state_mb103 == interpret_cmd_start)
		{
			startMB103Time = GetSysTick();
		}
	}
	if((state_mb103 != interpret_cmd_init) && isSysTickTimeout(startMB103Time,1000))
	{
		state_mb103 = interpret_cmd_init;
	}
	#else
	while(pipe_read(&receive_preamp_pipe,&value))
	{		
		uart0_putchar(value);
	}
	#endif
#endif
}

void Isr_receive_mb103(u8 value)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	
	pipe_write(&receive_mb103_pipe, value);
#endif
}

static BYTE getCmdChecksum(Command_TypeDef * cmd)
{
	BYTE checksum;
	checksum = cmd->address+cmd->sn+cmd->command+cmd->data_1+cmd->data_2+cmd->data_3+cmd->data_4;
	return checksum;
}

void _send_cmd(Command_TypeDef *cmdStruct)
{
#if !(CURRENT_DEVICE == _VIP_SMART)
	char buffer[20];
	++send_mb103_sn;
	cmdStruct->sn = send_mb103_sn;
	cmdStruct->address = 0;
	
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MB103_UART_PORT, buffer);
#endif	
}
void mb103_sendBuffer(u8 *buffer, int size)
{
#if !(CURRENT_DEVICE == _VIP_SMART)
	UARTSendBuffer(MB103_UART_PORT,(char *)buffer,size);
#endif
}

bool send_cmd_to_mb103_and_wait(Command_TypeDef *cmdStruct)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	
	char buffer[20],repeat = 3;
	u32 startTime;
	
	#if 0
	//++send_mb103_sn;
	//send_sn = 0; // for test
	//cmdStruct->sn = send_mb103_sn;
	//cmdStruct->address = 0;
	buffer[0] = 0x09;
	buffer[1] = 0xff;
	buffer[2] = cmdStruct->sn;
	buffer[3] = cmdStruct->command;
	buffer[4] = cmdStruct->data_1;
	buffer[5] = cmdStruct->data_2;
	buffer[6] = cmdStruct->data_3;
	buffer[7] = cmdStruct->data_4;
	buffer[8] = stCheckSum((BYTE *)buffer, 8);
	UARTSendBuffer(MB103_UART_PORT, buffer, 9);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	UARTSendBuffer(1, buffer, 9);
	
	#else
	_send_cmd(cmdStruct);
	
	startTime = GetSysTick();
	
	//++send_mb103_sn;
	//send_sn = 0; // for test
	//cmdStruct->sn = send_mb103_sn;
	//cmdStruct->address = 0;
	//cmdStruct->checksum = getCmdChecksum(cmdStruct);
	//sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
	//	cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	//UARTSendStr(MB103_UART_PORT, buffer);
	//startTime = GetSysTick();
	#endif
	
	#if 1
	task_mb103();
	
	
	// wait ack
	while((send_mb103_sn != receive_mb103_sn) && deviceIsConnet_mb103 && repeat)
	{
		task_mb103();
		if(isSysTickTimeout(startTime,3000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from main board = %d,receive_sn: %d\r\n", send_mb103_sn,receive_mb103_sn);
			if(repeat)
			{
				printf("repeat: %d\r\n", repeat);
				UARTSendBuffer(MB103_UART_PORT, buffer, 9);
				printf("send mb103:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				task_mb103();
				startTime = GetSysTick();
				continue;
			}
			
			#if 1 //判断不能跟主板通信,则中断跟video的通信
			printf("diable communication with video\r\n");
			//DisableVideoProcessDevice();
			#endif
			deviceIsConnet_mb103 = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_mb103 == FALSE)
	{
		while((send_mb103_sn != receive_mb103_sn))
		{
			task_mb103();
			if(isSysTickTimeout(startTime,200))
			{
				printf("no main board = %d,receive_sn: %d\r\n", send_mb103_sn,receive_mb103_sn);
				deviceIsConnet_mb103 = 0;
				return FALSE;
			}
		}
	}
	#endif
#endif
	return TRUE;
}

u8 getdeViceIsConnet(void)
{
	return deviceIsConnet_mb103;
}

void send_cmd_to_mb103(Command_TypeDef *cmdStruct)
{
	#if 0
	char buffer[20],repeat = 3;
	u32 startTime;
	
	//++send_mb103_sn;
	//send_sn = 0; // for test
	//cmdStruct->sn = send_mb103_sn;
	//cmdStruct->address = 0;
	buffer[0] = 0x09;
	buffer[1] = 0xff;
	buffer[2] = cmdStruct->sn;
	buffer[3] = cmdStruct->command;
	buffer[4] = cmdStruct->data_1;
	buffer[5] = cmdStruct->data_2;
	buffer[6] = cmdStruct->data_3;
	buffer[7] = cmdStruct->data_4;
	buffer[8] = stCheckSum((BYTE *)buffer, 8);
	UARTSendBuffer(MB103_UART_PORT, buffer, 9);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	UARTSendBuffer(1, buffer, 9);
	
	#else
	_send_cmd(cmdStruct);
	
	//startTime = GetSysTick();
	
	//++send_mb103_sn;
	//send_sn = 0; // for test
	//cmdStruct->sn = send_mb103_sn;
	//cmdStruct->address = 0;
	//cmdStruct->checksum = getCmdChecksum(cmdStruct);
	//sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
	//	cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	//UARTSendStr(MB103_UART_PORT, buffer);
	//startTime = GetSysTick();
	#endif
	
	#if 0
	task_mb103();
	
	
	// wait ack
	while((send_mb103_sn != receive_mb103_sn) && deviceIsConnet_mb103 && repeat)
	{
		task_mb103();
		if(isSysTickTimeout(startTime,3000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from main board = %d,receive_sn: %d\r\n", send_mb103_sn,receive_mb103_sn);
			if(repeat)
			{
				printf("repeat: %d\r\n", repeat);
				UARTSendBuffer(MB103_UART_PORT, buffer, 9);
				task_mb103();
				startTime = GetSysTick();
				continue;
			}
			
			deviceIsConnet_mb103 = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_mb103 == FALSE)
	{
		while((send_mb103_sn != receive_mb103_sn))
		{
			task_mb103();
			if(isSysTickTimeout(startTime,200))
			{
				printf("no main board = %d,receive_sn: %d\r\n", send_mb103_sn,receive_mb103_sn);
				deviceIsConnet_mb103 = 0;
				return FALSE;
			}
		}
	}
	return TRUE;
	#endif
}

bool _wait_ack(int timeout)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	

	u32 startTime;
	startTime = GetSysTick();

	// wait ack
	while((send_mb103_sn != receive_mb103_sn))
	{
		task_mb103();
		if(isSysTickTimeout(startTime,timeout))
		{
			printf("can't receive mainboard103 = %d\r\n", send_mb103_sn);
			deviceIsConnet_mb103 = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_mb103 == FALSE)
	{
		return FALSE;
	}
#endif
	return TRUE;	
}

void mb103_send(int cmd, int dat1, int dat2, int dat3, int dat4)
{
#if !(CURRENT_DEVICE == _VIP_SMART)		// No use in VipSmart,Cancel by hibernate	
	Command_TypeDef cmdStruct;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	send_cmd_to_mb103(&cmdStruct);
#endif
}

bool mb103_sendAndWait(int cmd, int dat1, int dat2, int dat3, int dat4)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	// No use in VipSmart,Cancel by hibernate
	Command_TypeDef cmdStruct;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	return send_cmd_to_mb103_and_wait(&cmdStruct);
#endif
}

int mb103_sendProgramMcuBuffer(int address, u8 *buffer, int size)
{
#if !(CURRENT_DEVICE == _VIP_SMART)	

	int ack=0;
	mb103_send(0x9a, 0xa0, 0x00, address, address>>8);
	mb103_sendBuffer(buffer, size);
	if(address == 0)
	{
		ack = _wait_ack(10000);
	}
	else
	{
		ack = _wait_ack(3000);
	}
	if(ack)
	{
		if((cmd_from_mb103.data_3+(cmd_from_mb103.data_4<<8)) == address)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
#endif

	return FALSE;
}

int mb103_goToBoot(void)
{
	//return mb103_sendAndWait(0x9a,0x20,0,0,0);
}
void mb103_readDeviceType(void)
{
	//mb103_sendAndWait(0x68,0x01,0,0,0);
}
void mb103_getSoftVersion(void)
{
	//readFromMCUFlag = 1;
	//mb103_sendAndWait(0x6a,0x03,0,0,0);
	//mb103_send(0x6a,0x03,0,0,0);
}
void mb103_getHardVersion(void)
{
	//readFromMCUFlag = 1;
	//mb103_sendAndWait(0x6a,0x05,0,0,0);
}
void mb103_setSwitch(u8 output,u8 input)//output 0(TTL)-1(HDMI)             input 0-3   0- 主板DVI   1- 输入板182_1    2- 输入板182_2    3- 输入板单独DVI
{
	//mb103_Setting.switchStatus[output] = input;
	//mb103_sendAndWait(0x73,0x00,0,output&0x01,input&0x03);
}
u8 mb103_getSwitch(u8 output)//output 0(TTL)-1(HDMI)             input 0-3  
{
	//return mb103_Setting.switchStatus[output] ;
}
void mb103_setSwitch_noOutput(u8 output,u8 input)//output 0(TTL)-1(HDMI)             input 0-3   0- 主板DVI   1- 输入板182_1    2- 输入板182_2    3- 输入板单独DVI
{
	//mb103_Setting.switchStatus[output] = input;
	//mb103_sendAndWait(0x73,0x04,0,output&0x01,input&0x03);
}
void mb103_setOutputEnable(u8 output,u8 enable)//output 0(TTL)-1(HDMI)            
{
	//mb103_Setting.outputEnable[output] = enable;
	
	//mb103_sendAndWait(0x73,0x02,0,output&0x01,enable&0x01);
}
void mb103_setfansDutyRatio(char value)           
{
	//mb103_sendAndWait(0x69,0x30,value,0,0);
}
char mb103_getfansDutyRatio(void)           
{
	//mb103_sendAndWait(0x69,0x31,0,0,0);
	//return mb103_Setting.fansDutyRatio;
}
void mb103_setAutomaticDuty(char value)//1- auto   0- Manual
{
	//mb103_sendAndWait(0x69,0x32,value,0,0);
}
char mb103_getAutomaticDuty(void)           
{
	//mb103_sendAndWait(0x69,0x33,0,0,0);
	//return mb103_Setting.autoDuty;
}
char mb103_getCurrentTemp(void)           
{
	//mb103_sendAndWait(0x69,0x41,0,0,0);
	//return mb103_Setting.temp;
}
void mb103_reset_IT6604(void)
{
	//mb103_sendAndWait(0x69,0x20,0,0,0);
}
void mb103_reset_SiI9134(char port)
{
	//mb103_sendAndWait(0x69,0x20,1,port,0);
}
void mb103_reset_TH8200(void)
{
	//mb103_sendAndWait(0x69,0x20,2,0,0);
}
void mb103_reset_AD4604(void)
{
	//mb103_sendAndWait(0x69,0x20,3,0,0);
}


void mb103_read_EDID(u8 port)  //0- input port DVI  1- output port 0  2- output port 1   3- input port VGA   4- output port VGA   5- input port HDMI(主板V1.3及以上)
{
	//mb103_sendAndWait(0xF1,0x0C,port,0,0);
}
void mb103_write_EDID(u8 port,u8 *data,int len)  //0- input port DVI  
{
	//mb103_send(0xF0,0x0C,port,len,(len>>8));
	//SendBuffer_mb103(data,len);
}

void mb103_setPS331(int port, int inputPrt) //port   0-input  1-output          inputPort   0-2
{
	//mb103_send(0x69,0x50,port,inputPrt,0);
	//mb103_sendAndWait(0x69,0x50,port,inputPrt,0);
}
u8 mb103_getPS331(int port) //port   0-input  1-output          inputPort   0-2
{
	//mb103_send(0x69,0x51,port,0,0);
	//mb103_sendAndWait(0x69,0x51,port,0,0);
	//return mb103_Setting.hdmiSwitch[port];
}
void mb103_setPS331InputEQ(int port,int inputPrt, int eqValue) //port   0-input  1-output      inputPort   0-2    eqValue   0-15
{
	//mb103_send(0x69,0x52,port,inputPrt,eqValue);
	//mb103_sendAndWait(0x69,0x52,port,inputPrt,eqValue);
}
u8 mb103_getPS331InputEQ(int port,int inputPrt) //port   0-input  1-output          inputPort   0-2
{
	//mb103_send(0x69,0x53,port,inputPrt,0);
	//mb103_sendAndWait(0x69,0x53,port,inputPrt,0);
	//return mb103_Setting.hdmiSwitchEQ[port][inputPrt];
}
void mb103_setPS331OutputDriver(int port, int drvValue) //port   0-input  1-output          drvValue   0-3
{
	//mb103_send(0x69,0x54,port,drvValue,0);
	//mb103_sendAndWait(0x69,0x54,port,drvValue,0);
}
u8 mb103_getPS331OutputDriver(int port) //port   0-input  1-output          inputPort   0-2
{
	//mb103_send(0x69,0x55,port,0,0);
	//mb103_sendAndWait(0x69,0x55,port,0,0);
	//return mb103_Setting.hdmiSwitchDrv[port];
}

void mb103_setEP9142OutputHDMIEnable(bool enable)
{
	//mb103_send(0x74,0x00,0,enable,0);
	//mb103_sendAndWait(0x74,0x00,0,enable,0);
}
u8 mb103_getEP9142OutputHDMIEnable(void) 
{
	//mb103_send(0x74,0x01,0,0,0);
	//mb103_sendAndWait(0x74,0x01,0,0,0);
	//return mb103_Setting.hdmiPortAudioEnble;
}

void mb103_setEnableFlashWP(void)
{
	//mb103_sendAndWait(0x9a,0xc5,0,0,0);
}
void mb103_setDisableFlashWP(void)
{
	//mb103_sendAndWait(0x9a,0xc5,1,0,0);
}

void resetSiI9134_out(char port)
{
	//mb103_reset_SiI9134(port);
}
void mb_resetIT6604(void)
{
	//mb103_reset_IT6604();
}
void mb_resetTh8200(void)
{
	//mb103_reset_TH8200();
}
void mb_resetAD4604(void)
{
	//mb103_reset_AD4604();
}

u8 getMB103_inputSource(u8 output)//output 0(TTL)-1(HDMI)         input 0- dvi in mainboard   1- mst182_1 in ext  input   2- mst182_2 in ext input  3- single dvi in ext input
{
#if 0
	u8 input = mb103_getSwitch(output);

	//printf("getMB103_inputSource(%d) = %d\r\n",output,input);
	if(input == 0)
	{
		//(MB103_HDMISWITCH_PORT_INPUT,MB103_INPUT_DVI);
		if(getMB103_HDMISwitch(MB103_HDMISWITCH_PORT_INPUT) == MB103_INPUT_DVI)
		{
			return VIPDRIVE_INPUT_DP;
		}
		else
		{
			return VIPDRIVE_INPUT_DVI;
		}
	}
	if(input < 4)
	{
		return VIPDRIVE_INPUT_DVI+input;
	}
#endif
	return VIPDRIVE_INPUT_SIZE;
	
}
int getMB103_SoftVersion(void)
{
	//mb103_getSoftVersion();
	//return ((mb103_Setting.ver_msb<<8) + mb103_Setting.ver_lsb);
}
int getMB103_HardVersion(void)
{
	//mb103_getHardVersion();
	//return mb103_Setting.hardVer;
}

void getMB103_EDID(char port) //0- input port DVI  1- output port 0  2- output port 1  3- input port VGA   4- output port VGA   5- input port HDMI(主板V1.3及以上)
{
	//if(port >= MB_EDID_PORT_SIZE)
	{
	//	return;
	}
	//printf("getMB103_EDID(%d)\r\n",port);
	//mb103_read_EDID(port);	
}
void setMB103_EDID(char port,u8 *edidData,int len)//port 0- input port DVI          3- input port VGA       5- input port HDMI(主板V1.3及以上的HDMI接口)
{
	//if((port != MB_EDID_INPUT_DVI)&&(port != MB_EDID_INPUT_VGA)&&(port != MB_EDID_INPUT_HDMI))
	{
	//	return;
	}
	//mb103_write_EDID(port,edidData,len);
}

void setMB103_outputEnable(char port)//0(TTL)-1(HDMI)  
{
	if(port > 1)
	{
		return;
	}
	mb103_setOutputEnable(port,1);
}
void setMB103_outputDisable(char port)//0(TTL)-1(HDMI)  
{
	if(port > 1)
	{
		return;
	}
	mb103_setOutputEnable(port,0);
}
void setMB103_fansDutyRatio(char value) // 0-99
{
	if(value >= 100)
	{
		return;
	}
	mb103_setfansDutyRatio(value);
}
char getMB103_fansDutyRatio(void)
{
	return mb103_getfansDutyRatio();
}

void setMB103_AutomaticDuty(char value)//1- auto   0- Manual
{
	if(value > 1)
	{
		return;
	}
	mb103_setAutomaticDuty(value);
}
char getMB103_AutomaticDuty(void)
{
	return mb103_getAutomaticDuty();
}

char getMB103_CurrentTemperature(void)
{
	return mb103_getCurrentTemp();
}

void refleshMainBoardInfo_fans(void)
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	printf("refleshMainBoardInfo_fans\r\n");
	mb103_getAutomaticDuty();
	mb103_getfansDutyRatio();	
	getMB103_CurrentTemperature();
  #endif
	
}

//PS331_INPUT设置   : input_port 0- dvi输入          input_port 1-HDMI输入    input_port 2-预留
//PS331_OUTPUT设置: input_port 0-8028输出        input_port 1- dvi环路       input_port 2-预留
void setMB103_HDMISwitch(char port, char inputPort) //port 0-1             inputPort  0-2
{
	if((port > 1)||(inputPort > 2))
	{
		return;
	}
	mb103_setPS331(port,inputPort);
}
char getMB103_HDMISwitch(char port) //port 0-1             inputPort  0-2
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	if((port > 1))
	{
		return 0;
	}
	return mb103_getPS331(port);
#endif
}

void setMB103_InputSwitchEQ(char port,char inputPort, char eqValue) //port 0-1       inputPort  0-2        eqValue  0-f
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	if((port > 1)||(eqValue > 0x0f)||(inputPort > 2))
	{
		return;
	}
	mb103_setPS331InputEQ(port,inputPort,eqValue);
#endif
}
char getMB103_InputSwitchEQ(char port,char inputPort) //port 0-1       inputPort  0-2        eqValue  0-f
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	if((port > 1)||(inputPort > 2))
	{
		return 0;
	}
	return mb103_getPS331InputEQ(port,inputPort);
   #endif
}
void setMB103_InputSwitchDriver(char port, char drvValue) //port 0-1             drvValue  0-3
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	if((port > 1)||(drvValue > 3))
	{
		return;
	}
	mb103_setPS331OutputDriver(port,drvValue);
   #endif
}
char getMB103_InputSwitchDriver(char port) //port 0-1             drvValue  0-3
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	if((port > 1))
	{
		return 0;
	}
	return mb103_getPS331OutputDriver(port);
#endif   
}




void setMB103_HDMIOutputEnable(void) 
{
	//mb103_send(0x69,0x60,1,0,0);
}
void setMB103_HDMIOutputDisable(void) 
{
	//mb103_send(0x69,0x60,0,0,0);
}

void setMB103_iicRegister(char chipAddr, char regAddr,char value)
{
	//mb103_send(0x69,0x00,chipAddr,regAddr,value);
	//mb103_sendAndWait(0x69,0x00,chipAddr,regAddr,value);
}
char getMB103_iicRegister(char chipAddr, char regAddr)
{
	//mb103_send(0x69,0x01,chipAddr,regAddr,0);
	//mb103_sendAndWait(0x69,0x01,chipAddr,regAddr,0);
	//return mb103_Setting.regValue;
}

void setMB103_HDMIPortOutputAudioEnable(bool enable) 
{
	//mb103_setEP9142OutputHDMIEnable(enable);
}
void setMB103_HDMIPortOutputHDMI(void) 
{
	//mb103_setEP9142OutputHDMIEnable(1);
}
void setMB103_HDMIPortOutputDVI(void) 
{
    //mb103_setEP9142OutputHDMIEnable(0);
}


void ResetMainBoardSetting(void)
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	mb103_Setting.autoDuty = 1;
	mb103_setAutomaticDuty(mb103_Setting.autoDuty);
   #endif
}

char setMB103_selfCheckIC(int id)
{
#if !(CURRENT_DEVICE == _VIP_SMART)

	printf("setMB103_selfCheckIC(%d)\r\n",id);
	mb103_sendAndWait(0xff,0x00,(id&0x00ff),(id>>8),0);
	return mb103_Setting.regValue;	
#endif
}
//+++++++++++++test++++++++++++++++++++++++++++++

