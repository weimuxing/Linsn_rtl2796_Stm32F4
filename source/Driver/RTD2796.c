

#include "RTD2796.h"
#include "rs232command.h"
#include "systick.h"
#include "uart.h"
#include "VideoFormatTable.h"
#include "userpref.h"
#include "menuData.h"
#include "fpga.h"
#include "keyScan.h"
#include "keyBoardStatus.h"
#include "menu.h"
#include "control.h"
#include "UartDebug.h"




#define RTD2796_UART_PORT	3


//--------------------------------------------------
// Definitions for LVDS Port Counts
//--------------------------------------------------
#define _LVDS_1_PORT                            0
#define _LVDS_2_PORT                            1
#define _LVDS_4_PORT                            2
#define _LVDS_8_PORT                            3


//--------------------------------------------------
// LVDS Data Port Power Selection
//--------------------------------------------------
#define _LVDS_1_PORT_A                                      0
#define _LVDS_1_PORT_B                                      1
#define _LVDS_1_PORT_C                                      2
#define _LVDS_1_PORT_D                                      3
#define _LVDS_2_PORT_AB                                     4
#define _LVDS_2_PORT_CD                                     5
#define _LVDS_2_PORT_EF                                     6
#define _LVDS_2_PORT_GH                                     7
#define _LVDS_4_PORT_ABCD                                   8
#define _LVDS_4_PORT_CDEF                                   9
#define _LVDS_4_PORT_EFGH                                   10
#define _LVDS_8_PORT_ABCDEFGH                               11

//--------------------------------------------------
// Definitions for LVDS Panel Type
//--------------------------------------------------
#define _LVDS_PANEL_1_SECTION                   0
#define _LVDS_PANEL_2_SECTION                   1
#define _LVDS_PANEL_4_SECTION                   2
#define _LVDS_PANEL_8_SECTION                   3

//--------------------------------------------------
// Definitions for Panel Bit Length
//--------------------------------------------------
#define _PANEL_DISP_18_BIT                      0
#define _PANEL_DISP_24_BIT                      1
#define _PANEL_DISP_30_BIT                      2
#define _PANEL_DISP_36_BIT                      3
#define _PANEL_DISP_48_BIT                      4

#define MAX_DISPLAY_WIDTH		3840
#define MAX_DISPLAY_HEIGHT		2160



u8 VIDEO_VERSION_HIGH = 0;  
u8 VIDEO_VERSION_LOW = 0;
u8 rtd2796Buf[100];
u8 PgmOldInput,PrvOldInput;
u8 PgmCurrentInput,PrvCurrentInput;
u8 PgmCurrentInputOld,PrvCurrentInputOld;
u8 PgmCurrentRegion,PrvCurrentRegion;
u8 CurrentDisplayMode = 0;
u8 isPtpModeEnable = 0;
u8 ReceiceStatus = 0;
u8 _videoReceive_sn = 0;
u8 receive_2796_buffer[512];


InputResolution_Struct InputResolution;
InputResolution_Struct InputResolutionOld;

PanelParamSt _panelParam;
FourPortSplitParamSt FourPortSplitPara;
SPLIT_PARAM_Typedef *panelInitSt;
PipWinSizeSt *gPipWinSizeSt;
OUTPUT_TYPE_PARAM_Typedef *outputTypeSt;
LAYER_PARAM_Typedef layerParam;
LAYER_PARAM_Typedef layerParamBackUp;
Pipe_t receive_preamp_pipe;
StructDisplayInfo g_stMDoUserOutputData;

Pipe_t receive_2796_pipe;


const char _2796_cmd_header[] = {'<','F'};

const char _2796_cmd_end[] = {'>'};

const u8 DisplayRegion[11] = {
	   							_DISPLAY_RGN_1P,_DISPLAY_RGN_MAIN,_DISPLAY_RGN_SUB,_DISPLAY_RGN_LEFT,_DISPLAY_RGN_RIGHT,
							  	_DISPLAY_RGN_TOP,_DISPLAY_RGN_BOTTOM,_DISPLAY_RGN_LT,_DISPLAY_RGN_LB,_DISPLAY_RGN_RT,_DISPLAY_RGN_RB
							 };

const u8 inputSourceIndex[4] = {1,3,2,5};

static Command_TypeDef cmd_from_2796;
static InterpretCmdState_TypeDef _videoState = interpret_cmd_init;
static vu32 TimeInputInfoScan = 0;
static vu8 InputErrorFlag = 0;
static u32 _videoStartPreampTime;

extern u8 takeStatus;
extern VideoDefine_Struct m_customVideo;  
extern Resolution_Struct m_customResolution;
extern u8 getCustomTimming(int h_pixels,int v_lines,int freq);
extern PanelParamSt _panelParam;
extern const VideoDefine_Struct tVideoDefine[MD_INDEX_SIZE];

bool gInputFlag = 0;



//###############################################################################################
//
void SetReceiveStatus(u8 x)
{
	ReceiceStatus = x;
}

//###############################################################################################
//
u8 GetReceiveStatus(void)
{
	return ReceiceStatus;
}

//###############################################################################################
//
void SetRtd2796Version(u8 vHeight,u8 vLow)
{
	VIDEO_VERSION_HIGH = vHeight;
	VIDEO_VERSION_LOW = vLow;
}

//###############################################################################################
//
u16 GetRtd2796Ver(void)
{
	u16 v;
	v = VIDEO_VERSION_HIGH;
	v = (v << 8) | VIDEO_VERSION_LOW;
}



//Rs232Command Rs232CmdUser;	
//###############################################################################################
//
char RTD2796Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(RTD2796_RESET_AHBxPeriph, ENABLE);
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = RTD2796_RESET_PIN;
	GPIO_Init(RTD2796_RESET_PORT, &GPIO_InitStruct);
	RTD2796_RESET_PIN_H();
}



//###############################################################################################
//
void RTD2796StopRun(void)
{
	RTD2796_RESET_PIN_L();
}



//###############################################################################################
//
void RTD2796Reset(void)
{
	RTD2796_RESET_PIN_L();
	SysDelay1ms(50);
	RTD2796_RESET_PIN_H();
}



//###############################################################################################
//
void SendRs232CommandToRTD2796(Command_TypeDef commandTo2796)
{  
	char buffer[50];
	int i;
	commandTo2796.address = 0;
	commandTo2796.sn = 0;
	commandTo2796.checksum = commandTo2796.data_1+commandTo2796.data_2+commandTo2796.data_3+commandTo2796.data_4+
		commandTo2796.address+commandTo2796.command+commandTo2796.sn;
	
	buffer[0] = '<';
	buffer[1] = 'T';
	for(i=0; i<sizeof(commandTo2796); i++)
	{
		ByteToHexChar(((BYTE *)&commandTo2796)[i],&buffer[2+i*2]);
	}
	buffer[2+sizeof(commandTo2796)*2] = '>';
	buffer[2+sizeof(commandTo2796)*2+1] = 0;

	UARTSendBuffer(RTD2796_UART_PORT,(char *)buffer,strlen(buffer));

	//#ifdef INPUT_INFO_DEBUG
	//SendBufferToPC((u8 *)buffer,strlen(buffer));
	DebugMessageRtd2796(
						"<T%02x%02x%02x%02x%02x%02x%02x%02x>\r\n",\
						commandTo2796.address,\
						commandTo2796.sn,\
						commandTo2796.command,\
						commandTo2796.data_1,\
						commandTo2796.data_2,\
						commandTo2796.data_3,\
						commandTo2796.data_4,\
						commandTo2796.checksum
						);
	//#endif
	
}




//###############################################################################################
//
void InitRtd2796Pipe(void)
{
	pipe_init(&receive_2796_pipe,receive_2796_buffer,sizeof(receive_2796_buffer));
}


//###############################################################################################
//
void Isr_receive_2796(u8 value)
{
	pipe_write(&receive_2796_pipe, value);
}


//###############################################################################################
//
int ReceiveBuffer_2796(char *buffer, int size, int timeout)
{
	u32 startTime;
	int index = 0;
	
	startTime = GetSysTick();
	
	while(!isSysTickTimeout(startTime,timeout))
	{
		BYTE value;
		if(pipe_read(&receive_2796_pipe, &value))
		{
			buffer[index++] = value;	
			if(index >= size)
			{
				break;
			}
		}
		if(isSysTickTimeout(startTime, timeout))
		{
			break;
		}
	}
	return index;
}


static u8 GetCheckVideo(u8 *buf,int size)
{
	u8 checkSum = 0;
	for(int i = 0;i < size;i++)
	{
		checkSum += buf[i];
	}
	return checkSum;
}


//###############################################################################################
//
static void process_2796_cmd(Command_TypeDef * cmd)
{
	//deviceIsConnet = 1;

	int index = 0;
	u16 Timing_h,Timing_v,Timing_f;

	DebugMessageRtd2796("process_2796_cmd:%x,%x,%d,%d,%d---->%d\r\n",cmd->command,cmd->data_1,cmd->data_2,cmd->data_3,cmd->data_4,GetSysTick());
	switch(cmd->command)
	{

		case 0x6a:
			switch(cmd->data_1)
			{
				case 0x07:
					VIDEO_VERSION_HIGH = cmd->data_2;
					VIDEO_VERSION_LOW = cmd->data_3;
					DebugMessageRtd2796("Rtd2796 version -> %d.%d:\r\n",VIDEO_VERSION_HIGH,VIDEO_VERSION_LOW);
				break;
			}
		break;
		case 0x85:
			switch(cmd->data_1)
			{
				case 0x11:
					DebugMessageRtd2796("No Signal !\r\n");
					SetSignalStatus(false);
					DisplayInputInfo();
				break;
				case 0x12:
					DebugMessageRtd2796("Add: port mapping %x\r\n",(cmd->data_3 << 8) | cmd->data_4);
					SetSignalStatus(true);
					GetSystemParam()->hotBsckup.hotBackupIndex = 0;
					index = ReceiveBuffer_2796(receive_2796_buffer,13,2000);
					if(index != 13)
					{
						DebugMessageRtd2796("Data len error! %d\r\n",index);
						return ;
					}
					if(GetCheckVideo(receive_2796_buffer,index-1) != receive_2796_buffer[index-1])
					{
						DebugMessageRtd2796("Video check error! %d %d\r\n",GetCheckVideo(receive_2796_buffer,index-1),receive_2796_buffer[index-1]);
						return ;
					}
					Timing_h = receive_2796_buffer[7];
					Timing_h = (Timing_h << 8) | receive_2796_buffer[6];
					Timing_v = receive_2796_buffer[9];
					Timing_v = (Timing_v << 8) | receive_2796_buffer[8];
					Timing_f = receive_2796_buffer[11];
					Timing_f = (Timing_f << 8) | receive_2796_buffer[10];
					
					if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
					{
						switch(cmd->data_4)
						{
							case 1:
								InputResolution.LT_inputStatus = 1;
								InputResolution.LT_inputWidth = Timing_h;
								InputResolution.LT_inputHeight = Timing_v;
								InputResolution.LT_inputFreq = Timing_f;
								InputResolution.LT_inputSource = cmd->data_3;
							break;
							case 2:
								InputResolution.LB_inputStatus = 1;
								InputResolution.LB_inputWidth = Timing_h;
								InputResolution.LB_inputHeight = Timing_v;
								InputResolution.LB_inputFreq = Timing_f;
								InputResolution.LB_inputSource = cmd->data_3;
							break;
							case 4:
								InputResolution.RT_inputStatus = 1;
								InputResolution.RT_inputWidth = Timing_h;
								InputResolution.RT_inputHeight = Timing_v;
								InputResolution.RT_inputFreq = Timing_f;
								InputResolution.RT_inputSource = cmd->data_3;
							break;
							case 8:
								InputResolution.RB_inputStatus = 1;
								InputResolution.RB_inputWidth = Timing_h;
								InputResolution.RB_inputHeight = Timing_v;
								InputResolution.RB_inputFreq = Timing_f;
								InputResolution.RB_inputSource = cmd->data_3;
							break;
							default:
							break;
						}
						DebugMessageRtd2796("Input Timing h = %d\r\n",Timing_h);
						DebugMessageRtd2796("Input Timing v = %d\r\n",Timing_v);
						DebugMessageRtd2796("Input Timing f = %d\r\n",Timing_f/10);
						DisplayInputInfo();
						return ;
					}
					switch(cmd->data_3)
					{
						#ifdef _VGA_INPUT_ENABLE
						case _VGA_INPUT:
							InputResolution.VGA_inputStatus = 1;
							InputResolution.VGA_inputWidth = Timing_h;
							InputResolution.VGA_inputHeight = Timing_v;
							InputResolution.VGA_inputFreq = Timing_f;
						break;
						#endif
						case _DP_INPUT:
							InputResolution.DP_inputStatus = 1;
							InputResolution.DP_inputWidth = Timing_h;
							InputResolution.DP_inputHeight = Timing_v;
							InputResolution.DP_inputFreq = Timing_f;
						break;
						case _HDMI20_INPUT:
							InputResolution.HDMI2_inputStatus = 1;
							InputResolution.HDMI2_inputWidth = Timing_h;
							InputResolution.HDMI2_inputHeight = Timing_v;
							InputResolution.HDMI2_inputFreq = Timing_f;
						break;
						case _HDMI14_INPUT:
							InputResolution.HDMI1_inputStatus = 1;
							InputResolution.HDMI1_inputWidth = Timing_h;
							InputResolution.HDMI1_inputHeight = Timing_v;
							InputResolution.HDMI1_inputFreq = Timing_f;
						break;
						case _DVI_INPUT:
							InputResolution.DVI_inputStatus = 1;
							InputResolution.DVI_inputWidth = Timing_h;
							InputResolution.DVI_inputHeight = Timing_v;
							InputResolution.DVI_inputFreq = Timing_f;
						break;
						default:
						break;
					}
					DebugMessageRtd2796("Input Timing h = %d\r\n",Timing_h);
					DebugMessageRtd2796("Input Timing v = %d\r\n",Timing_v);
					DebugMessageRtd2796("Input Timing f = %d\r\n",Timing_f/10);
					DisplayInputInfo();
				break;
				case 0x13:
					DebugMessageRtd2796("Remove: port mapping %x\r\n",(cmd->data_3 << 8) | cmd->data_4);
					if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
					{
						switch(cmd->data_4)
						{
							case 1:
								InputResolution.LT_inputStatus = 0;
								InputResolution.LT_inputWidth = 0;
								InputResolution.LT_inputHeight = 0;
								InputResolution.LT_inputFreq = 0;
								InputResolution.LT_inputSource = cmd->data_3;
							break;
							case 2:
								InputResolution.LB_inputStatus = 0;
								InputResolution.LB_inputWidth = 0;
								InputResolution.LB_inputHeight = 0;
								InputResolution.LB_inputFreq = 0;
								InputResolution.LB_inputSource = cmd->data_3;
							break;
							case 4:
								InputResolution.RT_inputStatus = 0;
								InputResolution.RT_inputWidth = 0;
								InputResolution.RT_inputHeight = 0;
								InputResolution.RT_inputFreq = 0;
								InputResolution.RT_inputSource = cmd->data_3;
							break;
							case 8:
								InputResolution.RB_inputStatus = 0;
								InputResolution.RB_inputWidth = 0;
								InputResolution.RB_inputHeight = 0;
								InputResolution.RB_inputFreq = 0;
								InputResolution.RB_inputSource = cmd->data_3;
							break;
							default:
							break;
						}
						return ;
					}
					if( GetPgmCurrentDisplayMode() == _OSD_DM_2P_LR ||\
						GetPgmCurrentDisplayMode() == _OSD_DM_2P_TB ||\
						GetPgmCurrentDisplayMode() == _OSD_DM_2P_PIP)
					{
						if(GetPgmCurrentInput() == cmd->data_3)
							break;
					}
					switch(cmd->data_3)
					{
						#ifdef _VGA_INPUT_ENABLE
						case _VGA_INPUT:
							InputResolution.VGA_inputStatus = 0;
							InputResolution.VGA_inputWidth = 0;
							InputResolution.VGA_inputHeight = 0;
							InputResolution.VGA_inputFreq = 0;
						break;
						#endif
						case _DP_INPUT:
							InputResolution.DP_inputStatus = 0;
							InputResolution.DP_inputWidth = 0;
							InputResolution.DP_inputHeight = 0;
							InputResolution.DP_inputFreq = 0;
						break;
						case _HDMI20_INPUT:
							InputResolution.HDMI2_inputStatus = 0;
							InputResolution.HDMI2_inputWidth = 0;
							InputResolution.HDMI2_inputHeight = 0;
							InputResolution.HDMI2_inputFreq = 0;
						break;
						case _HDMI14_INPUT:
							InputResolution.HDMI1_inputStatus = 0;
							InputResolution.HDMI1_inputWidth = 0;
							InputResolution.HDMI1_inputHeight = 0;
							InputResolution.HDMI1_inputFreq = 0;
						break;
						case _DVI_INPUT:
							InputResolution.DVI_inputStatus = 0;
							InputResolution.DVI_inputWidth = 0;
							InputResolution.DVI_inputHeight = 0;
							InputResolution.DVI_inputFreq = 0;
						break;
						default:
						break;
					}
				break;
			}
		break;
		case 0xf1:
			switch(cmd->data_1)
			{
				case 0x0a:
					
				break;
			}
		break;
		default :
			
		break;
	}
}



//###############################################################################################
//
static void interpret_2796_char(u8 value)
{
	
	static u8 index = 0;
	static char hexCharBuffer[2];
	
	if(value == '<')
	{
		_videoState = interpret_cmd_init;
	}
	switch(_videoState)
	{
		case interpret_cmd_init:
			if(value == '<')
			{
				index = 1;
				_videoState = interpret_cmd_start;
			}
		break;
		case interpret_cmd_start:		
			if(value == _2796_cmd_header[index++])
			{
				if(index >= sizeof(_2796_cmd_header))
				{
					_videoState = interpret_cmd_data;
					index = 0;
				}
			}
			else
			{
				_videoState = interpret_cmd_init;
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
					cmd_from_2796.address= HexCharToByte(hexCharBuffer);
					++index;
				break;
				case 3:
					hexCharBuffer[1] = value;
					cmd_from_2796.sn = HexCharToByte(hexCharBuffer);
					++index;
				break;	
				case 5:
					hexCharBuffer[1] = value;
					cmd_from_2796.command = HexCharToByte(hexCharBuffer);
					++index;
				break;			
				case 7:
					hexCharBuffer[1] = value;
					cmd_from_2796.data_1 = HexCharToByte(hexCharBuffer);
					++index;
				break;
				case 9:
					hexCharBuffer[1] = value;
					cmd_from_2796.data_2 = HexCharToByte(hexCharBuffer);
					++index;
				break;	
				case 11:
					hexCharBuffer[1] = value;
					cmd_from_2796.data_3 = HexCharToByte(hexCharBuffer);
					++index;
				break;
				case 13:
					hexCharBuffer[1] = value;
					cmd_from_2796.data_4 = HexCharToByte(hexCharBuffer);
					++index;
				break;
				case 15:
					// get checksum no use
					hexCharBuffer[1] = value;
					cmd_from_2796.checksum = HexCharToByte(hexCharBuffer);
					_videoState = interpret_cmd_end;
					index = 0;
				break;
				default:
					_videoState = interpret_cmd_init;
				break;
			}
		break;
		case interpret_cmd_end:
			if(value == _2796_cmd_end[index])
			{
				++index;
				if(index >= sizeof(_2796_cmd_end))
				{
					_videoState = interpret_cmd_init;
					_videoReceive_sn = cmd_from_2796.sn;
					//DebugPrintf("active=%d\r\n", receive_preamp_count);
					process_2796_cmd(&cmd_from_2796);
					index = 0;
				}
			}
			else
			{
				_videoState = interpret_cmd_init;
				//DebugPutString("preamp end header error\r\n");
				//DebugPrintf("end char = %x\r\n", value);
				//DebugPrintf("index = %d\r\n", index);
				//DebugPrintf("sizeof = %d\r\n", sizeof(preamp_cmd_end));
			}
		break;
		default:
			_videoState = interpret_cmd_init;
	}
	
}




//###############################################################################################
//
void task_2796(void)
{
	u8 value;
	while(pipe_read(&receive_2796_pipe,&value))
	{
		interpret_2796_char(value);
		if(_videoState == interpret_cmd_start)
		{
			_videoStartPreampTime = GetSysTick();
		}
	}
	if((_videoState != interpret_cmd_init) && isSysTickTimeout(_videoStartPreampTime,1000))
	{
		_videoState = interpret_cmd_init;
	}
}




//###############################################################################################
//
u8 GetPanelParamCheskSum(u8 *buf,int len)
{
	u8 checkSum = 0;
	int i;
	for(i = 0;i < len;i++)
	{
		checkSum += *(buf + i);
	}	
	return checkSum;
}



//###############################################################################################
//
InputResolution_Struct *GetInputResolution(void)
{
	return &InputResolution;
}



//###############################################################################################
//
u8 GetPgmCurrentDisplayMode(void)
{
	return CurrentDisplayMode;
}



//###############################################################################################
//
u8 GetPgmCurrentInput(void)
{
	return PgmCurrentInput;
}



//###############################################################################################
//
u8 GetPrvCurrentInput(void)
{
	return PrvCurrentInput;
}

//###############################################################################################
//
u8 GetPgmCurrentRegion(void)
{
	return PgmCurrentRegion;
}



//###############################################################################################
//
u8 GetPrvCurrentRegion(void)
{
	return PrvCurrentRegion;
}


//###############################################################################################
//
FunctionalState GetPtpStatus(void)
{
	return isPtpModeEnable;
}


//###############################################################################################
//
u8 SetPtpStatus(u8 x)
{
	isPtpModeEnable = x;
}



//###############################################################################################
//
void SetPgmCurrentDisplayMode(u8 x)
{
	CurrentDisplayMode = x;
}



//###############################################################################################
//
void SetPgmCurrentInput(u8 x)
{
	PgmCurrentInput = x;
}

//###############################################################################################
//
void SetPrvCurrentInput(u8 x)
{
	PrvCurrentInput = x;
}

//###############################################################################################
//
void SetPgmCurrentRegion(u8 x)
{
	PgmCurrentRegion = x;
}

//###############################################################################################
//
void SetPrvCurrentRegion(u8 x)
{
	PrvCurrentRegion = x;
}

//###############################################################################################
//
void PanelParamInit(void)
{
	PanelSplitParamSt *fpgaSplitParam;
	
	GetRtd2796Version();
	
	//FpgaOutputPortSel(DVI0_OUTPUT,DVI1_OUTPUT,DVI2_OUTPUT,DVI3_OUTPUT);
	
	//GetUserSettingParam()->userSplitParam.outputPort1Map = DVI0_OUTPUT;
	//GetUserSettingParam()->userSplitParam.outputPort2Map = DVI1_OUTPUT;
	//GetUserSettingParam()->userSplitParam.outputPort2Map = DVI2_OUTPUT;
	//GetUserSettingParam()->userSplitParam.outputPort4Map = DVI3_OUTPUT;
	//s
	panelInitSt = GetPanleAndSplitParam();
	LoadCurrentUserSettingParam();
	panelInitSt = GetPanleAndSplitParamRam();
	outputTypeSt = GetOutputTypeParamRam();
	//panelInitSt->outputFormatIndex = 10;

	FpgaOutputPortSel(	panelInitSt->outputPort1Map,
						panelInitSt->outputPort2Map,
						panelInitSt->outputPort3Map,
						panelInitSt->outputPort4Map
					);
	//gPipWinSizeSt = 
	OutputTypeInit(outputTypeSt);
	if(panelInitSt->outputFormatIndex == MD_CUSTOM)
	{
		memcpy((u8*)&tVideoDefineCustom,(u8*)&(panelInitSt->stCustomFormat),sizeof(VideoDefine_Struct));
	}
	else
	{
		memcpy((u8*)&tVideoDefineCustom,(u8*)&tVideoDefine[panelInitSt->outputFormatIndex],sizeof(VideoDefine_Struct));
	}

	
	SetOutputFormat(panelInitSt->outputFormatIndex);
	SetPgmCurrentDisplayMode(panelInitSt->splitMode);
	SetPgmCurrentInput(panelInitSt->inputSource1);
	SetPrvCurrentInput(panelInitSt->inputSource2);
	//panelInitSt->splitMode = _OSD_DM_1P;
	//panelInitSt->fieldSplit_UserTotalDisplayWidth = 3840;
	//panelInitSt->fieldSplit_UserTotalDisplayHeight;
	//panelInitSt->fieldSplit_FirstDisplayWidth = 1920;
	//panelInitSt->fieldSplit_FirstDisplayHeight = 1080;
	//
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
			FieldSplitSetting(panelInitSt);
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		case _OSD_DM_4P:
			IndependentOutputSetting(panelInitSt,_OSD_DM_4P);
			GetSystemParam()->Osd4pSwitch = 1;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		case _OSD_DM_2P_LR:
			TwoPortSplitSetting(panelInitSt,_OSD_DM_2P_LR);
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		case _OSD_DM_2P_TB:
			TwoPortSplitSetting(panelInitSt,_OSD_DM_2P_TB);
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		case _OSD_DM_2P_PIP:
			fpgaSplitParam = LoadFpgaSplitFromFlash(0);
			SetSplitParamToFpga(fpgaSplitParam);
			FPGA_set(0x06,GetSystemParam()->FpgaDisMode);
			SysDelay1ms(1000);
			SetPgmCurrentDisplayMode(_OSD_DM_2P_PIP);
			memcpy((u8*)&stPipWin,(u8*)GetPipParam(),sizeof(PipWinSizeSt));
			SetPipWindowStart(&stPipWin);
			UserPipParamSetting(&stPipWin);
			SetPipModeParam(GetSystemParam()->PipLayout,GetPgmCurrentInput(),GetPrvCurrentInput());
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 1;
		break;
		case _OSD_DM_2P_SACLE_TB:
			FourPortSplitSetting(panelInitSt,_OSD_DM_2P_SACLE_TB);
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		case _OSD_DM_3P_SACLE_TB:
			ThirdPortSplitSetting(panelInitSt,_OSD_DM_3P_SACLE_TB);
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
			Vertical2SplitSetting(panelInitSt,_OSD_DM_2P_TOW_PORT_LR);
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		case _OSD_DM_2P_TOW_PORT_TB:
			Horizontal2SplitSetting(panelInitSt,_OSD_DM_2P_TOW_PORT_TB);
			GetSystemParam()->Osd4pSwitch = 0;
			GetSystemParam()->PipModeSwitch = 0;
		break;
		default:
			FieldSplitSetting(panelInitSt);
		break;
	}
	SaveCurrentUserSettingParam();
	SysDelay1ms(500);
	LoadCurrentUserSettingParam();
}


//###############################################################################################
//
void VideoPanelSetting(u16 hWidth,u16 vHeight,u16 Frame)
{
	Command_TypeDef cmd;
	//u8 checkSum = 0;
	//int i;
	
	if(getCustomTimming(hWidth,vHeight,Frame))
	{
		DebugMessageRtd2796("_4KModeSetting()\r\n");
	    cmd.command = 0xF0;
	    cmd.data_1 = 0x018;
	    cmd.data_2 = 0x00;
	    cmd.data_3 = (u8)sizeof(PanelParamSt)-1;
    	cmd.data_4 = (u8)(sizeof(PanelParamSt) >> 8);
	    //send_cmd_to_video(&cmd);
		SendRs232CommandToRTD2796(cmd);	

		_panelParam._panelFrameActive = Frame*10;
		_panelParam._panelFrameMax = _panelParam._panelFrameActive + 5;
		_panelParam._panelFrameMin = _panelParam._panelFrameActive - 5;

		
		_panelParam._panelLvdsOutputPort = _LVDS_8_PORT;
		_panelParam._panelLvdsPortSel = _LVDS_8_PORT_ABCDEFGH;
		_panelParam._panelLvdsPanelSection = _LVDS_PANEL_2_SECTION;
		_panelParam._panelDispColorBits = _PANEL_DISP_30_BIT;
		
	    _panelParam._panelHsStart = m_customVideo.hstart;										//_PANEL_DH_START_INDEX,				//H 开始 根据屏参设定
	    _panelParam._panelHsWidth = m_customResolution.DisplayWidth;							//_PANEL_DH_WIDTH_INDEX,				//H 有效
	    _panelParam._panelHsEnd = m_customVideo.hstart + _panelParam._panelHsWidth;				//_PANEL_DH_END_INDEX,				//H 结束  开始+有效 ＝ 结束 ， 下同

	    _panelParam._panelHsTotal = m_customVideo.htotal;										//_PANEL_DH_TOTAL_INDEX,				//H Total
	    _panelParam._panelHsTotalMax = m_customVideo.htotal + 10;								//_PANEL_DH_TOTAL_MAX_INDEX,				//H Total 最大
	    _panelParam._panelHsTotalMin = m_customVideo.htotal - 10;								//_PANEL_DH_TOTAL_MIN_INDEX,				//H Total 最小  最大和最小比Total +- 10左右
	    _panelParam._panelLastLine = _panelParam._panelHsTotalMin;

	    _panelParam._panelVsStart = m_customVideo.vstart;										//_PANEL_DV_START_INDEX,				//V 开始 根据屏参设定				
	    _panelParam._panelVsHeight = m_customResolution.DisplayHeight;							//_PANEL_DV_HEIGHT_INDEX,				//V 有效
	    _panelParam._panelVsEnd = m_customVideo.vstart + _panelParam._panelVsHeight;			//_PANEL_DV_END_INDEX,				//V 结束

	    _panelParam._panelVsTotal = m_customVideo.vtotal;										//_PANEL_DV_TOTAL_INDEX,				//V Total
	    _panelParam._panelVsTotalMax = m_customVideo.vtotal + 10;								//_PANEL_DV_TOTAL_MAX_INDEX,				//V Total 最大
	    _panelParam._panelVsTotalMin = m_customVideo.vtotal - 10;								//_PANEL_DV_TOTAL_MIN_INDEX,				//V Total 最小

	    _panelParam._panelHsync = m_customVideo.hSyncWidth;										//_PANEL_DH_SYNC_WIDTH_INDEX,				//H 同步			
	    _panelParam._panelVsync = m_customVideo.vSyncWidth;										//_PANEL_DV_SYNC_HEIGHT_INDEX,			//V 同步

	    _panelParam._panelPclk = m_customVideo.pClk;											//_PANEL_PIXEL_CLOCK_INDEX,				//pClk 像素时钟
	    _panelParam._panelPclkMax = m_customVideo.pClk + 5;										//_PANEL_PIXEL_CLOCK_MAX_INDEX,			//pClk 最大
	    _panelParam._panelPclkMin = m_customVideo.pClk - 5;										//_PANEL_PIXEL_CLOCK_MIN_INDEX,			//pClk 最小

		_panelParam._panelCheckSum = GetPanelParamCheskSum((u8 *)&_panelParam,sizeof(_panelParam)-2);
		UARTSendBuffer(RTD2796_UART_PORT,(u8*)&_panelParam,sizeof(_panelParam)-1);
		
		DebugMessageRtd2796("\r\n_panelFrameActive = %d\r\n",_panelParam._panelFrameActive);
		DebugMessageRtd2796("_panelFrameMax = %d\r\n",_panelParam._panelFrameMax);
		DebugMessageRtd2796("_panelFrameMin = %d\r\n",_panelParam._panelFrameMin);
		DebugMessageRtd2796("_panelLvdsOutputPort = %d\r\n",_panelParam._panelLvdsOutputPort);
		DebugMessageRtd2796("_panelLvdsPortSel = %d\r\n",_panelParam._panelLvdsPortSel);
		DebugMessageRtd2796("_panelLvdsPanelSection = %d\r\n",_panelParam._panelLvdsPanelSection);
		DebugMessageRtd2796("_panelDispColorBits = %d\r\n",_panelParam._panelDispColorBits);
		DebugMessageRtd2796("_panelHsStart = %d\r\n",_panelParam._panelHsStart);
		DebugMessageRtd2796("_panelHsWidth = %d\r\n",_panelParam._panelHsWidth);
		DebugMessageRtd2796("_panelHsEnd = %d\r\n",_panelParam._panelHsEnd);
		DebugMessageRtd2796("_panelHsTotal = %d\r\n",_panelParam._panelHsTotal);
		DebugMessageRtd2796("_panelHsTotalMin = %d\r\n",_panelParam._panelHsTotalMin);
		DebugMessageRtd2796("_panelLastLine = %d\r\n",_panelParam._panelLastLine);
		DebugMessageRtd2796("_panelVsStart = %d\r\n",_panelParam._panelVsStart);
		DebugMessageRtd2796("_panelVsHeight = %d\r\n",_panelParam._panelVsHeight);
		DebugMessageRtd2796("_panelVsEnd = %d\r\n",_panelParam._panelVsEnd);
		DebugMessageRtd2796("_panelVsTotal = %d\r\n",_panelParam._panelVsTotal);
		DebugMessageRtd2796("_panelVsTotalMax = %d\r\n",_panelParam._panelVsTotalMax);
		DebugMessageRtd2796("_panelVsTotalMin = %d\r\n",_panelParam._panelVsTotalMin);
		DebugMessageRtd2796("_panelHsync = %d\r\n",_panelParam._panelHsync);
		DebugMessageRtd2796("_panelVsync = %d\r\n",_panelParam._panelVsync);
		DebugMessageRtd2796("_panelPclk = %d\r\n",_panelParam._panelPclk);
		DebugMessageRtd2796("_panelPclkMax = %d\r\n",_panelParam._panelPclkMax);
		DebugMessageRtd2796("_panelPclkMin = %d\r\n",_panelParam._panelPclkMin);
	}
	else
	{
		DebugMessageRtd2796("Error panel param...\r\n");
	}
}


//###############################################################################################
//
u8 OsdModeConver(u8 mode)
{
	switch(mode)
	{
		case _OSD_DM_2P_LR:
			return _OSD_DM_2P_LR;
			
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_TOW_PORT_LR:
			return _OSD_DM_2P_SACLE_LR;
			
		case _OSD_DM_2P_TB:
			return _OSD_DM_2P_TB;
			
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			return _OSD_DM_2P_SACLE_TB;
			
		case _OSD_DM_1P:
			return _OSD_DM_1P;
			
		case _OSD_DM_2P_PIP:
			return 13;//return _OSD_DM_2P_PIP;
			
		case _OSD_DM_4P:
			return _OSD_DM_4P;
			
		default:
			return _OSD_DM_1P;
	}
}


//###############################################################################################
//
void UserDisAreaSetting(u16 userTotalWidth,u16 userTotalHeigh,u16 userHStart,u16 userVStart,u8 OsdLayout)
{
	u8 buf[sizeof(StructDisplayInfo)];
	int i;
	Command_TypeDef cmd;

	printf("UserDisAreaSetting()\r\n");
    cmd.command = 0xF0;
    cmd.data_1 = WR_UserDisAreaSetting;
    cmd.data_2 = 0x00;
    cmd.data_3 = (u8)sizeof(g_stMDoUserOutputData);
	cmd.data_4 = (u8)(sizeof(PanelParamSt) >> 8);
	SendRs232CommandToRTD2796(cmd);	
		
	memset((u8*)&g_stMDoUserOutputData,0,sizeof(g_stMDoUserOutputData));

	if(OsdModeConver(OsdLayout) == _OSD_DM_2P_LR)
		g_stMDoUserOutputData.usHWidth = userTotalWidth/2;
	else 
		g_stMDoUserOutputData.usHWidth = userTotalWidth;
	if(OsdModeConver(OsdLayout) == _OSD_DM_2P_TB)
		g_stMDoUserOutputData.usVHeight = userTotalHeigh/2;
	else
		g_stMDoUserOutputData.usVHeight = userTotalHeigh;
	
	g_stMDoUserOutputData.usHStart = userHStart;
	g_stMDoUserOutputData.usVStart = userVStart;
	memcpy(buf,(u8*)&g_stMDoUserOutputData,sizeof(g_stMDoUserOutputData));

	buf[sizeof(g_stMDoUserOutputData)-1] = GetPanelParamCheskSum((u8 *)&g_stMDoUserOutputData,sizeof(g_stMDoUserOutputData));
	UARTSendBuffer(RTD2796_UART_PORT,buf,sizeof(g_stMDoUserOutputData));
	for(i = 0;i < sizeof(g_stMDoUserOutputData);i++)
	{
		DebugMessageRtd2796("buf[%d] = %d\r\n",i,buf[i]);
	}

	DebugMessageRtd2796("Data check sum = %d\r\n",buf[sizeof(g_stMDoUserOutputData)-2]);
	DebugMessageRtd2796("Data len = %d\r\n",sizeof(g_stMDoUserOutputData));
	DebugMessageRtd2796("User Dis Area H = %d\r\n",g_stMDoUserOutputData.usHWidth);
	DebugMessageRtd2796("User Dis Area V = %d\r\n",g_stMDoUserOutputData.usVHeight);
	DebugMessageRtd2796("User Dis Area H start = %d\r\n",g_stMDoUserOutputData.usHStart);
	DebugMessageRtd2796("User Dis Area V start = %d\r\n",g_stMDoUserOutputData.usVStart);
	
}


//###############################################################################################
//
void UserSplitParamSetting(FourPortSplitParamSt splitParam)
{
	u8 buf[sizeof(FourPortSplitParamSt)];
	int i;
	Command_TypeDef cmd;

	printf("UserDisAreaSetting()\r\n");
    cmd.command = 0xF0;
    cmd.data_1 = WR_UserDisAreaSetting;
    cmd.data_2 = 0x00;
    cmd.data_3 = (u8)sizeof(FourPortSplitParamSt);
	cmd.data_4 = (u8)(sizeof(FourPortSplitParamSt) >> 8);
	SendRs232CommandToRTD2796(cmd);	

	if(splitParam.stFirstDisplayWidth > 1920)
		splitParam.stFirstDisplayWidth = 1920;
	if(splitParam.stSecondDisplayWidth > 1920)
		splitParam.stSecondDisplayWidth = 1920;
	if(splitParam.stThirdDisplayWidth > 1920)
		splitParam.stThirdDisplayWidth = 1920;
	if(splitParam.stFourthDisplayWidth > 1920)
		splitParam.stFourthDisplayWidth = 1920;
	
	if(splitParam.stFirstDisplayHeight > 1080)
		splitParam.stFirstDisplayHeight = 1080;
	if(splitParam.stSecondDisplayHeight > 1080)
		splitParam.stSecondDisplayHeight = 1080;
	if(splitParam.stThirdDisplayHeight > 1080)
		splitParam.stThirdDisplayHeight = 1080;
	if(splitParam.stFourthDisplayHeight > 1080)
		splitParam.stFourthDisplayHeight = 1080;
	
	memcpy(buf,(u8*)&splitParam,sizeof(FourPortSplitParamSt));

	buf[sizeof(FourPortSplitParamSt)-1] = GetPanelParamCheskSum((u8 *)&splitParam,sizeof(FourPortSplitParamSt)-1);
	UARTSendBuffer(RTD2796_UART_PORT,buf,sizeof(FourPortSplitParamSt));

	DebugMessageRtd2796("User display mode = %d\r\n",		splitParam.splitMode);
	DebugMessageRtd2796("User total H = %d\r\n",			splitParam.stTotalWidth);
	DebugMessageRtd2796("User total V = %d\r\n",			splitParam.stTotalHeight);
	DebugMessageRtd2796("User first H  = %d\r\n",			splitParam.stFirstDisplayWidth);
	DebugMessageRtd2796("User first V  = %d\r\n",			splitParam.stFirstDisplayHeight);
	DebugMessageRtd2796("User second H = %d\r\n",			splitParam.stSecondDisplayWidth);
	DebugMessageRtd2796("User second V = %d\r\n",			splitParam.stSecondDisplayHeight);
	DebugMessageRtd2796("User third H  = %d\r\n",			splitParam.stThirdDisplayWidth);
	DebugMessageRtd2796("User third V  = %d\r\n",			splitParam.stThirdDisplayHeight);
	DebugMessageRtd2796("User fourth H  = %d\r\n",			splitParam.stFourthDisplayWidth);
	DebugMessageRtd2796("User fourth V  = %d\r\n",			splitParam.stFourthDisplayHeight);

	DebugMessageRtd2796("User first region H start = %d\r\n",	splitParam.stFirstRegionWidthStart);
	DebugMessageRtd2796("User first region V start = %d\r\n",	splitParam.stFirstRegionHeightStart);
	DebugMessageRtd2796("User second region H start = %d\r\n",	splitParam.stSecondRegionWidthStart);
	DebugMessageRtd2796("User second region V start = %d\r\n",	splitParam.stSecondRegionHeightStart);
	DebugMessageRtd2796("User third region H start = %d\r\n",	splitParam.stThirdRegionWidthStart);
	DebugMessageRtd2796("User third region V start = %d\r\n",	splitParam.stThirdRegionHeightStart);
	DebugMessageRtd2796("User fourth region H start = %d\r\n",	splitParam.stFourthRegionWidthStart);
	DebugMessageRtd2796("User fourth region V start = %d\r\n",	splitParam.stFourthRegionHeightStart);
	
}



//###############################################################################################
//
void UserPipParamSetting(pPipWinSizeSt stPipWinParam)
{
	u8 buf[sizeof(PipWinSizeSt)];
	int i;
	Command_TypeDef cmd;

	
	//memset((u8*)&stPipWin,0,sizeof(PipWinSizeSt));
	
	//memcpy((u8*)&stPipWin,(u8*)stPipWinParam,sizeof(PipWinSizeSt));
	
	//SetPipWindowStart(&stPipWin);
	//pstPipWin = &stPipWin;
		
	printf("W_PipWinParamSetting()\r\n");
    cmd.command = 0xF0;
    cmd.data_1 = W_PipWinParamSetting;
    cmd.data_2 = 0x00;
    cmd.data_3 = (u8)sizeof(PipWinSizeSt);
	cmd.data_4 = (u8)(sizeof(PipWinSizeSt) >> 8);
	SendRs232CommandToRTD2796(cmd);	

	memcpy((u8*)&stPipWinSizeBackUp,(u8*)GetPipParam(),sizeof(PipWinSizeSt));
	
	memcpy(buf,(u8*)stPipWinParam,sizeof(PipWinSizeSt));

	buf[sizeof(PipWinSizeSt)-1] = GetPanelParamCheskSum((u8 *)stPipWinParam,sizeof(PipWinSizeSt)-1);
	UARTSendBuffer(RTD2796_UART_PORT,buf,sizeof(PipWinSizeSt));

	DebugMessageRtd2796("gPipMainWinPosX = %d\r\n",		stPipWinParam->gPipMainWinPosX);
	DebugMessageRtd2796("gPipMainWinPosY = %d\r\n",		stPipWinParam->gPipMainWinPosY);
	DebugMessageRtd2796("gPipMainWinSizeX = %d\r\n",	stPipWinParam->gPipMainWinSizeX);
	DebugMessageRtd2796("gPipMainWinSizeY = %d\r\n",	stPipWinParam->gPipMainWinSizeY);
	DebugMessageRtd2796("gPipSubWinPosX = %d\r\n",		stPipWinParam->gPipSubWinPosX);
	DebugMessageRtd2796("gPipSubWinPosY = %d\r\n",		stPipWinParam->gPipSubWinPosY);
	DebugMessageRtd2796("gPipSubWinSizeX = %d\r\n",		stPipWinParam->gPipSubWinSizeX);
	DebugMessageRtd2796("gPipSubWinSizeY = %d\r\n",		stPipWinParam->gPipSubWinSizeY);
	DebugMessageRtd2796("gPbpLeftWinPosX = %d\r\n",		stPipWinParam->gPbpLeftWinPosX);
	DebugMessageRtd2796("gPbpLeftWinPosY = %d\r\n",		stPipWinParam->gPbpLeftWinPosY);
	DebugMessageRtd2796("gPbpLeftWinSizeX = %d\r\n",	stPipWinParam->gPbpLeftWinSizeX);
	DebugMessageRtd2796("gPbpLeftWinSizeY = %d\r\n",	stPipWinParam->gPbpLeftWinSizeY);
	DebugMessageRtd2796("gPbpRightWinPosX = %d\r\n",	stPipWinParam->gPbpRightWinPosX);
	DebugMessageRtd2796("gPbpRightWinPosY = %d\r\n",	stPipWinParam->gPbpRightWinPosY);
	DebugMessageRtd2796("gPbpRightWinSizeX = %d\r\n",	stPipWinParam->gPbpRightWinSizeX);
	DebugMessageRtd2796("gPbpRightWinSizeY = %d\r\n",	stPipWinParam->gPbpRightWinSizeY);
	DebugMessageRtd2796("gPbpTopWinPosX= %d\r\n",		stPipWinParam->gPbpTopWinPosX);
	DebugMessageRtd2796("gPbpTopWinPosY = %d\r\n",		stPipWinParam->gPbpTopWinPosY);
	DebugMessageRtd2796("gPbpTopWinSizeX = %d\r\n",		stPipWinParam->gPbpTopWinSizeX);
	DebugMessageRtd2796("gPbpTopWinSizeY = %d\r\n",		stPipWinParam->gPbpTopWinSizeY);
	DebugMessageRtd2796("gPbpBottomWinPosX = %d\r\n",	stPipWinParam->gPbpBottomWinPosX);
	DebugMessageRtd2796("gPbpBottomWinPosY = %d\r\n",	stPipWinParam->gPbpBottomWinPosY);
	DebugMessageRtd2796("gPbpBottomWinSizeX = %d\r\n",	stPipWinParam->gPbpBottomWinSizeX);
	DebugMessageRtd2796("gPbpBottomWinSizeY = %d\r\n",	stPipWinParam->gPbpBottomWinSizeY);
	
}



//###############################################################################################################
//
void IndependentOutputSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode)
{
	u8 offset;
	
	u8 alpha = fourPortSplitParam->alpha;
	u16 totalHeight1,totalHeight2;

	FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->_4P_FirstDisplayWidth;
	FourPortSplitPara.stFirstDisplayHeight = fourPortSplitParam->_4P_FirstDisplayHeight;
	FourPortSplitPara.stSecondDisplayWidth = fourPortSplitParam->_4P_SecongDisplayWidth;
	FourPortSplitPara.stSecondDisplayHeight = fourPortSplitParam->_4P_SecongDisplayHeight;
	FourPortSplitPara.stThirdDisplayWidth = fourPortSplitParam->_4P_ThirdDisplayWidth;
	FourPortSplitPara.stThirdDisplayHeight = fourPortSplitParam->_4P_ThirdDisplayHeight;
	FourPortSplitPara.stFourthDisplayWidth = fourPortSplitParam->_4P_FourthDisplayWidth;
	FourPortSplitPara.stFourthDisplayHeight = fourPortSplitParam->_4P_FourthDisplayHeight;


	if((FourPortSplitPara.stFirstDisplayWidth % 2) != 0)
		FourPortSplitPara.stFirstDisplayWidth += 1;
	if((FourPortSplitPara.stFirstDisplayHeight % 2) != 0)
		FourPortSplitPara.stFirstDisplayHeight += 1;
	if((FourPortSplitPara.stSecondDisplayWidth % 2) != 0)
		FourPortSplitPara.stSecondDisplayWidth += 1;
	if((FourPortSplitPara.stSecondDisplayHeight % 2) != 0)
		FourPortSplitPara.stSecondDisplayHeight += 1;
	if((FourPortSplitPara.stThirdDisplayWidth % 2) != 0)
		FourPortSplitPara.stThirdDisplayWidth += 1;
	if((FourPortSplitPara.stThirdDisplayHeight % 2) != 0)
		FourPortSplitPara.stThirdDisplayHeight += 1;
	if((FourPortSplitPara.stFourthDisplayWidth % 2) != 0)
		FourPortSplitPara.stFourthDisplayWidth += 1;
	if((FourPortSplitPara.stFourthDisplayHeight % 2) != 0)
		FourPortSplitPara.stFourthDisplayHeight += 1;

	if(FourPortSplitPara.stFirstDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stFirstDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stFirstDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stFirstDisplayWidth = tVideoDefineCustom.hActive;

	if(FourPortSplitPara.stSecondDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stSecondDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stSecondDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stSecondDisplayWidth = tVideoDefineCustom.hActive;

	if(FourPortSplitPara.stThirdDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stThirdDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stThirdDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stThirdDisplayWidth = tVideoDefineCustom.hActive;

	if(FourPortSplitPara.stFourthDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stFourthDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stFourthDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stFourthDisplayWidth = tVideoDefineCustom.hActive;

	//FourPortSplitPara.stTotalWidth = FourPortSplitPara.stFirstDisplayWidth + FourPortSplitPara.stSecondDisplayWidth;
	//totalHeight1 = FourPortSplitPara.stFirstDisplayHeight + FourPortSplitPara.stSecondDisplayHeight;
	//totalHeight2 = FourPortSplitPara.stThirdDisplayHeight + FourPortSplitPara.stFourthDisplayHeight;
	//FourPortSplitPara.stTotalHeight = (totalHeight1 > totalHeight2)?(totalHeight1):(totalHeight2);
	
	if(FourPortSplitPara.stFirstDisplayWidth > 1920)
		FourPortSplitPara.stFirstRegionWidthStart = 0;
	else
		FourPortSplitPara.stFirstRegionWidthStart = 1920 - FourPortSplitPara.stFirstDisplayWidth;
	
	if(FourPortSplitPara.stFirstDisplayHeight > 1080)
		FourPortSplitPara.stFirstRegionHeightStart = 0;
	else
		FourPortSplitPara.stFirstRegionHeightStart = 1080-FourPortSplitPara.stFirstDisplayHeight;

	//if(FourPortSplitPara.stSecondDisplayWidth > 1920)
	FourPortSplitPara.stSecondRegionWidthStart = 0;
	//else
	//	FourPortSplitPara.stSecondRegionWidthStart = 1920 - FourPortSplitPara.stSecondDisplayWidth;
	
	if(FourPortSplitPara.stSecondDisplayHeight > 1080)
		FourPortSplitPara.stSecondRegionHeightStart = 0;
	else
		FourPortSplitPara.stSecondRegionHeightStart = 1080-FourPortSplitPara.stSecondDisplayHeight;

	if(FourPortSplitPara.stThirdDisplayWidth > 1920)
		FourPortSplitPara.stThirdRegionWidthStart = 0;
	else
		FourPortSplitPara.stThirdRegionWidthStart = 1920 - FourPortSplitPara.stThirdDisplayWidth;
	
	//if(FourPortSplitPara.stThirdDisplayHeight > 1080)
		FourPortSplitPara.stThirdRegionHeightStart = 0;
	//else
	//	FourPortSplitPara.stThirdRegionHeightStart = 1080-FourPortSplitPara.stThirdDisplayHeight;

	//if(FourPortSplitPara.stFourthDisplayWidth > 1920)
		FourPortSplitPara.stFourthRegionWidthStart = 0;
	//else
	//	FourPortSplitPara.stFourthRegionWidthStart = 1920 - FourPortSplitPara.stFourthDisplayWidth;
	
	//if(FourPortSplitPara.stFourthDisplayHeight > 1080)
		FourPortSplitPara.stFourthRegionHeightStart = 0;
	//else
	//	FourPortSplitPara.stFourthRegionHeightStart = 1080-FourPortSplitPara.stFourthDisplayHeight;
	

	FourPortSplitPara.splitMode = mode;

	//###############################################################################################################
	//屏幕垂直方向四个参数(全局)
	_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
	_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stSecondRegionHeightStart;
	_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 1080 + FourPortSplitPara.stThirdDisplayHeight;
	if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy > 2160)
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
	_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 1080 + FourPortSplitPara.stFourthDisplayHeight;
	if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy > 2160)
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;


	if(FourPortSplitPara.stTotalWidth > 3840)
		FourPortSplitPara.stTotalWidth = 3840;
	if(FourPortSplitPara.stTotalHeight > 2160)
		FourPortSplitPara.stTotalHeight = 2160;
	UserSplitParamSetting(FourPortSplitPara);
	SetVideoDisplay(FourPortSplitPara.splitMode);
	SysDelay1ms(1000);

	layerParam.layerAWidth = FourPortSplitPara.stFirstDisplayWidth;
	layerParam.layerAHeight = FourPortSplitPara.stFirstDisplayHeight;
	layerParam.layerAXstart = 0;
	layerParam.layerAVstart = 0;
	layerParam.layerASplitStart = FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerACropStart = 0;

	layerParam.layerBWidth = FourPortSplitPara.stSecondDisplayWidth;
	layerParam.layerBHeight = FourPortSplitPara.stSecondDisplayHeight;
	layerParam.layerBXstart = 0;
	layerParam.layerBVstart = 0;
	layerParam.layerBSplitStart = 0;
	layerParam.layerBCropStart = 0;

	layerParam.layerCWidth = FourPortSplitPara.stThirdDisplayWidth;
	layerParam.layerCHeight = FourPortSplitPara.stThirdDisplayHeight;
	layerParam.layerCXstart = 0;
	layerParam.layerCVstart = 0;
	layerParam.layerCSplitStart = FourPortSplitPara.stSecondRegionWidthStart;
	layerParam.layerCCropStart = 0;

	layerParam.layerDWidth = FourPortSplitPara.stFourthDisplayWidth;
	layerParam.layerDHeight = FourPortSplitPara.stFourthDisplayHeight;
	layerParam.layerDXstart = 0;
	layerParam.layerDVstart = 0;
	layerParam.layerDSplitStart = 0;
	layerParam.layerDCropStart = 0;

	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = FourPortSplitPara.stFirstDisplayWidth;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = FourPortSplitPara.stFirstDisplayHeight;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = 0;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;
	

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = 1920;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropEndPos = 0;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = FourPortSplitPara.stSecondDisplayWidth;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = FourPortSplitPara.stSecondDisplayHeight;

	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stSecondRegionWidthStart;
	if(FourPortSplitPara.stSecondDisplayWidth > 1920)
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stSecondDisplayWidth;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropEndPos = 0;

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = FourPortSplitPara.stThirdDisplayWidth;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stThirdRegionWidthStart;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = 1920;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitCropEndPos = 0;

	//###############################################################################################################
	//输出口4参数

	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = FourPortSplitPara.stFourthDisplayWidth;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFourthRegionWidthStart;
	if(FourPortSplitPara.stFourthDisplayWidth > 1920)
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stFourthDisplayWidth;

	_panelSplitInfo.OutputPort3SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitCropEndPos = 0;
	
	SetSplitParamToFpga(&_panelSplitInfo);
	GetSystemParam()->FpgaDisMode = _BEYOND_WIDTH_SPLIT;
	FPGA_set(0x06,GetSystemParam()->FpgaDisMode);
	
}





//###############################################################################################################
//
void FieldSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam)
{
	u8 offset;
	float x;
	//FourPortSplitParamSt FourPortSplitPara;
	u16 firstH,secondH,thirdH,fourthH,minH,maxH;
	u16 firstV,secondV,thirdV,fourthV,minV,maxV;
	u16 firstSectionv,secondSectionv,thirdSectionv,fourthSectionv;
	u16 firstSectionH,secondSectionH,thirdSectionH,fourthSectionH;
	u8 alpha = fourPortSplitParam->alpha;
	GetSystemParam()->Osd4pSwitch = 0;
	GetSystemParam()->PipModeSwitch = 0;
	FourPortSplitPara.stTotalWidth = fourPortSplitParam->fieldSplit_UserTotalDisplayWidth;
	FourPortSplitPara.stTotalHeight = fourPortSplitParam->fieldSplit_UserTotalDisplayHeight;
	FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->fieldSplit_FirstDisplayWidth;
	FourPortSplitPara.stFirstDisplayHeight = fourPortSplitParam->fieldSplit_FirstDisplayHeight;

	if((FourPortSplitPara.stTotalWidth % 2) != 0)
		FourPortSplitPara.stTotalWidth += 1;
	if((FourPortSplitPara.stTotalHeight % 2) != 0)
		FourPortSplitPara.stTotalHeight += 1;

	if((FourPortSplitPara.stFirstDisplayWidth % 2) != 0)
		FourPortSplitPara.stFirstDisplayWidth += 1;
	if((FourPortSplitPara.stFirstDisplayHeight % 2) != 0)
		FourPortSplitPara.stFirstDisplayHeight += 1;

	if(FourPortSplitPara.stFirstDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stFirstDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stFirstDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stFirstDisplayWidth = tVideoDefineCustom.hActive;
	
	//FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;//FourPortSplitPara.stTotalWidth - FourPortSplitPara.stFirstDisplayWidth;
	FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stTotalWidth - FourPortSplitPara.stFirstDisplayWidth;
	FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
	
	FourPortSplitPara.stThirdDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
	FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stTotalHeight - FourPortSplitPara.stFirstDisplayHeight;
	//FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;//FourPortSplitPara.stTotalHeight-FourPortSplitPara.stFirstDisplayHeight;
	
	//FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;//FourPortSplitPara.stTotalWidth - FourPortSplitPara.stThirdDisplayWidth;
	FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stTotalWidth - FourPortSplitPara.stThirdDisplayWidth;
	FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stTotalHeight - FourPortSplitPara.stSecondDisplayHeight;

	if(FourPortSplitPara.stSecondDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stSecondDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stSecondDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stSecondDisplayWidth = tVideoDefineCustom.hActive;

	if(FourPortSplitPara.stThirdDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stThirdDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stThirdDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stThirdDisplayWidth = tVideoDefineCustom.hActive;

	if(FourPortSplitPara.stFourthDisplayHeight > tVideoDefineCustom.vActive)		
		FourPortSplitPara.stFourthDisplayHeight = tVideoDefineCustom.vActive;

	if(FourPortSplitPara.stFourthDisplayWidth > tVideoDefineCustom.hActive)	
		FourPortSplitPara.stFourthDisplayWidth = tVideoDefineCustom.hActive;
	
	FourPortSplitPara.stSecondRegionWidthStart = 0;
	FourPortSplitPara.stSecondRegionHeightStart = 0;
	FourPortSplitPara.stThirdRegionWidthStart = 0;
	FourPortSplitPara.stThirdRegionHeightStart = 0;
	FourPortSplitPara.stFourthRegionWidthStart = 0;
	FourPortSplitPara.stFourthRegionHeightStart = 0;

	firstH = FourPortSplitPara.stFirstDisplayWidth;
	secondH = FourPortSplitPara.stSecondDisplayWidth;
	thirdH = FourPortSplitPara.stThirdDisplayWidth;
	fourthH = FourPortSplitPara.stFourthDisplayWidth;

	firstV = FourPortSplitPara.stFirstDisplayHeight;
	secondV = FourPortSplitPara.stSecondDisplayHeight;
	thirdV = FourPortSplitPara.stThirdDisplayHeight;
	fourthV = FourPortSplitPara.stFourthDisplayHeight;

	#if 1
	firstSectionH = firstH;
	secondSectionH = secondH;
	thirdSectionH = thirdH;
	fourthSectionH = fourthH;

	firstSectionv = firstH;
	secondSectionv = secondH;
	thirdSectionv = thirdH;
	fourthSectionH = fourthH;
	
	firstSectionv = firstV;
	secondSectionv = secondV;
	thirdSectionv = thirdV;
	fourthSectionv = fourthV;
	#endif
	
	maxH = (firstH >= secondH)?(firstH):(secondH);
	minH = (firstH <= secondH)?(firstH):(secondH);
	DebugMessageRtd2796("maxH = %d\r\n",maxH);
	DebugMessageRtd2796("minH = %d\r\n",minH);
	if(maxH > 1920)
	{
		x = (float)maxH/(float)minH;
		DebugMessageRtd2796("x = %f\r\n",x);
		if(firstH > secondH)
		{
			firstH = 1920;
			secondH = 1920 / x;
			if((secondH % 2) != 0)
				secondH += 1;
		}
		else
		{
			secondH = 1920;
			firstH = 1920 / x;
			if((firstH % 2) != 0)
				firstH += 1;
		}
	}

	DebugMessageRtd2796("firstH = %d\r\n",firstH);
	DebugMessageRtd2796("secondH = %d\r\n",secondH);

	maxV = (firstV >= thirdV)?(firstV):(thirdV);
	minV = (firstV <= thirdV)?(firstV):(thirdV);
	DebugMessageRtd2796("maxV = %d\r\n",maxH);
	DebugMessageRtd2796("minV = %d\r\n",minH);
	if(maxV > 1080)
	{
		x = (float)maxV/(float)minV;
		DebugMessageRtd2796("x = %f\r\n",x);
		if(firstH > thirdV)
		{
			firstV = 1080;
			thirdV = 1080 / x;
			if((thirdV % 2) != 0)
				thirdV += 1;
		}
		else
		{
			thirdV = 1080;
			firstV = 1080 / x;
			if((firstV % 2) != 0)
				firstV += 1;
		}
	}

	DebugMessageRtd2796("firstV = %d\r\n",firstH);
	DebugMessageRtd2796("secondV = %d\r\n",secondH);

	FourPortSplitPara.stTotalWidth = firstH + secondH;
	FourPortSplitPara.stTotalHeight = firstV + thirdV;

	if(firstH > 1920)
		FourPortSplitPara.stFirstRegionWidthStart = 0;
	else
		FourPortSplitPara.stFirstRegionWidthStart = 1920 - firstH;
	
	if(firstV > 1080)
		FourPortSplitPara.stFirstRegionHeightStart = 0;
	else
		FourPortSplitPara.stFirstRegionHeightStart = 1080 - firstV;

	FourPortSplitPara.splitMode = _OSD_DM_1P;

	//###############################################################################################################
	//屏幕垂直方向四个参数(全局)
	_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
	_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
	_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 1080 + thirdV;
	if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy > 2160)
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
	_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 1080 + thirdV;
	if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy > 2160)
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;


	if(FourPortSplitPara.stTotalWidth > 3840)
		FourPortSplitPara.stTotalWidth = 3840;
	if(FourPortSplitPara.stTotalHeight > 2160)
		FourPortSplitPara.stTotalHeight = 2160;
	UserSplitParamSetting(FourPortSplitPara);
	SetVideoDisplay(FourPortSplitPara.splitMode);
	SysDelay1ms(1000);

	layerParam.layerAWidth = FourPortSplitPara.stFirstDisplayWidth;
	layerParam.layerAHeight = FourPortSplitPara.stFirstDisplayHeight;
	layerParam.layerAXstart = 0;
	layerParam.layerAVstart = 0;
	layerParam.layerASplitStart = FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerACropStart = 0;

	layerParam.layerBWidth = FourPortSplitPara.stSecondDisplayWidth;
	layerParam.layerBHeight = FourPortSplitPara.stSecondDisplayHeight;
	layerParam.layerBXstart = 0;
	layerParam.layerBVstart = 0;
	layerParam.layerBSplitStart = 0;
	layerParam.layerBCropStart = 0;

	layerParam.layerCWidth = FourPortSplitPara.stThirdDisplayWidth;
	layerParam.layerCHeight = FourPortSplitPara.stThirdDisplayHeight;
	layerParam.layerCXstart = 0;
	layerParam.layerCVstart = 0;
	layerParam.layerCSplitStart = FourPortSplitPara.stSecondRegionWidthStart;
	layerParam.layerCCropStart = 0;

	layerParam.layerDWidth = FourPortSplitPara.stFourthDisplayWidth;
	layerParam.layerDHeight = FourPortSplitPara.stFourthDisplayHeight;
	layerParam.layerDXstart = 0;
	layerParam.layerDVstart = 0;
	layerParam.layerDSplitStart = 0;
	layerParam.layerDCropStart = 0;

	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = FourPortSplitPara.stFirstDisplayWidth;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = FourPortSplitPara.stFirstDisplayHeight;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = 0;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;
	

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = MAX_DISPLAY_WIDTH/2;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = FourPortSplitPara.stSecondDisplayWidth;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = FourPortSplitPara.stSecondDisplayHeight;

	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = 0;
	if(FourPortSplitPara.stSecondDisplayWidth > 1920)
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stSecondDisplayWidth;

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = FourPortSplitPara.stThirdDisplayWidth;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = MAX_DISPLAY_WIDTH/2;
	

	//###############################################################################################################
	//输出口4参数

	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = FourPortSplitPara.stFourthDisplayWidth;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = 0;
	if(FourPortSplitPara.stFourthDisplayWidth > 1920)
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stFourthDisplayWidth;
	SetSplitParamToFpga(&_panelSplitInfo);
	if( FourPortSplitPara.stFirstDisplayWidth > 1920 ||
		FourPortSplitPara.stFirstDisplayHeight > 1080 ||
		FourPortSplitPara.stFourthDisplayWidth > 1920 ||
		FourPortSplitPara.stFourthDisplayHeight > 1080
	  )
	{
		GetSystemParam()->FpgaDisMode = _BEYOND_WIDTH_SPLIT;
	}
	else
	{
		GetSystemParam()->FpgaDisMode = _1P_POINT_TO_POINT;
	}
	FPGA_set(0x06,GetSystemParam()->FpgaDisMode);
}



void SetPipWindowStart(pPipWinSizeSt stPipLayoutSize)
{
	stPipLayoutSize = stPipLayoutSize;
	#if 1
	stPipLayoutSize->gPipMainWinPosX += FourPortSplitPara.stFirstRegionWidthStart;
	stPipLayoutSize->gPipMainWinPosY += FourPortSplitPara.stFirstRegionHeightStart;
	stPipLayoutSize->gPipSubWinPosX += FourPortSplitPara.stFirstRegionWidthStart;
	stPipLayoutSize->gPipSubWinPosY += FourPortSplitPara.stFirstRegionHeightStart;
	stPipLayoutSize->gPbpTopWinPosX += FourPortSplitPara.stFirstRegionWidthStart;
	stPipLayoutSize->gPbpTopWinPosY += FourPortSplitPara.stFirstRegionHeightStart;
	stPipLayoutSize->gPbpBottomWinPosX += FourPortSplitPara.stFirstRegionWidthStart;
	stPipLayoutSize->gPbpBottomWinPosY += FourPortSplitPara.stFirstRegionHeightStart;
	//stPipLayoutSize->gPbpLeftWinPosX += FourPortSplitPara.stFirstRegionWidthStart;
	//stPipLayoutSize->gPbpLeftWinPosY += FourPortSplitPara.stFirstRegionHeightStart;
	//stPipLayoutSize->gPbpRightWinPosX += FourPortSplitPara.stFirstRegionWidthStart;
	//stPipLayoutSize->gPbpRightWinPosY += FourPortSplitPara.stFirstRegionHeightStart;
	#endif
}



void CalcPipLayoutSize(BYTE layout,pPipWinSizeSt stPipLayoutSize)
{
	int x, y, w, h;
	x = FourPortSplitPara.stFirstRegionWidthStart;
	y = FourPortSplitPara.stFirstRegionHeightStart;
	w = FourPortSplitPara.stTotalWidth;
	h = FourPortSplitPara.stTotalHeight;
	switch(layout)
	{
		case LAYOUT_PIP_LT:
			stPipLayoutSize->gPipMainWinPosX = x;
			stPipLayoutSize->gPipMainWinPosY = y;
			stPipLayoutSize->gPipMainWinSizeX = w;
			stPipLayoutSize->gPipMainWinSizeY = h;
			
			stPipLayoutSize->gPipSubWinPosX = x;			
			stPipLayoutSize->gPipSubWinPosY = y;			
			stPipLayoutSize->gPipSubWinSizeX = w*2/5;			
			stPipLayoutSize->gPipSubWinSizeY = h*2/5;
		break;
		case LAYOUT_PIP_RT:
			stPipLayoutSize->gPipMainWinPosX = x;
			stPipLayoutSize->gPipMainWinPosY = y;
			stPipLayoutSize->gPipMainWinSizeX = w;
			stPipLayoutSize->gPipMainWinSizeY = h;
			
			stPipLayoutSize->gPipSubWinPosX = x + w*3/5;			
			stPipLayoutSize->gPipSubWinPosY = y;			
			stPipLayoutSize->gPipSubWinSizeX = w*2/5;			
			stPipLayoutSize->gPipSubWinSizeY = h*2/5;
		break;
		case LAYOUT_PIP_LB:
			stPipLayoutSize->gPipMainWinPosX = x;
			stPipLayoutSize->gPipMainWinPosY = y;
			stPipLayoutSize->gPipMainWinSizeX = w;
			stPipLayoutSize->gPipMainWinSizeY = h;
			
			stPipLayoutSize->gPipSubWinPosX = x;			
			stPipLayoutSize->gPipSubWinPosY = y + h*3/5;			
			stPipLayoutSize->gPipSubWinSizeX = w*2/5;			
			stPipLayoutSize->gPipSubWinSizeY = h*2/5;
		break;
		case LAYOUT_PIP_RB:
			stPipLayoutSize->gPipMainWinPosX = x;
			stPipLayoutSize->gPipMainWinPosY = y;
			stPipLayoutSize->gPipMainWinSizeX = w;
			stPipLayoutSize->gPipMainWinSizeY = h;
			
			stPipLayoutSize->gPipSubWinPosX = x + w*3/5;			
			stPipLayoutSize->gPipSubWinPosY = y + h*3/5;			
			stPipLayoutSize->gPipSubWinSizeX = w*2/5;			
			stPipLayoutSize->gPipSubWinSizeY = h*2/5;
		break;
		case LAYOUT_PIP_CENT:
			stPipLayoutSize->gPipMainWinPosX = x;
			stPipLayoutSize->gPipMainWinPosY = y;
			stPipLayoutSize->gPipMainWinSizeX = w;
			stPipLayoutSize->gPipMainWinSizeY = h;
			
			stPipLayoutSize->gPipSubWinPosX = x + w*3/10;			
			stPipLayoutSize->gPipSubWinPosY = y + h*3/10;			
			stPipLayoutSize->gPipSubWinSizeX = w*2/5;			
			stPipLayoutSize->gPipSubWinSizeY = h*2/5;
		break;
		case LAYOUT_PBP_LR:
			stPipLayoutSize->gPbpLeftWinPosX = x;
			stPipLayoutSize->gPbpLeftWinPosY = y;
			stPipLayoutSize->gPbpLeftWinSizeX = w/2;
			stPipLayoutSize->gPbpLeftWinSizeY = h;
			
			stPipLayoutSize->gPbpRightWinPosX = x + (w / 2);			
			stPipLayoutSize->gPbpRightWinPosY = y;			
			stPipLayoutSize->gPbpRightWinSizeX = w/2;			
			stPipLayoutSize->gPbpRightWinSizeY = h;
		break;
		case LAYOUT_PBP_TB:
			stPipLayoutSize->gPbpTopWinPosX = x;
			stPipLayoutSize->gPbpTopWinPosY = y;
			stPipLayoutSize->gPbpTopWinSizeX = w;
			stPipLayoutSize->gPbpTopWinSizeY = h/2;
			
			stPipLayoutSize->gPbpBottomWinPosX = x;			
			stPipLayoutSize->gPbpBottomWinPosY = y + (h / 2);			
			stPipLayoutSize->gPbpBottomWinSizeX = w;			
			stPipLayoutSize->gPbpBottomWinSizeY = h/2;
		break;
	}
}

//#define SCALE_POS_X()			((GetSystemParam()->CurrentWindow == 0) ? ())
u16 GetScalerPosSize(u8 index)
{
	u16 resoult;
	switch(index)
	{
		case scalePosxValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPipMainWinPosX;
					else
						resoult = GetPipParam()->gPipSubWinPosX;
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpLeftWinPosX;
					else
						resoult = GetPipParam()->gPbpRightWinPosX;
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpTopWinPosX;
					else
						resoult = GetPipParam()->gPbpBottomWinPosX;
				break;	
			}
		break;
		case scalePosyValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPipMainWinPosY;
					else
						resoult = GetPipParam()->gPipSubWinPosY;
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpLeftWinPosY;
					else
						resoult = GetPipParam()->gPbpRightWinPosY;
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpTopWinPosY;
					else
						resoult = GetPipParam()->gPbpBottomWinPosY;
				break;
					
			}
		break;
		case scaleWidthValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPipMainWinSizeX;
					else
						resoult = GetPipParam()->gPipSubWinSizeX;
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpLeftWinSizeX;
					else
						resoult = GetPipParam()->gPbpRightWinSizeX;
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpTopWinSizeX;
					else
						resoult = GetPipParam()->gPbpBottomWinSizeX;
				break;
					
			}
		break;
		case scaleHeightValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPipMainWinSizeY;
					else
						resoult = GetPipParam()->gPipSubWinSizeY;
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpLeftWinSizeY;
					else
						resoult = GetPipParam()->gPbpRightWinSizeY;
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
						resoult = GetPipParam()->gPbpTopWinSizeY;
					else
						resoult = GetPipParam()->gPbpBottomWinSizeY;
				break;
					
			}
		break;
	}
	return resoult;
}

u16 SetScalerPosSize(u8 index,u16 value)
{
	u16 resoult = value;
	switch(index)
	{
		case scalePosxValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						GetPipParam()->gPipMainWinPosX = (value >= 3840)?(3840):(value);;
					}
					else
					{
						if(value >= (GetPipParam()->gPipMainWinSizeX - GetPipParam()->gPipSubWinSizeX))
							GetPipParam()->gPipSubWinPosX = 0;
						else
							GetPipParam()->gPipSubWinPosX = value;
					}
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						
						if(value >= GET_PBP_LEFT_SIZE_X())
							GetPipParam()->gPbpLeftWinPosX = GET_PBP_LEFT_SIZE_X();
						else
							GetPipParam()->gPbpLeftWinPosX = value;
					}
					else
					{
						
						if(value >= ((1920 - GET_PBP_RIGHT_SIZE_X()) + 1920))
							GetPipParam()->gPbpRightWinPosX = GET_PBP_LEFT_SIZE_X();
						else
							GetPipParam()->gPbpRightWinPosX = value;
					}
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						
						if(value >= GET_PBP_TOP_SIZE_X())
							GetPipParam()->gPbpTopWinPosX = 0;
						else
							GetPipParam()->gPbpTopWinPosX = value;
					}
					else
					{
						if(value >= GET_PBP_BOTTOM_SIZE_X())
							GetPipParam()->gPbpBottomWinPosX = 0;
						else
							GetPipParam()->gPbpBottomWinPosX = value;
					}
				break;
					
			}
		break;
		case scalePosyValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						GetPipParam()->gPipMainWinPosY = (value >= 2160)?(2160):(value);;
					}
					else
					{
						if(value >= (GetPipParam()->gPipMainWinSizeY - GetPipParam()->gPipSubWinSizeY))
							GetPipParam()->gPipSubWinPosY = 0;
						else
							GetPipParam()->gPipSubWinPosY = value;
					}
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= (2160 - GET_PBP_LEFT_SIZE_Y()))
							GetPipParam()->gPbpLeftWinPosY = (2160 - GET_PBP_LEFT_SIZE_Y());
						else
							GetPipParam()->gPbpLeftWinPosY = value;
					}
					else
					{
						if(value >= (2160 - GET_PBP_RIGHT_SIZE_Y()))
							GetPipParam()->gPbpRightWinPosY = (2160 - GET_PBP_RIGHT_SIZE_Y());
						else
							GetPipParam()->gPbpRightWinPosY = value;
					}
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						
						if(value >= GET_PBP_TOP_SIZE_Y())
							GetPipParam()->gPbpTopWinPosY = GET_PBP_TOP_SIZE_Y();
						else
							GetPipParam()->gPbpTopWinPosY = value;
					}
					else
					{
						if(value >= GET_PBP_TOP_SIZE_Y())
							GetPipParam()->gPbpBottomWinPosY = GET_PBP_BOTTOM_SIZE_Y();
						else
							GetPipParam()->gPbpBottomWinPosY = value;
					}
				break;
					
			}
		break;
		case scaleWidthValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						GetPipParam()->gPipMainWinSizeX = (value >= 3840)?(3840):(value);;
					}
					else
					{
						if(value >= 2048)
							GetPipParam()->gPipSubWinSizeX = 2048;
						else
							GetPipParam()->gPipSubWinSizeX = value;
					}
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= 2048)
							GetPipParam()->gPbpLeftWinSizeX = 2048;
						else
							GetPipParam()->gPbpLeftWinSizeX = value;
					}
					else
					{
						if(value >= 2048)
							GetPipParam()->gPbpRightWinSizeX = 2048;
						else
							GetPipParam()->gPbpRightWinSizeX = value;
					}
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= 3840)
							GetPipParam()->gPbpTopWinSizeX = 3840;
						else
							GetPipParam()->gPbpTopWinSizeX = value;
					}
					else
					{
						if(value >= 3840)
							GetPipParam()->gPbpBottomWinSizeX = 3840;
						else
							GetPipParam()->gPbpBottomWinSizeX = value;
					}
				break;
					
			}
		break;
		case scaleHeightValue:
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
				case LAYOUT_PIP_RT:
				case LAYOUT_PIP_LB:
				case LAYOUT_PIP_RB:
				case LAYOUT_PIP_CENT:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						GetPipParam()->gPipMainWinSizeY = (value >= 2160)?(2160):(value);
					}
					else
					{	
						GetPipParam()->gPipSubWinSizeY = (value >= (GetPipParam()->gPipMainWinSizeY - GetPipParam()->gPipSubWinPosY))?((GetPipParam()->gPipMainWinSizeY - GetPipParam()->gPipSubWinPosY)):(value);
					}
				break;
				case LAYOUT_PBP_LR:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= 2160)
							GetPipParam()->gPbpLeftWinSizeY = 2160;
						else
							GetPipParam()->gPbpLeftWinSizeY = value;
					}
					else
					{
						if(value >= 2160)
							GetPipParam()->gPbpRightWinSizeY = 2160;
						else
							GetPipParam()->gPbpRightWinSizeY = value;
					}
				break;
				case LAYOUT_PBP_TB:
					if(GetSystemParam()->CurrentWindow == IMAGE_A)
					{
						if(value >= 1080)
							GetPipParam()->gPbpTopWinSizeY = 1080;
						else
							GetPipParam()->gPbpTopWinSizeY = value;
					}
					else
					{
						if(value >= 1080)
							GetPipParam()->gPbpBottomWinSizeY = 1080;
						else
							GetPipParam()->gPbpBottomWinSizeY = value;
					}
				break;
					
			}
		break;
	}
	return resoult;
}


void SetPipLayout(BYTE layout)
{
	menuSetBusy(1,3);
	FPGASetFreeze(1);
	GetSystemParam()->PipLayout = layout;
	CalcPipLayoutSize(layout,GetPipParam());
	UserPipParamSetting(GetPipParam());
	SaveFpgaSplitParamToFlash(0);
	SetPipModeParam(layout,GetPgmCurrentInput(),GetPrvCurrentInput());
	SysDelay1ms(6000);
	FPGASetFreeze(0);
	menuDisableBusy();
	//SaveSystemParam();
	SaveCurrentUserSettingParam();
}


//###############################################################################################################
//
void PipModeSetting(BYTE onOff)
{
	int x, y, w, h;
	GetSystemParam()->PipModeSwitch = onOff;
	if(onOff == 1)
	{
		menuEnterPage(MENU_INDEX_PIP);
		menuSetBusy(0,6);
		FPGASetFreeze(1);
		GetSystemParam()->PipLayout = LAYOUT_PIP_LT;
		GetSystemParam()->CurrentWindow = 1;
		CalcPipLayoutSize(GetSystemParam()->PipLayout,GetPipParam());
		GetSystemParam()->LastSplitMode = GetPgmCurrentDisplayMode();
		SaveFpgaSplitParamToFlash(0);
		SetPgmCurrentDisplayMode(_OSD_DM_2P_PIP);
		GetPanleAndSplitParamRam()->splitMode = _OSD_DM_2P_PIP;
		SetPrvCurrentInput(GetPgmCurrentInput());
		UserPipParamSetting(GetPipParam());
		SetPipModeParam(GetSystemParam()->PipLayout,GetPgmCurrentInput(),GetPgmCurrentInput());
		SysDelay1ms(6000);
		FPGASetFreeze(0);
		menuDisableBusy();
	}
	else
	{
		menuEnterPage(MENU_INDEX_PIP);
		menuSetBusy(0,6);
		switch(GetSystemParam()->LastSplitMode)
		{
			case _OSD_DM_1P:
				FieldSplitSetting(GetPanleAndSplitParamRam());
				SetPgmCurrentDisplayMode(_OSD_DM_1P);
			break;
			case _OSD_DM_2P_SACLE_TB:
				FourPortSplitSetting(GetPanleAndSplitParamRam(),_OSD_DM_2P_SACLE_TB);
				SetPgmCurrentDisplayMode(_OSD_DM_2P_SACLE_TB);
			break;
			case _OSD_DM_3P_SACLE_TB:
				ThirdPortSplitSetting(GetPanleAndSplitParamRam(),_OSD_DM_3P_SACLE_TB);
				SetPgmCurrentDisplayMode(_OSD_DM_3P_SACLE_TB);
			break;
			case _OSD_DM_2P_TOW_PORT_TB:
				Horizontal2SplitSetting(GetPanleAndSplitParamRam(),_OSD_DM_2P_TOW_PORT_TB);
				SetPgmCurrentDisplayMode(_OSD_DM_2P_TOW_PORT_TB);
			break;
			default:
				FieldSplitSetting(GetPanleAndSplitParamRam());
				SetPgmCurrentDisplayMode(_OSD_DM_1P);
			break;
		}
		SysDelay1ms(2000);
		menuDisableBusy();
	}
	GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
	SaveCurrentUserSettingParam();
}



//###############################################################################################################
//
void TwoPortSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode)
{
	u8 offset;
	//FourPortSplitParamSt FourPortSplitPara;
	u8 alpha = fourPortSplitParam->alpha;
	u16 firstH,secondH,thirdH,fourthH,minH,maxH;
	u16 firstSectionv,secondSectionv,thirdSectionv,fourthSectionv;
	u16 firstSectionH,secondSectionH,thirdSectionH,fourthSectionH;
	float x;
	GetSystemParam()->Osd4pSwitch = 0;
	GetSystemParam()->PipModeSwitch = 0;
	takeStatus = 0;
	if(mode == _OSD_DM_2P_TB)
	{
		
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->leftRight12_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->leftRight12_UserTotalDisplayHeight;

		if((FourPortSplitPara.stTotalWidth % 2) != 0)
			FourPortSplitPara.stTotalWidth += 1;
		if((FourPortSplitPara.stTotalHeight % 2) != 0)
			FourPortSplitPara.stTotalHeight += 1;

		if(FourPortSplitPara.stTotalHeight > tVideoDefineCustom.vActive)		
			FourPortSplitPara.stTotalHeight = tVideoDefineCustom.vActive;
		
		FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->leftRight12_FirstDisplayWidth;
		FourPortSplitPara.stFirstDisplayHeight = FourPortSplitPara.stTotalHeight;
		
		if((FourPortSplitPara.stFirstDisplayWidth % 2) != 0)
			FourPortSplitPara.stFirstDisplayWidth += 1;
		
		FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stTotalWidth - FourPortSplitPara.stFirstDisplayWidth;

		if(FourPortSplitPara.stSecondDisplayWidth > tVideoDefineCustom.hActive)
			FourPortSplitPara.stSecondDisplayWidth = tVideoDefineCustom.hActive;
		
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stThirdDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stSecondDisplayWidth;
		FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stTotalHeight;

		firstH = FourPortSplitPara.stFirstDisplayWidth;
		secondH = FourPortSplitPara.stSecondDisplayWidth;
		thirdH = FourPortSplitPara.stThirdDisplayWidth;
		fourthH = FourPortSplitPara.stFourthDisplayWidth;
		firstSectionH = firstH;
		secondSectionH = secondH;
		thirdSectionH = thirdH;
		fourthSectionH = fourthH;
		
		firstSectionv = FourPortSplitPara.stFirstDisplayHeight;
		secondSectionv = FourPortSplitPara.stSecondDisplayHeight;
		thirdSectionv = FourPortSplitPara.stThirdDisplayHeight;
		fourthSectionv = FourPortSplitPara.stFourthDisplayHeight;

		maxH = (firstH >= secondH)?(firstH):(secondH);
		minH = (firstH <= secondH)?(firstH):(secondH);
		DebugMessageRtd2796("maxH = %d\r\n",maxH);
		DebugMessageRtd2796("minH = %d\r\n",minH);
		if(maxH > 1920)
		{
			x = (float)maxH/(float)minH;
			DebugMessageRtd2796("x = %f\r\n",x);
			if(firstH > secondH)
			{
				firstH = 1920;
				secondH = 1920 / x;
				if((secondH % 2) != 0)
					secondH += 1;
			}
			else
			{
				secondH = 1920;
				firstH = 1920 / x;
				if((firstH % 2) != 0)
					firstH += 1;
			}
		}

		DebugMessageRtd2796("firstH = %d\r\n",firstH);
		DebugMessageRtd2796("secondH = %d\r\n",secondH);
		
		#if 1
		if(firstH > 1920)
			FourPortSplitPara.stFirstRegionWidthStart = 0;
		else
			FourPortSplitPara.stFirstRegionWidthStart = 1920 - firstH;

			FourPortSplitPara.stSecondRegionWidthStart = FourPortSplitPara.stFirstRegionWidthStart;
		#endif
		
		if(FourPortSplitPara.stTotalHeight > 1080)
			FourPortSplitPara.stFirstRegionHeightStart = 0;
		else
			FourPortSplitPara.stFirstRegionHeightStart = 1080 - FourPortSplitPara.stTotalHeight;
		
		FourPortSplitPara.stSecondRegionHeightStart = 0;

		FourPortSplitPara.splitMode = _OSD_DM_2P_TB;

		//屏幕垂直方向四个参数(全局)
		_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
		_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;

		

		FourPortSplitPara.stFirstDisplayWidth = firstH;
		FourPortSplitPara.stSecondDisplayWidth = secondH;
		FourPortSplitPara.stThirdDisplayWidth = firstH;
		FourPortSplitPara.stFourthDisplayWidth = secondH;
		FourPortSplitPara.stTotalWidth = firstH + secondH;

		if(FourPortSplitPara.stTotalWidth >= 3840)
			FourPortSplitPara.stTotalWidth = 3840;
		if(FourPortSplitPara.stTotalHeight >= 1080)
			FourPortSplitPara.stTotalHeight = 1080;
		
	}
	else if(mode == _OSD_DM_2P_LR)
	{
		
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->topBotton12_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->topBotton12_UserTotalDisplayHeight;

		if((FourPortSplitPara.stTotalWidth % 2) != 0)
			FourPortSplitPara.stTotalWidth += 1;
		if((FourPortSplitPara.stTotalHeight % 2) != 0)
			FourPortSplitPara.stTotalHeight += 1;

		if(FourPortSplitPara.stTotalWidth > tVideoDefineCustom.hActive)		
			FourPortSplitPara.stTotalWidth = tVideoDefineCustom.hActive;
		
		FourPortSplitPara.stFirstDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stFirstDisplayHeight = fourPortSplitParam->topBotton12_firstDisplayHeight;

		if((FourPortSplitPara.stFirstDisplayHeight % 2) != 0)
			FourPortSplitPara.stFirstDisplayHeight += 1;
		
		FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stTotalHeight-FourPortSplitPara.stFirstDisplayHeight;
		if(FourPortSplitPara.stSecondDisplayHeight > tVideoDefineCustom.vActive)
			FourPortSplitPara.stSecondDisplayHeight = tVideoDefineCustom.vActive;
		FourPortSplitPara.stThirdDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		
		FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stTotalHeight-FourPortSplitPara.stThirdDisplayHeight;

		firstH = FourPortSplitPara.stFirstDisplayHeight;
		secondH = FourPortSplitPara.stSecondDisplayHeight;
		thirdH = FourPortSplitPara.stThirdDisplayHeight;
		fourthH = FourPortSplitPara.stFourthDisplayHeight;

		firstSectionH = FourPortSplitPara.stTotalWidth;
		secondSectionH = FourPortSplitPara.stTotalWidth;
		thirdSectionH = FourPortSplitPara.stTotalWidth;
		fourthSectionH = FourPortSplitPara.stTotalWidth;
		
		firstSectionv = FourPortSplitPara.stFirstDisplayHeight;
		secondSectionv = FourPortSplitPara.stSecondDisplayHeight;
		thirdSectionv = FourPortSplitPara.stThirdDisplayHeight;
		fourthSectionv = FourPortSplitPara.stFourthDisplayHeight;
		

		maxH = (firstH >= secondH)?(firstH):(secondH);
		minH = (firstH <= secondH)?(firstH):(secondH);
		DebugMessageRtd2796("maxH = %d\r\n",maxH);
		DebugMessageRtd2796("minH = %d\r\n",minH);
		if(maxH > 1080)
		{
			x = (float)maxH/(float)minH;
			DebugMessageRtd2796("x = %f\r\n",x);
			if(firstH > secondH)
			{
				firstH = 1080;
				secondH = 1080 / x;
				if((secondH % 2) != 0)
					secondH += 1;
			}
			else
			{
				secondH = 1080;
				firstH = 1080 / x;
				if((firstH % 2) != 0)
					firstH += 1;
			}
		}
		
		//FourPortSplitPara.stFirstDisplayHeight = firstH;
		//FourPortSplitPara.stSecondDisplayHeight = secondH;
		FourPortSplitPara.stTotalHeight = firstH + secondH;

		DebugMessageRtd2796("firstH = %d\r\n",firstH);
		DebugMessageRtd2796("secondH = %d\r\n",secondH);
		
		if(FourPortSplitPara.stFirstDisplayWidth > 1920)
			FourPortSplitPara.stFirstRegionWidthStart = 0;
		else
			FourPortSplitPara.stFirstRegionWidthStart = 1920 - FourPortSplitPara.stFirstDisplayWidth;

		if(FourPortSplitPara.stThirdDisplayWidth > 1920)
			FourPortSplitPara.stSecondRegionWidthStart = 0;
		else
			FourPortSplitPara.stSecondRegionWidthStart = 1920 - FourPortSplitPara.stThirdDisplayWidth;

		if(firstH > 1080)
			FourPortSplitPara.stFirstRegionHeightStart = 0;
		else
			FourPortSplitPara.stFirstRegionHeightStart = 1080 - firstH;
		
		FourPortSplitPara.stSecondRegionHeightStart = FourPortSplitPara.stFirstRegionHeightStart;
		//FourPortSplitPara.stFirstRegionHeightStart = 1080 - FourPortSplitPara.stFirstDisplayHeight;
		//FourPortSplitPara.stSecondRegionHeightStart = 1080 - FourPortSplitPara.stFirstDisplayHeight;

		FourPortSplitPara.splitMode = _OSD_DM_2P_LR;

		//屏幕垂直方向四个参数(全局)
		_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
		_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;

		if(FourPortSplitPara.stTotalWidth >= 1920)
			FourPortSplitPara.stTotalWidth = 1920;
		if(FourPortSplitPara.stTotalHeight >= 2160)
			FourPortSplitPara.stTotalHeight = 2160;

		FourPortSplitPara.stFirstDisplayHeight = firstH;
		FourPortSplitPara.stSecondDisplayHeight = secondH;
		FourPortSplitPara.stThirdDisplayHeight = firstH;
		FourPortSplitPara.stFourthDisplayHeight = secondH;
		
		//FourPortSplitPara.stTotalHeight /= 2;
	}
	
	
	UserSplitParamSetting(FourPortSplitPara);	//设置2796有效显示区域
	SysDelay1ms(1000);
	SetVideoDisplay(FourPortSplitPara.splitMode);
	SysDelay1ms(1000);

	layerParam.layerAWidth = firstSectionH;
	layerParam.layerAHeight = firstSectionv;
	layerParam.layerAXstart = 0;
	layerParam.layerAVstart = 0;
	layerParam.layerASplitStart = FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerACropStart = 0;

	layerParam.layerBWidth = secondSectionH;
	layerParam.layerBHeight = secondSectionv;
	layerParam.layerBXstart = 0;
	layerParam.layerBVstart = 0;
	layerParam.layerBSplitStart = 0;
	layerParam.layerBCropStart = 0;

	layerParam.layerCWidth = thirdSectionH;
	layerParam.layerCHeight = thirdSectionv;
	layerParam.layerCXstart = 0;
	layerParam.layerCVstart = 0;
	layerParam.layerCSplitStart = FourPortSplitPara.stSecondRegionWidthStart;
	layerParam.layerCCropStart = 0;

	layerParam.layerDWidth = fourthSectionH;
	layerParam.layerDHeight = fourthSectionv;
	layerParam.layerDXstart = 0;
	layerParam.layerDVstart = 0;
	layerParam.layerDSplitStart = 0;
	layerParam.layerDCropStart = 0;
	
	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = firstSectionH;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = firstSectionv;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = firstSectionH;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = firstSectionv;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = MAX_DISPLAY_WIDTH/2;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = secondSectionH;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = secondSectionv;
	
	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = secondSectionH;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = secondSectionv;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = 0;
	if(secondSectionH >= 1920)
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = secondSectionH;

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = thirdSectionH;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = thirdSectionv;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = thirdSectionH;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = thirdSectionv;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stSecondRegionWidthStart;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = MAX_DISPLAY_WIDTH/2;
	

	//###############################################################################################################
	//输出口4参数
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = fourthSectionH;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = fourthSectionv;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = fourthSectionH;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = fourthSectionv;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = 0;
	if(fourthSectionH >= 1920)
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = fourthSectionH;

	SetSplitParamToFpga(&_panelSplitInfo);
	
	if(mode == _OSD_DM_2P_TB)
		FPGA_set(0x06,_PBP_UP_DOWN);
	else if(mode == _OSD_DM_2P_LR)
		FPGA_set(0x06,_PBP_LEFT_RIGHT);
}



//###############################################################################################################
//
void Vertical2SplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode)
{
	u8 offset;
	//FourPortSplitParamSt FourPortSplitPara;
	u8 alpha = fourPortSplitParam->alpha;
	u16 firstH,secondH,thirdH,fourthH,minH,maxH;
	u16 firstSectionv,secondSectionv,thirdSectionv,fourthSectionv;
	u16 firstSectionH,secondSectionH,thirdSectionH,fourthSectionH;
	float x;

	//takeStatus = 0;
	
	//if(mode == _OSD_DM_2P_TOW_PORT_LR)
	//{
		GetSystemParam()->Osd4pSwitch = 0;
		GetSystemParam()->PipModeSwitch = 0;
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->FourPortVer12_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->FourPortVer12_UserTotalDisplayHeight;

		if((FourPortSplitPara.stTotalWidth % 2) != 0)
			FourPortSplitPara.stTotalWidth += 1;
		if((FourPortSplitPara.stTotalHeight % 2) != 0)
			FourPortSplitPara.stTotalHeight += 1;

		if(FourPortSplitPara.stTotalWidth > tVideoDefineCustom.hActive)		
			FourPortSplitPara.stTotalWidth = tVideoDefineCustom.hActive;
		
		FourPortSplitPara.stFirstDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stFirstDisplayHeight = fourPortSplitParam->FourPortVer12_FirstDisplayHeight;

		if((FourPortSplitPara.stFirstDisplayHeight % 2) != 0)
			FourPortSplitPara.stFirstDisplayHeight += 1;
		
		FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stTotalHeight-FourPortSplitPara.stFirstDisplayHeight;
		if(FourPortSplitPara.stSecondDisplayHeight > tVideoDefineCustom.vActive)
			FourPortSplitPara.stSecondDisplayHeight = tVideoDefineCustom.vActive;
		FourPortSplitPara.stThirdDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		
		FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stTotalWidth;
		FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stTotalHeight-FourPortSplitPara.stThirdDisplayHeight;

		firstH = FourPortSplitPara.stFirstDisplayHeight;
		secondH = FourPortSplitPara.stSecondDisplayHeight;
		thirdH = FourPortSplitPara.stThirdDisplayHeight;
		fourthH = FourPortSplitPara.stFourthDisplayHeight;

		firstSectionH = FourPortSplitPara.stTotalWidth;
		secondSectionH = FourPortSplitPara.stTotalWidth;
		thirdSectionH = FourPortSplitPara.stTotalWidth;
		fourthSectionH = FourPortSplitPara.stTotalWidth;
		
		firstSectionv = FourPortSplitPara.stFirstDisplayHeight;
		secondSectionv = FourPortSplitPara.stSecondDisplayHeight;
		thirdSectionv = FourPortSplitPara.stThirdDisplayHeight;
		fourthSectionv = FourPortSplitPara.stFourthDisplayHeight;
		

		maxH = (firstH >= secondH)?(firstH):(secondH);
		minH = (firstH <= secondH)?(firstH):(secondH);
		DebugMessageRtd2796("maxH = %d\r\n",maxH);
		DebugMessageRtd2796("minH = %d\r\n",minH);
		if(maxH > 1080)
		{
			x = (float)maxH/(float)minH;
			DebugMessageRtd2796("x = %f\r\n",x);
			if(firstH > secondH)
			{
				firstH = 1080;
				secondH = 1080 / x;
				if((secondH % 2) != 0)
					secondH += 1;
			}
			else
			{
				secondH = 1080;
				firstH = 1080 / x;
				if((firstH % 2) != 0)
					firstH += 1;
			}
		}
		
		//FourPortSplitPara.stFirstDisplayHeight = firstH;
		//FourPortSplitPara.stSecondDisplayHeight = secondH;
		FourPortSplitPara.stTotalHeight = firstH + secondH;

		DebugMessageRtd2796("firstH = %d\r\n",firstH);
		DebugMessageRtd2796("secondH = %d\r\n",secondH);
		
		if(FourPortSplitPara.stFirstDisplayWidth > 1920)
			FourPortSplitPara.stFirstRegionWidthStart = 0;
		else
			FourPortSplitPara.stFirstRegionWidthStart = 1920 - FourPortSplitPara.stFirstDisplayWidth;

		if(FourPortSplitPara.stThirdDisplayWidth > 1920)
			FourPortSplitPara.stSecondRegionWidthStart = 0;
		else
			FourPortSplitPara.stSecondRegionWidthStart = 1920 - FourPortSplitPara.stThirdDisplayWidth;

		if(firstH > 1080)
			FourPortSplitPara.stFirstRegionHeightStart = 0;
		else
			FourPortSplitPara.stFirstRegionHeightStart = 1080 - firstH;
		
		FourPortSplitPara.stSecondRegionHeightStart = FourPortSplitPara.stFirstRegionHeightStart;
		//FourPortSplitPara.stFirstRegionHeightStart = 1080 - FourPortSplitPara.stFirstDisplayHeight;
		//FourPortSplitPara.stSecondRegionHeightStart = 1080 - FourPortSplitPara.stFirstDisplayHeight;

		FourPortSplitPara.splitMode = _OSD_DM_2P_TOW_PORT_LR;

		//屏幕垂直方向四个参数(全局)
		_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
		_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;

		if(FourPortSplitPara.stTotalWidth >= 1920)
			FourPortSplitPara.stTotalWidth = 1920;
		if(FourPortSplitPara.stTotalHeight >= 2160)
			FourPortSplitPara.stTotalHeight = 2160;

		FourPortSplitPara.stFirstDisplayHeight = firstH;
		FourPortSplitPara.stSecondDisplayHeight = secondH;
		FourPortSplitPara.stThirdDisplayHeight = firstH;
		FourPortSplitPara.stFourthDisplayHeight = secondH;
		
		//FourPortSplitPara.stTotalHeight /= 2;
	//}
	
	
	UserSplitParamSetting(FourPortSplitPara);	//设置2796有效显示区域
	SysDelay1ms(1000);
	SetVideoDisplay(FourPortSplitPara.splitMode);
	SysDelay1ms(1000);

	layerParam.layerAWidth = firstSectionH;
	layerParam.layerAHeight = firstSectionv;
	layerParam.layerAXstart = 0;
	layerParam.layerAVstart = 0;
	layerParam.layerASplitStart = FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerACropStart = 0;

	layerParam.layerBWidth = secondSectionH;
	layerParam.layerBHeight = secondSectionv;
	layerParam.layerBXstart = 0;
	layerParam.layerBVstart = 0;
	layerParam.layerBSplitStart = 0;
	layerParam.layerBCropStart = 0;

	layerParam.layerCWidth = 0;
	layerParam.layerCHeight = 0;
	layerParam.layerCXstart = 0;
	layerParam.layerCVstart = 0;
	layerParam.layerCSplitStart = 0;
	layerParam.layerCCropStart = 0;

	layerParam.layerDWidth = 0;
	layerParam.layerDHeight = 0;
	layerParam.layerDXstart = 0;
	layerParam.layerDVstart = 0;
	layerParam.layerDSplitStart = 0;
	layerParam.layerDCropStart = 0;

	memcpy((u8*)&layerParamBackUp,(u8*)&layerParam,sizeof(LAYER_PARAM_Typedef));
	
	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = firstSectionH;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = firstSectionv;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = 0;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = MAX_DISPLAY_WIDTH/2;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = secondSectionH;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = secondSectionv;
	
	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = 0;
	if(secondSectionH >= 1920)
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = secondSectionH;

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = thirdSectionH;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = thirdSectionv;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stSecondRegionWidthStart;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = MAX_DISPLAY_WIDTH/2;
	

	//###############################################################################################################
	//输出口4参数
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = fourthSectionH;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = fourthSectionv;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = 0;
	if(fourthSectionH >= 1920)
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = fourthSectionH;

	GetSystemParam()->FpgaDisMode = _PBP_LEFT_RIGHT;
	FPGA_set(0x06,GetSystemParam()->FpgaDisMode);
	SetSplitParamToFpga(&_panelSplitInfo);
}


//###############################################################################################################
//
void Two4kTakeModeSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode)
{
	//FourPortSplitParamSt FourPortSplitPara;
	u8 offset;
	u8 alpha = fourPortSplitParam->alpha;
	
	takeStatus = 0;
	
	if(mode == _OSD_DM_2P_TB)
	{
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->leftRight12_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->leftRight12_UserTotalDisplayHeight;
		FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->leftRight12_FirstDisplayWidth;
		FourPortSplitPara.stFirstDisplayHeight = fourPortSplitParam->leftRight12_FirstDisplayHeight;
		FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		FourPortSplitPara.stThirdDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		if(FourPortSplitPara.stFirstDisplayHeight > 1080)		//限高,第一块屏高度不能大于1080
			FourPortSplitPara.stFirstDisplayHeight = 1080;

		FourPortSplitPara.stFirstRegionWidthStart = 0;//1920-FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stSecondRegionWidthStart = 0;
		FourPortSplitPara.stFirstRegionHeightStart = 0;//FourPortSplitPara.leftRight12_FirstDisplayHeight;
		FourPortSplitPara.stSecondRegionHeightStart = 0;//FourPortSplitPara.leftRight12_FirstDisplayHeight;
		
		FourPortSplitPara.stTotalHeight *= 2;
	}
	else if(mode == _OSD_DM_2P_LR)
	{
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->topBotton12_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->topBotton12_UserTotalDisplayHeight;
		FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->topBotton12_firstDisplayWidth;
		FourPortSplitPara.stFirstDisplayHeight = fourPortSplitParam->topBotton12_firstDisplayHeight;
		FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		FourPortSplitPara.stThirdDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stFirstDisplayHeight;
		if(FourPortSplitPara.stFirstDisplayHeight > 1080)		//限高,第一块屏高度不能大于1080
			FourPortSplitPara.stFirstDisplayHeight = 1080;

		FourPortSplitPara.stFirstRegionWidthStart = 0;//1920-FourPortSplitPara.stFirstDisplayWidth;
		FourPortSplitPara.stSecondRegionWidthStart = 0;
		FourPortSplitPara.stFirstRegionHeightStart = 0;//FourPortSplitPara.leftRight12_FirstDisplayHeight;
		FourPortSplitPara.stSecondRegionHeightStart = 0;//FourPortSplitPara.leftRight12_FirstDisplayHeight;
		FourPortSplitPara.stTotalWidth *= 2;
	}
	
	
	FourPortSplitPara.splitMode = mode;

	//FourPortSplitPara.stTotalWidth = (MAX_DISPLAY_WIDTH - FourPortSplitPara.stTotalWidth) / 2;
	//FourPortSplitPara.stTotalWidth = (MAX_DISPLAY_HEIGHT - FourPortSplitPara.stTotalWidth) / 2;

	
	//屏幕垂直方向四个参数(全局)
	_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
	_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
	_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = FourPortSplitPara.stFirstRegionHeightStart + 2160;
	_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = FourPortSplitPara.stFirstRegionHeightStart + 2160;
	
	
	UserSplitParamSetting(FourPortSplitPara);	//设置2796有效显示区域
	SysDelay1ms(1000);
	SetVideoDisplay(FourPortSplitPara.splitMode);
	SysDelay1ms(1000);
	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = FourPortSplitPara.stFirstDisplayWidth/2;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = FourPortSplitPara.stFirstDisplayHeight;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = FourPortSplitPara.stFirstDisplayWidth/2;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = FourPortSplitPara.stFirstDisplayHeight;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;

	

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stFirstDisplayWidth/2;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropStartPos = FourPortSplitPara.stFirstDisplayWidth/2;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropEndPos = MAX_DISPLAY_WIDTH/2;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = FourPortSplitPara.stSecondDisplayWidth/2;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = FourPortSplitPara.stSecondDisplayHeight;

	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = FourPortSplitPara.stSecondDisplayWidth/2;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = FourPortSplitPara.stSecondDisplayHeight;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stSecondDisplayWidth/2;
	
	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropStartPos = FourPortSplitPara.stSecondDisplayWidth/2;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropEndPos = FourPortSplitPara.stSecondDisplayWidth;

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = FourPortSplitPara.stThirdDisplayWidth/2;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = FourPortSplitPara.stThirdDisplayWidth/2;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stThirdDisplayWidth/2;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitCropStartPos = FourPortSplitPara.stThirdDisplayWidth/2;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitCropEndPos = MAX_DISPLAY_WIDTH/2;
	

	//###############################################################################################################
	//输出口4参数
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = FourPortSplitPara.stFourthDisplayWidth/2;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = FourPortSplitPara.stFourthDisplayWidth/2;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = FourPortSplitPara.stFourthDisplayWidth/2;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitCropStartPos = FourPortSplitPara.stFourthDisplayWidth/2;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitCropEndPos = FourPortSplitPara.stFourthDisplayWidth;

	//if(mode == _OSD_DM_2P_TB)
	//	FPGA_set(0x06,_PBP_UP_DOWN);
	//else if(mode == _OSD_DM_2P_LR)
	FPGA_set(0x06,_TWO_4K_SWAP);
	SetSplitParamToFpga(&_panelSplitInfo);
	
}



//###############################################################################################################
//
void Horizontal2SplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode)
{
	u8 offset;
	//FourPortSplitParamSt FourPortSplitPara;
	u8 alpha = fourPortSplitParam->alpha;
	u16 firstH,secondH,thirdH,fourthH,minH,maxH;
	u16 firstSectionv,secondSectionv,thirdSectionv,fourthSectionv;
	u16 firstSectionH,secondSectionH,thirdSectionH,fourthSectionH;
	float x;
	GetSystemParam()->Osd4pSwitch = 0;
	GetSystemParam()->PipModeSwitch = 0;
	if(mode == _OSD_DM_2P_TOW_PORT_TB)
	{
		
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->FourPort12_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->FourPort12_UserTotalDisplayHeight;

		if((FourPortSplitPara.stTotalWidth % 2) != 0)
			FourPortSplitPara.stTotalWidth += 1;
		if((FourPortSplitPara.stTotalHeight % 2) != 0)
			FourPortSplitPara.stTotalHeight += 1;
		if(FourPortSplitPara.stTotalHeight > tVideoDefineCustom.vActive)		
			FourPortSplitPara.stTotalHeight = tVideoDefineCustom.vActive;
		FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->FourPort12_FirstDisplayWidth;
		FourPortSplitPara.stFirstDisplayHeight = FourPortSplitPara.stTotalHeight;
		if((FourPortSplitPara.stFirstDisplayWidth % 2) != 0)
			FourPortSplitPara.stFirstDisplayWidth += 1;
		FourPortSplitPara.stSecondDisplayWidth = FourPortSplitPara.stTotalWidth - FourPortSplitPara.stFirstDisplayWidth;
		if(FourPortSplitPara.stSecondDisplayWidth > tVideoDefineCustom.hActive)
			FourPortSplitPara.stSecondDisplayWidth = tVideoDefineCustom.hActive;
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stTotalHeight;
		firstH = FourPortSplitPara.stFirstDisplayWidth;
		secondH = FourPortSplitPara.stSecondDisplayWidth;
		firstSectionH = firstH;
		secondSectionH = secondH;
		firstSectionv = FourPortSplitPara.stFirstDisplayHeight;
		secondSectionv = FourPortSplitPara.stSecondDisplayHeight;
		maxH = (firstH >= secondH)?(firstH):(secondH);
		minH = (firstH <= secondH)?(firstH):(secondH);
		DebugMessageRtd2796("maxH = %d\r\n",maxH);
		DebugMessageRtd2796("minH = %d\r\n",minH);
		if(maxH > 1920)
		{
			x = (float)maxH/(float)minH;
			DebugMessageRtd2796("x = %f\r\n",x);
			if(firstH > secondH)
			{
				firstH = 1920;
				secondH = 1920 / x;
				if((secondH % 2) != 0)
					secondH += 1;
			}
			else
			{
				secondH = 1920;
				firstH = 1920 / x;
				if((firstH % 2) != 0)
					firstH += 1;
			}
		}
		DebugMessageRtd2796("firstH = %d\r\n",firstH);
		DebugMessageRtd2796("secondH = %d\r\n",secondH);
		#if 1
		if(firstH > 1920)
			FourPortSplitPara.stFirstRegionWidthStart = 0;
		else
			FourPortSplitPara.stFirstRegionWidthStart = 1920 - firstH;

			FourPortSplitPara.stSecondRegionWidthStart = FourPortSplitPara.stFirstRegionWidthStart;
		#endif
		
		if(FourPortSplitPara.stTotalHeight > 1080)
			FourPortSplitPara.stFirstRegionHeightStart = 0;
		else
			FourPortSplitPara.stFirstRegionHeightStart = 1080 - FourPortSplitPara.stTotalHeight;
		
		FourPortSplitPara.stSecondRegionHeightStart = 0;

		FourPortSplitPara.splitMode = mode;

		//屏幕垂直方向四个参数(全局)
		_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
		_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy > 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;

		FourPortSplitPara.stFirstDisplayWidth = firstH;
		FourPortSplitPara.stSecondDisplayWidth = secondH;
		FourPortSplitPara.stTotalWidth = firstH + secondH;

		if(FourPortSplitPara.stTotalWidth >= 3840)
			FourPortSplitPara.stTotalWidth = 3840;
		if(FourPortSplitPara.stTotalHeight >= 1080)
			FourPortSplitPara.stTotalHeight = 1080;
		
	}
	
	
	UserSplitParamSetting(FourPortSplitPara);	//设置2796有效显示区域
	SysDelay1ms(1000);
	SetVideoDisplay(FourPortSplitPara.splitMode);
	SysDelay1ms(1000);

	layerParam.layerAWidth = firstSectionH;
	layerParam.layerAHeight = firstSectionv;
	layerParam.layerAXstart = 0;
	layerParam.layerAVstart = 0;
	layerParam.layerASplitStart = FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerACropStart = 0;

	layerParam.layerBWidth = secondSectionH;
	layerParam.layerBHeight = secondSectionv;
	layerParam.layerBXstart = 0;
	layerParam.layerBVstart = 0;
	layerParam.layerBSplitStart = 0;
	layerParam.layerBCropStart = 0;

	#if 1
	layerParam.layerCWidth = 0;
	layerParam.layerCHeight = 0;
	layerParam.layerCXstart = 0;
	layerParam.layerCVstart = 0;
	layerParam.layerCSplitStart = 0;
	layerParam.layerCCropStart = 0;

	layerParam.layerDWidth = 0;
	layerParam.layerDHeight = 0;
	layerParam.layerDXstart = 0;
	layerParam.layerDVstart = 0;
	layerParam.layerDSplitStart = 0;
	layerParam.layerDCropStart = 0;
	#endif

	memcpy((u8*)&layerParamBackUp,(u8*)&layerParam,sizeof(LAYER_PARAM_Typedef));
	
	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = firstSectionH;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = firstSectionv;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = 0;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = 1920;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropEndPos = 0;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = secondSectionH;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = secondSectionv;
	
	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = 0;
	if(secondSectionH >= 1920)
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = 1920;
	else
		_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = secondH;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropEndPos = 0;

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = 0;
	
	_panelSplitInfo.OutputPort2SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitCropEndPos = 0;
	//###############################################################################################################
	//输出口4参数
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;


	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = 0;

	_panelSplitInfo.OutputPort3SplitInfo.hSplitCropStartPos = 0;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitCropEndPos = 0;

	//if(mode == _OSD_DM_2P_TB)
	//FPGA_set(0x06,_PBP_UP_DOWN);
	GetSystemParam()->FpgaDisMode = _PBP_UP_DOWN;
	FPGA_set(0x06,GetSystemParam()->FpgaDisMode);
	SetSplitParamToFpga(&_panelSplitInfo);
	
}



//###############################################################################################################
//
void ThirdPortSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode)
{
	u8 offset;
	//FourPortSplitParamSt FourPortSplitPara;
	u8 alpha = fourPortSplitParam->alpha;

	u32 firstH,secondH,thirdH,fourthH,frontSectionH,backSectionH;
	u32 maxH;
	u16 firstSectionH,secondSectionH,thirdSectionH,fourthSectionH;
	float scaleFactor;//比例系数
	GetSystemParam()->Osd4pSwitch = 0;
	GetSystemParam()->PipModeSwitch = 0;
	if(mode == _OSD_DM_3P_SACLE_TB)
	{
		
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->leftRight13_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->leftRight13_UserTotalDisplayHeight;

		if((FourPortSplitPara.stTotalWidth % 2) != 0)
			FourPortSplitPara.stTotalWidth += 1;
		if((FourPortSplitPara.stTotalHeight % 2) != 0)
			FourPortSplitPara.stTotalHeight += 1;

		if(FourPortSplitPara.stTotalHeight > tVideoDefineCustom.vActive)
	    {
	        FourPortSplitPara.stTotalHeight = tVideoDefineCustom.vActive;
	    }
		FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->leftRight13_FirstDisplayWidth;
		FourPortSplitPara.stSecondDisplayWidth = fourPortSplitParam->leftRight13_SecongDisplayWidth;
		FourPortSplitPara.stThirdDisplayWidth = fourPortSplitParam->leftRight14_ThirdDisplayWidth;

		if((FourPortSplitPara.stFirstDisplayWidth % 2) != 0)
			FourPortSplitPara.stFirstDisplayWidth += 1;
		if((FourPortSplitPara.stSecondDisplayWidth % 2) != 0)
			FourPortSplitPara.stSecondDisplayWidth += 1;
		if((FourPortSplitPara.stThirdDisplayWidth % 2) != 0)
			FourPortSplitPara.stThirdDisplayWidth += 1;
	
		FourPortSplitPara.stFourthDisplayWidth = 0;//FourPortSplitPara.stTotalWidth-FourPortSplitPara.stFirstDisplayWidth\
			-FourPortSplitPara.stSecondDisplayWidth-FourPortSplitPara.stThirdDisplayWidth;

		if(FourPortSplitPara.stFirstDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stFirstDisplayWidth = tVideoDefineCustom.hActive;
	    }
		if(FourPortSplitPara.stSecondDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stSecondDisplayWidth = tVideoDefineCustom.hActive;
	    }
		if(FourPortSplitPara.stThirdDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stThirdDisplayWidth = tVideoDefineCustom.hActive;
	    }
		if(FourPortSplitPara.stFourthDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stFourthDisplayWidth = tVideoDefineCustom.hActive;
	    }
		
		FourPortSplitPara.stFirstDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stTotalHeight;

		firstH = FourPortSplitPara.stFirstDisplayWidth;
		secondH = FourPortSplitPara.stSecondDisplayWidth;
		thirdH = FourPortSplitPara.stThirdDisplayWidth;
		fourthH = FourPortSplitPara.stFourthDisplayWidth;

		frontSectionH = firstH + secondH;
		backSectionH = thirdH + fourthH;
		maxH = (frontSectionH > backSectionH)?(frontSectionH):(backSectionH);
		DebugMessageRtd2796("The max SectionH = %d\r\n",maxH);

		if(maxH > 1920)
		{
			scaleFactor = (float)maxH/1920;
			DebugMessageRtd2796("The scale Factor = %f\r\n",scaleFactor);
			firstSectionH = (float)firstH/scaleFactor;
			secondSectionH = (float)secondH/scaleFactor;
			thirdSectionH = (float)thirdH/scaleFactor;
			fourthSectionH = (float)fourthH/scaleFactor;

			if((firstSectionH % 2) != 0)
				firstSectionH += 1;
			if((secondSectionH % 2) != 0)
				secondSectionH += 1;
			if((thirdSectionH % 2) != 0)
				thirdSectionH += 1;
			if((fourthSectionH % 2) != 0)
				fourthSectionH += 1;
		}
		else
		{
			firstSectionH = firstH;
			secondSectionH = secondH;
			thirdSectionH = thirdH;
			fourthSectionH = fourthH;
		}

		DebugMessageRtd2796("The firstSectionH = %d\r\n",firstSectionH);
		DebugMessageRtd2796("The secondSectionH = %d\r\n",secondSectionH);
		DebugMessageRtd2796("The thirdSectionH = %d\r\n",thirdSectionH);
		DebugMessageRtd2796("The fourthSectionH = %d\r\n",fourthSectionH);
		
		if((firstSectionH + secondSectionH) >= 1920)
			FourPortSplitPara.stFirstRegionWidthStart = 0;
		else
			FourPortSplitPara.stFirstRegionWidthStart = 1920 - (firstSectionH + secondSectionH);
		
		FourPortSplitPara.stSecondRegionWidthStart = FourPortSplitPara.stFirstRegionWidthStart;

		if(FourPortSplitPara.stTotalHeight > 1080)
			FourPortSplitPara.stFirstRegionHeightStart = 0;
		else
			FourPortSplitPara.stFirstRegionHeightStart = 1080 - FourPortSplitPara.stTotalHeight;
		
		FourPortSplitPara.stSecondRegionHeightStart = 0;
		FourPortSplitPara.stFourthRegionWidthStart = 0;
		FourPortSplitPara.stFourthRegionHeightStart = 0;

		FourPortSplitPara.stFirstDisplayWidth = (firstSectionH + secondSectionH);
		FourPortSplitPara.stSecondDisplayWidth = (thirdSectionH + fourthSectionH);
		FourPortSplitPara.stThirdDisplayWidth = (firstSectionH + secondSectionH);
		FourPortSplitPara.stFourthDisplayWidth = (thirdSectionH + fourthSectionH);
		FourPortSplitPara.stTotalWidth = FourPortSplitPara.stFirstDisplayWidth + FourPortSplitPara.stSecondDisplayWidth;

		//屏幕垂直方向四个参数(全局)
		_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
		_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy >= 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy >= 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;

		//FourPortSplitPara.stTotalHeight *=2; 

		if(FourPortSplitPara.stTotalHeight > 1080)
			FourPortSplitPara.stTotalHeight = 1080;	
		FPGA_set(0x06,_BEYOND_HEIGHT_WIDTH_SPLIT);	//超宽超高模式
		
	}


	//###############################################################################################################
	//屏幕垂直方向三个参数(全局)
	FourPortSplitPara.splitMode = mode;
	UserSplitParamSetting(FourPortSplitPara);	//设置2796有效显示区域
	SysDelay1ms(1000);
	SetVideoDisplay(mode);
	SysDelay1ms(1000);

	layerParam.layerAWidth = firstH;
	layerParam.layerAHeight = FourPortSplitPara.stFirstDisplayHeight;
	layerParam.layerAXstart = 0;
	layerParam.layerAVstart = 0;
	
	layerParam.layerASplitStart = FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerASplitWidth = firstSectionH;
	layerParam.layerACropStart = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;

	layerParam.layerBWidth = secondH;
	layerParam.layerBHeight = FourPortSplitPara.stSecondDisplayHeight;
	layerParam.layerBXstart = 0;
	layerParam.layerBVstart = 0;
	
	layerParam.layerBSplitStart = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerBSplitWidth = secondSectionH;
	layerParam.layerBCropStart = thirdSectionH+0;

	layerParam.layerCWidth = thirdH;
	layerParam.layerCHeight = FourPortSplitPara.stThirdDisplayHeight;
	layerParam.layerCXstart = 0;
	layerParam.layerCVstart = 0;
	
	layerParam.layerCSplitStart = 0;
	layerParam.layerCSplitWidth = thirdSectionH;
	layerParam.layerCCropStart = 0;

	layerParam.layerDWidth = fourthH;
	layerParam.layerDHeight = FourPortSplitPara.stFourthDisplayHeight;
	layerParam.layerDXstart = 0;
	layerParam.layerDVstart = 0;
	
	layerParam.layerDSplitStart = thirdSectionH;
	layerParam.layerCSplitWidth = fourthSectionH;
	layerParam.layerDCropStart = 0;

	memcpy((u8*)&layerParamBackUp,(u8*)&layerParam,sizeof(LAYER_PARAM_Typedef));
	
	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = firstH;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = FourPortSplitPara.stFirstDisplayHeight;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = thirdH;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropStartPos = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropEndPos = MAX_DISPLAY_WIDTH / 2;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;

	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = secondH;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = FourPortSplitPara.stSecondDisplayHeight;

	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = fourthH;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = secondSectionH+0;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropStartPos = thirdSectionH+0;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropEndPos = (thirdSectionH + secondSectionH);

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = thirdH;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = 0;
	
	//###############################################################################################################
	//输出口4参数
	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = fourthH;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = 0;
	GetSystemParam()->FpgaDisMode = _BEYOND_HEIGHT_WIDTH_SPLIT;
	FPGA_set(0x06,GetSystemParam()->FpgaDisMode);
	SetSplitParamToFpga(&_panelSplitInfo);
		
}



//###############################################################################################################
//
void FourPortSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode)
{
	u8 offset;
	//FourPortSplitParamSt FourPortSplitPara;
	u8 alpha = fourPortSplitParam->alpha;

	u32 firstH,secondH,thirdH,fourthH,frontSectionH,backSectionH;
	u32 maxH;
	u16 firstSectionH,secondSectionH,thirdSectionH,fourthSectionH;
	float scaleFactor;//比例系数
	GetSystemParam()->Osd4pSwitch = 0;
	GetSystemParam()->PipModeSwitch = 0;
	if(mode == _OSD_DM_2P_SACLE_TB)
	{
		
		FourPortSplitPara.stTotalWidth = fourPortSplitParam->leftRight14_UserTotalDisplayWidth;
		FourPortSplitPara.stTotalHeight = fourPortSplitParam->leftRight14_UserTotalDisplayHeight;

		if((FourPortSplitPara.stTotalWidth % 2) != 0)
			FourPortSplitPara.stTotalWidth += 1;
		if((FourPortSplitPara.stTotalHeight % 2) != 0)
			FourPortSplitPara.stTotalHeight += 1;

		if(FourPortSplitPara.stTotalHeight > tVideoDefineCustom.vActive)
	    {
	        FourPortSplitPara.stTotalHeight = tVideoDefineCustom.vActive;
	    }
		FourPortSplitPara.stFirstDisplayWidth = fourPortSplitParam->leftRight14_FirstDisplayWidth;
		FourPortSplitPara.stSecondDisplayWidth = fourPortSplitParam->leftRight14_SecongDisplayWidth;
		FourPortSplitPara.stThirdDisplayWidth = fourPortSplitParam->leftRight14_ThirdDisplayWidth;

		if((FourPortSplitPara.stFirstDisplayWidth % 2) != 0)
			FourPortSplitPara.stFirstDisplayWidth += 1;
		if((FourPortSplitPara.stSecondDisplayWidth % 2) != 0)
			FourPortSplitPara.stSecondDisplayWidth += 1;
		if((FourPortSplitPara.stThirdDisplayWidth % 2) != 0)
			FourPortSplitPara.stThirdDisplayWidth += 1;
	
		FourPortSplitPara.stFourthDisplayWidth = FourPortSplitPara.stTotalWidth-FourPortSplitPara.stFirstDisplayWidth\
			-FourPortSplitPara.stSecondDisplayWidth-FourPortSplitPara.stThirdDisplayWidth;

		if(FourPortSplitPara.stFirstDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stFirstDisplayWidth = tVideoDefineCustom.hActive;
	    }
		if(FourPortSplitPara.stSecondDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stSecondDisplayWidth = tVideoDefineCustom.hActive;
	    }
		if(FourPortSplitPara.stThirdDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stThirdDisplayWidth = tVideoDefineCustom.hActive;
	    }
		if(FourPortSplitPara.stFourthDisplayWidth > tVideoDefineCustom.hActive)
	    {
	        FourPortSplitPara.stFourthDisplayWidth = tVideoDefineCustom.hActive;
	    }
		
		FourPortSplitPara.stFirstDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stSecondDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stThirdDisplayHeight = FourPortSplitPara.stTotalHeight;
		FourPortSplitPara.stFourthDisplayHeight = FourPortSplitPara.stTotalHeight;

		firstH = FourPortSplitPara.stFirstDisplayWidth;
		secondH = FourPortSplitPara.stSecondDisplayWidth;
		thirdH = FourPortSplitPara.stThirdDisplayWidth;
		fourthH = FourPortSplitPara.stFourthDisplayWidth;

		frontSectionH = firstH + secondH;
		backSectionH = thirdH + fourthH;
		maxH = (frontSectionH > backSectionH)?(frontSectionH):(backSectionH);
		DebugMessageRtd2796("The max SectionH = %d\r\n",maxH);

		if(maxH > 1920)
		{
			scaleFactor = (float)maxH/1920;
			DebugMessageRtd2796("The scale Factor = %f\r\n",scaleFactor);
			firstSectionH = (float)firstH/scaleFactor;
			secondSectionH = (float)secondH/scaleFactor;
			thirdSectionH = (float)thirdH/scaleFactor;
			fourthSectionH = (float)fourthH/scaleFactor;

			if((firstSectionH % 2) != 0)
				firstSectionH += 1;
			if((secondSectionH % 2) != 0)
				secondSectionH += 1;
			if((thirdSectionH % 2) != 0)
				thirdSectionH += 1;
			if((fourthSectionH % 2) != 0)
				fourthSectionH += 1;
		}
		else
		{
			firstSectionH = firstH;
			secondSectionH = secondH;
			thirdSectionH = thirdH;
			fourthSectionH = fourthH;
		}

		DebugMessageRtd2796("The firstSectionH = %d\r\n",firstSectionH);
		DebugMessageRtd2796("The secondSectionH = %d\r\n",secondSectionH);
		DebugMessageRtd2796("The thirdSectionH = %d\r\n",thirdSectionH);
		DebugMessageRtd2796("The fourthSectionH = %d\r\n",fourthSectionH);
		
		if((firstSectionH + secondSectionH) >= 1920)
			FourPortSplitPara.stFirstRegionWidthStart = 0;
		else
			FourPortSplitPara.stFirstRegionWidthStart = 1920 - (firstSectionH + secondSectionH);
		
		FourPortSplitPara.stSecondRegionWidthStart = FourPortSplitPara.stFirstRegionWidthStart;

		if(FourPortSplitPara.stTotalHeight > 1080)
			FourPortSplitPara.stFirstRegionHeightStart = 0;
		else
			FourPortSplitPara.stFirstRegionHeightStart = 1080 - FourPortSplitPara.stTotalHeight;
		
		FourPortSplitPara.stSecondRegionHeightStart = 0;
		FourPortSplitPara.stFourthRegionWidthStart = 0;
		FourPortSplitPara.stFourthRegionHeightStart = 0;

		FourPortSplitPara.stFirstDisplayWidth = (firstSectionH + secondSectionH);
		FourPortSplitPara.stSecondDisplayWidth = (thirdSectionH + fourthSectionH);
		FourPortSplitPara.stThirdDisplayWidth = (firstSectionH + secondSectionH);
		FourPortSplitPara.stFourthDisplayWidth = (thirdSectionH + fourthSectionH);
		FourPortSplitPara.stTotalWidth = FourPortSplitPara.stFirstDisplayWidth + FourPortSplitPara.stSecondDisplayWidth;

		//屏幕垂直方向四个参数(全局)
		_panelSplitInfo.vPanelInfo.vPaneStart0Posy = FourPortSplitPara.stFirstRegionHeightStart;//
		_panelSplitInfo.vPanelInfo.vPaneStart1Posy = FourPortSplitPara.stFirstRegionHeightStart;
		_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd0Posy >= 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd0Posy = 2160;
		_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = FourPortSplitPara.stFirstRegionHeightStart + FourPortSplitPara.stTotalHeight*2;
		if(_panelSplitInfo.vPanelInfo.vPaneEnd1Posy >= 2160)
			_panelSplitInfo.vPanelInfo.vPaneEnd1Posy = 2160;

		//FourPortSplitPara.stTotalHeight *=2; 

		if(FourPortSplitPara.stTotalHeight > 1080)
			FourPortSplitPara.stTotalHeight = 1080;	
		FPGA_set(0x06,_BEYOND_HEIGHT_WIDTH_SPLIT);	//超宽超高模式
		
	}


	//###############################################################################################################
	//屏幕垂直方向三个参数(全局)
	FourPortSplitPara.splitMode = mode;
	UserSplitParamSetting(FourPortSplitPara);	//设置2796有效显示区域
	SysDelay1ms(1000);
	SetVideoDisplay(mode);
	SysDelay1ms(1000);

	layerParam.layerAWidth = firstH;
	layerParam.layerAHeight = FourPortSplitPara.stFirstDisplayHeight;
	layerParam.layerAXstart = 0;
	layerParam.layerAVstart = 0;
	
	layerParam.layerASplitStart = FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerASplitWidth = firstSectionH;
	layerParam.layerACropStart = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;

	layerParam.layerBWidth = secondH;
	layerParam.layerBHeight = FourPortSplitPara.stSecondDisplayHeight;
	layerParam.layerBXstart = 0;
	layerParam.layerBVstart = 0;
	
	layerParam.layerBSplitStart = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;
	layerParam.layerBSplitWidth = secondSectionH;
	layerParam.layerBCropStart = thirdSectionH+0;

	layerParam.layerCWidth = thirdH;
	layerParam.layerCHeight = FourPortSplitPara.stThirdDisplayHeight;
	layerParam.layerCXstart = 0;
	layerParam.layerCVstart = 0;
	
	layerParam.layerCSplitStart = 0;
	layerParam.layerCSplitWidth = thirdSectionH;
	layerParam.layerCCropStart = 0;

	layerParam.layerDWidth = fourthH;
	layerParam.layerDHeight = FourPortSplitPara.stFourthDisplayHeight;
	layerParam.layerDXstart = 0;
	layerParam.layerDVstart = 0;
	
	layerParam.layerDSplitStart = thirdSectionH;
	layerParam.layerCSplitWidth = fourthSectionH;
	layerParam.layerDCropStart = 0;

	memcpy((u8*)&layerParamBackUp,(u8*)&layerParam,sizeof(LAYER_PARAM_Typedef));
	
	//###############################################################################################################
	//输出口1参数
	_panelSplitInfo.OutpuPort0tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort0tLayerA.hLayerWidth = firstH;
	_panelSplitInfo.OutpuPort0tLayerA.vLayerHeight = FourPortSplitPara.stFirstDisplayHeight;

	_panelSplitInfo.OutputPort0LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort0LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort0LayerB.hLayerWidth = thirdH;
	_panelSplitInfo.OutputPort0LayerB.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;

	_panelSplitInfo.OutputPort0LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort0LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveStartPos = FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitActiveEndPos = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;

	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropStartPos = firstSectionH + FourPortSplitPara.stFirstRegionWidthStart;
	_panelSplitInfo.OutputPort0SplitInfo.hSplitCropEndPos = MAX_DISPLAY_WIDTH / 2;

	//###############################################################################################################
	//输出口2参数

	_panelSplitInfo.OutpuPort1tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerStartPos = 0;

	
	_panelSplitInfo.OutpuPort1tLayerA.hLayerWidth = secondH;
	_panelSplitInfo.OutpuPort1tLayerA.vLayerHeight = FourPortSplitPara.stSecondDisplayHeight;

	_panelSplitInfo.OutputPort1LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort1LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort1LayerB.hLayerWidth = fourthH;
	_panelSplitInfo.OutputPort1LayerB.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;
	
	_panelSplitInfo.OutputPort1LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort1LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitActiveEndPos = secondSectionH+0;

	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropStartPos = thirdSectionH+0;
	_panelSplitInfo.OutputPort1SplitInfo.hSplitCropEndPos = (thirdSectionH + secondSectionH);

	//###############################################################################################################
	//输出口3参数
	_panelSplitInfo.OutpuPort2tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerStartPos = 0;

	_panelSplitInfo.OutpuPort2tLayerA.hLayerWidth = thirdH;
	_panelSplitInfo.OutpuPort2tLayerA.vLayerHeight = FourPortSplitPara.stThirdDisplayHeight;

	_panelSplitInfo.OutputPort2LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort2LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort2LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort2LayerAlpha.layerA_Alpha = alpha;
	_panelSplitInfo.OutputPort2LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort2SplitInfo.hSplitActiveEndPos = 0;
	
	//###############################################################################################################
	//输出口4参数
	_panelSplitInfo.OutpuPort3tLayerA.hLayerStartPos = 0;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerStartPos = 0;
	
	_panelSplitInfo.OutpuPort3tLayerA.hLayerWidth = fourthH;
	_panelSplitInfo.OutpuPort3tLayerA.vLayerHeight = FourPortSplitPara.stFourthDisplayHeight;

	_panelSplitInfo.OutputPort3LayerB.hLayerStartPos = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerStartPos = 0;

	_panelSplitInfo.OutputPort3LayerB.hLayerWidth = 0;
	_panelSplitInfo.OutputPort3LayerB.vLayerHeight = 0;
	
	_panelSplitInfo.OutputPort3LayerAlpha.layerA_Alpha = 0;
	_panelSplitInfo.OutputPort3LayerAlpha.layerB_Alpha = 0;

	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveStartPos = 0;
	_panelSplitInfo.OutputPort3SplitInfo.hSplitActiveEndPos = 0;
	GetSystemParam()->FpgaDisMode = _BEYOND_HEIGHT_WIDTH_SPLIT;
	FPGA_set(0x06,GetSystemParam()->FpgaDisMode);
	SetSplitParamToFpga(&_panelSplitInfo);
}



//###############################################################################################################
//
void _4KModeSetting(void)
{
    Command_TypeDef cmd;

	DebugMessageRtd2796("_4KModeSetting()\r\n");
    cmd.command = 0xF0;
    cmd.data_1 = 0x18;
    cmd.data_2 = 0x00;
    cmd.data_3 = (u8)sizeof(PanelParamSt);
    cmd.data_4 = (u8)(sizeof(PanelParamSt) >> 8);
	SendRs232CommandToRTD2796(cmd);
}


//#define 


//###############################################################################################################
//
void SetDpEdid(_DP_EDID_ENUM edidIndex,u8 *edidBuf)
{
    Command_TypeDef cmd;
	u8 buf[DETAILED_DESCRIPTOR_SIZE + 1];
	u8 i;

	DebugMessageRtd2796("SetDpEdid\r\n");
    cmd.command = 0xF0;
    cmd.data_1 = 0x19;
    cmd.data_2 = edidIndex;
    cmd.data_3 = (u8)(DETAILED_DESCRIPTOR_SIZE+1);
    cmd.data_4 = (u8)((DETAILED_DESCRIPTOR_SIZE+1) >> 8);
	SendRs232CommandToRTD2796(cmd);

	memcpy(buf,edidBuf,DETAILED_DESCRIPTOR_SIZE);

	buf[DETAILED_DESCRIPTOR_SIZE] = GetPanelParamCheskSum(edidBuf,(DETAILED_DESCRIPTOR_SIZE));
	UARTSendBuffer(RTD2796_UART_PORT,buf,DETAILED_DESCRIPTOR_SIZE+1);

	for(i = 0;i <= DETAILED_DESCRIPTOR_SIZE;i++)
	{
		DebugMessageRtd2796("buf[%d] = %x\r\n",i,buf[i]);
	}
}


//###############################################################################################################
//
void videoSetInputSource(u8 source)
{

	//if(source > 3)
	//	source = 3;
    Command_TypeDef cmd;
	DebugMessageRtd2796("video_setInputSource(%d)\r\n",source);
    cmd.command = 0x73;
    cmd.data_1 = 0x20;
    cmd.data_2 = source;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;
    //send_cmd_to_video(&cmd);
	SendRs232CommandToRTD2796(cmd);
}

//###############################################################################################################
//
void videoSetPipInputSource(EnumDisplayRegion enumRegion,u8 source)
{
    Command_TypeDef cmd;
	DebugMessageRtd2796("videoSetPipInputSource(%d)\r\n",source);
    cmd.command = 0x73;
    cmd.data_1 = 0x22;
    cmd.data_2 = enumRegion;
    cmd.data_3 = source;
    cmd.data_4 = 0x00;
	SendRs232CommandToRTD2796(cmd);
}



//###############################################################################################################
//
void videoInputSwap(u8 source)
{

	//if(source > 3)
	//	source = 3;
    Command_TypeDef cmd;
	DebugMessageRtd2796("videoInputSwap(%d)\r\n",source);
    cmd.command = 0x73;
    cmd.data_1 = 0x20;
    cmd.data_2 = 8;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;
    //send_cmd_to_video(&cmd);
	SendRs232CommandToRTD2796(cmd);

}


//###############################################################################################################
//
void videoSendMassage(u8 Massage)
{
    Command_TypeDef cmd;
	DebugMessageRtd2796("videoSendMassage(%d)\r\n",Massage);
    cmd.command = 0x73;
    cmd.data_1 = 0x20;
    cmd.data_2 = 9;
    cmd.data_3 = Massage;
    cmd.data_4 = 0x00;
	SendRs232CommandToRTD2796(cmd);

}



//###############################################################################################################
//
void GetvideoInputSource(void)
{
    Command_TypeDef cmd;

	DebugMessageRtd2796("GetvideoInputSource()\r\n");
    cmd.command = 0x72;
    cmd.data_1 = 0x01;
    cmd.data_2 = 0x00;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;
	SendRs232CommandToRTD2796(cmd);	
}




//###############################################################################################################
//
void GetVideoOutputFormat(void)
{
    Command_TypeDef cmd;

	DebugMessageRtd2796("GetVideoFormat()\r\n");
    cmd.command = 0x73;
    cmd.data_1 = 0x01;
    cmd.data_2 = 0x00;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;
	SendRs232CommandToRTD2796(cmd);
}

//###############################################################################################################
//
void GetRtd2796Version(void)
{
    Command_TypeDef cmd;
	DebugMessageRtd2796("GetRtd2796Version()\r\n");
    cmd.command = 0x6a;
    cmd.data_1 = 0x07;
    cmd.data_2 = 0x00;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;
    //send_cmd_to_video(&cmd);
	SendRs232CommandToRTD2796(cmd);
}



//###############################################################################################################
//
void GetVideoOsdDisplayMode(void)
{
    Command_TypeDef cmd;

	DebugMessageRtd2796("GetVideoFormat()\r\n");
    cmd.command = 0x73;
    cmd.data_1 = 0x01;
    cmd.data_2 = 0x00;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;
	SendRs232CommandToRTD2796(cmd);
}


//###############################################################################################################
//
u8 GetvideoInputInfo(void)
{

	char buff[30];
	vu32 ReceiveTimeOut = 0;
	u8 i;
    Command_TypeDef cmd;
	//DebugMessageRtd2796("GetvideoInputInfo()\r\n");
    cmd.command = R_Sync;
    cmd.data_1 = WR_InputSourceInfo_2;
    cmd.data_2 = 0x00;
    cmd.data_3 = 0x00;
    cmd.data_4 = 0x00;

	SetReceiveStatus(0);
	ReceiveTimeOut = GetSysTick();
	SendRs232CommandToRTD2796(cmd);
	while(!GetReceiveStatus() && !isSysTickTimeout(ReceiveTimeOut,2000))
	{
		task_2796();
	}

	if(GetReceiveStatus())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}


extern const char * inputSourceStr_List1[];
extern u8 InputSourceConv(u8 x);

#if 0
void DisplayInputInfo(void)
{
	u8 buffer[20];

	if(!menuIsCurrentPageIndex(MENU_INDEX_STATUS))
	{
		return ;
	}
	
	if( 
		GetPgmCurrentDisplayMode() == _OSD_DM_1P || 
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_LR ||
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
		GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB
	  )
	{
		if(	
			InputResolution.InputPgmWidth == 0 ||
			InputResolution.InputPgmHeight == 0 ||
			InputResolution.InputPgmFreq == 0
		  )
		{
			InputErrorFlag++;
			if(InputErrorFlag > 1)
			{
				//InputErrorFlag = 0;
				writeStr_SSD1322(0,15,"               ");
				writeStr_SSD1322(0,15,"No Input");
			}
		}
		else
		{
			if( 
				InputResolutionOld.InputPgmWidth != InputResolution.InputPgmWidth ||
				InputResolutionOld.InputPgmHeight != InputResolution.InputPgmHeight ||
				InputResolutionOld.InputPgmFreq != InputResolution.InputPgmFreq || 
				InputErrorFlag != 0 || PgmCurrentInputOld != GetPgmCurrentInput()
			  )
			{
				InputErrorFlag = 0;
				PgmCurrentInputOld = GetPgmCurrentInput();
				InputResolutionOld.InputPgmWidth = InputResolution.InputPgmWidth;
				InputResolutionOld.InputPgmHeight = InputResolution.InputPgmHeight;
				InputResolutionOld.InputPgmFreq = InputResolution.InputPgmFreq;
				writeStr_SSD1322(0,15,"               ");
				sprintf(buffer, "%dx%d@%d",InputResolution.InputPgmWidth,InputResolution.InputPgmHeight,InputResolution.InputPgmFreq/10);
				writeStr_SSD1322(0,15,buffer);
			}
		}
	}
	else if(GetPgmCurrentDisplayMode() == _OSD_DM_2P_LR || GetPgmCurrentDisplayMode() == _OSD_DM_2P_TB)
	{
		if(InputResolution.InputPgmWidth == 0 || InputResolution.InputPgmHeight == 0 || InputResolution.InputPgmFreq == 0 ||
		InputResolution.InputPrvWidth == 0 || InputResolution.InputPrvHeight == 0 || InputResolution.InputPrvFreq == 0)
		{
			InputErrorFlag++;
			if(InputErrorFlag > 1)
			{
				InputErrorFlag = 0;
				if(InputResolution.InputPgmWidth == 0 || InputResolution.InputPgmHeight == 0 || InputResolution.InputPgmFreq == 0)
				{
					writeStr_SSD1322(0,15,"               ");
					writeStr_SSD1322(0,15,"No Input");
				}
				if(InputResolution.InputPrvWidth == 0 || InputResolution.InputPrvHeight == 0 || InputResolution.InputPrvFreq == 0)
				{
					writeStr_SSD1322(1,15,"               ");
					writeStr_SSD1322(1,15,"No Input");
				}
			}
		}
		else
		{
			InputErrorFlag = 0;
			if( 
				InputResolutionOld.InputPgmWidth != InputResolution.InputPgmWidth ||
				InputResolutionOld.InputPgmHeight != InputResolution.InputPgmHeight ||
				InputResolutionOld.InputPgmFreq != InputResolution.InputPgmFreq || 
				InputErrorFlag != 0 || PgmCurrentInputOld != GetPgmCurrentInput()
			  )
			{
				PgmCurrentInputOld = GetPgmCurrentInput();
				InputResolutionOld.InputPgmWidth = InputResolution.InputPgmWidth;
				InputResolutionOld.InputPgmHeight = InputResolution.InputPgmHeight;
				InputResolutionOld.InputPgmFreq = InputResolution.InputPgmFreq;
				writeStr_SSD1322(0,15,"               ");
				if(GetPgmCurrentRegion() == _DISPLAY_RGN_LEFT || GetPgmCurrentRegion() == _DISPLAY_RGN_TOP)
				sprintf(buffer, "%dx%d@%d",InputResolution.InputPgmWidth,InputResolution.InputPgmHeight,InputResolution.InputPgmFreq/10);
				else if(GetPgmCurrentRegion() == _DISPLAY_RGN_RIGHT || GetPgmCurrentRegion() == _DISPLAY_RGN_BOTTOM)
				sprintf(buffer, "%dx%d@%d",InputResolution.InputPrvWidth,InputResolution.InputPrvHeight,InputResolution.InputPrvFreq/10);
				writeStr_SSD1322(0,15,buffer);
			}
			if( 
				InputResolutionOld.InputPrvWidth != InputResolution.InputPrvWidth ||
				InputResolutionOld.InputPrvHeight != InputResolution.InputPrvHeight ||
				InputResolutionOld.InputPrvFreq != InputResolution.InputPrvFreq || 
				InputErrorFlag != 0 || PrvCurrentInputOld != GetPrvCurrentInput()
			  )
			{
				//InputErrorFlag = 0;
				PrvCurrentInputOld = GetPrvCurrentInput();
				InputResolutionOld.InputPrvWidth = InputResolution.InputPrvWidth;
				InputResolutionOld.InputPrvHeight = InputResolution.InputPrvHeight;
				InputResolutionOld.InputPrvFreq = InputResolution.InputPrvFreq;
				writeStr_SSD1322(1,15,"               ");
				if(GetPrvCurrentRegion() == _DISPLAY_RGN_LEFT || GetPrvCurrentRegion() == _DISPLAY_RGN_TOP)
				sprintf(buffer, "%dx%d@%d",InputResolution.InputPgmWidth,InputResolution.InputPgmHeight,InputResolution.InputPgmFreq/10);
				else if(GetPrvCurrentRegion() == _DISPLAY_RGN_RIGHT || GetPrvCurrentRegion() == _DISPLAY_RGN_BOTTOM)
				sprintf(buffer, "%dx%d@%d",InputResolution.InputPrvWidth,InputResolution.InputPrvHeight,InputResolution.InputPrvFreq/10);
				writeStr_SSD1322(1,15,buffer);
			}
		}
	}
	else if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	{
		if(	InputResolution.LT_inputWidth == 0 || InputResolution.LT_inputHeight == 0 || InputResolution.LT_inputFreq == 0 ||
			InputResolution.LB_inputWidth == 0 || InputResolution.LB_inputWidth == 0 || InputResolution.LB_inputWidth == 0 ||
			InputResolution.RT_inputWidth == 0 || InputResolution.RT_inputWidth == 0 || InputResolution.RT_inputWidth == 0 ||
			InputResolution.RB_inputWidth == 0 || InputResolution.RB_inputWidth == 0 || InputResolution.RB_inputWidth == 0
		  )
		{
			InputErrorFlag++;
			if(InputErrorFlag > 1)
			{
				InputErrorFlag = 0;
				if(InputResolution.LT_inputWidth == 0 || InputResolution.LT_inputHeight == 0 || InputResolution.LT_inputFreq == 0)
				{
					InputResolutionOld.LT_inputWidth = InputResolution.LT_inputWidth;
					InputResolutionOld.LT_inputHeight = InputResolution.LT_inputHeight;
					InputResolutionOld.LT_inputFreq = InputResolution.LT_inputFreq;
					sprintf(	buffer, "%-3s %s",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource1)],\
								"No Input "
							);
					writeStr_SSD1322(0,2,buffer);
					//writeStr_SSD1322(0,2,"No Input");
				}
				if(InputResolution.RT_inputWidth == 0 || InputResolution.RT_inputWidth == 0 || InputResolution.RT_inputWidth == 0)
				{
					InputResolutionOld.RT_inputWidth = InputResolution.RT_inputWidth;
					InputResolutionOld.RT_inputHeight = InputResolution.RT_inputHeight;
					InputResolutionOld.RT_inputFreq = InputResolution.RT_inputFreq;
					sprintf(	buffer, "%-3s %s",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource2)],\
								"No Input "
							);
					writeStr_SSD1322(0,19,buffer);
					//writeStr_SSD1322(0,19,"No Input");
				}
				if(InputResolution.LB_inputWidth == 0 || InputResolution.LB_inputHeight == 0 || InputResolution.LB_inputFreq == 0)
				{
					InputResolutionOld.LB_inputWidth = InputResolution.LB_inputWidth;
					InputResolutionOld.LB_inputHeight = InputResolution.LB_inputHeight;
					InputResolutionOld.LB_inputFreq = InputResolution.LB_inputFreq;
					sprintf(	buffer, "%-3s %s",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource3)],\
								"No Input "
							);
					writeStr_SSD1322(1,2,buffer);
					//writeStr_SSD1322(1,2,"No Input");
				}
				if(InputResolution.RB_inputWidth == 0 || InputResolution.RB_inputWidth == 0 || InputResolution.RB_inputWidth == 0)
				{
					InputResolutionOld.RB_inputWidth = InputResolution.RB_inputWidth;
					InputResolutionOld.RB_inputHeight = InputResolution.RB_inputHeight;
					InputResolutionOld.RB_inputFreq = InputResolution.RB_inputFreq;
					sprintf(	buffer, "%-3s %s",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource4)],\
								"No Input "
							);
					writeStr_SSD1322(1,19,buffer);
					//writeStr_SSD1322(1,19,"No Input");
				}
			}
		}
		//if()
		{
			//InputErrorFlag = 0;
			//const char * inputSourceStr_List[]
			//u8 InputSourceConv(u8 x)
			if( 
				InputResolutionOld.LT_inputWidth != InputResolution.LT_inputWidth ||
				InputResolutionOld.LT_inputHeight != InputResolution.LT_inputHeight ||
				InputResolutionOld.LT_inputFreq != InputResolution.LT_inputFreq
		 	 )
			{
				InputResolutionOld.LT_inputWidth = InputResolution.LT_inputWidth;
				InputResolutionOld.LT_inputHeight = InputResolution.LT_inputHeight;
				InputResolutionOld.LT_inputFreq = InputResolution.LT_inputFreq;
				sprintf(		buffer, "%-3s %dx%d",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource1)],\
								InputResolution.LT_inputWidth,\
								InputResolution.LT_inputHeight\
						);
				writeStr_SSD1322(0,2,buffer);
			}
			if( 
				InputResolutionOld.RT_inputWidth != InputResolution.RT_inputWidth ||
				InputResolutionOld.RT_inputHeight != InputResolution.RT_inputHeight ||
				InputResolutionOld.RT_inputFreq != InputResolution.RT_inputFreq 
			  )
			{
				InputResolutionOld.RT_inputWidth = InputResolution.RT_inputWidth;
				InputResolutionOld.RT_inputHeight = InputResolution.RT_inputHeight;
				InputResolutionOld.RT_inputFreq = InputResolution.RT_inputFreq;
				sprintf(		buffer, "%-3s %dx%d",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource2)],\
								InputResolution.RT_inputWidth,\
								InputResolution.RT_inputHeight\
						);
				writeStr_SSD1322(0,19,buffer);
			}
			if( 
				InputResolutionOld.LB_inputWidth != InputResolution.LB_inputWidth ||
				InputResolutionOld.LB_inputHeight != InputResolution.LB_inputHeight ||
				InputResolutionOld.LB_inputFreq != InputResolution.LB_inputFreq 
		 	 )
			{
				InputResolutionOld.LB_inputWidth = InputResolution.LB_inputWidth;
				InputResolutionOld.LB_inputHeight = InputResolution.LB_inputHeight;
				InputResolutionOld.LB_inputFreq = InputResolution.LB_inputFreq;
				sprintf(		buffer, "%-3s %dx%d",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource3)],\
								InputResolution.LB_inputWidth,\
								InputResolution.LB_inputHeight\
						);
				writeStr_SSD1322(1,2,buffer);
			}
			if( 
				InputResolutionOld.RB_inputWidth != InputResolution.RB_inputWidth ||
				InputResolutionOld.RB_inputHeight != InputResolution.RB_inputHeight ||
				InputResolutionOld.RB_inputFreq != InputResolution.RB_inputFreq 
			  )
			{
				InputResolutionOld.RB_inputWidth = InputResolution.RB_inputWidth;
				InputResolutionOld.RB_inputHeight = InputResolution.RB_inputHeight;
				InputResolutionOld.RB_inputFreq = InputResolution.RB_inputFreq;
				sprintf(		buffer, "%-3s %dx%d",\
								inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource4)],\
								InputResolution.RB_inputWidth,\
								InputResolution.RB_inputHeight\
						);
				writeStr_SSD1322(1,19,buffer);
			}
		}
	}
	else{}
}	

#else
void DisplayInputInfo(void)
{
	u8 buffer[20];

	if(!menuIsCurrentPageIndex(MENU_INDEX_STATUS))
	{
		return ;
	}
	
	if( 
		GetPgmCurrentDisplayMode() == _OSD_DM_1P || 
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_LR ||
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_SACLE_TB ||
		GetPgmCurrentDisplayMode() == _OSD_DM_3P_SACLE_TB ||
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_LR ||
		GetPgmCurrentDisplayMode() == _OSD_DM_2P_TOW_PORT_TB
	  )
	{
		switch(GetPgmCurrentInput())
		{
			#ifdef _VGA_INPUT_ENABLE
			case _VGA_INPUT:
				DebugMessageRtd2796("VGA_inputStatus = %d",InputResolution.VGA_inputStatus);
				if(InputResolution.VGA_inputStatus == 0)
				{
					sprintf(buffer, "No Input");
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.VGA_inputWidth,InputResolution.VGA_inputHeight,InputResolution.VGA_inputFreq/10);
				}
			break;
			#endif
			case _DP_INPUT:
				DebugMessageRtd2796("DP_inputStatus = %d",InputResolution.DP_inputStatus);
				if(InputResolution.DP_inputStatus == 0)
				{
					sprintf(buffer, "No Input");
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.DP_inputWidth,InputResolution.DP_inputHeight,InputResolution.DP_inputFreq/10);
				}
			break;
			case _HDMI20_INPUT:
				DebugMessageRtd2796("HDMI2_inputStatus = %d",InputResolution.HDMI2_inputStatus);
				if(InputResolution.HDMI2_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.HDMI2_inputWidth,InputResolution.HDMI2_inputHeight,InputResolution.HDMI2_inputFreq/10);
				}
			break;
			case _HDMI14_INPUT:
				DebugMessageRtd2796("HDMI1_inputStatus = %d",InputResolution.HDMI1_inputStatus);
				if(InputResolution.HDMI1_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.HDMI1_inputWidth,InputResolution.HDMI1_inputHeight,InputResolution.HDMI1_inputFreq/10);
				}
			break;
			case _DVI_INPUT:
				DebugMessageRtd2796("DVI_inputStatus = %d",InputResolution.DVI_inputStatus);
				if(InputResolution.DVI_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.DVI_inputWidth,InputResolution.DVI_inputHeight,InputResolution.DVI_inputFreq/10);
				}
			break;
			default:
			break;
		}
		writeStr_SSD1322(0,15,"               ");
		writeStr_SSD1322(0,15,buffer);
	}

	#if 1
	else if(GetPgmCurrentDisplayMode() == _OSD_DM_2P_LR || GetPgmCurrentDisplayMode() == _OSD_DM_2P_TB || GetPgmCurrentDisplayMode() == _OSD_DM_2P_PIP)
	{
		switch(GetPgmCurrentInput())
		{
			#ifdef _VGA_INPUT_ENABLE
			case _VGA_INPUT:
				DebugMessageRtd2796("VGA_inputStatus = %d",InputResolution.VGA_inputStatus);
				if(InputResolution.VGA_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.VGA_inputWidth,InputResolution.VGA_inputHeight,InputResolution.VGA_inputFreq/10);
				}
			break;
			#endif
			case _DP_INPUT:
				DebugMessageRtd2796("DP_inputStatus = %d",InputResolution.DP_inputStatus);
				if(InputResolution.DP_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.DP_inputWidth,InputResolution.DP_inputHeight,InputResolution.DP_inputFreq/10);
				}
			break;
			case _HDMI20_INPUT:
				DebugMessageRtd2796("HDMI2_inputStatus = %d",InputResolution.HDMI2_inputStatus);
				if(InputResolution.HDMI2_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.HDMI2_inputWidth,InputResolution.HDMI2_inputHeight,InputResolution.HDMI2_inputFreq/10);
				}
			break;
			case _HDMI14_INPUT:
				DebugMessageRtd2796("HDMI1_inputStatus = %d",InputResolution.HDMI1_inputStatus);
				if(InputResolution.HDMI1_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.HDMI1_inputWidth,InputResolution.HDMI1_inputHeight,InputResolution.HDMI1_inputFreq/10);
				}
			break;
			case _DVI_INPUT:
				DebugMessageRtd2796("DVI_inputStatus = %d",InputResolution.DVI_inputStatus);
				if(InputResolution.DVI_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.DVI_inputWidth,InputResolution.DVI_inputHeight,InputResolution.DVI_inputFreq/10);
				}
			break;
			default:
			break;
		}
		writeStr_SSD1322(0,15,"               ");
		writeStr_SSD1322(0,15,buffer);

		switch(GetPrvCurrentInput())
		{
			#ifdef _VGA_INPUT_ENABLE
			case _VGA_INPUT:
				DebugMessageRtd2796("VGA_inputStatus = %d",InputResolution.VGA_inputStatus);
				if(InputResolution.VGA_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.VGA_inputWidth,InputResolution.VGA_inputHeight,InputResolution.VGA_inputFreq/10);
				}
			break;
			#endif
			case _DP_INPUT:
				DebugMessageRtd2796("DP_inputStatus = %d",InputResolution.DP_inputStatus);
				if(InputResolution.DP_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.DP_inputWidth,InputResolution.DP_inputHeight,InputResolution.DP_inputFreq/10);
				}
			break;
			case _HDMI20_INPUT:
				DebugMessageRtd2796("HDMI2_inputStatus = %d",InputResolution.HDMI2_inputStatus);
				if(InputResolution.HDMI2_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.HDMI2_inputWidth,InputResolution.HDMI2_inputHeight,InputResolution.HDMI2_inputFreq/10);
				}
			break;
			case _HDMI14_INPUT:
				DebugMessageRtd2796("HDMI1_inputStatus = %d",InputResolution.HDMI1_inputStatus);
				if(InputResolution.HDMI1_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.HDMI1_inputWidth,InputResolution.HDMI1_inputHeight,InputResolution.HDMI1_inputFreq/10);
				}
			break;
			case _DVI_INPUT:
				DebugMessageRtd2796("DVI_inputStatus = %d",InputResolution.DVI_inputStatus);
				if(InputResolution.DVI_inputStatus == 0)
				{
					sprintf(buffer, "No Input");	
				}
				else
				{
					sprintf(buffer, "%dx%d@%d",InputResolution.DVI_inputWidth,InputResolution.DVI_inputHeight,InputResolution.DVI_inputFreq/10);
				}
			break;
			default:
			break;
		}
		writeStr_SSD1322(1,15,"               ");
		writeStr_SSD1322(1,15,buffer);
	}
	else if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	{
		if(InputResolution.LT_inputStatus == 0)
		{
			sprintf(	buffer, "%-3s %s",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource1)],\
						"No Input "
					);
		}
		else
		{
			sprintf(	buffer, "%-3s %dx%d",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource1)],\
						InputResolution.LT_inputWidth,\
						InputResolution.LT_inputHeight\
					);
		}
		writeStr_SSD1322(0,2,buffer);

		if(InputResolution.RT_inputStatus == 0)
		{
			sprintf(	buffer, "%-3s %s",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource2)],\
						"No Input "
					);
		}
		else
		{
			sprintf(	buffer, "%-3s %dx%d",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource2)],\
						InputResolution.RT_inputWidth,\
						InputResolution.RT_inputHeight\
					);
		}
		writeStr_SSD1322(0,19,buffer);

		if(InputResolution.LB_inputStatus == 0)
		{
			sprintf(	buffer, "%-3s %s",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource3)],\
						"No Input "
					);
		}
		else
		{
			sprintf(	buffer, "%-3s %dx%d",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource3)],\
						InputResolution.LB_inputWidth,\
						InputResolution.LB_inputHeight\
					);
		}
		writeStr_SSD1322(1,2,buffer);

		if(InputResolution.RB_inputStatus == 0)
		{
			sprintf(	buffer, "%-3s %s",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource4)],\
						"No Input "
					);
		}
		else
		{
			sprintf(	buffer, "%-3s %dx%d",\
						inputSourceStr_List1[InputSourceConv(GetPanleAndSplitParamRam()->inputSource4)],\
						InputResolution.RB_inputWidth,\
						InputResolution.RB_inputHeight\
					);
		}
		writeStr_SSD1322(1,19,buffer);
	}
	else{}
	#endif
}	
#endif




void TaskInputResolution(void)
{
	
	if(menuIsCurrentPageIndex(MENU_INDEX_STATUS) == TRUE)
	{
		if(isSysTickTimeout(TimeInputInfoScan, 3000))
		{
			TimeInputInfoScan = GetSysTick();
			if(GetvideoInputInfo() == TRUE)
			{
				DisplayInputInfo();
			}
			else
			{
				
			}
		}
	}
	else
	{
	}
}



//Set Region Input Port
void SetRegionInputPort(u8 enumDisplayRegion,u8 inputPort)
{
	Command_TypeDef cmd;
	//u8 pgmInputSource,prvInputSource;
	DebugMessageRtd2796("SetRegionInputPort()\r\n");
	DebugMessageRtd2796("enumDisplayRegion = %d\r\n",enumDisplayRegion);
	DebugMessageRtd2796("Input port = %d\r\n",inputPort);
	cmd.command = W_NAME2_BACK4;
	cmd.data_1 = WR_DisplaySetting;
	cmd.data_2 = 0x08;
	cmd.data_3 = enumDisplayRegion;
	cmd.data_4 = inputPort;
	SendRs232CommandToRTD2796(cmd);	
}



void SetVideoBrightness(u16 brightness)
{
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetVideoBrightness() -> %d\r\n",brightness);
	cmd.command = 0x67;
	cmd.data_1 = 0x02;
	cmd.data_2 = 0;
	cmd.data_3 = (u8)(brightness >> 8);
	cmd.data_4 = (u8)(brightness);
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
		case _OSD_DM_4P:
			cmd.data_2 = _4P_FULL_REGION;
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_LR:
			cmd.data_2 = _PBP_LR_FULL_REGION;
		break;
		case _OSD_DM_2P_TB:
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			cmd.data_2 = _PBP_TB_FULL_REGION;
		break;
		default:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
	}
	SendRs232CommandToRTD2796(cmd);
	//if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	//{
		SysDelay1ms(20);
		cmd.data_2 = 0;
		SendRs232CommandToRTD2796(cmd);
	//}
}



void SetVideoContrast(u16 cnntrast)
{
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetVideoContrast() -> %d\r\n",cnntrast);
	cmd.command = 0x67;
	cmd.data_1 = 0x03;
	cmd.data_2 = 0;
	cmd.data_3 = (u8)(cnntrast >> 8);
	cmd.data_4 = (u8)(cnntrast);
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
		case _OSD_DM_4P:
			cmd.data_2 = _4P_FULL_REGION;
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_LR:
			cmd.data_2 = _PBP_LR_FULL_REGION;
		break;
		case _OSD_DM_2P_TB:
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			cmd.data_2 = _PBP_TB_FULL_REGION;
		break;
		default:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
	}
	SendRs232CommandToRTD2796(cmd);	
	//SendRs232CommandToRTD2796(cmd);
	//if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	//{
		SysDelay1ms(20);
		cmd.data_2 = 0;
		SendRs232CommandToRTD2796(cmd);
	//}
}




void SetVideoColorEffect(u8 effect)
{
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetVideoColorEffect() -> %d\r\n",effect);
	cmd.command = 0x67;
	cmd.data_1 = 0x04;
	cmd.data_2 = 0;
	cmd.data_3 = effect;
	cmd.data_4 = 0;
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
		case _OSD_DM_4P:
			cmd.data_2 = _4P_FULL_REGION;
			//SendRs232CommandToRTD2796(cmd);
			#if 0
			SysDelay1ms(10);
			cmd.data_2 = _4P_LB;
			SendRs232CommandToRTD2796(cmd);
			SysDelay1ms(10);
			cmd.data_2 = _4P_RT;
			SendRs232CommandToRTD2796(cmd);
			#endif
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_LR:
			cmd.data_2 = _PBP_LR_FULL_REGION;
			//SendRs232CommandToRTD2796(cmd);	
		break;
		case _OSD_DM_2P_TB:
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			//cmd.data_2 = _PBP_TB_FULL_REGION;
			cmd.data_2 = _PBP_TB_FULL_REGION;
			//SendRs232CommandToRTD2796(cmd);	
		break;
		default:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
	}
	SendRs232CommandToRTD2796(cmd);	
	SysDelay1ms(20);
	cmd.data_2 = 0;
	SendRs232CommandToRTD2796(cmd);
	#if 0
	if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	{
		SysDelay1ms(20);
		cmd.data_2 = 0;
		SendRs232CommandToRTD2796(cmd);
	}
	#endif
}



void SetVideocolorTempType(u8 temp)
{
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetVideocolorTemp() -> %d\r\n",temp);
	cmd.command = 0x67;
	cmd.data_1 = 0x00;
	cmd.data_2 = 0;
	cmd.data_3 = temp;
	cmd.data_4 = 0;
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
		case _OSD_DM_4P:
			cmd.data_2 = _4P_FULL_REGION;
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_LR:
			cmd.data_2 = _PBP_LR_FULL_REGION;
		break;
		case _OSD_DM_2P_TB:
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			cmd.data_2 = _PBP_TB_FULL_REGION;
		break;
		default:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
	}
	
	SendRs232CommandToRTD2796(cmd);	
	
	//if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	//{
		SysDelay1ms(20);
		cmd.data_2 = 0;
		SendRs232CommandToRTD2796(cmd);
	//}
	
}


void SetVideocolorTemp(u8 sel,u8 temp)
{
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetVideocolorTemp() -> %d\r\n",temp);
	cmd.command = 0x67;
	cmd.data_1 = sel;
	cmd.data_2 = 0;
	cmd.data_3 = temp & 0x7;
	cmd.data_4 = temp;
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
		case _OSD_DM_4P:
			cmd.data_2 = _4P_FULL_REGION;
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_LR:
			cmd.data_2 = _PBP_LR_FULL_REGION;
		break;
		case _OSD_DM_2P_TB:
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			cmd.data_2 = _PBP_TB_FULL_REGION;
		break;
		default:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
	}
	SendRs232CommandToRTD2796(cmd);
	//if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	//{
		SysDelay1ms(20);
		cmd.data_2 = 0;
		SendRs232CommandToRTD2796(cmd);
	//}
}

void SetVideoGamma(u8 gamma)
{
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetVideoGamma() -> %d\r\n",gamma);
	cmd.command = 0x67;
	cmd.data_1 = 0x01;
	cmd.data_2 = 0;
	cmd.data_3 = gamma;
	cmd.data_4 = 0;
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
		case _OSD_DM_4P:
			cmd.data_2 = _4P_FULL_REGION;
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_LR:
			cmd.data_2 = _PBP_LR_FULL_REGION;
		break;
		case _OSD_DM_2P_TB:
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			cmd.data_2 = _PBP_TB_FULL_REGION;
		break;
		default:
			cmd.data_2 = _1P_ROTATION_FULL_REGION;
		break;
	}
	SendRs232CommandToRTD2796(cmd);	
	//if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	//{
		SysDelay1ms(20);
		cmd.data_2 = 0;
		SendRs232CommandToRTD2796(cmd);
	//}
}

void ResetVideoColorEffect(BYTE flag)
{
	if(flag == 1)
	{
		menuSetBusy(0,11);
	}
	SetVideoBrightness(DEFAULT_BRIGHTNESS);
	SysDelay1ms(100);
	SetVideoContrast(DEFAULT_CONTRAST);
	SysDelay1ms(100);
	SetVideoGamma(DEFAULT_GAMMA);
	SysDelay1ms(100);
	SetVideocolorTempType(5);
	SysDelay1ms(100);
	SetVideoColorEffect(0);
	SysDelay1ms(100);
	#if 0
	SetVideocolorTemp(0xa0,DEFAULT_COLOR_TEMP_RED);
	SysDelay1ms(100);
	SetVideocolorTemp(0xa1,DEFAULT_COLOR_TEMP_GREEN);
	SysDelay1ms(100);
	SetVideocolorTemp(0xa2,DEFAULT_COLOR_TEMP_BLUE);
	SysDelay1ms(100);
	#endif
	if(flag == 1)
	{
		menuDisableBusy();
		LCD_WriteStr_fb2(0,22, "   OK  ");
		SysDelay1ms(1000);
	}
	
	GetSystemParam()->brightness = DEFAULT_BRIGHTNESS;
	GetSystemParam()->cnntrast = DEFAULT_CONTRAST;
	GetSystemParam()->gamma = DEFAULT_GAMMA;
	GetSystemParam()->colorEffect = 0;
	GetSystemParam()->colorTemper = 5;
	GetSystemParam()->colorTemperRed = DEFAULT_COLOR_TEMP_RED;
	GetSystemParam()->colorTemperGreen = DEFAULT_COLOR_TEMP_GREEN;
	GetSystemParam()->colorTemperBlue = DEFAULT_COLOR_TEMP_BLUE;

	setValue(colorTempRedValue,GetSystemParam()->colorTemperRed);
	setValue(colorTempGreenValue,GetSystemParam()->colorTemperGreen);
	setValue(colorTempBlueValue,GetSystemParam()->colorTemperBlue);
	
	SaveSystemParam();
	
}


void SetVideoRegionParam(EnumOSDSelectRegion4PDef enumDisplayRegion,u16 x,u16 y,u16 w,u16 h)
{
	u8 buf[10];
//	u16 xPos,yPos;
	u16 width,height;
	u16 xendPos,yendPos;
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetVideoRegionParam() -> %d\r\n",enumDisplayRegion);
	cmd.command = W_Sync;
	cmd.data_1 = WR_RegionSize;
	cmd.data_2 = 0;
	cmd.data_3 = 10;
	cmd.data_4 = 0;
	SendRs232CommandToRTD2796(cmd);	

	w = w + w%2;
	h = h + h%2;

	if(w >= 1920)
	{
		x = 0;
		width = 1920;
	}
	else
	{
		x = 1920 - w;
		width = w;
	}

	if(h >= 1080)
	{
		y = 0;
		height = 1080;
	}
	else
	{
		y = 1080 - h;
		height = h;
	}
	
	switch(enumDisplayRegion)
	{
		case _OSD_SR_4P_LT_INSIDE:
			xendPos = 1920;
			FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_LSB,(u8)w);
			FPGA_set(FPGA_REG_h_Output0Layer_A_WidthPos_MSB,(u8)(w >> 8));
			FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_LSB,(u8)h);
			FPGA_set(FPGA_REG_v_Output0Layer_A_HeightPos_MSB,(u8)(h >> 8));
			FPGA_set(FPGA_REG_h_Output0SplitActiveStartPos_LSB,(u8)x);
			FPGA_set(FPGA_REG_h_Output0SplitActiveStartPos_MSB,(u8)(x >> 8));
			FPGA_set(FPGA_REG_h_Output0SplitActiveEndPos_LSB,(u8)xendPos);
			FPGA_set(FPGA_REG_h_Output0SplitActiveEndPos_MSB,(u8)(xendPos >> 8));
			FPGA_set(FPGA_REG_V_PaneStart0Pos_LSB,(u8)y);
			FPGA_set(FPGA_REG_V_PaneStart0Pos_MSB,(u8)(y >> 8));
		break;
		case _OSD_SR_4P_LB_INSIDE:
			xendPos = 1920;
			yendPos = 1080 + height;
			y = 0;
			FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_LSB,(u8)w);
			FPGA_set(FPGA_REG_h_Output2Layer_A_WidthPos_MSB,(u8)(w >> 8));
			FPGA_set(FPGA_REG_v_Output2Layer_A_HeightPos_LSB,(u8)h);
			FPGA_set(FPGA_REG_v_Output2Layer_A_HeightPos_MSB,(u8)(h >> 8));
			FPGA_set(FPGA_REG_h_Output2SplitActiveStartPos_LSB,(u8)x);
			FPGA_set(FPGA_REG_h_Output2SplitActiveStartPos_MSB,(u8)(x >> 8));
			FPGA_set(FPGA_REG_h_Output2SplitActiveEndPos_LSB,(u8)xendPos);
			FPGA_set(FPGA_REG_h_Output2SplitActiveEndPos_MSB,(u8)(xendPos >> 8));
			FPGA_set(FPGA_REG_V_PaneEnd0Pos_LSB,(u8)yendPos);
			FPGA_set(FPGA_REG_V_PaneEnd0Pos_MSB,(u8)(yendPos >> 8));
		break;
		case _OSD_SR_4P_RT_INSIDE:
			xendPos = width;
			x = 0;
			FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_LSB,(u8)w);
			FPGA_set(FPGA_REG_h_Output1Layer_A_WidthPos_MSB,(u8)(w >> 8));
			FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_LSB,(u8)h);
			FPGA_set(FPGA_REG_v_Output1Layer_A_HeightPos_MSB,(u8)(h >> 8));
			FPGA_set(FPGA_REG_h_Output1SplitActiveStartPos_LSB,(u8)x);
			FPGA_set(FPGA_REG_h_Output1SplitActiveStartPos_MSB,(u8)(x >> 8));
			FPGA_set(FPGA_REG_h_Output1SplitActiveEndPos_LSB,(u8)xendPos);
			FPGA_set(FPGA_REG_h_Output1SplitActiveEndPos_MSB,(u8)(xendPos >> 8));
			FPGA_set(FPGA_REG_V_PaneStart1Pos_LSB,(u8)y);
			FPGA_set(FPGA_REG_V_PaneStart1Pos_MSB,(u8)(y >> 8));
		break;
		case _OSD_SR_4P_RB_INSIDE:
			xendPos = width;
			y = 0;
			x = 0;
			yendPos = 1080 + height;
			FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_LSB,(u8)w);
			FPGA_set(FPGA_REG_h_Output3Layer_A_WidthPos_MSB,(u8)(w >> 8));
			FPGA_set(FPGA_REG_v_Output3Layer_A_HeightPos_LSB,(u8)h);
			FPGA_set(FPGA_REG_v_Output3Layer_A_HeightPos_MSB,(u8)(h >> 8));
			FPGA_set(FPGA_REG_h_Output3SplitActiveStartPos_LSB,(u8)x);
			FPGA_set(FPGA_REG_h_Output3SplitActiveStartPos_MSB,(u8)(x >> 8));
			FPGA_set(FPGA_REG_h_Output3SplitActiveEndPos_LSB,(u8)xendPos);
			FPGA_set(FPGA_REG_h_Output3SplitActiveEndPos_MSB,(u8)(xendPos >> 8));
			FPGA_set(FPGA_REG_V_PaneEnd1Pos_LSB,(u8)yendPos);
			FPGA_set(FPGA_REG_V_PaneEnd1Pos_MSB,(u8)(yendPos >> 8));
		break;
		default:
		break;
	}

	buf[0] = (u8)x;
	buf[1] = (u8)(x >> 8);

	buf[2] = (u8)y;
	buf[3] = (u8)(y >> 8);

	buf[4] = (u8)width;
	buf[5] = (u8)(width >> 8);

	buf[6] = (u8)height;
	buf[7] = (u8)(height >> 8);

	buf[8] = enumDisplayRegion;

	buf[9] = GetPanelParamCheskSum(buf,9);
	UARTSendBuffer(RTD2796_UART_PORT,buf,sizeof(FourPortSplitParamSt));

	
}




void Set4PRegionSource(EnumDisplayRegion enumDisRegion,u8 source)
{
	Command_TypeDef cmd;
	FPGASetFreeze(1);
	DebugMessageRtd2796("Set4PRegionSource() -> %d\r\n",source);
	cmd.command = 0x73;
	cmd.data_1 = 0x19;
	cmd.data_2 = enumDisRegion;
	cmd.data_3 = source;
	cmd.data_4 = 0;
	SendRs232CommandToRTD2796(cmd);	
	SysDelay1ms(2000);
	FPGASetFreeze(0);
}


void SetPipModeParam(BYTE mode,BYTE MainSource,BYTE SubSource)
{
	Command_TypeDef cmd;
	DebugMessageRtd2796("SetPipMode -> ON%d\r\n");
	DebugMessageRtd2796("SetPipMode main Source -> %d\r\n",MainSource);
	DebugMessageRtd2796("SetPipMode sub Source -> %d\r\n",SubSource);
	cmd.command = 0x73;
	cmd.data_1 = 0x20;
	cmd.data_2 = 0x0d;
	cmd.data_3 = mode;
	if(mode == LAYOUT_PBP_LR)
		cmd.data_2 = 0x0e;
	else if(mode == LAYOUT_PBP_TB)
		cmd.data_2 = 0x0f;
	cmd.data_4 = ((MainSource << 4) + (SubSource & 0x0f));
	SendRs232CommandToRTD2796(cmd);	
}



void SetPipPosAndSize(EnumDisplayRegion enumDisRegion,BYTE cmdIndex,u16 value)
{
	Command_TypeDef cmd;
	
	if(gInputFlag == 0)
		return ;
	gInputFlag = 0;
	if(GetSystemParam()->PipLayout == LAYOUT_PBP_LR)
	{
		if(GetSystemParam()->CurrentWindow == 0)
		{
			cmd.data_2 = _DISPLAY_RGN_LEFT;
		}
		else
		{
			cmd.data_2 = _DISPLAY_RGN_RIGHT;
		}
	}
	else if(GetSystemParam()->PipLayout == LAYOUT_PBP_TB)
	{
		if(GetSystemParam()->CurrentWindow == 0)
		{
			cmd.data_2 = _DISPLAY_RGN_TOP;
		}
		else
		{
			cmd.data_2 = _DISPLAY_RGN_BOTTOM;
		}
	}
	else
	{
		if(GetSystemParam()->CurrentWindow == 0)
		{
			cmd.data_2 = _DISPLAY_RGN_MAIN;
		}
		else
		{
			cmd.data_2 = _DISPLAY_RGN_SUB;
		}
	}
	cmd.command = W_PIP_POS_SIZE;
	cmd.data_1 = cmdIndex;
	cmd.data_3 = (BYTE)(value & 0xff);
	cmd.data_4 = (BYTE)((value >> 8) & 0xff);
	SendRs232CommandToRTD2796(cmd);	
	SavePipParam();
}



//SET_OSD_DISPLAY_MODE
void SetVideoDisplay(u8 modex)
{
	Command_TypeDef cmd;
	u8 inputSource1,inputSource2,inputSource3,inputSource4;
	
	inputSource1 = GetPanleAndSplitParamRam()->inputSource1;
	inputSource2 = GetPanleAndSplitParamRam()->inputSource2;
	inputSource3 = GetPanleAndSplitParamRam()->inputSource3;
	inputSource4 = GetPanleAndSplitParamRam()->inputSource4;
	inputSource1 = (inputSource1 & 0xf) | (inputSource2 << 4);
	inputSource3 = (inputSource3 & 0xf) | (inputSource4 << 4);
	DebugMessageRtd2796("pgmInputSourceValue = %d\r\n",inputSource1);
	DebugMessageRtd2796("prvInputSourceValue = %d\r\n",inputSource2);
	DebugMessageRtd2796("GetvideoInputInfo()\r\n");
	cmd.command = W_NAME2_BACK4;
	cmd.data_1 = WR_DisplaySetting;
	cmd.data_2 = OsdModeConver(modex);
	cmd.data_3 = inputSource1;
	cmd.data_4 = inputSource3;
	SetPgmCurrentInput(inputSource1&0xf);
	SetPrvCurrentInput((inputSource1 >> 4)&0xf);
	PgmOldInput = GetPgmCurrentInput();
	PrvOldInput = GetPrvCurrentInput();
	SetPgmCurrentDisplayMode(modex);
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_4P:
		break;
		case _OSD_DM_2P_LR:
			SetPgmCurrentRegion(_DISPLAY_RGN_LEFT);
			SetPrvCurrentRegion(_DISPLAY_RGN_RIGHT);
		break;
		case _OSD_DM_2P_TB:
			SetPgmCurrentRegion(_DISPLAY_RGN_TOP);
			SetPrvCurrentRegion(_DISPLAY_RGN_BOTTOM);
		break;
		case _OSD_DM_2P_SACLE_LR:
		case _OSD_DM_2P_TOW_PORT_LR:
			cmd.data_3 = (inputSource1 << 4) | (inputSource1 & 0xf);
			SetPgmCurrentRegion(_DISPLAY_RGN_LEFT);
			SetPrvCurrentRegion(_DISPLAY_RGN_RIGHT);
		break;
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			cmd.data_3 = (inputSource1 << 4) | (inputSource1 & 0xf);
			SetPgmCurrentRegion(_DISPLAY_RGN_TOP);
			SetPrvCurrentRegion(_DISPLAY_RGN_BOTTOM);	
		break;
		case _OSD_DM_2P_PIP:
			cmd.data_3 = (inputSource1 << 4) | (inputSource1 & 0xf);
			SetPgmCurrentRegion(_DISPLAY_RGN_MAIN);
			SetPrvCurrentRegion(_DISPLAY_RGN_SUB);	
		break;
		default:
			SetPgmCurrentRegion(_DISPLAY_RGN_NONE);
			SetPrvCurrentRegion(_DISPLAY_RGN_NONE);
		break;
	}
	SetInputPortLedStatus(GetPgmCurrentInput(),GetPrvCurrentInput(),GetPgmCurrentDisplayMode());
	SendRs232CommandToRTD2796(cmd);	
}



