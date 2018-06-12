#include <stdio.h>
#include "SysConfig.h"
#include "rgblinkProtocol.h"
#include "stdp9320.h"
#include "uart.h"
#include "pipe.h"
#include "systick.h"
#include "userpref.h"
#include "control.h"
#include "videoFormatTable.h"
#include "fpga.h"
#include "rs232command.h"

#ifdef USE_STDP9320

/* Private typedef -----------------------------------------------------------*/
#define STDP9320_UART_PORT 2

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//u8 receive_preamp_buffer[1024];


extern Pipe_t receive_preamp_pipe;
u8 receive_sn = 0;
u8 send_sn = 0;
VIDEO_Setting_TypeDef video_Setting;
static u8 deviceIsConnet = 1;
static bool forceCommFlag = false;
VideoDefine_Struct video_customVideo;  
Resolution_Struct video_customResolution;

char getValidFreqValue(int value);
	
/* Private function prototypes -----------------------------------------------*/
void ForceCommFlag(void)
{
	forceCommFlag = true;
}

VIDEO_Setting_TypeDef *getVideoSetting(void)
{
	return &video_Setting;
}

u8 getVideoStatus(void)
{
	return deviceIsConnet;
}



void init_stdp9320(void)
{
	if(!isSysTickTimeout(0,6000))
	{
		SysDelay1ms(6000-GetSysTick());
	}
//	pipe_init(&receive_preamp_pipe,receive_preamp_buffer,sizeof(receive_preamp_buffer));
	UARTInit(STDP9320_UART_PORT,9600); // 6m48
	video_Setting.sn_lsb = 0x34;
	video_Setting.sn_msb = 0x12;
	video_Setting.ver_lsb = 0x01;
	video_Setting.ver_msb = 0x00;

	video_Setting.inputFormat[0] = MD_NOINPUT;
	video_Setting.inputFormat[1] = MD_NOINPUT;
	video_Setting.inputSource[0] = INPUT_SIZE;
	video_Setting.inputSource[0] = INPUT_SIZE;
	
	//SysDelay1ms(1000);	
	
	#if 0 // 取消SN存到9320,改为只存在100M的eeprom
	video_getSn();
	#endif
	video_getSoftVersion();

#if (CURRENT_DEVICE == _VIP_SMART)
	//video_setInputSource(INPUT_DVI);
	video_setInputSource_ch(0,INPUT_HDMI);
	video_setInputSource_ch(1,INPUT_DVI);

	//video_setInputSource_Main(INPUT_DVI);	
	//video_setInputSource_PIP(INPUT_HDMI);

	//video_setBrightness(206,206,206);	
	video_setBrightness(FALSE,220,220,220);
#endif	

	printf("init_stdp9320------>%d\r\n",GetSysTick());
}

const char video_cmd_header[] = {'<','F'};
const char video_cmd_end[] = {'>'};

static Command_TypeDef cmd_from_video;
/*
 *	 ":TOVC02000000828004010007\r\n" = 27
 *     ":FRVC0600828004010007\r\n" = 23
 */
static InterpretCmdState_TypeDef state = interpret_cmd_init;

const BYTE SourceVideoToDevice[INPUT_SIZE] = 
{
	VIPDRIVE_INPUT_SIZE,//	INPUT_NO_USE_0 , // 0,
	VIPDRIVE_INPUT_SIZE,//	INPUT_NO_USE_1 , // 1,
	VIPDRIVE_INPUT_SIZE,//	INPUT_NO_USE_2 , // 2,
	VIPDRIVE_INPUT_DP,//	INPUT_VGA , 	 // 3,
	VIPDRIVE_INPUT_DVI,//	INPUT_DVI , 	 // 4,	   //MATRIX_OUTPUT_CORE_DVI
	VIPDRIVE_INPUT_SIZE,//	INPUT_YCbCr,	 // 5,
	VIPDRIVE_INPUT_DVI,//	INPUT_TTL,		 // 6,	  //MATRIX_OUTPUT_CORE_TTL
	VIPDRIVE_INPUT_HDMI,//	INPUT_HDMI, 	 // 7,	   //X1_Pro_HDMI
	VIPDRIVE_INPUT_SIZE,//	INPUT_CV1,		 //  8,
	VIPDRIVE_INPUT_SIZE,//	INPUT_CV2,		 //  9,
	VIPDRIVE_INPUT_DP,//	INPUT_DP,		 // 10,
	VIPDRIVE_INPUT_DP,//	INPUT_DP2,		 //  11,
	VIPDRIVE_INPUT_SIZE,//	INPUT_YCbCr2,	 //  12,
	VIPDRIVE_INPUT_DVI,//	INPUT_TTL2, 	 //  13,   
	VIPDRIVE_INPUT_SIZE,//	INPUT_SV,		 //  14,
};


BYTE getSourceToDevice(BYTE source)
{
	if(source >= INPUT_SIZE)
	{
		return VIPDRIVE_INPUT_SIZE;
	}
	return SourceVideoToDevice[source];
}

#if 0
static BYTE getCmdChecksum(Command_TypeDef * cmd)
{
	BYTE checksum;
	checksum = cmd->address+cmd->sn+cmd->command+cmd->data_1+cmd->data_2+cmd->data_3+cmd->data_4;
	return checksum;
}
#endif
int mst_readInputSourceCh(char ch);


int valueFromVideo;

int ReceiveBuffer_Video(char *buffer, int size, int timeout)
{
	u32 startTime;
	int i = 0;
	
	startTime = GetSysTick();
	
	while(1)
	{
		BYTE value;
		if(pipe_read(&receive_preamp_pipe, &value))
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

static void process_video_cmd(Command_TypeDef * cmd)
{
	deviceIsConnet = 1;
	
	//printf("process_mst6m48_cmd:%x,%x,%d,%d,%d---->%d\r\n",cmd->command,cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,GetSysTick());
	
	switch(cmd->command)
	{
		case 0x68:
			switch(cmd->data_1)
			{
				case 0x10:
				case 0x11:
					video_Setting.enableDisplay = cmd->data_2;
					break;
				case 0x13:
					video_Setting.deviceState = cmd->data_2;
					break;
				case 0x20:
					video_Setting.updateState = cmd->data_2;
					break;
				default:
					//printf("..mst_enableDisplay:%x,%x\n",cmd->data_1, cmd->data_2);
					break;
			}
			break;
		case 0x6a:
			switch(cmd->data_1)
			{
				case 0x01:
					video_Setting.ver_msb = cmd->data_2;
					//video_Setting.ver_lsb = cmd->data_3;
					video_Setting.ver_lsb = (cmd->data_3*10)+cmd->data_4;
					break;	
					#if 0  // 取消SN存到9320,改为只存在100M的eeprom
				case 0x10:
				case 0x11:
					// read write sn
					video_Setting.sn_msb = cmd->data_2;
					video_Setting.sn_lsb = cmd->data_3;
					if(getDeviceSN() != ((video_Setting.sn_msb<<8)+video_Setting.sn_lsb))
					{
						saveDeviceSN((video_Setting.sn_msb<<8)+video_Setting.sn_lsb);
					}
					break;
					#endif
			}
			break;
		case 0x6B:
			switch(cmd->data_1)
			{
				case 0x01:
					GetUserSetting()->pipMode = cmd->data_2;
					break;
				case 0x03:
					CurrentWindow = cmd->data_2;
					video_Setting.regValue = cmd->data_2;
					break;
				case 0x04:
				case 0x05:
					GetUserSetting()->swapMainPip = cmd->data_2;
					video_Setting.regValue = cmd->data_2;
					break;
				case 0x06:
				case 0x07:
					GetUserSetting()->swapMainPip = cmd->data_2;
					break;
                default:
                    break;
			}
			break;
		case 0x6c:
			video_Setting.regValue = cmd->data_4+(cmd->data_3<<8);
			break;

			#if !(CURRENT_DEVICE == _VIP_SMART)		// modify by hibernate for complie
			case 0x50:
			case 0x51:
				// scale x position width
				GetUserSetting()->outputBoard[outProgram].panel[GetUserSetting()->currentWindow].sizeX = cmd->data_2 + (cmd->data_1<<8);
				GetUserSetting()->outputBoard[outProgram].panel[GetUserSetting()->currentWindow].posX  = cmd->data_4 + (cmd->data_3<<8);		
				break;
			case 0x52:
			case 0x53:
				// scale y position height
				GetUserSetting()->outputBoard[outProgram].panel[GetUserSetting()->currentWindow].sizeY = cmd->data_2 + (cmd->data_1<<8);
				GetUserSetting()->outputBoard[outProgram].panel[GetUserSetting()->currentWindow].posY  = cmd->data_4 + (cmd->data_3<<8);		
				break;
			#else
		case 0x50:
			// scale x position width
			CurrentScaleWidth= cmd->data_2 + (cmd->data_1<<8);
			CurrentScaleX  = cmd->data_4 + (cmd->data_3<<8);
		
			break;		
		case 0x51:
			// scale x position width
			CurrentScaleWidth= cmd->data_2 + (cmd->data_1<<8);
			CurrentScaleX  = cmd->data_4 + (cmd->data_3<<8);
		
			break;
		case 0x52:
			// scale y position height
			CurrentScaleHeight = cmd->data_2 + (cmd->data_1<<8);
			CurrentScaleY = cmd->data_4 + (cmd->data_3<<8);
			break;		
		case 0x53:
			// scale y position height
			CurrentScaleHeight = cmd->data_2 + (cmd->data_1<<8);
			CurrentScaleY = cmd->data_4 + (cmd->data_3<<8);		
			break;
			#endif
		#if 0
		case 0x54:
		//case 0x55:
			// zoom
			CurrentZoomX = cmd->data_1;
			CurrentZoomY = cmd->data_2;
			CurrentZoomWidth = cmd->data_3;
			CurrentZoomHeight = cmd->data_4;
			break;
		case 0x55:
			CurrentZoomX = cmd->data_1;
			CurrentZoomY = cmd->data_2;
			CurrentZoomWidth = cmd->data_3;
			CurrentZoomHeight = cmd->data_4;		
			video_Setting.zoom[IMAGE_X] = cmd->data_1;
			video_Setting.zoom[IMAGE_Y] = cmd->data_2;
			video_Setting.zoom[IMAGE_WIDTH] = cmd->data_3;
			video_Setting.zoom[IMAGE_HEIGHT] = cmd->data_4;
			break;
		case 0x56:
		//case 0x57:
			// crop
			CurrentCropX = cmd->data_1;
			CurrentCropY = cmd->data_2;
			CurrentCropWidth = cmd->data_3;
			CurrentCropHeight = cmd->data_4;
			break;
		case 0x57:
			CurrentCropX = cmd->data_1;
			CurrentCropY = cmd->data_2;
			CurrentCropWidth = cmd->data_3;
			CurrentCropHeight = cmd->data_4;
			video_Setting.crop[IMAGE_X] = cmd->data_1;
			video_Setting.crop[IMAGE_Y] = cmd->data_2;
			video_Setting.crop[IMAGE_WIDTH] = cmd->data_3;
			video_Setting.crop[IMAGE_HEIGHT] = cmd->data_4;
			break;
		
		case 0x5c:
			//if((cmd->data_1 > 0)&&(cmd->data_1 < 4))
			if(cmd->data_1 < 4)
			{
				int value = cmd->data_2 + (cmd->data_3<<8);
				switch(cmd->data_1)
				{
					case 0x00:
						CurrentCropWidthDot = value - CurrentCropWidthDotPre;
						break;
					case 0x01:
						CurrentCropHeightDot = value - CurrentCropHeightDotPre;
						break;
					case 0x02:
						CurrentCropXDot = value - CurrentCropXDotPre;
						break;
					case 0x03:
						CurrentCropYDot = value - CurrentCropYDotPre;
						break;
				}
			}
			break;

		case 0x5d:
			video_Setting.cropDot[cmd->data_1] =	cmd->data_2 + (cmd->data_3<<8);
			break;
			#endif

		case 0x6E:	//CropDot
			switch(cmd->data_1)
			{
				case 0x01:
					video_Setting.regValue = cmd->data_4 + (cmd->data_3<<8);
					break;
				case 0x10:
				case 0x11:
					video_Setting.regValue = cmd->data_4 + (cmd->data_3<<8);
					#if !(CURRENT_DEVICE == _VIP_SMART)		// modify by hibernate for complie
					switch(cmd->data_2) //0- width  1- height  2- x   3- y 
					{
						case 0x00:							
							//GetUserSetting()->outputBoard[outProgram].inputWidth  = video_Setting.regValue;
							GetUserSetting()->outputBoard[outProgram].input.sizeX = video_Setting.regValue;
							break;
						case 0x01:
							//GetUserSetting()->outputBoard[outProgram].inputHeight = video_Setting.regValue;
							GetUserSetting()->outputBoard[outProgram].input.sizeY = video_Setting.regValue;
							break;
						case 0x02:
							//GetUserSetting()->outputBoard[outProgram].inputPosX   = video_Setting.regValue;
							GetUserSetting()->outputBoard[outProgram].input.posX = video_Setting.regValue;
							break;
						case 0x03:
							//GetUserSetting()->outputBoard[outProgram].inputPosX   = video_Setting.regValue;
							GetUserSetting()->outputBoard[outProgram].input.posY = video_Setting.regValue;
							break;
					}
					#endif
					break;
			}
			break;
			
		case 0x72:
			switch(cmd->data_1)
			{
				case 0x00:
				case 0x01:
					// 读写输入源			
					if(cmd->data_3 == 13 ) //DIP1
					{
						cmd->data_3 = 6; 
					}				
					//GetUserSetting()->inputSource[CurrentWindow] = cmd->data_3;
					video_Setting.inputSource[0]	= cmd->data_3;
					//video_Setting.inputSource[CurrentWindow]	= cmd->data_3;
					video_Setting.regValue = cmd->data_3;
					break;
				case 0x13:
					video_Setting.regValue = cmd->data_2;
					break;
				case 0x15: //读色空间
					valueFromVideo = cmd->data_2;
					break;
				case 0x1e:
				case 0x1f: //读通道信号源
					video_Setting.regValue = cmd->data_3;
				#if !(CURRENT_DEVICE == _VIP_SMART)			// no use in smart
					if(cmd->data_3 == INPUT_TTL2)
					{
						cmd->data_3 = INPUT_TTL;
					}
				#endif
					video_Setting.chSource[cmd->data_2&0x01] = cmd->data_3;
					break;			
				case 0x21: 
					video_Setting.regValue = cmd->data_2;
					break;
				case 0x17:
					video_Setting.regValue = cmd->data_2;
					break;
			} 
			break;
		case 0x73:
			switch(cmd->data_1)
			{
				case 0x01:
					// 读输入源信号格式
					/*
					if(video_Setting.inputFormat[CurrentWindow] != cmd->data_2)
					{
						video_Setting.inputFormat[CurrentWindow] = cmd->data_2;
					}
					*/
					//video_Setting.inputFormat[CurrentWindow] = cmd->data_2;
					video_Setting.regValue = cmd->data_2;
					break;
				case 0x03:
					// 读输入源timming
					//video_Setting.inputTimming[cmd->data_2&0x03] = cmd->data_4+(cmd->data_3<<8);
					{
						int value = cmd->data_4+(cmd->data_3<<8);
						switch(cmd->data_2)
						{
							case 0x00:
								video_Setting.inputTimming[video_Setting.currentCh].HStart = value;
								break;
							case 0x01:
								video_Setting.inputTimming[video_Setting.currentCh].VStart = value;
								break;
							case 0x02:
								video_Setting.inputTimming[video_Setting.currentCh].HActive = value;
								break;
							case 0x03:
								video_Setting.inputTimming[video_Setting.currentCh].VActive = value;
								break;
						}						
					}
					break;
				case 0x07:
					video_Setting.regValue = cmd->data_4+(cmd->data_3<<8);
					break;

				case 0x09:
					video_Setting.regValue = cmd->data_2; 
					break;
					
				case 0x0b:
					video_Setting.inputFormat[cmd->data_2] = cmd->data_3;
					video_Setting.regValue = cmd->data_3; 
					break;
				case 0x21:
					video_Setting.regValue = cmd->data_2; 
					//video_Setting.DVIInputMode = cmd->data_2; 
					//GetUserPref()->dviInputConfig = video_Setting.DVIInputMode;
					break;
				case 0x23:
					video_Setting.regValue = cmd->data_2; 
					break;
				case 0x25:
					video_Setting.regValue = cmd->data_3; 
					break;			
			}
			break;
		case 0x74:
			switch(cmd->data_1)
			{
				case 0x00:
					// 读写输出信号格式
					//GetUserSetting()->outputFormat = cmd->data_2;
					//break;
				case 0x01:
					video_Setting.regValue = cmd->data_2;
					video_Setting.outputFormat = cmd->data_2;
					break;
				case 0x07:
					video_Setting.frameLockMode = cmd->data_2;
					break;
				case 0x09:
					video_Setting.frameLockSource = cmd->data_2;
					break;
				case 0x0a:
				case 0x0b:
					//video_Setting.regValue = cmd->data_2;
					video_Setting.splitOnOff = cmd->data_2;
					break;
				case 0x0c:
				case 0x0d:
					{
						int value = (cmd->data_3) + (cmd->data_4<<8);
					switch(cmd->data_2)
					{
							case 0x00: //htotal
								video_Setting.splitHTotal = value;
								break;
							case 0x01: //vtotal
								video_Setting.splitVTotal = value;
								break;
							case 0x02: //hsize
								video_Setting.splitHSize = value;
								break;
							case 0x03: //vsize
								video_Setting.splitVSize = value;
								break;
							case 0x04: //hpos
								video_Setting.splitHPos = value;
								break;
							case 0x05: //vpos
								video_Setting.splitVPos = value;
								break;
					case 6:
						video_Setting.vLockLine = cmd->data_3+(cmd->data_4<<8);
						break;

							default:
								break;							
						}
					}
					break;
				case 0x0f:
					video_Setting.outputColorSpace = cmd->data_2;
					break;
				case 0x13:
					switch(cmd->data_2)
					{
						case 0:
							//mst6m48_customVideo.htotal = cmd->data_3+(cmd->data_4<<8);//video_Setting.
							//break;
						case 1:
							//mst6m48_customVideo.vtotal = cmd->data_3+(cmd->data_4<<8);
							//break;
						case 2:
							//mst6m48_customVideo.hstart = cmd->data_3+(cmd->data_4<<8);
							//break;
						case 3:
							//mst6m48_customVideo.vstart = cmd->data_3+(cmd->data_4<<8);
							{
								int value;
								WORD *p_int = &video_customVideo.htotal;
								//value = cmd->data_3+(cmd->data_4<<8);
								value = (cmd->data_3<<8)+cmd->data_4;
								*(p_int + cmd->data_2) = value;
							}
							break;
						case 4:
							//mst6m48_customVideo.vfreq = cmd->data_3;
							//break;
						case 5:
							//mst6m48_customVideo.hSyncWidth = cmd->data_3;
							//break;
						case 6:
							//mst6m48_customVideo.vSyncWidth = cmd->data_3;
							//break;
						case 7:
							//mst6m48_customVideo.hvSyncPolarity = cmd->data_3;
							{
								u8 value;
								BYTE *p_char = &video_customVideo.vfreq;
								value = cmd->data_3;

								*(p_char + cmd->data_2-4) = value;
							}
							break;
						case 8:
							//mst6m48_customResolution.DisplayWidth = cmd->data_3+(cmd->data_4<<8);
							//break;
						case 9:
							//mst6m48_customResolution.DisplayHeight = cmd->data_3+(cmd->data_4<<8);
							{
								int value;
								WORD *p_int = &video_customResolution.DisplayWidth;
								value = cmd->data_3+(cmd->data_4<<8);
								*(p_int + cmd->data_2-8) = value;
							}
							break;
							
					}
					break;
				case 0x14:
				case 0x15:
					video_Setting.regValue = cmd->data_3+(cmd->data_4<<8);
					break;
				case 0x23:
					video_Setting.regValue = cmd->data_2; 
					break;
				case 0x25:
					video_Setting.regValue = cmd->data_2; 
					break;
				case 0x27:
				case 0x29:
				case 0x2b:
					video_Setting.regValue = cmd->data_3+(cmd->data_2<<8);
					break;	
				case 0x2d:
					video_Setting.boardWidth = cmd->data_4;
					video_Setting.regValue = cmd->data_4;
					break;
				case 0x2f:
					video_Setting.boardHeight = cmd->data_4;
					video_Setting.regValue = cmd->data_4;
					break;
				case 0x31:
					video_Setting.boardColor = cmd->data_4;
					video_Setting.regValue = cmd->data_4; 
					break;
				case 0x33:
					video_Setting.boardOnOff = cmd->data_4;
					video_Setting.boardHighLight = cmd->data_3;
					video_Setting.regValue = cmd->data_3;
					break;
				case 0x39:
					GetUserSetting()->pc_x = (cmd->data_2<<8)+cmd->data_3;
					break;
				case 0x3b:
					GetUserSetting()->pc_y = (cmd->data_2<<8)+cmd->data_3;
					break;
				case 0x3d:
					GetUserSetting()->pc_clock = (cmd->data_2<<8)+cmd->data_3;
					break;
				case 0x3f:
					GetUserSetting()->pc_phase = (cmd->data_2<<8)+cmd->data_3;
					break;
				
				case 0x43:
					break;
			}
			break;
		case 0x76:
			switch(cmd->data_1)
			{
				case 0:
				case 1:
					FadeAlpha = cmd->data_2;
					break;
				case 3:
					FadeTime = cmd->data_2+(cmd->data_3<<8);
					video_Setting.regValue = cmd->data_2+(cmd->data_3<<8);
					break;
				case 5:
					video_Setting.regValue = cmd->data_2+(cmd->data_3<<8);
					break;
				case 9:
					video_Setting.switchMode = cmd->data_2;
					video_Setting.regValue = cmd->data_2;//SwitchMode
				case 0x0b:
					SwitchTo = cmd->data_2;
					video_Setting.switchTo = cmd->data_2;
					break;
				case 0x0d:
					video_Setting.currentCh = cmd->data_2;
					break;
			}
			break;
		case 0x77:
			switch(cmd->data_1)
			{
			case 1:
				video_Setting.regValue = cmd->data_2;//GetUserSetting()->gamma
				break;
			}
			break;
		case 0x79:
			switch(cmd->data_1)
			{
			case 0x01:
				//GetUserSetting()->displayMode[CurrentWindow] = cmd->data_2;
				GetUserSetting()->displayMode[0] = cmd->data_2;
				GetUserSetting()->displayMode[1] = cmd->data_2;
				break;
			case 0x0c:
			case 0x0d:
				video_Setting.regValue = cmd->data_2;
				break;
			}
			break;
		case 0x7c:
			switch(cmd->data_1)
			{
			case 0x03:
				video_Setting.regValue = cmd->data_2;
				break;
			}		
			break;
		case 0x7d:
			switch(cmd->data_1)
			{
			case 0x01:
				video_Setting.regValue = cmd->data_2;
				break;
			}
			break;
		case 0x7f:
			switch(cmd->data_1)
			{
			case 0x01:
				video_Setting.regValue = cmd->data_2;
				break;			
			}
			break;
		case 0x80:
			switch(cmd->data_1)
			{
			case 0x01:
			//case 0x01:
				// 读写亮度
				CurBrightnessR = cmd->data_2;
				CurBrightnessG = cmd->data_3;
				CurBrightnessB = cmd->data_4;
				break;
			case 0x03:
				video_Setting.brightness[GetUserSetting()->currentWindow][COLOR_R] = cmd->data_2;
				video_Setting.brightness[GetUserSetting()->currentWindow][COLOR_G] = cmd->data_2;
				video_Setting.brightness[GetUserSetting()->currentWindow][COLOR_B] = cmd->data_2;
				video_Setting.regValue = cmd->data_2;
				break;
			//case 0x02:
			case 0x00:
				//读写对比度
				CurContrastR = cmd->data_2;
				CurContrastG = cmd->data_3;
				CurContrastB = cmd->data_4;
				video_Setting.regValue = (cmd->data_2<<16)+(cmd->data_3<<8)+ cmd->data_4;
				break;
			//case 0x04:
			case 0x05:
				//读写色温//20110906
				//CurColorTempR = cmd->data_2;//((int)cmd->data_2*100/255);
				//CurColorTempG = cmd->data_3;//((int)cmd->data_3*100/255);
				//CurColorTempB = cmd->data_4;//((int)cmd->data_4*100/255);
				//video_Setting.regValue = (cmd->data_2<<16)+(cmd->data_3<<8)+ cmd->data_4;
				break;
			//case	0x06:
			case  0x07:
				//读写锐度//20110906
				CurSharpness =cmd->data_2;
				video_Setting.regValue = cmd->data_2;
				break;
			//case	0x08:
			case  0x09:
				//读写饱和度//20110909
				CurSaturation =cmd->data_2;
				video_Setting.regValue = cmd->data_2;
				break;
			case 0x0d:	//色温模式
				video_Setting.regValue = cmd->data_2;
				break;

			case 0x0f:
			case 0x11:
				video_Setting.regValue = (cmd->data_2<<16)+(cmd->data_3<<8)+ cmd->data_4;
				break;
			case 0x13:	//均匀度补偿开关
			case 0x15:	//视频增强开关
				video_Setting.regValue = cmd->data_2;
				break;		
				
			}
			break;
		case 0x81:
			switch(cmd->data_1)
			{
			case 0x00:
			case 0x01:
				// 读写音频输入
				GetUserSetting()->audioInputSource = cmd->data_2;
				break;
			case 0x02:
			case 0x03:
				//读写音频切换模式
				GetUserSetting()->audioMode = cmd->data_2;
				break;
			}
			break;
		case 0x82:
			switch(cmd->data_1)
			{
			case 0x00:
			case 0x01:
				//读写VGA信号输入参数
				switch(cmd->data_2)
				{
				case 0x00:
					GetUserSetting()->pc_x = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x01:
					GetUserSetting()->pc_y = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x02:
					GetUserSetting()->pc_clock = cmd->data_3 + (cmd->data_4<<8);
					break;
				case 0x03:
					GetUserSetting()->pc_phase = cmd->data_3 + (cmd->data_4<<8);
					break;
				}
				break;
			}
			break;
		case 0x83:
			switch(cmd->data_1)
			{
				case 0x01:
				case 0x03:
				case 0x05:
				case 0x07:
				case 0x09:
				case 0x0b:
				case 0x0d:
				case 0x0f:
				case 0x11:
				case 0x13:
					video_Setting.regValue = cmd->data_2<<8 | cmd->data_3;
					break;
				case 0x21:
				case 0x23:
				case 0x25:
				case 0x27:
				case 0x29:
				case 0x2b:
					video_Setting.regValue = (cmd->data_2<<16) |(cmd->data_3 <<8) | cmd->data_4;
					break;			
			}
		case 0xF2:
			switch(cmd->data_1)
			{
				case 0x1:
					video_Setting.regValue = cmd->data_2;
					break;
			}
			break;
			
		case 0xf9:
			switch(cmd->data_1)
			{
				case 0x01:
					{
						char receiveBuffer[300],len;
						if(cmd->data_3 == 0)   //因为9320还会回一条data3是0的帧,不带任何数据
						{
							break;
						}
						len = cmd->data_3 + 4;
						if(ReceiveBuffer_Video(receiveBuffer, len, 200) == len)
						//ReceiveBuffer_Mst(receiveBuffer, 20, 200);
						{
							char *UData = &receiveBuffer[4];
							#if 0
							printf("receive input param from core sucess,len = %d,value:",len);
							{
								char i = 0;
								for(i = 0; i < len; i++)
								{
									printf("%x,",receiveBuffer[i]);
								}
							}
							printf("\r\n");
							#endif
							#if(DISP_TEST_INFO)
							//printf("receive core:%x,%x,%x,%x,%x,%x--->%d\r\n",(receiveBuffer[6]-1),(receiveBuffer[7]-1),(receiveBuffer[8]-1),(receiveBuffer[9]-1),(receiveBuffer[10]-1),(receiveBuffer[11]-1),GetSysTick());
							#endif
							//前6字节是8028的帧头

							#if 0
							video_Setting.chSource[MAIN_WINDOW] = receiveBuffer[6]-1; 
							video_Setting.chSource[PIP_WINDOW] = receiveBuffer[7]-1;
							#if 1
							if(video_Setting.chSource[MAIN_WINDOW] == INPUT_TTL2)
							{
								video_Setting.chSource[MAIN_WINDOW] = INPUT_TTL; 
							}
							if(video_Setting.chSource[PIP_WINDOW] == INPUT_TTL2)
							{
								video_Setting.chSource[PIP_WINDOW] = INPUT_TTL; 
							}
							#endif
							video_Setting.inputFormat[MAIN_WINDOW] = receiveBuffer[8]-1;
							video_Setting.inputFormat[PIP_WINDOW] = receiveBuffer[9]-1;
							video_Setting.currentCh = receiveBuffer[10]-1;
							video_Setting.displayMode = receiveBuffer[11]-1;
							#else
							video_Setting.chSource[MAIN_WINDOW] = *UData; 
							video_Setting.chSource[PIP_WINDOW] = *(UData + 1);
							#if !(CURRENT_DEVICE == _VIP_SMART)
							if(video_Setting.chSource[MAIN_WINDOW] == INPUT_TTL2)
							{
								video_Setting.chSource[MAIN_WINDOW] = INPUT_TTL; 
							}
							if(video_Setting.chSource[PIP_WINDOW] == INPUT_TTL2)
							{
								video_Setting.chSource[PIP_WINDOW] = INPUT_TTL; 
							}
							#endif
							video_Setting.inputFormat[MAIN_WINDOW] = *(UData + 2);
							video_Setting.inputFormat[PIP_WINDOW] = *(UData + 3);
							video_Setting.currentCh = *(UData + 4);
							video_Setting.displayMode = *(UData + 5);

							if(cmd->data_3 == 6)  //V1.41及以前的版本只返回上述信息
							{
								
							}
							else if(cmd->data_3 == 18)   //V1.42及以上会增加返回输入active信息
							{
								video_Setting.inputTimming[MAIN_WINDOW].HActive = ((*(UData + 6))) + (*(UData + 7)<<8);
								video_Setting.inputTimming[PIP_WINDOW].HActive  = ((*(UData + 8))) + (*(UData + 9)<<8);
								video_Setting.inputTimming[MAIN_WINDOW].VActive = ((*(UData + 10))) + (*(UData + 11)<<8);
								video_Setting.inputTimming[PIP_WINDOW].VActive  = ((*(UData + 12))) + (*(UData + 13)<<8);
								video_Setting.inputTimming[MAIN_WINDOW].VFreq   = getValidFreqValue(((*(UData + 14) )) + (*(UData + 15)<<8));
								video_Setting.inputTimming[PIP_WINDOW].VFreq    = getValidFreqValue(((*(UData + 16))) + (*(UData + 17)<<8));
								#if 0
								printf("inputTimming: %d,%d,%d,%d,%d,%d\r\n",
									video_Setting.inputTimming[MAIN_WINDOW].HActive,video_Setting.inputTimming[PIP_WINDOW].HActive,
									video_Setting.inputTimming[MAIN_WINDOW].VActive,video_Setting.inputTimming[PIP_WINDOW].VActive,
									video_Setting.inputTimming[MAIN_WINDOW].VFreq,video_Setting.inputTimming[PIP_WINDOW].VFreq);
								#endif
							}
							else if(cmd->data_3 == 30)  //V1.64及以上,ACTIVE信息分四个字节发送
							{
								char * rec;
								int i,dataBuffer[150];

								rec = UData + 6;
								len = 6;//len>>2;
								for(i = 0;i < len;i++)
								{
									dataBuffer[i] = ((*(rec + (4*i))) - 1) + ((*(rec + (4*i) + 1) - 1)<<4)
												  + ((*(rec + (4*i) + 2) - 1)<<8) + ((*(rec + (4*i) + 3) - 1)<<12);
								}
								video_Setting.inputTimming[MAIN_WINDOW].HActive = dataBuffer[0];
								video_Setting.inputTimming[PIP_WINDOW].HActive  = dataBuffer[1];
								video_Setting.inputTimming[MAIN_WINDOW].VActive = dataBuffer[2];
								video_Setting.inputTimming[PIP_WINDOW].VActive  = dataBuffer[3];
								video_Setting.inputTimming[MAIN_WINDOW].VFreq   = getValidFreqValue(dataBuffer[4]);
								video_Setting.inputTimming[PIP_WINDOW].VFreq    = getValidFreqValue(dataBuffer[5]);
								#if 0
								printf("inputTimming: %d,%d,%d,%d,%d,%d\r\n",
									video_Setting.inputTimming[MAIN_WINDOW].HActive,video_Setting.inputTimming[PIP_WINDOW].HActive,
									video_Setting.inputTimming[MAIN_WINDOW].VActive,video_Setting.inputTimming[PIP_WINDOW].VActive,
									video_Setting.inputTimming[MAIN_WINDOW].VFreq,video_Setting.inputTimming[PIP_WINDOW].VFreq);
							#endif
							}							
							#endif
							
							GetUserSetting()->inputSourceReadFromVideo[0] = video_readInputSourceCh(0);
							GetUserSetting()->inputSourceReadFromVideo[1] = video_readInputSourceCh(1);
							GetUserSetting()->displayMode[0] = video_Setting.displayMode;
						}
					}					
					break;
				case 0x02: //scale
					{
						int len = (cmd->data_4<<8) + cmd->data_3,len1,i,dataBuffer[150];
						char receiveBuffer[300];
						char *rec;						
						if(len == 0)   //因为9320还会回一条data3是0的帧,不带任何数据
						{
							break;
						}
						len1 = len + 4;
						//printf("read scale,len = %d\r\n",len);
						if(ReceiveBuffer_Video(receiveBuffer, len1, 200) == len1)
						{
							#if 0
							printf("receive scale from core sucess,len1 = %d,value:\r\n",len1);
							{
								for(i = 0; i < len1; i++)
								{
									printf("%x,",receiveBuffer[i]);
									if((i%16) == 15)
									{
										printf("\r\n");
									}
								}
							}
							printf("\r\n");
							#endif
							#if 1
							rec = receiveBuffer + 4;
							len = len>>1;
							for(i = 0; i < len; i++)
							{
								dataBuffer[i] = ((*(rec + (2*i)))) + ((*(rec + (2*i) + 1) )<<8);
							}
							video_Setting.scaler[MAIN_WINDOW][0] = dataBuffer[0];  // 0-x 1-y 2-width 3-height
							video_Setting.scaler[MAIN_WINDOW][1] = dataBuffer[1];  
							video_Setting.scaler[MAIN_WINDOW][2] = dataBuffer[2]; 
							video_Setting.scaler[MAIN_WINDOW][3] = dataBuffer[3];  
							
							video_Setting.scaler[PIP_WINDOW][0]  = dataBuffer[4];  
							video_Setting.scaler[PIP_WINDOW][1]  = dataBuffer[5];  
							video_Setting.scaler[PIP_WINDOW][2]  = dataBuffer[6];  
							video_Setting.scaler[PIP_WINDOW][3]  = dataBuffer[7];  

						#if !(CURRENT_DEVICE == _VIP_SMART)
							GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].posX  = video_Setting.scaler[MAIN_WINDOW][0];
							GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].posY  = video_Setting.scaler[MAIN_WINDOW][1];
							GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].sizeX = video_Setting.scaler[MAIN_WINDOW][2];
							GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].sizeY = video_Setting.scaler[MAIN_WINDOW][3];
							
							GetUserSetting()->outputBoard[outProgram].panel[PIP_WINDOW].posX  = video_Setting.scaler[PIP_WINDOW][0];
							GetUserSetting()->outputBoard[outProgram].panel[PIP_WINDOW].posY  = video_Setting.scaler[PIP_WINDOW][1];
							GetUserSetting()->outputBoard[outProgram].panel[PIP_WINDOW].sizeX = video_Setting.scaler[PIP_WINDOW][2];
							GetUserSetting()->outputBoard[outProgram].panel[PIP_WINDOW].sizeY = video_Setting.scaler[PIP_WINDOW][3];
						#endif
							#if 0
							printf("receive data:\r\n");
							{
								for(i = 0; i < len; i++)
								{
									printf("%d,",dataBuffer[i]);
									if((i%16) == 15)
									{
										printf("\r\n");
									}
								}
							}
							printf("\r\n");

							printf("GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].posX  = %d\r\n",GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].posX);
							printf("GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].posY  = %d\r\n",GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].posY);
							printf("GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].sizeX  = %d\r\n",GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].sizeX);
							printf("GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].sizeY  = %d\r\n",GetUserSetting()->outputBoard[outProgram].panel[MAIN_WINDOW].sizeY);
							#endif
							#endif
						}					
					}
					break;
				case 0x03: //picture
					{
						int len = (cmd->data_4<<8) + cmd->data_3,len1,i,dataBuffer[150];
						char receiveBuffer[300];
						char *rec;						
						if(len == 0)   //因为9320还会回一条data3是0的帧,不带任何数据
						{
							break;
						}
						len1 = len + 4;
						//printf("read picture,len = %d\r\n",len);
						if(ReceiveBuffer_Video(receiveBuffer, len1, 200) == len1)
						{
							#if 0
							printf("receive scale from core sucess,len1 = %d,value:\r\n",len1);
							{
								for(i = 0; i < len1; i++)
								{
									printf("%x,",receiveBuffer[i]);
									if((i%16) == 15)
									{
										printf("\r\n");
									}
								}
							}
							printf("\r\n");
							#endif
							#if 1
							rec = receiveBuffer + 4;
							len = len;
							for(i = 0; i < len; i++)
							{
								dataBuffer[i] = (*(rec + i)) ;
							}
							
							video_Setting.brightness[MAIN_WINDOW][Red]  = dataBuffer[0];  
							video_Setting.brightness[MAIN_WINDOW][Green]= dataBuffer[1];  
							video_Setting.brightness[MAIN_WINDOW][Blue] = dataBuffer[2]; 							
							video_Setting.contrast[MAIN_WINDOW][Red]    = dataBuffer[3];  							
							video_Setting.contrast[MAIN_WINDOW][Green]  = dataBuffer[4];  
							video_Setting.contrast[MAIN_WINDOW][Blue]   = dataBuffer[5]; 							
							video_Setting.colorTemp[MAIN_WINDOW][Red]   = dataBuffer[6];  							
							video_Setting.colorTemp[MAIN_WINDOW][Green] = dataBuffer[7];  
							video_Setting.colorTemp[MAIN_WINDOW][Blue]  = dataBuffer[8]; 							
							video_Setting.sharpness[MAIN_WINDOW]        = dataBuffer[9];  
							video_Setting.saturation[MAIN_WINDOW]       = dataBuffer[10]; 
							
							video_Setting.brightness[PIP_WINDOW][Red]   = dataBuffer[11];  
							video_Setting.brightness[PIP_WINDOW][Green] = dataBuffer[12];  
							video_Setting.brightness[PIP_WINDOW][Blue]  = dataBuffer[13]; 							
							video_Setting.contrast[PIP_WINDOW][Red]     = dataBuffer[14];  							
							video_Setting.contrast[PIP_WINDOW][Green]   = dataBuffer[15];  
							video_Setting.contrast[PIP_WINDOW][Blue]    = dataBuffer[16]; 							
							video_Setting.colorTemp[PIP_WINDOW][Red]    = dataBuffer[17];  							
							video_Setting.colorTemp[PIP_WINDOW][Green]  = dataBuffer[18];  
							video_Setting.colorTemp[PIP_WINDOW][Blue]   = dataBuffer[19]; 							
							video_Setting.sharpness[PIP_WINDOW]         = dataBuffer[20];  
							video_Setting.saturation[PIP_WINDOW]        = dataBuffer[21]; 
				
							#if 0
							printf("receive data:\r\n");
							{
								for(i = 0; i < len; i++)
								{
									printf("%d,",dataBuffer[i]);
									if((i%16) == 15)
									{
										printf("\r\n");
									}
								}
							}
							printf("\r\n");
							#endif
							#endif
						}					
					}
					break;
				case 0x0c: //DP  EDID
					{
						int len = (cmd->data_4<<8) + cmd->data_3,len1,i;
						char receiveBuffer[300];
						char *rec;
						if(len == 0)   //因为9320还会回一条data3是0的帧,不带任何数据
						{
							break;
						}
						len = len + 4;
						//printf("read edid,len = %d\r\n",len);
						#if 1
						#if 1
						if(ReceiveBuffer_Video(receiveBuffer, len, 2000) == len)
						#else
						len1 = ReceiveBuffer_Video(receiveBuffer, len, 2000);
						printf("ReceiveBuffer_Video() = %d\r\n",len1);
						if(len == len1)
						#endif
						{
							#if 0
							printf("receive edid from core sucess,len = %d,value:\r\n",len);
							{
								for(i = 0; i < len; i++)
								{
									printf("%x,",receiveBuffer[i]);
									if((i%16) == 15)
									{
										printf("\r\n");
									}
								}
							}
							printf("\r\n");
							#endif
							
							rec = receiveBuffer + 6;
							switch(cmd->data_2)
							{
								case 0x00:
									for(i = 0; i < 128; i++)
									{
										video_Setting.DP_EDID[i] = (*(rec + i)) ;
									}
									#if 1
									printf("receive edid 0-128:\r\n");
									{
										for(i = 0; i < 128; i++)
										{
											printf("%02x,",video_Setting.DP_EDID[i]);
											if((i%16) == 15)
											{
												printf("\r\n");
											}
										}
									}
									printf("\r\n");
									#endif
									break;
									
								case 0x01:
									for(i = 0; i < 128; i++)
									{
										video_Setting.DP_EDID[128 + i] = (*(rec + i));
									}
									#if 1
									printf("receive edid 128-256:\r\n");
									{
										for(i = 128; i < 256; i++)
										{
											printf("%02x,",video_Setting.DP_EDID[i]);
											if((i%16) == 15)
											{
												printf("\r\n");
											}
										}
									}
									printf("\r\n");
									#endif
									break;
							}
						}
						#endif
					}
					break;
			}
			break;
			
		default:
			break;
	}
}

char getValidFreqValue(int value)
{
	char i,res = 0,isBegin = 0;
	int level[10] = {1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};

	//printf("getValidFreqValue(%d) ",value);
	for(i = 0; i < 10; i++)
	{
		if((value > level[i])&&(isBegin == 0))
		{
			isBegin = 1;
			res = (value/level[i]);
			value = (value%level[i]);
		}
		else if(isBegin == 1)
		{
			res *= 10;
			res += (value/level[i]);
			value = (value%level[i]);
		}
		if(res >= 10)
		{
			break;
		}
	}
	//printf("= %d\r\n",res);
	return res;
}

static void interpret_video_char(u8 value)
{
	
	static u8 index = 0;
	static char hexCharBuffer[2];
	
	if(value == '<')
	{
		state = interpret_cmd_init;
	}
	
	switch(state)
	{
	case interpret_cmd_init:
		if(value == '<')
		{
			index = 1;
			state = interpret_cmd_start;
		}
		break;
	case interpret_cmd_start:		
		if(value == video_cmd_header[index++])
		{
			if(index >= sizeof(video_cmd_header))
			{
				state = interpret_cmd_data;
				index = 0;
			}
		}
		else
		{
			state = interpret_cmd_init;
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
			cmd_from_video.address= HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 3:
			hexCharBuffer[1] = value;
			cmd_from_video.sn = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 5:
			hexCharBuffer[1] = value;
			cmd_from_video.command = HexCharToByte(hexCharBuffer);
			++index;
			break;			
		case 7:
			hexCharBuffer[1] = value;
			cmd_from_video.data_1 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 9:
			hexCharBuffer[1] = value;
			cmd_from_video.data_2 = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 11:
			hexCharBuffer[1] = value;
			cmd_from_video.data_3 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 13:
			hexCharBuffer[1] = value;
			cmd_from_video.data_4 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 15:
			// get checksum no use
			hexCharBuffer[1] = value;
			cmd_from_video.checksum = HexCharToByte(hexCharBuffer);
			state = interpret_cmd_end;
			index = 0;
			break;
		default:
			state = interpret_cmd_init;
			break;
		}
		break;
	case interpret_cmd_end:
		if(value == video_cmd_end[index])
		{
			++index;
			if(index >= sizeof(video_cmd_end))
			{
				state = interpret_cmd_init;
				// active cmd
				receive_sn = cmd_from_video.sn;
				//DebugPrintf("active=%d\r\n", receive_preamp_count);
				process_video_cmd(&cmd_from_video);
				index = 0;
			}
		}
		else
		{
			state = interpret_cmd_init;
			//DebugPutString("preamp end header error\r\n");
			//DebugPrintf("end char = %x\r\n", value);
			//DebugPrintf("index = %d\r\n", index);
			//DebugPrintf("sizeof = %d\r\n", sizeof(preamp_cmd_end));
			
		}
		break;
	default:
		state = interpret_cmd_init;
	}
	
}

static u32 startPreampTime;
void task_video(void)
{
	u8 value;
	#if 1
	while(pipe_read(&receive_preamp_pipe,&value))
	{
		//UARTSendByte(0, value);  // for test
		interpret_video_char(value);
		if(state == interpret_cmd_start)
		{
			startPreampTime = GetSysTick();
		}
	}
	if((state != interpret_cmd_init) && isSysTickTimeout(startPreampTime,1000))
	{
		state = interpret_cmd_init;
	}
	#else
	while(pipe_read(&receive_preamp_pipe,&value))
	{		
		uart0_putchar(value);
	}
	#endif
}

void Isr_receive_video(u8 value)
{
	pipe_write(&receive_preamp_pipe, value);
}


bool canSendCmd(void)
{
	if(receive_sn <= send_sn)
	{
		if((send_sn-receive_sn)<5)
		{
			return TRUE;
		}
	}
	else
	{
		if((send_sn + (256-receive_sn)) < 5)
		{
			return TRUE;			
		}
	}

    return FALSE;
}

static bool delayVideoCommand = FALSE;
Command_TypeDef cmdStruct_Delay[8];
u8 cmd_delay_num = 0;
static u32 taskDelayVideoCmd_sysTime = 0;

void pushVideoCommand(Command_TypeDef * cmd)
{
    if(cmd_delay_num < 8)
    {
        cmdStruct_Delay[cmd_delay_num].command = cmd->command;
        cmdStruct_Delay[cmd_delay_num].data_1 = cmd->data_1;     
        cmdStruct_Delay[cmd_delay_num].data_2 = cmd->data_2;   
        cmdStruct_Delay[cmd_delay_num].data_3 = cmd->data_3; 
        cmdStruct_Delay[cmd_delay_num].data_4 = cmd->data_4;  
        cmd_delay_num++;

        delayVideoCommand = TRUE;
        taskDelayVideoCmd_sysTime = GetSysTick(); 
    }
    else        /*已经多于5个没有发送，直接发送*/
    {
        send_cmd_to_video(cmd);
        cmd_delay_num = 0;
        delayVideoCommand = FALSE;
    }
}


 
void taskDelayVideoCommand(void)
{
    if(delayVideoCommand == TRUE)
    {
    	if(!isSysTickTimeout(taskDelayVideoCmd_sysTime,400))
    	{
    		return;
    	}
    	taskDelayVideoCmd_sysTime = GetSysTick();  

        if(cmd_delay_num == 0)
        {
            // ERROR!!
        }
        else
        {
            printf("deley send index = %d\r\n",cmd_delay_num);
            send_cmd_to_video(&cmdStruct_Delay[cmd_delay_num - 1]);
            cmd_delay_num = 0;
        }

        delayVideoCommand = FALSE;
    }
}



static bool delayVideoMsg = FALSE;
static u32 taskDelayVideoMsg_sysTime = 0;
int msgParam_Delay[8];

u8 msgType_Delay[8];
u8 msg_delay_num = 0;




void procesDelayMsg(u8 type,int param)
{
    switch(type)
    {
        case 1:
            SetBrightness(FALSE,param,param,param);
            break;
        case 2
:
            SetContrast(FALSE,param,param,param);
            break;
        case 3:
            SetColorTemp(FALSE,(param&0xFF0000) >> 16,(param&0xFF00) >> 8,param&0xFF);
            break;
        case 4:
            SetSharpness(FALSE,param);
            break;
        case 5:
            SetSaturation(FALSE,param);
            break;
        default:
            break;
    }
}


void pushVideoMsg(u8 type,int param)
{
    if(msg_delay_num < 10)
    {
        if((msg_delay_num > 0) && (msgType_Delay[msg_delay_num - 1]) != type)
        {
            procesDelayMsg(msgType_Delay[msg_delay_num - 1],msgParam_Delay[msg_delay_num - 1]);
            msg_delay_num = 0;
        }

        msgType_Delay[msg_delay_num] = type;
        msgParam_Delay[msg_delay_num] = param;
        msg_delay_num++;       
        
        delayVideoMsg = TRUE;
        taskDelayVideoMsg_sysTime = GetSysTick(); 
    }
    else        /*已经多于8个没有发送，直接发送*/
    {
        procesDelayMsg(msgType_Delay[msg_delay_num - 1],msgParam_Delay[msg_delay_num - 1]);
        msg_delay_num = 0;
        delayVideoMsg = FALSE;
    }
}


 
void taskDelayVideoMsg(void)
{
    if(delayVideoMsg == TRUE)
    {
    	//if(!isSysTickTimeout(taskDelayVideoMsg_sysTime,400))
        if(!isSysTickTimeout(taskDelayVideoMsg_sysTime,800))    
    	{
    		return;
    	}
    	taskDelayVideoMsg_sysTime = GetSysTick();  

        if(msg_delay_num == 0)
        {
            // ERROR!!
        }
        else
        {
            //printf("deley send index = %d\r\n",msg_delay_num);
            procesDelayMsg(msgType_Delay[msg_delay_num - 1],msgParam_Delay[msg_delay_num - 1]);
            cmd_delay_num = 0;
        }

        delayVideoMsg = FALSE;
    }
}



/*
 *	 "<T0000828004010007>"
 *     "<F0000828004010007>"
 */
static unsigned int setCheckSum(BYTE *buffer, int size)
{
	unsigned int sum = 0;
	int i;
	for(i=0; i<size; i++)
	{
		sum += buffer[i];
	}
	sum = (~sum) + 1;
	return sum;
}


bool send_cmd_to_video_no_wait(Command_TypeDef *cmdStruct)
{
	char buffer[20];
	++send_sn;
	//send_sn = 0; // for test
	cmdStruct->sn = send_sn;
	cmdStruct->address = 0;
	#if 1
	buffer[0] = 0x09;
	buffer[1] = 0xff;
	buffer[2] = cmdStruct->sn;
	buffer[3] = cmdStruct->command;
	buffer[4] = cmdStruct->data_1;
	buffer[5] = cmdStruct->data_2;
	buffer[6] = cmdStruct->data_3;
	buffer[7] = cmdStruct->data_4;
	buffer[8] = setCheckSum((BYTE *)buffer, 8);
	UARTSendBuffer(STDP9320_UART_PORT, buffer, 9);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	//UARTSendBuffer(0, (BYTE *)buffer, 9);
	
	#else
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MST6M48_UART_PORT, buffer);
	#endif
	return TRUE;
}




bool send_cmd_to_video(Command_TypeDef *cmdStruct)
{
	char buffer[20],repeat = 3;
	u32 startTime;
		
	++send_sn;
	//send_sn = 0; // for test
	cmdStruct->sn = send_sn;
	cmdStruct->address = 0;
	SendRs232Command();

	#if 0
	buffer[0] = 0x09;
	buffer[1] = 0xff;
	buffer[2] = cmdStruct->sn;
	buffer[3] = cmdStruct->command;
	buffer[4] = cmdStruct->data_1;
	buffer[5] = cmdStruct->data_2;
	buffer[6] = cmdStruct->data_3;
	buffer[7] = cmdStruct->data_4;
	buffer[8] = setCheckSum((BYTE *)buffer, 8);
	UARTSendBuffer(STDP9320_UART_PORT, buffer, 9);
#endif	
	//task_video();
	
	startTime = GetSysTick();
	

	#if 0	
	// wait ack
	while((send_sn != receive_sn) && deviceIsConnet && repeat)
	{
		task_video();
		if(isSysTickTimeout(startTime,2000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from video process = %d,receive_sn: %d\r\n", send_sn,receive_sn);
			if(repeat)
			{
				printf("repeat: %d\r\n", repeat);
				UARTSendBuffer(STDP9320_UART_PORT, buffer, 9);
				printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				task_video();
				startTime = GetSysTick();
				continue;
			}
			
			#if 1 //判断不能跟video通信,则不再尝试恢复通信,避免造成设备卡顿
			printf("diable communication with video\r\n");
			//DisableVideoProcessDevice();
			#endif
			deviceIsConnet = 0;
			return FALSE;
		}
	}
	
	if(deviceIsConnet == FALSE)
	{
		while((send_sn != receive_sn))
		{
			task_video();
			if(isSysTickTimeout(startTime,500))
			{
				printf("no video process = %d,receive_sn: %d\r\n", send_sn,receive_sn);
				printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				deviceIsConnet = 0;
				return FALSE;
			}
		}
	}

	#endif
	return TRUE;
}


void send_cmd_to_video_Long_no_wait(Command_TypeDef *cmdStruct,char *buf,char len)
{
	char buffer[256],i,sendLen;
	++send_sn;
	//send_sn = 0; // for test
	cmdStruct->sn = send_sn;
	cmdStruct->address = 0;
	#if 1
	if(len > (256 - 9))
	{
		len = (256 - 9);
	}
	sendLen = 0x09 + len;
	buffer[0] = sendLen;
	buffer[1] = 0xff;
	buffer[2] = cmdStruct->sn;
	buffer[3] = cmdStruct->command;
	buffer[4] = cmdStruct->data_1;
	buffer[5] = cmdStruct->data_2;
	buffer[6] = cmdStruct->data_3;
	buffer[7] = cmdStruct->data_4;
	for(i = 0; i < len; i++)
	{
		buffer[8+i] = *(buf + i);
	}
	buffer[sendLen-1] = setCheckSum((BYTE *)buffer, (sendLen-1));
	UARTSendBuffer(STDP9320_UART_PORT, buffer, sendLen);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	//UARTSend(0, (BYTE *)buffer, 9);
	
	#else
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MST6M48_UART_PORT, buffer);
	#endif
}


bool send_cmd_to_video_Long(Command_TypeDef *cmdStruct,char *buf,char len)
{
	char buffer[256],repeat = 3,i,sendLen;
	u32 startTime;
	++send_sn;
	//send_sn = 0; // for test
	cmdStruct->sn = send_sn;
	cmdStruct->address = 0;
	#if 1
	if(len > (256 - 9))
	{
		len = (256 - 9);
	}
	sendLen = 0x09 + len;
	buffer[0] = sendLen;
	buffer[1] = 0xff;
	buffer[2] = cmdStruct->sn;
	buffer[3] = cmdStruct->command;
	buffer[4] = cmdStruct->data_1;
	buffer[5] = cmdStruct->data_2;
	buffer[6] = cmdStruct->data_3;
	buffer[7] = cmdStruct->data_4;
	for(i = 0; i < len; i++)
	{
		buffer[8+i] = *(buf + i);
	}
	buffer[sendLen-1] = setCheckSum((BYTE *)buffer, (sendLen-1));
	UARTSendBuffer(STDP9320_UART_PORT, buffer, sendLen);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	//UARTSendBuffer(1, buffer, sendLen);
	
	#else
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MST6M48_UART_PORT, buffer);
	#endif
	task_video();
	
	startTime = GetSysTick();
	
	
	// wait ack
	while((send_sn != receive_sn) && deviceIsConnet && repeat)
	{
		task_video();
		if(isSysTickTimeout(startTime,3000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd long from video process = %d,receive_sn: %d, %x,%x,%x,%x,%x\r\n", send_sn,receive_sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			if(repeat)
			{
				printf("repeat  long : %d\r\n", repeat);
				UARTSendBuffer(STDP9320_UART_PORT, buffer, sendLen);
				task_video();
				startTime = GetSysTick();
				continue;
			}
			
			deviceIsConnet = 0;
			return FALSE;
		}
	}
	if(deviceIsConnet == FALSE)
	{
		while((send_sn != receive_sn))
		{
			task_video();
			if(isSysTickTimeout(startTime,200))
			{
				printf("no video process = %d,receive_sn: %d\r\n", send_sn,receive_sn);
				printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				deviceIsConnet = 0;
				return FALSE;
			}
		}
	}
	return TRUE;
}

bool send_cmd_to_video_and_wait(Command_TypeDef *cmdStruct)
{
	
	return send_cmd_to_video(cmdStruct);

}

bool send_cmd_to_mst6m48_and_wait_ack_data(Command_TypeDef *cmdStruct)
{
	
	if( send_cmd_to_video(cmdStruct) == FALSE)
	{
		return FALSE;
	}
	cmdStruct->data_1 = cmd_from_video.data_1;
	cmdStruct->data_2 = cmd_from_video.data_2;
	cmdStruct->data_3 = cmd_from_video.data_3;
	cmdStruct->data_4 = cmd_from_video.data_4;
	return TRUE;

}


void video_getSoftVersion(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x6a;
	cmd.data_1 = 01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_and_wait(&cmd);
}
void video_setSn(u16 sn)
{
	Command_TypeDef cmd;
	cmd.command = 0x6a;
	cmd.data_1 = 0x10;
	cmd.data_2 = sn>>8;
	cmd.data_3 = sn;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
void video_getSn(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x6a;
	cmd.data_1 = 0x11;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

void video_enableDisplay(u8 enable)
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x10;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
void video_resetFactory(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x06;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
void video_loadSetting(char index)// 1 ~ 10
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x18;//0x09;
	cmd.data_2 = index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
void video_saveSetting(int index) // 1 ~ 10
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x08;
	cmd.data_2 = index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	//send_cmd_to_video_no_wait(&cmd);
}

void video_setInputSource(u8 source)
{
    Command_TypeDef cmd;
#if 0
	printf("video_setInputSource(%d)\r\n",source);
    cmd.command = 0x72;
    cmd.data_1 = 0x00;
    cmd.data_2 = 0x00;
    cmd.data_3 = source;
    cmd.data_4 = 0x00;
    send_cmd_to_video(&cmd);
#else
	printf("video_setInputSource(%d)\r\n",source);
    cmd.command = 0x73;
    cmd.data_1 = 0x20;
    cmd.data_2 = source;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;
    send_cmd_to_video(&cmd);
#endif	
}

void video_setPipInputSource(u8 source)
{
    Command_TypeDef cmd;

	printf("video_setInputSource(%d)\r\n",source);
    cmd.command = 0x72;
    cmd.data_1 = 0x00;
    cmd.data_2 = 0x01;
    cmd.data_3 = source;
    cmd.data_4 = 0x00;
    send_cmd_to_video(&cmd);
}

void video_setInputSource_ch(u8 ch,u8 source)
{
    Command_TypeDef cmd;

    cmd.command = 0x72;
    cmd.data_1 = 0x00;//0x1e;//
    cmd.data_2 = ch;
    cmd.data_3 = source;
    cmd.data_4 = 0x00;
    send_cmd_to_video(&cmd);
}


void video_setInputSource_force(u8 source)
{
    Command_TypeDef cmd;

	printf("video_setInputSource_force(%d)\r\n",source);
    cmd.command = 0x72;
    cmd.data_1 = 0x00;
    cmd.data_2 = 0x00;
    cmd.data_3 = source;
    cmd.data_4 = 1;
    send_cmd_to_video(&cmd);
}


void video_setInputSource_Main(u8 source)
{
	Command_TypeDef cmd;
	printf("video_setInputSource_Main(%d)",source);

	cmd.command = 0x72;
	cmd.data_1 = 0x1e;//0x00;//
	cmd.data_2 = 00;
	cmd.data_3 = source;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	//send_cmd_to_video_no_wait(&cmd);
}
void video_setInputSource_PIP(u8 source)
{
	Command_TypeDef cmd;
	printf("video_setInputSource_PIP(%d)",source);
	cmd.command = 0x72;
	cmd.data_1 = 0x1e; //0x00;//
	cmd.data_2 = 1;
	cmd.data_3 = source;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	//send_cmd_to_video_no_wait(&cmd);
}


#if 1
char inputTableFromMst[INPUT_SIZE] = 
{
	//表格的上半部分的值是输入信号对应的8028的信号源的序号
	0, //INPUT_CV1,                     // 0
	10,//VIPDRIVE_INPUT_SIZE      // INPUT_SV,                    
	6, //VIPDRIVE__INPUT_DVI      // INPUT_SDI,                  
	6, //VIPDRIVE__INPUT_DVI      // INPUT_DVI,					   
	4, //INPUT_YPbPr,                   
	3, //INPUT_VGA,                    
	1, //INPUT_CV2,                    
	2, //INPUT_CV3,                     
	10,//VIPDRIVE_INPUT_SIZE      // INPUT_YCbCr,                  
	5, //SP1U_INPUT_DP,         //INPUT_DP,                     
	10,//VIPDRIVE__INPUT_SIZE      // INPUT_TTL,                     
	10,//VIPDRIVE_INPUT_SIZE      // INPUT_YPbPr2,                  
	//10,//VSP1U_INPUT_SIZE      // INPUT_HDMI,       			 
	//10,//VSP1U_INPUT_SIZE      // INPUT_VGA2,	                   
	//10,//VSP1U_INPUT_SIZE      // INPUT_YCbCr2,	               
};



char inputTableFrom9320[INPUT_SIZE] = 
{
	0,	//reserver
	7,	// 
	2,
	3,
	6,
	4,
	0,
	8,
	0,
	1,
	5,
};


#endif
int video_readInputSource(void)
{
#if 0
	if((mst6m48_Setting.inputSource[CurrentWindow] == INPUT_SDI)||(mst6m48_Setting.inputSource[CurrentWindow] == INPUT_TTL))
	{
		return getMB103_inputSource(0);
	}
	else if(mst6m48_Setting.inputSource[CurrentWindow] == INPUT_DVI)
	{
		return getMB103_inputSource(1);
	}
	else
	{
		return inputTableFromMst[mst6m48_Setting.inputSource[CurrentWindow]];
	}
#endif

	//return inputTableFrom9320[video_Setting.inputSource[CurrentWindow]];
	return inputTableFrom9320[video_Setting.inputSource[0]];
}
int video_readInputSourceCh(char ch)
{
#if !(CURRENT_DEVICE == _VIP_SMART)
	if(video_Setting.chSource[ch] == INPUT_TTL)
	{
		return matrix_getSwitch(0);
	}
	else if(video_Setting.chSource[ch] == INPUT_DVI)
	{
		return matrix_getSwitch(1);
	}
	else
#endif
	{
		return video_Setting.chSource[ch];
		//return inputTableFromMst[video_Setting.chSource[ch]];
	}
}

void video_getInputSource(void)
{
	//int curret_source;
	//curret_source = GetUserSetting()->inputSource[CurrentWindow];
	
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 01;
	cmd.data_2 = 00;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);

	//printf("video_getInputSource() = %d\r\n",video_Setting.inputSource[CurrentWindow]);
	//GetUserSetting()->inputSource[CurrentWindow] = video_readInputSource();//videoSetting.inputSource[CurrentWindow];
	
}

int video_getInputSourceNew(int ch)
{
	//int curret_source;
	//curret_source = GetUserSetting()->inputSource[CurrentWindow];
	
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 01;
	cmd.data_2 = ch&0x1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);

	return video_Setting.regValue;
}



int video_getInputSource_Main(void)
{	
	Command_TypeDef cmd;
	int res;
	cmd.command = 0x72;
	cmd.data_1 = 0x1f;
	cmd.data_2 = 00;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);

	res = video_readInputSourceCh(0);//mst6m48_Setting.ChSource[0];//mst6m48_Setting.regValue;
	//GetUserSetting()->inputSourceReadFromVideo[0] = res;
	return res;
}
int video_getInputSource_pip(void)
{	
	Command_TypeDef cmd;
	int res;
	cmd.command = 0x72;
	cmd.data_1 = 0x1f;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);

	res = video_readInputSourceCh(1);//mst6m48_Setting.ChSource[1];//mst6m48_Setting.regValue;
	//GetUserSetting()->inputSourceReadFromVideo[1] = res;
	return res;
}
int video_getInputSourceCh(char ch)//ch 0-main 1-pip
{	
	Command_TypeDef cmd;
	int res;
	cmd.command = 0x72;
	cmd.data_1 = 0x1f;
	cmd.data_2 = ch;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video_and_wait(&cmd);

	res = video_readInputSourceCh(ch);//mst6m48_Setting.ChSource[0];//mst6m48_Setting.regValue;
	//GetUserSetting()->inputSourceReadFromVideo[ch] = res;
	return res;
}

int video_getInputSource_notSave(void)
{	
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 01;
	cmd.data_2 = 00;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	
	return video_readInputSource();
}


// modified by barry 2015.04
static char isSetScale = 0;

void video_setScaleWidth(int value)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 0;
    cmd.data_2 = 0;
    cmd.data_3 = value >> 8;
    cmd.data_4 =  value & 0xFF;

    send_cmd_to_video(&cmd);
    isSetScale = 1;
}

void video_setScaleHeight(int value)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 0;
    cmd.data_2 = 1;
    cmd.data_3 = value >> 8;
    cmd.data_4 =  value & 0xFF;

    send_cmd_to_video(&cmd);
    isSetScale = 1;
}

void video_setScaleX(int value)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 0;
    cmd.data_2 = 2;
    cmd.data_3 = value >> 8;
    cmd.data_4 =  value & 0xFF;

    send_cmd_to_video(&cmd);
    isSetScale = 1;
}

void video_setScaleY(int value)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 0;
    cmd.data_2 = 3;
    cmd.data_3 = value >> 8;
    cmd.data_4 =  value & 0xFF;

    send_cmd_to_video(&cmd);
    isSetScale = 1;
}

void video_setScaleSetting(void)
{
    Command_TypeDef cmd;

    if (isSetScale == 0) return;

    cmd.command = 0x6C;
    cmd.data_1 = 4;
    cmd.data_2 = 0;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);
    isSetScale = 0;
}

#if 1
int video_getScaleWidth(void)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 1;
    cmd.data_2 = 0;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);
    return video_Setting.regValue;
}

int video_getScaleHeight(void)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 1;
    cmd.data_2 = 1;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);
    return video_Setting.regValue;
}

int video_getScaleX(void)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 1;
    cmd.data_2 = 2;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);
    return video_Setting.regValue;
}

int video_getScaleY(void)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 1;
    cmd.data_2 = 3;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);
    return video_Setting.regValue;
}
#endif


void video_setScaleRadio(u8 mode)
{
	Command_TypeDef cmd;

	cmd.command = 0x6c;
	cmd.data_1 = 0x02;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);
}

u8 video_getScaleRadio(void)
{
    Command_TypeDef cmd;

    cmd.command = 0x6C;
    cmd.data_1 = 0x03;
    cmd.data_2 = 0;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);
    return video_Setting.regValue;
}


void video_setScaleH(u16 position, u16 width)
{
    Command_TypeDef cmd;

	//printf("video_setScaleH(%d,%d)\r\n",position,width);
    cmd.command = 0x50;
    cmd.data_1 = width >> 8;
    cmd.data_2 = width & 0xFF;
    cmd.data_3 = position >> 8;
    cmd.data_4 = position & 0xFF;

	//printf("video_setScaleH...");
	//pushVideoCommand(&cmd);
    send_cmd_to_video(&cmd);
}

void video_setScaleV(u16 position, u16 height)
{
    Command_TypeDef cmd;

	//printf("video_setScaleV(%d,%d)\r\n",position,height);
    cmd.command = 0x52;
    cmd.data_1 = height >> 8;
    cmd.data_2 = height & 0xFF;
    cmd.data_3 = position >> 8;
    cmd.data_4 = position & 0xFF;

    send_cmd_to_video(&cmd);
    //pushVideoCommand(&cmd);
}

void video_getScaleH(void)
{
    Command_TypeDef cmd;

    cmd.command = 0x51;
    cmd.data_1 = 0;
    cmd.data_2 = 0;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);
}

void video_getScaleV(void)
{
    Command_TypeDef cmd;

    cmd.command = 0x53;
    cmd.data_1 = 0;
    cmd.data_2 = 0;
    cmd.data_3 = 0;
    cmd.data_4 = 0;

    send_cmd_to_video(&cmd);


}

// end

void video_setZoom(u16 x, u16 y, u16 width, u16 height)
{
	Command_TypeDef cmd;
	cmd.command = 0x54;
	cmd.data_1 = x;
	cmd.data_2 = y;
	cmd.data_3 = width;
	cmd.data_4 = height;	
	send_cmd_to_video(&cmd);
}
void video_getZoom(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x55;
	cmd.data_1 = 0;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);
}
#if 1
void video_setCrop(u16 x, u16 y, u16 width, u16 height)
{
	Command_TypeDef cmd;
	//printf("++mst_setCrop(%d,%d,%d,%d)\r\n",x,y,width,height);
	cmd.command = 0x56;
	cmd.data_1 = x;
	cmd.data_2 = y;
	cmd.data_3 = width;
	cmd.data_4 = height;	
	send_cmd_to_video(&cmd);
}
void video_getCrop(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x57;
	cmd.data_1 = 0;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
#else
void mst_setCrop(int index, int value) //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting
{
	Command_TypeDef cmd;
	cmd.command = 0x56;
	cmd.data_1 = index;
	cmd.data_2 = value&0xff;
	cmd.data_3 = value>>8;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
int  mst_getCrop(int index) //  0 -- width,1-- height,2-- x, 3 -- y, 
{
	Command_TypeDef cmd;
	cmd.command = 0x57;
	cmd.data_1 = index;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.crop[index];
}
#endif

#if 1 //added by cyj for dot crop
void video_setCropDot(int index, int value) //  0 -- width,1-- height,2-- x, 3 -- y
{
	Command_TypeDef cmd;
	//printf("video_setCropDot_%d_%d ",index,value);
	//printf("%d \r\n",value);
	cmd.command = 0x5c;
	cmd.data_1 = 0x00;
	cmd.data_2 = index;
	cmd.data_3 = value>>8;
	cmd.data_4 = value; 
	send_cmd_to_video(&cmd);
}
int  video_getCropDot(int index) //  0 -- width,1-- height,2-- x, 3 -- y, 
{
	Command_TypeDef cmd;
	cmd.command = 0x5c;
	cmd.data_1 = 0x01;
	cmd.data_2 = index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.crop[index];
}
#endif

void video_applayCropDot3(void) //lv@20160715
{
#if 0
	Command_TypeDef cmd;

	printf("video_applayCropDot2()\r\n");
	cmd.command = 0x6E;
	cmd.data_1 =0x12;
	cmd.data_2 = 0;
	cmd.data_3 = 1-getVideoSetting()->currentCh;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);	
#else
    Command_TypeDef cmd;
    printf("video_applayCropDot2()\r\n");
    cmd.command = 0x6E;
    cmd.data_1 =0x12;
    cmd.data_2 = 0;
    cmd.data_3 = getVideoSetting()->currentCh;
    cmd.data_4 = 0;
    send_cmd_to_video(&cmd);    

#endif
}


void video_setCropDotCh(char ch) //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--enable save crop ratio
{
	Command_TypeDef cmd;
	printf("video_setCropDotCh_%d \r\n",ch);
	cmd.command = 0x5c;
	cmd.data_1 = 5;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = ch;
	send_cmd_to_video(&cmd);
}


void video_setCropDot2(int index, int value) //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--set crop single
{
	Command_TypeDef cmd;
	printf("video_setCropDot2_%d_%d \r\n",index,value);
/*	
	switch(index)
	{
		case 0x00:
			value += CurrentCropWidthDotPre;
			break;
		case 0x01:
			value += CurrentCropHeightDotPre;
			break;
		case 0x02:
			value += CurrentCropXDotPre;
			break;
		case 0x03:
			value += CurrentCropYDotPre;
			break;
	}	
*/
	cmd.command = 0x6E;
	cmd.data_1 = 0x0;
	cmd.data_2 = index;
	cmd.data_3 = value>>8;
	cmd.data_4 = value&0xff;

	send_cmd_to_video(&cmd);
}
int  video_getCropDot2(int index) //  0 -- width,1-- height,2-- x, 3 -- y, 
{
	Command_TypeDef cmd;
	cmd.command = 0x6E;
	cmd.data_1 = 0x01;
	cmd.data_2 = index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_applayCropDot2(void)
{
	Command_TypeDef cmd;

	printf("video_applayCropDot2()\r\n");
	cmd.command = 0x6E;
	cmd.data_1 =2;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);	
}

void video_setCropDotCh2(char ch) //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--enable save crop ratio
{
	Command_TypeDef cmd;
	printf("video_setCropDotCh_%d \r\n",ch);
	cmd.command = 0x6E;
	cmd.data_1 = 4;
	cmd.data_2 = ch;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);
}

void video_setSplitZoomDot(char index,int value) //0- width  1- height  2- x   3- y 
{
	Command_TypeDef cmd;
	cmd.command = 0x6E;
	cmd.data_1 = 0x10;
	cmd.data_2 = index;
	cmd.data_3 = value>>8;
	cmd.data_4 = value&0xff;
	send_cmd_to_video(&cmd);
}

void video_applySplitZoom(char ch)
{
	Command_TypeDef cmd;
	cmd.command = 0x6E;
	cmd.data_1 = 0x10;
	cmd.data_2 = 4;
	cmd.data_3 = ch;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);
}

int video_getSplitZoomDot(char index) //0- width  1- height  2- x   3- y 
{
	Command_TypeDef cmd;
	cmd.command = 0x6E;
	cmd.data_1 = 0x11;
	cmd.data_2 = index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_forceReIdentifyCh(char ch)
{
	Command_TypeDef cmd;
	printf("video_forceReIdentifyCh(%d)\r\n",ch);
	cmd.command = 0x6E;
	cmd.data_1 = 0x12;
	cmd.data_2 = 0;
	cmd.data_3 = ch;
	cmd.data_4 = 0;
	send_cmd_to_video(&cmd);
}

void video_setScreenH(u16 position, u16 width)
{
	Command_TypeDef cmd;
	cmd.command = 0x58;
	cmd.data_1 = position & 0xff;
	cmd.data_2 = position >> 8;
	cmd.data_3 = width & 0xff;
	cmd.data_4 = width >> 8;	
	send_cmd_to_video(&cmd);
}
void video_setScreenV(u16 position, u16 height)
{
	Command_TypeDef cmd;
	cmd.command = 0x5a;
	cmd.data_1 = position & 0xff;
	cmd.data_2 = position >> 8;
	cmd.data_3 = height & 0xff;
	cmd.data_4 = height >> 8;	
	send_cmd_to_video(&cmd);
}

void video_setEnableExternMatrixCVBS(int enable) //9320暂时不支持此命令
{
	Command_TypeDef cmd;
	return ;
	cmd.command = 0x72;
	cmd.data_1 = 0x10;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
void video_setEnableTPSelf(int enable)
{
	Command_TypeDef cmd;
	return;
	cmd.command = 0x72;
	cmd.data_1 = 0x16;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

void video_setInterlaceSwitch(int value) // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x12;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
BYTE mst_getInterlaceSwitch(void) // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x13;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	printf("mst_getInterlaceSwitch() = %d\r\n",video_Setting.regValue);
	return video_Setting.regValue;
}

void video_setInputColorSpace_TTL(int value)// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x14;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

int video_getInputColorSpace_TTL(void)// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x15;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return valueFromVideo;
}

void video_waitDeviceReady(void)
{
	int count = 0;
	while(TRUE)
	{
		if(video_Setting.deviceState == DEVICE_READY)
		{
			return;
		}
		else
		{
			task_video();
			count++;
			SysDelay1ms(100);
			if(count > 180)
			{
				return;
			}
		}
	}
}

void video_SetCurrentWindow(int value)
{
	Command_TypeDef cmd;
	cmd.command = 0x6B;
	cmd.data_1 = 02;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

u8 video_getCurrentWindow(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x6B;
	cmd.data_1 = 03;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;		
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

u8 video_getInputFormat(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	//printf("++++++GetInputFormat__%d",mst6m48_Setting.regValue);//mst6m48_Setting.inputFormat[CurrentWindow]
	return video_Setting.regValue;//mst6m48_Setting.inputFormat[CurrentWindow];
}

u8 video_getInputFormatCh(BYTE ch)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x0b;
	cmd.data_2 = ch;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setInputTimming(int type, WORD value)// type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 02;
	cmd.data_2 = type;
	cmd.data_3 = value;
	cmd.data_4 = value>>8;	
	send_cmd_to_video(&cmd);
}
WORD video_getInputTimming(int type) // type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 03;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	switch(type)
	{
		case 0x00:
			return video_Setting.inputTimming[video_Setting.currentCh].HStart;
		case 0x01:
			return video_Setting.inputTimming[video_Setting.currentCh].VStart;
		case 0x02:
			return video_Setting.inputTimming[video_Setting.currentCh].HActive;
		case 0x03:
			return video_Setting.inputTimming[video_Setting.currentCh].VActive;
	}
	//return video_Setting.inputTimming[type&0x03];
}


int video_getInputSourceParameter(int type)		//0:Hactive 1:VActive 2:Freq 3:HTotal 4:Vtotal
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x07;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}

void video_resetSdiInputtimming(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 04;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

void video_resetAllSdi(int type)	// 0 -- FPGA SDI, 1 -- GS2971 SDI
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 06;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

void video_setOutputFormat(u8 format)
{
	
	Command_TypeDef cmd;
	int value = 0;
	
	#if ENABLE_USER_DEFINE
	if(format == MD_1024x768_59_94)
	{
		format = MD_1024x768_60;
	}
	#endif

	#if 0
	printf("format is %d\r\n", format);
	if((format == MD_3840x1080_60)||(format == MD_2560x1600_60) ||(format == MD_2560x1440_60))
	{
		value = GetHDEByVideoFormat(format);
		value = value >>1;
		printf("HActive is %d\r\n", value);
		FPGA_set(FPGA_REG_HACTIVE_LSB,value);
		FPGA_set(FPGA_REG_HACTIVE_MSB,value>>8);
		FPGA_set(FPGA_REG_0X10,0x40); // 0x70
	}
	else
	{
		FPGA_set(FPGA_REG_0X10,0x20); // 0x30
	}
	#endif
		

	cmd.command = 0x74;
	cmd.data_1 = 00;
	cmd.data_2 = format;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	
	video_Setting.deviceState = DEVICE_INIT;
	
	send_cmd_to_video(&cmd);
}
BYTE video_getOutputFormat(void)
{
	
	Command_TypeDef cmd;

	cmd.command = 0x74;
	cmd.data_1 = 1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
		
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setOutputMode(u8 mode)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 4;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);
}
void video_SetOutputColorSpace(int value) // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x0e;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);
}
int  video_GetOutputColorSpace(void) // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x0f;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);

	return video_Setting.outputColorSpace;
}


void video_setFrameLockMode(u8 mode) // 0 -- free run, 1 -- Fixed Frame Lock, 2 -- Dynamic Frame Lock
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 6;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);
}
int  video_getFrameLockMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 7;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);

	return video_Setting.frameLockMode;
}
void video_setFrameLockSource(u8 source) // 0 -- frame lock main , 1 -- frame lock pip
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 8;
	cmd.data_2 = source;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);
}
int  video_getFrameLockSource(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 9;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);

	return video_Setting.frameLockSource;
}
void video_setDisplayMode(u8 mode)
{
	Command_TypeDef cmd;
	//printf("mst_setDisplayMode(%d,%d)\r\n",CurrentWindow, mode);
	cmd.command = 0x79;
	cmd.data_1 = 0;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_video(&cmd);
}

int video_getDisplayMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return CurDisplayMode;
}
void video_setFlatColor(u8 r, u8 g, u8 b)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 2;
	cmd.data_2 = r;
	cmd.data_3 = g;
	cmd.data_4 = b;	
	send_cmd_to_video(&cmd);
}

void video_setTestPatternMode(u8 mode)
{
	u8 value;
	Command_TypeDef cmd;
	if(mode <= 20)
	{

		value = FPGA_get(FPGA_REG_PGM_PRV);
		value &= ~(3 << 6);
		value = value | (1 << 6);
		FPGA_set(FPGA_REG_PGM_PRV,value);
		
		
		cmd.command = 0x79;
		cmd.data_1 = 6;
		cmd.data_2 = mode;
		cmd.data_3 = 0;
		cmd.data_4 = 0;	
		send_cmd_to_video(&cmd);
	}
	else
	{
		value = FPGA_get(FPGA_REG_PGM_PRV);
		value &= ~(3 << 6);
		FPGA_set(FPGA_REG_PGM_PRV,value);			//在DVI输出内部测试模板
		FPGA_set(FPGA_REG_TESTPATTERN,mode-20);		//内部测试模板从21开始
		
		FPGA_set(FPGA_REG_H_GRID_LSB,10);
		FPGA_set(FPGA_REG_H_GRID_MSB,0);
		FPGA_set(FPGA_REG_V_GRID_LSB,10);
		FPGA_set(FPGA_REG_V_GRID_MSB,0);
	}
}
void video_setTestPatternColor(u8 r, u8 g, u8 b)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x08;
	cmd.data_2 = r;
	cmd.data_3 = g;
	cmd.data_4 = b;	
	send_cmd_to_video(&cmd);
}
void video_setTestPatternParam(u8 horStep, u8 verStep, u8 stepColor)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x0a;
	cmd.data_2 = horStep;
	cmd.data_3 = verStep;
	cmd.data_4 = stepColor;	
	send_cmd_to_video(&cmd);
}


void video_setBlackOut(u8 Enable)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x0c;
	cmd.data_2 = Enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);	

}


int video_getBlackOut(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x79	;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	

	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}

void video_setBrightness(u8 bCont,u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	//cmd.data_1 = 0;
    cmd.data_1 = 2;
	cmd.data_2 = red;
	cmd.data_3 = green;
	cmd.data_4 = blue;	

    if(bCont == TRUE)
    {
        pushVideoCommand(&cmd);
    }
    else
    {
	    send_cmd_to_video(&cmd);
    }
}

void video_setBrightness_nowait(u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	//cmd.data_1 = 0;
    cmd.data_1 = 2;
	cmd.data_2 = red;
	cmd.data_3 = green;
	cmd.data_4 = blue;	
	send_cmd_to_video_no_wait(&cmd);
}


int video_getBrightness(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	//cmd.data_1 = 1;
    cmd.data_1 = 3;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	 
}
void video_setContrast(u8 bCont,u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	//cmd.data_1 = 2;
    cmd.data_1 = 0;
	cmd.data_2 = red;
	cmd.data_3 = green;
	cmd.data_4 = blue;	
    if(bCont == TRUE)
    {
        pushVideoCommand(&cmd);
    }
    else
    {
	    send_cmd_to_video(&cmd);
    }

}

void video_setContrast_nowait(u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	//cmd.data_1 = 2;
    cmd.data_1 = 0;
	cmd.data_2 = red;
	cmd.data_3 = green;
	cmd.data_4 = blue;	
	send_cmd_to_video(&cmd);
}


int video_getContrast(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	//cmd.data_1 = 3;
    cmd.data_1 = 1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);	
	return video_Setting.regValue;	
}

void video_setColorTemp(u8 bCont,u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 04;
	//cmd.data_2 = red;//((int)red*255/100);//((int)red*100/255);
	//cmd.data_3 = green;//((int)green*255/100);
	//cmd.data_4 = blue;//((int)blue*255/100);	
	cmd.data_2 = (red*100/255);//((int)red*100/255);
	cmd.data_3 = ((int)green*100/255);
	cmd.data_4 = ((int)blue*100/255);		
    if(bCont == TRUE)
    {
        pushVideoCommand(&cmd);
    }
    else
    {
	    send_cmd_to_video(&cmd);
    }

}

void video_setColorTemp_nowait(u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 04;
	cmd.data_2 = ((int)red*255/100);//((int)red*100/255);
	cmd.data_3 = ((int)green*255/100);
	cmd.data_4 = ((int)blue*255/100);	
    pushVideoCommand(&cmd);
	//send_cmd_to_video_no_wait(&cmd);
}


int video_getColorTemp(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 5;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);	
	return video_Setting.regValue;	
}

void video_setColorTempMode(u8 mode)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x0c;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

int video_getColorTempMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0xd;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);	
	return video_Setting.regValue;	

}


void video_setSharpness(u8 sharpness)//20110906
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 06;
	cmd.data_2 = sharpness;
	cmd.data_3 = 0;
	cmd.data_4 = 0;		
	send_cmd_to_video(&cmd);
}

int video_getSharpness(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x07;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);		
	return video_Setting.regValue;	
}

void video_setSaturation(u8 saturation)//20110909
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x08;
	cmd.data_2 = saturation;
	cmd.data_3 = 0;
	cmd.data_4 = 0;		
	send_cmd_to_video(&cmd);
}

int video_getSaturation(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x09;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);		
	return video_Setting.regValue;	
}

void video_setAdcAutoAdjust(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x0a;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;		
	send_cmd_to_video(&cmd);
}

void video_ResetAdcSetting(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x0b;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;		
	send_cmd_to_video(&cmd);
}

void video_setRGBSaturation(u8 r, u8 g, u8 b)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0xe;
	cmd.data_2 = r;
	cmd.data_3 = g;
	cmd.data_4 = b;		
	send_cmd_to_video(&cmd);
}

int video_getRGBSaturation(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x0f;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);		
	return video_Setting.regValue;	
}

void video_setCMYSaturation(u8 c, u8 m, u8 y)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x10;
	cmd.data_2 = c;
	cmd.data_3 = m;
	cmd.data_4 = y;		
	send_cmd_to_video(&cmd);
}

int video_getCMYSaturation(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x11;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);		
	return video_Setting.regValue;	
}


void video_setEvenness(u8 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x12;
	cmd.data_2 = value&0x1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;		
	send_cmd_to_video(&cmd);
}

int video_getEvenness(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x13;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);		
	return video_Setting.regValue;	

}


void video_setVideoEnhance(u8 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x14;
	cmd.data_2 = value&0x1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;		
	send_cmd_to_video(&cmd);
}

int video_getVideoEnhance(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x15;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);		
	return video_Setting.regValue;	

}


void video_setPcX(u16 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x38;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
void video_setPcY(u16 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3a;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

void video_setPcClock(u16 clock)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3c;
	cmd.data_2 = clock>>8;
	cmd.data_3 = clock;
	cmd.data_4 = 0;	 
	send_cmd_to_video(&cmd);
}
void video_setPcPhase(u16 phase)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3e;
	cmd.data_2 = phase>>8;
	cmd.data_3 = phase;
	cmd.data_4 = 0;	 
	send_cmd_to_video(&cmd);
}

void video_setAutoAdjustVGA(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x36;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

void video_setGamma(int value)
{
	Command_TypeDef cmd;
	cmd.command = 0x77;
	cmd.data_1 = 00;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

BYTE video_getGamma(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x77;
	cmd.data_1 = 01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_getPcX(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x39;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video_and_wait(&cmd);
}
void video_getPcY(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3b;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video_and_wait(&cmd);
}
void video_getPcClock(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3d;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video_and_wait(&cmd);
}
void video_getPcPhase(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3f;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video_and_wait(&cmd);
}

void video_getDeviceState(void)
{
	int i = 0;
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x13;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	while((!send_cmd_to_video_and_wait(&cmd)) && (i++ < 3))
	{
		
	}
}

void video_setAudioSource(u8 source)
{
#if 0           // Cancel By hibernate

	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 00;
	cmd.data_2 = source;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
#endif
}
void video_setAudioMode(u8 mode)
{
#if 0           // Cancel By hibernate
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 02;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
#endif    
}

void video_setHdmiAudio(u8 value)
{
#if 0           // Cancel By hibernate
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x08;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
#endif
}
void video_setVolume(u8 value)
{
#if 0           // Cancel By hibernate
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x04;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
#endif
}

void video_setMuteVolume(u8 value)
{
#if 0           // Cancel By hibernate

	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x06;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
 #endif   
}

void video_setWindowRatio(u8 value) // 0 -- 4:3, 1--16:9, 2 --Normal
{
	Command_TypeDef cmd;
	cmd.command = 0x6C;
	cmd.data_1 = 0x02;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

void video_setScreenMode(u8 value) // 0 -- full window, 1 -- screen window
{
	Command_TypeDef cmd;
	
		
	
	cmd.command = 0x6C;
	cmd.data_1 = 0x04;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

int video_getAlpha(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);

	return FadeAlpha;
	
}
void video_setAlpha(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x00;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
void video_setSwitchMode(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x08;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
BYTE video_getSwitchMode(void)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x09;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}
void video_setFadeTime(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x02;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
int video_getFadeTime(void)
{
	Command_TypeDef cmd;
	int value = 0;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x03;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	value = video_Setting.regValue>>8;
	value += (video_Setting.regValue&0xff)<<8;
	return value;
}

void video_setSwitchAB(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x0A;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
int video_getSwitchAB(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x0B;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return SwitchTo;
}
int video_getCurrentCh(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.currentCh;
}
void video_setPipMode(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x00;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

int video_getPipMode(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return cmd_from_video.data_2;
}


void video_SwapMainPipInputSource(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x04;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}


int video_getSwapMainPipInputSource(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x05;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;

}


void video_setTopWindow(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x06;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

void video_setTextEnableMode(int enable, int mode)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x08;
	cmd.data_2 = enable;
	cmd.data_3 = mode;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
void video_setTextAboveAndOr(int above, int AndOr)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x0a;
	cmd.data_2 = above;
	cmd.data_3 = AndOr;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
void video_setTextColor(int r, int g, int b)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x0c;
	cmd.data_2 = r;
	cmd.data_3 = g;
	cmd.data_4 = b; 
	send_cmd_to_video(&cmd);
}

#if 1  //modify by yy @20150828
void video_setSplitSync(int onOff)    // data2:  0-0ff 1-on  data3: 0-FFL 1-DFL
{
	Command_TypeDef cmd;		
	cmd.command = 0x74;
	cmd.data_1 = 0x0a;
	cmd.data_2 = onOff;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

int video_getSplitSync(void) 
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0b;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}

#endif
void video_setSplit(int onOff, int syncMode)
{
	Command_TypeDef cmd;		
	syncMode = 1;
	cmd.command = 0x74;
	cmd.data_1 = 0x0a;
	cmd.data_2 = onOff;
	cmd.data_3 = syncMode;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

void video_getSplitOnOff(void)
{
	Command_TypeDef cmd;		
	cmd.command = 0x74;
	cmd.data_1 = 0x0b;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}



void video_setSplitHTotal(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 0;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_video(&cmd);
}
void video_setSplitVTotal(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 1;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_video(&cmd);
}
void video_setSplitHSize(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 2;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_video(&cmd);
}
void video_setSplitVSize(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 3;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_video(&cmd);
}
void video_setSplitHPos(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 4;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_video(&cmd);
}
void video_setSplitVPos(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 5;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_video(&cmd);
}

void video_setVLockLine(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 6;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_video(&cmd);
	
}

int video_getVLockLine(void)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 6;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.vLockLine;
}

void video_setOutputOffset(int x, int y)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x10;
	cmd.data_2 = x;
	cmd.data_3 = y;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}


void video_waitUpdateSoftware(void)
{
	int count = 0;
	while(TRUE)
	{
		if(video_Setting.updateState != 0)
		{
			return;
		}
		else if(video_Setting.deviceState == DEVICE_READY)
		{
			return;
		}
		else
		{
			task_video();
			count++;
			SysDelay1ms(100);
			if(count > 600)
			{
				return;
			}
		}
	}
}

bool video_updateSoftware(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x20;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	video_Setting.updateState = 0;
	video_Setting.deviceState = DEVICE_INIT;
	send_cmd_to_video(&cmd);

	video_waitUpdateSoftware();

	if(video_Setting.deviceState == DEVICE_READY)
	{
		video_Setting.updateState = 2;
		return TRUE;
	}
	else if(video_Setting.updateState == 3)
	{
		return FALSE;
	}
	else
	{
		video_Setting.updateState = 3;
		return FALSE;
	}
	
}
void video_setCustomParameter(int type, WORD value) //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x12;
	cmd.data_2 = type;
	cmd.data_3 = value>> 8;
	cmd.data_4 = value; 
	send_cmd_to_video(&cmd);
}

int video_getCustomParameter(int type) //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x13;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	switch(type)
	{
		case 0:
			return video_customVideo.htotal;
		case 1:
			return video_customVideo.vtotal;
		case 2:
			return video_customVideo.hstart;
		case 3:
			return video_customVideo.vstart;
		case 4:
			return video_customVideo.vfreq;
		case 5:
			return video_customVideo.hSyncWidth;
		case 6:
			return video_customVideo.vSyncWidth;
		case 7:
			return video_customVideo.hvSyncPolarity;
		case 8:
			return video_customResolution.DisplayWidth;
		case 9:
			return video_customResolution.DisplayHeight;
	}
	return 0;
}
#if 1
int video_getSTRegister(int type) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x14;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}
int video_getSTVideoValue(int type) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x15;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setDviTPClock(int enable)
{
	
	Command_TypeDef cmd;

	cmd.command = 0x72;
	cmd.data_1 = 0x16;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

int video_getDviTPClock(void) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x72;
	cmd.data_1 = 0x17;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}



void video_updateColorSpace(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x72;
	cmd.data_1 = 0x1C;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}
void video_setEnableInternalHDCP(int enable)
{
	Command_TypeDef cmd;

	return;
	if(enable == 0)
	{
		printf("disable HDCP\r\n");
	}
	cmd.command = 0x72;
	cmd.data_1 = 0x1d;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

void video_setsdiIndex_TTL(int value)//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
{
	Command_TypeDef cmd;
	return ;
	cmd.command = 0x72;
	cmd.data_1 = 0x20;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}
int video_getsdiIndex_TTL(void)//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x21;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


BYTE video_getTTLInterlaveMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 9;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}
void video_setTTLInterlaveMode(BYTE type)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 8;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
}

int video_getHActiveValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}
int video_getVActiveValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}
int video_getHTotalValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 3;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}
int video_getVTotalValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 4;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

int video_getFreqValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 2;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_setDpVersion(int version)	/* 0 - 1.1a 1-1.2 */
{
	Command_TypeDef cmd;
	
	cmd.command = 0x73;
	cmd.data_1 = 0x22;
	cmd.data_2 = version;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}

int video_getDpVersion(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x23;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}


void video_setDpOutputMode(int type)	/* 0-iDp 1-Dp loop */
{	
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x22;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);
}


int video_getDpOutputMode(void)  /* 0-iDp 1-Dp loop */
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x23;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	printf("video_getDpOutputMode() = %d\r\n",video_Setting.regValue);
	return video_Setting.regValue;	
}


void video_setCustomWidth(int value)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x26;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);	
}

int video_getCustomWidth(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x27;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}

void video_setCustomHeight(int value)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x28;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);	
}

int video_getCustomHeight(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x29;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}
void video_setCustomFreq(int value)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x2a;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);	
}

int video_getCustomFreq(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x2b;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}


void video_ApplyCustomResolution(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x34;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
}

int video_getEnableDoubleOutput(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x43;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return cmd_from_video.data_2;
}




void video_setDVIInputMode(int type)  // 0:单链路 1:双链路
{
	Command_TypeDef cmd;
	
	cmd.command = 0x73;
	cmd.data_1 = 0x20;
	cmd.data_2 = type&0x1;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);

}



int video_getDVIInputMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x21;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	//return video_Setting.DVIInputMode;
	return video_Setting.regValue;


}


void video_setDVIColorSpace(int ch, int type)  // 0: RGB 1: YUV 4:4:4   2: YUV 4:2:2  3: BT565
{
	Command_TypeDef cmd;
	
	cmd.command = 0x73;
	cmd.data_1 = 0x24;
	cmd.data_2 = ch&0x1;
	cmd.data_3 = type&0x3;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);

}

int video_getDVIColorSpace(int ch)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x25;
	cmd.data_2 = ch&0x1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}



void video_setPipColorSpace(int ch, int type)  // 0: RGB 1: YUV 4:4:4   
{
	Command_TypeDef cmd;
	
	cmd.command = 0x73;
	cmd.data_1 = 0x26;
	cmd.data_2 = ch&0x1;
	cmd.data_3 = type&0x1;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);

}

int video_getPipColorSpace(int ch)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x27;
	cmd.data_2 = ch&0x1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_setOverTurn(int type)   // 0:不翻转  1: 左右翻转  2: 上下翻转 3:对角翻转
{
	Command_TypeDef cmd;
	
	cmd.command = 0x7D;
	cmd.data_1 = 0x0;
	cmd.data_2 = type&0x3;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getOverTurn(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x7D;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}



void video_setMADI(int type)	// 00: MADI_OFF  01:MADI_ADAPTIVE  02:MADI_STADARD 03:MADI_LOW_MOTION 04:MADI_HIGHT_MOTION
{
	Command_TypeDef cmd;
	
	cmd.command = 0x7F;
	cmd.data_1 = 0x0;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	
}


int video_getMADI(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x7F;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;


}

void video_setEdidWp(int type)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x16;
	cmd.data_2 = type&0x1;		 // 0: enable  1:disable
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);		
}


void video_setDpEDIDPixclock(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x0;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidPixclock(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setDpEdidhActive(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x2;
	cmd.data_2 = (value&0xFF00)>>4;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidhActive(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x03;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setDpEdidvActive(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x04;
	cmd.data_2 = (value&0xFF00)>>4;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidvActive(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x05;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setDpEdidhBlank(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x06;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidhBlank(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x07;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setDpEdidvBlank(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x08;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidvBlank(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x09;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_setDpEdidhSyncOffset(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x0a;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidhSyncOffset(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x0b;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setDpEdidvSyncOffset(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x0c;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidvSyncOffset(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_setDpEdidhSyncWidth(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x0e;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidhSyncWidth(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x0f;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_setDpEdidvSyncWidth(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x10;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidvSyncWidth(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x11;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setDpEdidhvPolarity(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x12;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getDpEdidhvPolarity(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x83;
	cmd.data_1 = 0x13;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_setDpEdidUpdate(void)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x14;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}

void video_setEnableDpEdidWP(void)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x16;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}
void video_setDisableDpEdidWP(void)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x16;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}

void video_setDpEDIDContent(int num, int value)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x20+num*2;
	cmd.data_2 = value >>16;
	cmd.data_3 = value>>8;
	cmd.data_4 = value; 
	send_cmd_to_video(&cmd);	
}

int video_getDpEDIDContent(int num)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x83;
	cmd.data_1 = 0x21 + num*2;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;	
}

int video_getLVDSMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x25;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}


void video_setLVDSMode(int value)  
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x24;
	cmd.data_2 = value&0x1;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}

void video_setSence(int value) 	//0:normal 1:多媒体 2:游戏 3:电影4:文本
{
	Command_TypeDef cmd;
	
	cmd.command = 0x7C;
	cmd.data_1 = 0x2;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getSence(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x7C;
	cmd.data_1 = 0x3;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setOSDKey(int value)	// 0:menu 1:up 2:down 3:left 4:right 5:Enter 6: return 7:Source
{
	Command_TypeDef cmd;	
	
	cmd.command = 0x7C;
	cmd.data_1 = 0x16 + value*2;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	
}

void video_setBoardParam(int param, int value)
{
	Command_TypeDef cmd;	
	
	cmd.command = 0x74;
	cmd.data_1 = 0x2c + param*2;
	cmd.data_2 = 1;
	if(param == 3)	//hightlight
	{
		cmd.data_3 = value;  
		cmd.data_4 = video_Setting.boardOnOff; 
	}
	else if(param == 4)
	{
		cmd.data_1 = 0x32;
		cmd.data_3 = video_Setting.boardHighLight;
		cmd.data_4 = value;
	}
	else
	{
		cmd.data_3 = 0;  // value>>8
		cmd.data_4 = value; 
	}
	send_cmd_to_video_no_wait(&cmd);	
}

int video_getBoardParam(int param)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x2d+param*2;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	if(param == 4)
	{
		cmd.data_1 = 0x33;
		send_cmd_to_video(&cmd);
		return video_Setting.boardOnOff;
	}
	send_cmd_to_video(&cmd);
	
	return video_Setting.regValue;	
}

void video_AutoAdjust(void)
{
	Command_TypeDef cmd;	
	
	cmd.command = 0x74;
	cmd.data_1 = 0x36;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);		
}

void video_setAdjustH(int value) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x38;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getAdjustH(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x39;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setAdjustV(int value) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x3A;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getAdjustV(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3B;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setAdjustClock(int value) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x3C;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getAdjustClock(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3D;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_setAdjustPhase(int value) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x3E;
	cmd.data_2 = value>>8;
	cmd.data_3 = value;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);	

}


int video_getAdjustPhase(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x3F;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;
}

void video_VGAAuotoClock(void)
{
	Command_TypeDef cmd;	
	
	cmd.command = 0x74;
	cmd.data_1 = 0x40;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);		
}

void video_setSplitSetting(int value)
{
	Command_TypeDef cmd;	
	
	cmd.command = 0xF2;
	cmd.data_1 = 0x0;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video_no_wait(&cmd);			
}

int video_getSplitSetting(void)
{
	Command_TypeDef cmd;	
	
	cmd.command = 0xF2;
	cmd.data_1 = 0x1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_video(&cmd);
	return video_Setting.regValue;			
}

void video_getInputParams_Long(void)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf9;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);
}

void video_getDPEDID_First(void)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf9;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);	
}

void video_getDPEDID_Second(void)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf9;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);	
}

void video_setDPEDID_First(char *buffer,int len)
{
	Command_TypeDef cmd;
	//char sendBuf[256];
		
	cmd.command = 0xf8;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 0x00;
	cmd.data_3 = len;
	cmd.data_4 = (len>>8);
	
	send_cmd_to_video_Long(&cmd,buffer,len);
	
}

void video_setDPEDID_Second(char *buffer,int len)
{
	Command_TypeDef cmd;
	//char sendBuf[256];
		
	cmd.command = 0xf8;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 0x01;
	cmd.data_3 = len;
	cmd.data_4 = (len>>8);
	
	send_cmd_to_video_Long(&cmd,buffer,len);
	
}

void video_clearNVRAM(void)
{
	Command_TypeDef cmd;
	//char sendBuf[256];
		
	cmd.command = 0xf5;
	cmd.data_1 = 0x55;
	cmd.data_2 = 0x66;
	cmd.data_3 = 0x77;
	cmd.data_4 = 0x88;
	
	//send_cmd_to_video(&cmd);		
	send_cmd_to_video_no_wait(&cmd);		
}

void video_InitializeNvram(void)
{
	Command_TypeDef cmd;
	//char sendBuf[256];
		
	cmd.command = 0xf5;
	cmd.data_1 = 0x00;
	cmd.data_2 = 0x00;
	cmd.data_3 = 0x00;
	cmd.data_4 = 0x00;
	
	send_cmd_to_video_no_wait(&cmd);		
}

void video_getLayerScalerParams_Long(void)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf9;
	cmd.data_1 = 0x02;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);
}
void video_setLayerScalerParams(char *buffer,int len)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf8;
	cmd.data_1 = 0x02;
	cmd.data_2 = 0x00;
	cmd.data_3 = len;
	cmd.data_4 = (len>>8);
	
	send_cmd_to_video_Long(&cmd,buffer,len);
}
void video_getLayerPictureParams_Long(void)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf9;
	cmd.data_1 = 0x03;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_video(&cmd);
}

#endif

int GetDPEDID(void)
{
	int res = 128;
	video_getDPEDID_First();
	if(video_Setting.DP_EDID[126] == 0)
	{
		video_Setting.DP_EDID_Len = 128;
		return res;
	}
	video_getDPEDID_Second();
	res = 256;
	video_Setting.DP_EDID_Len = 256;
	return res;
}


void SetDPEDID(void)
{
	video_setDisableDpEdidWP();  
	
	video_setDPEDID_First(getVideoSetting()->DP_EDID,128);
	
	if(getVideoSetting()->DP_EDID_Len == 128)
	{

		return;
	}
	
	video_setDPEDID_Second(&getVideoSetting()->DP_EDID[128],128);
	
	video_setDpEdidUpdate();
	video_setEnableDpEdidWP();	
}





int GetVideoVersion(void)
{
	video_getSoftVersion();
	return ((getVideoSetting()->ver_msb<<8)+getVideoSetting()->ver_lsb);
}


void controlVideoEnable(bool enable)
{
	if(enable == 0)
	{
	#if !(CURRENT_DEVICE == _VIP_SMART)
		UARTDeinit(STDP9320_UART_PORT);
	#endif
		DisableVideoProcessDevice();
	}
	else
	{
		init_stdp9320();
		EnableVideoProcessDevice();
	}
}

void DisableEDIDWP(void)
{
	video_setEdidWp(1);  // disable WP
}

void EnableEDIDWP(void)
{
	video_setEdidWp(0);  // enable WP
}

void SetLayerScalerParams(void)
{
	char buffer[50];
	int len;

	buffer[0] = video_Setting.scaler[0][0];
	buffer[1] = video_Setting.scaler[0][0]>>8;
	buffer[2] = video_Setting.scaler[0][1];
	buffer[3] = video_Setting.scaler[0][1]>>8;
	buffer[4] = video_Setting.scaler[0][2];
	buffer[5] = video_Setting.scaler[0][2]>>8;
	buffer[6] = video_Setting.scaler[0][3];
	buffer[7] = video_Setting.scaler[0][3]>>8;
	
	buffer[8]  = video_Setting.scaler[1][0];
	buffer[9]  = video_Setting.scaler[1][0]>>8;
	buffer[10] = video_Setting.scaler[1][1];
	buffer[11] = video_Setting.scaler[1][1]>>8;
	buffer[12] = video_Setting.scaler[1][2];
	buffer[13] = video_Setting.scaler[1][2]>>8;
	buffer[14] = video_Setting.scaler[1][3];
	buffer[15] = video_Setting.scaler[1][3]>>8;
	len = 16;
	video_setLayerScalerParams(buffer,len);
}

void SetLayerScalerToCh(char ch, int x,int y,int width, int height)//只设置单一通道的SCALE
{
	printf("SetLayerScalerToCh%d_%d,%d,%d,%d\r\n",ch,x,y,width,height);

	video_Setting.scaler[ch][0] = x;
	video_Setting.scaler[ch][1] = y;
	video_Setting.scaler[ch][2] = width;
	video_Setting.scaler[ch][3] = height;
	
	SetLayerScalerParams();
}


#if (CURRENT_DEVICE == _VIP_SMART)
void mst_setAutoBackGroundControlEn(u8 enable)
{
#if 0
	Command_TypeDef cmd;
	//printf("mst_setDisplayMode(%d,%d)\r\n",CurrentWindow, mode);
	cmd.command = 0x79;
	cmd.data_1 = 0x10;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
#endif
}

void mst_setDisplayModeFreezeAll(bool freeze)
{
#if 0
	Command_TypeDef cmd;
	printf("mst_setDisplayModeFreezeAll(%d)\r\n",freeze);
	cmd.command = 0x79;
	cmd.data_1 = 0x12;
	cmd.data_2 = 0x03;
	cmd.data_3 = freeze;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
#endif
}

void mst_setCropSplitEnable(bool enable)
{
#if 0
	Command_TypeDef cmd;
	cmd.command = 0x74;  
	cmd.data_1 = 0x0c;
	cmd.data_2 = 7;
	cmd.data_3 = enable;
	cmd.data_4 = 0;
	send_cmd_to_mst6m48(&cmd);
#endif
}

void mst_setCropSplitInput(int index, int value) //  0 -- width,1-- height,2-- x, 3 -- y
{
#if 0

	Command_TypeDef cmd;
	cmd.command = 0x74;  
	cmd.data_1 = 0x0c;
	cmd.data_2 = 8+index;
	cmd.data_3 = value&0xff;
	cmd.data_4 = value>>8;
	send_cmd_to_mst6m48(&cmd);
#endif
}

void mst_setDPAudio(u8 value)
{
#if 0
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x10;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	//printf("mst_setDPAudio(%d)\r\n",value);
	send_cmd_to_mst6m48(&cmd);
#endif
}

void mst_setHDMIInterlaveMode(BYTE type)
{
#if 0
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x0c;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
#endif
}

void mst_setAudioType(u8 value)
{
#if 0
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x0e;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	//printf("mst_setAudioType(%d)\r\n",value);
	send_cmd_to_mst6m48(&cmd);
#endif
}

#endif

#endif			// #ifdef USE_STDP9320




