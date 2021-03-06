


#ifndef __KB_STATUS_H__
#define __KB_STATUS_H__


#include "stm32f4xx.h"
#include "define.h"
//#include "keyLed.h"


#define _SET_KB_HW_ID(chip,pin)           (((chip) << 8) | (pin))
#define _GET_KB_HW_ID(id,chip,pin)        {                           \
                                            chip = (id & 0x0000ff00) >> 8;   \
                                            pin = id & 0x000000FF;} 




#define KEY_GROUP_ID_SETTING            0
#define KEY_GROUP_ID_INPUT_SOURCE       1
#define KEY_GROUP_ID_LAYER              2
#define KEY_GROUP_ID_OUTPUT             3


//#define LED_ON_ALWAYS	1
//#define LED_OFF_ALWAYS	0

typedef enum
{
	LED_ON_ALWAYS = 0,
	LED_OFF_ALWAYS,
	LED_BLINK_50MS,
	LED_BLINK_100MS,
	LED_BLINK_200MS,
	LED_BLINK_500MS,
	LED_BLINK_1S,
	LED_STA_MAX_SIZE,
}keyLed_Mode;


typedef struct
{
	u8 brink_50ms;
	u8 brink_100ms;
	u8 brink_200ms;
	u8 brink_500ms;
	u8 brink_1s;
}brinkDef;


typedef struct
{
	u32 keyId;
	u32 ledId;
}Key_Id_Def;
	


typedef struct
{
    u32 ledId;      
    u32 rgb;        
    u8 color;       
    keyLed_Mode mode;
    u8 bright;
}KeyLed_StatusDef;


typedef struct 
{
	u8 key;	
	Key_Id_Def	id;
	KeyLed_StatusDef led;
	u8 key_reserve;			
	
}key_statusDef;


/*
 *   KB_Group_StatusDef
 *  这里用来表示一组软件上组合的按健组
*/
typedef struct
{
    u8 groupId;
    u8 KeyNum;
	key_statusDef * status;
}KB_Group_StatusDef;








typedef union
{
	u8 LedStatus;
	struct
	{
		u8 LedStatusBit0 : 1;
		u8 LedStatusBit1 : 1;
		u8 LedStatusBit2 : 1;
		u8 LedStatusBit3 : 1;
		u8 LedStatusBit4 : 1;
		u8 LedStatusBit5 : 1;
		u8 LedStatusBit6 : 1;
		u8 LedStatusBit7 : 1;
	}LedStatusBits;
}LedState;










extern u8 keyScan_Check_NoZeroData(u8 *data,u8 * idx);
extern u16 KB_keyIdToKey(u32 keyId);
extern u16 KB_TransTo_Key(u8 chip,u8 pin);
extern void TaskKeyLed(void);
extern void UpdateKeyLedStatus(void);
extern u32 KeyVlaueToLedId(u16 key);
extern keyLed_Mode GetKeyLedMode(u32 keyValue);
extern void SetKeyLedMode(u32 keyValue,keyLed_Mode mode);
extern void LED_Disp(u16 value);


#endif









































