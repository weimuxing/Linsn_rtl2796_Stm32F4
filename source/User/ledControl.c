
#include <stdio.h>
#include <string.h>
#include "SysTick.h"
#include "SysConfig.h"
#include "langStr.h"
#include "pipe.h"
#include "UART.h"
#include "ledcontrol.h"
#include "userPref.h"
#include "Mst6m48.h"
//#if defined(USE_STDP9320)
#include "stdp9320.h"
//#elif defined(USE_STDP8028)
//#include "mst6m48.h"
//#endif
#include "menu.h"
#include "menuData.h"
#include "LCD.H"


#include "rs232command.h"
#include "sendcard.h"
#include "sendcard_colorLight.h"
#include "sendcard_lxy.h"

#include "FPGA.h"


#define NO_SEND_CARD_DEBUG			// 软件调试时先关闭发送到卡


extern int UART_PORT;


#define LED_UART_PORT  4
//u8 receive_led_buffer[1024];
Pipe_t receive_led_pipe;
u8 receive_led_sn = 0;
u8 send_led_sn = 0;
//static u8 deviceIsConnet_led = 1;
//const char led_cmd_header[] = {'<','F'};
//const char led_cmd_header_from_wifi[] = {'<','T'};
const char led_cmd_end[] = {'>'};
static char CmdFromWifiFlag = 0; // 0- receive ack from LED_103    1- receive cmd from wifi

//static Command_TypeDef cmd_from_led;
//static InterpretCmdState_TypeDef state_led = interpret_cmd_init;
//static u32 startLEDTime;

//static WifiLedControl_Setting_TypeDef  wifiAndLedControlSetting;
#if 0
char formatToLED_LXY[] = 
{
	#if 0 //这几项X1不作为标准分辨率
	"1440x1080",// 8
	"1600x576",// 9
	"1600x900",// 10
	"1600x1000",// 11
	"1728x1296",// 14
	"2048x1280",// 18
	"2560x1440",// 19
	"2560x1600",// 20
	#endif

	21, //  MD_720x480_60p 		= 0,
	21, //  MD_720x576_50p 		= 1,
	21, //  MD_1280x720_60 		= 2,
	21, //  MD_1280x720_59_94 	= 3,
	21, //  MD_1280x720_50 		= 4,
	21, //  MD_1280x720_30 		= 5,
	21, //  MD_1280x720_29_97 	= 6,
	21, //  MD_1280x720_25 		= 7,
	21, //  MD_1280x720_24 		= 8,
	21, //  MD_1280x720_23_98 	= 9,	
	15, //MD_1920x1080_60 	= 10,
	21, //  MD_1920x1080_59_94 	= 11,
	21, //  MD_1920x1080_50 	= 12,
	21, //  MD_1920x1080_30 	= 13,
	21, //  MD_1920x1080_30psf 	= 14,
	21, //  MD_1920x1080_29_97 	= 15,
	21, //  MD_1920x1080_29_97psf = 16,
	21, //  MD_1920x1080_25 	= 17,
	21, //  MD_1920x1080_25psf 	= 18,
	21, //  MD_1920x1080_24 	= 19,
	21, //  MD_1920x1080_24psf 	= 20,
	21, //  MD_1920x1080_23_98 	= 21,
	21, //  MD_1920x1080_23_98psf= 22,
	0, // MD_640x480_60 		= 23,
	21, //  MD_640x480_75 		= 24,
	21, //  MD_640x480_85 		= 25,
	1, // MD_800x600_60 		= 26,
	21, //  MD_800x600_75 		= 27,
	21, //  MD_800x600_85 		= 28,
	2, // MD_1024x768_60 		= 29,
	21, //  MD_1024x768_75 		= 30,
	21, //  MD_1024x768_85 		= 31,	
	5, // MD_1280x1024_60 	= 32,
	21, //  MD_1280x1024_75 	= 33,
	21, //  MD_1280x1024_85 	= 34,
	12, // MD_1600x1200_60 	= 35,  
	// 以上是GS4911 支持的格式
	// 以下为新增格式
	21, //  MD_720x480_60i 		= 36,
	21, //  MD_720x576_50i 		= 37,
	//
	21, //  MD_1920x1080_60i 	= 38,
	21, //  MD_1920x1080_50i 	= 39,
	//
	21, //  MD_1152x864_75 		= 40,
	3, // MD_1280x768_60 		= 41,
	21, //  MD_1280x960_60 		= 42,
	21, //  MD_1280x960_85 		= 43,
	21, //  MD_1360x768_60 		= 44,
	6, // MD_1366x768_60 		= 45,
	21, //  MD_1400x1050_60 	= 46,
	7, // MD_1440x900_60 		= 47,
	17, //MD_2048x1152_60 	= 48,
	13, //MD_1680x1050_60 	= 49,
	16, //MD_1920x1200_60 	= 50,

	21, //  MD_1600x1200_60_reduce 	= 51,
	21, //  MD_1920x1080_60_reduce	= 52,
	21, //  MD_1440x900_60_reduce 	= 53,
	21, //  MD_1680x1050_60_reduce  = 54,
	21, //  MD_1400x1050_60_reduce  = 55,
	21, //  MD_2176x544_60_reduce	= 56,  // 有问题
	21, //  MD_1152x1152_60         = 57,  // 有问题
	21, //  MD_1280x1280_60         = 58,
	21, //  MD_2560x812_60			= 59,
	21, //  MD_2560x816_60			= 60,
	4, // MD_1280x800_60			= 61,
	21, //  MD_1024x768_59_94       = 62, //added for 8028 v0.40
	21, //  MD_1920x1080_59_94i     = 63, //added for 8028 v0.40
	
	//MD_CUSTOM               = 64,// 62,//MD_1024x768_59_94		= 62,
	
	21, //  MD_1440X240 	= 64,  //added by cyj  for 480I from Hard Disk Player_HDMI
	21, //  MD_1440X288 	= 65,  //added by cyj  for 576I from Hard Disk Player_HDMI

	
	21, //  MD_CUSTOM		    	=66,  //= 64,
	21, //  MD_1440x800_60_r		    = 67,   //added by julia 
	21, //  MD_1920x1120_60_r 	    = 68,   //added by juila 
	21, //  MD_2304x1152x50_r			= 69,	//added by julia 
	21, //  MD_1536x1536x60_r		= 70,	//added by juila 
};
#endif

bool led_sendAndWait(int cmd, int dat1, int dat2, int dat3, int dat4);
extern u8 GetSendCardUsbPlusState(void);
extern void delayms(int ms);


void init_ledControl(void)
{
#if 0		// Cancel by hibernate
	u8 i,j;
	SENDCARD_List_Typedef * send_Card;
	
	printf("init_ledControl\r\n");
	pipe_init(&receive_led_pipe,receive_led_buffer,sizeof(receive_led_buffer));
	UARTInit(LED_UART_PORT,115200);

	send_Card = getsendcard();

	if(send_Card == NULL)
	{
		return;
	}
	send_Card->sendcardtype = 0;
	send_Card->brightness = 255;
	send_Card->gamma = 18;

	send_Card->posx[0] = 0;
	send_Card->posy[0] = 0;
	send_Card->sizex[0] = 0x400;
	send_Card->sizey[0] = 0x200;

	send_Card->posx[1] = 0;
	send_Card->posy[1] = 0x200;
	send_Card->sizex[1] = 0x400;
	send_Card->sizey[1] = 0x100;

	send_Card->colortemR =0xff;
	send_Card->colortemG =0xff;
	send_Card->colortemB =0xff;

	for(i = 0; i < 2;i++)
	{
		for(j = 0;j < 2 ;j++)
		{
			send_Card->connecttype[i][j] = 0;
	
			send_Card->vline[i][j] = 1;
			send_Card->hline[i][j] = 1;

			send_Card->vsize[i][j] = 64;
			send_Card->hsize[i][j] = 64;	

			send_Card->xoffset[i][j] = 0;
			send_Card->yoffset[i][j] = 0;		
		}
	}

	send_Card->virtuaPort = 0;
	send_Card->choosecard = 0;
	send_Card->receivecardnumber = 1;		//changed by @cc20140725接收卡卡号默认从1开始到256；
	send_Card->x= 0;
	send_Card->y= 0;
	send_Card->sendcardinfo= 0;
	send_Card->receivecardinfo = 0;
	send_Card->lxynetwork = 1;			//changed by @cc20140726	默认U口

	send_Card->curoutput= 0;
	send_Card->cascade =0;

	if(formatToLED_LXY[GetUserSetting()->outputFormat] != 21)
	{
		send_Card->outputformat = formatToLED_LXY[GetUserSetting()->outputFormat];
	}
	else
	{ 
		send_Card->outputformat = 15; //1920x1080
	}
	

	led_sendAndWait(0x6A, 0x03, 0x00, 0x00, 0x00);	// read soft version
#endif
}



void SwitchCmdToPc_LED(Command_TypeDef *cmd)
{
#if 0		// Cancel by hibernate

	cmd->address = 0x06;
	cmd->checksum += 6;
	printf("<F%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", cmd->address,cmd->sn,cmd->command,
			cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,cmd->checksum);
#endif    
}
void SwitchCmdToMeduim_LED(Command_TypeDef *cmd)
{
#if 0		// Cancel by hibernate

	char buffer[30];
	sprintf(buffer, "<F%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", cmd->address,cmd->sn,cmd->command,
			cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,cmd->checksum);
	
	SendBufferToPC(buffer,19);
 #endif
}

int ReceiveBuffer_LED(char *buffer, int size, int timeout)
{
	int i = 0;
#if 0		// Cancel by hibernate

	u32 startTime;

	
	startTime = GetSysTick();
	
	while(1)
	{
		BYTE value;
		if(pipe_read(&receive_led_pipe, &value))
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

void SendBuffer_led(u8 * buffer, int size)
{
#if 0
UARTSendBuffer(LED_UART_PORT,(char *)buffer,size);
#endif
}

int led_readBuffer( char *buffer, int size, int timeout)
{
	return ReceiveBuffer_LED(buffer,size,timeout);
}

//static bool readFromMCUFlag = 0;

static void process_led_cmd(Command_TypeDef * cmd)
{
#if 0
	u8 curCard;
	u8 curPort;
	char len,buffer[100];
	deviceIsConnet_led = 1;
	#if 0
	if(cmd->checksum != getCmdChecksum(cmd))
	{
		return;
	}
	#endif
	
	if(CmdFromWifiFlag == 1)
	{
		printf("process_wifi_cmd\r\n");
		CmdFromWifiFlag = 0;
		setCommandToLEDNew(COMMAND_FROM_WIFI,cmd->address,cmd->sn,cmd->command,cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4);
		processCmdData();
		return;
	}	

	if((cmd->command == 0x6a)&&(readFromMCUFlag == 0))
	{
		SwitchCmdToPc_LED( cmd);
	}	
	
	//printf("process_LED_cmd---->%d\r\n",GetSysTick());
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
					getsendcard()->ver_msb = cmd->data_2;
					getsendcard()->ver_lsb = cmd->data_3;
					break;	
				default:
					break;
			}
			break;	
		case 0x8e:					
			if((cmd->data_2 == 0xff)&&(cmd->data_3 == 0xff)&&(cmd->data_4 == 0xff))
			{
				cmd->data_2 = 0;
				cmd->data_3 = 0;
				cmd->data_4 = 0;
			}
			switch(cmd->data_1)
			{
				case 0x04:
				case 0x05:
					break;

				case 0x0e:
				case 0x0f:
					len = cmd->data_4;	
					if(len == led_readBuffer(buffer, len, 200))
					{

					}
					break;
					
				case 0x10:
				case 0x11:
					if((cmd->data_2 == 0xff)&&(cmd->data_3 == 0xff)&&(cmd->data_4 == 0xff))
					{
						cmd->data_2 = 0;
						cmd->data_3 = 0;
						cmd->data_4 = 0;
					}

					len = cmd->data_4;
					if(len == led_readBuffer(buffer, len, 200))
					{
					}
					break;
					
				case 0x18:
				case 0x19:
					if(cmd->data_4 == 0xff)
					{
						cmd->data_4 = 0;
					}
					len = cmd->data_4;
					if(len == led_readBuffer(buffer, len, 200))
					{
					}
					break;
					
				case 0x1a:
				case 0x1b:
					if(cmd->data_2 == 0xff)
					{
						cmd->data_2 = 0;
					}
					break;

				case 0x1d:
					if(cmd->data_2 == 0xff)
					{
						cmd->data_2 = 0;
					}
					break;

				case 0x1f:
					if(cmd->data_4 == 0xff)
					{
						cmd->data_4 = 0;
					}
					len = cmd->data_4;	
					if(len == led_readBuffer(buffer, len, 200))
					{
					}
					break;

				case 0x24:
				case 0x25:
					if(cmd->data_4 == 0xff)
					{
						cmd->data_4 = 0;
					}
					len = cmd->data_4;
					if(len == led_readBuffer(buffer, len, 200))
					{		
					}
					break;

				case 0xf0:
					if(cmd->data_3 == 0xff)
					{
						cmd->data_3 = 0;
					}
					break;
					
				default:
					break;
			}
			break;
		case 0x90:
			switch(cmd->data_1)
			{
				case 0x00:
				case 0x01:
					getsendcard()->sendcardtype = cmd->data_3;
					break;
				case 0x02:
				case 0x03:
					getsendcard()->connecttype[CurCard][cmd->data_2] = cmd->data_3;
					break;
				case 0x04:
				case 0x05:
					getsendcard()->brightness = cmd->data_3;
					break;
				case 0x06:
				case 0x07:
					getsendcard()->gamma = cmd->data_3;
					break;
				case 0x08:
				case 0x09:
					getsendcard()->posx[cmd->data_2] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x0a:
				case 0x0b:
					getsendcard()->posy[cmd->data_2] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x0c:
				case 0x0d:
					getsendcard()->sizex[cmd->data_2] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x0e:
				case 0x0f:
					getsendcard()->sizey[cmd->data_2] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x10:
				case 0x11:
					switch(cmd->data_1)
					{
						case 0x00:
							getsendcard()->colortemR = cmd->data_3;
							break;
						case 0x01:
							getsendcard()->colortemG = cmd->data_3;
							break;
						case 0x02:
							getsendcard()->colortemB = cmd->data_3;
							break;
						default:
							break;
					}
					break;
				case 0x12:
				case 0x13:
					curCard = getsendcard()->choosecard;
					getsendcard()->hline[curCard][cmd->data_2] = cmd->data_3;
					break;
				case 0x14:
				case 0x15:
					curCard = getsendcard()->choosecard;
					getsendcard()->vline[curCard][cmd->data_2] = cmd->data_3;
					break;
				case 0x16:
				case 0x17:
					curCard = getsendcard()->choosecard;
					curPort = getsendcard()->lxynetwork;
					getsendcard()->hsize[curCard][curPort] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x18:
				case 0x19:
					curCard = getsendcard()->choosecard;
					curPort = getsendcard()->lxynetwork;
					getsendcard()->vsize[curCard][curPort] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x1a:
				case 0x1b:
					getsendcard()->choosecard = cmd->data_3;
					break;
				case 0x1c:
				case 0x1d:
					getsendcard()->receivecardnumber =  cmd->data_3;
					break;
				case 0x1e:
				case 0x1f:
					getsendcard()->x = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x20:
				case 0x21:
					getsendcard()->y = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x22:
				case 0x23:
					getsendcard()->xoffset[CurCard][cmd->data_2] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x24:
				case 0x25:
					getsendcard()->yoffset[CurCard][cmd->data_2] = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x26:
				case 0x27:
				case 0x3a:
				case 0x3b:
					getsendcard()->lxynetwork = cmd->data_3;
					break;
				case 0x28:
				case 0x29:
					getsendcard()->cascade = cmd->data_3;
					break;
				case 0x2b:
					getsendcard()->sendcardinfo = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x2d:
					getsendcard()->receivecardinfo = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x3c:
				case 0x3d:
					getsendcard()->outputformat = cmd->data_3;
					break;
					
				case 0xf0:
					getsendcard()->sendcardtype = cmd->data_3; //0-colorlight 1 -linsn 
					break;
			}
			break;

		case 0xff:
			getsendcard()->regValue = cmd->data_4;
			break;
			
	}
	//pipe_init(&receive_mb103_pipe,receive_mb103_buffer,sizeof(receive_mb103_buffer));
    #endif
}


#if 0
static void interpret_led_char(u8 value)
{
	static u8 index = 0;
	static char hexCharBuffer[2];
	
	if(value == '<')
	{
		state_led = interpret_cmd_init;
	}
	
	switch(state_led)
	{
	case interpret_cmd_init:
		if(value == '<')
		{
			index = 1;
			state_led = interpret_cmd_start;
		}
		break;
	case interpret_cmd_start:		
		//if(value == led_cmd_header[index++])
		if(value == led_cmd_header[index])
		{
			index++;
			if(index >= sizeof(led_cmd_header))
			{
				//printf("-");
				state_led = interpret_cmd_data;
				index = 0;
				CmdFromWifiFlag = 0;
			}
		}
		//else if(value == led_cmd_header_from_wifi[index++])
		else if(value == led_cmd_header_from_wifi[index])
		{
			index++;
			if(index >= sizeof(led_cmd_header_from_wifi))
			{
				//printf("+");
				state_led = interpret_cmd_data;
				index = 0;
				CmdFromWifiFlag = 1;
			}
		}
		else
		{
			state_led = interpret_cmd_init;
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
			cmd_from_led.address= HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 3:
			hexCharBuffer[1] = value;
			cmd_from_led.sn = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 5:
			hexCharBuffer[1] = value;
			cmd_from_led.command = HexCharToByte(hexCharBuffer);
			++index;
			break;			
		case 7:
			hexCharBuffer[1] = value;
			cmd_from_led.data_1 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 9:
			hexCharBuffer[1] = value;
			cmd_from_led.data_2 = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 11:
			hexCharBuffer[1] = value;
			cmd_from_led.data_3 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 13:
			hexCharBuffer[1] = value;
			cmd_from_led.data_4 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 15:
			// get checksum no use
			hexCharBuffer[1] = value;
			cmd_from_led.checksum = HexCharToByte(hexCharBuffer);
			state_led = interpret_cmd_end;
			index = 0;
			break;
		default:
			state_led = interpret_cmd_init;
			break;
		}
		break;
	case interpret_cmd_end:
		if(value == led_cmd_end[index])
		{
			++index;
			if(index >= sizeof(led_cmd_end))
			{
				state_led = interpret_cmd_init;
				// active cmd
				receive_led_sn = cmd_from_led.sn;
				//DebugPrintf("active=%d\r\n", receive_preamp_count);
				process_led_cmd(&cmd_from_led);
				index = 0;
			}
		}
		else
		{
			state_led = interpret_cmd_init;
			//DebugPutString("preamp end header error\r\n");
			//DebugPrintf("end char = %x\r\n", value);
			//DebugPrintf("index = %d\r\n", index);
			//DebugPrintf("sizeof = %d\r\n", sizeof(preamp_cmd_end));
			
		}
		break;
	default:
		state_led = interpret_cmd_init;
	}
}
#endif

void task_LED(void)
{
#if 0
	u8 value;
	#if 1
	while(pipe_read(&receive_led_pipe,&value))
	{
		//USART_SendData(USART1, value);  // for test
		interpret_led_char(value);
		if(state_led == interpret_cmd_start)
		{
			startLEDTime = GetSysTick();
		}
	}
	if((state_led != interpret_cmd_init) && isSysTickTimeout(startLEDTime,1000))
	{
		state_led = interpret_cmd_init;
	}
	#else
	while(pipe_read(&receive_preamp_pipe,&value))
	{		
		uart0_putchar(value);
	}
	#endif
  #endif
}

void Isr_receive_led(u8 value)
{
	pipe_write(&receive_led_pipe, value);
}

void _send_cmd_led(Command_TypeDef *cmdStruct)
{
#if 0
	char buffer[20];
	++send_led_sn;
	cmdStruct->sn = send_led_sn;
	cmdStruct->address = 0;
	
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	//UARTSendStr(LED_UART_PORT, buffer);
	//UARTSendStr(1, buffer);
   #endif
	
}
void led_sendBuffer(u8 *buffer, int size)
{
#if 0

	UARTSendBuffer(LED_UART_PORT,(char *)buffer,size);
	UARTSendBuffer(1,(char *)buffer,size);
  #endif  
}
void send_cmd_to_led(Command_TypeDef *cmdStruct)
{
	_send_cmd_led(cmdStruct);
}

bool send_cmd_to_led_and_wait(Command_TypeDef *cmdStruct)
{
#if 0

	char buffer[20],repeat = 3;
	u32 startTime;

	_send_cmd_led(cmdStruct);

	startTime = GetSysTick();
		

	task_LED();	
	
	// wait ack
	//while((send_led_sn != receive_led_sn) && deviceIsConnet_led&& repeat)
	while((send_led_sn != receive_led_sn) && repeat)
	{
		task_LED();
		if(isSysTickTimeout(startTime,2000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from led = %d,receive_sn: %d\r\n", send_led_sn,receive_led_sn);
			if(repeat)
			{
				printf("repeat: %d\r\n", repeat);
				UARTSendBuffer(LED_UART_PORT, buffer, 9);
				printf("send led:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				task_LED();
				startTime = GetSysTick();
				continue;
			}
			
			#if 1 //判断不能跟主板通信,则中断跟video的通信
			printf("diable communication with LED\r\n");
			//DisableVideoProcessDevice();
			#endif
			deviceIsConnet_led = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_led == FALSE)
	{
		while((send_led_sn != receive_led_sn))
		{
			task_LED();
			if(isSysTickTimeout(startTime,200))
			{
				printf("no main board = %d,receive_sn: %d\r\n", send_led_sn,receive_led_sn);
				deviceIsConnet_led = 0;
				return FALSE;
			}
		}
	}
	#endif
	return TRUE;
}

bool _wait_ack_led(int timeout)
{
#if 0

	u32 startTime;
	startTime = GetSysTick();

	// wait ack
	while((send_led_sn != receive_led_sn))
	{
		task_LED();
		if(isSysTickTimeout(startTime,timeout))
		{
			printf("can't receive led = %d\r\n", send_led_sn);
			deviceIsConnet_led = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_led == FALSE)
	{
		return FALSE;
	}
#endif    
	return TRUE;
	
}

void led_send(int cmd, int dat1, int dat2, int dat3, int dat4)
{
#if 0

	Command_TypeDef cmdStruct;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	send_cmd_to_led(&cmdStruct);
 #endif  
}

bool led_sendAndWait(int cmd, int dat1, int dat2, int dat3, int dat4)
{
	Command_TypeDef cmdStruct;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	return send_cmd_to_led_and_wait(&cmdStruct);
}

int led_sendProgramMcuBuffer(int address, u8 *buffer, int size)
{
    #if 0
	int ack=0;
	led_send(0x9a, 0xa0, 0x00, address, address>>8);
	led_sendBuffer(buffer, size);
	if(address == 0)
	{
		ack = _wait_ack_led(10000);
	}
	else
	{
		ack = _wait_ack_led(3000);
	}
	if(ack)
	{
		if((cmd_from_led.data_3+(cmd_from_led.data_4<<8)) == address)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
    #endif
    return FALSE;
}

int led_goToBoot(void)
{
	return led_sendAndWait(0x9a,0x20,0,0,0);
}

void led_getSoftVersion(void)
{
#if 0

	readFromMCUFlag = 1;
	led_sendAndWait(0x6A, 0x03, 0x00, 0x00, 0x00);	// read soft version
#endif	
}

void led_setSendCardType(BYTE value)
{
#if 1
	if(value > 2)
	{
		return;
	}
	getsendcard()->sendcardtype = value;
	//led_sendAndWait(0x90, 0x00, 0x00, value, 0x00);	
#endif    
}
BYTE led_getSendCardType(void)
{
	//led_sendAndWait(0x90, 0x01, 0x00, 0x00, 0x00);	// read send card type
	//printf("sendcard is %d\r\n",getsendcard()->sendcardtype);
	return getsendcard()->sendcardtype;
}

void led_setSendCardConnectType(BYTE port,BYTE value)
{	
	if((port > 1)||(value > 7))
	{
		return;
	}
	led_sendAndWait(0x90, 0x02, port, value, 0x00);	
}
BYTE led_getSendCardConnectType(BYTE port)
{
	if(port > 1)
	{
		port = 0;
	}
	led_sendAndWait(0x90, 0x03, port, 0x00, 0x00);	
	return getsendcard()->connecttype[CurCard][port];
}

void led_setSendCardBrightness(BYTE value)
{
	led_sendAndWait(0x90, 0x04, 0x00, value, 0x00);	
}
BYTE led_getSendCardBrightness(void)
{
	led_sendAndWait(0x90, 0x05, 0x00, 0x00, 0x00);	
	return getsendcard()->brightness;
}

void led_setSendCardGamma(BYTE value)
{
	led_sendAndWait(0x90, 0x06, 0x00, value, 0x00);	
}
BYTE led_getSendCardGamma(void)
{
	led_sendAndWait(0x90, 0x07, 0x00, 0x00, 0x00);	
	return getsendcard()->gamma;
}

void led_setSendCardPosX(BYTE port, int value)
{
	led_sendAndWait(0x90, 0x08, port, value, (value>>8));	
}
int led_getSendCardPosX(BYTE port)
{
	led_sendAndWait(0x90, 0x09, 0x00, 0x00, 0x00);	
	return getsendcard()->posx[port];
}

void led_setSendCardPosY(BYTE port, int value)
{
	led_sendAndWait(0x90, 0x0a, port, value, (value>>8));	
}
int led_getSendCardPosY(BYTE port)
{
	led_sendAndWait(0x90, 0x0b, port, 0x00, 0x00);	
	return getsendcard()->posy[port];
}

void led_setSendCardSizeX(BYTE port, int value)
{
	led_sendAndWait(0x90, 0x0c, port, value, (value>>8));	
}
int led_getSendCardSizeX(BYTE port)
{
	led_sendAndWait(0x90, 0x0d, port, 0x00, 0x00);	
	return getsendcard()->sizex[port];
}

void led_setSendCardSizeY(BYTE port, int value)
{
	led_sendAndWait(0x90, 0x0e, port, value, (value>>8));	
}
int led_getSendCardSizeY(BYTE port)
{
	led_sendAndWait(0x90, 0x0f, port, 0x00, 0x00);	
	return getsendcard()->sizey[port];
}

void led_setSendCardColorTempRed(BYTE value)
{
	led_sendAndWait(0x90, 0x10, 0x00, value, 0x00);	
}
BYTE led_getSendCardColorTempRed(void)
{
	led_sendAndWait(0x90, 0x11, 0x00, 0x00, 0x00);	
	return getsendcard()->colortemR;
}
void led_setSendCardColorTempGreen(BYTE value)
{
	led_sendAndWait(0x90, 0x10, 0x01, value, 0x00);	
}
BYTE led_getSendCardColorTempGreen(void)
{
	led_sendAndWait(0x90, 0x11, 0x01, 0x00, 0x00);	
	return getsendcard()->colortemG;
}
void led_setSendCardColorTempBlue(BYTE value)
{
	led_sendAndWait(0x90, 0x10, 0x02, value, 0x00);	
}
BYTE led_getSendCardColorTempBlue(void)
{
	led_sendAndWait(0x90, 0x11, 0x02, 0x00, 0x00);	
	return getsendcard()->colortemB;
}
void led_setSendCardColorTemp(BYTE index,BYTE value)
{
	switch(index)
	{
		case 0x00:
			led_setSendCardColorTempRed(value);
			break;
		case 0x01:
			led_setSendCardColorTempGreen(value);
			break;
		case 0x02:
			led_setSendCardColorTempBlue(value);
			break;
		default:
			break;
	}
}
BYTE led_getSendCardColorTemp(BYTE index)
{
	BYTE res = 0;
	switch(index)
	{
		case 0x00:
			res = led_getSendCardColorTempRed();
			break;
		case 0x01:
			res = led_getSendCardColorTempGreen();
			break;
		case 0x02:
			res = led_getSendCardColorTempBlue();
			break;
		default:
			break;
	}

    return res;
}

void led_setSendCardHLine(BYTE port, BYTE value)
{
	led_sendAndWait(0x90, 0x12, port, value, 0x00);	
}
BYTE led_getSendCardHLine(BYTE port)
{
	u8	  choosecard;

	choosecard = getsendcard()->choosecard;

	led_sendAndWait(0x90, 0x13, port, 0x00, 0x00);	
	return getsendcard()->hline[choosecard][port];
}
void led_setSendCardVLine(BYTE port, BYTE value)
{
	led_sendAndWait(0x90, 0x14, port, value, 0x00);	
}
BYTE led_getSendCardVLine(BYTE port)
{
	u8	  choosecard;

	choosecard = getsendcard()->choosecard;

	led_sendAndWait(0x90, 0x15, port, 0x00, 0x00);	
	return getsendcard()->vline[choosecard][port];
}

void led_setSendCardHSize(int value)
{
	led_sendAndWait(0x90, 0x16, 0x00, value, (value>>8));	
}
int led_getSendCardHSize(void)
{
	u8	  choosecard;
	u8	  lxynetwork;

	choosecard = getsendcard()->choosecard;
	lxynetwork = getsendcard()->lxynetwork;	

	led_sendAndWait(0x90, 0x17, 0x00, 0x00, 0x00);	
	return getsendcard()->hsize[choosecard][lxynetwork];
}
void led_setSendCardVSize(int value)
{
	led_sendAndWait(0x90, 0x18, 0x00, value, (value>>8));	
}
int led_getSendCardVSize(void)
{
	u8	  choosecard;
	u8	  lxynetwork;

	choosecard = getsendcard()->choosecard;
	lxynetwork = getsendcard()->lxynetwork;	

	led_sendAndWait(0x90, 0x19, 0x00, 0x00, 0x00);	
	return getsendcard()->vsize[choosecard][lxynetwork];
}

void led_setSendCardChooseCard(BYTE value)
{
	led_sendAndWait(0x90, 0x1a, 0x00, value, 0x00);	
}
BYTE led_getSendCardChooseCard(void)
{
	led_sendAndWait(0x90, 0x1b, 0x00, 0x00, 0x00);	
	return getsendcard()->choosecard;
}


void led_setSendCardReceiveNumber(BYTE value)
{
	led_sendAndWait(0x90, 0x1c, 0x00, value, 0x00);	
}
BYTE led_getSendCardReceiveNumber(void)
{
	led_sendAndWait(0x90, 0x1d, 0x00, 0x00, 0x00);	
	return getsendcard()->receivecardnumber;
}

void led_setSendCardX(int value)
{
	led_sendAndWait(0x90, 0x1e, 0x00, value, (value>>8));	
}
int led_getSendCardX(void)
{
	led_sendAndWait(0x90, 0x1f, 0x00, 0x00, 0x00);	
	return getsendcard()->x;
}
void led_setSendCardY(int value)
{
	led_sendAndWait(0x90, 0x20, 0x00, value, (value>>8));	
}
int led_getSendCardY(void)
{
	led_sendAndWait(0x90, 0x21, 0x00, 0x00, 0x00);	
	return getsendcard()->y;
}

void led_setSendCardXOffset(BYTE port,int value)
{
	led_sendAndWait(0x90, 0x22, port, value, (value>>8));	
}
int led_getSendCardXOffset(BYTE port)
{
	u8	  curCard;

	curCard = getsendcard()->choosecard;

	led_sendAndWait(0x90, 0x23, port, 0x00, 0x00);	
	return getsendcard()->xoffset[curCard][port];
}
void led_setSendCardYOffset(BYTE port,int value)
{
	led_sendAndWait(0x90, 0x24, port, value, (value>>8));	
}
int led_getSendCardYOffset(BYTE port)
{
	u8	  curCard;	

	curCard = getsendcard()->choosecard;

	led_sendAndWait(0x90, 0x25, port, 0x00, 0x00);	
	return getsendcard()->yoffset[curCard][port];
}

void led_setSendCardNetWork_LXY(BYTE value)
{
	led_sendAndWait(0x90, 0x26, 0x00, value, 0x00);	
}
BYTE led_getSendCardNetWork_LXY(void)
{
	led_sendAndWait(0x90, 0x27, 0x00, 0x00, 0x00);	
	return getsendcard()->lxynetwork;
}

void led_setSendCardCascade(BYTE value)
{
	led_sendAndWait(0x90, 0x28, 0x00, value, 0x00);	
}
BYTE led_getSendCardCascade(void)
{
	led_sendAndWait(0x90, 0x29, 0x00, 0x00, 0x00);	
	return getsendcard()->cascade;
}

int led_getSendCardInfo(void)
{
	led_sendAndWait(0x90, 0x2b, 0x00, 0x00, 0x00);	
	return getsendcard()->sendcardinfo;
}
int led_getReceiveCardInfo(void)
{
	led_sendAndWait(0x90, 0x2d, 0x00, 0x00, 0x00);	
	return getsendcard()->receivecardinfo;
}

void led_setSendCardFactory(void)
{
	led_sendAndWait(0x90, 0x30, 0x01, 0x00, 0x00);	
}

void led_setSendCardConnectParam(void)
{
	led_sendAndWait(0x90, 0x32, 0x01, 0x00, 0x00);	
}

void led_setSendCardReceiveCardParam(void)
{
	led_sendAndWait(0x90, 0x34, 0x01, 0x00, 0x00);	
}

void led_setSendCardColorTempParam(void)
{
	led_sendAndWait(0x90, 0x36, 0x01, 0x00, 0x00);	
}

void led_setSendCardParam(void)
{
	led_sendAndWait(0x90, 0x38, 0x01, 0x00, 0x00);	
}

#if 0
void led_setSendCardNetWorkChoose(BYTE value)
{
	led_sendAndWait(0x90, 0x3a, 0x00, value, 0x00);	
}
BYTE led_getSendCardNetWorkChoose(void)
{
	led_sendAndWait(0x90, 0x3b, 0x00, 0x00, 0x00);
	return getsendcard()->lxynetwork;
}
#endif

void led_setSendCardOutputFormat(BYTE value)
{
	led_sendAndWait(0x90, 0x3c, 0x00, value, 0x00);	
}
BYTE led_getSendCardOutputFormat(void)
{	
	led_sendAndWait(0x90, 0x3d, 0x00, 0x00, 0x00);	
	return getsendcard()->outputformat;
}



void refleshLEDParam(void) //刷新发送卡控制参数
{
	char i;
	led_getSendCardType();
	led_getSendCardBrightness();
	led_getSendCardGamma();
	led_getSendCardColorTempRed();
	led_getSendCardColorTempGreen();
	led_getSendCardColorTempBlue();
	led_getSendCardHSize();
	led_getSendCardVSize();
	led_getSendCardChooseCard();
	led_getSendCardReceiveNumber();
	led_getSendCardX();
	led_getSendCardY();
	led_getSendCardCascade();
	led_getSendCardInfo();
	led_getSendCardNetWork_LXY();
	led_getReceiveCardInfo();

	for(i = 0; i < 2; i++)
	{
		led_getSendCardConnectType(i);
		led_getSendCardPosX(i);
		led_getSendCardPosY(i);
		led_getSendCardSizeX(i);
		led_getSendCardSizeY(i);
		led_getSendCardHLine(i);
		led_getSendCardVLine(i);
		led_getSendCardXOffset(i);
		led_getSendCardYOffset(i);
	}
	
	led_getSendCardColorTemp(0);
	led_getSendCardColorTemp(1);
	led_getSendCardColorTemp(2);
	
	led_getSendCardOutputFormat();
	
}

void setLEDSendCardType(BYTE value)
{
	if(value > 2)
	{
		return;
	}
	getsendcard()->sendcardtype = value;
	led_setSendCardType(value);
	savesendcard();
}


u8 getLEDSendCardType(void)
{
	bool res;
	u8 i;
	for(i = 0;i < 3;i++)
	{
		res = readsendcardlxy_check();
		if(res == TRUE)
		{
			setLEDSendCardType(_SENDCARD_LXY);
			return _SENDCARD_LXY;
		}
	}
	if(res == FALSE)
	{
		for(i = 0;i < 3;i++)
		{
			res == readsendcard();
			if(res == TRUE)
			{
				setLEDSendCardType(_SENDCARD_COLORLIGHT);
				return _SENDCARD_COLORLIGHT;
			}
		}
	}
	return -1;
	//return led_getSendCardType();
}

void setLEDSendCardChooseCard(BYTE value)
{
	//if(value > 1)
	//{
	//	return;
	//}
	getsendcard()->choosecard = value;
	//led_setSendCardChooseCard(value);
	savesendcard();
}
BYTE getLEDSendCardChooseCard(void)
{
	return led_getSendCardChooseCard();
}

void setLedSendCardStandOuputFormat(BYTE format)
{
	BYTE value = 0xFF;

	//640x480 --0
	//800x600 --1
	//1024x768 -- 2
	//1280x768 --3
	//1280x800 --4
	//1280x1024--5
	//1366x768--6
	//1440x900--7
	//1440x1080--8
	//1600x576--9
	//1600x900--10
	//1600x1000--11
	//1600x1200--12
	//1680x1050--13
	//1728x1296--14
	//1920x1080--15
	//1920x1200--16
	//2048x1152--17
	//2048x1280--18
	//2560x1440--19
	//2560x1600--20


	// this option is copy from menuData.c->outFormatList[]
	switch(format)
	{
		case MD_800x600_60:
			value = 1;//800x600 --1
			break;			
		case MD_1024x768_60:
			value = 2;//1024x768 -- 2
			break;			
    #if ENABLE_USER_DEFINE
		case MD_1024x768_59_94:
    #endif
		case MD_1024x768_75:
			value = 2;//1024x768 -- 2
			break;				
		case MD_1280x720_60:
		case MD_1280x720_50:
			value = 3;	//1280x768 --3
			break;				
		//MD_1280x720_59_94,
		case MD_1280x768_60:
			value = 3;	//1280x768 --3
			break;			
		case MD_1280x800_60:
			value = 4;	//1280x800 --4
			break;			
		case MD_1280x1024_60:
			value = 5;	//1280x1024--5
			break;			
		case MD_1360x768_60:
			value = 6;	//1366x768--6
			break;			
		case MD_1366x768_60:
			value = 6;	//1366x768--6
			break;			
		case MD_1400x1050_60:
			value = 8;//1440x1080--8
			break;			
//		MD_1440x800_60_r,	//added by julia 
		case MD_1440x900_60:
			value = 7;		//1440x900--7
			break;			
		case MD_1600x1200_60:
			value = 12;//1600x1200--12
			break;			
		case MD_1680x1050_60:
			value = 13;//1680x1050--13
			break;			
		case MD_1920x1080_60:
			value = 15;//1920x1080--15
			break;			
		case MD_1920x1080_50:
			value = 15;//1920x1080--15
			break;			
//		MD_1920x1120_60_r,	 //added by juila 
    //MD_1920x1080_59_94,
		case MD_1920x1200_60:
			value = 16;//1920x1200--16
			break;			
		case MD_2048x1152_60:
			value = 17;//2048x1152--17
			break;			
		case MD_2560x812_60:
			value = 17;//2048x1152--17
			break;			
		case MD_2560x816_60:
			value = 17;//2048x1152--17
			break;
		default:
			value = 0xFF;
			break;
	}

	if(value == 0xFF)
	{
		return ;
	}

	menuSetBusy(3,6);
	
	getsendcard()->outputformat = value;
	savesendcard();
	//if(getsendcard()->choosecard==1)//20150917
		sendcardlxy_changecard_command(0x00);//20150917
	//SysDelay1ms(50);
	sendcardlxy_write_edid(value);
	sendcardlxy_save_edid(value);
	//sendcardlxy_write_edid_Test(value);//20150917
	//if(getsendcard()->choosecard==1)//20150917
		sendcardlxy_changecard_command(0x00);//20150917

		
		//if(getsendcard()->choosecard==1)//20150917
		sendcardlxy_changecard_command(0x01);//20150917
		//SysDelay1ms(50);
	sendcardlxy_write_edid(value);
	sendcardlxy_save_edid(value);
		//sendcardlxy_write_edid_Test(value);//20150917
	//if(getsendcard()->choosecard==1)//20150917
		sendcardlxy_changecard_command(0x00);//20150917

	menuDisableBusy();  //added by @cc20140717

}

void setLEDSendCardOutputFormat(BYTE value)
{
	getsendcard()->outputformat = value;
	led_setSendCardOutputFormat(value);
	savesendcard();
}
BYTE getLEDSendCardOutputFormat(void)
{
	return led_getSendCardOutputFormat();
}

void setLEDSendCardPosX(BYTE port,int value)
{
	if(port > 1)
	{	
		return ;
	}
	getsendcard()->posx[port] = value;
	//led_setSendCardPosX(port,value);
	savesendcard();
}
int getLEDSendCardPosX(BYTE port)
{
	return led_getSendCardPosX(port);
}
void setLEDSendCardPosY(BYTE port,int value)
{
	if(port > 1)
	{	
		return ;
	}
	getsendcard()->posy[port] = value;
	//led_setSendCardPosY(port,value);
	savesendcard();
}
int getLEDSendCardPosY(BYTE port)
{
	return led_getSendCardPosY(port);
}

void setLEDSendCardSizeX(BYTE port,int value)
{
	getsendcard()->sizex[port] = value;
	led_setSendCardSizeX(port,value);
	savesendcard();
}

int getLEDSendCardSizeX(BYTE port)
{
	return led_getSendCardSizeX(port);
}

void setLEDSendCardSizeY(BYTE port,int value)
{
	getsendcard()->sizex[port] = value;
	led_setSendCardSizeY(port,value);
	savesendcard();
}

int getLEDSendCardSizeY(BYTE port)
{
	return led_getSendCardSizeY(port);
}


void setLEDSendCardParam(void)
{
	led_setSendCardParam();
}
void resetLEDSendCardParam(void)
{
	getsendcard()->posx[CurPort] = 0;
	getsendcard()->posy[CurPort] = 0;
	getsendcard()->brightness = 255;
	led_setSendCardFactory();
}



void setLEDNetworkChoose(BYTE value)
{
	if(value > 1)
	{
		return;
	}
	getsendcard()->lxynetwork = value;	
	led_setSendCardNetWork_LXY(value);
	savesendcard();
}

void setSendCardPortSelNoSave(BYTE value)
{
    //FPGA_Set_TestGrid_index(value);	
	if(getsendcard()->virtuaPort == value)
	{
		return;	
	}
	
	if(value > 3)
	{
		return;
	}
	
	getsendcard()->virtuaPort = value;
	
	if(value == 0)
	{
		getsendcard()->choosecard = 0;
		getsendcard()->lxynetwork = 1;
	}
	else if(value == 1)
	{
		getsendcard()->choosecard = 0;
		getsendcard()->lxynetwork = 0;
	}
	else if (value == 2)
	{
		getsendcard()->choosecard = 1;
		getsendcard()->lxynetwork = 1;
	}
	else if(value == 3)
	{
		getsendcard()->choosecard = 1;
		getsendcard()->lxynetwork = 0;
	}
#ifdef NO_SEND_CARD_DEBUG
	led_setSendCardNetWork_LXY(value);
#endif
}

void setSendCardPortSel(BYTE value)
{
	//FPGA_Set_TestGrid_index(value);	

	if(getsendcard()->virtuaPort == value)
	{
		return;	
	}

	if(value > 3)
	{
		return;
	}

	getsendcard()->virtuaPort = value;
	
	if(value == 0)  // port 1
	{
		getsendcard()->choosecard = 0;
		getsendcard()->lxynetwork = 1;
	}
	else if(value == 1) // port  2
	{
		getsendcard()->choosecard = 0;
		getsendcard()->lxynetwork = 0;
	}
	else if (value == 2)
	{
		getsendcard()->choosecard = 1;
		getsendcard()->lxynetwork = 1;
	}
	else if(value == 3)
	{
		getsendcard()->choosecard = 1;
		getsendcard()->lxynetwork = 0;
	}
#ifdef NO_SEND_CARD_DEBUG
	led_setSendCardNetWork_LXY(value);
#endif
	savesendcard();
}


void setLEDSendCardHLine(BYTE port,BYTE value)
{
	u8	  choosecard;

	choosecard = getsendcard()->choosecard;

	getsendcard()->hline[choosecard][port] = value;
	//led_setSendCardHLine(port,value);
	savesendcard();
}
void setLEDSendCardVLine(BYTE port,BYTE value)
{
	u8	  choosecard;

	choosecard = getsendcard()->choosecard;

	getsendcard()->vline[choosecard][port] = value;
	//led_setSendCardVLine(port,value);
	savesendcard();
}
void setLEDSendCardHSize(int value)
{
	u8	  choosecard;
	u8	  lxynetwork;

	choosecard = getsendcard()->choosecard;
	lxynetwork = getsendcard()->lxynetwork;	

	getsendcard()->hsize[choosecard][lxynetwork] = value;
	//led_setSendCardHSize(value);
	savesendcard();
}
void setLEDSendCardVSize(int value)
{
	u8	  choosecard;
	u8	  lxynetwork;

	choosecard = getsendcard()->choosecard;
	lxynetwork = getsendcard()->lxynetwork;	
	

	getsendcard()->vsize[choosecard][lxynetwork] = value;
	//led_setSendCardVSize(value);
	savesendcard();
}
void setLEDSendCardXOffset(BYTE port,int value)
{
	u8	  choosecard;

	choosecard = getsendcard()->choosecard;

	getsendcard()->xoffset[choosecard][port] = value;	
	led_setSendCardXOffset(port,value);
	savesendcard();
}
void setLEDSendCardYOffset(BYTE port,int value)
{
	u8	  choosecard;

	choosecard = getsendcard()->choosecard;
	getsendcard()->yoffset[choosecard][port] = value;	
	led_setSendCardYOffset(port,value);
	savesendcard();
}
void setLEDSendCardConnectType(BYTE port,BYTE value)
{
	u8	  choosecard;
	
	choosecard = getsendcard()->choosecard;

	getsendcard()->connecttype[choosecard][port] = value;

	//led_setSendCardConnectType(port,value);

	savesendcard();
}

int getLedConnectIndex(	BYTE  connecttype,int hline,int vline,int x,int y)
{
    int i,j;
    int  index;
    
    switch(connecttype)
    {
        case 0:
            if(y%2 == 0)
            {
                index = y * hline + x;
            }
            else
            {
                index = y * hline + (hline - 1 - x);
            }
            break;
            
        case 1:
            if(y%2 == 0)
            {
               index = y * hline + (hline - 1 - x);
            }
            else
            {
                index = y * hline + x;
            }
            break;
            
        case 2:
            if(vline%2 == 0)
            {
                if(y%2 == 0)
                {
                    index = (vline - 1 - y)*hline + hline - 1 - x;
                }
                else
                {
                   index = (vline-1-y)*hline + x;
                } 

            }
            else
            {
                if(y%2 == 0)
                {
                   index = (vline-1-y)*hline + x;
                }
                else
                {
                    index = (vline - 1 - y)*hline + hline - 1 - x;
                }
            }
            break;
            
        case 3:
            if(vline%2 == 0)
            {
                if(y%2 == 0)
                {
                   index = (vline-1-y)*hline + x;
                }
                else
                {
                   index = (vline - 1 - y)*hline + hline - 1 - x;
                }
            }
            else
            {
                if(y%2 == 0)
                {
                   index = (vline - 1 - y)*hline + hline - 1 - x;
                }
                else
                {
                   index = (vline - 1 - y)*hline + x;
                }
            }            
            break;
            
        case 4:
            if(x%2 == 0)
            {
               index = x * vline + y;
            }
            else
            {
               index = x * vline + (vline - 1 - y);
            }
            break;
        case 5:
            if(x%2 == 0)
            {
                index = x * vline + (vline - 1 - y);
            }
            else
            {
                index = x * vline + y;    
            }
            break;
        case 6:    
            if(hline%2 == 0)
            {
                if(x%2 == 0)
                {
                   index = (hline - 1 - x) * vline + (vline - 1 - y);
                }
                else
                {
                   index = (hline - 1 - x) * vline + y;
                }
            }
            else
            {
                if(x%2 == 0)
                {
                    index = (hline - 1 - x) * vline + y;
                }
                else
                {
                   index = (hline - 1 - x) * vline + (vline - 1 - y);
                }            
            }
            break;
        case 7:
            if(hline%2 == 0)
            {
                if(x%2 == 0)
                {
                    index = (hline - 1 - x) * vline + y;
                }
                else
                {
                   index = (hline - 1 - x) * vline + (vline - 1 - y);
                }              
            }
            else
            {
                if(x%2 == 0)
                {
                   index = (hline - 1 - x) * vline + (vline - 1 - y);
                }
                else
                {
                   index = (hline - 1 - x) * vline + y;  
                }         
            }
            break;

        default:
            break;
    }

    return index;
}

void getLEDSendCardCardSize(u8 card,int * width,int * height)
{
    SENDCARD_List_Typedef * send_Card;
    int width0,height0;
    int width1,height1;
    int width2,height2; 
    int width3,height3; 

    if((width == NULL) || (height == NULL))
    {
        return;
    }
    
    send_Card = getsendcard();

    width0 = send_Card->hline[card][1] * send_Card->hsize[card][1];
    height0 = send_Card->vline[card][1] * send_Card->vsize[card][1];
    width1 = send_Card->hline[card][0] * send_Card->hsize[card][0];
    height1 = send_Card->vline[card][0] * send_Card->vsize[card][0];  

    if(send_Card->xoffset[card][0] != 0)
    {
        *width = width0 + width1;
    }
    else
    {
        *width = width0;        
    }

    if(send_Card->yoffset[card][0] != 0)
    {
        *height = height0 + height1;
    }
    else
    {
        *height = height0;        
    }    
}


void getLEDSendCardSize(int * width,int * height)
{
	SENDCARD_List_Typedef * send_Card;
    int width0,height0;
    int width1,height1;
    int width2,height2; 
    int width3,height3; 

    if((width == NULL) || (height == NULL))
    {
        return;
    }
    
	send_Card = getsendcard();

    width0 = send_Card->hline[0][1] * send_Card->hsize[0][1];
    height0 = send_Card->vline[0][1] * send_Card->vsize[0][1];
    width1 = send_Card->hline[0][0] * send_Card->hsize[0][0];
    height1 = send_Card->vline[0][0] * send_Card->vsize[0][0];  

    if(send_Card->xoffset[0][0] != 0)
    {
        *width = width0 + width1;
    }
    else
    {
        *width = width0;        
    }

    if(send_Card->yoffset[0][0] != 0)
    {
        *height = height0 + height1;
    }
    else
    {
        *height = height0;        
    }    

    /* 需要加上第二张卡的位置*/
    width2 = send_Card->hline[1][1] * send_Card->hsize[1][1];
    height2 = send_Card->vline[1][1] * send_Card->vsize[1][1];
    width3 = send_Card->hline[1][0] * send_Card->hsize[1][0];
    height3 = send_Card->vline[1][0] * send_Card->vsize[1][0];  

    if((getsendcard()->posx[1] != 0))
    { 
        if(send_Card->xoffset[1][0] != 0)
        {
            *width += width2 + width3;
        }
        else
        {
            *width += width2;        
        }
    }
    else
    {
        if(send_Card->xoffset[1][0] != 0)
        {
            width2 = width2 + width3;
        }

        if(*width < width2)   
        {
            *width = width2;
        }       
    }

    if((getsendcard()->posy[1] != 0))
    {

        if(send_Card->yoffset[1][0] != 0)
        {
            *height += height2 + height3;
        }
        else
        {
            *height += height2;        
        }         
    } 
    else
    {
        if(send_Card->yoffset[1][0] != 0)
        {
            height2 = height2 + height3;
        }

        if(*height < height2)   
        {
            *height = height2;
        }    
    }
}


extern u8 SendTimeoutFlg;
u8 choosecard_old = 0xFF;
u16 uport_width;
u16 uport_height;
u16 dport_width;
u16 dport_height;

u8 setLEDSendCardAllAutoConnect(u8 display,int value)
{
	u8 i;

	setLEDSendCardAutoConnect(TRUE,0,0,0);
	setLEDSendCardAutoConnect(TRUE,0,0,1);
	setLEDSendCardAutoConnect(TRUE,0,1,0);
	setLEDSendCardAutoConnect(TRUE,0,1,1);
    return TRUE;
}


u8 setLEDSendCardAutoConnect(u8 display,int value,u8 card,u8 port)
{
    SENDCARD_List_Typedef * send_Card;
    BYTE value1;
    BYTE  vline,hline;
    int   xoffset,yoffset;
    int   vsize, hsize;
    int i,j;
    BYTE  connecttype;
    u8 flagtime = 5;
    bool timeoutflg1=FALSE,timeoutflg2=FALSE;   //added by @cc20140716
    int width,height;
    int pixelMax;
    u8 cardn = 0;
  
    send_Card = getsendcard();
	
    height = send_Card->wallvline[card][port];	//端口带像素总高
    width = send_Card->wallhline[card][port];   //端口带像素总宽  

    vsize = send_Card->wallVSize;				//箱体高
	hsize =  send_Card->wallHSize;				//箱体宽

    if(vsize == 0x0000 || hsize == 0x0000)
    {
        return FALSE; 
    }
        
    vline = height / vsize;
    hline = width / hsize;

    switch(send_Card->wallConnecttype)
    {
        case 0:
            if(CurCard == 0)
            {
                if(CurPort == 0)
                {
                     xoffset = 0x0000;
                     yoffset = 0x0000;
                }
                else
                {
                     xoffset = width;
                     yoffset = 0x0000;                
                }
            }
            else
            {
                if(CurPort == 0)
                {
                     xoffset = 0x0000;
                     yoffset = height;                
                }
                else
                {
                     xoffset = width;
                     yoffset = height;                   
                }                
            }
            break;


        case 1:
            if(CurCard == 0)
            {
                if(CurPort == 0)
                {
                     xoffset = 0x0000;
                     yoffset = 0x0000;
                }
                else
                {
                     xoffset = width;
                     yoffset = 0x0000;                
                }
            }
            else
            {
                if(CurPort == 0)
                {
                     xoffset = 2 * width;
                     yoffset = 0x0000;                
                }
                else
                {
                     xoffset = 3 * width;
                     yoffset = 0x0000;                   
                }                
            }              
          
            break;
        case 2:
            if(CurCard == 0)
            {
                if(CurPort == 0)
                {
                     xoffset = 0x0000;
                     yoffset = 0x0000;
                }
                else
                {
                     xoffset = 0x0000;
                     yoffset = height;                
                }
            }
            else
            {
                if(CurPort == 0)
                {
                     xoffset = 0x0000;
                     yoffset = 2 * height;                
                }
                else
                {
                     xoffset = 0x0000;
                     yoffset = 3 * height;                   
                }                
            }              
            break;

        default:
            break;
    }  
    
    // 进行参数的check
    pixelMax = width * height;
    
 #if 1       // Cancel In V1.34   
    if(GetSendCardUsbPlusState() == 1)
    {
        LCD_WriteStr(3,0, langGetStr(deviceInRemoteControlStr));

        SysDelay1ms(2000);  
        
        return FALSE;        
    }   
 #endif
    
    if(display == TRUE)
    {
        if(height > 1024)
        {
            LCD_WriteStr(3,1, "The Height Out of Support");

            SysDelay1ms(2000);  

            return FALSE;
        }
        else if(width > (655360 / height))
        {
            LCD_WriteStr(3,1, "The Width Out of Support");

            SysDelay1ms(2000);  

            return FALSE;
        }
        else if(pixelMax > 655360)
        {
            LCD_WriteStr(3,1, "The Pixel  Out of Support");

            SysDelay1ms(2000);  

            return FALSE;
        }


        delayms(5);   //added by @cc20140717
        //LCD_Clear();
        //LCD_WriteLine3("Plese wait ");
        menuSetBusy(3,6);
    }
    
    if((hsize == 1) &&  (vsize == 1))
    {
        if(display == TRUE)
        {
            delayms(50);
            menuDisableBusy(); //added by @cc20140717

            LCD_WriteLine4("    Save To Port Success");
            SetSetWidthEnable(FALSE);
            
            SysDelay1ms(2000);
        }
        return TRUE;
    }
    

    getLEDSendCardCardSize(CurCard,&width,&height);
    dport_width = width;
    if(dport_width > 0)
    {
        dport_width -= 1;
    }
    dport_height = height;
    if(dport_height > 0)
    {
        dport_height -= 1;
    }
    if(dport_height > 1023)
    {
        dport_height = 1023;
    }
    uport_width = dport_width;
    uport_height = dport_height; 
    
    if(send_Card->sendcardtype == _SENDCARD_LXY) //linsn
    //if(0)
    {  
        flagtime = 5;
        //if(send_Card->choosecard == 1)
        //  sendcardlxy_changecard_command(0x01);
        //else
        //  sendcardlxy_changecard_command(0x00);
        sendcardlxy_changecard_command(CurCard);

        //sendcardlxy_write_recvie_card_command1();
        connecttype = send_Card->connecttype[CurCard][CurPort];
        for(i = 0;i < vline;i++)
        {
            for(j = 0;j < hline;j++)
            {
                if(flagtime==5)
                {
                    flagtime=0;

                    sendcardlxy_write_en_command_pre(0);
                    sendcardlxy_write_command_pre(0,0,0,0,0);
                    sendcardlxy_write_un_command_pre(0);
                }
                cardn = getLedConnectIndex(connecttype,hline,vline,j,i);
               // printf("%d ",cardn);
                timeoutflg1 = sendcardlxy_write_en_command(cardn);
                timeoutflg2 = sendcardlxy_write_command(cardn,xoffset +j * hsize,yoffset + i * vsize,hsize - 1,vsize - 1);
                //timeoutflg2 = sendcardlxy_write_command(cardn,j * hsize,i * vsize,hsize - 1,vsize - 1);
                if((timeoutflg1 == FALSE)||(timeoutflg2== FALSE))
                {
                    SendTimeoutFlg = TRUE;

                    if(display == TRUE)
                    {
                        menuDisableBusy();  //added by @cc20140717
                        LCD_WriteLine4("    Save To Port  Fail");

                        SysDelay1ms(2000);
                    }

                    sendcardlxy_changecard_command(0x00);
                    return FALSE;
                }
    
                SendCardLxyWriteUnknowCmd(cardn,connecttype);
                sendcardlxy_write_un_command(cardn);

                sendcardlxy_save_en_command(cardn);
                sendcardlxy_save_clear_command(cardn);
 
                sendcardlxy_save_command(cardn,xoffset + j*hsize,yoffset + i*vsize,hsize - 1,vsize - 1);//20150918
                
                sendcardlxy_save_un_command(cardn);
            }
            //printf("\r\n");
        }
        sendcardlxy_changecard_command(0x00);   
  
    }
	if(send_Card->sendcardtype == _SENDCARD_COLORLIGHT)   // ColorLight
    //else 
    {
        connecttype = send_Card->connecttype[CurCard][CurPort];
        for(i = 0;i < vline;i++)
        {
            for(j = 0;j < hline;j++)
            {
                cardn = getLedConnectIndex(connecttype,hline,vline,j,i);
                    
            #if 1   //@cc20141227
               timeoutflg1 =Sendcard_area_Command(cardn,   \
                                xoffset +j * hsize,yoffset + i * vsize,   \
                            xoffset + (j + 1) * hsize,yoffset + (i + 1) * vsize );
            #else
               timeoutflg1 =Sendcard_area_Command(i*getsendcard()->hline[curport]+j,j*getsendcard()->vsize,i*getsendcard()->hsize,j*getsendcard()->vsize+getsendcard()->vsize,i*getsendcard()->hsize+getsendcard()->hsize);
            #endif
               if(timeoutflg1 == FALSE)
               {
                   SendTimeoutFlg = TRUE;

                    if(display == TRUE)
                    {
                        menuDisableBusy();  //added by @cc20140717
                        LCD_WriteLine4("    Save To Port  Fail");

                        SysDelay1ms(2000);
                    }                
                    return FALSE;  //send data Fail;
               }
           }

        }
        Sendcard_Command_jiazai();
    
        
        if(CurCard != choosecard_old)
        {           
            erase_flash(0x50000);

            // ------------------------------------这里需要计算以下这几个参数(luo 2016.12.20)------------------------------------------------------------------------
            
            timeoutflg1 = Sendcard_position_Command(0x50000,getsendcard()->posx[0],getsendcard()->posy[0],getsendcard()->sizex[0],getsendcard()->sizey[0]);
            timeoutflg2 = Sendcard_position_Command1(0x50400,getsendcard()->posx[1],getsendcard()->posy[1],getsendcard()->sizex[1],getsendcard()->sizey[1]);
            Sendcard_position_Command(0x50500,getsendcard()->posx[0],getsendcard()->posy[0],getsendcard()->sizex[0],getsendcard()->sizey[0]);
            Sendcard_position_Command(0x50600,getsendcard()->posx[0],getsendcard()->posy[0],getsendcard()->sizex[0],getsendcard()->sizey[0]);
            Sendcard_jizai_Command(0x60000); //0x60000 ->0x50000 @cc20141227
           
           if((timeoutflg1 == FALSE)||(timeoutflg2 == FALSE))
           {
                if(display == TRUE)
                {
                    menuDisableBusy();  //added by @cc20140717
                    LCD_WriteLine4("    Save To Port  Fail");

                    SysDelay1ms(2000);
                }           
                SendTimeoutFlg = TRUE;

                 return FALSE;
            }  
        }
    }

    if(display == TRUE)
    {
        delayms(50);
        menuDisableBusy(); //added by @cc20140717

        LCD_WriteLine4("    Save To Port Success");
        
        SysDelay1ms(2000);
    }

    SetSetWidthEnable(FALSE);
        
    return TRUE;
}




u8 setLEDSendCardConnect(u8 display,int value)
{
	SENDCARD_List_Typedef * send_Card;
	BYTE value1;
	BYTE  vline,hline;
	int   xoffset,yoffset;
	int   vsize, hsize;
	int i,j;
	BYTE  connecttype;
	u8 flagtime = 5;
	bool timeoutflg1=FALSE,timeoutflg2=FALSE;	//added by @cc20140716
	int width,height;
	int pixelMax;
	u8 cardn = 0;

	send_Card = getsendcard();
	
	vline = send_Card->vline[CurCard][CurPort];
	hline = send_Card->hline[CurCard][CurPort];	
	xoffset = send_Card->xoffset[CurCard][CurPort];
    xoffset %= 32;
	//xoffset += 1;
	yoffset = send_Card->yoffset[CurCard][CurPort];
    //yoffset %= 32;
    yoffset = 0;		// 20160928 in xiamen	
	hsize  = send_Card->hsize[CurCard][CurPort];
	vsize  = send_Card->vsize[CurCard][CurPort];
	cardn = vline*hline;

	// 进行参数的check
	height = vline *  vsize;
	width = hline * hsize;
	pixelMax = width * height;

 #if 1       // Cancel In V1.34   
    if(GetSendCardUsbPlusState() == 1)
    {
		LCD_WriteStr(3,0, langGetStr(deviceInRemoteControlStr));

		SysDelay1ms(2000);	
		
		return FALSE;        
	}	
 #endif

    if(display == TRUE)
    {
    	if(height > 1024)
    	{
    		LCD_WriteStr(3,1, "The Height Out of Support");

    		SysDelay1ms(2000);	
    		
    		return FALSE;
    	}
    	else if(width > (655360 / height))
    	{
    		LCD_WriteStr(3,1, "The Width Out of Support");

    		SysDelay1ms(2000);	
    		
    		return FALSE;
    	}
    	else if(pixelMax >= 655360)
    	{
    		LCD_WriteStr(3,1, "The Pixel  Out of Support");

    		SysDelay1ms(2000);	
    		
    		return FALSE;
    	}
      

    	delayms(5);	  //added by @cc20140717
    	//LCD_Clear();
    	//LCD_WriteLine3("Plese wait ");
    	menuSetBusy(3,6);
      }

    if((hsize == 1) &&  (vsize == 1))
    {
    	if(display == TRUE)
    	{
	    	delayms(50);
	    	menuDisableBusy(); //added by @cc20140717

	    	LCD_WriteLine4("    Save To Port Success");
			SetSetWidthEnable(FALSE);
	    	
	    	SysDelay1ms(2000);
    	}
		return TRUE;
    }

	//dport_width = send_Card->hline[CurCard][0] * send_Card->hsize[CurCard][0];
	//dport_height = send_Card->vline[CurCard][0]* send_Card->vsize[CurCard][0];
	//uport_width = send_Card->hline[CurCard][1] * send_Card->hsize[CurCard][1];
	//uport_height = send_Card->vline[CurCard][1]* send_Card->vsize[CurCard][1];    

	//dport_width = send_Card->hline[CurCard][0] * send_Card->hsize[CurCard][0] + send_Card->hline[CurCard][1] * send_Card->hsize[CurCard][1];
	getLEDSendCardCardSize(CurCard,&width,&height);
	dport_width = width;
    if(dport_width > 0)
    {
        dport_width -= 1;
    }
	//dport_height = send_Card->vline[CurCard][0]* send_Card->vsize[CurCard][0] + send_Card->vline[CurCard][1]* send_Card->vsize[CurCard][1];
	dport_height = height;
	if(dport_height > 0)
    {
        dport_height -= 1;
    }
    if(dport_height > 1023)
    {
        dport_height = 1023;
    }
	uport_width = dport_width;
	uport_height = dport_height; 

    if(send_Card->sendcardtype == _SENDCARD_LXY) //linsn
    //if(0)
    {  
        flagtime = 5;
        //if(send_Card->choosecard == 1)
        //	sendcardlxy_changecard_command(0x01);
        //else
        //	sendcardlxy_changecard_command(0x00);
        sendcardlxy_changecard_command(CurCard);

		//sendcardlxy_write_recvie_card_command1();
        connecttype = send_Card->connecttype[CurCard][CurPort];
        for(i = 0;i < vline;i++)
        {
            for(j = 0;j < hline;j++)
            {
                if(flagtime==5)
                {
                    flagtime=0;

                    sendcardlxy_write_en_command_pre(0);
                    sendcardlxy_write_command_pre(0,0,0,0,0);
                    sendcardlxy_write_un_command_pre(0);
                }
                cardn = getLedConnectIndex(connecttype,hline,vline,j,i);
               // printf("%d ",cardn);
                timeoutflg1 = sendcardlxy_write_en_command(cardn);
                timeoutflg2 = sendcardlxy_write_command(cardn,xoffset +j * hsize,yoffset + i * vsize,hsize - 1,vsize - 1);
                //timeoutflg2 = sendcardlxy_write_command(cardn,j * hsize,i * vsize,hsize - 1,vsize - 1);
                if((timeoutflg1 == FALSE)||(timeoutflg2== FALSE))
                {
                    SendTimeoutFlg = TRUE;

                    if(display == TRUE)
                    {
                        menuDisableBusy();  //added by @cc20140717
                        LCD_WriteLine4("    Save To Port  Fail");

                        SysDelay1ms(2000);
                    }

                    sendcardlxy_changecard_command(0x00);

                    return FALSE;
                }

                SendCardLxyWriteUnknowCmd(cardn,connecttype);
                sendcardlxy_write_un_command(cardn);

                sendcardlxy_save_en_command(cardn);
                sendcardlxy_save_clear_command(cardn);
                //20150918 sendcardlxy_save_command(i*hline+j,j*vsize,i*hsize,vsize-1,hsize-1);
                //sendcardlxy_save_command(cardn,xoffset+j*hsize,yoffset+i*vsize,hsize - 1,vsize - 1);//20150918
                //sendcardlxy_save_command(cardn,send_Card->xoffset[CurCard][CurPort] + j*hsize,send_Card->xoffset[CurCard][CurPort] + i*vsize,hsize - 1,vsize - 1);//20150918
                sendcardlxy_save_command(cardn,send_Card->xoffset[CurCard][CurPort] + j*hsize,send_Card->xoffset[CurCard][CurPort] + i*vsize,hsize - 1,vsize - 1);//20150918
                //sendcardlxy_save_command(cardn,xoffset + j*hsize,yoffset + i*vsize,hsize - 1,vsize - 1);//20150918
                //sendcardlxy_save_command(cardn,xoffset + j*hsize,yoffset + i*vsize,hsize - 1,vsize - 1);//20150918
                //sendcardlxy_save_command(cardn,j*hsize,i*vsize,hsize - 1,vsize - 1);//20150918
                
                sendcardlxy_save_un_command(cardn);
            }
            //printf("\r\n");
        }
        sendcardlxy_changecard_command(0x00);	

		//printf("\r\n");
		SysDelay1ms(1000);
		if(CurCard != choosecard_old)
		//if(0)
		{
			sendcardlxy_changecard_command(CurCard);

			//info ("getsendcard()->choosecard :%d\r\n",getsendcard()->choosecard);
			readsendcardlxy_check();
			readsendcardlxy();
			//timeoutflg1 = sendcardlxy_xy_command(getsendcard()->posx[CurCard],getsendcard()->posy[CurCard],
			//        send_Card->xoffset[CurCard][1],send_Card->yoffset[CurCard][1],
			//         send_Card->xoffset[CurCard][0],send_Card->yoffset[CurCard][0]);
			timeoutflg1 = sendcardlxy_xy_command(getsendcard()->posx[CurCard],getsendcard()->posy[CurCard],
					send_Card->xoffset[CurCard][1],send_Card->yoffset[CurCard][1],
					 send_Card->xoffset[CurCard][0],send_Card->yoffset[CurCard][0]);    
			//timeoutflg1 = sendcardlxy_xy_command(getsendcard()->posx[CurCard],getsendcard()->posy[CurCard],xoffset,yoffset);

			//sendcardlxy_xy_command1();
			timeoutflg2 = sendcardlxy_command_xy_save(28,getsendcard()->posx[CurCard],getsendcard()->posy[CurCard]);
			if((timeoutflg1 == FALSE)||(timeoutflg2 == FALSE))
			{
				SendTimeoutFlg = TRUE;
				if(display == TRUE)
				{
					menuDisableBusy();  //added by @cc20140717
					LCD_WriteLine4("    Save To Port  Fail");

					SysDelay1ms(2000);
				}

				sendcardlxy_changecard_command(0x00);

				return FALSE;
			}
			//if(CurCard == 1)
				sendcardlxy_changecard_command(0x00);		
		}
    }
    //else 
	if(send_Card->sendcardtype == _SENDCARD_COLORLIGHT)   // ColorLight
    {
        connecttype = send_Card->connecttype[CurCard][CurPort];
        for(i = 0;i < vline;i++)
        {
            for(j = 0;j < hline;j++)
            {
                cardn = getLedConnectIndex(connecttype,hline,vline,j,i);
                
       #if 1   //@cc20141227
               timeoutflg1 =Sendcard_area_Command(cardn,   \
                            xoffset +j * hsize,yoffset + i * vsize,   \
                        xoffset + (j + 1) * hsize,yoffset + (i + 1) * vsize );
       #else
               timeoutflg1 =Sendcard_area_Command(i*getsendcard()->hline[curport]+j,j*getsendcard()->vsize,i*getsendcard()->hsize,j*getsendcard()->vsize+getsendcard()->vsize,i*getsendcard()->hsize+getsendcard()->hsize);
       #endif
               if(timeoutflg1 == FALSE)
               {
                   SendTimeoutFlg = TRUE;

                       SendTimeoutFlg = TRUE;

                    if(display == TRUE)
                    {
                        menuDisableBusy();  //added by @cc20140717
                        LCD_WriteLine4("    Save To Port  Fail");

                        SysDelay1ms(2000);
                    }                
                    return FALSE;  //send data Fail;
               }
           }

       }
        Sendcard_Command_jiazai();

    
        if(CurCard != choosecard_old)
        {
    		#if 1//@cc20141227
    		
            #if 1	 //20150330
            //setOutputSize(getsendcard()->outputformat);	// 20150402
            #else
            getsendcard()->sizex[0] = 0x400;
            getsendcard()->sizey[0] =0x200;
            getsendcard()->sizex[1] = 0x400;
            getsendcard()->sizey[1] =0x200;
            #endif
    		
    		 erase_flash(0x50000);
           timeoutflg1 =Sendcard_position_Command(0x50000,getsendcard()->posx[0],getsendcard()->posy[0],getsendcard()->sizex[0],getsendcard()->sizey[0]);
           timeoutflg2 =Sendcard_position_Command1(0x50400,getsendcard()->posx[1],getsendcard()->posy[1],getsendcard()->sizex[1],getsendcard()->sizey[1]);
           Sendcard_position_Command(0x50500,getsendcard()->posx[0],getsendcard()->posy[0],getsendcard()->sizex[0],getsendcard()->sizey[0]);
           Sendcard_position_Command(0x50600,getsendcard()->posx[0],getsendcard()->posy[0],getsendcard()->sizex[0],getsendcard()->sizey[0]);
           Sendcard_jizai_Command(0x60000);	//0x60000 ->0x50000 @cc20141227
    		#else
           erase_flash(0x50000);
           timeoutflg1 =Sendcard_position_Command(0x50000,getsendcard()->posx[0],getsendcard()->posy[0],getsendcard()->sizex[0],getsendcard()->sizey[0]);
           timeoutflg2 =Sendcard_position_Command1(0x50400,getsendcard()->posx[1],getsendcard()->posy[1],getsendcard()->sizex[1],getsendcard()->sizey[1]);
           Sendcard_jizai_Command(0x60000);
           #endif
           if((timeoutflg1 == FALSE)||(timeoutflg2 == FALSE))
           {
                if(display == TRUE)
                {
        			menuDisableBusy();  //added by @cc20140717
        			LCD_WriteLine4("    Save To Port  Fail");

        			SysDelay1ms(2000);
                }           
           		SendTimeoutFlg = TRUE;

                 return FALSE;
           }  
        }
    }
    


    if(display == TRUE)
    {
    	delayms(50);
    	menuDisableBusy(); //added by @cc20140717

    	LCD_WriteLine4("    Save To Port Success");
    	
    	SysDelay1ms(2000);
    }

    SetSetWidthEnable(FALSE);
		
	return TRUE;
}

void sendcard_Connect_init(void)
{
    BYTE setWidthEnable;

    setWidthEnable = GetUserPref()->setWidthEnable;
    
    setSendCardPortSelNoSave(0);
    setLEDSendCardConnect(FALSE,0);
    setSendCardPortSelNoSave(1);
    setLEDSendCardConnect(FALSE,0);
    setSendCardPortSelNoSave(2);
    setLEDSendCardConnect(FALSE,0);
    setSendCardPortSelNoSave(3);
    setLEDSendCardConnect(FALSE,0); 

    if(setWidthEnable == TRUE)
    {
        if(GetSendcardSetWidthChoose() == 0)
        {
        //    setWidthScale(0);
            setLEDSendCardWidth(FALSE,0);
        }
        else
        {
        //   setWidthScale(1); 
            setLEDSendCardWidth(FALSE,1);
        }        
    }

    SetSetWidthEnable(setWidthEnable);
}

u8 setLEDSendCardWidth(u8 display,int value)
{
    int width,height;
	SENDCARD_List_Typedef * send_Card;
 	bool timeoutflg1=FALSE,timeoutflg2=FALSE;	//added by @cc20140716   

	send_Card = getsendcard();

    if(display == TRUE)
    {
        menuSetBusy(3,6);
    }
    if(value == 0)
	{        
        getLEDSendCardSize(&width,&height);       
        //getLEDSendCardCardSize(1,&width,&height);
	} 
    else
    {
		width = GetUserPref()->SetWidthH;
		height = GetUserPref()->SetWidthV;  
    }
    //width += send_Card->posx[1];
	dport_width = send_Card->hline[1][0] * send_Card->hsize[1][0];// + send_Card->hline[1][1] * send_Card->hsize[1][1];	
	if(dport_width > 0)
    {
        dport_width -= 1;
    }
	dport_height = send_Card->vline[1][0]* send_Card->vsize[1][0];// + send_Card->vline[1][1]* send_Card->vsize[1][1];
	if(dport_height > 0)
    {
        dport_height -= 1;
    }	
    if(dport_height > 1023)
    {
       dport_height = 1023;
    }
	uport_width = send_Card->hline[1][1] * send_Card->hsize[1][1];//dport_width;
	if(uport_width > 0)
    {
        uport_width -= 1;
    }	
	uport_height = send_Card->vline[1][1]* send_Card->vsize[1][1];//dport_height;  
	if(uport_height > 0)
    {
        uport_height -= 1;
    }	

    sendcardlxy_changecard_command(1);
    
    //info ("getsendcard()->choosecard :%d\r\n",getsendcard()->choosecard);
    readsendcardlxy_check();
    readsendcardlxy();
    //timeoutflg1 = sendcardlxy_xy_command(getsendcard()->posx[CurCard],getsendcard()->posy[CurCard],
    //        send_Card->xoffset[CurCard][1],send_Card->yoffset[CurCard][1],
    //         send_Card->xoffset[CurCard][0],send_Card->yoffset[CurCard][0]);
    timeoutflg1 = sendcardlxy_setWidth_command(getsendcard()->posx[1],getsendcard()->posy[1],width,
                send_Card->xoffset[1][1],send_Card->yoffset[1][1],
                 send_Card->xoffset[1][0],send_Card->yoffset[1][0]);    
    //timeoutflg1 = sendcardlxy_xy_command(getsendcard()->posx[CurCard],getsendcard()->posy[CurCard],xoffset,yoffset);
    timeoutflg2 = sendcardlxy_command_xy_save(28,getsendcard()->posx[1],getsendcard()->posy[1]);
    //if(CurCard == 1)
		if((timeoutflg1 == FALSE)||(timeoutflg2 == FALSE))
		{
			SendTimeoutFlg = TRUE;

            if(display == TRUE)
            {
    			menuDisableBusy();  //added by @cc20140717
    			LCD_WriteLine4("    Set The Width  Fail");

    			SysDelay1ms(2000);
            }

			sendcardlxy_changecard_command(0x00);

			return FALSE;
		}       
        sendcardlxy_changecard_command(0x00);  

        
    if(value == 0)
    {        
        getLEDSendCardSize(&width,&height);       
        //getLEDSendCardCardSize(0,&width,&height);
    } 
    else
    {
        width = GetUserPref()->SetWidthH;
        height = GetUserPref()->SetWidthV;  
    }
    //width += send_Card->posx[1];

	dport_width = send_Card->hline[0][0] * send_Card->hsize[0][0] ;//+ send_Card->hline[0][1] * send_Card->hsize[0][1];
	if(dport_width > 0)
    {
        dport_width -= 1;
    }	
	dport_height = send_Card->vline[0][0]* send_Card->vsize[0][0];// + send_Card->vline[0][1]* send_Card->vsize[0][1];
	if(dport_height > 0)
    {
        dport_height -= 1;
    }	
    if(dport_height > 1023)
    {
       dport_height = 1023;
    }
	uport_width = send_Card->hline[0][1] * send_Card->hsize[0][1];//dport_width;
	if(uport_width > 0)
    {
        uport_width -= 1;
    }	
	uport_height = send_Card->vline[0][1]* send_Card->vsize[0][1];//dport_height; 
	if(uport_height > 0)
    {
        uport_height -= 1;
    }	
    
    //info ("getsendcard()->choosecard :%d\r\n",getsendcard()->choosecard);
    readsendcardlxy_check();
    readsendcardlxy();
    //timeoutflg1 = sendcardlxy_xy_command(getsendcard()->posx[CurCard],getsendcard()->posy[CurCard],
    //        send_Card->xoffset[CurCard][1],send_Card->yoffset[CurCard][1],
    //         send_Card->xoffset[CurCard][0],send_Card->yoffset[CurCard][0]);
    timeoutflg1 = sendcardlxy_setWidth_command(getsendcard()->posx[0],getsendcard()->posy[0],width,
                send_Card->xoffset[0][1],send_Card->yoffset[0][1],
                 send_Card->xoffset[0][0],send_Card->yoffset[0][0]);  
    //timeoutflg1 = sendcardlxy_xy_command(getsendcard()->posx[CurCard],getsendcard()->posy[CurCard],xoffset,yoffset);
        timeoutflg2 =sendcardlxy_command_xy_save(28,getsendcard()->posx[0],getsendcard()->posy[0]);
		if((timeoutflg1 == FALSE)||(timeoutflg2 == FALSE))
		{
			SendTimeoutFlg = TRUE;

            if(display == TRUE)
            {
    			menuDisableBusy();  //added by @cc20140717
    			LCD_WriteLine4("    Set The Width  Fail");

    			SysDelay1ms(2000);
            }

			sendcardlxy_changecard_command(0x00);

			return FALSE;
		}     
    //if(CurCard == 1)
    sendcardlxy_changecard_command(0x00); 

    if(display == TRUE)
    {
    	menuDisableBusy(); //added by @cc20140717

    	LCD_WriteLine4("    Set The Width Success");
        
        	
    	SysDelay1ms(2000);
    }

    SetSetWidthEnable(TRUE);
		
	return TRUE;

}


void setLEDSendCardCascade(BYTE value)
{
	getsendcard()->cascade = value;
	led_setSendCardCascade(value);
	savesendcard();
}


void setLEDSendCardBrightness(BYTE value)
{
	getsendcard()->brightness = value;
	//savesendcard();
	//setBrightness(value);

    // if(getsendcard()->choosecard==1)
    sendcardlxy_changecard_command(0x01);
    readsendcardlxy_check();
    readsendcardlxy();
    sendcardlxy_brightness_command(value);
    // if(getsendcard()->choosecard==1)
    sendcardlxy_changecard_command(0x00);
    //sendcardlxy_brightness_command(value);
    //led_setSendCardBrightness(value);	//设置发送卡亮度

    readsendcardlxy_check();
    readsendcardlxy();
    sendcardlxy_brightness_command(value);
    sendcardlxy_changecard_command(0x00);
       
	savesendcard();
}

void setLEDGamma(BYTE value)
{
	getsendcard()->gamma = value;
	led_setSendCardGamma(value);
	savesendcard();
}

void setLEDSendCardReceiveNumber(BYTE value)
{
#if 0
	getsendcard()->receivecardnumber = value;
	led_setSendCardReceiveNumber(value);
	savesendcard();
#else		// For VIP Drive 43s
	getsendcard()->receivecardnumber = value;
	savesendcard();	
#endif
}
void setLEDSendcardX(int value)
{
#if 0

	getsendcard()->x = value;
	led_setSendCardX(value);
	savesendcard();
#else		// For VIP Drive 43s
	getsendcard()->x = value;
	//led_setSendCardX(value);
	savesendcard();

#endif
}
void setLEDSendCardY(int value)
{
#if 0
	getsendcard()->y = value;
	led_setSendCardY(value);
	savesendcard();
#else		// For VIP Drive 43s
	getsendcard()->x = value;
	//led_setSendCardX(value);
	savesendcard();
#endif
}
void setLEDSendCardReceiveCardParam(void)
{
#if 0
	led_setSendCardReceiveCardParam();
#elif 0		// For VIP Drive 43s,merge from ZYK Code
	// 这个函数最后还需要调试(罗正华-20160516)
	menuSetBusy(0,12);	//added by @cc	20140723	
	//UARTInit(1,115200);

	if(getsendcard()->receivecardnumber < 1)	//added by @cc20140725
	{
		getsendcard()->receivecardnumber = 1;
	}
	if(getsendcard()->sendcardtype==1)
	{
		Sendcard_area_Command(getsendcard()->receivecardnumber -1,getsendcard()->x,getsendcard()->y,getsendcard()->x+getsendcard()->vsize,getsendcard()->y+getsendcard()->hsize);
		Sendcard_Command_jiazai();
	}
	else if(getsendcard()->sendcardtype==0)
	{
		u8 flag =1;
		if(getsendcard()->choosecard==1)
			sendcardlxy_changecard_command(0x01);

		if(flag==1)	//added by @cc20140728	不加此项有时候会发送失败 ，无回复
		{
			flag=0;

			sendcardlxy_write_en_command_pre(0);
			sendcardlxy_write_command_pre(0,0,0,0,0);
			sendcardlxy_write_un_command_pre(0);
		}
		timeoutflg1 = sendcardlxy_write_en_command(getsendcard()->receivecardnumber-1);
		 timeoutflg2 = sendcardlxy_write_command(getsendcard()->receivecardnumber-1,getsendcard()->x,getsendcard()->y,getsendcard()->vsize-1,getsendcard()->hsize-1);
		 if((timeoutflg1 == FALSE)||(timeoutflg2== FALSE))
		 {
		 	 SendTimeoutFlg = TRUE;
		 	 menuDisableBusy();//added by @cc20140717
		     break;	//send data Fail;
		 }
		 sendcardlxy_write_un_command(getsendcard()->receivecardnumber-1);

		 sendcardlxy_save_en_command(getsendcard()->receivecardnumber-1);
		 sendcardlxy_save_clear_command(getsendcard()->receivecardnumber-1);
		 sendcardlxy_save_command(getsendcard()->receivecardnumber-1,getsendcard()->x,getsendcard()->y,getsendcard()->vsize-1,getsendcard()->hsize-1);
		 sendcardlxy_save_un_command(getsendcard()->receivecardnumber-1);
		 if(getsendcard()->choosecard==1)
	 	     sendcardlxy_changecard_command(0x00);
	}
	menuDisableBusy();//added by @cc20140723

#endif
}


void setLEDSendCardColorTemp(BYTE index,BYTE value)
{	
	switch(index)
	{
		case 0x00:
			getsendcard()->colortemR = value;
			break;
		case 0x01:
			getsendcard()->colortemG = value;
			break;
		case 0x02:
			getsendcard()->colortemB = value;
			break;
	}
	led_setSendCardColorTemp(index,value);
	savesendcard();
}
void setLEDSendCardColorTempParam(void)
{
#if 0
	led_setSendCardColorTempParam();
#else
    bool timeoutflg1;

    menuSetBusy(3,12);  //added by @cc20140717
   // USART2_SW_Init();
    if(getsendcard()->sendcardtype==0)
    {   
         if(getsendcard()->choosecard==1)
             sendcardlxy_changecard_command(0x01);
    
        sendcardlxy_write_en_commandNew(0x51);
        //timeoutflg1 =sendcardlxy_write_commandNew(0,getsendcard()->posx[0],getsendcard()->posy[0]);
        timeoutflg1 = sendcardlxy_write_command_ColorTem(0x002200,0x1A);
        sendcardlxy_write_un_commandNew(0x53);

        //20150922
        sendcardlxy_write_en_commandNew(0x51);
        //timeoutflg1 =sendcardlxy_write_commandNew(0,getsendcard()->posx[0],getsendcard()->posy[0]);
        timeoutflg1 = sendcardlxy_write_command_ColorTem(0x002200,0x1A);
        sendcardlxy_write_un_commandNew(0x53);
        //20150922 end
        if(getsendcard()->choosecard==1)
            sendcardlxy_changecard_command(0x00);

        if(timeoutflg1 == TRUE)
        {
        }
        
    }
    delayms(50);
    menuDisableBusy();  //added by @cc

#endif
}

void checkLEDSendCard(void)
{
	led_sendAndWait(0x90, 0xf0, 0x00, 0x00, 0x00);	
}


