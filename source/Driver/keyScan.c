#include "keyScan.h"
#include "D74HC165.h"
#include "D74HC595.h"
#include "i2c.h"
#include "systick.h"
#include "keyBoardStatus.h"
#include <stdio.h>
#include "rotaryEncoder.h"
#include "UartDebug.h"


//#define KEY_SCAN_DBG



static u8 keyValue = 0;
static KeyType_TypeDef keyType = KEY_TYPE_NONE;
static u32 preKeyValue = 0;
static u32 keyStartTime = 0;

static u32 keyContinueStart = 0;//added by cyj
static u32 keyContinueTime = 0;
static u32 keyPressHoldStatus = 0; //0 key all release  1 some key continue


	
#define KEY_STATE_INIT              0
#define KEY_STATE_SCAN_START        1
#define KEY_STATE_SCAN              2
#define KEY_STATE_DOWN              3
#define KEY_STATE_CONTINUE_TIMEOUT  4
#define KEY_STATE_CONTINUE          5


#ifdef KEYSCAN_BY_TIMER
#define KEY_SCAN_TIME           2
#define KEY_DOWN_TIME           5
#define KEY_CONTINUE_TIME       10
#define KEY_CONTINUE_TIME_OUT   200
#else
#define KEY_SCAN_TIME           10
#define KEY_DOWN_TIME           100
#define KEY_CONTINUE_TIME       30
#define KEY_CONTINUE_TIME_OUT   2000

#endif

//#define KEY_DOWN_TIME 50
//#define KEY_CONTINUE_TIME 400


//key_statusDef key;
static u32 totaryDelay = 100;


KeyScan_Drive_St keyScan_Drive;
keyScan_Value_St keyScan_Value;





//#################################################################################################
//


void TCA9555_Init(u8 portType)
{
	u16 INPUT_TYPE = 0XFFFF;
	u16 OUTPUT_TYPE = 0X0000;
	switch(portType)
	{
		case KEY_BOARD_SW:
			i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_SW_ADDRESS,TCA9555_CONFIG0_REG,(u8*)&INPUT_TYPE,2);
			//i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_SW_ADDRESS,TCA9555_CONFIG1_REG,0XFF);
		break;
		case KEY_BOARD_LED:
			i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_LED_ADDRESS,TCA9555_CONFIG0_REG,(u8*)&OUTPUT_TYPE,2);
			//i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_LED_ADDRESS,TCA9555_CONFIG1_REG,(u8*)&INPUT_TYPE,2);
			SysDelay1ms(5);
			//i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_LED_ADDRESS,TCA9555_OUTPUT0_REG,(u8*)&OUTPUT_TYPE,2);
		break;
		default:
			i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_SW_ADDRESS,TCA9555_CONFIG0_REG,(u8*)&INPUT_TYPE,2);
			//i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_SW_ADDRESS,TCA9555_CONFIG1_REG,0XFF);

			i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_LED_ADDRESS,TCA9555_CONFIG0_REG,(u8*)&OUTPUT_TYPE,2);
			//i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_LED_ADDRESS,TCA9555_CONFIG1_REG,(u8*)&INPUT_TYPE,2);
		break;
	}
	
}



void TestLed(void)
{
	u16 st = 1;
	char i = 0;
	for(i = 0;i < 16;i++)
	{
		i2cBurstWriteBytes(TCA9555_I2C_PORT, KEY_LED_ADDRESS,TCA9555_CONFIG1_REG,(u8*)&st,2);
		st <<= 1;
		SysDelay1ms(100);
	}
}

//#################################################################################################
//
void init_key(void)
{
	#if defined(USE_HC595_HC165)
	init_74HC165();
	init_74HC595();
	#elif defined(USE_TCA9555)

	TCA9555_Init(0xf);
	#endif
}


//#################################################################################################
//
u8 read_key_value(int countOf,u8 *data)
{
	#if defined(USE_HC595_HC165)
		return read_74HC165(countOf,data);
	#elif defined(USE_TCA9555)
	u8 *buf;
	countOf = countOf;
	i2cBurstReadBytes(TCA9555_I2C_PORT,KEY_SW_ADDRESS,TCA9555_INPUT0_REG,buf,2);
	data[0] = ~buf[0];
	data[1] = ~buf[1];
	return true;
	#endif
}




static void task_rotaryEncoder(void)//20111018
{
	static u32 readTime = 0;
	//static int rotaryStatus = 0;
	if(isSysTickTimeout(readTime,totaryDelay))
	{
		int value = GetpluseValue(pulse_A);

		readTime = GetSysTick();
		if(value)
		{
			if(value > 0)
			{
				DebugMessageKeyScan("rotaryEncoder_left\r\n");
				if(value < 5)
				{
					DebugMessageKeyScan("STEP_1 value %d\r\n",value);
				}
				else if(value < 8)
				{
					DebugMessageKeyScan("STEP_10 value %d\r\n",value);
				}
				else
				{
					DebugMessageKeyScan("STEP_100 value %d\r\n",value);
				}
			}
			else
			{
				DebugMessageKeyScan("rotaryEncoder_right\r\n");
				if(value > -5)
				{
					DebugMessageKeyScan("STEP_1 value %d\r\n",value);
				}
				else if(value > -8)
				{
					DebugMessageKeyScan("STEP_10 value %d\r\n",value);
				}
				else
				{
					DebugMessageKeyScan("STEP_100 value %d\r\n",value);
				}
			}
		}	
	}
}

//#################################################################################################
//

void key_Scan(KeyScan_Drive_St * keyScan_Drv,keyScan_Value_St * key_Value)
{
    u8 read_key[KEY_GROUP_MAX_CHIP];
	switch(keyScan_Drv->state)
	{
	case KEY_STATE_INIT:
#if defined(USE_HC595_HC165)
		init_74HC165();
		init_74HC595();
#elif defined(USE_TCA9555)
		init_key();
#endif
		keyScan_Drv->state = KEY_STATE_SCAN_START;
		break;
    case KEY_STATE_SCAN_START:
		keyScan_Drv->startTime = GetSysTick();
	    keyScan_Drv->state = KEY_STATE_SCAN;    
	break;
	case KEY_STATE_SCAN:
        if(isSysTickTimeout(keyScan_Drv->startTime, KEY_SCAN_TIME) == FALSE)
        {
            break;
        }
		if(read_key_value(KEY_GROUP_MAX_CHIP,read_key) == FALSE)
		{
            break;
        }

        keyScan_Drv->preValue = keyScan_Check_NoZeroData(read_key,&(keyScan_Drv->chip));
		
		if(keyScan_Drv->preValue != 0x00)
		{
			keyScan_Drv->startTime = GetSysTick();
			keyScan_Drv->state = KEY_STATE_DOWN;	
		}
		else
		{
			keyScan_Drv->holdStatus = 0;
            keyScan_Drv->state = KEY_STATE_SCAN; 
			//printf("no any key down\r\n");
		}
	break;
	case KEY_STATE_DOWN:
		if(read_key_value(KEY_GROUP_MAX_CHIP,read_key) == FALSE)
		{
            keyScan_Drv->state = KEY_STATE_SCAN_START;
            break;
        }		
		if(keyScan_Drv->preValue == read_key[keyScan_Drv->chip])
		{
			if(isSysTickTimeout(keyScan_Drv->startTime, KEY_DOWN_TIME))
			{
				keyScan_Drv->startTime = GetSysTick();
				keyScan_Drv->state = KEY_STATE_CONTINUE_TIMEOUT;
                key_Value->type = KEY_TYPE_DOWN;     
                key_Value->chip = keyScan_Drv->chip + 1;
                key_Value->value = keyScan_Drv->preValue;
            #if 1
                key_Value->key = KB_TransTo_Key(key_Value->chip,key_Value->value);
				keyValue = key_Value->key;
				keyType = key_Value->type;
            #else
                key_Value->key = keyScan_Get_Key(keyScan_Drv);
                #endif
                    
                //keyScan_ReportKey(key_Value->key,KEY_TYPE_DOWN);
                DebugMessageKeyScan("chip %d,value 0x%x Key down,key code = %x\r\n",key_Value->chip,key_Value->key,key_Value->value);
			}
		}
		else
		{
			keyScan_Drv->state = KEY_STATE_SCAN;
//			//LED_OFF_KEY(preKeyValue);
		}
		break;
	case KEY_STATE_CONTINUE_TIMEOUT:
    		if(read_key_value(KEY_GROUP_MAX_CHIP,read_key) == FALSE)
            {
                keyScan_Drv->state = KEY_STATE_SCAN_START;
                break;
            } 

            if(keyScan_Drv->preValue == read_key[keyScan_Drv->chip])    
    		{
    			if(isSysTickTimeout(keyScan_Drv->startTime, KEY_CONTINUE_TIME_OUT))
    			{
    				keyScan_Drv->startTime = GetSysTick();
                    
    				keyScan_Drv->state = KEY_STATE_CONTINUE;
                    key_Value->type = KEY_TYPE_CONTINUE;     
                    //key_Value->group = keyScan_Drv->group;  
                    key_Value->chip = keyScan_Drv->chip + 1;
                    key_Value->value = keyScan_Drv->preValue;                   
    				DebugMessageKeyScan("chip %d,value %x Timeout,key code =%x\r\n",key_Value->chip,key_Value->value,key_Value->key);
    			}
    		}
    		else
    		{
    			keyScan_Drv->state = KEY_STATE_SCAN_START;
                key_Value->type = KEY_TYPE_UP; 

                //keyScan_ReportKey(key_Value->key,KEY_TYPE_UP);
                
                DebugMessageKeyScan("chip %d,value %x Up,key code =%x\r\n",key_Value->chip,key_Value->value,key_Value->key);
    		}            
            break;
    	case KEY_STATE_CONTINUE:
    		if(read_key_value(KEY_GROUP_MAX_CHIP,read_key) == FALSE)
            {
                /* 这里是代表出错*/
                keyScan_Drv->state = KEY_STATE_SCAN_START;
                break;
            } 

            //if(keyScan_Check_DataChange(read_key,keyScan_Drv->preValue) == FALSE)	
            if(keyScan_Drv->preValue == read_key[keyScan_Drv->chip])     
    		{
    			if(isSysTickTimeout(keyScan_Drv->startTime, KEY_CONTINUE_TIME))
    			{
    				keyScan_Drv->startTime = GetSysTick();  
    				keyScan_Drv->state = KEY_STATE_SCAN_START;
                    key_Value->type = KEY_TYPE_UP;     
					//keyScan_Drv->holdStatus = 1;
                    //key_Value->group = keyScan_Drv->group;  
                    key_Value->chip = keyScan_Drv->chip + 1;
                    key_Value->value = keyScan_Drv->preValue;     
                    //keyScan_ReportKey(key_Value->key,KEY_TYPE_CONTINUE);
					keyValue = key_Value->key;
					keyType = key_Value->type;
					//keyScan_Drv->holdStatus = 0;
                    DebugMessageKeyScan("chip %d,value %x Continue,key code =%x\r\n",key_Value->chip,key_Value->value,key_Value->key);
    			}
    		}
    		else
    		{
    			keyScan_Drv->state = KEY_STATE_SCAN_START;
				//keyScan_Drv->startTime = GetSysTick();
                key_Value->type = KEY_TYPE_UP;  
				keyScan_Drv->holdStatus = 0;
				keyScan_Drv->startTime = 0;
                //keyScan_ReportKey(key_Value->key,KEY_TYPE_UP);
                
                DebugMessageKeyScan("chip %d,value %x uP,key code =%x\r\n",key_Value->chip,key_Value->value,key_Value->key);
    		}  

    		break;
    	default:
    		keyScan_Drv->state = KEY_STATE_INIT;
    		break;		
	}
}



void task_Key_Scan(void)
{
	//task_rotaryEncoder();
    key_Scan(&keyScan_Drive,&keyScan_Value);
}

//#################################################################################################
//
u32 get_keyPressHoldStatus(void)
{
	return keyScan_Drive.holdStatus;
}

u32 set_keyPressHoldStatus(u8 status)
{
	keyScan_Drive.holdStatus = status;
}



//#################################################################################################
//
u32 get_keyContinueTime(void)
{
	u32 res;
	res = keyContinueStart;//keyContinueTime;
	keyContinueTime = 0;

	return res;
}




//#################################################################################################
//
void set_keyContinueStart(u32 time)
{
	keyContinueStart = time;
}




//#################################################################################################
//
u32 get_key(void)
{
	u32 result;
	result = keyValue;//keyScan_Value.key;
	keyValue = 0;
	return result;
}




//#################################################################################################
//
BYTE get_keyType(void)
{
	BYTE result; 
	result = keyType;//keyScan_Value.type;	
	keyType = KEY_TYPE_NONE;
	return result;
}




//#################################################################################################
//
void setVirKey(u32 value)
{
	keyValue = value;
	keyType = KEY_TYPE_DOWN;
}

u32 KeyChange(u32 key)
{
	switch(key)
	{
		case 1:return KEY_DP;
		case 2:return KEY_VGA;
		case 3:return KEY_M4;
		case 4:return KEY_TAKE;
		case 5:return KEY_M3;
		case 6:return KEY_SAVE;
		case 7:return KEY_SEL;
		case 8:return KEY_SCALE;
		case 9:return KEY_SPLIT;
		case 10:return KEY_MENU;
		case 11:return KEY_M2;
		case 12:return 0xff;
		case 13:return KEY_M1;
		case 14:return KEY_DVI;
		case 15:return KEY_HDMI2;
		case 16:return KEY_HDMI1;
		default:return 0xff;
	}
}


