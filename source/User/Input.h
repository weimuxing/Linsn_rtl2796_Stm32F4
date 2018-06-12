#ifndef _INPUT_H_
#define _INPUT_H_

#include "define.h"
#include "userPref.h"

typedef enum
{
	MST6M182_PORT_1,
	MST6M182_PORT_2,
	MST6M182_PORT_3,
	//MST6M182_PORT_4,
	MST6M182_PORT_SIZE,
} MST6M182_PortTypedef;

typedef enum
{
	REPEAT_ALL, //0
	NORMAL,     // 1
	REPSELECT,  // 2
	PLAYSEL,    // 3
	RANDOMLY,   // 4
	PLAYLIST,   // 5
	PLAYORDER_SIZE,// 6
}UsbPlayorder_TypeDef;

typedef enum
{
	ExtInput_NoBoard = 0,
	ExtInput_DVI     = 1,
	ExtInput_HDMI    = 2,
	ExtInput_SDI     = 3,
	ExtInput_DP      = 4,
	ExtInput_VGA     = 5,
	ExtInput_YPbPr   = 6,
	ExtInput_CV      = 7,
	ExtInput_SV      = 8,
	ExtInput_D_HDMI  = 9,
	ExtInput_USB     = 10,
	ExtInput_AUDIO	 = 11,
	ExtInput_D_DVI	 = 12,
	
	ExtInput_MAXSIZE,
}ExtInputType_Typedef;

typedef struct
{
	BYTE inputPort;		// 0 ~ 1  port 仅针对CV    USB 输入板
	BYTE inputType;		// 0 - 无输入，1 - DVI输入，2 - HDMI, 3 - SDI, 4 - DP, 5 - VGA, 6 - YpbPr, 7 - CVBS, 8 - SV,  9 --（Double）DVI， 10--USB
	BYTE inputFormat;
	int HActive;
	int VActive;
	BYTE VFreq;
	
	BYTE freeze;        // 0 -- off, 1 -- on
	BYTE horizontalFlip; // 0 -- off, 1 -- on
	BYTE rotate; // 0 -- off, 1 -- left rotation, 2 -- right rotation.
	PosSizeSetting cropDot; //逐点裁剪,表示宽度,高度等裁剪掉的点数
	PosSizeSetting scale;		
	PosSizeSetting cropPre;//预裁剪   因为8028需要先对输入信号按分辨率裁剪才能出现满屏的效果
	
	BYTE alpha;
	BYTE layerOnOff;
	BYTE brightness[3];
	BYTE contrast[3];
	BYTE sharpness;
	BYTE saturation;      //for 182 input board
	BYTE colorTemp[3];    //for 182 input board
	BYTE outputFormat;
}ExtInputSetting;

typedef struct
{
	int pos_x;
	int pos_y;
	int clock;
	int phase;
}ExtVGAAdjustSetting;

typedef struct
{
	BYTE isConnect[3];           //0-EXT7      1- EXT8          2- EXT 9
	BYTE inputChanged[3];        //0-EXT7      1- EXT8          2- EXT 9(无效)
	ExtInputSetting extInput[3]; //0-EXT7      1- EXT8          2- EXT 9(仅inputType有效) 
	//u8 ver_msb;   
	//u8 ver_lsb;
	int mstVersion[3];// 0- 对应EXT7的182版本   1- 对应EXT8的182版本   2-对应EXT9的182版本(只针对外扩主板V1.3)
	int softVer;//对应输入板版本
	u8 hardVer;
	
	int edidLen[3]; //3个输入口的EDID长度         //0-EXT7      1- EXT8          2- EXT 9
	u8 edidDetail[3][256]; //3个输入口的 EDID数据 //0-EXT7      1- EXT8          2- EXT 9		
	ExtVGAAdjustSetting extVGAAdjust[2]; //0-EXT7      1- EXT8 
	u8 regValue;
	bool autoChangeOutputFormatEnable[2];//0-EXT7      1- EXT8 
}InputSetting;

InputSetting * getInputSetting(void);

void EnableRefleshInput(void);
void DisableRefleshInput(void);

void Isr_receive_Input(u8 value);
void task_Input(void);
void init_Input(void);
void Input_readDeviceType(void);
int Input_goToBoot(void);
int Input_sendProgramMcuBuffer(int address, u8 *buffer, int size);
void Input_send(int cmd, int dat1, int dat2, int dat3, int dat4);
bool Input_sendAndWait(int cmd, int dat1, int dat2, int dat3, int dat4);
void taskRefleshInputInfo(void);
int getChangeExtInputFormatFlag(void);

int Input_GetVersion(void);//input board version
u8 Input_GetHardVersion(void);
int Input_GetMstVersion(char port);//port: 0- ext7    1- ext8
void Input_setUSBPort(char board,char port); //board 1- ext7  2- ext8    port 0-1
char Input_getUSBPort(char board);    //board 1- ext7  2- ext8 
void Input_setUSBPlayMode(char board,char port,char mode); //board 1- ext7  2- ext8    mode 0-photo 1-video
char Input_getUSBPlayMode(char board,char port); //board 1- ext7  2- ext8    
void Input_setUSBPlayState(char board,char state); //board 1- ext7  2- ext8    state 0-play 1-pause
char Input_getUSBPlayState(char board); //board 1- ext7  2- ext8   
void Input_setUSBPlayOrder(char board,char order); //board 1- ext7  2- ext8   
char Input_getUSBPlayOrder(char board); //board 1- ext7  2- ext8   

char Input_getUSBFileNum(char board); //board 1- ext7  2- ext8
void Input_getUSBFileName(char board,char index); //board 1- ext7  2- ext8
void Input_setUSBPlayIndex(char board,char index); //board 1- ext7  2- ext8   
char Input_getUSBPlayIndex(char board); //board 1- ext7  2- ext8   
int Input_getUSBFileLength(char board); //board 1- ext7  2- ext8
void Input_setUSBPlayCurrentTime(char board,int time); //board 1- ext7  2- ext8   
int Input_getUSBPlayCurrentTime(char board); //board 1- ext7  2- ext8   
void Input_setUSBPhotoTime(char board,char time); //board 1- ext7  2- ext8   
char Input_getUSBPhotoTime(char board); //board 1- ext7  2- ext8   
void Input_getUSBPlayList(char board,char index); //board 1- ext7  2- ext8   

u32 getUSBPlayListlen(void);
char getUSB_RefleshFileNameFlag(void);

void Input_RefleshUSBParams(char board); //board 1- ext7  2- ext8

void Input_readEDID(u8 port); //port 0-EXT7      1- EXT8          2- EXT 9
void Input_writeEDID(u8 port,u8 *data,int len); //port 0-EXT7      1- EXT8          2- EXT 9

u8 getInputStatus(void);

void Input_setCVBSPort(char board,char port); //board 1- ext7  2- ext8    port 0-1



void Input_seteVgaAutoAdjust(char board);//board 0- ext7  1- ext8  
void Input_seteVgaAdjustPosX(char board,int pos_x);//board 0- ext7  1- ext8  
int Input_geteVgaAdjustPosX(char board);//board 0- ext7  1- ext8  
void Input_seteVgaAdjustPosY(char board,int pos_y);//board 0- ext7  1- ext8  
int Input_geteVgaAdjustPosY(char board);//board 0- ext7  1- ext8  
void Input_seteVgaAdjustClock(char board,int clock);//board 0- ext7  1- ext8  
int Input_geteVgaAdjustClock(char board);//board 0- ext7  1- ext8  
void Input_seteVgaAdjustPhase(char board,int phase);//board 0- ext7  1- ext8  
int Input_geteVgaAdjustPhase(char board);//board 0- ext7  1- ext8  

void Input_geHardVersion(void);

void Input_resetInputPort(char port);
void Input_writePCA8574(char value);
char Input_readPCA8574(void);
void Input_taskTest8574(void);

void ResetExtInputSetting(void);

void setInputBoardOutputHDMIEnable(char port,bool enable);// port  0-182_1   1-182_2   2-ext_1   3-ext_2   4-ext_3
bool getInputBoardOutputHDMIEnable(char port);
bool getReadingUSBParanmsFlag(void);

void setInputBoardOutputEnableDataRange(char port,bool enable);// port  0-6 
bool getInputBoardOutputEnableDataRange(char port);


void enableInputBoardOutputAudio(void);
void disableInputBoardOutputAudio(void);

void relocationExtInputType_Audio(char port);
char Input_getExtInputTypeRelocation(char port);

BYTE Input_getOutputFormat(char board);//board 0- ext7  1- ext8  
void Input_setOutputFormat(char board,char format);//board 0- ext7  1- ext8  

void Input_setAutoChangeOutputFormatEnable(char port, bool enable);
bool Input_getAutoChangeOutputFormatEnable(char port);


void Input_setOutputAudioVolumn(char volumn);
char Input_getOutputAudioVolumn(void);

void Input_setAudioSwitchInPort(char inPort); //inPort 0-2
char Input_getAudioSwitchInPort(void);

char Input_isMstInput_V13(char port);
char Input_isDigitIputPort_V13(char port);
char Input_isHDMIIputPort_V13(char port);

char Input_selfCheckIC(int id);

#endif
