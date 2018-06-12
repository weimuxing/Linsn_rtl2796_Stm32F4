#include "rgblinkProtocol.h"
#include <string.h>
#include "systick.h"
#include "rs232command.h"
#include <stdio.h>


//typedef enum 
//{
//	interpret_cmd_init,
//	interpret_cmd_start,
//	interpret_cmd_data,
//	interpret_cmd_end,
//} InterpretCmdState_TypeDef;

static const char cmd_header[] = {'<','F'};
static const char cmd_end[] = {'>'};

static void _interpret_cmd_char(RGBlinkProtocol * protocol, u8 value)
{
	
	if(value == '<')
	{
		protocol->state = interpret_cmd_init;
	}
	
	switch(protocol->state)
	{
	case interpret_cmd_init:
		if(value == '<')
		{
			protocol->index = 1;
			protocol->state = interpret_cmd_start;
		}
		break;
	case interpret_cmd_start:		
		if(value == cmd_header[protocol->index++])
		{
			if(protocol->index >= sizeof(cmd_header))
			{
				protocol->state = interpret_cmd_data;
				protocol->index = 0;
			}
		}
		else
		{
			protocol->state = interpret_cmd_init;
			//DebugPutString("Preamp Start Header error\r\n");
		}
		break;
	case interpret_cmd_data:
		if((protocol->index & 0x01) == 0)
		{
			protocol->hexCharBuffer[0] = value;
			++protocol->index;
			break;
		}
		if(protocol->index/2 >= CMD_INDEX_SIZE)
		{
			protocol->state = interpret_cmd_init;
		}
		else
		{
			protocol->hexCharBuffer[1] = value;
			protocol->receiveBuffer[protocol->index/2] = HexCharToByte((char *) protocol->hexCharBuffer);
			++protocol->index;
			if(protocol->index/2 >= CMD_INDEX_SIZE)
			{
				protocol->state = interpret_cmd_end;
				protocol->index = 0;
			}
		}
		break;
	case interpret_cmd_end:
		if(value == cmd_end[protocol->index])
		{
			++protocol->index;
			if(protocol->index >= sizeof(cmd_end))
			{
				protocol->state = interpret_cmd_init;
				
				protocol->receiveSn = protocol->receiveBuffer[CMD_INDEX_SN];
				
				protocol->index = 0;
				protocol->isConnect = 1;
				
				// active cmd
				protocol->callback(protocol->receiveBuffer);
			}
		}
		else
		{
			protocol->state = interpret_cmd_init;			
		}
		break;
	default:
		protocol->state = interpret_cmd_init;
		break;
	}
	
}


void Task_RGBlinkProtocol(RGBlinkProtocol * protocol)
{
	u8 value;
	while(pipe_read(protocol->pipe, &value))
	{
		_interpret_cmd_char(protocol, value);
	}
		
}


void Init_RGBlinkProtocol(RGBlinkProtocol * protocol, int address, Pipe_t *pipe, RGBlinkProtocolCallback callback, SendBufferFunc sendBuffer )
{
	protocol->pipe = pipe;
	protocol->callback = callback;
	protocol->index = 0;
	protocol->state = interpret_cmd_init;
	protocol->receiveSn = 0;
	protocol->sendSn = 0;
	protocol->isConnect = 0;
	protocol->address = address;
	protocol->sendBuffer = sendBuffer;
}

static void _send_cmd_rgblink(RGBlinkProtocol * protocol, int cmd, int dat1, int dat2, int dat3, int dat4)
{
	char buffer[20];
	BYTE checksum = 0;
	
	++protocol->sendSn;
	
	checksum = protocol->sendSn+protocol->address+cmd+dat1+dat2+dat3+dat4;
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		protocol->address, protocol->sendSn, cmd, dat1, dat2, dat3, dat4, checksum);

	protocol->sendBuffer(buffer, strlen(buffer));
}

static bool canSendCmd_rgblink(RGBlinkProtocol * protocol)
{
	int receive_sn;
	int send_sn;

	receive_sn = protocol->receiveSn;
	send_sn = protocol->sendSn;
	
	if(receive_sn <= send_sn)
	{
		if((send_sn-receive_sn)<5)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if((send_sn + (256-receive_sn)) < 5)
		{
			return TRUE;			
		}
		else
		{
			return FALSE;
		}
	}
}


int SendCmd_RGBlinkProtocol(RGBlinkProtocol * protocol, int cmd, int dat1, int dat2, int dat3, int dat4)
{
	u32 startTime;
	
	_send_cmd_rgblink(protocol,cmd,dat1,dat2,dat3,dat4);

	startTime = GetSysTick();
	
	// wait ack
	while((!canSendCmd_rgblink(protocol)) && protocol->isConnect)
	{
		Task_RGBlinkProtocol(protocol);
		if(isSysTickTimeout(startTime,3000))
		{
			printf("can't receive %d = %d\r\n", protocol->address, protocol->sendSn);
			protocol->isConnect = 0;
			return FALSE;
		}
	}
	if(protocol->isConnect == FALSE)
	{
		while((!canSendCmd_rgblink(protocol)))
		{
			Task_RGBlinkProtocol(protocol);
			if(isSysTickTimeout(startTime,200))
			{
				printf("no connected %d = %d\r\n",  protocol->address, protocol->sendSn);
				protocol->isConnect = 0;
				return FALSE;
			}
		}
	}
	return TRUE;
	
}

int WaitAck_RgblinkProtocol(RGBlinkProtocol * protocol, int timeout)
{
	u32 startTime;

	startTime = GetSysTick();
	
	// wait ack
	while((protocol->receiveSn != protocol->sendSn))
	{
		Task_RGBlinkProtocol(protocol);
		if(isSysTickTimeout(startTime,timeout))
		{
			printf("can't receive %d = %d\r\n",  protocol->address, protocol->sendSn);
			protocol->isConnect = 0;
			return FALSE;
		}
	}
	return TRUE;
}

int SendCmdAndWait_RGBlinkProtocol(RGBlinkProtocol * protocol, int cmd, int dat1, int dat2, int dat3, int dat4)
{
	_send_cmd_rgblink(protocol,cmd,dat1,dat2, dat3,dat4);
	
	if(protocol->isConnect)
	{
		return WaitAck_RgblinkProtocol(protocol, 3000);
	}
	else
	{
		return WaitAck_RgblinkProtocol(protocol, 200);
	}
		
}

void SendBuffer_RGBlinkProtocol(RGBlinkProtocol * protocol, char *buffer, int size)
{
	protocol->sendBuffer(buffer, size);
}







