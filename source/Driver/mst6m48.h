#ifndef _PREAMP_H_
#define _PREAMP_H_

#include "define.h"

#ifdef USE_STDP8028

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

typedef enum 
{
	interpret_cmd_init,
	interpret_cmd_start,
	interpret_cmd_data,
	interpret_cmd_end,
} InterpretCmdState_TypeDef;

typedef struct 
{
	u8 inputFormat[2];
	BYTE inputSource[2];
	BYTE ChSource[2];
	BYTE switchTo;
	BYTE currentCh;
	u8 outputColorSpace;
	u8 ver_msb;
	u8 ver_lsb;
	u8 sn_msb;
	u8 sn_lsb;
	u8 enableDisplay;
	u8 deviceState;
	u8 updateState; // 0(start），1(updating),  2(success),  3(fail)
	u8 frameLockMode;
	u8 frameLockSource;
	WORD inputTimming[4];
	WORD vLockLine;
	WORD cropSplitEnable;
	WORD cropSplit[4];//  0 -- width,1-- height,2-- x, 3 -- y, 
	WORD regValue;
	WORD zoom[4]; // 0-x 1-y 2-width 3-height
	WORD crop[4]; // 0-x 1-y 2-width 3-height
	WORD cropDot[4]; // 0-x 1-y 2-width 3-height
	BYTE brightness[2][3];
	BYTE contrast[2][3];
	BYTE colorTemp[2][3];
	BYTE sharpness[2];
	BYTE saturation[2];
	BYTE displayMode;
	BYTE HDMIInputAudio;
	BYTE TTLInputAudio;
	WORD dat1Value;
	WORD dat2Value;
	WORD dat3Value;
	WORD dat4Value;
	BYTE switchMode;
} Mst6m48_Setting_TypeDef;

typedef enum
{
	DEVICE_INIT = 1,
	DEVICE_READY,
} DeviceState_TypeDef;

typedef enum
{
	INPUT_CV3 , // 0,
	INPUT_SDI , // 1,
	INPUT_VGA2, // 2,
	INPUT_VGA ,      // 3,
	INPUT_DVI ,      // 4,     //MATRIX_OUTPUT_CORE_DVI
	INPUT_YCbCr,     // 5,
	INPUT_TTL,       // 6,    //MATRIX_OUTPUT_CORE_TTL
	INPUT_HDMI,      // 7,     //X1_Pro_HDMI
	INPUT_CV1,       //  8,
	INPUT_CV2,       //  9,
	INPUT_DP,        // 10,
	INPUT_YPbPr,       //  11,
	INPUT_YCbCr2,    //  12,
	INPUT_YPbPr2,      //  13,   
	INPUT_SV,        //  14,
	
	INPUT_SIZE,	
} InputSource_TypeDef;

void ForceCommFlag(void);

void init_mst6m48(void);
void task_mst6m48(void);
void Isr_receive_mst6m48(u8 value);
bool send_cmd_to_mst6m48(Command_TypeDef *cmdStruct);
bool send_cmd_to_mst6m48_and_wait(Command_TypeDef *cmdStruct);

Mst6m48_Setting_TypeDef * getMst6m48Setting(void);


void mst_resetFactory(void);
void mst_loadSetting(int index);
void mst_saveSetting(int index);
void mst_enableDisplay(u8 enable);
void mst_setScaleH(u16 position, u16 width);
void mst_setScaleV(u16 position, u16 height);
void mst_setScaleX(int value);
void mst_setScaleY(int value);
void mst_setScaleWidth(int value);
void mst_setScaleHeight(int value);
void mst_setScaleXSingle(int value);
void mst_setScaleYSingle(int value);
void mst_setScaleWidthSingle(int value);
void mst_setScaleHeightSingle(int value);

void mst_setScaleSetting(void);
void mst_setZoom(u16 x, u16 y, u16 width, u16 height);
#if 1
void mst_setCrop(u16 x, u16 y, u16 width, u16 height);
void mst_getCrop(void);
#else
void mst_setCrop(int index, int value); //  0 -- width,1-- height,2-- x, 3 -- y, 
int  mst_getCrop(int index); //  0 -- width,1-- height,2-- x, 3 -- y, 
#endif
#if 1
void mst_setCropDot(int index, int value); //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting
int  mst_getCropDot(int index); //  0 -- width,1-- height,2-- x, 3 -- y, 
#endif
#if 1
void mst_setCropDotCh(char ch); //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--enable save crop ratio
#endif

void mst_setCropSplitEnable(bool enable);
char mst_getCropSplitEnable(bool enable);
void mst_setCropSplitInput(int index, int value); //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting
int  mst_getCropSplitInput(int index); //  0 -- width,1-- height,2-- x, 3 -- y, 

void mst_setScreenH(u16 position, u16 width);
void mst_setScreenV(u16 position, u16 height);

void mst_setInputSource(u8 source);
void mst_setInputSource_Main(u8 source);
void mst_setInputSource_PIP(u8 source);
void mst_getInputSource(void);
int mst_getInputSource_Main(void);
int mst_getInputSource_pip(void);
int mst_getInputSourceCh(char ch);//ch 0-main 1-pip
void mst_setEnableExternMatrixCVBS(int enable);
void mst_setEnableTPSelf(int enable);
void mst_setInterlaceSwitch(int value); // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
BYTE mst_getInterlaceSwitch(void); // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面

void mst_waitDeviceReady(void);
void mst_setOutputFormat(u8 format);
BYTE mst_getOutputFormat(void);
void mst_setOutputMode(u8 mode);
void mst_SetOutputColorSpace(int value); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
int  mst_GetOutputColorSpace(void); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
void mst_setFrameLockMode(u8 mode);
int  mst_getFrameLockMode(void);
void mst_setFrameLockSource(u8 source);
int  mst_getFrameLockSource(void);

u8 mst_getInputFormat(void);
u8 mst_getInputFormatCh(BYTE ch);

void mst_setInputTimming(int type, WORD value);// type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct
WORD mst_getInputTimming(int type); // type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct
void mst_resetSdiInputtimming(void);

void mst_resetAllSdi(int type); // 0 -- FPGA SDI, 1 -- GS2971 SDI

void mst_setDisplayMode(u8 mode);
int mst_getDisplayMode(void);
void mst_setAutoBackGroundControlEn(u8 enable);
int mst_getAutoBackGroundControlEn(void);
void mst_setDisplayModeFreezeChannel(bool freeze,char channel);
void mst_setDisplayModeFreezeCurrent(bool freeze);
void mst_setDisplayModeFreezeTheOther(bool freeze);
void mst_setDisplayModeFreezeAll(bool freeze);

void mst_setFlatColor(u8 r, u8 g, u8 b);
void mst_setTestPatternMode(u8 mode);
void mst_setTestPatternColor(u8 r, u8 g, u8 b);
void mst_setTestPatternParam(u8 horStep, u8 verStep, u8 stepColor);


void mst_setBrightness(u8 red, u8 green, u8 blue);
void mst_setContrast(u8 red, u8 green, u8 blue);
void mst_setColorTemp(u8 red, u8 green, u8 blue);
void mst_setSharpness(u8 sharpness);//20110906
void mst_setAdcAutoAdjust(void);
void mst_ResetAdcSetting(void);



void mst_setSaturation(u8 saturation);//20110909
void mst_setPcX(u16 value);
void mst_setPcY(u16 value);
void mst_setPcClock(u16 clock);
void mst_setPcPhase(u16 phase);
void mst_setAutoAdjustVGA(void);

void mst_getPcX(void);
void mst_getPcY(void);
void mst_getPcClock(void);
void mst_getPcPhase(void);

void mst_getSoftVersion(void);
void mst_setSn(u16 sn);
void mst_getSn(void);

void mst_getDeviceState(void);

// audio
void mst_setAudioSource(u8 source);
void mst_setAudioMode(u8 mode);
void mst_setHdmiAudio(u8 value);
u8 mst_getHdmiAudio(void);
void mst_setDPAudio(u8 value);
void mst_setVolume(u8 value);
void mst_setMuteVolume(u8 value); // 0 -- mute off, 1 -- mute on
void mst_setttlInputAudio(u8 value);  //value: 1- audio1       2-audio2
u8 mst_getttlInputAudio(void);  //value: 1- audio1       2-audio2
void mst_setHdmiInputAudio(u8 value);  //value: 1- audio1       2-audio2
u8 mst_getHdmiInputAudio(void);  //value: 1- audio1       2-audio2
void mst_setAudioType(u8 value);

void mst_setWindowRatio(u8 value); // 0 -- 4:3, 1--16:9, 2 --Normal

void mst_setScreenMode(u8 value); // 0 -- full window, 1 -- screen window

void mst_setGamma(int value);
BYTE mst_getGamma(void);

int mst_getAlpha(void);
void mst_setAlpha(int value);
void mst_setSwitchMode(int value);
BYTE mst_getSwitchMode(void);
void mst_setFadeTime(int value);
BYTE mst_getFadeTime(void);
void mst_setSwitchAB(int value);
int mst_getSwitchAB(void);
int mst_getCurrentCh(void);

void mst_setPipMode(int value);
void mst_SwapMainPipInputSource(void);
void mst_setTopWindow(int value);
void mst_SetCurrentWindow(int value);

void mst_setTextEnableMode(int enable, int mode);
void mst_setTextAboveAndOr(int above, int AndOr);
void mst_setTextColor(int r, int g, int b);

void mst_setSplit(int onOff, int syncMode);
void mst_setSplitHTotal(int value);
void mst_setSplitVTotal(int value);
void mst_setSplitHSize(int value);
void mst_setSplitVSize(int value);
void mst_setSplitHPos(int value);
void mst_setSplitVPos(int value);
void mst_setVLockLine(int value);
int mst_getVLockLine(void);
void mst_setOutputOffset(int x, int y);



void mst_setCustomParameter(int type, WORD value); //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
int mst_getCustomParameter(int type); //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
int mst_getSTRegister(int type) ;
int mst_getSTVideoValue(int type);

void mst_setInputColorSpace_TTL(int value);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
int mst_getInputColorSpace_TTL(void);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2

void mst_updateColorSpace(void);
void mst_setEnableInternalHDCP(int enable);

int mst_getScaleX(void);
int mst_getScaleY(void);
int mst_getScaleWidth(void);
int mst_getScaleHeight(void);
void mst_getZoom(void);

int mst_getHActiveValue(void);
int mst_getFreqValue(void);
int mst_getVActiveValue(void);
int mst_getHTotalValue(void);
int mst_getVTotalValue(void);


Mst6m48_Setting_TypeDef *Get6m48Setting(void);

void mst_setsdiIndex_TTL(int value);
int mst_getsdiIndex_TTL(void);//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 


int mst_getInputSource_notSave(void);

BYTE mst_getTTLInterlaveMode(void);
void mst_setTTLInterlaveMode(BYTE type);

BYTE mst_getHDMInterlaveMode(void);
void mst_setHDMIInterlaveMode(BYTE type);

void mst_setScaleH_Long(u16 position, u16 width);

void mst_setLayerParam_Long(BYTE layer);//layer 0-main   1-pip
void mst_getInputParams_Long(void);
void mst_setTestParam(u8 dat1,u8 dat2,u8 dat3,u8 dat4);
void mst_setDeviceRegValue(int reg,int value);
int mst_getDeviceRegValue(int reg);


u8 getVideoStatus(void);

#endif		// #ifdef USE_STDP8028
#endif // _PREAMP_H_

