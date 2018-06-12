/*
 * rs232command.h
 *
 *  Created on: 2009-5-14
 *      Author: Administrator
 */

#ifndef RS232COMMAND_H_
#define RS232COMMAND_H_

#include "define.h"
#include "stm32f4xx.h"
#include "rgblinkProtocol.h"


#ifdef _RS232COMMAND_C_
    #define INTERFACE
#else
    #define INTERFACE extern
#endif

// define for FPGA program saved in SPI flash
extern u8 FPGA_GUID[16];
extern u8 FPGA_END_SIGN[4];
#define FPGA_END_SIGN_OFFSET (0x10)
#define FPGA_FLASH_ADDRESS_OFFSET (0x00)
#define FPGA_FLASH_SIZE (0x800000)  			// 4MB
#define FPGA1_FLASH_ADDRESS_OFFSET (0x400000)
#define FPGA1_FLASH_SIZE (0x400000)  			// 4MB

#define UPDATE_BlOCK_SIZE (1024)

#define SOFT_VERSION_HIGH  	1  
#define SOFT_VERSION_LOW	01

#define RTDVIDEO_VERSION_HIGH  1  
#define RTDVIDEO_VERSION_LOW	01

#define FONT_VERSION_HIGH  1  
#define FONT_VERSION_LOW	1

#define EXT1_VERSION_HIGH  	0  
#define EXT1_VERSION_LOW	0

#define EXT2_VERSION_HIGH  	0  
#define EXT2_VERSION_LOW	0

//#define VIDEO_VERSION_HIGH  1  
//#define VIDEO_VERSION_LOW	2

#define HARD_VERSION_HIGH	0
#define HARD_VERSION_LOW	1

#define EXT1_HARD_VERSION_HIGH	0
#define EXT1_HARD_VERSION_LOW	0

#define EXT2_HARD_VERSION_HIGH	0
#define EXT2_HARD_VERSION_LOW	0

//1.21优化输入分辨率显示慢的问题,并修复上一版本引起的自定义EDID菜单的BUG
//1.20四口拼接下增加了垂直一分二模式
//1.09FPGA做水平一分四放大
//1.08增加了输出的4个DVI接口可以任意切换的功能

/*
 * RS232 COMMANDS LED-DIRECTOR
 */

#define P_DATA_LENGTH	256

#pragma pack(1) //让编译器对这个结构作1字节对齐

#if 0
typedef struct 
{
	BYTE pBegin;
	BYTE pVersion;
	WORD pDataLen;
	WORD pCmd;
	u32  pAddr;
	BYTE pFunc1;
	BYTE pFunc2;
	BYTE pFunc3;
	BYTE pFunc4;
	WORD pCmdCheckSum;
	BYTE pDatabuf[P_DATA_LENGTH];
	WORD pCrc16;
	BYTE pEnd;
}stControlProtocal;
#else
typedef struct 
{
	BYTE pBegin;
	BYTE pCmd;
	BYTE pAddr;
	BYTE pReserve0;
	BYTE pReserve1;
	BYTE pFunc1;
	BYTE pFunc2;
	BYTE pFunc3;
	BYTE pFunc4;
	BYTE pSum;
	//BYTE pDatabuf[P_DATA_LENGTH];
	//WORD pCrc16;
	BYTE pEnd;
}stControlProtocal;
#endif

#pragma pack() //取消1字节对齐，恢复为默认4字节对齐

extern stControlProtocal st_ControlProtocal;

#define STX (0x02)
#define ACK (0x06)
#define NACK (0x15)
#define VC_SUCCESS (1)
#define NO_SUCCESS (0)

#define VC_ADDRESS (0x00)

typedef enum 
{
	COMMAND_FROM_UART,  // 0
	COMMAND_FROM_UDP,   // 1
	COMMAND_FROM_USB,   // 2
	COMMAND_FROM_WIFI,  // 3
} CommandFrom_Type;


//static void SendAckOrNack(BYTE value);

enum _RS232_COMMAND
{
	W_COMPANY_LOGO	   		= 0x68,
	WR_DEVICE_REGISTER 		= 0x69,
	WR_HDMIOUT_DE 	   		= 0x86,
	WR_OTHER_FUN       		= 0x87,
	W_IP_ADDRESS	   		= 0x88,
	R_IP_ADDRESS	   		= 0x89,
	SOFTWARE_UPDATE 		= 0x9A,
	WR_OUTPUT_FORMAT 		= 0xA0,
	WR_INPUT_SOURCE 		= 0xA1,
	R_LED_VERSION 			= 0x6A,
	WR_DEVICE_SETTING 		= 0x6B,
	W_NAME1_FRONT4 			= 0x70,
	W_NAME1_BACK4 			= 0x71,
	W_NAME2_FRONT4 			= 0x72,
	W_NAME2_BACK4 			= 0x73,
	WR_NETWORK_CONTROL 		= 0X8E,
	W_Sync					= 0xF0,
	R_Sync					= 0xF1,
	EDIT_EDID 				= 0xf2,
	R_PARAM 				= 0xF3,
	W_PARAM 				= 0xF4,
	WR_LAYER_PARAM 			= 0x75,
} ;

/*
* write/read i2c register value of some slaver address device
*/
//WR_DEVICE_REGISTER
void WrDeviceRegisterTask(void);

/*
* software update 
*/
// SOFTWARE_UPDATE
void SoftwareUpdate(void);

void WrOutputFormat(void);
void WrLayerSetting(void);
void WrLayerSetting_splitJoint(void);
void WrOutputSetting(void);
void WrInputSource(void);
void WRInputSourceTask(void); //0x72
void WrInputSetting(void);
void WrSwitchSetting(void);
void Wr90degreeRotationSetting(void);

//write/read param
void ReadParameterTask(void);  //0xf1
void WriteParameterTask(void);
void EditEDIDTask(void);
void ExtInputBoardTask(void);  //0x69
void WRLayerParameterTask(void); //0x75


/*
 * read  version write serial number
 */
void RLedVersionTask(void);

void WrVSP320RegisterTask(void);

void ReadHdmiATask(void); 
void ReadHdmiBTask(void); 
void WRVSP330SaveTask(void);
void WRHdmiOutDeTask(void);
void WROtherFunTask(void);

void WriteCompanyLogoTask(void);

// cmd == 0x90
void VSP320_CmdTask(void);


void processCmdFromLocal(BYTE cmd, BYTE dat1, BYTE dat2, BYTE dat3, BYTE dat4);
void processCmdFromLocal_USB_X1(BYTE usbPort, BYTE cmd, BYTE dat1, BYTE dat2, BYTE dat3, BYTE dat4);


typedef struct _rs232commandToLED
{
	BYTE startbyte;
	BYTE offset;
	WORD address;  // 确认命令的格式
	BYTE command;
	BYTE data_1;
	BYTE data_2;
	BYTE data_3;
	BYTE data_4;
	BYTE checksum;
} Rs232CommandToLED;


typedef struct _rs232commandToLEDNew
{
	BYTE address;
	BYTE sn;
	BYTE command;
	BYTE data_1;
	BYTE data_2;
	BYTE data_3;
	BYTE data_4;
	BYTE checksum;
} Rs232CommandToLEDNew;


void SendCmdToPC(Rs232CommandToLED *cmd);
static u8 CheckSum(Rs232CommandToLED *rsCom);

// 读取ASCII命令串，转换为Rs232CommandToLED结构
// 要求ASCII命令串结构是正确的。
// ASCII命令串  :  ":TOVC协议数据\r\n"
void StrToRs232Command(char *str, Rs232CommandToLED *rsCom);

typedef Command_TypeDef Rs232CommandFromLED;


/*
 * Function: InitRs232CommandVariable
 * Usage: InitRs232CommandVariable();
 * ----------------------------------------
 * 初始化全局变量
 */
INTERFACE void InitRs232CommandVariable(void);
INTERFACE int IsUpdateSoftware(void);

//static void SetCheckSum(Rs232CommandFromLED *rsFromLED);


/*
 * Function: Rs232CommandToStr
 * Usage: Rs232CommandToStr(rsFromLED, str);
 * In: rsFromLED
 * Out: str
 * ---------------------------------------------
 * 1. 计数checksum
 * 2. 把Rs232CommandFromLED结构体打包为字符串
 * ASCII命令串  :  ":FRVC协议数据\r\n"
 * 要求事先要把协议数据以外的内容填入。
 */
void Rs232CommandToStr(Rs232CommandFromLED *rsFromLED, char *str);

//void putchar11(BYTE ucVal);  // for test

void StrToRs232Command(char *str, Rs232CommandToLED *rsCom);

// char2由两个char组成 把两个ASCII码转换为一个BYTE
 BYTE HexCharToByte(char *char2);

/*
 * Function: ByteToChar
 * Usage: ByteToChar(value, char2);
 * -------------------------------------------
 * char2指向有两个char组成的数组
 * 把一个BYTE转换为两个ASCII码
 */
char * ByteToHexChar(BYTE value, char *char2);

/*
 * Function: TaskCommand
 * Usage: TaskCommand(str);
 * -------------------------------------------
 * 1. 解析命令字符串；
 * 2. 做相应的处理；
 * 3. 回复命令字符串；
 */
INTERFACE void TaskCommand(void);

/*
 * Function: IsrReceiveCommand
 * Usage: IsrReceiveCommand
 * ---------------------------------------
 * 此函数在uart中断处理函数中调用，以接受命令。
 */
INTERFACE void IsrReceiveCommand(int from, BYTE value);

INTERFACE u16 GetChecksumFromFlash(u32 address, u32 size);

bool isValidFpgaFile(void);
unsigned char isValidVideoFileHeader(u8 *buffer);   // buffer have 129 byte


void WrLayOutMode(void);
void WrSplitMode(void);
void WrSyncData(void);
void WrCurMode(void);
void WrTpMode(void);
void WrMaxAlpha(void);


void SendBufferToPC_ByMeduim(u8 * buffer, int size,char meduim);
void setCommandToLEDNew(char from, char address,char sn,char cmd,char dat1,char dat2, char dat3, char dat4);
void processCmdData(void);
void Debug_Printf(u8 *buffer,u16 len,u8 setStart);
char TaskPcControl(u8 *buf);
void PcDebugTask(void);


#ifdef _useArtNet
extern void SetNetMask(u8 *IpAdr);
extern void SetLocalIpAddress(u8 *IpAdr);
extern void SetRemoteIpAddress(u8 *IpAdr);
extern void GetRemoteIpAddress(u8 *IpAdr);
extern void GetLocalIpAddress(u8 *IpAdr);
extern void GetNetMask(u8 *IpAdr);
extern void SetDefGW(u8 *IpAdr);
extern void GetDefGW(u8 * IpAdr);
#endif


void SendRs232Command(void);

#undef INTERFACE

#endif /* RS232COMMAND_H_ */

