#include "Input.h"
#include "rs232command.h"
#include <stdio.h>
#include "pipe.h"
#include "UART.h"
#include "SysTick.h"
#include "Control.h"
#include "stdp9320.h"
//#include "mst6m48.h"
#include "timeControl.h"
#include "userpref.h"
#include "menu.h"
#include "langStr.h"


#if 0
typedef enum 
{
	interpret_cmd_init,
	interpret_cmd_start,
	interpret_cmd_data,
	interpret_cmd_end,
} InterpretCmdState_TypeDef;
#endif

#define Input_UART_PORT  3
u8 receive_Input_buffer[1024];
Pipe_t receive_Input_pipe;
u8 receive_Input_sn = 0;
u8 send_Input_sn = 0;
static u8 deviceIsConnet_Input = 1;
const char Input_cmd_header[] = {'<','F'};
const char Input_cmd_end[] = {'>'};

static bool refleshInputEnable =  true;

static Command_TypeDef cmd_from_Input;
static InterpretCmdState_TypeDef state_Input = interpret_cmd_init;
static u32 startInputTime;


static InputSetting inputSetting;

#if 0
static char USB_ReConnectFlag = 1; // 0- no need to reflesh file number,play list and file names    1- reflesh file number,play list and names
#else
static char USB_ReConnectFlag[3] = {1,1,1}; // 0- no need to reflesh file number,play list and file names    1- reflesh file number,play list and names
#endif
static char USB_ReadSuccessFlag[3] = {0,0,0}; // 0- fail to reflesh file number,play list and file names    1- reflesh file number,play list and names successful
static char USB_RefleshFileNameFlag = 0; // 0- 文件名已刷新  1- 文件名待刷新

//char USB_FileNames[10][128]; //最多一次存10个文件名    getUSBControlSetting()->fileName里面只存现在正播放的文件名
char USB_FileNames[128]; //最近一次读取的文件名全128字节
char USB_ReadSuccess = 1; // 1-read success   0- read fail ,need to read again

#define debugInput103EDID(x)      //x


void usb_readPlayListAndSave(char board)	;


u8 getInputStatus(void)
{
	return deviceIsConnet_Input;
}

void EnableRefleshInput(void)
{
	refleshInputEnable = true;
}
void DisableRefleshInput(void)
{
	refleshInputEnable = false;
}

char *getUSBFileNames(void)
{
	return USB_FileNames;
}

#if 0
char getUSB_ReConnectFlag(void)
{
	return USB_ReConnectFlag;
}
#endif
char getUSB_RefleshFileNameFlag(void)
{
	return USB_RefleshFileNameFlag;
}

void Input_send_Port(int port,int cmd, int dat1, int dat2, int dat3, int dat4);
	
InputSetting * getInputSetting(void)
{
	return &inputSetting;
}


void init_Input(void)
{
#if 0
	char i;
	pipe_init(&receive_Input_pipe,receive_Input_buffer,sizeof(receive_Input_buffer));
	UARTInit(Input_UART_PORT,115200); //main board 103

	

	for(i = 0; i < 3; i++)
	{
		inputSetting.isConnect[i] = 0;
		inputSetting.inputChanged[i] = 0;
		inputSetting.extInput[i].inputPort = 0;
		inputSetting.extInput[i].inputFormat = 0;
		inputSetting.extInput[i].inputType = 0;
	}
	inputSetting.extInput[0].outputFormat = MD_1920x1080_60;
	inputSetting.extInput[1].outputFormat = MD_1920x1080_60;
	inputSetting.extInput[2].outputFormat = 0;
	inputSetting.hardVer = 0xff;
	inputSetting.regValue = 0xff;
	inputSetting.autoChangeOutputFormatEnable[0] = 0; //1;
	inputSetting.autoChangeOutputFormatEnable[1] = 0; //1;
	
	Input_sendAndWait(0x6A, 0x03, 0x00, 0x00, 0x00);	// read soft version
	Input_sendAndWait(0x6A, 0x07, 0x00, 0x00, 0x00);	// read hard version
#endif	
}

#if 0
void SwitchCmdToPc(Command_TypeDef *cmd)
{
	#if 0
	char buffer[30];
	Command_TypeDef cmdToPc;
	cmdToPc = *cmd;
	cmdToPc.checksum = cmdToPc.address+cmdToPc.sn+cmdToPc.command+cmdToPc.data_1+cmdToPc.data_2+cmdToPc.data_3+cmdToPc.data_4;
	sprintf(buffer, "<F%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", cmdToPc.address,cmdToPc.sn,cmdToPc.command,
			cmdToPc.data_1,cmdToPc.data_2,cmdToPc.data_3,cmdToPc.data_4,cmdToPc.checksum);
	SerialPutString(buffer);
	#else
	printf("<F%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", cmd->address,cmd->sn,cmd->command,
			cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,cmd->checksum);
	#endif
}
#endif

void SwitchCmdToPc_Input(Command_TypeDef *cmd)
{
	#if 0
	char buffer[30];
	Command_TypeDef cmdToPc;
	cmdToPc = *cmd;
	cmdToPc.checksum = cmdToPc.address+cmdToPc.sn+cmdToPc.command+cmdToPc.data_1+cmdToPc.data_2+cmdToPc.data_3+cmdToPc.data_4;
	sprintf(buffer, "<F%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", cmdToPc.address,cmdToPc.sn,cmdToPc.command,
			cmdToPc.data_1,cmdToPc.data_2,cmdToPc.data_3,cmdToPc.data_4,cmdToPc.checksum);
	SerialPutString(buffer);
	#else
	cmd->address += 0x02;
	cmd->checksum += cmd->address;
	//if((cmd->command == 0x6a)&&(cmd->address > 2))  // 2 = mcuID of ext input board 	3 = ext7_mst182 	4 = ext8_mst182 
	if((cmd->command == 0x6a))	// 2 = mcuID of ext input board 	3 = ext7_mst182 	4 = ext8_mst182 
	{
		printf("<F%02x%02x%02x%02x%02x%02x%02x%02x>", cmd->address,cmd->sn,cmd->command,
				cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,cmd->checksum);
	}
	#endif
}
int ReceiveBuffer_Input(char *buffer, int size, int timeout)
{
	u32 startTime;
	int i = 0;
	
	startTime = GetSysTick();
	
	while(1)
	{
		BYTE value;
		if(pipe_read(&receive_Input_pipe, &value))
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
	return i;
}

int input_readBuffer( char *buffer, int size, int timeout)
{
	return ReceiveBuffer_Input(buffer,size,timeout);
}
int getInputType(int port)  // 0 - 无输入，1 - DVI输入，2 - HDMI, 3 - SDI, 4 - DP, 5 - VGA, 6 - YpbPr, 7 - CVBS, 8 - SV,  9 --（Double）DVI， 10--USB
{
	if(inputSetting.isConnect[port] == 1)
	{
		return inputSetting.extInput[port].inputType;
	}
	else
	{
		return 0;
	}
}

#define READ_INPUT_BLOCK_SIZE 128
static u32 usbPlayListlen = 0;
u32 getUSBPlayListlen(void)
{
	return usbPlayListlen;
}
u32 getUsbPlaylistBlock(void)	
{
	return usbPlayListlen/128;
}

static bool readFromMCUFlag_input = 0;
static void process_Input_cmd(Command_TypeDef * cmd)
{
#if 0
	int len;
	char buffer[256];
	deviceIsConnet_Input = 1;
	#if 0
	if(cmd->checksum != getCmdChecksum(cmd))
	{
		return;
	}
	#endif
	
	#if 0
	if((cmd->command == 0x6a)||(cmd->command == 0x8a))
	//if((cmd->command == 0x6a))
	{
		if(cmd->address != 2) //    2 = mcuID of ext input board     3 = ext7_mst182     4 = ext8_mst182 
 		{
			//inputSetting.isConnect[cmd->address - 3] = 1;
		}
		SwitchCmdToPc_Input(cmd);
	}
	#else
	if(readFromMCUFlag_input  == 0)
	{
		SwitchCmdToPc_Input(cmd);
	}
	#endif
	
	//printf("process_Input_cmd:%x,%x,%d,%d,%d---->%d\r\n",cmd->command,cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,GetSysTick());
	
	switch(cmd->command)
	{	
		case 0x68:
			switch(cmd->data_1)
			{
				case 0x10:
				case 0x11:
					break;
				case 0x13:
					break;
				case 0x20:
					break;
					
			}
			break;
		case 0x6a:
			switch(cmd->data_1)
			{
				#if 1
				case 0x03:
					if(readFromMCUFlag_input == 1)
					{
						readFromMCUFlag_input = 0;
					}
					//inputSetting.ver_msb = cmd->data_2;
					//inputSetting.ver_lsb = cmd->data_3;
					{
						//int value;
						//*(u8 *)&value = cmd->data_3;
						//*((u8 *)&value + 1) = cmd->data_2;
						switch(cmd->address)
						{
							//case 2://input board;
							//	inputSetting.version[2] = value;
							//	break;
							case 3://ext7 mst182;
								inputSetting.isConnect[0] = 1;
								inputSetting.mstVersion[0] = cmd->data_3+(cmd->data_2<<8);//value;
								printf("inputSetting.version[0] = %x\r\n",inputSetting.mstVersion[0]);
								break;
							case 4://ext8 mst182;
								inputSetting.isConnect[1] = 1;
								inputSetting.mstVersion[1] = cmd->data_3+(cmd->data_2<<8);//value;
								printf("inputSetting.version[1] = %x\r\n",inputSetting.mstVersion[1]);
								break;
							case 5://ext9 mst182
								inputSetting.isConnect[2] = 1;
								inputSetting.mstVersion[2] = cmd->data_3+(cmd->data_2<<8);//value;
								//printf("inputSetting.mstVersion[2] = %x\r\n",inputSetting.mstVersion[2]);
								break;
						}
					}
					break;	
					#endif
				case 0x05:
					if(readFromMCUFlag_input == 1)
					{
						readFromMCUFlag_input = 0;
					}
					{
						inputSetting.softVer = cmd->data_3+(cmd->data_2<<8) ;
					}
					break;	
					
				case 0x07:
					if(readFromMCUFlag_input == 1)
					{
						readFromMCUFlag_input = 0;
					}
					{
						if((cmd->data_4 == 0)&&(cmd->data_2 == 0))
						{
							inputSetting.hardVer = cmd->data_3;
						}
						printf("input board hard version:V1.%d\r\n",inputSetting.hardVer);
					}
					break;	
				case 0x09:
					if(readFromMCUFlag_input == 1)
					{
						readFromMCUFlag_input = 0;
					}
					{
						switch(cmd->data_2)
						{
							case 0://ext7 mst182;
								inputSetting.isConnect[0] = 1;
								inputSetting.mstVersion[0] = cmd->data_3+(cmd->data_4<<8);//value;
								//printf("inputSetting.mstVersion[0] = %x\r\n",inputSetting.mstVersion[0]);
								break;
							case 1://ext8 mst182;
								inputSetting.isConnect[1] = 1;
								inputSetting.mstVersion[1] = cmd->data_3+(cmd->data_4<<8);//value;
								//printf("inputSetting.mstVersion[1] = %x\r\n",inputSetting.mstVersion[1]);
								break;
							case 2://ext9 mst182
								inputSetting.isConnect[2] = 1;
								inputSetting.mstVersion[2] = cmd->data_3+(cmd->data_4<<8);//value;
								//printf("inputSetting.mstVersion[2] = %x\r\n",inputSetting.mstVersion[2]);
								break;
						}
					}
					break;
				
				default:
					break;
			}
			break;
		case 0x73:
			switch(cmd->data_1)			
			{
				case 0x06:
				case 0x07:
					//printf("Input_readPCA8574() = %d\r\n",inputSetting.regValue);
					inputSetting.regValue = cmd->data_2;
					break;
				case 0x08:
				case 0x09:
					inputSetting.regValue = cmd->data_3;
					break;
			}
			break;
			
		case 0x74:
			switch(cmd->data_1)
			{
				//case 0x02:
				case 0x03:
					//printf("read port%d output format = %d\r\n",cmd->data_2,cmd->data_3);
					inputSetting.extInput[cmd->data_2].outputFormat = cmd->data_3;
					break;
			}
			break;
			
		case 0x81:
			switch(cmd->data_1) 		
			{
				case 0x02:
				case 0x03:
					inputSetting.regValue = cmd->data_3;
					break;
				case 0x04:
				case 0x05:
					inputSetting.regValue = cmd->data_3;
					break;
					
			}
			break;
			
		case 0x86:
			switch(cmd->data_1)			
			{
				case 0x0a:
				case 0x0b:
					inputSetting.regValue = cmd->data_3;
					break;
					
				case 0x0e:
				case 0x0f:
					inputSetting.regValue = cmd->data_3;
					break;
			}
			break;
			
		case 0x82: //vga
			switch(cmd->data_1)			
			{
				case 0x00:
				case 0x01:
					{
						char port = cmd->data_2>>4,index = cmd->data_2&0x0f;
						int value = cmd->data_3 + (cmd->data_4<<8);
						switch(index)
						{
							case 0x00: 
								inputSetting.extVGAAdjust[port].pos_x = value;
								break;
							
							case 0x01: 
								inputSetting.extVGAAdjust[port].pos_y = value;
								break;
							
							case 0x02: 
								inputSetting.extVGAAdjust[port].clock = value;
								break;
							
							case 0x03: 
								inputSetting.extVGAAdjust[port].phase = value;
								break;
							
						}
					}
					break;
			}			
			break;
		case 0xf1:
			if((cmd->data_3 == 0xff) && (cmd->data_4 == 0xff))
			{
				break;
			}
			switch(cmd->data_1)
			{
				case 9:
					len = cmd->data_3+(cmd->data_4<<8);
					//printf("0xf1, 0x09,len = %d \r\n", len);
					if((len == input_readBuffer(buffer, len, 200)) && (len >= 32))
					{
						char inputPort = cmd->data_2;
						int inputAddr;
						ExtInputSetting * inputSet;
				
						inputAddr = getInputType(cmd->data_2);

						#if 0 //(DISP_TEST_INFO)
						{
							int i;
							printf("0xf1, len = %d\r\n", len);
							for(i = 0; i < len; i++)
							{
								printf(" %x, ",buffer[i]);
							}
							printf("\r\n");
						}
						#endif
						//printf("9. read addr_%d, input port = %d,dat2 = %d", adr,inputPort,dat2);
						inputSet = &inputSetting.extInput[inputPort];
						
						//if(inputPort > 1)
						if(((inputPort > 1)&&(getInputSetting()->hardVer < 3))
						  ||((inputPort > 2)&&(getInputSetting()->hardVer == 3)))
						{
							return;
						}
						inputSet->cropDot.posX = buffer[0]+(buffer[1]<<8);
						inputSet->cropDot.posY = buffer[2]+(buffer[3]<<8);
						inputSet->cropDot.sizeX = buffer[4]+(buffer[5]<<8);
						inputSet->cropDot.sizeY = buffer[6]+(buffer[7]<<8);
						inputSet->scale.posX = buffer[8]+(buffer[9]<<8);
						inputSet->scale.posY = buffer[10]+(buffer[11]<<8);
						inputSet->scale.sizeX = buffer[12]+(buffer[13]<<8);
						inputSet->scale.sizeY = buffer[14]+(buffer[15]<<8);
						inputSet->freeze = buffer[16];
						inputSet->horizontalFlip = buffer[17];
				
						#if 0
						//inputSet->inputPort = 3-buffer[18];		 //for 732 input board
						inputSet->inputPort = buffer[18];  //for 182 input board
						#else
						//printf("getInputType(adr) = %d,",inputAddr);
						if(inputAddr == ExtInput_CV)
						{
							printf("ExtInput_CV:inputSet->inputPort = %d,",buffer[18]);
							inputSet->inputPort = buffer[18];  //for CVBS_182/USB_182  input board
						}
						else
						{
							printf("inputSet->inputPort = %d,",0);
							inputSet->inputPort = 0;  //for other 182 input boards
						}
						#endif
						
						inputSet->alpha = buffer[19];
						inputSet->layerOnOff = 0;
						
						if(len >= 32) 	//for 182 input board	  VSP1U
						{
							inputSet->brightness[0] = buffer[21];
							inputSet->brightness[1] = buffer[22];
							inputSet->brightness[2] = buffer[23];
							
							inputSet->contrast[0] = buffer[24];
							inputSet->contrast[1] = buffer[25];
							inputSet->contrast[2] = buffer[26];
							inputSet->sharpness = buffer[27];
				
							inputSet->saturation = buffer[28];		//for 182 input board
							inputSet->colorTemp[0] = buffer[29];	//for 182 input board
							inputSet->colorTemp[1] = buffer[30];	//for 182 input board
							inputSet->colorTemp[2] = buffer[31];	//for 182 input board
						}				
					}				
					break;
				case 10:
					len = cmd->data_3+(cmd->data_4<<8);
					//printf("0xf1, len = %d\r\n", len);
					if((len == input_readBuffer(buffer, len, 1000)) && (len >= 21))
					{
						//char inputPort = 0;//getInputPortByAdr(adr);
						int i;
				
						//LED_ON(0x00000001);   //for test
						#if 0//(DISP_TEST_INFO)
						{
							printf("0xf1, len = %d,---->%d\r\n", len,GetSysTick());
							for(i = 0; i < len; i++)
							{
								printf(" %x, ",buffer[i]);
							}
							printf("\r\n");
						}
						#endif
						
						#if 1
						for(i=0; i<3; i++)
						{
							int value;
							//char inputPortTemp;

							//printf("port %d,", i);
							if(inputSetting.extInput[i].inputType != buffer[0+i])
							{
							//	printf("inputType change %d,%d\r\n", inputSetting.extInput[i].inputType,buffer[0+i]);
								inputSetting.inputChanged[i] = 1;
								inputSetting.extInput[i].inputType = buffer[0+i];
							}
							
							if((inputSetting.extInput[i].inputType != 0)&&(inputSetting.extInput[i].inputType != 0xff))
							{
								inputSetting.isConnect[i] = 1;
							}

							#if 0
							if(inputSetting.extInput[i].inputFormat != buffer[3+i])
							{
							//	printf("inputFormat change %d,%d\r\n", inputSetting.extInput[i].inputFormat,buffer[3+i]);
								inputSetting.inputChanged[i] = 1;
								inputSetting.extInput[i].inputFormat = buffer[3+i];
							}
							#else
							//  182的返回格式里面,										
								//MD_1024x1280_60 		= 64,// 20121229
								//MD_1024x1920_60 		= 65,// 20121229							
								//MD_1920x1080_60_reduce_new	= 66, // 20130112							
								//MD_1248x936_60_reduce	= 67,// 20130116
								//MD_1976x1144_60_reduce	= 68,// 20130116
								//MD_1872x1040_60_reduce	= 69,// 20130116							
								//MD_1080x1920_60 = 70,
							//这一段8028都不支持,所以读到分辨率在这个区间的时候要修改为MD_UNDEFINE = 126,
							//之后的
								//MD_720x480_60i_2 = 71,
								//MD_720x576_50i_2 = 72,
							//要转换成8082的
								//MD_1440X240 	= 64,  //added by cyj  for 480I from Hard Disk Player_HDMI
								//MD_1440X288 	= 65,  //added by cyj  for 576I from Hard Disk Player_HDMI
							//否则会造成这两个分辨率的识别问题
							if((buffer[3+i] > 63)&&(buffer[3+i] < 71))
							{
								buffer[3+i] = 126;
							}
							else if((buffer[3+i] == 71)||(buffer[3+i] == 72))
							{
								buffer[3+i] -= 7;
							}
							if(inputSetting.extInput[i].inputFormat != buffer[3+i])
							{
								//printf("inputFormat change %d,%d\r\n", inputSetting.extInput[i].inputFormat,buffer[3+i]);
								inputSetting.inputChanged[i] = 1;
								inputSetting.extInput[i].inputFormat = buffer[3+i];
							}
							#endif
							
							value = buffer[6+i*2]+(buffer[6+i*2+1]<<8);
							if(inputSetting.extInput[i].HActive != value)
							{
							//	printf("HActive change %d,%d\r\n",inputSetting.extInput[i].HActive, value);
								inputSetting.inputChanged[i] = 1;
								inputSetting.extInput[i].HActive = value;
							}
							 
							value = buffer[12+i*2]+(buffer[12+i*2+1]<<8);
							if(inputSetting.extInput[i].VActive != value)
							{
							//	printf("VActive change %d,%d\r\n",inputSetting.extInput[i].VActive, value);
								inputSetting.inputChanged[i] = 1;
								inputSetting.extInput[i].VActive = value;
							}
							
							if(inputSetting.extInput[i].VFreq != buffer[18 + i])
							{
							//	printf("VFreq change %d,%d\r\n",inputSetting.extInput[i].VFreq, buffer[18 + i]);
								inputSetting.inputChanged[i] = 1;
								inputSetting.extInput[i].VFreq = buffer[18 + i];
							}

							if(len >= 24)
							{
								if(inputSetting.extInput[i].outputFormat != buffer[21 + i])
								{
								//	printf("outputFormat change %d,%d\r\n",inputSetting.extInput[i].outputFormat, buffer[21 + i]);
									inputSetting.inputChanged[i] = 1;
									inputSetting.extInput[i].outputFormat = buffer[21 + i];
								}
								//printf("\r\n");
							}
							else
							{
								inputSetting.extInput[i].outputFormat = MD_1680x1050_60;
							}
						}
						#endif
					
					}
					//else
					//{
					//	printf("read len error = %d\r\n", readLen);
					//}					
					break;
					
				case 0x0C: //edid				
					if(cmd->data_2 > 2)
					{
						return;
					}
					{
						int index,len;
						char buffer[257]; //256 + 1
						len = cmd->data_3 + (cmd->data_4<<8);
						if(len == 0)
						{
							inputSetting.edidLen[cmd->data_2] = 0;
							break;
						}
						index = input_readBuffer(buffer, len, 1000);
						if(index != len)
						{								
							printf("<F:receive %d data from input fail>", index);
							return;
						}
						inputSetting.edidLen[cmd->data_2] = len;
						//printf receive file
						debugInput103EDID(printf("read port %d edid,len %d ,= \r\n",cmd->data_2,inputSetting.edidLen[cmd->data_2]);)
						for(index = 0; index < len; index++)
						{
							inputSetting.edidDetail[cmd->data_2][index] = buffer[index];
							debugInput103EDID(printf(" %02x  ",inputSetting.edidDetail[cmd->data_2][index]);)//buffer[index]
						}
					}
					break;
			}
			break;
		#if 0
		case 0x8a:  //USB 
			if((cmd->address == (getUSBControlSetting()->operatBoard + 2))&&(cmd->address != 2))
			{
				switch(cmd->data_1)
				{
					case 0x01: //get file number
						getUSBControlSetting()->fileNumber = cmd->data_2;
						printf("getUSBControlSetting()->fileNumber = %d\r\n",getUSBControlSetting()->fileNumber);
						break;
					case 0x03: //get file name
						{
							#if 1
							#if 0
							int i;
							u8 value;
							u32 startTime = GetSysTick();
							getUSBControlSetting()->fileName[129] = cmd->data_2;
							
							i=0;
							printf("\r\nfile %d name is ",cmd->data_2);
							while(TRUE)
							{
								if(pipe_read(&receive_Input_pipe, &value))
								{
									getUSBControlSetting()->fileName[i++] = value;
									printf(" %d ,",value);
								}
								if(i>128)
								{
									break;
								}
								if(isSysTickTimeout(startTime,3000))
								{
									break;
								}
							}
							#else
							int index;
							//char buffer[129];
							index = input_readBuffer(USB_FileNames, READ_INPUT_BLOCK_SIZE,1000);
							if(index != READ_INPUT_BLOCK_SIZE)
							{								
								printf("read file name<F:receive %d data from input fail>", index);
								USB_ReadSuccess = 0;
								return;
							}
							USB_ReadSuccess = 1;
							printf("\r\nfile %d is\r\n",cmd->data_2);
							{
								char i,skip = 0,dotPos = 0,dotLen = 0,nameLen = 0;
								char *fileName = USB_FileNames;
								#if 1
								//字符:m   ascii 码:0x64   182返回:0x0064
								//182发送文件名的时候总是会在0x0a前加上 0x0d,特奇怪,所以只能在接收端滤调这个0x0d <-------20150415
								//从128字节中筛出24个字节的文件名,滤除莫名出现在0X0A前的0X0D
								for(i = 0; i < 24; i++)
								{
									if((*fileName == 0x0d)&&(*(fileName+1) == 0x0a))
									{
										fileName++;
									}
									getUSBControlSetting()->fileName[cmd->data_2-1][i] = *fileName;
									printf(" %02x ",*fileName);
									fileName++;
								}
								#else
								//从头取24字节作为文件名保存
								for(i = 0; i < 24; i++)
								{
									printf(" %02x ",getUSBControlSetting()->fileName[cmd->data_2-1][i]);
									getUSBControlSetting()->fileName[cmd->data_2-1][i] = USB_FileNames[i];
								}
								#endif
							}
							printf(" \r\n");
							#endif
							USB_RefleshFileNameFlag = 0;
							#endif
						}
						break;
						
					case 0x04: //set current file index
					case 0x05: //get current file index
						if(getUSBControlSetting()->playFileIndex != cmd->data_2)
						{
							getUSBControlSetting()->playFileIndex = cmd->data_2;
							//getUSBControlSetting()->fileChangeEn = 1;
							inputSetting.inputChanged[getUSBControlSetting()->operatBoard - 1] = 1;
							USB_RefleshFileNameFlag = 1;
						}
						break;
					case 0x07: //get current file total length
						getUSBControlSetting()->playFileIndex = cmd->data_2;
						getUSBControlSetting()->fileTotalLength = cmd->data_3 + (cmd->data_4<<8);
						break;
					case 0x08: //set file current time 
					case 0x09: //get file current time 
						getUSBControlSetting()->playFileIndex = cmd->data_2;
						getUSBControlSetting()->fileCurrentTime = cmd->data_3 + (cmd->data_4<<8);
						break;
					case 0x0a: //set play state
					case 0x0b: //get play state
						getUSBControlSetting()->playState = cmd->data_2;
						break;
					case 0x0c: //set photo time
					case 0x0d: //get photo time
						getUSBControlSetting()->photoTime = cmd->data_2;
						break;
					case 0x0e: //set play mode
					case 0x0f: //get play mode
						getUSBControlSetting()->playMode = cmd->data_2;
						//printf("getUSBControlSetting()->playMode = %d\r\n",getUSBControlSetting()->playMode);
						break; 
					case 0x10:
					case 0x11:
						getUSBControlSetting()->port = cmd->data_2;
						//printf("getUSBControlSetting()->port = %d\r\n",getUSBControlSetting()->port);
						break;
					case 0x13: //get play list
						#if 1
						{
							char buffer[257],blocknum = 0;
							int index;
							usbPlayListlen = (cmd->data_4<<8)+cmd->data_3;
							#if 0
							index = input_readBuffer(buffer, READ_INPUT_BLOCK_SIZE,200);
							if(index != READ_INPUT_BLOCK_SIZE)
							#else
							//字符:m   ascii 码:0x64   182返回:0x0064
							//182发送的时候总是会在0x0a前加上 0x0d,特奇怪
							//所以182发送的实际字节数,总是>= ((cmd->data_4<<8)+cmd->data_3), 只好在接收端多收一些数据,然后滤除0x0d 
							index = input_readBuffer(buffer,(READ_INPUT_BLOCK_SIZE*2),200);
							if(index < READ_INPUT_BLOCK_SIZE)
							#endif
							{								
								printf("<F:receive	%d data from input fail>", index);
								return;
							}
							
							#if 1
							#if 0 
							printf("\r\nlen =%d;blk=%d\r\n",usbPlayListlen,cmd->data_2);
							{
								int i;
								for(i = 0; i < (READ_INPUT_BLOCK_SIZE*2); i++)
								{
									printf(" %02x  ",buffer[i]);
								}
							}
							printf(" \r\n");
							#endif
							//滤除 0x0d							
							{
								int i;
								char *dataFilte = buffer;
								for(i = 0; i < READ_INPUT_BLOCK_SIZE; i++)
								{
									if((*dataFilte == 0x0d)&&(*(dataFilte+1) == 0x0a))
									{
										dataFilte++;
									}
									GetTimeControl()->timeControlBlock[i] = *dataFilte;
									dataFilte++;
								}
							}
							#if 1
							printf("\r\nafter filter:\r\n");
							{
								char i;
								for(i = 0; i < READ_INPUT_BLOCK_SIZE; i++)
								{
									printf(" %02x  ",GetTimeControl()->timeControlBlock[i]);
								}
							}
							printf(" \r\n");
							#endif
							#else
							printf("\r\nlen =%d;blk=%d\r\n",usbPlayListlen,cmd->data_2);
							{
								char i;
								for(i = 0; i < READ_INPUT_BLOCK_SIZE; i++)
								{
									printf(" %02x  ",buffer[i]);
									GetTimeControl()->timeControlBlock[i] = buffer[i];
								}
							}
							printf(" \r\n");
							#endif							
							GetTimeControl()->timeControlId = TIME_CONTROL_ID;	//added by @cc20140702
							GetTimeControl()->timeControlValid = 1;
							//GetTimeControl()->timeControlBlock = buffer;
							#if 1
							blocknum = TIMECOTROL_USBPLSYLISTMINBLOCK + cmd->data_2; //timeControl_getMaxBlocknum()+1;
							saveTimeControl(blocknum);		//Block From Block2
							timeControl_setMaxBlocknum(blocknum);
							printf("usbpfile:blk=%d;\r\n",blocknum);	
							#endif
						}
						#else
						{
							int i,j;
							u8 value,blocknum=0,rxBuffer[129];
							BYTE checksum;
							//u32 len;
							u8 *pBlock;
							u32 startTime = GetSysTick();
							usbPlayListlen = (cmd->data_4<<8)+cmd->data_3;
							i=0;
							checksum =0;
							while(TRUE)
							{
								if(pipe_read(&receive_Input_pipe, &value))
								{
									rxBuffer[i++] = value;
									//printf(" %x",rxBuffer[i]);
										
								}
								if(i>128)
								{
									i =0;
									checksum =0;
									#if 1
									for(j=0;j<128;j++)
									{
										checksum += rxBuffer[j];
										//printf("%x ",rxBuffer[j]);
									}
									if(checksum !=rxBuffer[128])
									{
										printf("<F:receive 129 data from 182 fail>");
										printf("\r\nchecksum=%d;rxdatacheck=%d;\r\n",checksum,rxBuffer[128]);
										//break;		//commented by @cc20140707
									}
									#endif
									printf("\r\nlen =%d;blk=%d\r\n",usbPlayListlen,cmd->data_2);
									#if 0
									for(j=0;j<128;j++)
									{
										printf(" %x",rxBuffer[j]);
									}
									//blocknum++;
									#endif
									#if 1
									pBlock = &(GetTimeControl()->timeControlBlock[0]);
									for(j=0; j<128; j++)
									{
										pBlock[j] = rxBuffer[j];
									}
									GetTimeControl()->timeControlId = TIME_CONTROL_ID;	//added by @cc20140702
									GetTimeControl()->timeControlValid = 1;
									blocknum =timeControl_getMaxBlocknum()+1;
									saveTimeControl(blocknum);		//Block From Block2
									timeControl_setMaxBlocknum(blocknum);
									printf("usbpfile:blk=%d;\r\n",blocknum);
									#endif
									break;
									
								}
								
								if(isSysTickTimeout(startTime,3000))
								{
									printf("\r\n=UART TIME OUT=\r\n");
									break;
								}
							}
						}
						#endif
						break;
					case 0x15: //set play order
					case 0x16: //get play order
						getUSBControlSetting()->playOrder = cmd->data_2;
						break;
					
				}
			}
			break;
		#else
		case 0x8a:  //USB 
			if((cmd->address == (getUSBControlSetting()->operatBoard + 2))&&(cmd->address != 2))
			{
				switch(cmd->data_1)
				{
					case 0x01: //get file number
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber = cmd->data_2;
						printf("getUSBControlSetting()->fileNumber = %d\r\n",getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber);
						break;
					case 0x03: //get file name
						{
							#if 1
							#if 0
							int i;
							u8 value;
							u32 startTime = GetSysTick();
							getUSBControlSetting()->fileName[129] = cmd->data_2;
							
							i=0;
							printf("\r\nfile %d name is ",cmd->data_2);
							while(TRUE)
							{
								if(pipe_read(&receive_Input_pipe, &value))
								{
									getUSBControlSetting()->fileName[i++] = value;
									printf(" %d ,",value);
								}
								if(i>128)
								{
									break;
								}
								if(isSysTickTimeout(startTime,3000))
								{
									break;
								}
							}
							#else
							int index;
							//char buffer[129];
							index = input_readBuffer(USB_FileNames, READ_INPUT_BLOCK_SIZE,1000);
							if(index != READ_INPUT_BLOCK_SIZE)
							{								
								printf("read file name<F:receive %d data from input fail>", index);
								USB_ReadSuccess = 0;
								return;
							}
							USB_ReadSuccess = 1;
							printf("\r\nfile %d is\r\n",cmd->data_2);
							{
								char i,skip = 0,dotPos = 0,dotLen = 0,nameLen = 0;
								char *fileName = USB_FileNames;
								#if 1
								//字符:m   ascii 码:0x64   182返回:0x0064
								//182发送文件名的时候总是会在0x0a前加上 0x0d,特奇怪,所以只能在接收端滤调这个0x0d <-------20150415
								//从128字节中筛出24个字节的文件名,滤除莫名出现在0X0A前的0X0D
								for(i = 0; i < USB_FileNameLength; i++)
								{
									if((*fileName == 0x0d)&&(*(fileName+1) == 0x0a))
									{
										fileName++;
									}
									getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileName[cmd->data_2-1][i] = *fileName;
									printf(" %02x ",*fileName);
									fileName++;
								}
								#else
								//从头取24字节作为文件名保存
								for(i = 0; i < 24; i++)
								{
									printf(" %02x ",getUSBControlSetting()->fileName[cmd->data_2-1][i]);
									getUSBControlSetting()->fileName[cmd->data_2-1][i] = USB_FileNames[i];
								}
								#endif
							}
							printf(" \r\n");
							#endif
							USB_RefleshFileNameFlag = 0;
							#endif
						}
						break;
						
					case 0x04: //set current file index
					case 0x05: //get current file index
						if(getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex != cmd->data_2)
						{
							getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex = cmd->data_2;
							//getUSBControlSetting()->fileChangeEn = 1;
							inputSetting.inputChanged[getUSBControlSetting()->operatBoard - 1] = 1;
							USB_RefleshFileNameFlag = 1;
						}
						break;
					case 0x07: //get current file total length
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex = cmd->data_2;
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileTotalLength = cmd->data_3 + (cmd->data_4<<8);
						break;
					case 0x08: //set file current time 
					case 0x09: //get file current time 
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex = cmd->data_2;
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileCurrentTime = cmd->data_3 + (cmd->data_4<<8);
						break;
					case 0x0a: //set play state
					case 0x0b: //get play state
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playState = cmd->data_2;
						break;
					case 0x0c: //set photo time
					case 0x0d: //get photo time
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].photoTime = cmd->data_2;
						break;
					case 0x0e: //set play mode
					case 0x0f: //get play mode
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playMode = cmd->data_2;
						//printf("getUSBControlSetting()->playMode = %d\r\n",getUSBControlSetting()->playMode);
						break; 
					case 0x10:
					case 0x11:
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].port = cmd->data_2;
						//printf("getUSBControlSetting()->port = %d\r\n",getUSBControlSetting()->port);
						break;
					case 0x13: //get play list
						#if 1
						{
							char buffer[257],blocknum = 0;
							int index;
							usbPlayListlen = (cmd->data_4<<8)+cmd->data_3;
							#if 0
							index = input_readBuffer(buffer, READ_INPUT_BLOCK_SIZE,200);
							if(index != READ_INPUT_BLOCK_SIZE)
							#else
							//字符:m   ascii 码:0x64   182返回:0x0064
							//182发送的时候总是会在0x0a前加上 0x0d,特奇怪
							//所以182发送的实际字节数,总是>= ((cmd->data_4<<8)+cmd->data_3), 只好在接收端多收一些数据,然后滤除0x0d 
							index = input_readBuffer(buffer,(READ_INPUT_BLOCK_SIZE*2),200);
							if(index < READ_INPUT_BLOCK_SIZE)
							#endif
							{								
								printf("<F:receive	%d data from input fail>", index);
								return;
							}
							
							#if 1
							#if 0 
							printf("\r\nlen =%d;blk=%d\r\n",usbPlayListlen,cmd->data_2);
							{
								int i;
								for(i = 0; i < (READ_INPUT_BLOCK_SIZE*2); i++)
								{
									printf(" %02x  ",buffer[i]);
								}
							}
							printf(" \r\n");
							#endif
							//滤除 0x0d							
							{
								int i;
								char *dataFilte = buffer;
								for(i = 0; i < READ_INPUT_BLOCK_SIZE; i++)
								{
									if((*dataFilte == 0x0d)&&(*(dataFilte+1) == 0x0a))
									{
										dataFilte++;
									}
									GetTimeControl()->timeControlBlock[i] = *dataFilte;
									dataFilte++;
								}
							}
							#if 1
							printf("\r\nafter filter:\r\n");
							{
								char i;
								for(i = 0; i < READ_INPUT_BLOCK_SIZE; i++)
								{
									printf(" %02x  ",GetTimeControl()->timeControlBlock[i]);
								}
							}
							printf(" \r\n");
							#endif
							#else
							printf("\r\nlen =%d;blk=%d\r\n",usbPlayListlen,cmd->data_2);
							{
								char i;
								for(i = 0; i < READ_INPUT_BLOCK_SIZE; i++)
								{
									printf(" %02x  ",buffer[i]);
									GetTimeControl()->timeControlBlock[i] = buffer[i];
								}
							}
							printf(" \r\n");
							#endif							
							GetTimeControl()->timeControlId = TIME_CONTROL_ID;	//added by @cc20140702
							GetTimeControl()->timeControlValid = 1;
							//GetTimeControl()->timeControlBlock = buffer;
							#if 1
							#if 0 //只保存一个USB播放列表
							blocknum = TIMECOTROL_USB1PLSYLISTMINBLOCK + cmd->data_2; //timeControl_getMaxBlocknum()+1;
							#else //最多允许保存3个USB的播放列表,每个USB最长允许设置32*128字节的播放列表
							blocknum = TIMECOTROL_USB1PLSYLISTMINBLOCK + (getUSBControlSetting()->operatBoard - 1)*TIMECOTROL_USBPLSYLIST_MAXPERPORT + cmd->data_2; //timeControl_getMaxBlocknum()+1;
							#endif
							saveTimeControl(blocknum);		//Block From Block2
							timeControl_setMaxBlocknum(blocknum);
							printf("usbpfile:blk=%d;\r\n",blocknum);	
							#endif
						}
						#else
						{
							int i,j;
							u8 value,blocknum=0,rxBuffer[129];
							BYTE checksum;
							//u32 len;
							u8 *pBlock;
							u32 startTime = GetSysTick();
							usbPlayListlen = (cmd->data_4<<8)+cmd->data_3;
							i=0;
							checksum =0;
							while(TRUE)
							{
								if(pipe_read(&receive_Input_pipe, &value))
								{
									rxBuffer[i++] = value;
									//printf(" %x",rxBuffer[i]);
										
								}
								if(i>128)
								{
									i =0;
									checksum =0;
									#if 1
									for(j=0;j<128;j++)
									{
										checksum += rxBuffer[j];
										//printf("%x ",rxBuffer[j]);
									}
									if(checksum !=rxBuffer[128])
									{
										printf("<F:receive 129 data from 182 fail>");
										printf("\r\nchecksum=%d;rxdatacheck=%d;\r\n",checksum,rxBuffer[128]);
										//break;		//commented by @cc20140707
									}
									#endif
									printf("\r\nlen =%d;blk=%d\r\n",usbPlayListlen,cmd->data_2);
									#if 0
									for(j=0;j<128;j++)
									{
										printf(" %x",rxBuffer[j]);
									}
									//blocknum++;
									#endif
									#if 1
									pBlock = &(GetTimeControl()->timeControlBlock[0]);
									for(j=0; j<128; j++)
									{
										pBlock[j] = rxBuffer[j];
									}
									GetTimeControl()->timeControlId = TIME_CONTROL_ID;	//added by @cc20140702
									GetTimeControl()->timeControlValid = 1;
									blocknum =timeControl_getMaxBlocknum()+1;
									saveTimeControl(blocknum);		//Block From Block2
									timeControl_setMaxBlocknum(blocknum);
									printf("usbpfile:blk=%d;\r\n",blocknum);
									#endif
									break;
									
								}
								
								if(isSysTickTimeout(startTime,3000))
								{
									printf("\r\n=UART TIME OUT=\r\n");
									break;
								}
							}
						}
						#endif
						break;
					case 0x15: //set play order
					case 0x16: //get play order
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playOrder = cmd->data_2;
						break;
					
				}
			}
			break;
		#endif
		
		case 0xff:
			inputSetting.regValue = cmd->data_4;
			break;
	}
	pipe_init(&receive_Input_pipe,receive_Input_buffer,sizeof(receive_Input_buffer));
#endif
}



static void interpret_Input_char(u8 value)
{
	
	static u8 index = 0;
	static char hexCharBuffer[2];
	
	if(value == '<')
	{
		state_Input = interpret_cmd_init;
	}
		
	switch(state_Input)
	{
		case interpret_cmd_init:
			if(value == '<')
			{
				index = 1;
				state_Input = interpret_cmd_start;
			}
			break;
		case interpret_cmd_start:		
			if(value == Input_cmd_header[index++])
			{
				if(index >= sizeof(Input_cmd_header))
				{
					state_Input = interpret_cmd_data;
					index = 0;
				}
			}
			else
			{
				state_Input = interpret_cmd_init;
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
				cmd_from_Input.address= HexCharToByte(hexCharBuffer);
				++index;
				break;
			case 3:
				hexCharBuffer[1] = value;
				cmd_from_Input.sn = HexCharToByte(hexCharBuffer);
				++index;
				break;	
			case 5:
				hexCharBuffer[1] = value;
				cmd_from_Input.command = HexCharToByte(hexCharBuffer);
				++index;
				break;			
			case 7:
				hexCharBuffer[1] = value;
				cmd_from_Input.data_1 = HexCharToByte(hexCharBuffer);
				++index;
				break;
			case 9:
				hexCharBuffer[1] = value;
				cmd_from_Input.data_2 = HexCharToByte(hexCharBuffer);
				++index;
				break;	
			case 11:
				hexCharBuffer[1] = value;
				cmd_from_Input.data_3 = HexCharToByte(hexCharBuffer);
				++index;
				break;
			case 13:
				hexCharBuffer[1] = value;
				cmd_from_Input.data_4 = HexCharToByte(hexCharBuffer);
				++index;
				break;
			case 15:
				// get checksum no use
				hexCharBuffer[1] = value;
				cmd_from_Input.checksum = HexCharToByte(hexCharBuffer);
				state_Input = interpret_cmd_end;
				index = 0;
				break;
			default:
				state_Input = interpret_cmd_init;
				break;
			}
			break;
		case interpret_cmd_end:
			if(value == Input_cmd_end[index])
			{
				++index;
				if(index >= sizeof(Input_cmd_end))
				{
					state_Input = interpret_cmd_init;
					// active cmd
					receive_Input_sn = cmd_from_Input.sn;
					//DebugPrintf("active=%d\r\n", receive_preamp_count);
					process_Input_cmd(&cmd_from_Input);
					index = 0;
				}
			}
			else
			{
				state_Input = interpret_cmd_init;
				//DebugPutString("preamp end header error\r\n");
				//DebugPrintf("end char = %x\r\n", value);
				//DebugPrintf("index = %d\r\n", index);
				//DebugPrintf("sizeof = %d\r\n", sizeof(preamp_cmd_end));
			}
			break;
		default:
			state_Input = interpret_cmd_init;
			break;
	}	
}

void task_Input(void)
{
	u8 value;
	#if 1
	while(pipe_read(&receive_Input_pipe,&value))
	{
		//USART_SendData(USART1, value);  // for test
		interpret_Input_char(value);
		if(state_Input == interpret_cmd_start)
		{
			startInputTime = GetSysTick();
		}
	}
	if((state_Input != interpret_cmd_init) && isSysTickTimeout(startInputTime,1000))
	{
		state_Input = interpret_cmd_init;
	}
	#else
	while(pipe_read(&receive_preamp_pipe,&value))
	{		
		uart0_putchar(value);
	}
	#endif
}

void Isr_receive_Input(u8 value)
{
#if 0
	pipe_write(&receive_Input_pipe, value);
#endif
}

static BYTE getCmdChecksum(Command_TypeDef * cmd)
{
	BYTE checksum;
	checksum = cmd->address+cmd->sn+cmd->command+cmd->data_1+cmd->data_2+cmd->data_3+cmd->data_4;
	return checksum;
}

void _send_cmd_Input(Command_TypeDef *cmdStruct)
{
#if 0
	char buffer[20];
	++send_Input_sn;
	cmdStruct->sn = send_Input_sn;
	cmdStruct->address = 0;
	
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(Input_UART_PORT, buffer);

#endif
}
void _send_cmd_Input_port(Command_TypeDef *cmdStruct)
{
	char buffer[20];
	++send_Input_sn;
	cmdStruct->sn = send_Input_sn;
	
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(Input_UART_PORT, buffer);
	//UARTSendStr(1, buffer);
}
void Input_sendBuffer(u8 *buffer, int size)
{
#if 0
	UARTSendBuffer(Input_UART_PORT,(char *)buffer,size);
#endif
}

bool send_cmd_to_Input_and_wait(Command_TypeDef *cmdStruct)
{
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
	_send_cmd_Input(cmdStruct);
	
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
	
	#if 0
	task_Input();
	
	
	// wait ack
	while((send_Input_sn != receive_Input_sn) && deviceIsConnet_Input && repeat)
	{
		task_Input();
		if(isSysTickTimeout(startTime,3000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from ext input board = %d,receive_sn: %d\r\n", send_Input_sn,receive_Input_sn);
			if(repeat)
			{
				printf("repeat: %d\r\n", repeat);
				UARTSendBuffer(Input_UART_PORT, buffer, 9);
				task_Input();
				startTime = GetSysTick();
				continue;
			}
			
			deviceIsConnet_Input = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_Input == FALSE)
	{
		while((send_Input_sn != receive_Input_sn))
		{
			task_Input();
			if(isSysTickTimeout(startTime,200))
			{
				printf("no ext input board = %d,receive_sn: %d\r\n", send_Input_sn,receive_Input_sn);
				deviceIsConnet_Input = 0;
				return FALSE;
			}
		}
	}
	#else
	task_Input();
	
	
	// wait ack
	while((send_Input_sn != receive_Input_sn) && deviceIsConnet_Input)
	{
		task_Input();
		if(isSysTickTimeout(startTime,2000))
		{
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from ext input board = %d,receive_sn: %d,", send_Input_sn,receive_Input_sn);
			printf("%x,%x,%x,%x,%x\r\n",cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			deviceIsConnet_Input = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_Input == FALSE)
	{
		while((send_Input_sn != receive_Input_sn))
		{
			task_Input();
			if(isSysTickTimeout(startTime,200))
			{
				printf("no ext input board = %d,receive_sn: %d,", send_Input_sn,receive_Input_sn);
				printf("%x,%x,%x,%x,%x\r\n",cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				deviceIsConnet_Input = 0;
				return FALSE;
			}
		}
	}
	#endif
	return TRUE;
}

bool send_cmd_to_Input_and_wait_port(Command_TypeDef *cmdStruct)
{
	char buffer[20],repeat = 3;
	u32 startTime;
	
	_send_cmd_Input_port(cmdStruct);
	
	startTime = GetSysTick();
	
	task_Input();	
	
	// wait ack
	while((send_Input_sn != receive_Input_sn) && deviceIsConnet_Input)
	{
		task_Input();
		if(isSysTickTimeout(startTime,2000))
		{
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from ext input board = %d,receive_sn: %d,", send_Input_sn,receive_Input_sn);
			printf("%x,%x,%x,%x,%x\r\n",cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			deviceIsConnet_Input = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_Input == FALSE)
	{
		while((send_Input_sn != receive_Input_sn))
		{
			task_Input();
			if(isSysTickTimeout(startTime,200))
			{
				printf("no ext input board = %d,receive_sn: %d,", send_Input_sn,receive_Input_sn);
				printf("%x,%x,%x,%x,%x\r\n",cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				deviceIsConnet_Input = 0;
				return FALSE;
			}
		}
	}
	return TRUE;
}

void send_cmd_to_Input(Command_TypeDef *cmdStruct)
{
	//char buffer[20],repeat = 3;
	//u32 startTime;
	
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
	_send_cmd_Input(cmdStruct);
	
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
	
}

void send_cmd_to_Input_Port(Command_TypeDef *cmdStruct)
{
	_send_cmd_Input_port(cmdStruct);
}

bool _wait_ack_Input(int timeout)
{
	u32 startTime;
	startTime = GetSysTick();

	// wait ack
	while((send_Input_sn != receive_Input_sn))
	{
		task_Input();
		if(isSysTickTimeout(startTime,timeout))
		{
			printf("can't receive ext input board = %d\r\n", send_Input_sn);
			deviceIsConnet_Input = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet_Input == FALSE)
	{
		return FALSE;
	}
	return TRUE;
	
}

void Input_send(int cmd, int dat1, int dat2, int dat3, int dat4)
{
	Command_TypeDef cmdStruct;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	send_cmd_to_Input(&cmdStruct);
}

bool Input_sendAndWait(int cmd, int dat1, int dat2, int dat3, int dat4)
{
	Command_TypeDef cmdStruct;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	return send_cmd_to_Input_and_wait(&cmdStruct);
}

bool Input_sendAndWait_Port(int port,int cmd, int dat1, int dat2, int dat3, int dat4)
{
	Command_TypeDef cmdStruct;
	cmdStruct.address = port;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	return send_cmd_to_Input_and_wait_port(&cmdStruct);
}
void Input_send_Port(int port,int cmd, int dat1, int dat2, int dat3, int dat4)
{
	Command_TypeDef cmdStruct;
	cmdStruct.address = port;
	cmdStruct.command = cmd;
	cmdStruct.data_1 = dat1;
	cmdStruct.data_2 = dat2;
	cmdStruct.data_3 = dat3;
	cmdStruct.data_4 = dat4;
	send_cmd_to_Input_Port(&cmdStruct);
}

int Input_goToBoot(void)
{
	return Input_sendAndWait(0x9a,0x20,0,0,0);
}
void Input_readDeviceType(void)
{
	Input_sendAndWait(0x68,0x01,0,0,0);
}
void Input_getSoftVersion(void)
{
	readFromMCUFlag_input = 1;
	Input_sendAndWait(0x6a,0x05,0,0,0);
	//Input_send(0x6a,0x05,0,0,0);
}
void Input_geHardVersion(void)
{
	readFromMCUFlag_input = 1;
	Input_sendAndWait(0x6a,0x07,0,0,0);
}
void Input_getMstVersion(char port) //port 0-mst1    1-mst2     2-mst3
{
	readFromMCUFlag_input = 1;
	Input_sendAndWait(0x6a,0x09,port,0,0);
}
int Input_sendProgramMcuBuffer(int address, u8 *buffer, int size)
{
	int ack=0;
	Input_send(0x9a, 0xa0, 0x00, address, address>>8);
	Input_sendBuffer(buffer, size);
	if(address == 0)
	{
		ack = _wait_ack_Input(10000);
	}
	else
	{
		ack = _wait_ack_Input(3000);
	}
	if(ack)
	{
		if((cmd_from_Input.data_3+(cmd_from_Input.data_4<<8)) == address)
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
		return FALSE;
	}
}

#if 0
char checkInputCropPre(char ch)
//void checkInputCropPre(char ch,PosSizeSetting *cropPre)
{
	char res = FALSE;
	//cropPre->posX = 0;
	//cropPre->posY = 0;

	//cropPre->sizeX = 0;
	//cropPre->sizeY = 0;
	
	if((inputSetting.extInput[ch].inputFormat != MD_NOINPUT)
	 &&(inputSetting.extInput[ch].inputType != 10))
	{
		res = TRUE;
		
		inputSetting.extInput[ch].cropPre.posX = 0;
		inputSetting.extInput[ch].cropPre.posY = 0;
		inputSetting.extInput[ch].cropPre.sizeX = 0;
		inputSetting.extInput[ch].cropPre.sizeY = 0;	
		
		if(inputSetting.extInput[ch].inputFormat != MD_UNDEFINE)
		{
			if(getInputSetting()->extInput[ch].inputType == 7) //CVBS板,需要参考后面的active信息
			{
				inputSetting.extInput[ch].cropPre.sizeX = getInputSetting()->extInput[ch].HActive;
				inputSetting.extInput[ch].cropPre.sizeY = getInputSetting()->extInput[ch].VActive;
			}
			else
			{
				if((getInputSetting()->extInput[ch].inputFormat == MD_1440X240)||(getInputSetting()->extInput[ch].inputFormat == MD_1440X288))
				{
					inputSetting.extInput[ch].cropPre.sizeX = getInputSetting()->extInput[ch].HActive;
					inputSetting.extInput[ch].cropPre.sizeY = GetVDEByVideoFormat(getInputSetting()->extInput[ch].inputFormat)<<1;
				}
				else
				{
					inputSetting.extInput[ch].cropPre.sizeX = GetHDEByVideoFormat(getInputSetting()->extInput[ch].inputFormat);
					inputSetting.extInput[ch].cropPre.sizeY = GetVDEByVideoFormat(getInputSetting()->extInput[ch].inputFormat);
				}
			}
		}
		else
		{
			inputSetting.extInput[ch].cropPre.sizeX = getInputSetting()->extInput[ch].HActive;
			inputSetting.extInput[ch].cropPre.sizeY = getInputSetting()->extInput[ch].VActive;
		}		
		if(inputSetting.extInput[ch].cropPre.sizeX <= 1920)
		{
			inputSetting.extInput[ch].cropPre.sizeX = 1920 - inputSetting.extInput[ch].cropPre.sizeX;
		}
		else
		{
			inputSetting.extInput[ch].cropPre.sizeX = 0;
		}
		if(inputSetting.extInput[ch].cropPre.sizeY <= 1080)
		{
			inputSetting.extInput[ch].cropPre.sizeY = 1080 - inputSetting.extInput[ch].cropPre.sizeY;
		}
		else
		{
			inputSetting.extInput[ch].cropPre.sizeY = 0;
		}
		printf("cropPre_width_%d,cropPre_height_%d\r\n",inputSetting.extInput[ch].cropPre.sizeX,inputSetting.extInput[ch].cropPre.sizeY);	
	}
	return res;
}
#endif

//static 
int ChangeExtInputFormatFlag = 0;
int getChangeExtInputFormatFlag(void)
{
	return ChangeExtInputFormatFlag;
}

int Input_GetVersion(void)
{
	if(getDeviceInfo()->extType == 0)
	{
		inputSetting.softVer= 0;
		return 0;
	}
	Input_getSoftVersion();
	return inputSetting.softVer;
}
u8 Input_GetHardVersion(void)
{
	Input_geHardVersion();
	return inputSetting.hardVer;
}
int Input_GetMstVersion(char port)//value: 0-through   0xff-no ext board    port: 0- ext7    1- ext8     2- ext9(只针对外扩板V1.3以上)   
{	
	#if 0
	Input_sendAndWait_Port(port+1,0x6a,0x03,0,0,0);
	//Input_send_Port(port+1,0x6a,0x03,0,0,0);
	#else
	if(getInputSetting()->hardVer < 3)
	{
		if(port > 1)
		{
			if((getInputSetting()->extInput[2].inputType != ExtInput_DVI)
			 &&(getInputSetting()->extInput[2].inputType != ExtInput_HDMI)
			 &&(getInputSetting()->extInput[2].inputType != ExtInput_SDI)
			 &&(getInputSetting()->extInput[2].inputType != ExtInput_D_DVI)
			 &&(getInputSetting()->extInput[2].inputType != ExtInput_DP))
			{
				return 0xff;
			}
			return 0;
		}
		Input_getMstVersion(port);
	}
	else
	{
		if(Input_isMstInput_V13(port) == FALSE)
		{
			if(getInputSetting()->extInput[port].inputType == ExtInput_NoBoard)
			{
				return 0xff;
			}
			return 0;
		}
		Input_getMstVersion(port);
	}
	#endif
	return inputSetting.mstVersion[port];
}
extern int ledHorseCnt;
void taskRefleshInputInfo(void)
{
	static u32 taskRefleshInputInfo_sysTime = 0;
	static int taskRefleshInputInfo_delay = 14000;
	static int extTypeLast = 0;		
	static char checkEDIDPortEnable = 1;
	static char checkInputBoardAudioEnable = 1;
	char i;
	
	if(refleshInputEnable == false)
	{
		return;
	}
#if 0
	if(!isSysTickTimeout(taskRefleshInputInfo_sysTime,taskRefleshInputInfo_delay))
	{
		return;
	}
	taskRefleshInputInfo_sysTime = GetSysTick();
	
	//menuProcessEvent(MENU_KEY_SD_EXHANCE_END);
	
	Input_sendAndWait(0xf1, 0x0a, 0x00, 0x00, 0x00);

	if(deviceIsConnet_Input == 0)
	{
		taskRefleshInputInfo_delay =  60000;
		inputSetting.extInput[0].inputType = 0;
		inputSetting.extInput[1].inputType = 0;
		inputSetting.extInput[2].inputType = 0;
	}
	else
	{
		taskRefleshInputInfo_delay =  2000;
	}
	{
		int extType = inputSetting.extInput[0].inputType + (inputSetting.extInput[1].inputType << 8) + (inputSetting.extInput[2].inputType << 16);

		if(extTypeLast != extType)
		{
			//printf("extTypeLast = %x,read_extType = %x---->%d\r\n",extTypeLast,extType,GetSysTick());
			extTypeLast = extType;
			checkInputBoardAudioEnable = 1;
			
			if((inputSetting.extInput[0].inputType == 10)&&(getUSBControlSetting()->operatBoard == 0)) //ext7 == USB
			{
				getUSBControlSetting()->operatBoard = 1;
				//printf("getUSBControlSetting()->operatBoard = 1---->%d\r\n",GetSysTick());
				//Input_getUSBFileNum(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
				//Input_getUSBPlayIndex(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
			}
			else if((inputSetting.extInput[1].inputType == 10)&&(getUSBControlSetting()->operatBoard == 0))//ext8 == usb && ext7 != usb
			{
				getUSBControlSetting()->operatBoard = 2;
				//Input_getUSBFileNum(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
				//Input_getUSBPlayIndex(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
				//printf("getUSBControlSetting()->operatBoard = 2---->%d\r\n",GetSysTick());
			}
			else if((inputSetting.extInput[2].inputType == 10)&&(getUSBControlSetting()->operatBoard == 0))//ext8 == usb && ext7 != usb
			{
				getUSBControlSetting()->operatBoard = 3;
				//Input_getUSBFileNum(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
				//Input_getUSBPlayIndex(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
				//printf("getUSBControlSetting()->operatBoard = 2---->%d\r\n",GetSysTick());
			}
		}
		else
		{
			if(checkInputBoardAudioEnable)
			{
				RefleshSDIIndexArray();
				
				checkInputBoardAudioEnable = 0;
				#if 1
				//check audio coard
				//if((inputSetting.extInput[2].inputType == ExtInput_D_HDMI)&&(GetUserSetting()->extInputConfigType[2] == 0))
				if(inputSetting.extInput[2].inputType == ExtInput_AUDIO)
				{
					//if(SwitchType == 0)
					{
						printf("enableInputBoardOutputAudio\r\n");
						SetSwitchType(1);
						enableInputBoardOutputAudio();
						if(getFrontType() == 2)
						{
							setMB103_HDMIPortOutputHDMI();
						}
						if(getInputSetting()->hardVer == 3)
						{
							refleshAudioInputEnable_total();//refleshAudioInputEnable(0);//
						}
						//SetDVIAndEXTAudio_notSave(HdmiAudio);
						//if((HdmiAudio == 1)&&(getInputSetting()->hardVer == 3))
						if(getInputSetting()->hardVer == 3)
						{
							SetAudioSwitch_ExtV13(getAudioInputConfig(GetUserSetting()->inputSource[CurrentWindow]));
						}
					}
				}
				else 
				{
					if(((GetHDMIoutIsHDMI(0) + GetHDMIoutIsHDMI(1)) == 0))//两个口都输出DVI格式,默认只切换视频
					{
						printf("disableInputBoardOutputAudio\r\n");
						#if 1
						SetSwitchType(0);
						disableInputBoardOutputAudio();
						if(getFrontType() == 2)
						{
							setMB103_HDMIPortOutputDVI();
						}
						#else	   //test audio
						SetSwitchType(1);
						enableInputBoardOutputAudio();
						if(getFrontType() == 2)
						{
							setMB103_HDMIPortOutputHDMI();
						}
						#endif
					}
				}
				#else
				//默认设置外扩板输入HDMI
				printf("enableInputBoardOutputAudio\r\n");
				SetSwitchType(1);
				enableInputBoardOutputAudio();
				if(getFrontType() == 2)
				{
					setMB103_HDMIPortOutputHDMI();
				}
				#endif
				
				//设置USB/CVBS备用端口
				printf("getDeviceInfo()->extInputPort:%d,%d,%d\r\n",getDeviceInfo()->extInputPort[0],getDeviceInfo()->extInputPort[1],getDeviceInfo()->extInputPort[2]);
				printf("extInput.inputType:%d,%d,%d\r\n",inputSetting.extInput[0].inputType,inputSetting.extInput[1].inputType,inputSetting.extInput[2].inputType);
				for(i = 0; i < 3; i++)
				{
					if((inputSetting.extInput[i].inputType == ExtInput_USB)&&(getDeviceInfo()->extInputPort[i] != 0))
					{
						printf("usb module ,and need to use USB_B\r\n");
						//Input_setUSBPort(getUSBControlSetting()->operatBoard,port);					
						Input_sendAndWait_Port(i+1,0x8a,0x10,getDeviceInfo()->extInputPort[i],0,0);
						getUSBControlSetting()->USBPortParam[i].port = getDeviceInfo()->extInputPort[i];
					}					
					else if((inputSetting.extInput[i].inputType == ExtInput_CV)&&(getDeviceInfo()->extInputPort[i] != 0))
					{
						printf("cv module ,and need to use CV2\r\n");
						Input_setCVBSPort(i,getDeviceInfo()->extInputPort[i]);
					}
				}
			}
			//printf("extTypeLast = read_extType = %x---->%d\r\n",extType,GetSysTick());
			if(extTypeLast != getDeviceInfo()->extType)
			{
				//printf("saveExtInputType---->%d\r\n",GetSysTick());
				saveExtInputType(extTypeLast);
				//checkEDIDDetinationList();
			}
			else if(checkEDIDPortEnable)
			{
				checkEDIDPortEnable = 0;
				//checkEDIDDetinationList();
			}

		}
	}
	if(deviceIsConnet_Input == 0)
	{
		return;
	}

	for(i = 0; i < MST6M182_PORT_SIZE; i++)
	{
		if((i == MST6M182_PORT_3)&&(inputSetting.hardVer < 3))
		{
			continue;
		}
		if(inputSetting.inputChanged[i])
		{
			int source = VSP1U_INPUT_EXT_7 + i;
			char setch,curch,cropEn = 0;
			//reflesh input info
			printf("input %d changed------>%d\r\n", i,GetSysTick());
			Input_sendAndWait(0xf1, 0x09, i, 0x20, 0x00); //get scale /crop /inputPort
			if(inputSetting.extInput[i].inputType == 10) //USB 
			{
				if(inputSetting.extInput[i].inputFormat == MD_NOINPUT)
				{
					char nameIndex,fileIndex;
					USB_ReadSuccessFlag[i] = 0; //should read usb params next time
					if(USB_ReConnectFlag[i] == 0)
					{
						printf("input %d no USB input\r\n",i);
						#if 0
						getUSBControlSetting()->fileNumber = 0;
						getUSBControlSetting()->playFileIndex = 0;
						for(fileIndex = 0; fileIndex < 40; fileIndex++)
						{
							for(nameIndex = 0; nameIndex < 25; nameIndex++)
							{
								getUSBControlSetting()->fileName[fileIndex][nameIndex] = 0;
							}
						}
						for(nameIndex = 0; nameIndex < 128; nameIndex++)
						{
							getUSBControlSetting()->playList[nameIndex] = 0;
						}
						#else
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber = 0;
						getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex = 0;
						for(fileIndex = 0; fileIndex < 40; fileIndex++)
						{
							for(nameIndex = 0; nameIndex < USB_FileNameLength; nameIndex++)
							{
								getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileName[fileIndex][nameIndex] = 0;
							}
						}
						#if 0
						for(nameIndex = 0; nameIndex < 128; nameIndex++)
						{
							getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playList[nameIndex] = 0;
						}
						#endif
						#endif
					}
					USB_ReConnectFlag[i] = 1;
				}
				else
				{
					if((USB_ReConnectFlag[i] == 1)&&(USB_ReadSuccessFlag[i] == 0))
					{
						#if 0
						char read_index,read_max;
						printf("new USB input_");
						Input_getUSBFileNum(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
						Input_getUSBPlayIndex(getUSBControlSetting()->operatBoard);//Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x01,0,0,0);
						read_max = (getUSBControlSetting()->fileNumber > 40)? 40 : getUSBControlSetting()->fileNumber;
						printf("read %d file names\r\n",read_max);
						
						for(read_index == 1; read_index <= read_max; read_index++) //这段循环不执行,原因不明 <-----2014.12.15
						{
							printf("Input_getUSBFileName(%d)\r\n",read_index);
							Input_getUSBFileName(getUSBControlSetting()->operatBoard,read_index); //Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x03,getUSBControlSetting()->playFileIndex,0,0);
						}
						#else
						if(ledHorseCnt == 0)
						{
							char buffer[40];
							menuEnterPage(MENU_INDEX_NOTHING);
							LCD_Clear();
							LCD_WriteLine1_L("");
							#if 1
							sprintf(buffer,"%sEXT_%d %s",langGetStr(readFilesFromUSBStr),(7+i),langGetStr(readFilesFromUSBStr2));
							LCD_WriteLine2(buffer);
							#else
							LCD_WriteLine2(langGetStr(readFilesFromUSBStr));
							#endif
							//dispLineCenter_L(1,langGetStr(readFilesFromUSBStr));	
							LCD_WriteLine3_L("");
							LCD_WriteLine4_L("");
							menuSetBusy(1,12);
						}
						#if 0
						Input_RefleshUSBParams(getUSBControlSetting()->operatBoard);	
						#else
						getUSBControlSetting()->operatBoard = (i + 1);
						Input_RefleshUSBParams(getUSBControlSetting()->operatBoard);		
						#endif
						if(ledHorseCnt == 0)
						{
							menuDisableBusy();	
							menuProcessEvent(MENU_KEY_INFO);
						}
						//returnToLastMenu();	  //如果之前正处于MENU_INDEX_NOTHING,会导致返回后黑屏			
						if(USB_ReadSuccessFlag[i] == 0)
						{
							return;
						}
						#endif
						
					}
					//Input_getUSBFileName(getUSBControlSetting()->operatBoard,getUSBControlSetting()->playFileIndex); //Input_send_Port(getUSBControlSetting()->operatBoard,0x8a,0x03,getUSBControlSetting()->playFileIndex,0,0);
					USB_ReConnectFlag[i] = 0;
				}
			}
			else if(inputSetting.extInput[i].inputType == 5) //vga 
			{
				Input_geteVgaAdjustPosX(i);
				Input_geteVgaAdjustPosY(i);
				Input_geteVgaAdjustClock(i);
				Input_geteVgaAdjustPhase(i);
			}
			else if((inputSetting.extInput[i].inputType == ExtInput_SDI)||(inputSetting.extInput[i].inputType == ExtInput_HDMI)) //HDMI  SDI
			{
				#if 0
				if(inputSetting.autoChangeOutputFormatEnable[i] != 0)
				{
					if((inputSetting.extInput[i].inputFormat == MD_720x480_60i)||(inputSetting.extInput[i].inputFormat == MD_720x576_50i))
					{
						if(inputSetting.extInput[i].outputFormat != MD_1280x720_60)
						{
							printf("port %d: input format = %d is SD,change output format to 720P\r\n",i,inputSetting.extInput[i].inputFormat);
							Input_setOutputFormat(i,MD_1280x720_60);						
							menuProcessEvent(MENU_KEY_SD_EXHANCE);
							taskRefleshInputInfo_delay = 16000;//SysDelay1ms(16000);
						}
					}
					else if(inputSetting.extInput[i].inputFormat != MD_NOINPUT)
					{
						if(inputSetting.extInput[i].outputFormat != MD_1920x1080_60)
						{
							printf("port %d: input format = %d is not SD,change output format to 1080P\r\n",i,inputSetting.extInput[i].inputFormat);
							Input_setOutputFormat(i,MD_1920x1080_60);
							menuProcessEvent(MENU_KEY_SD_EXHANCE);
							taskRefleshInputInfo_delay = 16000;//SysDelay1ms(16000);
						}
					}
				}
				#endif
			}

			inputSetting.inputChanged[i] = 0;
			//checkInputCropPre(i,&inputSetting.extInput[i].cropPre);
			#if 0
			if(checkInputCropPre(i) == FALSE)
			{
				ChangeExtInputFormatFlag = 3000;
				return;
			}
			GetUserSetting()->source[source].cropPre = getInputSetting()->extInput[i].cropPre;			
			#endif
			ChangeExtInputFormatFlag = 0;

			#if 0
			printf("source_%d,cropPre%d,%d,",source,GetUserSetting()->source[source].cropPre.sizeX,GetUserSetting()->source[source].cropPre.sizeY);
			printf("%d,%d\r\n",GetUserSetting()->source[source].cropPre.posX,GetUserSetting()->source[source].cropPre.posY);
			printf("living:%d,",GetUserSetting()->switchTo);
			printf("main:%d,",GetUserSetting()->inputSourceReadFromVideo[0]);
			printf("pip:%d\r\n",GetUserSetting()->inputSourceReadFromVideo[1]);
			if(!PipMode)
			{
				curch = GetUserSetting()->switchTo;
				setch = 1 - GetUserSetting()->switchTo;
				printf("current_%d,Setto_%d\r\n",curch,setch);
			}
			else
			{
				curch = CurrentWindow;
				setch = 1 - CurrentWindow;
				printf("PIPMode_current_%d,Setto_%d\r\n",curch,setch);
			}
			if(GetUserSetting()->inputSourceReadFromVideo[curch] == source)
			{
				printf("this port is living\r\n");

				CurrentCropDotPre = GetUserSetting()->source[source].cropPre;
				//crop_dot = GetUserSetting()->source[source].cropDot;
				printf("port%d,cropPre%d,%d,",curch,CurrentCropDotPre.sizeX,CurrentCropDotPre.sizeY);
				printf("%d,%d\r\n",CurrentCropDotPre.posX,CurrentCropDotPre.posY);
				ChangeExtInputFormatFlag = 0;
				mst_setCropDot(0, CurrentCropWidthDot);//mst_setCropDot(0, CurrentCropWidthDot + inputSetting.extInput[i].cropPre.sizeX);
				mst_setCropDot(1, CurrentCropHeightDot);//mst_setCropDot(1, CurrentCropHeightDot + inputSetting.extInput[i].cropPre.sizeY); 
				mst_setCropDot(2, CurrentCropXDot);//mst_setCropDot(2, CurrentCropXDot + inputSetting.extInput[i].cropPre.posX);
				mst_setCropDot(3, CurrentCropYDot);//mst_setCropDot(3, CurrentCropYDot + inputSetting.extInput[i].cropPre.posY);
				mst_setCropDotCh(curch);
				
			}
			else 
			{
				cropEn = 0x01;
			}
			if(GetUserSetting()->inputSourceReadFromVideo[setch] == source)
			{
				if(!PipMode)
				{

					char source_cur;
					printf("this port is waitting\r\n");
					source_cur = GetUserSetting()->inputSourceReadFromVideo[curch]; 
					GetUserSetting()->source[source_cur].cropDot = CurrentCropDot;
					GetUserSetting()->source[source_cur].cropParam = CurrentCropParam;
					GetUserSetting()->source[source_cur].cropPre = CurrentCropDotPre;
					
					CurrentCropDotPre = GetUserSetting()->source[source].cropPre;
					CurrentCropDot = GetUserSetting()->source[source].cropDot;
					printf("set port%d,cropPre%d,%d,",setch,CurrentCropDotPre.sizeX,CurrentCropDotPre.sizeY);
					printf("%d,%d\r\n",CurrentCropDotPre.posX,CurrentCropDotPre.posY);
					ChangeExtInputFormatFlag = 0;
					mst_setCropDot(0, CurrentCropWidthDot);//mst_setCropDot(0, CurrentCropWidthDot + inputSetting.extInput[i].cropPre.sizeX);
					mst_setCropDot(1, CurrentCropHeightDot);//mst_setCropDot(1, CurrentCropHeightDot + inputSetting.extInput[i].cropPre.sizeY); 
					mst_setCropDot(2, CurrentCropXDot);//mst_setCropDot(2, CurrentCropXDot + inputSetting.extInput[i].cropPre.posX);
					mst_setCropDot(3, CurrentCropYDot);//mst_setCropDot(3, CurrentCropYDot + inputSetting.extInput[i].cropPre.posY);
					mst_setCropDotCh(setch);
					
					CurrentCropDotPre = GetUserSetting()->source[source_cur].cropPre;
					CurrentCropDot = GetUserSetting()->source[source_cur].cropDot;
					mst_setCropDot(0, CurrentCropWidthDot);//mst_setCropDot(0, CurrentCropWidthDot + inputSetting.extInput[i].cropPre.sizeX);
					mst_setCropDot(1, CurrentCropHeightDot);//mst_setCropDot(1, CurrentCropHeightDot + inputSetting.extInput[i].cropPre.sizeY); 
					mst_setCropDot(2, CurrentCropXDot);//mst_setCropDot(2, CurrentCropXDot + inputSetting.extInput[i].cropPre.posX);
					mst_setCropDot(3, CurrentCropYDot);//mst_setCropDot(3, CurrentCropYDot + inputSetting.extInput[i].cropPre.posY);
				}
				else
				{
					char window = (CurrentWindow == MAIN_WINDOW)?PIP_WINDOW: MAIN_WINDOW;
					SetCurrentWindow(window);
					CurrentCropDotPre = GetUserSetting()->source[source].cropPre;
					CurrentCropDot = GetUserSetting()->source[source].cropDot;
					mst_setCropDot(0, CurrentCropWidthDot);//mst_setCropDot(0, CurrentCropWidthDot + inputSetting.extInput[i].cropPre.sizeX);
					mst_setCropDot(1, CurrentCropHeightDot);//mst_setCropDot(1, CurrentCropHeightDot + inputSetting.extInput[i].cropPre.sizeY); 
					mst_setCropDot(2, CurrentCropXDot);//mst_setCropDot(2, CurrentCropXDot + inputSetting.extInput[i].cropPre.posX);
					mst_setCropDot(3, CurrentCropYDot);//mst_setCropDot(3, CurrentCropYDot + inputSetting.extInput[i].cropPre.posY);
					mst_setCropDotCh(window);

					window = (CurrentWindow == MAIN_WINDOW)?PIP_WINDOW: MAIN_WINDOW;
					mst_setCropDot(0, CurrentCropWidthDot);//mst_setCropDot(0, CurrentCropWidthDot + inputSetting.extInput[i].cropPre.sizeX);
					mst_setCropDot(1, CurrentCropHeightDot);//mst_setCropDot(1, CurrentCropHeightDot + inputSetting.extInput[i].cropPre.sizeY); 
					mst_setCropDot(2, CurrentCropXDot);//mst_setCropDot(2, CurrentCropXDot + inputSetting.extInput[i].cropPre.posX);
					mst_setCropDot(3, CurrentCropYDot);//mst_setCropDot(3, CurrentCropYDot + inputSetting.extInput[i].cropPre.posY);
				}
					
			}
			else
			{
				cropEn |= 0x02;
			}
			if(cropEn == 0)
			{
				printf("this port is rest\r\n");
			}
			#endif
			
			//taskRefleshInputInfo_delay = 3000;
		}
	}	
#endif
}

void Input_readEDID(u8 port) //port 0-EXT7      1- EXT8          2- EXT 9
{
	if(port > 2)
	{
		return;
	}
	Input_sendAndWait(0xf1,0x0c,port,0,0);
}

void Input_writeEDID(u8 port,u8 *data,int len) //port 0-EXT7      1- EXT8          2- EXT 9
{
	if(port > 2)
	{
		return;
	}
	Input_send(0xF0,0x0C,port,len,(len>>8));
	Input_sendBuffer(data,len);
}


void Input_setUSBPort(char board,char port) //board 1- ext7  2- ext8    port 0-1
{
	if((port > 1)||(board > 3)||(board == 0))
	{
		return;
	}

	DisableRefleshInput();
	//printf("setUSBPort(%d,%d)\r\n",getUSBControlSetting()->operatBoard,port);
	Input_sendAndWait_Port(board,0x8a,0x10,port,0,0);
	
	SysDelay1ms(10000);
	Input_RefleshUSBParams(board);
}
char Input_getUSBPort(char board)    //board 1- ext7  2- ext8  3-ext9
{
#if 0
	Input_sendAndWait_Port(board,0x8a,0x11,0,0,0);
	#if 0
	return getUSBControlSetting()->port;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].port;	
	#endif
#endif
}
void Input_setUSBPlayMode(char board,char port,char mode) //board 1- ext7  2- ext8  3-ext9   mode 0-photo 1-video
{
#if 0
	if((mode > 1)||(board > 3)||(board == 0))
	{
		return;
	}
	
	//Input_sendAndWait_Port(board,0x5e,0,0x1d,mode,getUSBControlSetting()->port);
	DisableRefleshInput();
	Input_sendAndWait_Port(board,0x8a,0x0e,mode,port,0);
	
	SysDelay1ms(10000);
	Input_RefleshUSBParams(board);
	EnableRefleshInput();
#endif
}
char Input_getUSBPlayMode(char board,char port) //board 1- ext7  2- ext8    
{
#if 0
	Input_sendAndWait_Port(board,0x8a,0x0f,0,port,0);
	#if 0
	return getUSBControlSetting()->playMode;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playMode;
	#endif
#endif
}

void Input_setUSBPlayState(char board,char state) //board 1- ext7  2- ext8  3- ext9  state 0-play 1-pause
{
	if((state > 1)||(board > 3)||(board == 0))
	{
		return;
	}
	
	//Input_sendAndWait_Port(board,0x5e,0,0x1b,state,0);
	Input_sendAndWait_Port(board,0x8a,0x0a,state,0,0);
}
char Input_getUSBPlayState(char board) //board 1- ext7  2- ext8   
{
#if 0
	Input_sendAndWait_Port(board,0x8a,0x0b,0,0,0);
	#if 0
	return getUSBControlSetting()->playState;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playState;	
	#endif
#endif
}

void Input_setUSBPlayOrder(char board,char order) //board 1- ext7  2- ext8   
{
#if 0
	if((order >= PLAYORDER_SIZE)||(board > 2)||(board == 0))
	{
		return;
	}
	
	//Input_sendAndWait_Port(board,0x5e,0,0x1e,order,0);
	Input_sendAndWait_Port(board,0x8a,0x15,order,0,0);
	
	if(order == PLAYLIST)	//added by @cc20140618
	{	
		printf("\r\n=Play playlist=\r\n");
		
		{
			LCD_Clear();
			LCD_WriteLine1_L("");
			LCD_WriteLine2(langGetStr(readUSBPlayListStr));
			//dispLineCenter_L(1,langGetStr(readFilesFromUSBStr));	
			LCD_WriteLine3_L("");
			LCD_WriteLine4_L("");
			menuSetBusy(1,12);
		}
		
		//读播放列表
		timeControl_setCurBlocknum(0);
		usb_readPlayListAndSave(board);

		#if 0  //只保存一个USB播放列表
		timeControl_readBlock(TIMECOTROL_USB1PLSYLISTMINBLOCK);
		#else //最多允许保存3个USB的播放列表,每个USB最长允许设置32*128字节的播放列表		
		timeControl_readBlock(TIMECOTROL_USB1PLSYLISTMINBLOCK + (getUSBControlSetting()->operatBoard - 1)*TIMECOTROL_USBPLSYLIST_MAXPERPORT);
		#endif
		GetTimeControl()->enableTimeControl = 1;

		{
			menuDisableBusy();	
			menuProcessEvent(MENU_EVENT_ENTER);
		}
		
		//saveTimeControl(timeControl_getCurBlocknum()); //已在process_Input_cmd()中执行
	}
	else
	{
		GetTimeControl()->enableTimeControl = 0;
		saveTimeControl(timeControl_getCurBlocknum());
	}
#endif
}
char Input_getUSBPlayOrder(char board) //board 1- ext7  2- ext8   
{
#if 0
	Input_sendAndWait_Port(board,0x8a,0x16,0,0,0);
	#if 0
	return getUSBControlSetting()->playOrder;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playOrder;	
	#endif
#endif
}
char Input_getUSBFileNum(char board) //board 1- ext7  2- ext8
{
#if 0
	Input_sendAndWait_Port(board,0x8a,0x01,0,0,0);
	#if 0
	return getUSBControlSetting()->fileNumber;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber;
	#endif
#endif
}
void Input_getUSBFileName(char board,char index) //board 1- ext7  2- ext8
{
	Input_sendAndWait_Port(board,0x8a,0x03,index,0,0);
	//return getUSBControlSetting()->fileNumber;
}

void Input_setUSBPlayIndex(char board,char index) //board 1- ext7  2- ext8   
{
#if 0
	#if 0
	if((index > getUSBControlSetting()->fileNumber)||(board > 2)||(board == 0))
	#else
	if((index > getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber)||(board > 2)||(board == 0))
	#endif	
	{
		return;
	}
	USB_RefleshFileNameFlag = 1;
	Input_sendAndWait_Port(board,0x8a,0x04,index,0,0);
#endif
}
char Input_getUSBPlayIndex(char board) //board 1- ext7  2- ext8   
{
#if 0
	USB_RefleshFileNameFlag = 1;
	Input_sendAndWait_Port(board,0x8a,0x05,0,0,0);
	#if 0
	return getUSBControlSetting()->playFileIndex;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex;
	#endif
#endif
}
int Input_getUSBFileLength(char board) //board 1- ext7  2- ext8
{
#if 0
	Input_sendAndWait_Port(board,0x8a,0x07,0,0,0);
	#if 0
	return getUSBControlSetting()->fileTotalLength;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileTotalLength;
	#endif
#endif
}
void Input_setUSBPlayCurrentTime(char board,int time) //board 1- ext7  2- ext8   
{
#if 0
	#if 0
	if((time > getUSBControlSetting()->fileTotalLength)||(board > 2)||(board == 0))
	{
		return;
	}
	Input_sendAndWait_Port(board,0x8a,0x08,getUSBControlSetting()->playFileIndex,time,(time>>8));
	#else
	if((time > getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileTotalLength)||(board > 2)||(board == 0))
	{
		return;
	}
	Input_sendAndWait_Port(board,0x8a,0x08,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex,time,(time>>8));
	#endif
#endif
}
int Input_getUSBPlayCurrentTime(char board) //board 1- ext7  2- ext8   
{
#if 0
	#if 0
	Input_sendAndWait_Port(board,0x8a,0x09,getUSBControlSetting()->playFileIndex,0,0);
	return getUSBControlSetting()->fileCurrentTime;
	#else
	Input_sendAndWait_Port(board,0x8a,0x09,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playFileIndex,0,0);
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileCurrentTime;
	#endif
#endif
}
void Input_setUSBPhotoTime(char board,char time) //board 1- ext7  2- ext8   
{
	if((time > 100)||(board > 2)||(board == 0))
	{
		return;
	}
	Input_sendAndWait_Port(board,0x8a,0x0c,time,0,0);
}
char Input_getUSBPhotoTime(char board) //board 1- ext7  2- ext8   
{
#if 0
	Input_sendAndWait_Port(board,0x8a,0x0d,0,0,0);
	//printf("Input_getUSBPhotoTime() = %d\r\n",getUSBControlSetting()->photoTime);
	#if 0
	return getUSBControlSetting()->photoTime;
	#else
	return getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].photoTime;
	#endif	
#endif
}

void Input_getUSBPlayList(char board,char index) //board 1- ext7  2- ext8   
{
	Input_sendAndWait_Port(board,0x8a,0x13,index,0,0);
}
void usb_readPlayListAndSave(char board)	
{
#if 0
	u8 len=0,i;
	printf("readPlaylist_%d block 0;\r\n",board);
	Input_getUSBPlayList(board,0);
	SysDelay1ms(1000);	//added by @cc20140707
	len = getUsbPlaylistBlock();
	if(len>0)
	{
		timeControl_setMaxBlocknum(len);
		for(i=1;i<len;i++)
		{	
			printf("readPlaylist_%d block %d;\r\n",board,i);
			Input_getUSBPlayList(board,i);
			SysDelay1ms(1000);
		}
	}
#endif
}
static bool readingUSBParanmsFlag = 0;
bool getReadingUSBParanmsFlag(void)
{
	return readingUSBParanmsFlag;
}
//刷新USB状态,文件数,文件名,播放类型等
void Input_RefleshUSBParams(char board) //board 1- ext7  2- ext8
{
#if 0
	char i,fileNum,mst_port = (board - 1),res = TRUE;
	printf("Input_RefleshUSBParams = %d\r\n",board);
	readingUSBParanmsFlag = 1;
	Input_getUSBPort(board);
	if(deviceIsConnet_Input == 0)
	{
		USB_ReadSuccessFlag[mst_port] =  FALSE;
		return;
	}
	#if 0
	Input_getUSBPlayMode(board,getUSBControlSetting()->playMode);
	if(getUSBControlSetting()->playMode == 0)
	#else
	Input_getUSBPlayMode(board,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playMode);
	if(deviceIsConnet_Input == 0)
	{
		USB_ReadSuccessFlag[mst_port] =  FALSE;
		return;
	}
	if(getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playMode == 0)
	#endif
	{
		Input_getUSBPhotoTime(board);
		if(deviceIsConnet_Input == 0)
		{
			USB_ReadSuccessFlag[mst_port] =  FALSE;
			return;
		}
		Input_setUSBPlayState(board,1);
		if(deviceIsConnet_Input == 0)
		{
			USB_ReadSuccessFlag[mst_port] =  FALSE;
			return;
		}
	}
	Input_getUSBPlayState(board);
	if(deviceIsConnet_Input == 0)
	{
		USB_ReadSuccessFlag[mst_port] =  FALSE;
		return;
	}
	Input_getUSBFileNum(board);
	if(deviceIsConnet_Input == 0)
	{
		USB_ReadSuccessFlag[mst_port] =  FALSE;
		return;
	}
	Input_getUSBPlayIndex(board);
	#if 0
	fileNum = (getUSBControlSetting()->fileNumber <= 40)?getUSBControlSetting()->fileNumber : 40 ;
	if(getUSBControlSetting()->fileNumber != 0xff)
	#else
	fileNum = (getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber <= 40)?getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber : 40 ;
	if(deviceIsConnet_Input == 0)
	{
		USB_ReadSuccessFlag[mst_port] =  FALSE;
		return;
	}
	if(getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].fileNumber != 0xff)
	#endif		
	{
		for(i=1;i<=fileNum;i++)
		{
			Input_getUSBFileName(board,i); 
			if(deviceIsConnet_Input == 0)
			{
				USB_ReadSuccessFlag[mst_port] =  FALSE;
				return;
			}
			SysDelay1ms(1000);
			if(USB_ReadSuccess == 0)
			{
				Input_getUSBFileName(board,i); 
			}
		}
	}

	#if 0
	if(getUSBControlSetting()->playOrder == PLAYLIST)
	#else
	if(getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playOrder == PLAYLIST)
	#endif	
	{
		//刷新播放列表
		timeControl_setCurBlocknum(0);
		usb_readPlayListAndSave(board);
	}
	readingUSBParanmsFlag = 0;
	USB_ReadSuccessFlag[board - 1] = 1;
#endif
}

void Input_setLayerParam(BYTE board)
{
#if 0
	ExtInputSetting * inputSet;
	char buffer[32];

	if(board > 2)
	{
		return;
	}
	
	inputSet = &inputSetting.extInput[board];

	buffer[0] = inputSet->cropDot.posX;
	buffer[1] = inputSet->cropDot.posX>>8;
	buffer[2] = inputSet->cropDot.posY;
	buffer[3] = inputSet->cropDot.posY>>8;
	buffer[4] = inputSet->cropDot.sizeX;
	buffer[5] = inputSet->cropDot.sizeX>>8;
	buffer[6] = inputSet->cropDot.sizeY;
	buffer[7] = inputSet->cropDot.sizeY>>8;

	buffer[8] = inputSet->scale.posX;
	buffer[9] = inputSet->scale.posX>>8;
	buffer[10] = inputSet->scale.posY;
	buffer[11] = inputSet->scale.posY>>8;
	buffer[12] = inputSet->scale.sizeX;
	buffer[13] = inputSet->scale.sizeX>>8;
	buffer[14] = inputSet->scale.sizeY;
	buffer[15] = inputSet->scale.sizeY>>8;
	
	buffer[16] = inputSet->freeze;
	buffer[17] = inputSet->horizontalFlip;
	
	buffer[18] = inputSet->inputPort; 
	
	buffer[19] = inputSet->alpha;
	buffer[20] = inputSet->layerOnOff;
	
	buffer[21] = inputSet->brightness[0];
	buffer[22] = inputSet->brightness[1];
	buffer[23] = inputSet->brightness[2];
		
	buffer[24] = inputSet->contrast[0];
	buffer[25] = inputSet->contrast[1];
	buffer[26] = inputSet->contrast[2];
	buffer[27] = inputSet->sharpness;
	
	buffer[28] = inputSet->saturation;		
	buffer[29] = inputSet->colorTemp[0];	
	buffer[30] = inputSet->colorTemp[1];	
	buffer[31] = inputSet->colorTemp[2];	
	
	Input_send(0xF0,0x09,board,32,0);
	Input_sendBuffer(buffer,32);
#endif
}

void Input_setCVBSPort(char board,char port) //board 0- ext7  1- ext8   2- ext9         port 0-1
{
	if((port > 1)||(board > 2))
	{
		return;
	}

	printf("Input_setCVBSPort(%d,%d)\r\n",board, port);

	DisableRefleshInput();
	inputSetting.extInput[board].inputPort = port;
	Input_setLayerParam(board);
	EnableRefleshInput();
}

void Input_seteVgaAutoAdjust(char board)//board 0- ext7  1- ext8  
{
	if(board > 2)
	{
		return;
	}
	Input_sendAndWait(0x82,0x02,board,0,0);
}

void Input_seteVgaAdjustPosX(char board,int pos_x)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return;
	}
	dat2 = (board<<4);
	inputSetting.extVGAAdjust[board].pos_x = pos_x;
	Input_sendAndWait(0x82,0x00,dat2,pos_x,(pos_x>>8));
}

int Input_geteVgaAdjustPosX(char board)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return 0;
	}
	dat2 = (board<<4);
	Input_sendAndWait(0x82,0x01,dat2,0,0);
	return inputSetting.extVGAAdjust[board].pos_x;
}

void Input_seteVgaAdjustPosY(char board,int pos_y)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return;
	}
	dat2 = (board<<4)+ 0x01;
	inputSetting.extVGAAdjust[board].pos_y = pos_y;
	Input_sendAndWait(0x82,0x00,dat2,pos_y,(pos_y>>8));
}

int Input_geteVgaAdjustPosY(char board)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return 0;
	}
	dat2 = (board<<4)+ 0x01;
	Input_sendAndWait(0x82,0x01,dat2,0,0);
	return inputSetting.extVGAAdjust[board].pos_y;
}

void Input_seteVgaAdjustClock(char board,int clock)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return;
	}
	dat2 = (board<<4) + 0x02;
	inputSetting.extVGAAdjust[board].clock = clock;
	Input_sendAndWait(0x82,0x00,dat2,clock,(clock>>8));
}

int Input_geteVgaAdjustClock(char board)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return 0;
	}
	dat2 = (board<<4) + 0x02;
	Input_sendAndWait(0x82,0x01,dat2,0,0);
	return inputSetting.extVGAAdjust[board].clock;
}

void Input_seteVgaAdjustPhase(char board,int phase)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return;
	}
	dat2 = (board<<4) + 0x03;
	inputSetting.extVGAAdjust[board].phase = phase;
	Input_sendAndWait(0x82,0x00,dat2,phase,(phase>>8));
}

int Input_geteVgaAdjustPhase(char board)//board 0- ext7  1- ext8  
{
	BYTE dat2;
	if(board > 2)
	{
		return 0;
	}
	dat2 = (board<<4) + 0x03;
	Input_sendAndWait(0x82,0x01,dat2,0,0);
	return inputSetting.extVGAAdjust[board].phase;
}

BYTE Input_getOutputFormat(char board)//board 0- ext7  1- ext8  
{
	if(board > 2)
	{
		return 0;
	}
	Input_sendAndWait(0x74,0x03,board,0,0);
	return inputSetting.extInput[board].outputFormat;
}

void Input_setOutputFormat(char board,char format)//board 0- ext7  1- ext8  
{
	if((board > 2)||((format != MD_1280x720_60)&&(format != MD_1920x1080_60)))
	{
		return;
	}
	Input_sendAndWait(0x74,0x02,board,format,0);
}


#if 0  //test 
void Input_resetInputPort(char port)
{
	if(port > 2)
	{
		return;
	}
	Input_sendAndWait(0x73,0x04,port,0,0);
}

void Input_writePCA8574(char value)
{
	Input_sendAndWait(0x73,0x06,value,0,0);
}

char Input_readPCA8574(void)
{
	Input_sendAndWait(0x73,0x07,0,0,0);
	return inputSetting.regValue;
}

void Input_taskTest8574(void)
{
	static u32 taskTest8574_sysTime = 0;
	char i;
	
	if(!isSysTickTimeout(taskTest8574_sysTime,1000))
	{
		return;
	}
	taskTest8574_sysTime = GetSysTick();
	
	Input_readPCA8574();
}

#endif



void ResetExtInputSetting(void)
{
#if 0
	char i;
	int extType = inputSetting.extInput[0].inputType + (inputSetting.extInput[1].inputType << 8) + (inputSetting.extInput[2].inputType << 16);
	DisableRefleshInput();
	for(i = 0; i < MST6M182_PORT_SIZE; i++)
	{
		if(inputSetting.extInput[i].inputType ==  10) //usb
		{
			getUSBControlSetting()->operatBoard = i+1;
			
			#if 0
			getUSBControlSetting()->port = 0;   //portA
			getUSBControlSetting()->playMode = 1; //video
			getUSBControlSetting()->playState = 0; //play
			getUSBControlSetting()->playOrder = 0; //repeat all
			getUSBControlSetting()->photoTime = 5; 
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x10,getUSBControlSetting()->port,0,0);
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x0e,getUSBControlSetting()->playMode,getUSBControlSetting()->port,0);
			Input_setUSBPlayState(getUSBControlSetting()->operatBoard,getUSBControlSetting()->playState); //board 1- ext7  2- ext8	  state 0-play 1-pause
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x15,getUSBControlSetting()->playOrder,0,0);
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x0c,getUSBControlSetting()->photoTime,0,0);
			#else
			getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].port = 0;   //portA
			getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playMode = 1; //video
			getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playState = 0; //play
			getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playOrder = 0; //repeat all
			getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].photoTime = 5; 
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x10,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].port,0,0);
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x0e,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playMode,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].port,0);
			Input_setUSBPlayState(getUSBControlSetting()->operatBoard,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playState); //board 1- ext7  2- ext8	  state 0-play 1-pause
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x15,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playOrder,0,0);
			Input_sendAndWait_Port(getUSBControlSetting()->operatBoard,0x8a,0x0c,getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].photoTime,0,0);
			#endif
		}
		else if(inputSetting.extInput[i].inputType ==  7) //cv
		{
			inputSetting.extInput[i].inputPort = 0;
			Input_setLayerParam(i);
		}
	}
#endif
}

void setInputBoardOutputHDMIEnable(char port,bool enable)// port  0-182_1   1-182_2   2-ext_1   3-ext_2   4-ext_3
{
	Input_sendAndWait(0x86,0x0a,port,enable,0);
}

bool getInputBoardOutputHDMIEnable(char port)
{
	Input_sendAndWait(0x86,0x0a,port,0,0);
	return inputSetting.regValue;
}

void setInputBoardOutputEnableDataRange(char port,bool enable)// port  0-6 
{
	Input_sendAndWait(0x86,0x0e,port,enable,0);
}

bool getInputBoardOutputEnableDataRange(char port)
{
	Input_sendAndWait(0x86,0x0f,port,0,0);
	return inputSetting.regValue;
}


void Input_setOutputAudioEnable(u8 enable)
{

	if(enable > 1)
	{
		return;
	}
	Input_sendAndWait(0x74,0x00,0x00,enable,0);
}
void enableInputBoardOutputAudio(void)
{
	Input_setOutputAudioEnable(1);
}
void disableInputBoardOutputAudio(void)
{
	Input_setOutputAudioEnable(0);
}

void Input_setExtInputTypeRelocation(char port,char value)
{
	if(port > 2)
	{
		return;
	}
	Input_sendAndWait(0x73,0x08,port,value,0);
}
char Input_getExtInputTypeRelocation(char port)
{
	Input_sendAndWait(0x73,0x09,port,0,0);
	return inputSetting.regValue;
}
void relocationExtInputType_Audio(char port)
{
	if(port != 2)
	{
		return;
	}
	Input_setExtInputTypeRelocation(port,ExtInput_AUDIO);
}

void Input_setAutoChangeOutputFormatEnable(char port, bool enable)
{
	if(port > 1)
	{
		return;
	}
	inputSetting.autoChangeOutputFormatEnable[port] = enable;	
}
bool Input_getAutoChangeOutputFormatEnable(char port)
{
	return inputSetting.autoChangeOutputFormatEnable[port];	
}

void Input_setOutputAudioVolumn(char volumn)
{
	if(volumn > 100)
	{
		volumn = 100;
	}
	Input_sendAndWait(0x81,0x02,0,volumn,0);
}

char Input_getOutputAudioVolumn(void)
{
	Input_sendAndWait(0x81,0x03,0,0,0);
	return inputSetting.regValue;
}

void Input_setAudioSwitchInPort(char inPort) //inPort 0-2   3:预留,对应切换无音频
{
	if(inPort > 3)
	{
		inPort = 3;
	}
	Input_sendAndWait(0x81,0x04,0,inPort,0);
}

char Input_getAudioSwitchInPort(void)
{
	Input_sendAndWait(0x81,0x05,0,0,0);
	return inputSetting.regValue;
}

char Input_isMstInput_V13(char port)
{
	if((getInputSetting()->hardVer < 3)||(port > 2)||(getDeviceInfo()->extType == 0))
	{
		return FALSE;
	}
	switch(getInputSetting()->extInput[port].inputType)
	{
		case ExtInput_SDI:
		case ExtInput_VGA:
		case ExtInput_YPbPr:
		case ExtInput_CV :
		case ExtInput_SV :
		case ExtInput_D_HDMI:
		case ExtInput_USB:
			return TRUE;
			
		case ExtInput_NoBoard:
		case ExtInput_DVI:
		case ExtInput_HDMI:
		case ExtInput_AUDIO:
		case ExtInput_D_DVI:
		case ExtInput_DP :
			return FALSE;
	}
}

char Input_isDigitIputPort_V13(char port)
{
	if((getInputSetting()->hardVer < 3)||(port > 2)||(getDeviceInfo()->extType == 0))
	{
		return FALSE;
	}
	switch(getInputSetting()->extInput[port].inputType)
	{
		case ExtInput_SDI:
		case ExtInput_DVI:
		case ExtInput_HDMI:
		case ExtInput_D_DVI:
		case ExtInput_DP :
			return TRUE;
			
		case ExtInput_VGA:
		case ExtInput_YPbPr:
		case ExtInput_CV :
		case ExtInput_SV :
		case ExtInput_D_HDMI:
		case ExtInput_USB:
		case ExtInput_NoBoard:
		case ExtInput_AUDIO:
			return FALSE;
	}
}

char Input_isHDMIIputPort_V13(char port)
{
	if((getInputSetting()->hardVer < 3)||(port > 2)||(getDeviceInfo()->extType == 0))
	{
		return FALSE;
	}
	if(getInputSetting()->extInput[port].inputType == ExtInput_HDMI)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

char Input_selfCheckIC(int id)
{
	Input_sendAndWait(0xff,0x00,(id&0x00ff),(id>>8),0);
	return inputSetting.regValue;	
}





