#ifndef _YDS_PROTOCOL_H_
#define _YDS_PROTOCOL_H_

#include "define.h"
#include "pipe.h"


typedef void (*RGBlinkProtocolCallback)(BYTE *receiveBuffer);
typedef void (*SendBufferFunc)(char * buffer, int size);

typedef enum 
{
	interpret_cmd_init,
	interpret_cmd_start,
	interpret_cmd_data,
	interpret_cmd_end,
} InterpretCmdState_TypeDef;


enum {
	
	CMD_INDEX_ADDRESS,
	CMD_INDEX_SN,
	CMD_INDEX_CMD,
	CMD_INDEX_DAT1,
	CMD_INDEX_DAT2,
	CMD_INDEX_DAT3,
	CMD_INDEX_DAT4,
	CMD_INDEX_CHECKSUM,
	CMD_INDEX_SIZE,
	
};


typedef struct
{
	BYTE address;
	BYTE sn;
	BYTE command;
	BYTE data_1;
	BYTE data_2;
	BYTE data_3;
	BYTE data_4;
	BYTE checksum;
} Command_TypeDef;


typedef struct {
	BYTE isConnect;
	BYTE address;
	BYTE index;
	BYTE state;
	BYTE sendSn;
	BYTE receiveSn;
	BYTE hexCharBuffer[2];
	BYTE receiveBuffer[CMD_INDEX_SIZE];	
	RGBlinkProtocolCallback callback;
	SendBufferFunc sendBuffer;
	Pipe_t *pipe;
}RGBlinkProtocol;



void Init_RGBlinkProtocol(RGBlinkProtocol * protocol, int address, Pipe_t *pipe, RGBlinkProtocolCallback callback, SendBufferFunc sendBuffer );
void Task_RGBlinkProtocol(RGBlinkProtocol * protocol);
int  SendCmd_RGBlinkProtocol(RGBlinkProtocol * protocol, int cmd, int dat1, int dat2, int dat3, int dat4);
int  SendCmdAndWait_RGBlinkProtocol(RGBlinkProtocol * protocol, int cmd, int dat1, int dat2, int dat3, int dat4);
void SendBuffer_RGBlinkProtocol(RGBlinkProtocol * protocol, char *buffer, int size);
int WaitAck_RgblinkProtocol(RGBlinkProtocol * protocol, int timeout);







#endif

