#include "UART.h"
#include "SysTick.h"
#include "SysConfig.h"
#include "control.h"
#include "userpref.h"
#include "videoFormatTable.h"
#include "it6604.h"
#include "langStr.h"
#include "SiI9134.h"
#include "rotaryEncoder.h"
#include "rs232command.h"
#include <stdio.h>
#include "fpga.h"
#include "processKey.h"
#include "edid.h"
#include "lcd.h"
#include "pipe.h"
#include "Input.h"
#include "keyScan.h"
#include "menu.h"
#include "menuData.h"
#include "sendcard.h"
#include "IDT5P49V5901.h"
#include "RTD2796.h"
#include "keyBoardStatus.h"
#include "UartDebug.h"



#define PANEL_PARAM_LEN		(sizeof(vPaneParamSt))
#define OUTPUT_PORT_PARAM_LEN	(sizeof(OutputPortLayerParamSt)*2+sizeof(OutputPorlayerAlphaSt)+sizeof(OutputPortSplitParamSt))
#define getOtherWindow()   ((CurrentWindow == MAIN_WINDOW)?PIP_WINDOW: MAIN_WINDOW)
#define CHANGE_PIP_SOURCE_DELAY  50//100
#define LocM   localm[NETIF_ETH]
#define SWITCH_VIDEO_AND_AUDIO       0  // 0 //

extern BYTE IT6604Signal;
extern int menuValueList[30];
extern const VideoDefine_Struct tVideoDefine[MD_INDEX_SIZE];
static int saveSourceDelay = 0;//5000;
static char enable_udp = 0;
static char CurrentIsEnterNumberStatus = 0xff;//getCurrentIsEnterNumberFlag();
static char PrvIsEnterNumberFlag = 0xff;
static EDIDManageStruct EDIDManageSetting = {0,0};
char sdiColorSpace[4] = {0,0,0,0};// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
char fromSetInputSourceFlag = 0;
BYTE videoProcessDeviceEnable = 1;
BYTE inputSource_old[2] = {0xFF,0xFF};
BYTE previewSource_old = 0xFF;
BYTE pipMode_old; 
u8 usbconnectflg =0;	//added by @cc20140722
int enableMBSourceCnt = 60000;

bool hotBackSignalStatus = true;

// 值对应4604的输入的序号	
char inputTableForMst[VIPDRIVE_INPUT_SIZE] = 
{
	VIPDRIVE_INPUT_SIZE,  		// VIPDRIVE_INPUT_VGA,					 					
	VIPDRIVE_INPUT_SIZE, 		// VIPDRIVE_INPUT_DP,

	1,                  		// VIPDRIVE_INPUT_DVI,	
	0,	 						//  VIPDRIVE_INPUT_HDMI
	2,							// VIPDRIVE_INPUT_SDI
	VIPDRIVE_INPUT_SIZE,        // VIPDRIVE_INPUT_EXT_7,					
	VIPDRIVE_INPUT_SIZE,        // VIPDRIVE_INPUT_EXT_8,					
	VIPDRIVE_INPUT_SIZE,        //VIPDRIVE_INPUT_EXT_9,					
};



#if defined(LING_XIN)

	 #define KEY_NUMBER_1	KEY_HDMI1
	 #define KEY_NUMBER_2	KEY_HDMI2
	 #define KEY_NUMBER_3	KEY_DVI
	 #define KEY_NUMBER_4	KEY_M1
	 #define KEY_NUMBER_5	KEY_M2
	 #define KEY_NUMBER_6	KEY_LOAD
	 #define KEY_NUMBER_7	KEY_TAKE
	 #define KEY_NUMBER_8	KEY_M4
	 #define KEY_NUMBER_9	KEY_M3
	 #define KEY_NUMBER_0	KEY_SPLIT

#else
	  #define KEY_NUMBER_1	KEY_M1
	  #define KEY_NUMBER_2	KEY_M2
	  #define KEY_NUMBER_3	KEY_M3
	  #define KEY_NUMBER_4	KEY_M4
	  #define KEY_NUMBER_5	KEY_LAYER_E
	  #define KEY_NUMBER_6	KEY_LAYER_F
	  #define KEY_NUMBER_7	KEY_LAYER_G
	  #define KEY_NUMBER_8	KEY_LAYER_H
	  #define KEY_NUMBER_9	KEY_OUTPUT3
	  #define KEY_NUMBER_0	KEY_OUTPUT4
#endif


void InitDeSetting(void);
void saveScaleAndCropParamCh(int source);
void loadScaleAndCropParamCh(int source);
void loadCropParamCh(int source);
void loadCropParamAndSetPreCh(int source,int ch);
void setInterlaceMonitor_delay(int value);
void setInterlaceMonitor_sysTime(int value);
char getVideoSourceInput(char source);
bool getDelayForTransEnable(void);
void editEDID(BYTE outputFormat,BYTE inputPort);
char getDVIPreviewSourceInput(void);
char getPreviewSourceInput(void);

u8 freezeStatus = 0;



void TaskControl(void)
{
	task_led_control();
	task_HotBackup();
}




#if defined(LING_XIN)

u16 NumberKeyValueSwitch(u8 i)
{
	switch(i)
	{
		case 0:
			return KEY_SPLIT;
		case 1:
			return KEY_HDMI1;
		case 2:
			return KEY_HDMI2;
		case 3:
			return KEY_DVI;
		case 4:
			return KEY_M1;
		case 5:
			return KEY_M2;
		case 6:
			return KEY_LOAD;
		case 7:
			return KEY_TAKE;
		case 8:
			return KEY_M3;
		case 9:
			return KEY_M4;
		default:
			return KEY_DVI;
	}
}

#else
u16 NumberKeyValueSwitch(u8 i)
{
	switch(i)
	{
		case 0:
			return KEY_M1;
		case 1:
			return KEY_M2;
		case 2:
			return KEY_M3;
		case 3:
			return KEY_M4;
		case 4:
			return KEY_LAYER_E;
		case 5:
			return KEY_LAYER_F;
		case 6:
			return KEY_LAYER_G;
		case 7:
			return KEY_LAYER_H;
		case 8:
			return KEY_OUTPUT3;
		case 9:
			return KEY_OUTPUT4;

		default:
			return KEY_M1;
	}
}

#endif



void SetCurrentEnterNumberStatus(char status)
{
	CurrentIsEnterNumberStatus = status;
}

char GetCurrentEnterNumberStatus(void)
{
	return CurrentIsEnterNumberStatus;
}

void SetPrvEnterNumberStatus(char status)
{
	PrvIsEnterNumberFlag = status;
}

char GetPrvEnterNumberStatus(void)
{
	return PrvIsEnterNumberFlag;
}
	
void task_led_control(void)
{
	BYTE i;
	SetCurrentEnterNumberStatus(getCurrentIsEnterNumberFlag());
	if(menuIsCurrentPageIndex(MENU_INDEX_PIP))
		return ;
	if(GetPrvEnterNumberStatus() != GetCurrentEnterNumberStatus())
	{
		SetPrvEnterNumberStatus(GetCurrentEnterNumberStatus());
		if(GetCurrentEnterNumberStatus())	
		{
			SetKeyLedMode(KEY_NUMBER_1,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_2,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_3,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_4,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_5,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_6,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_7,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_8,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_9,LED_ON_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_0,LED_ON_ALWAYS);
			if(menuIsCurrentPageIndex(MENU_INDEX_SAVE_TO))
			{
				for(i = KEY_MENU;i < KEY_SIZE;i++)
				{
					SetKeyLedMode(i,LED_OFF_ALWAYS);
				}
				SetKeyLedMode(KEY_NUMBER_1,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_2,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_3,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_4,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_5,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_6,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_7,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_8,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_9,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_0,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_SAVE,LED_ON_ALWAYS);
				
				for(i = 0;i < USER_SAVE_MAX_SIZE;i++)
				{
					if(isUserSettingUsed_ram((UserSaveIndex_TypeDef)i)) //usersetting is not empty
					{
						//printf("record %d is not empty\r\n",i);
						SetKeyLedMode(NumberKeyValueSwitch(i),LED_BLINK_100MS);
					}
				}
			}
			else if(menuIsCurrentPageIndex(MENU_INDEX_LOAD_FROM))
			{
				for(i = KEY_MENU;i < KEY_SIZE;i++)
				{
					SetKeyLedMode(i,LED_OFF_ALWAYS);
				}
				SetKeyLedMode(KEY_NUMBER_1,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_2,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_3,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_4,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_5,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_6,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_7,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_8,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_9,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_NUMBER_0,LED_ON_ALWAYS);
				SetKeyLedMode(KEY_LOAD,LED_ON_ALWAYS);
				for(i = 0;i < USER_SAVE_MAX_SIZE;i++)
				{
					if(isUserSettingUsed_ram((UserSaveIndex_TypeDef)i)) //usersetting is not empty
					{
						//printf("record %d is not empty\r\n",i);
						SetKeyLedMode(NumberKeyValueSwitch(i),LED_ON_ALWAYS);
					}
					else
					{
						SetKeyLedMode(NumberKeyValueSwitch(i),LED_OFF_ALWAYS);
					}
				}
				if(getCurrentRecordIndex() < USER_SAVE_MAX_SIZE)
					SetKeyLedMode(NumberKeyValueSwitch(getCurrentRecordIndex()),LED_BLINK_100MS);
			}
		}
		else
		{
			SetKeyLedMode(KEY_NUMBER_1,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_2,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_3,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_4,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_5,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_6,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_7,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_8,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_9,LED_OFF_ALWAYS);
			SetKeyLedMode(KEY_NUMBER_0,LED_OFF_ALWAYS);
			SetInputPortLedStatus(GetPgmCurrentInput(),GetPrvCurrentInput(),GetPgmCurrentDisplayMode());
		}
	}

	if(!getCurrentIsEnterNumberFlag())
	{
		switch(GetPgmCurrentDisplayMode())
		{
			case _OSD_DM_1P:
			case _OSD_DM_4P:
			case _OSD_DM_2P_SACLE_LR:
			case _OSD_DM_2P_SACLE_TB:
			case _OSD_DM_3P_SACLE_TB:
			case _OSD_DM_2P_TOW_PORT_TB:
			case _OSD_DM_2P_TOW_PORT_LR:
				//SetKeyLedMode(KEY_OUTPUT1,LED_ON_ALWAYS);
				//SetKeyLedMode(KEY_OUTPUT2,LED_ON_ALWAYS);
				//SetKeyLedMode(KEY_OUTPUT3,LED_ON_ALWAYS);
				//SetKeyLedMode(KEY_OUTPUT4,LED_ON_ALWAYS);
			break;
			case _OSD_DM_2P_PIP:
				if(GetSystemParam()->CurrentWindow == IMAGE_A)
				{
					if(GetPgmCurrentInput() == GetPrvCurrentInput())
					{
						SetKeyLedMode(InputSourceToKey(GetPgmCurrentInput()),LED_BLINK_100MS);
					}
					else
					{
						SetKeyLedMode(InputSourceToKey(GetPgmCurrentInput()),LED_ON_ALWAYS);
						SetKeyLedMode(InputSourceToKey(GetPrvCurrentInput()),LED_BLINK_100MS);
					}
				}
				else if(GetSystemParam()->CurrentWindow == IMAGE_B)
				{
					if(GetPgmCurrentInput() == GetPrvCurrentInput())
					{
						SetKeyLedMode(InputSourceToKey(GetPgmCurrentInput()),LED_BLINK_100MS);
					}
					else
					{
						SetKeyLedMode(InputSourceToKey(GetPrvCurrentInput()),LED_BLINK_100MS);
						SetKeyLedMode(InputSourceToKey(GetPgmCurrentInput()),LED_ON_ALWAYS);
					}
				}
			break;
			default:
				//SetKeyLedMode(KEY_OUTPUT1,LED_ON_ALWAYS);
				//SetKeyLedMode(KEY_OUTPUT2,LED_ON_ALWAYS);
				//SetKeyLedMode(KEY_OUTPUT3,LED_BLINK_100MS);
				//SetKeyLedMode(KEY_OUTPUT4,LED_BLINK_100MS);
			break;
		}
	}
	
}


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



BYTE GetOutputFormatIndex(void)
{
	return GetUserSettingParam()->userSplitParam.outputFormatIndex;
}


void ResetFactory(void)
{
	getUserLang();	
    resetUserPref();
	SaveUserPref();  // 要保存
	ResetVideoColorEffect(0);
	resetEDID(hdmi20PortHdmi20EdidValue);
	resetEDID(hdmi14PortHdmi14EdidValue);
	resetEDID(dpPort4k2k60Value);
	resetEDID(dviPortDviEdidValue);
	resetUserSettingParam();
	SaveCurrentUserSettingParam();
	//SavePanleAndSplitParam();
	SysDelay1ms(100);
	SaveOutputTypeParam();
	SysDelay1ms(100);
	SaveSystemParam();
	SysDelay1ms(5000);
}


void SetLanguage(int value)
{
	lang_SetLang((Lang_TypeDef) (value&0x01));
	saveUserLang(value&0x01);
}

int GetLanguage(void)
{
	return lang_GetLang();
}


int get_enable_udp(void)
{
	return enable_udp;
}

void set_enable_udp(int enable)
{
	enable_udp = enable;
}



int ScaleOffsetX = 0;
int ScaleOffsetY = 0;

bool GetSignalStatus(void)
{
	return hotBackSignalStatus;
}

void SetSignalStatus(bool status)
{
	hotBackSignalStatus = status;
}

BYTE getHotBackupEnable(void)
{
	return GetSystemParam()->hotBsckup.hotBackupEn;
}


void setHotBackupEnable(BYTE enable)
{
	GetSystemParam()->hotBsckup.hotBackupEn = enable&0x01;
	
	resetHotBackupEnable();
	
	if(enable)  //每次打开热备份,初始化全部热备份组成员都是当前信号
	{
		int i;
		GetSystemParam()->hotBsckup.hotBackupIndex = 0;
		#if 0
		for(i = 0; i < sizeof(GetSystemParam()->hotBsckup.hotBackupDetail); i++)
		{
			GetSystemParam()->hotBsckup.hotBackupDetail[i] = GetPgmCurrentInput();
			setHotBackupDisable(GetPgmCurrentInput());
		}
		#endif
	}
	
	SaveSystemParam();	
}


BYTE getHotBackupDetials(BYTE cnt)
{
	if(cnt > HOT_BACK_NUMBERS)
	{
		cnt = HOT_BACK_NUMBERS;
	}
	return GetSystemParam()->hotBsckup.hotBackupDetail[cnt];
}

void setHotBackupDetials(BYTE cnt,BYTE source)
{
	int i;

	#if 0
	if((!getHotBackupEnable())||(cnt > HOT_BACK_NUMBERS))
	{
		return;
	}
	#endif
	if(cnt > HOT_BACK_NUMBERS)
	{
		return ;
	}
	
	GetSystemParam()->hotBsckup.hotBackupIndex = 0;
	for(i = 0; i < cnt; i++)
	{
		if(GetSystemParam()->hotBsckup.hotBackupDetail[i] == source)
		{
			return;
		}
	}
	for(i = cnt; i < sizeof(GetSystemParam()->hotBsckup.hotBackupDetail); i++)
	{
		GetSystemParam()->hotBsckup.hotBackupDetail[i] = source;
	}

	refleshHotBackupEnable();

	SaveSystemParam();
}

void task_HotBackup(void)
{
	BYTE index = GetSystemParam()->hotBsckup.hotBackupIndex;
	
	if(!getHotBackupEnable())
	{
		return;
	}

	if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	{
		return ;
	}
	if(GetSignalStatus() == true)
	{
		//DebugMessageControl("stable signal \r\n");
		return;
	}

	//if(GetSystemParam()->hotBsckup.hotBackupDetail[index] == GetSystemParam()->hotBsckup.hotBackupDetail[index+1])
	//{
	//	return ;
	//}
	
	SetSignalStatus(true);
	
	if(index > HOT_BACK_NUMBERS)
	{
		//DebugMessageControl("no hot backup signal\r\n");
		return;
	}
	if(GetPgmCurrentInput() == InputSourceConv1(GetSystemParam()->hotBsckup.hotBackupDetail[index]))
	{
		index++;
	}
	DebugMessageControl("change to signal_%s\r\n",valueToString(inputSourceValue,GetSystemParam()->hotBsckup.hotBackupDetail[index]));
	if(GetPgmCurrentDisplayMode() == _OSD_DM_2P_LR || GetPgmCurrentDisplayMode() == _OSD_DM_2P_TB)
	{
		SetVideoInputSource(0,0,InputSourceConv1(GetSystemParam()->hotBsckup.hotBackupDetail[index]),0,0);
	}
	else
	{
		SetVideoInputSource(0,InputSourceConv1(GetSystemParam()->hotBsckup.hotBackupDetail[index]),0,0,0);
	}
	GetSystemParam()->hotBsckup.hotBackupIndex++;
	menuProcessEvent(MENU_KEY_INFO);	
}


//static int inputFormat = 0;

typedef enum
{
	TYPE_CROP_WIDTH_DOT = 0,
	TYPE_CROP_HEIGHT_DOT,
	TYPE_CROP_X_DOT,
	TYPE_CROP_Y_DOT,
	
}CROP_INDEX_TYPE;





void SetHDMIoutEnableDataRange(int hdmix, bool enable)
{
	if((SiI9134_PORT_1<=hdmix) && (hdmix <= SiI9134_PORT_5))
	{
		SiI9134_SetDataRange((SiI9134PortTypeDef) hdmix, enable);
		
		if(hdmix < 4){GetUserPref()->deSetting[hdmix].enableDataRange = enable;}
	}
}





bool GetHDMIoutEnableDataRange(int hdmix)
{
	return (bool) GetUserPref()->deSetting[hdmix&0x03].enableDataRange;
}



void SetHDMIoutBitWidth(int hdmix, int bitWidth) // bitWidth: 0 -- 8bit, 1 -- 10bit, 2 -- 12bit
{
	if((SiI9134_PORT_1<=hdmix) && (hdmix <= SiI9134_PORT_5))
	{
		if(bitWidth == 0)
		{
			SiI9134_SetOutput8Bits((SiI9134PortTypeDef) hdmix);
		}
		else if(bitWidth == 1)
		{
			SiI9134_SetOutput10Bits((SiI9134PortTypeDef) hdmix);
		}
		else if(bitWidth == 2)
		{
			SiI9134_SetOutput12Bits((SiI9134PortTypeDef) hdmix);
		}
		
		if(hdmix < 4)
		{
			if(bitWidth > 0)
			{
				GetUserPref()->deSetting[hdmix].enableHDMI = 1;
			}
			GetUserPref()->deSetting[hdmix].bitWidth = bitWidth;
		}
	}
}



int GetHDMIoutBitWidth(int hdmix)
{
	if(hdmix < 4)
	{
		return GetUserPref()->deSetting[hdmix].bitWidth;
	}
	return 0;
}




int getSaveSourceDelay(void)
{
	return saveSourceDelay;
}




void LoadSave(int index)
{
	PanelSplitParamSt *fpgaSplitParam;
	setCurrentRecordIndex(index);
	if(!getUserMode((UserSaveIndex_TypeDef) index))
		return ;
	
	FPGASetFreeze(1);
	#if 0
	if( GetPgmCurrentDisplayMode() != GetUserSettingSave()->userSplitParam.splitMode ||\
		GetSystemParam()->PipLayout != GetUserSettingSave()->stSystemParam.PipLayout
	  )
	#else
	if(memcmp((u8*)&_userSettingParam,(u8*)&userMode,sizeof(UserSettingParam_Typedef)))
	#endif
	{
		menuSetBusy(0,6);
		memcpy((u8*)&_userSettingParam,(u8*)&userMode,sizeof(UserSettingParam_Typedef));
		switch(GetUserSettingSave()->userSplitParam.splitMode)
		{
			case _OSD_DM_1P:
				setValue(FieldSplitParamSendValue,-1);
			break;
			case _OSD_DM_4P:
				RTD2796Reset();
				SysDelay1ms(3000);
				setValue(mode4PSplitParamSendValue,-1);
			break;
		    case _OSD_DM_2P_LR:
				setValue(Vertical2SplitParamSendValue,-1);
			break;
		    case _OSD_DM_2P_TB:
				setValue(Horizontal2SplitParamSendValue,-1);
			break;
		    case _OSD_DM_2P_SACLE_TB:
				setValue(Horizontal4SplitParamSendValue,-1);
			break;
		    case _OSD_DM_3P_SACLE_TB:
				setValue(Horizontal3SplitParamSendValue,-1);
			break;
		    case _OSD_DM_2P_TOW_PORT_TB:
				setValue(Horl2SplitParamSendValue,-1);
			break;
		    case _OSD_DM_2P_TOW_PORT_LR:
				setValue(Verl2SplitParamSendValue,-1);
			break;
			case _OSD_DM_2P_PIP:
				fpgaSplitParam = LoadFpgaSplitFromFlash(index+1);
				SetSplitParamToFpga(fpgaSplitParam);
				SetPgmCurrentDisplayMode(_OSD_DM_2P_PIP);
				SetPgmCurrentInput(GetPanleAndSplitParamRam()->inputSource1);
				SetPrvCurrentInput(GetPanleAndSplitParamRam()->inputSource2);
				memcpy((u8*)&stPipWin,(u8*)GetPipParam(),sizeof(PipWinSizeSt));
				SetPipWindowStart(&stPipWin);
				UserPipParamSetting(&stPipWin);
				SetPipModeParam(GetSystemParam()->PipLayout,GetPgmCurrentInput(),GetPrvCurrentInput());
				GetSystemParam()->Osd4pSwitch = 0;
				GetSystemParam()->PipModeSwitch = 1;//
		    case _OSD_DM_2P_SACLE_LR:
			default:
			break;
		}
		SaveCurrentUserSettingParam();
		SaveFpgaSplitParamToFlash(0);
		SysDelay1ms(100);
		LoadCurrentUserSettingParam();
		SysDelay1ms(6000);
		menuDisableBusy();
		FPGASetFreeze(0);
		
	}
	else
	{
		DebugMessageControl("Param is no different !\r\n");
		menuDisableBusy();
		FPGASetFreeze(0);
		return ;
	}

#if 0
	if(
			GetUserSettingSave()->userSplitParam.inputSource1 == GetUserSettingParam()->userSplitParam.inputSource1 &&\
			GetUserSettingSave()->userSplitParam.inputSource2 == GetUserSettingParam()->userSplitParam.inputSource2 &&\
			GetUserSettingSave()->userSplitParam.inputSource3 == GetUserSettingParam()->userSplitParam.inputSource3 &&\
			GetUserSettingSave()->userSplitParam.inputSource4 == GetUserSettingParam()->userSplitParam.inputSource4
		)
	{
		menuDisableBusy();
		FPGASetFreeze(0);
		return ;
	}
	else
	{
		DebugMessageControl("Input source is no change !\r\n");
	}

	
	if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
	{
		SetVideoInputSource(
								_DISPLAY_RGN_LT,
								GetUserSettingSave()->userSplitParam.inputSource1,
								GetUserSettingSave()->userSplitParam.inputSource1,
								GetUserSettingSave()->userSplitParam.inputSource1,
								GetUserSettingSave()->userSplitParam.inputSource1
							);
		SetVideoInputSource(
								_DISPLAY_RGN_LB,
								GetUserSettingSave()->userSplitParam.inputSource3,
								GetUserSettingSave()->userSplitParam.inputSource3,
								GetUserSettingSave()->userSplitParam.inputSource3,
								GetUserSettingSave()->userSplitParam.inputSource3
							);
		SetVideoInputSource(
								_DISPLAY_RGN_RT,
								GetUserSettingSave()->userSplitParam.inputSource2,
								GetUserSettingSave()->userSplitParam.inputSource2,
								GetUserSettingSave()->userSplitParam.inputSource2,
								GetUserSettingSave()->userSplitParam.inputSource2
							);
		SetVideoInputSource(
								_DISPLAY_RGN_RB,
								GetUserSettingSave()->userSplitParam.inputSource4,
								GetUserSettingSave()->userSplitParam.inputSource4,
								GetUserSettingSave()->userSplitParam.inputSource4,
								GetUserSettingSave()->userSplitParam.inputSource4
							);
	}
	else
	{
		SetVideoInputSource(
								_DISPLAY_RGN_NONE,
								GetUserSettingSave()->userSplitParam.inputSource1,
								GetUserSettingSave()->userSplitParam.inputSource2,
								GetUserSettingSave()->userSplitParam.inputSource3,
								GetUserSettingSave()->userSplitParam.inputSource4
							);
	}
	SetInputPortLedStatus(GetPgmCurrentInput(),GetPrvCurrentInput(),GetPgmCurrentDisplayMode());
	menuDisableBusy();
	FPGASetFreeze(0);
#endif
	
}



#define FPGA_DEBUG_FORMAT

void SetFpgaOutputFormat(BYTE format,u8 index)
{

	//1,给FPGA发送分辨率参数
	//2,让5901产生相应的时钟给FPGA
	//VideoDefine_Struct cusVideoDefine;
	u8 buf[16];
	u8 i;
	u16 dd;
	u16 HsActive,HsTotal;
	u16 HSyncStart,HSyncEnd;
	u16 VsActive,VsTotal;
	u16 VSyncStart,VSyncEnd;
	u16 FirstDisplayW,FirstDisplayH;

	u16 hsBp = tVideoDefineCustom.hstart - tVideoDefineCustom.hSyncWidth;
	u16 vsBp = tVideoDefineCustom.vstart - tVideoDefineCustom.vSyncWidth;

	FpgaOutputPortSel(	GetPanleAndSplitParamRam()->outputPort1Map,
						GetPanleAndSplitParamRam()->outputPort2Map,
						GetPanleAndSplitParamRam()->outputPort3Map,
						GetPanleAndSplitParamRam()->outputPort4Map
					);
	
#ifndef FPGA_DEBUG_FORMAT
	FPGA_Program_Start();
	SysDelay1ms(100);
	FPGA_Program_Done();
#endif
	
	switch(format)
	{
		case 3:
		case 11:
		case 16:
			menuSetBusy(0,10);
		break;
		case 7:
		case 12:
		case 17:
			menuSetBusy(1,10);
		break;
		case 9:
		case 13:
		case 18:
			menuSetBusy(2,10);
		break;
		case 10:
		case 15:
			menuSetBusy(3,10);
		break;
		default:
		break;
	}
	
	#ifndef FPGA_DEBUG_FORMAT
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	#endif

	HsActive = tVideoDefineCustom.hActive;				//有效
	HsTotal = tVideoDefineCustom.htotal;				//总total
	HSyncStart = HsActive + hsBp;						//开始 = 有效 +后肩 88 + 800
	HSyncEnd = HsActive + tVideoDefineCustom.hstart;	//结束 = 有效 +后肩 + 同步宽度 88 + 800 + 128 

	VsActive = tVideoDefineCustom.vActive;				//有效
	VsTotal = tVideoDefineCustom.vtotal;				//总total
	VSyncStart = VsActive + vsBp;						//tVideoDefine[format].vstart;	//开始
	VSyncEnd = VsActive + tVideoDefineCustom.vstart;	//结束
	//fReq = tVideoDefineCustom.vfreq;	//帧频
//	pClk = tVideoDefineCustom.pClk;	//时钟

	buf[0] = (u8)(HsActive & 0xff);
	buf[1] = (u8)((HsActive >> 8) & 0xff);
	buf[2] = (u8)(HsTotal & 0xff);
	buf[3] = (u8)((HsTotal >> 8) & 0xff);
	buf[4] = (u8)(HSyncStart & 0xff);
	buf[5] = (u8)((HSyncStart >> 8) & 0xff);
	buf[6] = (u8)(HSyncEnd & 0xff);
	buf[7] = (u8)((HSyncEnd >> 8) & 0xff);
	
	buf[8] = (u8)(VsActive & 0xff);
	buf[9] = (u8)((VsActive >> 8) & 0xff);
	buf[10] = (u8)(VsTotal & 0xff);
	buf[11] = (u8)((VsTotal >> 8) & 0xff);
	buf[12] = (u8)(VSyncStart & 0xff);
	buf[13] = (u8)((VSyncStart >> 8) & 0xff);
	buf[14] = (u8)(VSyncEnd & 0xff);
	buf[15] = (u8)((VSyncEnd >> 8) & 0xff);

	DebugMessageControl("Set output format..\r\n");
	
	for(i = 0;i < 16;i++)	
	{
		FPGA_set(i+FPGA_REG_Format_h_Active_LSB,buf[i]);
		if((i+1)%2 == 0)
		{
			dd = buf[i];
			dd = dd << 8 | buf[i-1];
			DebugMessageControl("FPGA REG = %x Value = %d\r\n",i+FPGA_REG_Format_h_Active_LSB,dd);
		}
	}

	FirstDisplayW = HsActive;
	FirstDisplayH = VsActive;
	
	setValue(totalWidthValue,HsActive*2);
	setValue(totalHeighValue,VsActive*2);
	setValue(firstDisWidthValue,FirstDisplayW);
	setValue(firstDisHeighValue,FirstDisplayH);

	setValue(h12TotalWidthValue,HsActive*2);
	setValue(h12TotalHeighValue,VsActive);
	setValue(h12FirstDisWidthValue,FirstDisplayW);
	setValue(h12FirstDisHeighValue,FirstDisplayH);

	setValue(v12TotalWidthValue,HsActive);
	setValue(v12TotalHeighValue,VsActive*2);
	setValue(v12FirstDisWidthValue,FirstDisplayW);
	setValue(v12FirstDisHeighValue,FirstDisplayH);

	setValue(h14TotalWidthValue,HsActive*4);
	setValue(h14TotalHeighValue,VsActive);
	setValue(h14FirstDisWidthValue,FirstDisplayW);
	setValue(h14SecondDisWidthValue,FirstDisplayW);
	setValue(h14ThirdDisWidthValue,FirstDisplayW);

	setValue(h13TotalWidthValue,HsActive*3);
	setValue(h13TotalHeighValue,VsActive);
	setValue(h13FirstDisWidthValue,FirstDisplayW);
	setValue(h13SecondDisWidthValue,FirstDisplayW);
	//setValue(h13ThirdDisWidthValue,FirstDisplayW);

	setValue(hor12TotalWidthValue,HsActive*2);
	setValue(hor12TotalHeighValue,VsActive);
	setValue(hor12FirstDisWidthValue,FirstDisplayW);
	//setValue(h14SecondDisWidthValue,FirstDisplayW);
	//setValue(h14ThirdDisWidthValue,FirstDisplayW);

	setValue(ver12TotalWidthValue,HsActive);
	setValue(ver12TotalHeighValue,VsActive*2);
	setValue(ver12FirstDisHeighValue,FirstDisplayW);
	//setValue(h14SecondDisWidthValue,FirstDisplayW);
	//setValue(h14ThirdDisWidthValue,FirstDisplayW);

	setValue(mode4PTotalWidthValue,HsActive);
	setValue(mode4PTotalHeighValue,VsActive);
	setValue(mode4PFirstDisWidthValue,FirstDisplayW);
	setValue(mode4PFirstDisHeighValue,FirstDisplayH);
	setValue(mode4PSecondDisWidthValue,FirstDisplayW);
	setValue(mode4PSecondDisHeighValue,FirstDisplayH);
	setValue(mode4PThirdDisWidthValue,FirstDisplayW);
	setValue(mode4PThirdDisHeighValue,FirstDisplayH);
	setValue(mode4PFourDisWidthValue,FirstDisplayW);
	setValue(mode4PFourDisHeighValue,FirstDisplayH);
    
	switch(GetPgmCurrentDisplayMode())
	{
		case _OSD_DM_1P:
		case _OSD_DM_2P_PIP:
			FieldSplitSetting(&(GetUserSettingParam()->userSplitParam));
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case _OSD_DM_4P:
			IndependentOutputSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_4P);
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case _OSD_DM_2P_LR:
			TwoPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_LR);
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case _OSD_DM_2P_TB:
			TwoPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_TB);
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		#if 0
		case _OSD_DM_2P_PIP:
			FieldSplitSetting(&(GetUserSettingParam()->userSplitParam));
			
			GetSystemParam()->PipLayout = LAYOUT_PIP_LT;
			GetSystemParam()->CurrentWindow = 1;
			CalcPipLayoutSize(GetSystemParam()->PipLayout,GetPipParam());
			SetPgmCurrentDisplayMode(_OSD_DM_2P_PIP);
			GetPanleAndSplitParamRam()->splitMode = _OSD_DM_2P_PIP;
			SetPrvCurrentInput(GetPgmCurrentInput());
			UserPipParamSetting(GetPipParam());
			SetPipModeParam(GetSystemParam()->PipLayout,GetPgmCurrentInput(),GetPgmCurrentInput());
		break;
		#endif
		case _OSD_DM_2P_SACLE_TB:
			FourPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_SACLE_TB);
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case _OSD_DM_2P_TOW_PORT_TB:
			Horizontal2SplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_TOW_PORT_TB);
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case _OSD_DM_2P_TOW_PORT_LR:
			Vertical2SplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_2P_TOW_PORT_LR);
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		case _OSD_DM_3P_SACLE_TB:
			ThirdPortSplitSetting(&(GetUserSettingParam()->userSplitParam),_OSD_DM_3P_SACLE_TB);
			GetUserSettingParam()->userSplitParam.splitMode = GetPgmCurrentDisplayMode();
			SavePanleAndSplitParam();
			SysDelay1ms(100);
			LoadPanleAndSplitParam();
		break;
		default:
		break;
		
	}

	menuDisableBusy();
	switch(format)
	{
		case 3:
		case 11:
		case 16:
			LCD_WriteStr_fb2(0,20, "   OK  ");
		break;
		case 7:
		case 12:
		case 17:
			LCD_WriteStr_fb2(1,20, "   OK  ");
		break;
		case 9:
		case 13:
			LCD_WriteStr_fb2(2,20, "   OK  ");
		break;
		case 10:
		case 15:
			LCD_WriteStr_fb2(3,20, "   OK  ");
		break;
		default:
		break;
	}
	
	SysDelay1ms(1000);
	
}


void SetOutputFormat(BYTE format)
{
	//1,给FPGA发送分辨率参数
	//2,让5901产生相应的时钟给FPGA
	u8 buf[16];
	u8 i;
	u16 dd;
	u16 HsActive,HsTotal;
	u16 HSyncStart,HSyncEnd;
	u16 VsActive,VsTotal;
	u16 VSyncStart,VSyncEnd;
//	u16 fReq;
	u64 pClk = 0;

	#if 1
	u16 hsBp = tVideoDefineCustom.hstart - tVideoDefineCustom.hSyncWidth;
	u16 vsBp = tVideoDefineCustom.vstart - tVideoDefineCustom.vSyncWidth;

	HsActive = tVideoDefineCustom.hActive;					//有效
	HsTotal = tVideoDefineCustom.htotal;					//总total
	HSyncStart = HsActive + hsBp;							//开始 = 有效 +后肩 88 + 800
	HSyncEnd = HsActive + tVideoDefineCustom.hstart;		//结束 = 有效 +后肩 + 同步宽度 88 + 800 + 128 

	VsActive = tVideoDefineCustom.vActive;					//有效
	VsTotal = tVideoDefineCustom.vtotal;					//总total
	VSyncStart = VsActive + vsBp;							//tVideoDefine[format].vstart;	//开始
	VSyncEnd = VsActive + tVideoDefineCustom.vstart;		//结束
	//fReq = tVideoDefineCustom.vfreq;	//帧频
	//pClk = tVideoDefineCustom.pClk;	//时钟

	buf[0] = (u8)(HsActive & 0xff);
	buf[1] = (u8)((HsActive >> 8) & 0xff);
	buf[2] = (u8)(HsTotal & 0xff);
	buf[3] = (u8)((HsTotal >> 8) & 0xff);
	buf[4] = (u8)(HSyncStart & 0xff);
	buf[5] = (u8)((HSyncStart >> 8) & 0xff);
	buf[6] = (u8)(HSyncEnd & 0xff);
	buf[7] = (u8)((HSyncEnd >> 8) & 0xff);
	
	buf[8] = (u8)(VsActive & 0xff);
	buf[9] = (u8)((VsActive >> 8) & 0xff);
	buf[10] = (u8)(VsTotal & 0xff);
	buf[11] = (u8)((VsTotal >> 8) & 0xff);
	buf[12] = (u8)(VSyncStart & 0xff);
	buf[13] = (u8)((VSyncStart >> 8) & 0xff);
	buf[14] = (u8)(VSyncEnd & 0xff);
	buf[15] = (u8)((VSyncEnd >> 8) & 0xff);

	DebugMessageControl("Set output format..\r\n");
	
	for(i = 0;i < 16;i++)	
	{
		FPGA_set(i+FPGA_REG_Format_h_Active_LSB,buf[i]);
		if((i+1)%2 == 0)
		{
			dd = buf[i];
			dd = dd << 8 | buf[i-1];
			DebugMessageControl("FPGA REG = %x Value = %d\r\n",i+FPGA_REG_Format_h_Active_LSB,dd);
		}
	}
	#endif
	pClk = tVideoDefineCustom.pClk;						//时钟
	Set5901FreqIndependent(IDT5P49V5901_1,pClk,2);		//让5901产生相应的时钟给FPGA
	//Set5901Freq(IDT5P49V5901_1,pClk,pClk,pClk,pClk);	//让5901产生相应的时钟给FPGA
}




extern const u8 FormatIndex[];
void SetOutFormat(BYTE index)
{
	//GetUserSettingParam()->userSplitParam.outputFormatIndex = index - output800x600x60Value;
	memcpy((u8*)&tVideoDefineCustom,(u8*)&tVideoDefine[FormatIndex[index]],sizeof(VideoDefine_Struct));
	SetOutputFormat(index);
	//SavePanleAndSplitParam();
	SetFpgaOutputFormat(index,0);
}



void SetOutputFormatCustom(void)
{
	menuEnterPage(MENU_INDEX_CUSTOM_FORMAT);
}

void setEDIDSourcePort(char value)
{
	if(value >= EDID_SOURCE_SIZE)
	{
		return;
	}
	#if 1
	EDIDManageSetting.sourcePort = value;
//	menuValueList[1] = getCustomEDIDH();
//	menuValueList[2] = getCustomEDIDV();
//	menuValueList[3] = getCustomEDIDF();
	switch(EDIDManageSetting.sourcePort)
	{
		case EDID_SOURCE_FOLLOW:
			{
				LCD_WriteLine3(langGetStr(edidEditingStr));
				menuSetBusy(2,11);
				saveUserEDIDMode(EDIDManageSetting.desPort,1);
				//editEDID(GetUserSetting()->outputFormat,EDIDManageSetting.desPort);
				SysDelay1ms(5000);
				menuDisableBusy();
				LCD_WriteLine3(langGetStr(studyEDIDSuccessStr));
				//menuProcessEvent(MENU_EVENT_STUDY_EDID_SUCCESS);
				SysDelay1ms(1000);
				menuProcessEvent(MENU_KEY_INFO);
			}
			break;
		case EDID_SOURCE_CUSTOM:
			{
				//saveUserEDIDMode(EDIDManageSetting.desPort,2);
				menuEnterPage(MENU_INDEX_CUSTOM_EDID);
				//menuProcessEvent(MENU_KEY_INFO);
			}
			break;
		default:
			{
				LCD_WriteLine3(langGetStr(edidEditingStr));
				menuSetBusy(2,11);
				saveUserEDIDMode(EDIDManageSetting.desPort,0);
				if(studyEDID(EDIDManageSetting.sourcePort,EDIDManageSetting.desPort) == FALSE)
				{
					//edid false
					menuDisableBusy();
					LCD_WriteLine3(langGetStr(studyEDIDFailStr));
					SysDelay1ms(1000);
					menuProcessEvent(MENU_KEY_INFO);
				}
				else
				{
					SysDelay1ms(5000);
					menuDisableBusy();
					LCD_WriteLine3(langGetStr(studyEDIDSuccessStr));
					//menuProcessEvent(MENU_EVENT_STUDY_EDID_SUCCESS);
					SysDelay1ms(1000);
					menuProcessEvent(MENU_KEY_INFO);
				}
			}
			break;
			
	}
#endif
}


char getEDIDSourcePort(void)
{
	return EDIDManageSetting.sourcePort;
}


void setEDIDDesPort(int value) 
{
	EDIDManageSetting.desPort = value;
}


int getEDIDDesPort(void)
{
	return EDIDManageSetting.desPort;
}


void refleshEDIDSourcePort(void)
{
	char value = EDIDManageSetting.desPort;	
	{
		switch(getUserEDIDMode(value))
		{
			case 0x01: //enable edid follow
				EDIDManageSetting.sourcePort = EDID_SOURCE_FOLLOW;
				defaultMenuPageProcess(MENU_EVENT_ENTER, 1);
				break;
			case 0x02: //enable edid custom
				EDIDManageSetting.sourcePort = EDID_SOURCE_CUSTOM;
				defaultMenuPageProcess(MENU_EVENT_ENTER, 1);
				break;
			default:
				if(EDIDManageSetting.desPort == VIPDRIVE_INPUT_DP)  //VGA端口不能设置为DVI/HDMI的EDID
				{
					EDIDManageSetting.sourcePort = EDID_SOURCE_RGBLINK_VGA; 
					defaultMenuPageProcess(MENU_EVENT_ENTER, 1);
					break;
				}
				break;
		}
	}
}


const u8 EDID_GreatView_DVI[128] = 
{
	0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x48,0xe2,0x14,0x13,0x37,0x47,0x44,0x59,
	0x1e,0x14,0x01,0x03,0x80,0x30,0x1b,0x78,0x2a,0x12,0xf1,0xa7,0x55,0x4a,0x9b,0x25,
	0x15,0x50,0x54,0x21,0x08,0x00,0xd1,0x00,0x81,0x00,0x81,0x40,0x81,0x80,0x95,0x00,
	0xe1,0xc0,0xa9,0x40,0xb3,0x00,0x02,0x3a,0x80,0x18,0x71,0x38,0x2d,0x40,0x58,0x2c,
	0x45,0x00,0xdd,0x0c,0x11,0x00,0x00,0x1e,0x00,0x00,0x00,0xfd,0x00,0x38,0x4b,0x1e,
	0x51,0x11,0x00,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfc,0x00,'L' ,
	'X' ,'Y' ,0x44,0x56,0x49,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x66,0x21,0x56,0xaa,
	0x51,0x00,0x1e,0x30,0x46,0x8f,0x33,0x00,0xdd,0x0c,0x11,0x00,0x00,0x20,0x00,0xc5
};


const u8 EDID_DualLinkDVI4K2K30HZ[128] = 
{
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x50, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1d, 0x19, 0x01, 0x04, 0xa1, 0x34, 0x20, 0x78, 0x33, 0x87, 0xe5, 0xa4, 0x56, 0x50, 0x9e, 0x26,
	0x0d, 0x50, 0x54, 0xef, 0xcf, 0x00, 0x45, 0x40, 0x61, 0x40, 0x81, 0xc0, 0xa9, 0xc0, 0xb3, 0xc0,
	0xd1, 0xc0, 0xd1, 0xfc, 0x01, 0x01, 0xb4, 0x66, 0x00, 0xa0, 0xf0, 0x70, 0x1f, 0x80, 0x30, 0x20,
	0x34, 0x00, 0x56, 0x50, 0x21, 0x00, 0x00, 0x1e, 0x79, 0x80, 0x80, 0xa0, 0x70, 0x38, 0x12, 0x40,
	0x58, 0x2c, 0x45, 0x00, 0xdd, 0x0c, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xfc, 0x00, 'L',
	'X',  'Y',  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x32, 0x71, 0x80, 0xa0,
	0x70, 0x38, 0x51, 0x40, 0x58, 0x2c, 0x45, 0x00, 0xdd, 0x0c, 0x11, 0x00, 0x00, 0x18, 0x00, 0x24,
};


const u8 EDID_GreatView_HDMI[256] = 
{
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x48, 0xe2, 0x22, 0x38, 0x01, 0x01, 0x01, 0x01, 
	0x07, 0x13, 0x01, 0x03, 0x80, 0x34, 0x1d, 0x78, 0x2e, 0xee, 0xd5, 0xa5, 0x55, 0x48, 0x9b, 0x26, 
	0x12, 0x50, 0x54, 0x21, 0x08, 0x00, 0xd1, 0x00, 0xb3, 0x00, 0xa9, 0x40, 0x95, 0x00, 0xe1, 0xc0, 
	0x81, 0x80, 0x90, 0x40, 0x81, 0x00, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c, 
	0x45, 0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xff, 0x00, 0x52, 0x38, 0x57, 
	0x30, 0x39, 0x30, 0x37, 0x30, 0x31, 0x36, 0x37, 0x30, 0x0a, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x32, 
	0x4b, 0x0f, 0x52, 0x12, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfc, 
	0x00, 'L' , 'X' , 'Y' , 0x48, 0x44, 0x4d, 0x49, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x20,

	0x02, 0x03, 0x36, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0xE4, 0x0F, 0x00,
    0x00, 0x01, 0xE3, 0x05, 0x03, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50,
    0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,

#if 0
	#if 0
	0x02, 0x03, 0x31, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0xE3, 0x05, 0x03,
    0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21,
    0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00, 0x54,
    0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79,

	#else
	0x02, 0x03, 0x25, 0xc1, 0x52, 0x90, 0x05, 0x04, 0x03, 0x02, 0x07, 0x05, 0x0f, 0x0e, 0x1f, 0x14, 
	0x1e, 0x1d, 0x13, 0x12, 0x11, 0x16, 0x01, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x65, 
	0x03, 0x0c, 0x00, 0x10, 0x00, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c, 0x45, 
	0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1e, 0x84, 0x35, 0x00, 0xa0, 0xa0, 0x30, 0x18, 0x30, 0x30, 
	0x20, 0x3a, 0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1a, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 
	0x20, 0x6e, 0x28, 0x55, 0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1e, 0x02, 0x3a, 0x80, 0xd0, 0x72, 
	0x38, 0x2d, 0x40, 0x10, 0x2c, 0x45, 0x80, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1e, 0x8c, 0x0a, 0xd0, 
	0x8a, 0x20, 0xe0, 0x2d, 0x10, 0x10, 0x3e, 0x96, 0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x18, 0xbf, 
    #endif
#endif
};



const u8 EDID_HDMI2_4K2K60Hz_PlayBox[256] = 
{

	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4A, 0x8B, 0x10, 0x64, 0x10, 0x64, 0x00, 0x00,
    0x19, 0x19, 0x01, 0x03, 0x80, 0x3C, 0x22, 0x78, 0x2E, 0xFC, 0xD0, 0xA6, 0x54, 0x4A, 0x9D, 0x24,
    0x0E, 0x50, 0x54, 0xA5, 0x6B, 0x80, 0xD1, 0xC0, 0x81, 0xC0, 0x81, 0x00, 0x81, 0x80, 0xA9, 0xC0,
    0xB3, 0x00, 0x01, 0x01, 0x01, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x52, 0x54, 0x4B,
    0x20, 0x55, 0x48, 0x44, 0x20, 0x36, 0x30, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x4C, 0x1E, 0xA0, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 'L',  'X',  'Y',  0x20, 'H',  'D',  'M',  'I',  0x0A, 0x20, 0x20, 0x20, 0x20, 0x01, 0xFE,

	#if 1
	0x02, 0x03, 0x31, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0xE3, 0x05, 0x03,
    0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21,
    0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00, 0x54,
    0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79,

	#else
    0x02, 0x03, 0x36, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0xE4, 0x0F, 0x00,
    0x00, 0x01, 0xE3, 0x05, 0x03, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50,
    0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    #endif
};

const u8 EDID_HDMI2_4K2K60Hz[256] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4A, 0x8B, 0x10, 0x64, 0x10, 0x64, 0x00, 0x00,
    0x19, 0x19, 0x01, 0x03, 0x80, 0x3C, 0x22, 0x78, 0x2E, 0xFC, 0xD0, 0xA6, 0x54, 0x4A, 0x9D, 0x24,
    0x0E, 0x50, 0x54, 0xA5, 0x6B, 0x80, 0xD1, 0xC0, 0x81, 0xC0, 0x81, 0x00, 0x81, 0x80, 0xA9, 0xC0,
    0xB3, 0x00, 0x01, 0x01, 0x01, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x52, 0x54, 0x4B,
    0x20, 0x55, 0x48, 0x44, 0x20, 0x36, 0x30, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x4C, 0x1E, 0xA0, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 'L',  'X',  'Y',  0x20, 'H',  'D',  'M',  'I',  0x0A, 0x20, 0x20, 0x20, 0x20, 0x01, 0xFE,

	0x02, 0x03, 0x36, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0xE4, 0x0F, 0x00,
    0x00, 0x01, 0xE3, 0x05, 0x03, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50,
    0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
	#if 0
    0x02, 0x03, 0x3E, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0xE4, 0x0F, 0x00, 0x00, 0x01, 0xE3, 0x05, 0x03, 0x01, 0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38,
    0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0x67, 0xD8, 0x5D, 0xC4, 0x01, 0x78, 0xC0, 0x00, 0x4D, 0xD0,
    0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E,
    0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00,
    0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF,
    //#else
	0x02, 0x03, 0x25, 0xc1, 0x52, 0x90, 0x05, 0x04, 0x03, 0x02, 0x07, 0x05, 0x0f, 0x0e, 0x1f, 0x14, 
	0x1e, 0x1d, 0x13, 0x12, 0x11, 0x16, 0x01, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x65, 
	0x03, 0x0c, 0x00, 0x10, 0x00, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c, 0x45, 
	0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1e, 0x84, 0x35, 0x00, 0xa0, 0xa0, 0x30, 0x18, 0x30, 0x30, 
	0x20, 0x3a, 0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1a, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 
	0x20, 0x6e, 0x28, 0x55, 0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1e, 0x02, 0x3a, 0x80, 0xd0, 0x72, 
	0x38, 0x2d, 0x40, 0x10, 0x2c, 0x45, 0x80, 0x08, 0x22, 0x21, 0x00, 0x00, 0x1e, 0x8c, 0x0a, 0xd0, 
	0x8a, 0x20, 0xe0, 0x2d, 0x10, 0x10, 0x3e, 0x96, 0x00, 0x08, 0x22, 0x21, 0x00, 0x00, 0x18, 0xbf, 
    #endif
};

const u8 EDID_HDMI2_NO_HFVSDB_NO420[256] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4A, 0x8B, 0x10, 0x64, 0x10, 0x64, 0x00, 0x00,
    0x19, 0x19, 0x01, 0x03, 0x80, 0x3C, 0x22, 0x78, 0x2E, 0xFC, 0xD0, 0xA6, 0x54, 0x4A, 0x9D, 0x24,
    0x0E, 0x50, 0x54, 0xA5, 0x6B, 0x80, 0xD1, 0xC0, 0x81, 0xC0, 0x81, 0x00, 0x81, 0x80, 0xA9, 0xC0,
    0xB3, 0x00, 0x01, 0x01, 0x01, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x52, 0x54, 0x4B,
    0x20, 0x55, 0x48, 0x44, 0x20, 0x36, 0x30, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x4C, 0x1E, 0xA0, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 'L',  'X',  'Y',  0x20, 'H',  'D',  'M',  'I',  0x0A, 0x20, 0x20, 0x20, 0x20, 0x01, 0xFE,
    
    0x02, 0x03, 0x31, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0xE3, 0x05, 0x03,
    0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21,
    0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00, 0x54,
    0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79,
};

const u8 EDID_HDMI2_NO_HFVSDB_WITH420[256] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4A, 0x8B, 0x10, 0x64, 0x10, 0x64, 0x00, 0x00,
    0x19, 0x19, 0x01, 0x03, 0x80, 0x3C, 0x22, 0x78, 0x2E, 0xFC, 0xD0, 0xA6, 0x54, 0x4A, 0x9D, 0x24,
    0x0E, 0x50, 0x54, 0xA5, 0x6B, 0x80, 0xD1, 0xC0, 0x81, 0xC0, 0x81, 0x00, 0x81, 0x80, 0xA9, 0xC0,
    0xB3, 0x00, 0x01, 0x01, 0x01, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x52, 0x54, 0x4B,
    0x20, 0x55, 0x48, 0x44, 0x20, 0x36, 0x30, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x4C, 0x1E, 0xA0, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 'L',  'X',  'Y',  0x20, 'H',  'D',  'M',  'I',  0x0A, 0x20, 0x20, 0x20, 0x20, 0x01, 0xFE,
    
    0x02, 0x03, 0x36, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0xE4, 0x0F, 0x00,
    0x00, 0x01, 0xE3, 0x05, 0x03, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50,
    0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
};

const u8 EDID_HDMI2_WITH_HFVSDB_NO420[256] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4A, 0x8B, 0x10, 0x64, 0x10, 0x64, 0x00, 0x00,
    0x19, 0x19, 0x01, 0x03, 0x80, 0x3C, 0x22, 0x78, 0x2E, 0xFC, 0xD0, 0xA6, 0x54, 0x4A, 0x9D, 0x24,
    0x0E, 0x50, 0x54, 0xA5, 0x6B, 0x80, 0xD1, 0xC0, 0x81, 0xC0, 0x81, 0x00, 0x81, 0x80, 0xA9, 0xC0,
    0xB3, 0x00, 0x01, 0x01, 0x01, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x52, 0x54, 0x4B,
    0x20, 0x55, 0x48, 0x44, 0x20, 0x36, 0x30, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x4C, 0x1E, 0xA0, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 'L',  'X',  'Y',  0x20, 'H',  'D',  'M',  'I',  0x0A, 0x20, 0x20, 0x20, 0x20, 0x01, 0xFE,
    
    0x02, 0x03, 0x39, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0xE3, 0x05, 0x03, 0x01, 0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38, 0x78, 0x20, 0x00, 0x40, 0x01,
    0x02, 0x67, 0xD8, 0x5D, 0xC4, 0x01, 0x78, 0xC0, 0x00, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E,
    0x80, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x56, 0x5E, 0x00, 0xA0, 0xA0,
    0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8,
};

const u8 EDID_HDMI2_WITH_HFVSDB_NO420_HDR[256] = 
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x5A, 0x63, 0x32, 0xBC, 0x01, 0x01, 0x01, 0x01,
	0x09, 0x1A, 0x01, 0x03, 0xB3, 0x3C, 0x22, 0x78, 0x3E, 0x64, 0x35, 0xA5, 0x54, 0x4F, 0x9E, 0x27, 
	0x12, 0x50, 0x54, 0xBF, 0xEF, 0x80, 0xD1, 0xC0, 0xD1, 0xE8, 0xD1, 0xFC, 0x95, 0x00, 0x90, 0x40,
    0x81, 0x80, 0x81, 0x40, 0x81, 0xC0, 0x40, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x52, 0x54, 0x4B,
    0x20, 0x48, 0x44, 0x52, 0x20, 0x34, 0x4B, 0x20, 0x36, 0x30, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x64, 
    0x65, 0x6D, 0x6F, 0x73, 0x65, 0x74, 0x2D, 0x31, 0x0A, 0x20, 0x30, 0x20, 0x00, 0x00, 0x00, 0xFD, 
    0x00, 'L',  'X',  'Y',  0x20, 'H',  'D',  'M',  'I',  0x0A, 0x20, 0x20, 0x20, 0x20, 0x01, 0xBE, 

	0x02, 0x03, 0x52, 0xF2, 0x5E, 0x04, 0x05, 0x10, 0x13, 0x14, 0x1F, 0x20, 0x21, 0x22, 0x27, 0x48, 
	0x49, 0x4A, 0x4B, 0x4C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 
	0x69, 0x6A, 0x6B, 0xE2, 0x00, 0xC0, 0xE3, 0x05, 0xC0, 0x00, 0x23, 0x09, 0x7F, 0x07, 0x83, 0x01, 
	0x00, 0x00, 0x67, 0x03, 0x0C, 0x00, 0x10, 0x00, 0x38, 0x78, 0x67, 0xD8, 0x5D, 0xC4, 0x01, 0x78, 
	0x88, 0x00, 0xE6, 0x06, 0x05, 0x01, 0x69, 0x69, 0x4F, 0x68, 0x1A, 0x00, 0x00, 0x01, 0x01, 0x30, 
	0x90, 0xE6, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x25, 0x00, 0x55, 0x50, 
	0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00, 
	0x55, 0x50, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD1,

};

const u8 EDID_HDMI2_WITH_HFVSDB_WITH420[256] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4A, 0x8B, 0x10, 0x64, 0x10, 0x64, 0x00, 0x00,
    0x19, 0x19, 0x01, 0x03, 0x80, 0x3C, 0x22, 0x78, 0x2E, 0xFC, 0xD0, 0xA6, 0x54, 0x4A, 0x9D, 0x24,
    0x0E, 0x50, 0x54, 0xA5, 0x6B, 0x80, 0xD1, 0xC0, 0x81, 0xC0, 0x81, 0x00, 0x81, 0x80, 0xA9, 0xC0,
    0xB3, 0x00, 0x01, 0x01, 0x01, 0x01, 0x4D, 0xD0, 0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20,
    0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x52, 0x54, 0x4B,
    0x20, 0x55, 0x48, 0x44, 0x20, 0x36, 0x30, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x4C, 0x1E, 0xA0, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 'L',  'X',  'Y',  0x20, 'H',  'D',  'M',  'I',  0x0A, 0x20, 0x20, 0x20, 0x20, 0x01, 0xFE,
    
    0x02, 0x03, 0x3E, 0xF2, 0x53, 0x10, 0x1F, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x20, 0x61, 0x5E, 0x5F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0xE4, 0x0F, 0x00, 0x00, 0x01, 0xE3, 0x05, 0x03, 0x01, 0x6C, 0x03, 0x0C, 0x00, 0x20, 0x00, 0x38,
    0x78, 0x20, 0x00, 0x40, 0x01, 0x02, 0x67, 0xD8, 0x5D, 0xC4, 0x01, 0x78, 0xC0, 0x00, 0x4D, 0xD0,
    0x00, 0xA0, 0xF0, 0x70, 0x3E, 0x80, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00, 0x00, 0x1E,
    0x56, 0x5E, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00, 0x54, 0x4F, 0x21, 0x00,
    0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF,

};

const u8 EDID_GreatView_DP[256] = 
{
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x48, 0xe2, 0x17, 0xf0, 0x53, 0x59, 0x35, 0x30, 
	0x07, 0x17, 0x01, 0x04, 0xb5, 0x34, 0x20, 0x78, 0x3a, 0x1e, 0xc5, 0xae, 0x4f, 0x34, 0xb1, 0x26, 
	0x0e, 0x50, 0x54, 0x21, 0x08, 0x00, 0x81, 0x80, 0xa9, 0x40, 0xd1, 0x00, 0x81, 0x00, 0x95, 0x00, 
	0x90, 0x40, 0xa9, 0xc0, 0xb3, 0x00, 0x28, 0x3c, 0x80, 0xa0, 0x70, 0xb0, 0x23, 0x40, 0x30, 0x20, 
	0x36, 0x00, 0x06, 0x44, 0x21, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0xff, 0x00, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x0a, 0x00, 0x00, 0x00, 0xfc, 0x00, 'L', 
	'X', 'Y', 0x20, 0x44, 0x50, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd, 
	
	0x00, 0x38, 0x4c, 0x1e, 0x51, 0x11, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xe7, 
	0x02, 0x03, 0x1d, 0xc1, 0x50, 0x90, 0x05, 0x04, 0x03, 0x02, 0x07, 0x16, 0x01, 0x1f, 0x12, 0x13, 
	0x14, 0x20, 0x15, 0x11, 0x06, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x02, 0x3a, 0x80, 
	0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x06, 0x44, 0x21, 0x00, 0x00, 0x1e, 0x84, 
	0x35, 0x00, 0xa0, 0xa0, 0x30, 0x18, 0x30, 0x30, 0x20, 0x3a, 0x00, 0x06, 0x44, 0x21, 0x00, 0x00, 
	0x9a, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28, 0x55, 0x00, 0x06, 0x44, 0x21, 
	0x00, 0x00, 0x1e, 0x8c, 0x0a, 0xd0, 0x8a, 0x20, 0xe0, 0x2d, 0x10, 0x10, 0x3e, 0x96, 0x00, 0x06, 
	0x44, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
};





BYTE EdidCheckHeader(unsigned char * edid_data)
{
	if(edid_data[0] == 0x00 && edid_data[7] == 0x00
		&& edid_data[1] == 0xFF && edid_data[2] == 0xFF && edid_data[3] == 0xFF 
		&& edid_data[4] == 0xFF && edid_data[5] == 0xFF && edid_data[6] == 0xFF )
	{
		return TRUE;
	}
	return FALSE;
}



char studyEDID(char studyFrom,char writeTo) 
{
	u8 previewEdid[256];
	u8 res = TRUE;
	switch(studyFrom)
	{	
		case EDID_SOURCE_OUTPUT1:
			ReadEDID_EEPROM(EDID_PORT_2, 0,previewEdid,256);	
			if(EdidCheckHeader(previewEdid) == FALSE)
			{
				res = FALSE;
				return res;			
			}
		break;
		case EDID_SOURCE_OUTPUT2:
		break;
		case EDID_SOURCE_OUTPUTVGA:
		break;
		default:
		break;
	}
	
	switch(writeTo)
	{
		case EDID_DES_PORT_DP:
			break;
		case EDID_DES_PORT_DVI:
			editEDID(0,EDID_DES_PORT_DVI);
			break;
		case EDID_DES_PORT_M1:
		case EDID_DES_PORT_M2:
		case EDID_DES_PORT_M3:
			break;
		case EDID_DES_PORT_HDMI:
			editEDID(0,EDID_DES_PORT_HDMI);
		break;
		case EDID_DES_PORT_HDMI2:
			editEDID(0,EDID_DES_PORT_HDMI2);
		break;
		default:
			break;
	}

	return res;
}

void VideoTimmingToEdidTimming(edid_timming_type *pEditTimming, int format)
{
	
	pEditTimming->pixelClock = GetDlckByVideoFormat(format)/10000;
	pEditTimming->hActive = GetHDEByVideoFormat(format);
	pEditTimming->vActive = GetVDEByVideoFormat(format);
	pEditTimming->hBlank = GetHTotalByVideoFormat(format) - GetHDEByVideoFormat(format);
	pEditTimming->vBlank = GetVTotalByVideoFormat(format) - GetVDEByVideoFormat(format);
	pEditTimming->hSyncOffset = GetHTotalByVideoFormat(format) - GetHDEByVideoFormat(format) - GetHStartByVideoFormat(format);
	pEditTimming->vSyncOffset = GetVTotalByVideoFormat(format) - GetVDEByVideoFormat(format) - GetVStartByVideoFormat(format);
	pEditTimming->hSyncWidth = GetHSyncWidthByVideoFormat(format);
	pEditTimming->vSyncWidth = GetVSyncWidthByVideoFormat(format);
	pEditTimming->hvPolarity = (~ GetHSyncPolarityByVideoFormat(format))&0x03;
	pEditTimming->hvPolarity += (GetHSyncPolarityByVideoFormat(format))&0x80;
	if((GetHSyncPolarityByVideoFormat(format))&0x80) // interlace
	{
		pEditTimming->vActive /= 2;
		pEditTimming->vBlank /= 2;
		pEditTimming->vSyncOffset = pEditTimming->vBlank - GetVStartByVideoFormat(format);
	}
}

void VideoTimmingToEdidTimmingByActive(edid_timming_type *pEditTimming, VideoDefine_Struct formatTiming,Resolution_Struct formatResolution)
{	
	pEditTimming->pixelClock = (formatTiming.htotal*formatTiming.vtotal*formatTiming.vfreq )/10000;
	pEditTimming->hActive = formatResolution.DisplayWidth;
	pEditTimming->vActive = formatResolution.DisplayHeight;
	pEditTimming->hBlank = formatTiming.htotal - formatResolution.DisplayWidth;
	pEditTimming->vBlank = formatTiming.vtotal - formatResolution.DisplayHeight;
	pEditTimming->hSyncOffset = formatTiming.htotal - formatResolution.DisplayWidth - formatTiming.hstart;
	pEditTimming->vSyncOffset = formatTiming.vtotal - formatResolution.DisplayHeight - formatTiming.vstart;
	pEditTimming->hSyncWidth = formatTiming.hSyncWidth;
	pEditTimming->vSyncWidth = formatTiming.vSyncWidth;
	pEditTimming->hvPolarity = (~ formatTiming.hvSyncPolarity)&0x03;
	pEditTimming->hvPolarity += (formatTiming.hvSyncPolarity)&0x80;
	if(formatTiming.hvSyncPolarity&0x80) // interlace
	{
		pEditTimming->vActive /= 2;
		pEditTimming->vBlank /= 2;
		pEditTimming->vSyncOffset = pEditTimming->vBlank - formatTiming.vstart;
	}
}

void CreateDetailedTimmingDescriptor(unsigned char *buffer, edid_timming_type timming)
{
	buffer[0] = timming.pixelClock;
	buffer[1] = timming.pixelClock>>8;
	buffer[2] = timming.hActive;
	buffer[3] = timming.hBlank;
	buffer[4] = (timming.hBlank>>8)&0x0f;
	buffer[4] += (timming.hActive>>4)&0xf0;
	buffer[5] = timming.vActive;
	buffer[6] = timming.vBlank;
	buffer[7] = (timming.vBlank>>8)&0x0f;
	buffer[7] += (timming.vActive>>4)&0xf0;
	buffer[8] = timming.hSyncOffset;
	buffer[9] = timming.hSyncWidth;
	buffer[10] = (timming.vSyncWidth)&0x0f;
	buffer[10] += (timming.vSyncOffset<<4)&0xf0;
	buffer[11] = (timming.vSyncWidth>>4)&0x03;
	buffer[11] += (timming.vSyncOffset>>2)&0x0C;
	buffer[11] += (timming.hSyncWidth>>4)&0x30;
	buffer[11] += (timming.hSyncOffset>>2)&0xC0;
#if 0
	// image size mm
	buffer[12] = timming.hActive; 
	buffer[13] = timming.vActive;
	buffer[14] = (timming.vActive>>8)&0x0f;
	buffer[14] += (timming.hActive>>4)&0xf0;
	buffer[15] = 0; // hor border
	buffer[16] = 0; // ver border
#endif
	buffer[17] = 0x18;  // Digital separate
	buffer[17] += (timming.hvPolarity<<1)&0x06;
	buffer[17] += (timming.hvPolarity&0x80);

	if(1)
	{
		int i;
		DebugMessageControl("\r\nedit :	 \r\n");
		for(i = 0; i < 18; i++)
		{
			DebugMessageControl(" %02x, ",*(buffer + i));
		}
		DebugMessageControl("\r\n");
	}
	
}



void ResetEdidChecksum(unsigned char * edid_data)
{
	int i;
	unsigned char sum = 0;
	for(i=0; i<127; i++)
	{
		sum += edid_data[i];
	}
	edid_data[127] = (~sum)+1;
}

void editEDID(BYTE outputFormat,BYTE inputPort)
{
	int i;
	//u8 *edid_data;
	u8 edid_data[256];
	u8 edidReadBuf[256];
//	edid_timming_type edid_timming;

	DebugMessageControl("editEDID(%d,%d)\r\n",outputFormat,inputPort);

	for(i = 0; i < 256;i++)
	{
		edid_data[i] = EDID_GreatView_HDMI[i];
	}

	//VideoTimmingToEdidTimming(&edid_timming, outputFormat);
	//CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
	//ResetEdidChecksum(edid_data);

	switch(inputPort)
	{
		case EDID_DES_PORT_DP:
			break;
		case EDID_DES_PORT_DVI:
			//video_setEdidWp(1);
			memcpy(edid_data,EDID_GreatView_DVI,sizeof(EDID_GreatView_DVI));
			WriteEDID_EEPROM(EDID_PORT_3, 0x0,edid_data,128);
			//video_setEdidWp(0);
			SetHPD_Pin(EDID_PORT_3,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_3,0);				
			break;
		case EDID_DES_PORT_M1:
		case EDID_DES_PORT_M2:
		case EDID_DES_PORT_M3:
			break;
		case EDID_DES_PORT_HDMI:
			DebugMessageControl("Edit stand edid HDMI-1\r\n");
			//video_setEdidWp(1);
			WriteEDID_EEPROM(EDID_PORT_1, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_1, 0,edidReadBuf,256);
			//video_setEdidWp(0);
			SetHPD_Pin(EDID_PORT_1,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_1,0);	
			break;
		case EDID_DES_PORT_HDMI2:
			DebugMessageControl("Edit stand edid HDMI-2\r\n");
			WriteEDID_EEPROM(EDID_PORT_2, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_2, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_2,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_2,0);	
		break;
		default:
			break;
	}
}

void EdidBufCopy(u8 *dst,const u8 *from)
{
	int i;
	for(i = 0; i < 256;i++)
	{
		dst[i] = from[i];
	}
}

extern VideoDefine_Struct m_customVideo;  
extern Resolution_Struct m_customResolution;
extern u8 getCustomTimming(int h_pixels,int v_lines,int freq);


void WriteEdid(int index)
{
//	int i;
	u8 edid_data[256];
	u8 edidReadBuf[256];
	u8 buf[DETAILED_DESCRIPTOR_SIZE];
	edid_timming_type edid_timming;
	//VideoDefine_Struct formatTiming;
	//Resolution_Struct formatResolution;
	
	DebugMessageControl("WriteEdid(%d)\r\n",index);
	switch(index)
	{
		case hdmi20PortHdmi20EdidValue:
			DebugMessageControl("Edit hdmi2.0 Port Hdmi2.0 Edid Value\r\n");
			menuSetBusy(1,9);
		
			EdidBufCopy(edid_data,EDID_HDMI2_4K2K60Hz);
			//getCustomTimming(3840,2160,60);
			//VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			//CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
	
			WriteEDID_EEPROM(EDID_PORT_2, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_2, 0,edidReadBuf,256);
		
			menuDisableBusy();
			SetHPD_Pin(EDID_PORT_2,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_2,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(1,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_HDMI20_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case hdmi20PortHdmi14EdidValue:
			DebugMessageControl("Edit hdmi2.0 Port Hdmi1.4 Edid Value\r\n");
			menuSetBusy(2,9);
			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
			getCustomTimming(3840,2160,30);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
		
//			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
//			ResetEdidChecksum(edid_data);
			WriteEDID_EEPROM(EDID_PORT_2, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_2, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_2,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_2,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_HDMI20_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case hdmi20PortHdmi13EdidValue:
			DebugMessageControl("Edit hdmi2.0 Port Hdmi1.3 Edid Value\r\n");
			menuSetBusy(3,9);
			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
			getCustomTimming(1920,1080,60);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
		
//			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
//			ResetEdidChecksum(edid_data);
			WriteEDID_EEPROM(EDID_PORT_2, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_2, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_2,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_2,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(3,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_HDMI20_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;

		case hdmi14PortHdmi14EdidValue:
			DebugMessageControl("Edit hdmi1.4 Port Hdmi1.4 Edid Value\r\n");
			menuSetBusy(1,9);
			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
			getCustomTimming(3840,2160,30);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
		
//			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
//			ResetEdidChecksum(edid_data);
			WriteEDID_EEPROM(EDID_PORT_1, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_1, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_1,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_1,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(1,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_HDMI14_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case hdmi14PortHdmi13EdidValue:
			DebugMessageControl("Edit hdmi1.4 Port Hdmi1.3 Edid Value\r\n");
			menuSetBusy(2,9);
			
			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
			getCustomTimming(1920,1080,60);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
		
//			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
//			ResetEdidChecksum(edid_data);
			WriteEDID_EEPROM(EDID_PORT_1, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_1, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_1,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_1,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_HDMI14_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case dpPortDpEdidValue:
		break;
		case dpPort4k2k60Value:
			DebugMessageControl("Edit Dp Port Dp Edid Value\r\n");
			menuSetBusy(1,9);
			getCustomTimming(3840,2160,60);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			//VideoTimmingToEdidTimmingByActive(&edid_timming, formatTiming,formatResolution);
			CreateDetailedTimmingDescriptor(buf, edid_timming);
			SysDelay1ms(2000);
			SetDpEdid(_DP_EDID_CUSTOM,buf);
			menuDisableBusy();
			LCD_WriteStr_fb2(1,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_DP_PORT);
		break;
		case dpPort4k2k30Value:
			DebugMessageControl("Edit Dp Port Dp Edid Value\r\n");
			menuSetBusy(2,9);
			getCustomTimming(3840,2160,30);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			CreateDetailedTimmingDescriptor(buf, edid_timming);
			SysDelay1ms(2000);
			SetDpEdid(_DP_EDID_CUSTOM,buf);
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_DP_PORT);
		break;
		case dviPortDualLinkDviEdidValue:
			DebugMessageControl("Edit dvi Port DualLinkDvi Edid Value\r\n");
			menuSetBusy(1,9);
		
			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
			getCustomTimming(3840,2160,30);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
		
//			EdidBufCopy(edid_data,EDID_DualLinkDVI4K2K30HZ);
//			ResetEdidChecksum(edid_data);
			WriteEDID_EEPROM(EDID_PORT_3, 0x0,edid_data,128);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_3, 0,edidReadBuf,128);
			SetHPD_Pin(EDID_PORT_3,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_3,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(1,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_DVI_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case dviPortDviEdidValue:
			DebugMessageControl("Edit dvi Port Dvi Edid Value\r\n");
			menuSetBusy(2,9);
			EdidBufCopy(edid_data,EDID_GreatView_DVI);
			ResetEdidChecksum(edid_data);
			WriteEDID_EEPROM(EDID_PORT_3, 0x0,edid_data,128);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_3, 0,edidReadBuf,128);
			SetHPD_Pin(EDID_PORT_3,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_3,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_DVI_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case dviPortHdmi13EdidValue:
			DebugMessageControl("Edit dvi Port Dvi Edid Value\r\n");
			menuSetBusy(3,9);
			EdidBufCopy(edid_data,EDID_GreatView_HDMI);
//
			getCustomTimming(1920,1080,60);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
			
			WriteEDID_EEPROM(EDID_PORT_3, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_3, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_3,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_3,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(3,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_DVI_PORT);
		break;
		default:
		break;
	}
}



void editCustomEDID(VideoDefine_Struct formatTiming,Resolution_Struct formatResolution,BYTE inputPort)
{
//	int i;
	u8 edid_data[256];	
	u8 edidReadBuf[256];
	u8 buf[DETAILED_DESCRIPTOR_SIZE];
	edid_timming_type edid_timming;
	EdidBufCopy(edid_data,EDID_GreatView_HDMI);
	VideoTimmingToEdidTimmingByActive(&edid_timming, formatTiming,formatResolution);
	CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
	ResetEdidChecksum(edid_data);

	switch(inputPort)
	{
		case 0://hdmi20PortCustomEdidValue
			DebugMessageControl("Edit hdmi2.0 Port Hdmi2.0 Edid Value\r\n");
			menuSetBusy(2,9);
			WriteEDID_EEPROM(EDID_PORT_2, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_2, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_2,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_2,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_HDMI20_PORT);
		break;
		case 1://hdmi14PortCustomEdidValue
			DebugMessageControl("Edit hdmi2.0 Port Hdmi2.0 Edid Value\r\n");
			menuSetBusy(2,9);
			//EdidBufCopy(edid_data,EDID_HDMI2_4K2K60Hz);
			WriteEDID_EEPROM(EDID_PORT_1, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_1, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_1,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_1,0);///////////////////////////////
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_HDMI14_PORT);
		break;
		case 2://dpPortCustomEdidValue
			DebugMessageControl("Edit Dp Port Dp Edid Value\r\n");
			menuSetBusy(2,9);
			CreateDetailedTimmingDescriptor(buf, edid_timming);
			SysDelay1ms(2000);
			SetDpEdid(_DP_EDID_CUSTOM,buf);
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_DP_PORT);
		break;
		case 3://dviPortCustomEdidValue
			DebugMessageControl("Edit hdmi2.0 Port Hdmi2.0 Edid Value\r\n");
			menuSetBusy(2,9);
			//EdidBufCopy(edid_data,EDID_DualLinkDVI4K2K30HZ);
			WriteEDID_EEPROM(EDID_PORT_3, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_3, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_3,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_3,0);
			menuDisableBusy();
			LCD_WriteStr_fb2(2,20, "   OK  ");
			SysDelay1ms(1000);
			menuEnterPage(MENU_INDEX_EDID_DVI_PORT);
		break;
			
		default:
			break;
	}
	
}




void resetEDID(int index)
{
//	int i;
	u8 edid_data[256];
	u8 edidReadBuf[256];
	u8 buf[DETAILED_DESCRIPTOR_SIZE];
	edid_timming_type edid_timming;
	//VideoDefine_Struct formatTiming;
	//Resolution_Struct formatResolution;
	

	DebugMessageControl("WriteEdid(%d)\r\n",index);
	switch(index)
	{
		case hdmi20PortHdmi20EdidValue:
			DebugMessageControl("Edit hdmi2.0 Port Hdmi2.0 Edid Value\r\n");
			//menuSetBusy(1,11);
			//EdidBufCopy(edid_data,EDID_HDMI2_4K2K60Hz);

			EdidBufCopy(edid_data,EDID_HDMI2_4K2K60Hz);
			//getCustomTimming(3840,2160,60);
			//VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			//CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
	
			WriteEDID_EEPROM(EDID_PORT_1, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_1, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_1,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_1,0);
			//menuDisableBusy();
			//LCD_WriteStr_fb2(1,22, "   OK  ");
			//SysDelay1ms(1000);
			//menuEnterPage(MENU_INDEX_EDID_HDMI20_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case hdmi14PortHdmi14EdidValue:
			DebugMessageControl("Edit hdmi1.4 Port Hdmi2.0 Edid Value\r\n");
			//menuSetBusy(1,11);
			EdidBufCopy(edid_data,EDID_GreatView_HDMI);

			//getCustomTimming(3840,2160,30);
			//VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			//CreateDetailedTimmingDescriptor(&edid_data[DETAILED_DESCRIPTOR_OFFSET], edid_timming);
			ResetEdidChecksum(edid_data);
			
			WriteEDID_EEPROM(EDID_PORT_2, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_2, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_2,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_2,0);
			//menuDisableBusy();
			//LCD_WriteStr_fb2(1,22, "   OK  ");
			//SysDelay1ms(1000);
			//menuEnterPage(MENU_INDEX_EDID_HDMI14_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		case dpPort4k2k60Value:
			DebugMessageControl("Edit Dp Port Dp Edid Value\r\n");
			//menuSetBusy(1,11);
			getCustomTimming(3840,2160,60);
			VideoTimmingToEdidTimmingByActive(&edid_timming, m_customVideo,m_customResolution);
			//VideoTimmingToEdidTimmingByActive(&edid_timming, formatTiming,formatResolution);
			CreateDetailedTimmingDescriptor(buf, edid_timming);
			SysDelay1ms(2000);
			SetDpEdid(_DP_EDID_CUSTOM,buf);
			//menuDisableBusy();
			//LCD_WriteStr_fb2(1,22, "   OK  ");
			//SysDelay1ms(1000);
			//menuEnterPage(MENU_INDEX_EDID_DP_PORT);
		break;
		case dviPortDviEdidValue:
			DebugMessageControl("Edit dvi Port Dvi Edid Value\r\n");
			//menuSetBusy(2,11);
			EdidBufCopy(edid_data,EDID_DualLinkDVI4K2K30HZ);
			ResetEdidChecksum(edid_data);
			WriteEDID_EEPROM(EDID_PORT_3, 0x0,edid_data,256);
			memset(edidReadBuf,0,256);
			ReadEDID_EEPROM(EDID_PORT_3, 0,edidReadBuf,256);
			SetHPD_Pin(EDID_PORT_3,1);
			SysDelay1ms(2000);
			SetHPD_Pin(EDID_PORT_3,0);
			//menuDisableBusy();
			//LCD_WriteStr_fb2(2,22, "   OK  ");
			//SysDelay1ms(1000);
			//menuEnterPage(MENU_INDEX_EDID_DVI_PORT);
			//menuProcessEvent(MENU_KEY_INFO);
		break;
		default:
		break;
	}
}



void SetSplitParamToFpga(PanelSplitParamSt* panelSt)
{

	u8 buf[sizeof(PanelSplitParamSt)];
	int i;
//	u8 dataToFpga;
	u16 dd = 0;
	memcpy(buf,(u8*)panelSt,sizeof(PanelSplitParamSt));

	
	//DebugMessageControl("FPGA REG TEST W %d,R %d\r\n",dataToFpga,dataToFpga);

	DebugMessageControl("Write panel V param\r\n");
	for(i = 0;i < PANEL_PARAM_LEN;i++)	//写整屏垂直方向参数
	{
		FPGA_set(i+FPGA_REG_V_PaneStart0Pos_LSB,buf[i]);
		if((i+1)%2 == 0)
		{
			dd = buf[i];
			dd = dd << 8 | buf[i-1];
			DebugMessageControl("FPGA REG = %x Value = %d\r\n",i+FPGA_REG_V_PaneStart0Pos_LSB,dd);
		}
	}

	DebugMessageControl("Write output 0 param\r\n");
	for(i = 0;i < OUTPUT_PORT_PARAM_LEN;i++)	//写OUTPUT0参数
	{
		FPGA_set(i+FPGA_REG_h_Output0Layer_A_StartPos_LSB,buf[i+PANEL_PARAM_LEN]);
		if((i+1)%2 == 0)
		{
			dd = buf[i+PANEL_PARAM_LEN];
			dd = dd << 8 | buf[i+PANEL_PARAM_LEN-1];
			DebugMessageControl("FPGA REG = %x Value = %d\r\n",i+FPGA_REG_h_Output0Layer_A_StartPos_LSB,dd);
		}
	}

	DebugMessageControl("Write output 1 param\r\n");
	for(i = 0;i < OUTPUT_PORT_PARAM_LEN;i++)	//写OUTPUT1参数
	{
		FPGA_set(i+FPGA_REG_h_Output1Layer_A_StartPos_LSB,buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN]);
		if((i+1)%2 == 0)
		{
			dd = buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN];
			dd = dd << 8 | buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN-1];
			DebugMessageControl("FPGA REG = %x Value = %d\r\n",i+FPGA_REG_h_Output1Layer_A_StartPos_LSB,dd);
		}
		//DebugMessageControl("FPGA REG = %x Value = %x\r\n",i+FPGA_REG_h_Output1Layer_A_StartPos_LSB,buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN]);
	}

	DebugMessageControl("Write output 2 param\r\n");
	for(i = 0;i < OUTPUT_PORT_PARAM_LEN;i++)	//写OUTPUT2参数
	{
		FPGA_set(i+FPGA_REG_h_Output2Layer_A_StartPos_LSB,buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*2]);
		if((i+1)%2 == 0)
		{
			dd = buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*2];
			dd = dd << 8 | buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*2-1];
			DebugMessageControl("FPGA REG = %x Value = %d\r\n",i+FPGA_REG_h_Output2Layer_A_StartPos_LSB,dd);
		}
		//CONTROL_DBG("FPGA REG = %x Value = %x\r\n",i+FPGA_REG_h_Output2Layer_A_StartPos_LSB,buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*2]);
	}

	DebugMessageControl("Write output 3 param\r\n");
	for(i = 0;i < OUTPUT_PORT_PARAM_LEN;i++)	//写OUTPUT3参数
	{
		FPGA_set(i+FPGA_REG_h_Output3Layer_A_StartPos_LSB,buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*3]);
		if((i+1)%2 == 0)
		{
			dd = buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*3];
			dd = dd << 8 | buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*3-1];
			DebugMessageControl("FPGA REG = %x Value = %d\r\n",i+FPGA_REG_h_Output3Layer_A_StartPos_LSB,dd);
		}
		//DebugMessageControl("FPGA REG = %x Value = %x\r\n",i+FPGA_REG_h_Output3Layer_A_StartPos_LSB,buf[i+PANEL_PARAM_LEN+OUTPUT_PORT_PARAM_LEN*3]);
	}

	//SysDelay1ms(5000);
	FPGA_set(FPGA_REG_DisplayMode,GetSystemParam()->FpgaDisMode);
	
	FpgaOutputPortSel(	GetPanleAndSplitParamRam()->outputPort1Map,
						GetPanleAndSplitParamRam()->outputPort2Map,
						GetPanleAndSplitParamRam()->outputPort3Map,
						GetPanleAndSplitParamRam()->outputPort4Map
					);
}


void SetDisplayAlpha(u8 alpha)
{
	FPGA_set(0x20,alpha);
	FPGA_set(0x60,alpha);
	//FPGA_set(0x40,alpha);
	//FPGA_set(0x80,alpha);
}


void SetFreeze(void)
{
	if(freezeStatus == 0)
	{
		SetKeyLedMode(KEY_LOAD,LED_ON_ALWAYS);
		freezeStatus = 1;
	}
	else
	{
		SetKeyLedMode(KEY_LOAD,LED_OFF_ALWAYS);
		freezeStatus = 0;
	}
	FPGASetFreeze(freezeStatus);
}


//###############################################################################################




