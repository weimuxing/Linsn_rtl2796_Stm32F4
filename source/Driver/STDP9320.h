#ifndef _STDP9320_H_
#define _STDP9320_H_

#include "define.h"
#include "rgblinkProtocol.h"
#include "control.h"

#ifdef USE_STDP9320
#if 0			// cancel by hibernate

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
#endif

typedef struct
{
	WORD HStart;
	WORD VStart;
	WORD HActive;
	WORD VActive;
	WORD VFreq;
}InputTimming_Type;

typedef enum
{
	Red,
	Green,
	Blue,
}ColorParam_Type;

typedef struct 
{
	u8 ver_msb;
	u8 ver_lsb;
	u8 sn_msb;
	u8 sn_lsb;
	u8 enableDisplay;
	u8 deviceState;
	u8 updateState; // 0(start），1(updating),  2(success),  3(fail)
	
	u8 inputFormat[2];
	u8 inputSource[2];
	u8 chSource[2];	
	u8 switchTo;
	u8 currentCh;
	u8 outputColorSpace;
	u8 frameLockMode;
	u8 frameLockSource;
	InputTimming_Type inputTimming[2];
	WORD vLockLine;
	u32 regValue;
	WORD zoom[4]; // 0-x 1-y 2-width 3-height
	WORD crop[4]; // 0-x 1-y 2-width 3-height
	WORD cropDot[4]; // 0-x 1-y 2-width 3-height
	WORD cropDot2[4]; // 0-x 1-y 2-width 3-height

	WORD scaler[2][4];    // 0-x 1-y 2-width 3-height
	
	u8 brightness[2][3];
	u8 contrast[2][3];
	u8 colorTemp[2][3];
	u8 sharpness[2];
	u8 saturation[2];
	u8 boardWidth;
	u8 boardHeight;
	u8 boardColor;
	u8 boardHighLight;
	u8 boardOnOff;
	
	u8 switchMode;

	u8 displayMode;
	
	edid_timming_type EDID[3];	// 0: DP  1: DVI 2:HDMI
	u8   DP_EDID[256];
	int  DP_EDID_Len;
	
	WORD width[3];
	WORD height[3];
	u8 freq[3];

	u8 splitOnOff;
	u8 outputFormat;
	u32 splitHTotal;
	u32 splitVTotal;
	u32 splitHSize;
	u32 splitVSize;
	u32 splitHPos;
	u32 splitVPos;	
} VIDEO_Setting_TypeDef;

typedef enum
{
	DEVICE_INIT = 1,
	DEVICE_READY,
} DeviceState_TypeDef;

typedef enum
{
	#if 1
	INPUT_CV3 , // 0,
	INPUT_SDI , // 1,
	INPUT_VGA2 , // 2,
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
	#else
	INPUT_RGB,
	INPUT_YUV,
	INPUT_TTL,
	INPUT_DVIS,
	INPUT_DVID,
	INPUT_HDMI,
	INPUT_DP,

	INPUT_CV1,                     // 0
	INPUT_SV,                      // 1
	INPUT_SDI,                     // 2 
	INPUT_DVI,					   // 3
	INPUT_YPbPr,                   // 4
	INPUT_VGA,                     // 5
	INPUT_CV2,                     // 6
	INPUT_CV3,                     // 7
	INPUT_YCbCr,                   // 8
//	INPUT_DP,                      // 9
//	INPUT_TTL,                     // 10
	INPUT_YPbPr2,                  // 11
//	INPUT_HDMI,       			   // 12
	INPUT_VGA2,	                   // 13
	INPUT_YCbCr2,	               // 14
	#endif
	INPUT_SIZE,	
} InputSource_TypeDef;

BYTE getSourceToDevice(BYTE source);

void ForceCommFlag(void);
u8 getVideoStatus(void);
void init_stdp9320(void);

void task_video(void);
void Isr_receive_video(u8 value);
void pushVideoCommand(Command_TypeDef * cmd);
void taskDelayVideoCommand(void);
void pushVideoMsg(u8 type,int param);
void taskDelayVideoMsg(void);


bool send_cmd_to_video(Command_TypeDef *cmdStruct);
bool send_cmd_to_video_and_wait(Command_TypeDef *cmdStruct);
bool send_cmd_to_mst6m48_and_wait_ack_data(Command_TypeDef *cmdStruct);

VIDEO_Setting_TypeDef * getVideoSetting(void);


void video_getSoftVersion(void);
void video_setSn(u16 sn);
void video_getSn(void);
void video_enableDisplay(u8 enable);
void video_resetFactory(void);
void video_loadSetting(char index);// 1 ~ 10
void video_saveSetting(int index); // 1 ~ 10


void video_setInputSource(u8 source);
void video_setInputSource_ch(u8 ch,u8 source);
void video_setInputSource_Main(u8 source);
void video_setInputSource_PIP(u8 source);
int video_readInputSource(void);
int video_readInputSourceCh(char ch);
void video_getInputSource(void);
int video_getInputSourceNew(int ch);
int video_getInputSource_Main(void);
int video_getInputSource_pip(void);
int video_getInputSourceCh(char ch);//ch 0-main 1-pip
int video_getInputSource_notSave(void);

void video_setScaleWidth(int value);
void video_setScaleHeight(int value);
void video_setScaleX(int value);
void video_setScaleY(int value);
void video_setScaleSetting(void);

int video_getScaleWidth(void);
int video_getScaleHeight(void);
int video_getScaleX(void);
int video_getScaleY(void);

void video_setScaleRadio(u8 mode);
u8 video_getScaleRadio(void);
void video_setScaleH(u16 position, u16 width);
void video_setScaleV(u16 position, u16 height);
void video_getScaleH(void);
void video_getScaleV(void);


void video_setZoom(u16 x, u16 y, u16 width, u16 height);
void video_getZoom(void);

void video_setCrop(u16 x, u16 y, u16 width, u16 height);
void video_getCrop(void);

void video_setCropDot(int index, int value); //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--set crop single
int  video_getCropDot(int index); //  0 -- width,1-- height,2-- x, 3 -- y, 
void video_applayCropDot3(void); //lv@20160715

void video_setCropDotCh(char ch); //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--enable save crop ratio
void video_setCropDot2(int index, int value); //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--set crop single
int  video_getCropDot2(int index); //  0 -- width,1-- height,2-- x, 3 -- y, 
void video_applayCropDot2(void);
void video_setCropDotCh2(char ch); //  0 -- width,1-- height,2-- x, 3 -- y, 4--setcropsetting,5--enable save crop ratio

void video_setScreenH(u16 position, u16 width);
void video_setScreenV(u16 position, u16 height);

void video_setEnableExternMatrixCVBS(int enable);
void video_setEnableTPSelf(int enable);

void video_setInterlaceSwitch(int value); // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面
BYTE mst_getInterlaceSwitch(void); // 0 -- 特效切换，1 -- 隔行信号直接切换到主画面

void video_setInputColorSpace_TTL(int value);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
int video_getInputColorSpace_TTL(void);// 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2

void video_waitDeviceReady(void);

void video_SetCurrentWindow(int value);
u8 video_getCurrentWindow(void);

//u8 video_getInputFormat(void);
//u8 video_getInputFormatCh(BYTE ch);
void video_setInputTimming(int type, WORD value);// type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct
WORD video_getInputTimming(int type); // type=0 HStart, type=1 VStart, type=2 HAct, type=3 VAct

int video_getInputSourceParameter(int type);		//0:Hactive 1:VActive 2:Freq 3:HTotal 4:Vtotal
void video_resetSdiInputtimming(void);
void video_resetAllSdi(int type);	// 0 -- FPGA SDI, 1 -- GS2971 SDI

void video_setOutputFormat(u8 format);
BYTE video_getOutputFormat(void);
void video_setOutputMode(u8 mode);
void video_SetOutputColorSpace(int value); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2
int  video_GetOutputColorSpace(void); // 0 -- RGB 4:4:4, 1 -- YCbCr 4:4:4, 2 -- YCbCr 4:2:2

void video_setFrameLockMode(u8 mode); // 0 -- free run, 1 -- Fixed Frame Lock, 2 -- Dynamic Frame Lock
int  video_getFrameLockMode(void);
void video_setFrameLockSource(u8 source); // 0 -- frame lock main , 1 -- frame lock pip
int  video_getFrameLockSource(void);

void video_setDisplayMode(u8 mode);
int video_getDisplayMode(void);
void video_setFlatColor(u8 r, u8 g, u8 b);
void video_setTestPatternMode(u8 mode);
void video_setTestPatternColor(u8 r, u8 g, u8 b);
void video_setTestPatternParam(u8 horStep, u8 verStep, u8 stepColor);
void video_setBlackOut(u8 Enable);
int video_getBlackOut(void);

void video_setBrightness(u8 bCont,u8 red, u8 green, u8 blue);
void video_setBrightness_nowait(u8 red, u8 green, u8 blue);
int video_getBrightness(void);
void video_setContrast(u8 bCont,u8 red, u8 green, u8 blue);
void video_setContrast_nowait(u8 red, u8 green, u8 blue);
int video_getContrast(void);
void video_setColorTemp(u8 bCont,u8 red, u8 green, u8 blue);
void video_setColorTemp_nowait(u8 red, u8 green, u8 blue);
int video_getColorTemp(void);
void video_setColorTempMode(u8 mode);
int video_getColorTempMode(void);
void video_setSharpness(u8 sharpness);//20110906
int video_getSharpness(void);
void video_setSaturation(u8 saturation);//20110909
int video_getSaturation(void);

void video_setAdcAutoAdjust(void);
void video_ResetAdcSetting(void);
void video_setRGBSaturation(u8 r, u8 g, u8 b);
int video_getRGBSaturation(void);

void video_setCMYSaturation(u8 c, u8 m, u8 y);
int video_getCMYSaturation(void);
void video_setEvenness(u8 value);
int video_getEvenness(void);

void video_setVideoEnhance(u8 value);
int video_getVideoEnhance(void);

void video_setPcX(u16 value);
void video_setPcY(u16 value);
void video_setPcClock(u16 clock);
void video_setPcPhase(u16 phase);
void video_setAutoAdjustVGA(void);
void video_getPcX(void);
void video_getPcY(void);
void video_getPcClock(void);
void video_getPcPhase(void);

void video_setGamma(int value);
BYTE video_getGamma(void);

void video_getDeviceState(void);

void video_setAudioSource(u8 source);
void video_setAudioMode(u8 mode);
void video_setHdmiAudio(u8 value);
void video_setVolume(u8 value);
void video_setMuteVolume(u8 value);

void video_setWindowRatio(u8 value); // 0 -- 4:3, 1--16:9, 2 --Normal
void video_setScreenMode(u8 value); // 0 -- full window, 1 -- screen window

int video_getAlpha(void);
//void video_setAlpha(int value);

void video_setSwitchMode(int value);
BYTE video_getSwitchMode(void);
void video_setFadeTime(int value);
int video_getFadeTime(void);
void video_setSwitchAB(int value);
int video_getSwitchAB(void);
int video_getCurrentCh(void);

void video_setPipMode(int value);
int video_getPipMode(void);
void video_SwapMainPipInputSource(void);
int video_getSwapMainPipInputSource(void);
void video_setTopWindow(int value);

void video_setTextEnableMode(int enable, int mode);
void video_setTextAboveAndOr(int above, int AndOr);
void video_setTextColor(int r, int g, int b);

#if 1  //modify by yy @20150828
void video_setSplitSync(int onOff);    // data2:  0-0ff 1-on  data3: 0-FFL 1-DFL
int video_getSplitSync(void) ;
#endif
void video_setSplit(int onOff, int syncMode);
void video_setSplitHTotal(int value);
void video_setSplitVTotal(int value);
void video_setSplitHSize(int value);
void video_setSplitVSize(int value);
void video_setSplitHPos(int value);
void video_setSplitVPos(int value);
void video_setVLockLine(int value);
int video_getVLockLine(void);

void video_setOutputOffset(int x, int y);
void video_waitUpdateSoftware(void);
bool video_updateSoftware(void);
void video_setCustomParameter(int type, WORD value); //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
int video_getCustomParameter(int type); //type:0-HTotal 1-VTotal 2-HStart 3-VStart 4-VFreq 5-HSyncWidth 6-VSyncWidth 7-HVSyncPolarity 8-DisplayWidth 9-DisplayHeight
#if 1
int video_getSTRegister(int type) ;
int video_getSTVideoValue(int type) ;
void video_setDviTPClock(int enable);
int video_getDviTPClock(void) ;
void video_updateColorSpace(void);
void video_setEnableInternalHDCP(int enable);
void video_setsdiIndex_TTL(int value);//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
int video_getsdiIndex_TTL(void);//0  SDI   1 HDMI  2 VGA2  3 YPbPr2  ;// only 0 for 516s  0-3 for 198 
BYTE video_getTTLInterlaveMode(void);
void video_setTTLInterlaveMode(BYTE type);
int video_getHActiveValue(void);
int video_getVActiveValue(void);
int video_getHTotalValue(void);
int video_getVTotalValue(void);
int video_getFreqValue(void);
void video_setDpVersion(int version);	/* 0 - 1.1a 1-1.2 */
int video_getDpVersion(void);
void video_setDpOutputMode(int type);	/* 0-iDp 1-Dp loop */
int video_getDpOutputMode(void);
void video_setCustomWidth(int value);
int video_getCustomWidth(void);
void video_setCustomHeight(int value);
//int video_getCustomHeight(void);
void video_setCustomFreq(int value);
//int video_getCustomFreq(void);
void video_ApplyCustomResolution(void);
int video_getEnableDoubleOutput(void);
void video_setDVIInputMode(int type);  // 0:单链路 1:双链路
int video_getDVIInputMode(void);
void video_setDVIColorSpace(int ch, int type);  // 0: RGB 1: YUV 4:4:4   2: YUV 4:2:2  3: BT565
int video_getDVIColorSpace(int ch);
void video_setPipColorSpace(int ch, int type);  // 0: RGB 1: YUV 4:4:4   
int video_getPipColorSpace(int ch);
void video_setOverTurn(int type);   // 0:不翻转  1: 左右翻转  2: 上下翻转 3:对角翻转
int video_getOverTurn(void);
void video_setMADI(int type);	// 00: MADI_OFF  01:MADI_ADAPTIVE  02:MADI_STADARD 03:MADI_LOW_MOTION 04:MADI_HIGHT_MOTION
int video_getMADI(void);
void video_setEdidWp(int type) ; 
void video_setDpEDIDPixclock(int value);  
int video_getDpEdidPixclock(void);
void video_setDpEdidhActive(int value) ; 
int video_getDpEdidhActive(void);
void video_setDpEdidvActive(int value);  
int video_getDpEdidvActive(void);
void video_setDpEdidhBlank(int value)  ;
int video_getDpEdidhBlank(void);
void video_setDpEdidvBlank(int value)  ;
int video_getDpEdidvBlank(void);
void video_setDpEdidhSyncOffset(int value) ; 
int video_getDpEdidhSyncOffset(void);
void video_setDpEdidvSyncOffset(int value) ; 
int video_getDpEdidvSyncOffset(void);
void video_setDpEdidhSyncWidth(int value) ; 
int video_getDpEdidhSyncWidth(void);
void video_setDpEdidvSyncWidth(int value)  ;
int video_getDpEdidvSyncWidth(void);
void video_setDpEdidhvPolarity(int value)  ;
int video_getDpEdidhvPolarity(void);
void video_setDpEdidUpdate(void)  ;
void video_setDpEDIDContent(int num, int value);
int video_getDpEDIDContent(int num);
int video_getLVDSMode(void);
void video_setLVDSMode(int value)  ;
void video_setSence(int value) ;	//0:normal 1:多媒体 2:游戏 3:电影4:文本
int video_getSence(void);
void video_setOSDKey(int value);	// 0:menu 1:up 2:down 3:left 4:right 5:Enter 6: return 7:Source
void video_setBoardParam(int param, int value);
int video_getBoardParam(int param);
void video_AutoAdjust(void);
void video_setAdjustH(int value) ;
int video_getAdjustH(void);
void video_setAdjustV(int value) ;
int video_getAdjustV(void);
void video_setAdjustClock(int value) ;
int video_getAdjustClock(void);
void video_setAdjustPhase(int value) ;
int video_getAdjustPhase(void);
void video_VGAAuotoClock(void);
void video_setSplitSetting(int value);
int video_getSplitSetting(void);
void video_getInputParams_Long(void);
void video_getLayerScalerParams_Long(void);

//void video_setDPEDID_First(char *buffer,int len);
//void video_setDPEDID_Second(char *buffer,int len);
void video_setSplitZoomDot(char index,int value); //0- width  1- height  2- x   3- y 
int video_getSplitZoomDot(char index);//0- width  1- height  2- x   3- y 
void video_applySplitZoom(char ch);

void video_forceReIdentifyCh(char ch);

void video_clearNVRAM(void);

void SetLayerScalerToCh(char ch, int x,int y,int width, int height);

#endif


int GetVideoVersion(void);
int GetDPEDID(void);
void SetDPEDID(void);

void controlVideoEnable(bool enable);

void DisableEDIDWP(void);
void EnableEDIDWP(void);

void mst_setAutoBackGroundControlEn(u8 enable);
void mst_setDisplayModeFreezeAll(bool freeze);
void mst_setCropSplitEnable(bool enable);
void mst_setCropSplitInput(int index, int value);
void mst_setDPAudio(u8 value);
void mst_setHDMIInterlaveMode(BYTE type);
void mst_setAudioType(u8 value);
void mst_setLayerParam_Long(BYTE layer);


#endif		// #ifdef USE_STDP9320

#endif // _PREAMP_H_

