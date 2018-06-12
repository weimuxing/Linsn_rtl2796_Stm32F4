#include "systick.h"
#include "uart.h"
#include "SysConfig.h"

#include "mst6m48.h"
#include "pipe.h"
#include "userpref.h"
#include "control.h"
#include "videoFormatTable.h"

#include <stdio.h>

//#define _BEBUG
#if defined(DEBUG_PRINT_ENABLE)&&defined(_BEBUG)
//#define STDP9320_DBG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"/n", __LINE__, ##__VA_ARGS__)
#define STDP9320_DBG(format,...) printf("[stdp9320]"format,  ##__VA_ARGS__)
#else
#define STDP9320_DBG(format,...)	do{}while(0)
#endif


#ifdef USE_STDP8028

/* Private typedef -----------------------------------------------------------*/
#define MST6M48_UART_PORT 3

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern u8 receive_preamp_buffer[512];
Pipe_t receive_preamp_pipe;
u8 receive_sn = 0;
u8 send_sn = 0;
Mst6m48_Setting_TypeDef mst6m48_Setting;
static u8 deviceIsConnet = 1;
static bool forceCommFlag = 0;
VideoDefine_Struct mst6m48_customVideo;  
Resolution_Struct mst6m48_customResolution;

/* Private function prototypes -----------------------------------------------*/
void ForceCommFlag(void)
{
	forceCommFlag = 1;
}

Mst6m48_Setting_TypeDef *Get6m48Setting(void)
{
	return &mst6m48_Setting;
}

u8 getVideoStatus(void)
{
	return deviceIsConnet;
}

void init_mst6m48(void)
{
	pipe_init(&receive_preamp_pipe,receive_preamp_buffer,sizeof(receive_preamp_buffer));
	UARTInit(MST6M48_UART_PORT,115200); // 6m48
	mst6m48_Setting.enableDisplay = 0;
	mst6m48_Setting.inputFormat[0] = 127; // no input
	mst6m48_Setting.inputFormat[1] = 127; // no input
	mst6m48_Setting.sn_lsb = 0x34;
	mst6m48_Setting.sn_msb = 0x12;
	mst6m48_Setting.ver_lsb = 0x01;
	mst6m48_Setting.ver_msb = 0x00;
	mst6m48_Setting.deviceState = DEVICE_INIT;
	mst6m48_Setting.updateState = 0; // 0(start），1(updating),  2(success),  3(fail)
	//mst_getDeviceState();
	mst_getSn();
	mst_getSoftVersion();
	//mst_enableDisplay(1);
	
#if (CURRENT_DEVICE == _VIP_SMART)
	mst_setInputSource(4);
	//mst_setBrightness(206,206,206);	
	mst_setBrightness(229,229,229);
#endif
	
	//mst_getInputFormat();
	STDP9320_DBG("init_mst6m48------>%d\r\n",GetSysTick());
}

const char mst6m48_cmd_header[] = {'<','F'};
const char mst6m48_cmd_end[] = {'>'};

static Command_TypeDef cmd_from_mst6m48;
/*
 *	 ":TOVC02000000828004010007\r\n" = 27
 *     ":FRVC0600828004010007\r\n" = 23
 */
static InterpretCmdState_TypeDef state = interpret_cmd_init;

#if 0
static BYTE getCmdChecksum(Command_TypeDef * cmd)
{
	BYTE checksum;
	checksum = cmd->address+cmd->sn+cmd->command+cmd->data_1+cmd->data_2+cmd->data_3+cmd->data_4;
	return checksum;
}
#endif
int mst_readInputSourceCh(char ch);


int valueFrom8028;

int ReceiveBuffer_Mst(char *buffer, int size, int timeout)
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

static void process_mst6m48_cmd(Command_TypeDef * cmd)
{
	deviceIsConnet = 1;
	#if 0
	if(cmd->checksum != getCmdChecksum(cmd))
	{
		return;
	}
	#endif
	
	//printf("process_mst6m48_cmd:%x,%x,%d,%d,%d---->%d\r\n",cmd->command,cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,GetSysTick());
	
	switch(cmd->command)
	{
	case 0x68:
		switch(cmd->data_1)
		{
		case 0x10:
		case 0x11:
			mst6m48_Setting.enableDisplay = cmd->data_2;
			break;
		case 0x13:
			mst6m48_Setting.deviceState = cmd->data_2;
			break;
		case 0x20:
			mst6m48_Setting.updateState = cmd->data_2;
			break;			
		}
		break;
	case 0x6a:
		switch(cmd->data_1)
		{
		case 0x01:
			mst6m48_Setting.ver_msb = cmd->data_2;
			mst6m48_Setting.ver_lsb = cmd->data_3;
			break;	
		//case 0x10:
		//case 0x11:
			// read write sn
		//	mst6m48_Setting.sn_msb = cmd->data_2;
		//	mst6m48_Setting.sn_lsb = cmd->data_3;
		//	if(getDeviceSN() != ((mst6m48_Setting.sn_msb<<8)+mst6m48_Setting.sn_lsb))
		//	{
		//		saveDeviceSN((mst6m48_Setting.sn_msb<<8)+mst6m48_Setting.sn_lsb);
		//	}
		//	break;
			default:
				break;
		}
		break;
	#if 1
	case 0x6B:
		switch(cmd->data_1)
		{
		case 0x01:
			GetUserSetting()->pipMode = cmd->data_2;
			break;
		case 0x03:
			CurrentWindow = cmd->data_2;
			break;
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			GetUserSetting()->swapMainPip = cmd->data_2;
			break;
		}
		break;
	case 0x6c:
		mst6m48_Setting.regValue = cmd->data_3+(cmd->data_4<<8);
		break;
		
	case 0x50:
		//break;
	case 0x51:
		// scale x position width
		CurrentScaleX = cmd->data_1 + (cmd->data_2<<8);
		CurrentScaleWidth = cmd->data_3 + (cmd->data_4<<8);
		break;
	case 0x52:
		//break;
	case 0x53:
		// scale y position height
		CurrentScaleY = cmd->data_1 + (cmd->data_2<<8);
		CurrentScaleHeight = cmd->data_3 + (cmd->data_4<<8);
		break;
	case 0x54:
	//case 0x55:
		// zoom
		CurrentZoomX = cmd->data_1;
		CurrentZoomY = cmd->data_2;
		CurrentZoomWidth = cmd->data_3;
		CurrentZoomHeight = cmd->data_4;
		break;
	case 0x55:
		mst6m48_Setting.zoom[IMAGE_X] = cmd->data_1;
		mst6m48_Setting.zoom[IMAGE_Y] = cmd->data_2;
		mst6m48_Setting.zoom[IMAGE_WIDTH] = cmd->data_3;
		mst6m48_Setting.zoom[IMAGE_HEIGHT] = cmd->data_4;
		break;
	case 0x56:
	//case 0x57:
		// crop
		#if 0
		CurrentCropX = cmd->data_1;
		CurrentCropY = cmd->data_2;
		CurrentCropWidth = cmd->data_3;
		CurrentCropHeight = cmd->data_4;
		#else
		#if 0
		if((cmd->data_1 > 0)&&(cmd->data_1 < 4))
		{
			CurrentCrop[cmd->data_1] =  cmd->data_2 + (cmd->data_3<<8);
		}
		#else
		{
			int value = cmd->data_2 + (cmd->data_3<<8);
			switch(cmd->data_1)
			{
				case 0:
					CurrentCropWidth = value;
					break;
				case 1:
					CurrentCropHeight = value;
					break;
				case 2:
					CurrentCropX = value;
					break;
				case 3:
					CurrentCropY = value;
					break;
			}
		}
		#endif
		#endif
		break;
	case 0x57:
		#if 0
		mst6m48_Setting.crop[IMAGE_X] = cmd->data_1;
		mst6m48_Setting.crop[IMAGE_Y] = cmd->data_2;
		mst6m48_Setting.crop[IMAGE_WIDTH] = cmd->data_3;
		mst6m48_Setting.crop[IMAGE_HEIGHT] = cmd->data_4;
		#else
		mst6m48_Setting.crop[cmd->data_1] =  cmd->data_2 + (cmd->data_3<<8);
		#endif
		break;
	#endif

	#if 1
	case 0x5c:
	case 0x5e:
		//if((cmd->data_1 > 0)&&(cmd->data_1 < 4))
		if(cmd->data_1 < 4)
		{
			int value = cmd->data_2 + (cmd->data_3<<8);
			#if 1
			#if 0
			switch(cmd->data_1)
			{
				case 0x00:
					CurrentCropWidthDot = value;
					break;
				case 0x01:
					CurrentCropHeightDot = value;
					break;
				case 0x02:
					CurrentCropXDot = value;
					break;
				case 0x03:
					CurrentCropYDot = value;
					break;
			}
			#else
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
			#endif
			#else
			mst6m48_Setting.cropDotInput[cmd->data_1] = value;
			switch(cmd->data_1)
			{
				case 0x00:
					CurrentCropWidthDotInput = value;
					break;
				case 0x01:
					CurrentCropHeightDotInput = value;
					break;
				case 0x02:
					CurrentCropXDotInput = value;
					break;
				case 0x03:
					CurrentCropYDotInput = value;
					break;
			}
			#endif
		}
		break;
		#endif
	case 0x5d:
	case 0x5f:
		mst6m48_Setting.cropDot[cmd->data_1] =	cmd->data_2 + (cmd->data_3<<8);
		break;
	#if 0	
	case 0x58:
	case 0x59:
		// screen x position width
		CurScreenX = cmd->data_1 + (cmd->data_2<<8);
		CurScreenWidth = cmd->data_3 + (cmd->data_4<<8);
		break;
	case 0x5a:
	case 0x5b:
		// screen y position height
		CurScreenY = cmd->data_1 + (cmd->data_2<<8);
		CurScreenHeight = cmd->data_3 + (cmd->data_4<<8);
		break;
		#endif
	case 0x72:
		switch(cmd->data_1)
		{
			case 0x00:
			case 0x01:
				// 读写输入源			
				//GetUserSetting()->inputSource[CurrentWindow] = cmd->data_3;
				mst6m48_Setting.inputSource[CurrentWindow]  = cmd->data_3;
				break;
			case 0x13:
				mst6m48_Setting.regValue = cmd->data_2;
				break;
			case 0x15: //读色空间
				valueFrom8028 = cmd->data_2;
				break;
			case 0x1e:
			case 0x1f: //读通道信号源
				mst6m48_Setting.regValue = cmd->data_3;
				mst6m48_Setting.ChSource[cmd->data_2&0x01] = cmd->data_3;
				break;			
			case 0x21: 
				mst6m48_Setting.regValue = cmd->data_2;
				break;
		} 
		break;
		#if 1
	case 0x73:
		switch(cmd->data_1)
		{
		case 0x01:
			// 读输入源信号格式
			/*
			if(mst6m48_Setting.inputFormat[CurrentWindow] != cmd->data_2)
			{
				mst6m48_Setting.inputFormat[CurrentWindow] = cmd->data_2;
			}
			*/
			mst6m48_Setting.regValue = cmd->data_2;
			break;
		case 0x03:
			// 读输入源timming
			mst6m48_Setting.inputTimming[cmd->data_2&0x03] = cmd->data_3+(cmd->data_4<<8);
			break;
		case 0x07:
			mst6m48_Setting.regValue = cmd->data_3+(cmd->data_4<<8);
			break;

		case 0x09:
			mst6m48_Setting.regValue = cmd->data_2;	
			break;
			
		case 0x0b:
			mst6m48_Setting.regValue = cmd->data_2; 
			break;
			
		case 0x0c:
		case 0x0d:
			mst6m48_Setting.regValue = cmd->data_2;
			break;
		}
		break;
	case 0x74:
		switch(cmd->data_1)
		{
		case 0x00:
			// 读写输出信号格式
			//GetUserSetting()->outputFormat = cmd->data_2;
			break;
		case 0x01:
			mst6m48_Setting.regValue = cmd->data_2;
			break;
		case 0x07:
			mst6m48_Setting.frameLockMode = cmd->data_2;
			break;
		case 0x09:
			mst6m48_Setting.frameLockSource = cmd->data_2;
			break;
		case 0x0f:
			mst6m48_Setting.outputColorSpace = cmd->data_2;
			break;
		case 0x0c:
			switch(cmd->data_2)
			{
				case 7:
					mst6m48_Setting.cropSplitEnable = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCropEnable = mst6m48_Setting.cropSplitEnable;
					break;
				case 8:
					mst6m48_Setting.cropSplit[0] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.sizeX = mst6m48_Setting.cropSplit[0] ;
					break;
				case 9:
					mst6m48_Setting.cropSplit[1] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.sizeY = mst6m48_Setting.cropSplit[0] ;
					break;
				case 10:
					mst6m48_Setting.cropSplit[2] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.posX = mst6m48_Setting.cropSplit[0] ;
					break;
				case 11:					
					mst6m48_Setting.cropSplit[3] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.posY = mst6m48_Setting.cropSplit[0] ;
					break;
			}
			break;
		case 0x0d:
			switch(cmd->data_2)
			{
				case 6:
					mst6m48_Setting.vLockLine = cmd->data_3+(cmd->data_4<<8);
					break;
				case 7:
					mst6m48_Setting.cropSplitEnable = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCropEnable = mst6m48_Setting.cropSplitEnable;
					break;
				case 8:
					mst6m48_Setting.cropSplit[0] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.sizeX = mst6m48_Setting.cropSplit[0] ;
					break;
				case 9:
					mst6m48_Setting.cropSplit[1] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.sizeY = mst6m48_Setting.cropSplit[0] ;
					break;
				case 10:
					mst6m48_Setting.cropSplit[2] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.posX = mst6m48_Setting.cropSplit[0] ;
					break;
				case 11:					
					mst6m48_Setting.cropSplit[3] = cmd->data_3+(cmd->data_4<<8);
					GetUserSetting()->splitCrop.posY = mst6m48_Setting.cropSplit[0] ;
					break;
			}
			break;
		case 0x13:
			switch(cmd->data_2)
			{
				case 0:
					//mst6m48_customVideo.htotal = cmd->data_3+(cmd->data_4<<8);//mst6m48_Setting.
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
						WORD *p_int = &mst6m48_customVideo.htotal;
						value = cmd->data_3+(cmd->data_4<<8);
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
						BYTE *p_char = &mst6m48_customVideo.vfreq;
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
						WORD *p_int = &mst6m48_customResolution.DisplayWidth;
						value = cmd->data_3+(cmd->data_4<<8);
						*(p_int + cmd->data_2-8) = value;
					}
					break;
			}
			break;
		case 0x14:
		case 0x15:
			mst6m48_Setting.regValue = cmd->data_3+(cmd->data_4<<8);
			break;
		}
		break;
		#endif
	case 0x76:
		switch(cmd->data_1)
		{
			#if 1
			case 0:
			case 1:
				FadeAlpha = cmd->data_2;
				break;
			case 3:
				FadeTime = cmd->data_2+(cmd->data_3<<8);
				break;
			case 5:
				mst6m48_Setting.regValue = cmd->data_2+(cmd->data_3<<8);
				break;
			case 8: 
			case 9:
				mst6m48_Setting.switchMode = cmd->data_2;
				if(cmd->data_2 == 0)
				{
					mst6m48_Setting.regValue = 1;//SwitchMode
				}
				else if(cmd->data_2 == 1)
				{
					mst6m48_Setting.regValue = 0;//SwitchMode
				}
				else
				{
					mst6m48_Setting.regValue = cmd->data_2;//SwitchMode
				}
				break;
			#endif
			case 0x0b:
				SwitchTo = cmd->data_2;
				mst6m48_Setting.switchTo = cmd->data_2;
				break;
			case 0x0d:
				mst6m48_Setting.currentCh = cmd->data_2;
				break;
		}
		break;
	case 0x77:
		switch(cmd->data_1)
		{
		case 1:
			mst6m48_Setting.regValue = cmd->data_2;//GetUserSetting()->gamma
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
			case 0x10:
			case 0x11:				
				mst6m48_Setting.regValue = cmd->data_2;
				break;
		}
		break;
	case 0x80:
		switch(cmd->data_1)
		{
			#if 0
		case 0x00:
		//case 0x01:
			// 读写亮度
			CurBrightnessR = cmd->data_2;
			CurBrightnessG = cmd->data_3;
			CurBrightnessB = cmd->data_4;
			break;
			#endif
		case 0x01:
			mst6m48_Setting.brightness[GetUserSetting()->currentWindow][COLOR_R] = cmd->data_2;
			mst6m48_Setting.brightness[GetUserSetting()->currentWindow][COLOR_G] = cmd->data_2;
			mst6m48_Setting.brightness[GetUserSetting()->currentWindow][COLOR_B] = cmd->data_2;
		//case 0x02:
		case 0x03:
			//读写对比度
			CurContrastR = cmd->data_2;
			CurContrastG = cmd->data_3;
			CurContrastB = cmd->data_4;
			break;
		//case 0x04:
		case 0x05:
			//读写色温//20110906
			CurColorTempR = cmd->data_2;
			CurColorTempG = cmd->data_3;
			CurColorTempB = cmd->data_4;
			break;
		case  0x06:
		case  0x07:
			//读写锐度//20110906
			CurSharpness =cmd->data_2;
			break;
		case  0x08:
		case  0x09:
			//读写饱和度//20110909
			CurSaturation =cmd->data_2;
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
		case 0x08: 
		case 0x09:
			GetUserSetting()->hdmiAudio = cmd->data_2;
			break;
		case 0x0a:
		case 0x0b:
			mst6m48_Setting.TTLInputAudio =  cmd->data_2;
			break;
		case 0x0c:
		case 0x0d: 
			mst6m48_Setting.HDMIInputAudio =  cmd->data_2;
			break;
		//case 0x10: 
		case 0x11:
			GetUserSetting()->dpAudio = cmd->data_2;
			break;
		case 0x12:
		case 0x13:
			mst6m48_Setting.regValue =  cmd->data_2;
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
		case 0xf1:
			switch(cmd->data_1)
			{
				case 0x01:
					{
						char receiveBuffer[20],len;
						len = cmd->data_3 + 4;
						if(ReceiveBuffer_Mst(receiveBuffer, 10, 200) == len)
						//ReceiveBuffer_Mst(receiveBuffer, 20, 200);
						{
							#if 1
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
							printf("receive core:%x,%x,%x,%x,%x,%x--->%d\r\n",(receiveBuffer[4]-1),(receiveBuffer[5]-1),(receiveBuffer[6]-1),(receiveBuffer[7]-1),(receiveBuffer[8]-1),(receiveBuffer[9]-1),GetSysTick());
							#endif
							mst6m48_Setting.ChSource[MAIN_WINDOW] = receiveBuffer[4]-1; //前4字节是8028的帧头
							mst6m48_Setting.ChSource[PIP_WINDOW] = receiveBuffer[5]-1;
							mst6m48_Setting.inputFormat[MAIN_WINDOW] = receiveBuffer[6]-1;
							mst6m48_Setting.inputFormat[PIP_WINDOW] = receiveBuffer[7]-1;
							mst6m48_Setting.currentCh = receiveBuffer[8]-1;
							mst6m48_Setting.displayMode = receiveBuffer[9]-1;
							
							GetUserSetting()->inputSourceReadFromVideo[0] = mst_readInputSourceCh(0);
							GetUserSetting()->inputSourceReadFromVideo[1] = mst_readInputSourceCh(1);
							GetUserSetting()->displayMode[0] = mst6m48_Setting.displayMode;
						}
					}					
					break;
			}
			break;
		case 0xf2:
			mst6m48_Setting.dat1Value = cmd->data_1;
			mst6m48_Setting.dat2Value = cmd->data_2;
			mst6m48_Setting.dat3Value = cmd->data_3;
			mst6m48_Setting.dat4Value = cmd->data_4;
			break;
		case 0xf4:
		case 0xf5:
			mst6m48_Setting.regValue = cmd->data_3+(cmd->data_4<<8);
			break;
	}
}



static void interpret_mst6m48_char(u8 value)
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
		if(value == mst6m48_cmd_header[index++])
		{
			if(index >= sizeof(mst6m48_cmd_header))
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
			cmd_from_mst6m48.address= HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 3:
			hexCharBuffer[1] = value;
			cmd_from_mst6m48.sn = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 5:
			hexCharBuffer[1] = value;
			cmd_from_mst6m48.command = HexCharToByte(hexCharBuffer);
			++index;
			break;			
		case 7:
			hexCharBuffer[1] = value;
			cmd_from_mst6m48.data_1 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 9:
			hexCharBuffer[1] = value;
			cmd_from_mst6m48.data_2 = HexCharToByte(hexCharBuffer);
			++index;
			break;	
		case 11:
			hexCharBuffer[1] = value;
			cmd_from_mst6m48.data_3 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 13:
			hexCharBuffer[1] = value;
			cmd_from_mst6m48.data_4 = HexCharToByte(hexCharBuffer);
			++index;
			break;
		case 15:
			// get checksum no use
			hexCharBuffer[1] = value;
			cmd_from_mst6m48.checksum = HexCharToByte(hexCharBuffer);
			state = interpret_cmd_end;
			index = 0;
			break;
		default:
			state = interpret_cmd_init;
			break;
		}
		break;
	case interpret_cmd_end:
		if(value == mst6m48_cmd_end[index])
		{
			++index;
			if(index >= sizeof(mst6m48_cmd_end))
			{
				state = interpret_cmd_init;
				// active cmd
				receive_sn = cmd_from_mst6m48.sn;
				//DebugPrintf("active=%d\r\n", receive_preamp_count);
				process_mst6m48_cmd(&cmd_from_mst6m48);
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
void task_mst6m48(void)
{
	u8 value;
	#if 1
	while(pipe_read(&receive_preamp_pipe,&value))
	{
		//UARTSendByte(0, value);  // for test
		interpret_mst6m48_char(value);
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

void Isr_receive_mst6m48(u8 value)
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

Mst6m48_Setting_TypeDef * getMst6m48Setting(void)
{
	return &mst6m48_Setting;
}


/*
 *	 "<T0000828004010007>"
 *     "<F0000828004010007>"
 */
#if 0
unsigned int stCheckSum(BYTE *buffer, int size)
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
#endif
bool send_cmd_to_mst6m48_no_wait(Command_TypeDef *cmdStruct)
{
	char buffer[20],repeat = 3;
	u32 startTime;
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
	buffer[8] = stCheckSum((BYTE *)buffer, 8);
	UARTSendBuffer(MST6M48_UART_PORT, buffer, 9);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	//UARTSend(0, (BYTE *)buffer, 9);
	
	#else
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MST6M48_UART_PORT, buffer);
	#endif
	return TRUE;
}
bool send_cmd_to_mst6m48(Command_TypeDef *cmdStruct)
{
	char buffer[20],repeat = 3;
	u32 startTime;
		
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
	buffer[8] = stCheckSum((BYTE *)buffer, 8);
	UARTSendBuffer(MST6M48_UART_PORT, buffer, 9);
	//STDP9320_DBG("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	//UARTSend(0, (BYTE *)buffer, 9);
	
	#if 1 //判断不能跟video通信,则不再尝试恢复通信,避免造成设备卡顿
	//if(deviceIsConnet == 0)//((getdeViceIsConnet() + getVideoStatus()) != 2)
	if((deviceIsConnet == 0)&&(forceCommFlag == 0))
	{
		return FALSE;
	}
	#endif
	
	#else
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MST6M48_UART_PORT, buffer);
	#endif
	task_mst6m48();
	
	startTime = GetSysTick();
	
	
	// wait ack
	while((send_sn != receive_sn) && deviceIsConnet && repeat)
	{
		task_mst6m48();
		if(isSysTickTimeout(startTime,2000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd from video process = %d,receive_sn: %d\r\n", send_sn,receive_sn);
			if(repeat)
			{
				printf("repeat: %d\r\n", repeat);
				UARTSendBuffer(MST6M48_UART_PORT, buffer, 9);
				printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
				task_mst6m48();
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
			task_mst6m48();
			if(isSysTickTimeout(startTime,500))
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
void send_cmd_to_mst6m48_Long_no_wait(Command_TypeDef *cmdStruct,char *buf,char len)
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
	buffer[sendLen-1] = stCheckSum((BYTE *)buffer, (sendLen-1));
	UARTSendBuffer(MST6M48_UART_PORT, buffer, sendLen);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	//UARTSend(0, (BYTE *)buffer, 9);
	
	#else
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MST6M48_UART_PORT, buffer);
	#endif
}
bool send_cmd_to_mst6m48_Long(Command_TypeDef *cmdStruct,char *buf,char len)
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
	buffer[sendLen-1] = stCheckSum((BYTE *)buffer, (sendLen-1));
	UARTSendBuffer(MST6M48_UART_PORT, buffer, sendLen);
	//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
	//UARTSend(0, (BYTE *)buffer, 9);
	
	#else
	cmdStruct->checksum = getCmdChecksum(cmdStruct);
	sprintf(buffer, "<T%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x>", 
		cmdStruct->address, cmdStruct->sn, cmdStruct->command, cmdStruct->data_1, cmdStruct->data_2, cmdStruct->data_3, cmdStruct->data_4, cmdStruct->checksum);
	UARTSendStr(MST6M48_UART_PORT, buffer);
	#endif
	task_mst6m48();
	
	startTime = GetSysTick();
	
	
	// wait ack
	while((send_sn != receive_sn) && deviceIsConnet && repeat)
	{
		task_mst6m48();
		if(isSysTickTimeout(startTime,2000))
		{
			repeat--;
			//printf("send mst6m48:%d,%x,%x,%x,%x,%x\r\n",cmdStruct->sn,cmdStruct->command,cmdStruct->data_1,cmdStruct->data_2,cmdStruct->data_3,cmdStruct->data_4);
			printf("can't receive cmd long from video process = %d,receive_sn: %d\r\n", send_sn,receive_sn);
			if(repeat)
			{
				printf("repeat  long : %d\r\n", repeat);
				UARTSendBuffer(MST6M48_UART_PORT, buffer, sendLen);
				task_mst6m48();
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
			task_mst6m48();
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

bool send_cmd_to_mst6m48_and_wait(Command_TypeDef *cmdStruct)
{
	
	return send_cmd_to_mst6m48(cmdStruct);

}



void mst_enableDisplay(u8 enable)
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x10;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}


void mst_resetFactory(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x06;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_loadSetting(int index)  // 1 ~ 10
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x09;
	cmd.data_2 = index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_saveSetting(int index) // 1 ~ 10
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x08;
	cmd.data_2 = index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}


void mst_setScaleH(u16 position, u16 width)
{
	Command_TypeDef cmd;	
	cmd.command = 0x50;
	cmd.data_1 = position & 0xff;
	cmd.data_2 = position >> 8;
	cmd.data_3 = width & 0xff;
	cmd.data_4 = width >> 8;	
	//send_cmd_to_mst6m48_no_wait(&cmd);
	send_cmd_to_mst6m48(&cmd);
}
void mst_setScaleV(u16 position, u16 height)
{
	Command_TypeDef cmd;	
	cmd.command = 0x52;
	cmd.data_1 = position & 0xff;
	cmd.data_2 = position >> 8;
	cmd.data_3 = height & 0xff;
	cmd.data_4 = height >> 8;	
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}

static char isSetScale = 0;
void mst_setScaleX(int value)
{
	
	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 6;
	cmd.data_2 = 2;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);

	isSetScale = 1;
}
void mst_setScaleXSingle(int value)
{
	
	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 0;
	cmd.data_2 = 2;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}
int mst_getScaleX(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x6c;
	cmd.data_1 = 1;
	cmd.data_2 = 2;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setScaleY(int value)
{

	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 6;
	cmd.data_2 = 3;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
	isSetScale = 1;
}
void mst_setScaleYSingle(int value)
{

	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 0;
	cmd.data_2 = 3;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}
int mst_getScaleY(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x6c;
	cmd.data_1 = 1;
	cmd.data_2 = 3;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setScaleWidth(int value)
{
	
	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 6;
	cmd.data_2 = 0;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
	isSetScale = 1;
}
void mst_setScaleWidthSingle(int value)
{
	
	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 0;
	cmd.data_2 = 0;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}
int mst_getScaleWidth(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x6c;
	cmd.data_1 = 1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setScaleHeight(int value)
{
	
	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 6;
	cmd.data_2 = 1;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
	isSetScale = 1;
}
void mst_setScaleHeightSingle(int value)
{
	
	Command_TypeDef cmd;	
	cmd.command = 0x6c;
	cmd.data_1 = 0;
	cmd.data_2 = 1;
	cmd.data_3 = value & 0xff;
	cmd.data_4 = value >> 8;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}
int mst_getScaleHeight(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x6c;
	cmd.data_1 = 1;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setScaleSetting(void)
{
	Command_TypeDef cmd;	

	if(isSetScale == 0) return;

	isSetScale = 0;
		
	
	cmd.command = 0x6c;
	cmd.data_1 = 6;
	cmd.data_2 = 4;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}

void mst_setZoom(u16 x, u16 y, u16 width, u16 height)
{
	Command_TypeDef cmd;
	cmd.command = 0x54;
	cmd.data_1 = x;
	cmd.data_2 = y;
	cmd.data_3 = width;
	cmd.data_4 = height;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_getZoom(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x55;
	cmd.data_1 = 0;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
#if 1
void mst_setCrop(u16 x, u16 y, u16 width, u16 height)
{
	Command_TypeDef cmd;
	//printf("++mst_setCrop(%d,%d,%d,%d)\r\n",x,y,width,height);
	cmd.command = 0x56;
	cmd.data_1 = x;
	cmd.data_2 = y;
	cmd.data_3 = width;
	cmd.data_4 = height;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_getCrop(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x57;
	cmd.data_1 = 0;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
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
void mst_setCropDot(int index, int value) //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--set crop single
{
	Command_TypeDef cmd;
	//printf("mst_setCropDot_%d_%d ",index,value);
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
	//printf("%d \r\n",value);
	#if 0//(GetUserSetting()->splitOnOff == 0)
	{
		cmd.command = 0x5e; 	//针对输出分辨率的裁剪
	}
	#else
	{
		cmd.command = 0x5c; 	//针对输入分辨率的裁剪
	}
	#endif
	cmd.data_1 = index;
	cmd.data_2 = value&0xff;
	cmd.data_3 = value>>8;
	if(index == 5)
	{
		cmd.data_4 = value; 
	}
	else
	{
		cmd.data_4 = 0; 
	}
	send_cmd_to_mst6m48(&cmd);
}
int  mst_getCropDot(int index) //  0 -- width,1-- height,2-- x, 3 -- y, 
{
	Command_TypeDef cmd;
	#if 0//(GetUserSetting()->splitOnOff == 0)
	{
		cmd.command = 0x5f; 	//针对输出分辨率的裁剪
	}
	#else
	{
		cmd.command = 0x5d; 	//针对输入分辨率的裁剪
	}
	#endif
	cmd.data_1 = index;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.crop[index];
}
#endif

void mst_setCropDotCh(char ch) //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--enable save crop ratio
{
	Command_TypeDef cmd;
	printf("mst_setCropDotCh_%d \r\n",ch);
	cmd.command = 0x5c;
	cmd.data_1 = 5;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = ch;
	send_cmd_to_mst6m48(&cmd);
}

void mst_setScreenH(u16 position, u16 width)
{
	Command_TypeDef cmd;
	cmd.command = 0x58;
	cmd.data_1 = position & 0xff;
	cmd.data_2 = position >> 8;
	cmd.data_3 = width & 0xff;
	cmd.data_4 = width >> 8;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setScreenV(u16 position, u16 height)
{
	Command_TypeDef cmd;
	cmd.command = 0x5a;
	cmd.data_1 = position & 0xff;
	cmd.data_2 = position >> 8;
	cmd.data_3 = height & 0xff;
	cmd.data_4 = height >> 8;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setInputSource_Main(u8 source)
{
	Command_TypeDef cmd;
	//printf("mst_setInputSource_Main(%d)",source);

	cmd.command = 0x72;
	cmd.data_1 = 0x1e;
	cmd.data_2 = 2;
	cmd.data_3 = source;
	cmd.data_4 = 0;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}
void mst_setInputSource_PIP(u8 source)
{
	Command_TypeDef cmd;
	//printf("mst_setInputSource_PIP(%d)",source);
	cmd.command = 0x72;
	cmd.data_1 = 0x1e;
	cmd.data_2 = 1;
	cmd.data_3 = source;
	cmd.data_4 = 0;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}
void mst_setInputSource(u8 source)
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 00;
	cmd.data_2 = 00;
	cmd.data_3 = source;
	cmd.data_4 = 0;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
	
}
#if 1
char inputTableFromMst[INPUT_SIZE] = 
{
	//表格的上半部分的值是输入信号对应的8028的信号源的序号
	0, //INPUT_CV3,                   
	10,//INPUT_SDI       
	6, //INPUT_VGA2     
	6, //INPUT_VGA	   
#if (CURRENT_DEVICE == _VIP_SMART)
	2, //INPUT_DVI	   ---对应4604的输出通道 
#else
	4, //INPUT_DVI,  
#endif
	3, //INPUT_YCbCr,                    
	1, //INPUT_TTL,   
#if (CURRENT_DEVICE == _VIP_SMART)
	3, //INPUT_HDMI	   ---对应4604的输出通道
#else	
	2, //INPUT_HDMI, 
#endif                     
	10,      // INPUT_YCbCr,                  
	5, //SP1U_INPUT_DP,         //INPUT_DP,                     
	10,      // INPUT_TTL,                     
	10,      // INPUT_YPbPr2,                  
	10,      // INPUT_HDMI,       			 
	10,      // INPUT_VGA2,	                   
	10,      // INPUT_YCbCr2,	 
 };


char inputTableFromMstFront1[INPUT_SIZE] = 
{
	//表格的上半部分的值是输入信号对应的8028的信号源的序号
	0, //INPUT_CV1,                     // 0
	10,//VSP1U_INPUT_SIZE      // INPUT_SV,                    
	5, //VSP1U_INPUT_DVI      // INPUT_SDI,                  
	5, //VSP1U_INPUT_DVI      // INPUT_DVI,					   
	4, //INPUT_YPbPr,                   
	3, //INPUT_VGA,                    
	1, //INPUT_CV2,                    
	2, //INPUT_CV3,                     
	10,//VSP1U_INPUT_SIZE      // INPUT_YCbCr,                  
	6, //VSP1U_INPUT_DP,         //INPUT_DP,                     
	10,//VSP1U_INPUT_SIZE      // INPUT_TTL,                     
	10,//VSP1U_INPUT_SIZE      // INPUT_YPbPr2,                  
	10,//VSP1U_INPUT_SIZE      // INPUT_HDMI,       			 
	10,//VSP1U_INPUT_SIZE      // INPUT_VGA2,	                   
	10,//VSP1U_INPUT_SIZE      // INPUT_YCbCr2,	               
};

#endif
int mst_readInputSource(void)
{
#if !(CURRENT_DEVICE == _VIP_SMART)
	if((mst6m48_Setting.inputSource[CurrentWindow] == INPUT_SDI)||(mst6m48_Setting.inputSource[CurrentWindow] == INPUT_TTL))
	{
		return getMB103_inputSource(0);
	}
	else if(mst6m48_Setting.inputSource[CurrentWindow] == INPUT_DVI)
	{
		return getMB103_inputSource(1);
	}
	else
#endif
	{
		return inputTableFromMst[mst6m48_Setting.inputSource[CurrentWindow]];
	}

	return 0;
}
int mst_readInputSourceCh(char ch)
{
#if !(CURRENT_DEVICE == _VIP_SMART)
	if((mst6m48_Setting.ChSource[ch] == INPUT_SDI)||(mst6m48_Setting.ChSource[ch] == INPUT_TTL))
	{
		return getMB103_inputSource(0);
	}
	else if(mst6m48_Setting.ChSource[ch] == INPUT_DVI)
	{
		return getMB103_inputSource(1);
	}
	else
#endif
	{
		if(getFrontType() == 2)
		{
			return inputTableFromMst[mst6m48_Setting.ChSource[ch]];
		}
		else
		{
			return inputTableFromMstFront1[mst6m48_Setting.ChSource[ch]];
		}
	}
}
void mst_getInputSource(void)
{
	//int curret_source;
	//curret_source = GetUserSetting()->inputSource[CurrentWindow];
	
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 01;
	cmd.data_2 = 00;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);

	//printf("mst_getInputSource() = %d\r\n",mst6m48_Setting.inputSource[CurrentWindow]);
	GetUserSetting()->inputSource[CurrentWindow] = mst_readInputSource();//mst6m48_Setting.inputSource[CurrentWindow];
}

int mst_getInputSource_Main(void)
{	
	Command_TypeDef cmd;
	int res;
	cmd.command = 0x72;
	cmd.data_1 = 0x1f;
	cmd.data_2 = 00;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);

	res = mst_readInputSourceCh(0);//mst6m48_Setting.ChSource[0];//mst6m48_Setting.regValue;
	GetUserSetting()->inputSourceReadFromVideo[0] = res;
	return res;
}
int mst_getInputSource_pip(void)
{	
	Command_TypeDef cmd;
	int res;
	cmd.command = 0x72;
	cmd.data_1 = 0x1f;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);

	res = mst_readInputSourceCh(1);//mst6m48_Setting.ChSource[1];//mst6m48_Setting.regValue;
	GetUserSetting()->inputSourceReadFromVideo[1] = res;
	return res;
}
int mst_getInputSourceCh(char ch)//ch 0-main 1-pip
{	
	Command_TypeDef cmd;
	int res;
	cmd.command = 0x72;
	cmd.data_1 = 0x1f;
	cmd.data_2 = ch;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);

	res = mst_readInputSourceCh(ch);//mst6m48_Setting.ChSource[0];//mst6m48_Setting.regValue;
	GetUserSetting()->inputSourceReadFromVideo[ch] = res;
	return res;
}

int mst_getInputSource_notSave(void)
{	
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 01;
	cmd.data_2 = 00;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	
	return mst_readInputSource();
}

void mst_setEnableExternMatrixCVBS(int enable)
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x10;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setEnableTPSelf(int enable)
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x16;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setInterlaceSwitch(int value) // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x12;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
BYTE mst_getInterlaceSwitch(void) // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x13;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setInputColorSpace_TTL(int value)// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x14;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

int mst_getInputColorSpace_TTL(void)// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x15;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return valueFrom8028;
}

void mst_waitDeviceReady(void)
{
	int count = 0;
	while(TRUE)
	{
		if(mst6m48_Setting.deviceState == DEVICE_READY)
		{
			return;
		}
		else
		{
			task_mst6m48();
			count++;
			SysDelay1ms(100);
			if(count > 180)
			{
				return;
			}
		}
	}
}

void mst_SetCurrentWindow(int value)
{
	Command_TypeDef cmd;
	cmd.command = 0x6B;
	cmd.data_1 = 02;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

u8 mst_getInputFormat(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	//printf("++++++GetInputFormat__%d",mst6m48_Setting.regValue);//mst6m48_Setting.inputFormat[CurrentWindow]
	return mst6m48_Setting.regValue;//mst6m48_Setting.inputFormat[CurrentWindow];
}

u8 mst_getInputFormatCh(BYTE ch)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x0b;
	cmd.data_2 = ch;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setInputTimming(int type, WORD value)// type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 02;
	cmd.data_2 = type;
	cmd.data_3 = value;
	cmd.data_4 = value>>8;	
	send_cmd_to_mst6m48(&cmd);
}
WORD mst_getInputTimming(int type) // type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 03;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.inputTimming[type&0x03];
}

void mst_resetSdiInputtimming(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 04;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_resetAllSdi(int type)	// 0 -- FPGA SDI, 1 -- GS2971 SDI
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 06;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setOutputFormat(u8 format)
{
	
	Command_TypeDef cmd;
	#if ENABLE_USER_DEFINE
	if(format == MD_1024x768_59_94)
	{
		format = MD_1024x768_60;
	}
	#endif

	cmd.command = 0x74;
	cmd.data_1 = 00;
	cmd.data_2 = format;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	
	mst6m48_Setting.deviceState = DEVICE_INIT;
	
	send_cmd_to_mst6m48(&cmd);
}
BYTE mst_getOutputFormat(void)
{
	
	Command_TypeDef cmd;

	cmd.command = 0x74;
	cmd.data_1 = 1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
		
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setOutputMode(u8 mode)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 4;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);
}
void mst_SetOutputColorSpace(int value) // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x0e;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);
}
int  mst_GetOutputColorSpace(void) // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 0x0f;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);

	return mst6m48_Setting.outputColorSpace;
}


void mst_setFrameLockMode(u8 mode) // 0 -- free run, 1 -- Fixed Frame Lock, 2 -- Dynamic Frame Lock
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 6;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);
}
int  mst_getFrameLockMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 7;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);

	return mst6m48_Setting.frameLockMode;
}
void mst_setFrameLockSource(u8 source) // 0 -- frame lock main , 1 -- frame lock pip
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 8;
	cmd.data_2 = source;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);
}
int  mst_getFrameLockSource(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;
	cmd.data_1 = 9;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);

	return mst6m48_Setting.frameLockSource;
}
void mst_setDisplayMode(u8 mode)
{
	Command_TypeDef cmd;
	//printf("mst_setDisplayMode(%d,%d)\r\n",CurrentWindow, mode);
	cmd.command = 0x79;
	cmd.data_1 = 0;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}

int mst_getDisplayMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return CurDisplayMode;
}

void mst_setAutoBackGroundControlEn(u8 enable)
{
	Command_TypeDef cmd;
	//printf("mst_setDisplayMode(%d,%d)\r\n",CurrentWindow, mode);
	cmd.command = 0x79;
	cmd.data_1 = 0x10;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

int mst_getAutoBackGroundControlEn(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x10;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
void mst_setDisplayModeFreezeChannel(bool freeze,char channel)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x12;
	cmd.data_2 = 0x00;
	cmd.data_3 = freeze;
	cmd.data_4 = channel;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setDisplayModeFreezeCurrent(bool freeze)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x12;
	cmd.data_2 = 0x01;
	cmd.data_3 = freeze;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setDisplayModeFreezeTheOther(bool freeze)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x12;
	cmd.data_2 = 0x02;
	cmd.data_3 = freeze;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setDisplayModeFreezeAll(bool freeze)
{
	Command_TypeDef cmd;
	printf("mst_setDisplayModeFreezeAll(%d)\r\n",freeze);
	cmd.command = 0x79;
	cmd.data_1 = 0x12;
	cmd.data_2 = 0x03;
	cmd.data_3 = freeze;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setFlatColor(u8 r, u8 g, u8 b)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 2;
	cmd.data_2 = r;
	cmd.data_3 = g;
	cmd.data_4 = b;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setTestPatternMode(u8 mode)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 6;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}
void mst_setTestPatternColor(u8 r, u8 g, u8 b)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x08;
	cmd.data_2 = r;
	cmd.data_3 = g;
	cmd.data_4 = b;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setTestPatternParam(u8 horStep, u8 verStep, u8 stepColor)
{
	Command_TypeDef cmd;
	cmd.command = 0x79;
	cmd.data_1 = 0x0a;
	cmd.data_2 = horStep;
	cmd.data_3 = verStep;
	cmd.data_4 = stepColor;	
	send_cmd_to_mst6m48(&cmd);
}


void mst_setBrightness(u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0;
	cmd.data_2 = red;
	cmd.data_3 = green;
	cmd.data_4 = blue;	
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}
void mst_getBrightness(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 1;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setContrast(u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 2;
	cmd.data_2 = red;
	cmd.data_3 = green;
	cmd.data_4 = blue;	
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}

void mst_setColorTemp(u8 red, u8 green, u8 blue)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 04;
	cmd.data_2 = red;
	cmd.data_3 = green;
	cmd.data_4 = blue;	
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}
void mst_setSharpness(u8 sharpness)//20110906
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 06;
	cmd.data_2 = sharpness;
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}
void mst_setSaturation(u8 saturation)//20110909
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x08;
	cmd.data_2 = saturation;
	send_cmd_to_mst6m48(&cmd);
	//send_cmd_to_mst6m48_no_wait(&cmd);
}

void mst_setAdcAutoAdjust(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x0a;
	cmd.data_2 = 0;
	send_cmd_to_mst6m48(&cmd);
}

void mst_ResetAdcSetting(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x80;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 0;
	send_cmd_to_mst6m48(&cmd);
}

void mst_setPcX(u16 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 00;
	cmd.data_2 = 0;
	cmd.data_3 = value;
	cmd.data_4 = value>>8;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setPcY(u16 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 00;
	cmd.data_2 = 1;
	cmd.data_3 = value;
	cmd.data_4 = value>>8;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setPcClock(u16 clock)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 00;
	cmd.data_2 = 2;
	cmd.data_3 = clock;
	cmd.data_4 = clock>>8;	 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setPcPhase(u16 phase)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 00;
	cmd.data_2 = 3;
	cmd.data_3 = phase;
	cmd.data_4 = phase>>8;	 
	send_cmd_to_mst6m48(&cmd);
}

void mst_setAutoAdjustVGA(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 02;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}

void mst_setGamma(int value)
{
	Command_TypeDef cmd;
	cmd.command = 0x77;
	cmd.data_1 = 00;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}

BYTE mst_getGamma(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x77;
	cmd.data_1 = 01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}


void mst_getPcX(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48_and_wait(&cmd);
}
void mst_getPcY(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 01;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48_and_wait(&cmd);
}
void mst_getPcClock(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 01;
	cmd.data_2 = 2;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48_and_wait(&cmd);
}
void mst_getPcPhase(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x82;
	cmd.data_1 = 01;
	cmd.data_2 = 3;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48_and_wait(&cmd);
}

void mst_getSoftVersion(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x6a;
	cmd.data_1 = 01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48_and_wait(&cmd);
}
void mst_setSn(u16 sn)
{
	Command_TypeDef cmd;
	cmd.command = 0x6a;
	cmd.data_1 = 0x10;
	cmd.data_2 = sn>>8;
	cmd.data_3 = sn;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_getSn(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x6a;
	cmd.data_1 = 0x11;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_getDeviceState(void)
{
	int i = 0;
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x13;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	while((!send_cmd_to_mst6m48_and_wait(&cmd)) && (i++ < 3))
	{
		
	}
}

void mst_setAudioSource(u8 source)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 00;
	cmd.data_2 = source;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}
void mst_setAudioMode(u8 mode)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 02;
	cmd.data_2 = mode;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setHdmiAudio(u8 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x08;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
u8 mst_getHdmiAudio(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x09;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return GetUserSetting()->hdmiAudio;
}
void mst_setDPAudio(u8 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x10;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	//printf("mst_setDPAudio(%d)\r\n",value);
	send_cmd_to_mst6m48(&cmd);
}
void mst_setVolume(u8 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x04;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}

void mst_setMuteVolume(u8 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x06;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setttlInputAudio(u8 value)  //value: 0- i2s    1- audio1       2-audio2
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x0a;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	//printf("mst_setttlInputAudio(%d)\r\n",value);
	send_cmd_to_mst6m48(&cmd);
}
u8 mst_getttlInputAudio(void)  //value: 0- i2s    1- audio1       2-audio2
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x0b;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.TTLInputAudio;
}
void mst_setHdmiInputAudio(u8 value)  //value: 1- audio1       2-audio2
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x0c;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	//printf("mst_setHdmiInputAudio(%d)\r\n",value);
	send_cmd_to_mst6m48(&cmd);
}
u8 mst_getHdmiInputAudio(void)  //value: 1- audio1       2-audio2
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.HDMIInputAudio;
}
void mst_setAudioType(u8 value)
{
	Command_TypeDef cmd;
	cmd.command = 0x81;
	cmd.data_1 = 0x0e;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	//printf("mst_setAudioType(%d)\r\n",value);
	send_cmd_to_mst6m48(&cmd);
}

void mst_setWindowRatio(u8 value) // 0 -- 4:3, 1--16:9, 2 --Normal
{
	Command_TypeDef cmd;
	cmd.command = 0x6C;
	cmd.data_1 = 0x02;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}

void mst_setScreenMode(u8 value) // 0 -- full window, 1 -- screen window
{
	Command_TypeDef cmd;
	
		
	
	cmd.command = 0x6C;
	cmd.data_1 = 0x04;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}

int mst_getAlpha(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);

	return FadeAlpha;
	
}
void mst_setAlpha(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x00;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}
void mst_setSwitchMode(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x08;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
BYTE mst_getSwitchMode(void)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x09;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
void mst_setFadeTime(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x04;
	cmd.data_2 = value;
	cmd.data_3 = value >> 8;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
BYTE mst_getFadeTime(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x05;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setSwitchAB(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x0A;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
int mst_getSwitchAB(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x0B;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return SwitchTo;
}
int mst_getCurrentCh(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x76;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.currentCh;
}
void mst_setPipMode(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x00;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}


void mst_SwapMainPipInputSource(void)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x04;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setTopWindow(int value)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x06;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}

void mst_setTextEnableMode(int enable, int mode)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x08;
	cmd.data_2 = enable;
	cmd.data_3 = mode;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setTextAboveAndOr(int above, int AndOr)
{
	Command_TypeDef cmd;
		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x0a;
	cmd.data_2 = above;
	cmd.data_3 = AndOr;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setTextColor(int r, int g, int b)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x6B;
	cmd.data_1 = 0x0c;
	cmd.data_2 = r;
	cmd.data_3 = g;
	cmd.data_4 = b; 
	send_cmd_to_mst6m48(&cmd);
}

void mst_setSplit(int onOff, int syncMode)
{
	Command_TypeDef cmd;		
	syncMode = 1;
	cmd.command = 0x74;
	cmd.data_1 = 0x0a;
	cmd.data_2 = onOff;
	cmd.data_3 = syncMode;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setSplitHTotal(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 0;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setSplitVTotal(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 1;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setSplitHSize(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 2;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setSplitVSize(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 3;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setSplitHPos(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 4;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setSplitVPos(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 5;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_mst6m48(&cmd);
}

void mst_setVLockLine(int value)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0c;
	cmd.data_2 = 6;
	cmd.data_3 = value;
	cmd.data_4 = value>>8; 
	send_cmd_to_mst6m48(&cmd);
	
}

void mst_setCropSplitEnable(bool enable)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;  
	cmd.data_1 = 0x0c;
	cmd.data_2 = 7;
	cmd.data_3 = enable;
	cmd.data_4 = 0;
	send_cmd_to_mst6m48(&cmd);
}
char mst_getCropSplitEnable(bool enable)
{
	Command_TypeDef cmd;
	cmd.command = 0x74;  
	cmd.data_1 = 0x0d;
	cmd.data_2 = 7;
	cmd.data_3 = enable;
	cmd.data_4 = 0;
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.cropSplitEnable;//mst6m48_Setting.regValue;
}
void mst_setCropSplitInput(int index, int value) //  0 -- width,1-- height,2-- x, 3 -- y
{
	Command_TypeDef cmd;
	cmd.command = 0x74;  
	cmd.data_1 = 0x0c;
	cmd.data_2 = 8+index;
	cmd.data_3 = value&0xff;
	cmd.data_4 = value>>8;
	send_cmd_to_mst6m48(&cmd);
}
int  mst_getCropSplitInput(int index) //  0 -- width,1-- height,2-- x, 3 -- y, 
{
	Command_TypeDef cmd;
	cmd.command = 0x74;    
	cmd.data_1 = 0x0d;
	cmd.data_2 = 8+index;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	//printf("send mst6m48:%x,%x,%x,%x,%x,result: %d\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4,mst6m48_Setting.cropSplit[index]);
	return mst6m48_Setting.cropSplit[index];
}

int mst_getVLockLine(void)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 6;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.vLockLine;
}

void mst_setOutputOffset(int x, int y)
{
	Command_TypeDef cmd;		
	
	cmd.command = 0x74;
	cmd.data_1 = 0x10;
	cmd.data_2 = x;
	cmd.data_3 = y;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}


void mst_waitUpdateSoftware(void)
{
	int count = 0;
	while(TRUE)
	{
		if(mst6m48_Setting.updateState != 0)
		{
			return;
		}
		else if(mst6m48_Setting.deviceState == DEVICE_READY)
		{
			return;
		}
		else
		{
			task_mst6m48();
			count++;
			SysDelay1ms(100);
			if(count > 600)
			{
				return;
			}
		}
	}
}

bool mst_updateSoftware(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x68;
	cmd.data_1 = 0x20;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	mst6m48_Setting.updateState = 0;
	mst6m48_Setting.deviceState = DEVICE_INIT;
	send_cmd_to_mst6m48(&cmd);

	mst_waitUpdateSoftware();

	if(mst6m48_Setting.deviceState == DEVICE_READY)
	{
		mst6m48_Setting.updateState = 2;
		return TRUE;
	}
	else if(mst6m48_Setting.updateState == 3)
	{
		return FALSE;
	}
	else
	{
		mst6m48_Setting.updateState = 3;
		return FALSE;
	}
	
}
void mst_setCustomParameter(int type, WORD value) //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x12;
	cmd.data_2 = type;
	cmd.data_3 = value;
	cmd.data_4 = value >> 8; 
	send_cmd_to_mst6m48(&cmd);
}

int mst_getCustomParameter(int type) //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x13;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	switch(type)
	{
		case 0:
			return mst6m48_customVideo.htotal;
		case 1:
			return mst6m48_customVideo.vtotal;
		case 2:
			return mst6m48_customVideo.hstart;
		case 3:
			return mst6m48_customVideo.vstart;
		case 4:
			return mst6m48_customVideo.vfreq;
		case 5:
			return mst6m48_customVideo.hSyncWidth;
		case 6:
			return mst6m48_customVideo.vSyncWidth;
		case 7:
			return mst6m48_customVideo.hvSyncPolarity;
		case 8:
			return mst6m48_customResolution.DisplayWidth;
		case 9:
			return mst6m48_customResolution.DisplayHeight;
	}
	return 0;
}
#if 1
int mst_getSTRegister(int type) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x14;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
int mst_getSTVideoValue(int type) 
{
	Command_TypeDef cmd;
	
	cmd.command = 0x74;
	cmd.data_1 = 0x15;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
void mst_updateColorSpace(void)
{
	Command_TypeDef cmd;
	
	cmd.command = 0x72;
	cmd.data_1 = 0x1C;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}
void mst_setEnableInternalHDCP(int enable)
{
	Command_TypeDef cmd;

	if(enable == 0)
	{
		printf("disable HDCP\r\n");
	}
	cmd.command = 0x72;
	cmd.data_1 = 0x1d;
	cmd.data_2 = enable;
	cmd.data_3 = 0;
	cmd.data_4 = 0; 
	send_cmd_to_mst6m48(&cmd);
}

int mst_getHActiveValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
int mst_getVActiveValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 1;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
int mst_getHTotalValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 3;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
int mst_getVTotalValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 4;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

int mst_getFreqValue(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 07;
	cmd.data_2 = 2;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

void mst_setsdiIndex_TTL(int value)//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x20;
	cmd.data_2 = value;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	//printf("send mst6m48:%x,%x,%x,%x,%x\r\n",cmd.command,cmd.data_1,cmd.data_2,cmd.data_3,cmd.data_4);
	send_cmd_to_mst6m48(&cmd);
}
int mst_getsdiIndex_TTL(void)//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
{
	Command_TypeDef cmd;
	cmd.command = 0x72;
	cmd.data_1 = 0x21;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}

BYTE mst_getTTLInterlaveMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 9;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
void mst_setTTLInterlaveMode(BYTE type)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 8;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

BYTE mst_getHDMInterlaveMode(void)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x0d;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
void mst_setHDMIInterlaveMode(BYTE type)
{
	Command_TypeDef cmd;
	cmd.command = 0x73;
	cmd.data_1 = 0x0c;
	cmd.data_2 = type;
	cmd.data_3 = 0;
	cmd.data_4 = 0;	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setScaleH_Long(u16 position, u16 width)
{
	Command_TypeDef cmd;
	char buffer[4];
	
	cmd.command = 0xf0;
	cmd.data_1 = 0x00;
	cmd.data_2 = 0x00;
	cmd.data_3 = 0x04;
	cmd.data_4 = 0x00;
	buffer[0] = position & 0xff;
	buffer[1] = position >> 8;
	buffer[2] = width & 0xff;
	buffer[3] = width >> 8;	
	send_cmd_to_mst6m48_Long(&cmd,buffer,4);
}

void mst_setLayerParam_Long(BYTE layer)//layer 0-main   1-pip
{
	Command_TypeDef cmd;
	char buffer[19];
	
	if(layer > 1)
	{
		return;
	}
	isSetScale = 1;
	
	cmd.command = 0xf0;
	cmd.data_1 = 0x00;
	cmd.data_2 = layer;
	cmd.data_3 = 19;
	cmd.data_4 = 0x00;

	//crop参数发下去但是8028不处理
	buffer[0] = GetUserSetting()->layer[layer].cropDot.posX;
	buffer[1] = (GetUserSetting()->layer[layer].cropDot.posX>>8);	
	buffer[2] = GetUserSetting()->layer[layer].cropDot.posY;
	buffer[3] = (GetUserSetting()->layer[layer].cropDot.posY>>8);
	buffer[4] = GetUserSetting()->layer[layer].cropDot.sizeX;
	buffer[5] = (GetUserSetting()->layer[layer].cropDot.sizeX>>8);
	buffer[6] = GetUserSetting()->layer[layer].cropDot.sizeY;
	buffer[7] = (GetUserSetting()->layer[layer].cropDot.sizeY>>8);
	

	buffer[8] = GetUserSetting()->layer[layer].scale.posX;
	buffer[9] = (GetUserSetting()->layer[layer].scale.posX>>8);	
	buffer[10] = GetUserSetting()->layer[layer].scale.posY;
	buffer[11] = (GetUserSetting()->layer[layer].scale.posY>>8);
	buffer[12] = GetUserSetting()->layer[layer].scale.sizeX;
	buffer[13] = (GetUserSetting()->layer[layer].scale.sizeX>>8);
	buffer[14] = GetUserSetting()->layer[layer].scale.sizeY;
	buffer[15] = (GetUserSetting()->layer[layer].scale.sizeY>>8);

	//以下3个参数虽然下发但是8028不处理
	buffer[16] = GetUserSetting()->displayMode[layer];
	buffer[17] = 0;
	buffer[18] = GetUserSetting()->inputSource[layer];
	
	send_cmd_to_mst6m48_Long(&cmd,buffer,19);
	//send_cmd_to_mst6m48_Long_no_wait(&cmd,buffer,19);
}

void mst_getInputParams_Long(void)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf1;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);
}

void mst_setTestParam(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf2;
	cmd.data_1 = dat1;
	cmd.data_2 = dat2;
	cmd.data_3 = dat3;
	cmd.data_4 = dat4;
	
	send_cmd_to_mst6m48(&cmd);
}	

void mst_setDeviceRegValue(int reg,int value)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf4;
	cmd.data_1 = reg;
	cmd.data_2 = (reg>>8);
	cmd.data_3 = value;
	cmd.data_4 = (value>>8);
	
	send_cmd_to_mst6m48(&cmd);
}
int mst_getDeviceRegValue(int reg)
{
	Command_TypeDef cmd;
		
	cmd.command = 0xf5;
	cmd.data_1 = reg;
	cmd.data_2 = (reg>>8);
	cmd.data_3 = 0;
	cmd.data_4 = 0;
	
	send_cmd_to_mst6m48(&cmd);
	return mst6m48_Setting.regValue;
}
#endif

#endif			// #ifdef USE_STDP8028

