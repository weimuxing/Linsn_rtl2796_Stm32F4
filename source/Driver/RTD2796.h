



#ifndef __RTD2796_H__
#define __RTD2796_H__

//#include "sys.h"
//#include "delay.h"
//#include "usart.h"
#include "define.h"
#include "rgblinkProtocol.h"
#include "userPref.h"

#define _VGA_INPUT_ENABLE

#define RTD2796_RESET_AHBxPeriph 	RCC_AHB1Periph_GPIOH
#define RTD2796_RESET_PIN			GPIO_Pin_13
#define  RTD2796_RESET_PORT			GPIOH


#define RTD2796_RESET_PIN_H()	(GPIO_SetBits(RTD2796_RESET_PORT,RTD2796_RESET_PIN))
#define RTD2796_RESET_PIN_L()	(GPIO_ResetBits(RTD2796_RESET_PORT,RTD2796_RESET_PIN))


#if 0
#define _MENU_KEY_MESSAGE                           0
#define _RIGHT_KEY_MESSAGE                          1
#define _LEFT_KEY_MESSAGE                           2
#define _EXIT_KEY_MESSAGE                           3
#define _5_KEY_MESSAGE                              4
#else
#define _MENU_KEY_MESSAGE                              0x02
#define _RIGHT_KEY_MESSAGE                             0x04
#define _LEFT_KEY_MESSAGE                              0x08
#define _EXIT_KEY_MESSAGE                              0x10
#endif

#ifdef _VGA_INPUT_ENABLE
#define _VGA_INPUT	0
#endif
#define _DP_INPUT	1
#define _HDMI20_INPUT	2
#define _HDMI14_INPUT	3
#define _DVI_INPUT		5

#ifdef _VGA_INPUT_ENABLE
#define INPUT_SOURCE_MAX	(4)
#else
#define INPUT_SOURCE_MAX	(3)
#endif

#define GET_INPUT_SOURCE_INDEX(x)		((x > INPUT_SOURCE_MAX)?(INPUT_SOURCE_MAX):(x))

extern bool gInputFlag;

typedef enum 
{
	_DP_EDID_4K_2K_60 = 0,
	_DP_EDID_4K_2K_30 = 1,
	_DP_EDID_1080P_60 = 2,
	_DP_EDID_CUSTOM,//ustom

	_DP_EDID_MAX_SIZE,
}_DP_EDID_ENUM;


#if 0
typedef struct
{
	WORD InputPgmWidth;
	WORD InputPgmHeight;
	WORD InputPgmFreq;
	
	WORD InputPrvWidth;
	WORD InputPrvHeight;
	WORD InputPrvFreq;

#if 1
	WORD LT_inputWidth;
	WORD LT_inputHeight;
	WORD LT_inputFreq;
	
	WORD LB_inputWidth;
	WORD LB_inputHeight;
	WORD LB_inputFreq;

	WORD RT_inputWidth;
	WORD RT_inputHeight;
	WORD RT_inputFreq;
	
	WORD RB_inputWidth;
	WORD RB_inputHeight;
	WORD RB_inputFreq;
#endif
	
} InputResolution_Struct;

#else

typedef struct
{
	
#if 1
	WORD DVI_inputWidth;
	WORD DVI_inputHeight;
	WORD DVI_inputFreq;
	BYTE DVI_inputStatus;
	
	WORD HDMI1_inputWidth;
	WORD HDMI1_inputHeight;
	WORD HDMI1_inputFreq;
	BYTE HDMI1_inputStatus;

	WORD HDMI2_inputWidth;
	WORD HDMI2_inputHeight;
	WORD HDMI2_inputFreq;
	BYTE HDMI2_inputStatus;
	
	WORD DP_inputWidth;
	WORD DP_inputHeight;
	WORD DP_inputFreq;
	BYTE DP_inputStatus;

	WORD VGA_inputWidth;
	WORD VGA_inputHeight;
	WORD VGA_inputFreq;
	BYTE VGA_inputStatus;

	WORD LT_inputWidth;
	WORD LT_inputHeight;
	WORD LT_inputFreq;
	BYTE LT_inputStatus;
	BYTE LT_inputSource;

	WORD LB_inputWidth;
	WORD LB_inputHeight;
	WORD LB_inputFreq;
	BYTE LB_inputStatus;
	BYTE LB_inputSource;

	WORD RT_inputWidth;
	WORD RT_inputHeight;
	WORD RT_inputFreq;
	BYTE RT_inputStatus;
	BYTE RT_inputSource;

	WORD RB_inputWidth;
	WORD RB_inputHeight;
	WORD RB_inputFreq;
	BYTE RB_inputStatus;
	BYTE RB_inputSource;
#endif

} InputResolution_Struct;

#endif





enum Branch_WR_SYNC	
{
	WR_InputSourceInfo					= 0x03,
	WR_CropScale						= 0x09,
	WR_InputSourceInfo_2				= 0x0a,
	WR_4KModeSetting					= 0x18,
	WR_EDIDSetting						= 0x19,
	WR_DisplaySetting					 = 0x20,
	WR_ModeSetting						= 0x26,
	WR_PQSetting                        = 0x27,
	WR_UserDisAreaSetting               = 0x28,//用户显示区域大小设置

	WR_RegionInputSource,	
	WR_RegionSize,

	W_PIP_POS_SIZE = 0XB0,
	
	W_PIP_SUB_POS_X,
	W_PIP_SUB_POS_Y,
	W_PIP_SUB_SIZE_X,
	W_PIP_SUB_SIZE_Y,

	W_PIP_MAIN_POS_X,
	W_PIP_MAIN_POS_Y,
	W_PIP_MAIN_SIZE_X,
	W_PIP_MAIN_SIZE_Y,

	W_PipWinParamSetting = 0xC0,
	
	W_PIP_SUB_INPUT_SPURCE,
};




typedef enum 
{
	_1P_POINT_TO_POINT = 8,	//点对点任意拼接模式
	_BEYOND_WIDTH_SPLIT = 1,	//超宽拼接模式
	_BEYOND_HEIGHT_WIDTH_SPLIT = 0,	//超高拼接模式
	_TWO_4K_SWAP = 3,	//2个4K互切显示模式
	_PBP_UP_DOWN = 5,	//2796上下输出预监模式
	_PBP_LEFT_RIGHT = 7,	//2796左右输出预临模式
	
}FpgaDisplayMode;



typedef enum
{
    _OSD_DM_1P = 4,
	_OSD_DM_4P,
    _OSD_DM_2P_LR,
    _OSD_DM_2P_TB,
    _OSD_DM_2P_PIP,
    _OSD_DM_2P_SACLE_LR,
    _OSD_DM_2P_SACLE_TB,
    _OSD_DM_3P_SACLE_TB,
    _OSD_DM_2P_TOW_PORT_TB,
    _OSD_DM_2P_TOW_PORT_LR,
    _OSD_DM_AMOUNT,
}EnumOSDDisplayModeDef;



typedef enum
{
    _DISPLAY_MODE_1P = 0x00,
    _DISPLAY_MODE_PIP = 0x10,
    _DISPLAY_MODE_PBP_LR = 0x20,
    _DISPLAY_MODE_PBP_TB = 0x30,
    _DISPLAY_MODE_4P = 0x40,
    _DISPLAY_MODE_NONE = 0xF0,
} EnumDisplayMode;



typedef enum
{
    _REGION_INDEX_0 = 0x00,
    _REGION_INDEX_1 = 0x01,
    _REGION_INDEX_2 = 0x02,
    _REGION_INDEX_3 = 0x03,
    _REGION_INDEX_NONE = 0x0F,
} EnumRegionIndex;



typedef enum
{
    _DISPLAY_RGN_1P = _DISPLAY_MODE_1P,
		
    _DISPLAY_RGN_MAIN = (_DISPLAY_MODE_PIP | _REGION_INDEX_0),
    _DISPLAY_RGN_SUB = (_DISPLAY_MODE_PIP | _REGION_INDEX_1),
    
    _DISPLAY_RGN_LEFT = (_DISPLAY_MODE_PBP_LR | _REGION_INDEX_0),
    _DISPLAY_RGN_RIGHT = (_DISPLAY_MODE_PBP_LR | _REGION_INDEX_1),
    
    _DISPLAY_RGN_TOP = (_DISPLAY_MODE_PBP_TB | _REGION_INDEX_0),
    _DISPLAY_RGN_BOTTOM = (_DISPLAY_MODE_PBP_TB | _REGION_INDEX_1),

    _DISPLAY_RGN_LT = (_DISPLAY_MODE_4P | _REGION_INDEX_0),
    _DISPLAY_RGN_LB = (_DISPLAY_MODE_4P | _REGION_INDEX_1),
    _DISPLAY_RGN_RT = (_DISPLAY_MODE_4P | _REGION_INDEX_2),
    _DISPLAY_RGN_RB = (_DISPLAY_MODE_4P | _REGION_INDEX_3),

    _DISPLAY_RGN_NONE = (_DISPLAY_MODE_NONE | _REGION_INDEX_NONE),
} EnumDisplayRegion;


typedef enum
{
    _1P_NORMAL_FULL_REGION = _DISPLAY_RGN_1P,
    _1P_ROTATION_FULL_REGION,
    _1P_NORMAL_DEMO_INSIDE,
    _1P_NORMAL_DEMO_OUTSIDE,

    _PIP_MAIN = _DISPLAY_RGN_MAIN,
    _PIP_SUB = _DISPLAY_RGN_SUB,
    _PIP_FULL_REGION,
    _PIP_DEMO_INSIDE,
    _PIP_DEMO_OUTSIDE,

    _PBP_LR_L = _DISPLAY_RGN_LEFT,
    _PBP_LR_R = _DISPLAY_RGN_RIGHT,
    _PBP_LR_FULL_REGION,
    _PBP_LR_DEMO_INSIDE,
    _PBP_LR_DEMO_OUTSIDE,

    _PBP_TB_T = _DISPLAY_RGN_TOP,
    _PBP_TB_B = _DISPLAY_RGN_BOTTOM,
    _PBP_TB_FULL_REGION,
    _PBP_TB_DEMO_INSIDE,
    _PBP_TB_DEMO_OUTSIDE,

    _4P_LT = _DISPLAY_RGN_LT,
    _4P_LB = _DISPLAY_RGN_LB,
    _4P_RT = _DISPLAY_RGN_RT,
    _4P_RB = _DISPLAY_RGN_RB,
    _4P_LT_OUTSIDE,
    _4P_LB_OUTSIDE,
    _4P_RT_OUTSIDE,
    _4P_RB_OUTSIDE,
    _4P_FULL_REGION,
    _4P_DEMO_INSIDE,
    _4P_DEMO_OUTSIDE,

    _NON_REGION = _DISPLAY_RGN_NONE,
} EnumSelRegion;


typedef enum
{
    _OSD_SR_4P_LT_INSIDE,
    _OSD_SR_4P_LT_OUTSIDE,
    _OSD_SR_4P_LB_INSIDE,
    _OSD_SR_4P_LB_OUTSIDE,
    _OSD_SR_4P_RT_INSIDE,
    _OSD_SR_4P_RT_OUTSIDE,
    _OSD_SR_4P_RB_INSIDE,
    _OSD_SR_4P_RB_OUTSIDE,
    _OSD_SR_4P_FULL,
}EnumOSDSelectRegion4PDef;



typedef enum
{
    _DIS_RGN_1P = 0,
		
    _DIS_RGN_MAIN,
    _DIS_RGN_SUB,
    
    _DIS_RGN_LEFT,
    _DIS_RGN_RIGHT,
    
    _DIS_RGN_TOP,
    _DIS_RGN_BOTTOM,

    _DIS_RGN_LT,
    _DIS_RGN_LB,
    _DIS_RGN_RT,
    _DIS_RGN_RB,

    _DIS_RGN_NONE,
} EnumDisRegion;




typedef enum
{
	FIELD_SPLIT_MODE = 0,
	HORIZONTAL_1_4_MODE,
	VERICAL_1_4_MODE,
	HORIZONTAL_1_2_MODE,
	VERICAL_1_2_MODE,
	SPLIT_MODE_SIZE,
}ENUM_SPLIT_MODE;



typedef enum
{

    _PANEL_TYP_FRAME_RATE_INDEX,			//帧频
    _PANEL_MAX_FRAME_RATE_INDEX,			//帧频最大值
    _PANEL_MIN_FRAME_RATE_INDEX,			//帧频最小值

    _PANEL_LAST_LINE_TARGET_INDEX,			//4180 ?

	_PANEL_LVDS_OUTPUT_PORT,			//LVDS输出端口,默认_LVDS_8_PORT
	_PANEL_LVDS_PORT_SEL,				//_LVDS_8_PORT_ABCDEFGH
	_PANEL_LVDS_PANEL_SECTION,			//_LVDS_PANEL_2_SECTION

    _PANEL_DISP_COLOR_BIT_INDEX,			//_PANEL_DISP_30_BIT
//--------------------------------
    _PANEL_DH_START_INDEX,				//H 开始 根据屏参设定
    _PANEL_DH_WIDTH_INDEX,				//H 有效
    _PANEL_DH_END_INDEX,				//H 结束  开始+有效 ＝ 结束 ， 下同

    _PANEL_DH_TOTAL_INDEX,				//H Total
    _PANEL_DH_TOTAL_MAX_INDEX,				//H Total 最大
    _PANEL_DH_TOTAL_MIN_INDEX,				//H Total 最小  最大和最小比Total +- 10左右

    _PANEL_DV_START_INDEX,				//V 开始 根据屏参设定				
    _PANEL_DV_HEIGHT_INDEX,				//V 有效
    _PANEL_DV_END_INDEX,				//V 结束

    _PANEL_DV_TOTAL_INDEX,				//V Total
    _PANEL_DV_TOTAL_MAX_INDEX,				//V Total 最大
    _PANEL_DV_TOTAL_MIN_INDEX,				//V Total 最小

    _PANEL_DH_SYNC_WIDTH_INDEX,				//H 同步			
    _PANEL_DV_SYNC_HEIGHT_INDEX,			//V 同步

    _PANEL_PIXEL_CLOCK_INDEX,				//pClk 像素时钟
    _PANEL_PIXEL_CLOCK_MAX_INDEX,			//pClk 最大
    _PANEL_PIXEL_CLOCK_MIN_INDEX,			//pClk 最小

    _PANEL_DATA_AMOUNT,
} EnumMultiPanelIndex;


typedef enum 
{

	_DVI_INPUT_SOURCE = 0,
	_HDMI1_INPUT_COURCE,
	_HDMI2_INPUT_SOURCE,
	_DP_INPUT_SOURCE,

#ifdef _VGA_INPUT_ENABLE
	_VGA_INPUT_SOURCE = 12,
#endif
}ENUM_INPUT_SOURCE;


typedef struct 
{

    u16 _panelFrameActive;			//帧频
    u16 _panelFrameMax;				//帧频最大值
    u16 _panelFrameMin;				//帧频最小值

    u16 _panelLastLine;				//4180 ?

	u16 _panelLvdsOutputPort;		//_PANEL_LVDS_OUTPUT_PORT,			//LVDS输出端口,默认_LVDS_8_PORT
	u16 _panelLvdsPortSel;			//_PANEL_LVDS_PORT_SEL,				//_LVDS_8_PORT_ABCDEFGH
	u16 _panelLvdsPanelSection;		//_PANEL_LVDS_PANEL_SECTION,			//_LVDS_PANEL_2_SECTION

    u16 _panelDispColorBits;		//_PANEL_DISP_COLOR_BIT_INDEX,			//_PANEL_DISP_30_BIT
//--------------------------------
    u16 _panelHsStart;				//_PANEL_DH_START_INDEX,				//H 开始 根据屏参设定
    u16 _panelHsWidth;				//_PANEL_DH_WIDTH_INDEX,				//H 有效
    u16 _panelHsEnd;				//_PANEL_DH_END_INDEX,				//H 结束  开始+有效 ＝ 结束 ， 下同

    u16 _panelHsTotal;				//_PANEL_DH_TOTAL_INDEX,				//H Total
    u16 _panelHsTotalMax;			//_PANEL_DH_TOTAL_MAX_INDEX,				//H Total 最大
    u16 _panelHsTotalMin;			//_PANEL_DH_TOTAL_MIN_INDEX,				//H Total 最小  最大和最小比Total +- 10左右

    u16 _panelVsStart;				//_PANEL_DV_START_INDEX,				//V 开始 根据屏参设定				
    u16 _panelVsHeight;				//_PANEL_DV_HEIGHT_INDEX,				//V 有效
    u16 _panelVsEnd;				//_PANEL_DV_END_INDEX,				//V 结束

    u16 _panelVsTotal;				//_PANEL_DV_TOTAL_INDEX,				//V Total
    u16 _panelVsTotalMax;			//_PANEL_DV_TOTAL_MAX_INDEX,				//V Total 最大
    u16 _panelVsTotalMin;			//_PANEL_DV_TOTAL_MIN_INDEX,				//V Total 最小

    u16 _panelHsync;				//_PANEL_DH_SYNC_WIDTH_INDEX,				//H 同步			
    u16 _panelVsync;				//_PANEL_DV_SYNC_HEIGHT_INDEX,			//V 同步

    u16 _panelPclk;					//_PANEL_PIXEL_CLOCK_INDEX,				//pClk 像素时钟
    u16 _panelPclkMax;				//_PANEL_PIXEL_CLOCK_MAX_INDEX,			//pClk 最大
    u16 _panelPclkMin;				//_PANEL_PIXEL_CLOCK_MIN_INDEX,			//pClk 最小
    u8 _panelCheckSum;

   // _PANEL_DATA_AMOUNT,
}PanelParamSt;


typedef struct 
{
	u8 workMode;	//0~2  工作模式：0 - 4Kx2K， 1 - 4Kx1K，2 - 2Kx1K
	u8 PTPswtich;	//点对点开关：0 - 关闭， 1 - 打开
	u8 firstInput;	//第一输入源：0 - DVI, 1 - HDMI, 2 - DP	
	u8 secondInput;	//第二输入源：0 - DVI, 1 - HDMI, 2 - DP
	u8 totalWidthLsb;	//
	u8 totalWidthMsb;	//
	u8 totalHeightLsb;	//
	u8 totalHeightMsb;	//
	u8 vFreqLSB;
	u8 vFreqMSB;		
}ModeSettingSt;



extern InputResolution_Struct InputResolution;
extern FourPortSplitParamSt FourPortSplitPara;


#define GetFirstRegionWidthStart()		(FourPortSplitPara.stFirstRegionWidthStart)
#define GetFirstRegionHeightStart()		(FourPortSplitPara.stFirstRegionHeightStart)
#define GetSecondRegionWidthStart()		(FourPortSplitPara.stSecondRegionWidthStart)
#define GetSecondRegionHeightStart()	(FourPortSplitPara.stSecondRegionHeightStart)
#define GetThirdRegionWidthStart()		(FourPortSplitPara.stThirdRegionWidthStart)
#define GetThirdRegionHeightStart()		(FourPortSplitPara.stThirdRegionHeightStart)
#define GetFourthRegionWidthStart()		(FourPortSplitPara.stFourthRegionWidthStart)
#define GetFourthRegionHeightStart()	(FourPortSplitPara.stFourthRegionHeightStart)



extern char RTD2796Init(void);
extern u8 GetvideoInputInfo(void);
extern u8 GetPgmCurrentDisplayMode(void);
extern u8 GetPgmCurrentInput(void);
extern u8 GetPrvCurrentInput(void);
extern u8 GetPgmCurrentRegion(void);
extern u8 GetPrvCurrentRegion(void);

extern u16 GetRtd2796Ver(void);
extern u16 GetScalerPosSize(u8 index);
extern u16 SetScalerPosSize(u8 index,u16 value);
extern InputResolution_Struct *GetInputResolution(void);
extern FunctionalState GetPtpStatus(void);

extern void RTD2796StopRun(void);
extern void RTD2796Reset(void);
extern void RTD2796SendCmdData(void);
extern void SendRs232CommandToRTD2796(Command_TypeDef commandTo2796);
extern void videoSetInputSource(u8 source);
extern void GetvideoInputSource(void);
extern void GetVideoOutputFormat(void);
extern void _4KModeSetting(void);
extern void VideoPanelSetting(u16 hWidth,u16 vHeight,u16 Frame);
extern void UserDisAreaSetting(u16 userTotalWidth,u16 userTotalHeigh,u16 userHStart,u16 userVStart,u8 OsdLayout);
extern void UserSplitParamSetting(FourPortSplitParamSt splitParam);
extern void FieldSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam);
extern void TwoPortSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode);
extern void ThirdPortSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode);
extern void FourPortSplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode);
extern void Two4kTakeModeSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode);
extern void Horizontal2SplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode);
extern void Vertical2SplitSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode);
extern void IndependentOutputSetting(SPLIT_PARAM_Typedef *fourPortSplitParam,u8 mode);
extern void videoInputSwap(u8 source);
extern void videoSendMassage(u8 Massage);
extern int  ReceiveBuffer_2796(char *buffer, int size, int timeout);
extern void InitRtd2796Pipe(void);
extern void SetVideoDisplay(u8 modex);
extern void SetRegionInputPort(u8 enumDisplayRegion,u8 inputPort);
extern void PanelParamInit(void);
extern void GetRtd2796Version(void);
extern void SetRtd2796Version(u8 vHeight,u8 vLow);
extern void SetPgmCurrentDisplayMode(u8 x);
extern void SetPgmCurrentInput(u8 x);
extern void SetPrvCurrentInput(u8 x);
extern void SetPgmCurrentRegion(u8 x);
extern void SetPrvCurrentRegion(u8 x);
extern void SetVideoDisplayMode(u8 modex);
extern void task_2796(void);
extern void TaskInputResolution(void);
extern void ResetInputResolution(void);
extern void SetDpEdid(_DP_EDID_ENUM edidIndex,u8 *edidBuf);
extern void OutputTypeInit(OUTPUT_TYPE_PARAM_Typedef *stOutputType);
extern void SetVideoBrightness(u16 brightness);
extern void SetVideoContrast(u16 cnntrast);
extern void SetVideoGamma(u8 gamma);
extern void ResetVideoColorEffect(BYTE flag);
extern void SetVideoRegionWidth(EnumOSDSelectRegion4PDef enumDisplayRegion,u16 x);
extern void SetVideoRegionHeight(EnumOSDSelectRegion4PDef enumDisplayRegion,u16 x);
extern void SetVideoRegionParam(EnumOSDSelectRegion4PDef enumDisplayRegion,u16 x,u16 y,u16 w,u16 h);
extern void Set4PRegionSource(EnumDisplayRegion enumDisRegion,u8 source);
extern void SetVideoColorEffect(u8 effect);
extern void SetVideocolorTempType(u8 temp);
extern void SetVideocolorTemp(u8 sel,u8 temp);
extern void DisplayInputInfo(void);
extern void SetPipModeParam(BYTE mode,BYTE MainSource,BYTE SubSource);
extern void PipModeSetting(BYTE onOff);
extern void SetPipPosAndSize(EnumDisplayRegion enumDisRegion,BYTE cmdIndex,u16 value);
extern void videoSetPipInputSource(EnumDisplayRegion enumRegion,u8 source);
extern void SetPipLayout(BYTE layout);
extern void UserPipParamSetting(pPipWinSizeSt stPipWinParam);
extern void CalcPipLayoutSize(BYTE layout,pPipWinSizeSt stPipLayoutSize);
extern void SetPipWindowStart(pPipWinSizeSt stPipLayoutSize);





#endif


