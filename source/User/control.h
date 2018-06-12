#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "define.h"
#include "userpref.h"


#define SCALE_MIN_WIDTH  (2)//(61)
#define SCALE_MIN_HEIGHT  (2)//(36)


#define LOBYTE(w)           ((BYTE)(WORD)(w))
#define HIBYTE(w)           ((BYTE)((WORD)(w) >> 8))
#define min(x,y)			((x<y)? x:y)

extern BYTE OutputAlpha[4][4];// [OUTPUT][LAYER]
extern BYTE OutputAlphaUpdate[4];// [OUTPUT]

extern BYTE InputFormatOld[4];
extern BYTE OutputAlphaOld[4][4];

#define INPUT_PORT_DVI_1 (0)
#define INPUT_PORT_DVI_2 (1)
#define INPUT_PORT_DVI_3 (2)
#define INPUT_PORT_DVI_4 (3)
#define DETAILED_DESCRIPTOR_OFFSET (0x36)
#define DETAILED_DESCRIPTOR_SIZE   (18) 


#define IMAGE_A		0
#define IMAGE_B		1

enum
{
	IMAGE_WIDTH,
	IMAGE_HEIGHT,
	IMAGE_X,
	IMAGE_Y,
};



enum TimingID{
	timing_id_800_600_60,
	timing_id_1024_768_60,
	timing_id_1280_960_60,
	timing_id_1280_1024_60,
	timing_id_1366_768_60,
	timing_id_1440_900_60,
	timing_id_1440_1050_60,
	timing_id_1600_900_60,
	timing_id_1680_1050_60,
	timing_id_1920_1080_60,
	timing_id_1920_1200_60,
	timing_id_2048_1152_60,
	timing_id_800_600_120,
	timing_id_count
};


enum TimingVarName{
	timing_var_name_h_width,
	timing_var_name_h_blank,
	timing_var_name_h_sync_offset,
	timing_var_name_h_sync,
	timing_var_name_h_border,
	timing_var_name_v_width,
	timing_var_name_v_blank,
	timing_var_name_v_sync_offset,
	timing_var_name_v_sync,
	timing_var_name_v_border,
	timing_var_count
};



enum HblankE{
	hBlank_800,
	hBlank_1024,
	hBlank_1280,
	hBlank_1366,
	hBlank_1440,
	hBlank_1600,
	hBlank_1680,
	hBlank_1920,
	hBlank_2048,
	hBlank_Counts
};

enum VblankE{
	vBlank_600,
	vBlank_768,
	vBlank_900,
	vBlank_960,
	vBlank_1024,
	vBlank_1050,
	vBlank_1080,
	vBlank_1152,
	vBlank_1200,
	vBlank_Counts
};

typedef enum
{
	//VIPDRIVE_INPUT_VGA,					 // 0
	VIPDRIVE_INPUT_DVI,					 // 2
	VIPDRIVE_INPUT_DP,					 // 1
	VIPDRIVE_INPUT_HDMI,				// 3	
	VIPDRIVE_INPUT_HDMI2,				// 3	
	VIPDRIVE_INPUT_M1,					// 4
	VIPDRIVE_INPUT_M2,				// 5
	VIPDRIVE_INPUT_M3,				// 6
	VIPDRIVE_INPUT_M4,				// 7
	VIPDRIVE_INPUT_SIZE,
	VIPDRIVE_INPUT_MASK = 0x0F,

	VIPDRIVE_INPUT_PROGRAM_MASK = 0x40,
	VIPDRIVE_INPUT_PREVEW_MASK = 0x80,
	VIPDRIVE_INPUT_TYPE_MASK = 0xF0,
}VipDrive_Source_TypeDef;

typedef enum
{
	LAYOUT_PIP_LT,
	LAYOUT_PIP_RT,	 //added by cyj
	LAYOUT_PIP_LB,	 //added by cyj
	LAYOUT_PIP_RB,	 //added by cyj
	LAYOUT_PIP_CENT, //added by cyj
	LAYOUT_PBP_LR,
	LAYOUT_PBP_TB,
	LAYOUT_SIZE,
} PipLayout_TypeDef;

enum
{
	SWITCH_MODE_FADE,
	SWITCH_MODE_CUT,
	SWITCH_MODE_POP_CENTER,
	SWITCH_MODE_PUSH_CENTER,
	SWITCH_MODE_POP_LT,
	SWITCH_MODE_PUSH_LT,
	SWITCH_MODE_POP_RT,
	SWITCH_MODE_PUSH_RT,
	SWITCH_MODE_POP_LB,
	SWITCH_MODE_PUSH_LB,
	SWITCH_MODE_POP_RB,
	SWITCH_MODE_PUSH_RB,
	SWITCH_MODE_POP_LEFT,
	SWITCH_MODE_PUSH_LEFT,
	SWITCH_MODE_POP_RIGHT,
	SWITCH_MODE_PUSH_RIGHT,
	SWITCH_MODE_POP_TOP,
	SWITCH_MODE_PUSH_TOP,
	SWITCH_MODE_POP_BOTTOM,
	SWITCH_MODE_PUSH_BOTTOM,
	SWITCH_MODE_SIZE,
};

typedef enum
{
	TP_COLOR_BAR_H 	= 0x00,
	TP_COLOR_BAR_V 	= 0x01,
	TP_GRAY_BAR_H 	= 0x02,
	TP_GRAY_BAR_V 	= 0x03,
	TP_MANUAL_COLOR = 0x04,
	TP_AUTO_COLOR 	= 0x08,
	TP_ROLL_LEFT 	= 0x05,
	TP_ROLL_RIGHT 	= 0x85,	
	TP_ROLL_DOWN 	= 0x06,
	TP_ROLL_UP 		= 0x86,
	TP_ROLL_UPLEFT  = 0x07,
	TP_ROLL_DOWNRIGHT = 0x87,
} TPmode_TypeDef;

typedef enum
{
	ROLL_WHITE,
	ROLL_YELLOW,
	ROLL_SKY_BLUE,
	ROLL_GREEN,
	ROLL_PINK,
	ROLL_RED,
	ROLL_BLUE,
	ROLL_BLACK,
} RollColor_TypeDef;



typedef enum
{
	SWITCH_MODE_FULL = 0x00,
	SWITCH_MODE_FULL_ALPHA = 0x01,
	SWITCH_MODE_WIPE = 0x02,
	SWITCH_MODE_WIPE_ALPHA = 0x03,
} PreampSwitchMode_TypeDef;


typedef enum
{
	EDID_SOURCE_RGBLINK_DVI = 0,
	EDID_SOURCE_RGBLINK_HDMI = 1,
	EDID_SOURCE_RGBLINK_VGA = 2,
	EDID_SOURCE_RGBLINK_DP = 3,
	EDID_SOURCE_OUTPUT1 = 4,
	EDID_SOURCE_OUTPUT2 = 5,
	EDID_SOURCE_OUTPUTVGA = 6,
	EDID_SOURCE_FOLLOW = 7,
	EDID_SOURCE_CUSTOM = 8,
	EDID_SOURCE_SIZE,

}EDID_SourcePort_Typedef;

typedef enum
{
	//EDID_DES_PORT_VGA = 0,
	EDID_DES_PORT_DVI = 0,
	EDID_DES_PORT_DP  = 1,   //针对主板V1.3及以上
	EDID_DES_PORT_HDMI  = 2,
	EDID_DES_PORT_HDMI2  = 3,
	EDID_DES_PORT_M1 = 4,//2,
	EDID_DES_PORT_M2 = 5,//3,
	EDID_DES_PORT_M3 = 6,//4,
	EDID_DES_PORT_M4 = 7,//4,
	EDID_DES_PORT_SIZE,
}EDID_DesPort_Typedef;

typedef struct
{
	char sourcePort; //0- dvi  1- hdmi  2- vga 3 -output port 1   4-output port 2    5-follow output format   6-custom
	char desPort;    //0- dvi in mainboard  1-ext7  2- ext8  3- ext9 4- VGA
}EDIDManageStruct;

typedef struct 
{
	/* data */
	WORD pixelClock;
	WORD hActive;
	WORD vActive;
	WORD hBlank;
	WORD vBlank;
	WORD hSyncOffset;
	WORD vSyncOffset;
	WORD hSyncWidth;
	WORD vSyncWidth;
	BYTE hvPolarity; //bit0 -- h sync  bit1 -- v sync,  1 -- 姝? 0 -- 璐? bit7 == 1 interlace

} edid_timming_type;

char getPreviewSourceInput(void);
char getTakeOperationFinishedFlag(void);
char getSourceNeedTake(void);
char getEDIDSourcePort(void);
char studyEDID(char studyFrom,char writeTo); //studyFrom: 0-GBLINK DVI   1-GBLINK HDMI  2-输出口1  3-输出口2    writeTo:0- DVI1  1- ext7  2-ext8  3-ext9

bool GetHDMIoutEnableDataRange(int hdmix);
bool GetHDMIoutIsHDMI(int hdmix);
bool GetHDMIoutEnableDE(int hdmix);

BYTE GetOutputFormat(void);
BYTE GetOutputFormatIndex(void);
BYTE getVideoProcessEnable(void);
BYTE GetInputSource(void);
BYTE rgb_GetTTLInterlaveMode(void);      //  0  -- no adjust, 1 -- even = odd+1, 2 -- odd = even +1, 3 -- odd = even
BYTE rgb_GetHDMIInterlaveMode(void);      //  0  -- no adjust, 1 -- even = odd+1, 2 -- odd = even +1, 3 -- odd = even
BYTE getFanControlAutoDutyEnable(void);
BYTE getFanControlDutyRatio(void);
BYTE getVGAMode(BYTE port);
BYTE getScaleKeyType(void);
BYTE getMapKeyType(void);
BYTE getRealTimeSaveEnable(void);
BYTE rgb_GetFixedSplitIndex(void);
BYTE rgb_GetFixedSplitHNum(void);
BYTE rgb_GetFixedSplitVNum(void);
BYTE rgb_GetFixedSplitType(void);
BYTE getExtInputTypeConfig(BYTE board);
BYTE getHotBackupDetials(BYTE cnt);
//BYTE getHotBackupDetials(BYTE cnt)



int rgb_GetCropDot(int index); // 0 -- up, 1 -- down, 2 -- up/down, 3 -- left, 4 -- right, 5 -- left/right, 6 -- center
int rgb_getCropDotInput(char index);
int GetVgaPosX(void);
int GetVgaPosY(void);
int GetVgaClock(void);
int GetVgaPhase(void);
int GetAlpha(void);
int GetSwitchAB(void);
int  GetFrameLockSource(void);
int  GetFrameLockMode(void);
int rgb_getHActiveValue(void);
int rgb_getVActiveValue(void);
int rgb_getFreqValue(void);
int rgb_getHTotalValue(void);
int rgb_getVTotalValue(void);
int rgb_getsdiIndex_TTL(void);//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
int rgb_GetInputColorSpace_TTL(void);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
int GetDisplayMode(void); // 0 - none, 1 - live video, 2 - freeze, 3 - falt, 4 - test pattern
int ReadDisplayMode(void);   // 0 - none, 1 - live video, 2 - freeze, 3 - falt, 4 - test pattern
int getChangeInputSourceFlag(void);
int getFormatNeedTake(void);
int rgb_getSTRegister(int type);
int rgb_getSTVideoValue(int type);
int getEDIDDesPort(void);
int rgb_getCropSplit(char index);
int GetOutputPort1Config(void);  // 0- output  1- dvi loop        //just for mainboard >= V1.3
int GetHDMIoutDE_TOP(int hdmix);
int GetHDMIoutDE_DLY(int hdmix);
int GetHDMIoutDE_WIDTH(int hdmix);
int GetHDMIoutDE_HEIGHT(int hdmix);
int GetHDMIoutBitWidth(int hdmix);
int GetHDMIoutHsVs(int hdmix);
int GetHDMIoutHs(int hdmix);
int GetHDMIoutVs(int hdmix);
int  rgb_GetOutputColorSpace(void); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
int  GetPipMode(void);
int GetLanguage(void);
int GetDHCPEnable(void);
int  GetNetWorkEnable(void);
int GetArtNetUniverse(void);
int GetArtNetStartAddr(void);
int GetScaleWidth(void);
int GetScaleHeight(void);
int GetScalePosX(void);
int GetScalePosY(void);
int rgb_getVActiveValueByFormat(void);
int rgb_getHActiveValueByFormat(void);
unsigned int stCheckSum(BYTE *buffer, int size);

WORD getCustomEDIDH(void);
WORD getCustomEDIDV(void);
WORD getCustomV(void);
WORD rgb_GetInputHStart(void);
WORD rgb_GetInputVStart(void);
WORD rgb_GetInputHAct(void);
WORD rgb_GetInputVAct(void);

u16 GetWidthByFormat(u8 format);
u16 GetHeightByFormat(u8 format);

u32  rgb_GetTotalWorkTime(void);
u32  rgb_GetBootTimes(void);
u32 rgb_getLastWorkTime(void);
u32 GetWorkTime(void);   // how many second

bool GetSignalStatus(void);


void SetSignalStatus(bool status);
void taskRefleshVideoInfo(void);
void DisableVideoProcessDevice(void);
void EnableVideoProcessDevice(void);
void ResetVideoProcessDevice(void);
void SetSetWidthEnable(u8 enable);
void ResetFactory(void);
void LoadSaveSetting(int index);
void ResetIP(void);
void rgb_SetOutputColorSpace(int value); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
void rgb_SetInterlaceSwitch(int value); // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
void rgb_SetImageEnhance(int value); //0--特效切换   1--所有通道切换到主通道,当前信号立即切换到主通道
void rgb_SetHDMIColorSpaceExhance(char enable);
void rgb_SetSplitSetting(void);
void rgb_resetSplitParameter(void);
void rgb_SetSplitOnOff(int value);
void rgb_SetSplitSyncMode(int value);
void rgb_SetSplitHTotal(int value);
void rgb_SetSplitVTotal(int value);
void rgb_SetSplitHSize(int value);
void rgb_SetSplitVSize(int value);
void rgb_SetSplitHPos(int value);
void rgb_SetSplitVPos(int value);
void rgb_SetTextSetting(void);
void rgb_SetTextPreset(int preset);
void setPipModeInit(void);
void SetPipMode(int value);
void SetSwapMainPipInputSource(void);
void SetTopWindow(int value); // 0 -- pip on top, 1 -- main on top
void setPipSource(int value);
void SetLanguage(int value);
void SetDHCPEnable(int enable);
void SetArtNetUniverse(int universe);
void SetArtNetStartAddr(int startAddr);
void SetOutputOffset(int x, int y);
void SetScaleWidth(int value);
void SetScaleWidth_confirm(int value);
void SetScaleHeight(int value);
void SetScaleHeight_Confirm(int value);
void SetScaleWidthAndHeight(int value);
void SetScalePosX(int value);
void SetScalePosX_Confirm(int value);
void SetScalePosY(int value);
void SetScalePosY_Confirm(int value);
void ResetScaleSetting(int window);
void setSendCardScale(void);
void SetScreenWidth(int value);
void SetScreenHeight(int value);
void SetScreenPosX(int value);
void SetScreenPosY(int value);
void SetCrop(int x, int y, int w, int h);
void SetCropDotSingle(int index,int value); //	0 -- width,1-- height,2-- x, 3 -- y,
void SetCropDot(int x, int y, int w, int h);
void rgb_SetCropDotValue(int index, int value); // 0 -- up, 1 -- down, 2 -- up/down, 3 -- left, 4 -- right, 5 -- left/right, 6 -- center
void rgb_SetCropDot(int index, int value); // 0 -- up, 1 -- down, 2 -- up/down, 3 -- left, 4 -- right, 5 -- left/right, 6 -- center
void resetCropDot(void);
void rgb_getCrop(void);
void rgb_setCrop(u16 x, u16 y, u16 width, u16 height);
void rgb_setCropSingle(int index,int value);//  0 -- width,1-- height,2-- x, 3 -- y,
void rgb_doCropSingle(void);
void rgb_setCropDotSingle(int index,int value);//  0 -- width,1-- height,2-- x, 3 -- y,
void rgb_doCropDotSingle(void);
void rgb_setCropDotAll(u16 x, u16 y, u16 width, u16 height);
void rgb_getCropDotAll(void);
void rgb_setCropDotInput(char index,int value);//  0 -- width,1-- height,2-- x, 3 -- y,4 -- do it
void rgb_setCropDotInputAll(u16 x, u16 y, u16 width, u16 height);
void setWidthScale(u8 value);
void SetScreenMode(u8 value);
void SetColorTempMode(int mode);
void SetColorTemp(u8 bCont,int r, int g, int b);
void SetContrast(u8 bCont,int r, int g, int b);
void SetBrightness(u8 bCont,int r, int g, int b);
void SetSharpness(u8 bCont,int value);
void SetSaturation(u8 bCont,int value);
void SetVgaPosX(int value);
void SetVgaPosY(int value);
void SetVgaClock(int value);
void SetVgaPhase(int value);
void SetVgaAutoAdjust(void);
void SetGamma(int value);
void SetAlpha(int value);
void SetSwitchMode(int value);
void SetFadeTime(int value);
void SetSwitchAB(int value);
void SetSwitchType(int value);
void SetPreviewDblTapSwitch(int value);
void SetPreviewSwitch(int value);
void SetAdcAutoAdjust(void);
void ResetAdcSetting(void);
void SetFrameLockMode(int value);
void SetFrameLockSource(int value);
void SetDisplayMode(int mode); // 0 - none, 1 - live video, 2 - freeze, 3 - falt, 4 - test pattern
void rgb_setsdiIndex_TTL(int value);//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
void rgb_ComSetInputHStart(WORD type, WORD value);
void rgb_ComSetInputVStart(WORD type, WORD value);
void rgb_ComSetTTLInterlaveMode(WORD type, WORD value);
void rgb_ResetSdiInput(void);
void rgb_SetInputHStart(WORD value);
void rgb_SetInputVStart(WORD value);
void rgb_SetInputHAct(WORD value);
void rgb_SetInputVAct(WORD value);
void SetTestPatternSwitch(int mode);
void SetTestPatternMode(int mode); // 1 ~ 66
void SetTestPatternColor(int red, int green, int blue);
void SetTestPatternParam(int horStep, int verStep, int stepColor);
void SetFlatColor(int red, int green, int blue);
void SetCurrentWindow(int mainOrPip);
void SetZoom(int x, int y, int w, int h);
void SetScaleSetting(void);
void SetWindowRatio(int ratio); // 0 -- 4:3, 1--16:9, 2 -- Normal
void taskSetDisplayModeLive(void);
void taskSaveSource(void);
void checkExtCrop(u8 extPort,u8 doAtOnceEnable);
void setChangeInputSourceFlag(int value);
void checkSourceNeedTake(void);   //只在开机初始化的时候调用
void setPausePort(char port);
void SetPreviewSource(BYTE source);
void SetProgramSource(BYTE source,BYTE port);
void SetInputSource(BYTE source);
void setInputSourceToMainAndDisp(BYTE source); //将source输入到main_Window,并显示main_Window
void setInputSourceToPIPAndDisp(BYTE source); //将source输入到pip_window,并显示pip_window
void doTakeOperation(void);
void SetOutputFormat(BYTE format);
void rgb_updateColorSpace(void);
void setCustomH(int value);
void setCustomV(int value);
void setCustomF(int value);
void editCustomEDID(VideoDefine_Struct formatTiming,Resolution_Struct formatResolution,BYTE inputPort);
void rgb_SetTTLInterlaveMode(BYTE type); //  0  -- no adjust, 1 -- even = odd+1, 2 -- odd = even +1, 3 -- odd = even
void rgb_SetHDMIInterlaveMode(BYTE type); //  0  -- no adjust, 1 -- even = odd+1, 2 -- odd = even +1, 3 -- odd = even
void SetAudioMute(int onOff);
void SetAudioMode(int value);
void SetHdmiAudio(int value);
void checkDVIAndExtAudio(void);
void SetDVIAndEXTAudio(int value);  	// 0 -- internal, 1 -- external
void SetDVIAndEXTAudio_notSave(int value);  	// 0 -- internal, 1 -- external
void SetDPAudio(int value);  	// 0 -- internal, 1 -- external
void SetAudioVolume(int value);
void LoadSave(int index);
void setHotBackupEnable(BYTE enable);
//void setHotBackupDetials(BYTE cnt,BYTE source);

void task_HotBackup(void);
void setFanControlAutoDutyEnable(bool enable);
void setFanControlDutyRatio(char value);
void rgb_GetBootList(BYTE *buffer, int address, int size);
void rgb_ClearBootList(void);
void task_workTime(void);  // ervery second call
void setVGAMode(BYTE port,BYTE value); //port 0-vga in mainboard       value: 0-vga  1-ypbpr
void setBlackKeyType(BYTE value);
void setScaleKeyType(BYTE value);
void setMapKeyType(BYTE value);
void task_led_control(void);
void task_testDispMenu(void);  //for test
void TaskControl(void);
void setWaitSelectToVideo(char value);
void taskSelectParamToVideo(void);
void setEDIDSourcePort(char value);
void setEDIDDesPort(int value);
void refleshEDIDSourcePort(void);
void resetEDID(int index); 
void SetTextAbove(char value);
void SetTextAndOr(char value);
void SetTextEnable(char value);
void SetTextMode(char value);
void SetTextAlpha(char value);
void SetTextRed(char value);
void SetTextGreen(char value);
void SetTextBlue(char value);
void SetTextPreset(char value);
void SetOutputPort1Config(int value);  // 0- output  1- dvi loop        //just for mainboard >= V1.3
void setOutputSize(u8 format);
void task_InterlaceMonitor(void);
void CreateColorLightEdidTable(int width, int height, int freq);
void SetColorLight_EDID(int width,int height);
void WriteEdid(int index);
void SetHDMIoutEnableDE(int hdmix, bool enable);
void SetHDMIoutDE_TOP(int hdmix, int DE_TOP);
void SetHDMIoutDE_DLY(int hdmix, int DE_DLY);
void SetHDMIoutDE_WIDTH(int hdmix, int DE_WIDTH);
void SetHDMIoutDE_HEIGHT(int hdmix, int DE_HEIGHT);
void SetHDMIoutHsVs(int hdmix, int HsVs);
void SetHDMIoutHs(int hdmix,int value);
void SetHDMIoutVs(int hdmix,int value);
void SetHDMIoutEnableHDMI(int hdmix, bool enable);
void CheckHDMIOutEnable_X1(void);
void SetHDMIoutEnableDataRange(int hdmix, bool enable);
void SetHDMIoutBitWidth(int hdmix, int bitWidth);
void setRealTimeSaveEnable(bool enable);
void rgb_SetFixedSplitIndex(BYTE value);
void rgb_SetFixedSplitHNum(BYTE value);
void rgb_SetFixedSplitVNum(BYTE value);
void rgb_SetFixedSplitType(BYTE value);
void rgb_setSplitCropEnable(bool enable);
void rgb_setCropSplit(char index,int value);//  0 -- width,1-- height,2-- x, 3 -- y
void rgb_setCropSplitAll(void);
void rgb_enableCropSplit(void);
void rgb_disableCropSplit(void);
void setExtInputTypeConfig(BYTE board,BYTE value);
void SetScreeFollowScaleEn(char value);
char GetScreeFollowScaleEn(void);


extern char GetCurrentEnterNumberStatus(void);
extern char GetPrvEnterNumberStatus(void);

extern void SetSplitParamToFpga(PanelSplitParamSt* panelSt);
extern void SetDisplayAlpha(u8 alpha);
extern void SetFpgaOutputFormat(BYTE format,u8 index);
extern void SetOutputFormatCustom(void);
extern void SetCurrentEnterNumberStatus(char status);
extern void SetPrvEnterNumberStatus(char status);



int rgb_getSTRegister(int type);
int rgb_getSTVideoValue(int type);
int getEDIDDesPort(void);
int rgb_getCropSplit(char index);
int GetOutputPort1Config(void);  // 0- output  1- dvi loop        //just for mainboard >= V1.3
int GetHDMIoutDE_TOP(int hdmix);
int GetHDMIoutDE_DLY(int hdmix);
int GetHDMIoutDE_WIDTH(int hdmix);
int GetHDMIoutDE_HEIGHT(int hdmix);
int GetHDMIoutBitWidth(int hdmix);
int GetHDMIoutHsVs(int hdmix);
int GetHDMIoutHs(int hdmix);
int GetHDMIoutVs(int hdmix);
int  rgb_GetOutputColorSpace(void); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
int  GetPipMode(void);
int GetLanguage(void);
int GetDHCPEnable(void);
int  GetNetWorkEnable(void);
int GetArtNetUniverse(void);
int GetArtNetStartAddr(void);
int GetScaleWidth(void);
int GetScaleHeight(void);
int GetScalePosX(void);
int GetScalePosY(void);
int rgb_getVActiveValueByFormat(void);
int rgb_getHActiveValueByFormat(void);
unsigned int stCheckSum(BYTE *buffer, int size);

WORD getCustomEDIDH(void);
WORD getCustomEDIDV(void);

WORD getCustomV(void);
WORD rgb_GetInputHStart(void);
WORD rgb_GetInputVStart(void);
WORD rgb_GetInputHAct(void);
WORD rgb_GetInputVAct(void);

u16 GetWidthByFormat(u8 format);
u16 GetHeightByFormat(u8 format);

u32  rgb_GetTotalWorkTime(void);
u32  rgb_GetBootTimes(void);
u32 rgb_getLastWorkTime(void);
u32 GetWorkTime(void);   // how many second


void taskRefleshVideoInfo(void);
void DisableVideoProcessDevice(void);
void EnableVideoProcessDevice(void);
void ResetVideoProcessDevice(void);
void SetSetWidthEnable(u8 enable);
void ResetFactory(void);
void LoadSaveSetting(int index); 
void ResetIP(void);
void rgb_SetOutputColorSpace(int value); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
void rgb_SetInterlaceSwitch(int value); // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
void rgb_SetImageEnhance(int value); //0--特效切换   1--所有通道切换到主通道,当前信号立即切换到主通道
void rgb_SetHDMIColorSpaceExhance(char enable);
void rgb_SetSplitSetting(void);
void rgb_resetSplitParameter(void);
void rgb_SetSplitOnOff(int value);
void rgb_SetSplitSyncMode(int value);
void rgb_SetSplitHTotal(int value);
void rgb_SetSplitVTotal(int value);
void rgb_SetSplitHSize(int value);
void rgb_SetSplitVSize(int value);
void rgb_SetSplitHPos(int value);
void rgb_SetSplitVPos(int value);
void rgb_SetTextSetting(void);
void rgb_SetTextPreset(int preset);
void setPipModeInit(void);
void SetPipMode(int value);
void SetSwapMainPipInputSource(void);
void SetTopWindow(int value); // 0 -- pip on top, 1 -- main on top
void setPipSource(int value);
void SetLanguage(int value);
void SetDHCPEnable(int enable);
void SetArtNetUniverse(int universe);
void SetArtNetStartAddr(int startAddr);
void SetOutputOffset(int x, int y);
void SetScaleWidth(int value);
void SetScaleWidth_confirm(int value);
void SetScaleHeight(int value);
void SetScaleHeight_Confirm(int value);
void SetScaleWidthAndHeight(int value);
void SetScalePosX(int value);
void SetScalePosX_Confirm(int value);
void SetScalePosY(int value);
void SetScalePosY_Confirm(int value);
void ResetScaleSetting(int window);
void setSendCardScale(void);
void SetScreenWidth(int value);
void SetScreenHeight(int value);
void SetScreenPosX(int value);
void SetScreenPosY(int value);
void SetCrop(int x, int y, int w, int h);
void SetCropDotSingle(int index,int value); //	0 -- width,1-- height,2-- x, 3 -- y,
void SetCropDot(int x, int y, int w, int h);
void rgb_SetCropDotValue(int index, int value); // 0 -- up, 1 -- down, 2 -- up/down, 3 -- left, 4 -- right, 5 -- left/right, 6 -- center
void rgb_SetCropDot(int index, int value); // 0 -- up, 1 -- down, 2 -- up/down, 3 -- left, 4 -- right, 5 -- left/right, 6 -- center
void resetCropDot(void);
void rgb_getCrop(void);
void rgb_setCrop(u16 x, u16 y, u16 width, u16 height);
void rgb_setCropSingle(int index,int value);//  0 -- width,1-- height,2-- x, 3 -- y,
void rgb_doCropSingle(void);
void rgb_setCropDotSingle(int index,int value);//  0 -- width,1-- height,2-- x, 3 -- y,
void rgb_doCropDotSingle(void);
void rgb_setCropDotAll(u16 x, u16 y, u16 width, u16 height);
void rgb_getCropDotAll(void);
void rgb_setCropDotInput(char index,int value);//  0 -- width,1-- height,2-- x, 3 -- y,4 -- do it
void rgb_setCropDotInputAll(u16 x, u16 y, u16 width, u16 height);
void setWidthScale(u8 value);
void SetScreenMode(u8 value);
void SetColorTempMode(int mode);
void SetColorTemp(u8 bCont,int r, int g, int b);
void SetContrast(u8 bCont,int r, int g, int b);
void SetBrightness(u8 bCont,int r, int g, int b);
void SetSharpness(u8 bCont,int value);
void SetSaturation(u8 bCont,int value);
void SetVgaPosX(int value);
void SetVgaPosY(int value);
void SetVgaClock(int value);
void SetVgaPhase(int value);
void SetVgaAutoAdjust(void);
void SetGamma(int value);
void SetAlpha(int value);
void SetSwitchMode(int value);
void SetFadeTime(int value);
void SetSwitchAB(int value);
void SetSwitchType(int value);
void SetPreviewDblTapSwitch(int value);
void SetPreviewSwitch(int value);
void SetAdcAutoAdjust(void);
void ResetAdcSetting(void);
void SetFrameLockMode(int value);
void SetFrameLockSource(int value);
void SetDisplayMode(int mode); // 0 - none, 1 - live video, 2 - freeze, 3 - falt, 4 - test pattern
void rgb_setsdiIndex_TTL(int value);//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
void rgb_ComSetInputHStart(WORD type, WORD value);
void rgb_ComSetInputVStart(WORD type, WORD value);
void rgb_ComSetTTLInterlaveMode(WORD type, WORD value);
void rgb_ResetSdiInput(void);
void rgb_SetInputHStart(WORD value);
void rgb_SetInputVStart(WORD value);
void rgb_SetInputHAct(WORD value);
void rgb_SetInputVAct(WORD value);
void SetTestPatternSwitch(int mode);
void SetTestPatternMode(int mode); // 1 ~ 66
void SetTestPatternColor(int red, int green, int blue);
void SetTestPatternParam(int horStep, int verStep, int stepColor);
void SetFlatColor(int red, int green, int blue);
void SetCurrentWindow(int mainOrPip);
void SetZoom(int x, int y, int w, int h);
void SetScaleSetting(void);
void SetWindowRatio(int ratio); // 0 -- 4:3, 1--16:9, 2 -- Normal
void taskSetDisplayModeLive(void);
void taskSaveSource(void);
void checkExtCrop(u8 extPort,u8 doAtOnceEnable);
void setChangeInputSourceFlag(int value);
void checkSourceNeedTake(void);   //只在开机初始化的时候调用
void setPausePort(char port);
void SetPreviewSource(BYTE source);
void SetProgramSource(BYTE source,BYTE port);
void SetInputSource(BYTE source);
void setInputSourceToMainAndDisp(BYTE source); //将source输入到main_Window,并显示main_Window
void setInputSourceToPIPAndDisp(BYTE source); //将source输入到pip_window,并显示pip_window
void doTakeOperation(void);
void SetOutputFormat(BYTE format);
void rgb_updateColorSpace(void);
void setCustomH(int value);
void setCustomV(int value);
void setCustomF(int value);
void editCustomEDID(VideoDefine_Struct formatTiming,Resolution_Struct formatResolution,BYTE inputPort);
void rgb_SetTTLInterlaveMode(BYTE type); //  0  -- no adjust, 1 -- even = odd+1, 2 -- odd = even +1, 3 -- odd = even
void rgb_SetHDMIInterlaveMode(BYTE type); //  0  -- no adjust, 1 -- even = odd+1, 2 -- odd = even +1, 3 -- odd = even
void SetAudioMute(int onOff);
void SetAudioMode(int value);
void SetHdmiAudio(int value);
void checkDVIAndExtAudio(void);
void SetDVIAndEXTAudio(int value);  	// 0 -- internal, 1 -- external
void SetDVIAndEXTAudio_notSave(int value);  	// 0 -- internal, 1 -- external
void SetDPAudio(int value);  	// 0 -- internal, 1 -- external
void SetAudioVolume(int value);
void LoadSave(int index);
void setHotBackupEnable(BYTE enable);
void setHotBackupDetials(BYTE cnt,BYTE source);
void task_HotBackup(void);
void setFanControlAutoDutyEnable(bool enable);
void setFanControlDutyRatio(char value);
void rgb_GetBootList(BYTE *buffer, int address, int size);
void rgb_ClearBootList(void);
void task_workTime(void);  // ervery second call
void setVGAMode(BYTE port,BYTE value); //port 0-vga in mainboard       value: 0-vga  1-ypbpr
void setBlackKeyType(BYTE value);
void setScaleKeyType(BYTE value);
void setMapKeyType(BYTE value);
void task_led_control(void);
void task_testDispMenu(void);  //for test
void TaskControl(void);
void setWaitSelectToVideo(char value);
void taskSelectParamToVideo(void);
void setEDIDSourcePort(char value);
void setEDIDDesPort(int value);
void refleshEDIDSourcePort(void);
void resetEDID(int index); 
void SetTextAbove(char value);
void SetTextAndOr(char value);
void SetTextEnable(char value);
void SetTextMode(char value);
void SetTextAlpha(char value);
void SetTextRed(char value);
void SetTextGreen(char value);
void SetTextBlue(char value);
void SetTextPreset(char value);
void SetOutputPort1Config(int value);  // 0- output  1- dvi loop        //just for mainboard >= V1.3
void setOutputSize(u8 format);
void task_InterlaceMonitor(void);
void CreateColorLightEdidTable(int width, int height, int freq);
void SetColorLight_EDID(int width,int height);
void WriteEdid(int index);
void SetHDMIoutEnableDE(int hdmix, bool enable);
void SetHDMIoutDE_TOP(int hdmix, int DE_TOP);
void SetHDMIoutDE_DLY(int hdmix, int DE_DLY);
void SetHDMIoutDE_WIDTH(int hdmix, int DE_WIDTH);
void SetHDMIoutDE_HEIGHT(int hdmix, int DE_HEIGHT);
void SetHDMIoutHsVs(int hdmix, int HsVs);
void SetHDMIoutHs(int hdmix,int value);
void SetHDMIoutVs(int hdmix,int value);
void SetHDMIoutEnableHDMI(int hdmix, bool enable);
void CheckHDMIOutEnable_X1(void);
void SetHDMIoutEnableDataRange(int hdmix, bool enable);
void SetHDMIoutBitWidth(int hdmix, int bitWidth);
void setRealTimeSaveEnable(bool enable);
void rgb_SetFixedSplitIndex(BYTE value);
void rgb_SetFixedSplitHNum(BYTE value);
void rgb_SetFixedSplitVNum(BYTE value);
void rgb_SetFixedSplitType(BYTE value);
void rgb_setSplitCropEnable(bool enable);
void rgb_setCropSplit(char index,int value);//  0 -- width,1-- height,2-- x, 3 -- y
void rgb_setCropSplitAll(void);
void rgb_enableCropSplit(void);
void rgb_disableCropSplit(void);
void setExtInputTypeConfig(BYTE board,BYTE value);
void SetScreeFollowScaleEn(char value);
char GetScreeFollowScaleEn(void);

extern void SetFreeze(void);
extern char GetCurrentEnterNumberStatus(void);
extern char GetPrvEnterNumberStatus(void);

//extern void SetSplitParamToFpga(PanelSplitParamSt* panelSt);
extern void SetDisplayAlpha(u8 alpha);
extern void SetFpgaOutputFormat(BYTE format,u8 index);
extern void SetOutputFormatCustom(void);
extern void SetCurrentEnterNumberStatus(char status);
extern void SetPrvEnterNumberStatus(char status);


#endif

