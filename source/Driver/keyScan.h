#ifndef _KEYSCAN_H_

#define _KEYSCAN_H_

#include "define.h"
#include "userPref.h"
//#include "keyBoardStatus.h"

#define ALL_KEY_NUMBER (19)
#define NUMBER_OF_BYTE ((ALL_KEY_NUMBER+7)/8)
#define NUMBER_OF_BYTE_LED (5)


#define KEY_GROUP_MAX_CHIP      2
#define KEY_GROUP_MAX_KEY       8
//#define KEY_GROUP_MAX_KEY       8

#define USE_TCA9555


#define TCA9555_I2C_PORT	I2C_J

#define KEY_BOARD_SW		0
#define KEY_BOARD_LED		1

#define KEY_SW_ADDRESS		(0X20 << 1)
#define KEY_LED_ADDRESS		(0X21 << 1)


#define TCA9555_INPUT0_REG		0X00
#define TCA9555_INPUT1_REG		0X01
#define TCA9555_OUTPUT0_REG		0X02
#define TCA9555_OUTPUT1_REG		0X03
#define TCA9555_POLARITY0_REG	0X04
#define TCA9555_POLARITY1_REG	0X05
#define TCA9555_CONFIG0_REG		0X06
#define TCA9555_CONFIG1_REG		0X07


typedef enum
{
	KEY_COLOR_R = 0,
	KEY_COLOR_Y = 1,
	KEY_COLOR_O = 2,
}Key_Color_TypeDef;





typedef struct
{
    u8 chip;
	u8 state;
    u8 holdStatus;
	u32 startTime;
	u8 preValue;
    u8 keyValue;

    u8 keyMax;
    u32 keyId[KEY_GROUP_MAX_KEY];
    u16 key[KEY_GROUP_MAX_KEY];
} KeyScan_Drive_St;


typedef struct
{
	u8 chip;  
	u8 value; 
    u16 key;
    u8 type;
}keyScan_Value_St;


typedef enum 
{


	KEY_2_4K = 0,
//################################################
// menu and fountion 
	KEY_SAVE 			= 1,
	KEY_LOAD 			= 2,
	KEY_MENU 			= 3,
	KEY_SPLIT 			= 4,
	KEY_SCALE 			= 5,
	KEY_DP 				= 6,
	KEY_SEL 			= 7,
	KEY_M1 				= 8,
	KEY_M2 				= 9,
	KEY_M3 				= 10,
	KEY_M4 				= 11,
	KEY_TAKE 			= 12,
	KEY_VGA 			= 13,
	KEY_DVI  			= 14,
	KEY_HDMI2 			= 15,
	KEY_HDMI1 			= 16,
	

//################################################
//layer operating	
	KEY_LAYER_A,
	KEY_LAYER_B,
	KEY_LAYER_C,
	KEY_LAYER_D,
	KEY_LAYER_E,
	KEY_LAYER_F,
	KEY_LAYER_G,
	KEY_LAYER_H,

//################################################
//output 	
	KEY_OUTPUT1,
	KEY_OUTPUT2,
	KEY_OUTPUT3,
	KEY_OUTPUT4,

	KEY_SIZE,
	
	//KEY_MENU 	= 0x00000001,		//G3-NET 前面板
	//KEY_SCALE	= 0x00000002,		//G3-NET 前面板
	KEY_PIP     = 0x01000000,		//G3-NET 前面板
	KEY_MAP     = 0x02000008,		//G3-NET 前面板
	KEY_HDMI 	= 0x03000080,		//G3-NET 前面板
	KEY_SDI 	= 0x04000040,		//G3-NET 前面板
	//KEY_DVI 	= 0x00000020,		//G3-NET 前面板
	KEY_BLACK   = 0x05000010,		//G3-NET 前面板
	//KEY_TAKE    = 0x00000100,		//G3-NET 前面板
	//KEY_SEL		= 0x00000200,		//G3-NET 前面板
	KEY_REMOTE	= 0x00000400,		// this is a virtual key
	
	
	//KEY_SAVE	= 0x00004000,
	//KEY_LOAD    = 0x00000400,
	KEY_FS		= 0x00020000,
	KEY_1OR2    = 0x00001000,
	//KEY_SPLIT	= 0x00002000,
	#if 0
	KEY_CV1		= 0x00000200,
	KEY_CV2		= 0x00000100,
	KEY_CV3 	= 0x00000010,
	KEY_VGA		= 0x00000008,
	KEY_DVI 	= 0x00000004,	
	
	KEY_DP	    = 0x00004000,
	KEY_EXT_7 	= 0x00008000,
	KEY_EXT_8 	= 0x00000020,
	KEY_EXT_9	= 0x00000040,
	KEY_BLACK   = 0x00000080,
	#else
	//KEY_DP	    = 0x00000200,		
	KEY_CV1		= 0x00000800,
	KEY_CV2		= 0x00000100,		
	KEY_CV3 	= 0x01100010,
	//KEY_VGA		= 0x00011100,	
	KEY_EXT_7 	= 0x00008000,
	KEY_EXT_8 	= 0x00010000,
	KEY_EXT_9	= 0x00000080,
	#endif
	
	
	KEY_INFO_V     	 	= 0x10010000,
	KEY_MB_SOURCE_V		= 0x10020000,
	KEY_DEVICE_INFO_V	= 0x10040000,
	KEY_DEVICE_INIT_V   = 0x10080000,
	KEY_SD_INPUT_EXHANVE_V	= 0x10100000,

} KeyValue_TypeDef;

typedef enum
{
	KEY_TYPE_NONE,
	KEY_TYPE_DOWN,
	KEY_TYPE_UP,
	KEY_TYPE_CONTINUE,
} KeyType_TypeDef;



#define LED_DVI		KEY_DVI
#define LED_HDMI1	KEY_HDMI1
#define LED_HDMI2	KEY_HDMI2
#define LED_DP		KEY_DP

#if defined(LING_XIN)
#define LED_VGA		KEY_VGA
#else
#define LED_VGA		KEY_M1
#endif



extern KeyScan_Drive_St keyScan_Drive;
extern keyScan_Value_St keyScan_Value;

void setVirKey(u32 value);
	
void init_key(void);
void task_Key_Scan(void);
u32 get_key(void);
BYTE get_keyType(void);
void led_horse_race(void);

//void LED_ON_KEY(u64 keyValue);
//void LED_OFF_KEY(u64 keyValue);

//void LED_Disp(u64 value);

//void LED_ON(u64 value);
//void LED_OFF(u64 value);
void setDispLED(u64 disp,u64 hide);


Key_Color_TypeDef getKeyColorBaseKeyValue(u64 value);
void setKeyColor(u64 value,Key_Color_TypeDef color);
void setAllKeyRed(void);
u32 get_keyPressHoldStatus(void);
u32 set_keyPressHoldStatus(u8 status);
u32 get_keyContinueTime(void);
void set_keyContinueStart(u32 time);
u32 KeyChange(u32 key);

void CheckSlikScreenVersion(void) ; //因为需要根据扫描的键值判断丝印版本,所以一定要在init_key()之后调用
char getSlikScreenVersion(void);

#endif // _KEYSCAN_H_
