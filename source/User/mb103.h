#ifndef _MB103_H_
#define _MB103_H_

#include "define.h"
#include "userpref.h"


void setMB103_outputEnable(char port);//0(TTL)-1(HDMI)  
void setMB103_outputDisable(char port);//0(TTL)-1(HDMI)  

void refleshMainBoardInfo_fans(void);
void ResetMainBoardSetting(void);


enum
{
	CMD_DATA_1,
	CMD_DATA_2,
	CMD_DATA_3,
	CMD_DATA_4,
	CMD_DATA_VER_LSB,
	CMD_DATA_VER_MSB,
	CMD_DATA_SIZE,
};


typedef struct 
{
	BYTE switchStatus[2];//��������ڵ�����״̬   0- ����DVI   1- �����182_1    2- �����182_2    3- ����嵥��DVI	
	BYTE outputEnable[2];//��������ڵ����״̬	
	u8 ver_msb;
	u8 ver_lsb;
	u8 hardVer;
	int edidLen[6]; //���������,����DVI��,VGA�����,VGA����� ��HDMI����ڵ�EDID ����          0- input port DVI  1- output port 0  2- output port 1  3- input port VGA  4- output port VGA  5- input port HDMI(����V1.3������)
	u8 edidDetail[6][256]; //���������,����DVI��,VGA�����,VGA����� ��HDMI����ڵ� EDID����  0- input port DVI  1- output port 0  2- output port 1  3- input port VGA	4- output port VGA   5- input port HDMI(����V1.3������)
	char fansDutyRatio;
	char autoDuty;
	char temp;
	char regValue;
	char hdmiSwitch[2]; //����331������  0- DVI�����331     1-OUTPUT1ǰ�˵�331 
	char hdmiSwitchEQ[2][3]; //����331������eq         0- DVI�����331     1-OUTPUT1ǰ�˵�331   
	char hdmiSwitchDrv[2]; //����331�����driver   0- DVI�����331     1-OUTPUT1ǰ�˵�331 
	char hdmiPortAudioEnble; //
}MainBoard_Setting_TypeDef;

typedef enum
{
	MB_EDID_INPUT_DVI  = 0,
	MB_EDID_INPUT_DP = MB_EDID_INPUT_DVI,     //�������V1.3������
	MB_EDID_OUTPUT_0   = 1,
	MB_EDID_OUTPUT_1   = 2,
	MB_EDID_INPUT_VGA  = 3,
	MB_EDID_OUTPUT_VGD = 4,
	MB_EDID_INPUT_HDMI = 5,
	MB_EDID_PORT_SIZE,
}MB_EDID_PORT_TYPEDEF;

MainBoard_Setting_TypeDef * getMb103Setting(void);
void Isr_receive_mb103(u8 value);
void task_mb103(void);
void init_mb103(void);
int mb103_sendProgramMcuBuffer(int address, u8 *buffer, int size);
void mb103_send(int cmd, int dat1, int dat2, int dat3, int dat4);
bool mb103_sendAndWait(int cmd, int dat1, int dat2, int dat3, int dat4);
void mb103_readDeviceType(void);
int mb103_goToBoot(void);
void mb103_setSwitch(u8 output,u8 input);
void mb103_setSwitch_noOutput(u8 output,u8 input);//output 0(TTL)-1(HDMI)             input 0-3   0- ����DVI   1- �����182_1    2- �����182_2    3- ����嵥��DVI
u8 mb103_getSwitch(u8 output);//output 0(TTL)-1(HDMI)             input 0-3  
u8 getMB103_inputSource(u8 output);//output 0(TTL)-1(HDMI)         input 0- dvi in mainboard   1- ext_7   2- ext_8  3- ext_9
void mb103_setOutputEnable(u8 output,u8 enable);//output 0(TTL)-1(HDMI)            
int getMB103_SoftVersion(void);

void setMB103_fansDutyRatio(char value); // 1-99
char getMB103_fansDutyRatio(void);
void setMB103_AutomaticDuty(char value);//1- auto   0- Manual
char getMB103_AutomaticDuty(void);
char getMB103_CurrentTemperature(void);


u8 getdeViceIsConnet(void);
void resetSiI9134_out(char port);
void mb_resetIT6604(void);
void mb_resetTh8200(void);
void mb_resetAD4604(void);

void getMB103_EDID(char port); //0- input port DVI  1- output port 0  2- output port 1
void setMB103_EDID(char port,u8 *edidData,int len);//port 0- input port DVI  


#define MB103_HDMISWITCH_PORT_INPUT  0
#define MB103_HDMISWITCH_PORT_OUTPUT  1

//input port 
#define MB103_INPUT_DVI        0 //V1.3���ϵ�DVI�ӿ�
#define MB103_INPUT_HDMI       1 //V1.3���ϵ�HDMI�ӿ�
#define MB103_INPUT_RESERVED   2

//output port 
#define MB103_OUTPUT_CORE      0
#define MB103_OUTPUT_DVI       1
#define MB103_OUTPUT_RESERVED  2

void setMB103_HDMIPortOutputHDMI(void) ;
void setMB103_HDMIPortOutputDVI(void) ;

typedef enum
{
	HDMI_SWITCH_INPUT,
	HDMI_SWITCH_OUTPUT,
	HDMI_SWITCH_MAX_SIZE,
}HDMI_SWITCH_TYPEDEF;

typedef enum
{
	HDMI_SWITCH_INPUT_PORT_DVI,
	HDMI_SWITCH_INPUT_PORT_HDMI,
	HDMI_SWITCH_INPUT_PORT_3,
	HDMI_SWITCH_INPUT_PORT_MAX_SIZE,
}HDMI_SITCH_INPUT_PORT_TYPEDEF;

//PS331_INPUT����   : input_port 0- dvi����          input_port 1-HDMI����    input_port 2-Ԥ��
//PS331_OUTPUT����: input_port 0-8028���        input_port 1- dvi��·       input_port 2-Ԥ��
void setMB103_HDMISwitch(char port, char inputPort); //port 0-1             inputPort  0-2
char getMB103_HDMISwitch(char port); //port 0-1             inputPort  0-2
void setMB103_InputSwitchEQ(char port,char inputPort, char eqValue); //port 0-1 	 inputPort	0-2 	   eqValue	0-f 		
char getMB103_InputSwitchEQ(char port,char inputPort); //port 0-1	   inputPort  0-2		 eqValue  0-f			
void setMB103_InputSwitchDriver(char port, char drvValue); //port 0-1			 drvValue  0-3			
char getMB103_InputSwitchDriver(char port); //port 0-1			  drvValue	0-3

void setMB103_iicRegister(char chipAddr, char regAddr,char value);
char getMB103_iicRegister(char chipAddr, char regAddr); 

void mb103_setEnableFlashWP(void);
void mb103_setDisableFlashWP(void);

#endif		// #ifndef _MB103_H_


