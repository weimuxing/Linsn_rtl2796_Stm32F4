
#include <string.h>
#include <stdio.h>
#include <math.h>


#include "systick.h"
//#include "SysConfig.h"
#include "keyBoardStatus.h"
#include "keyScan.h"

#include "D74HC595.h"
#include "D74HC165.h"
#include "uart.h"
#include "UartDebug.h"
//#include "keyDef.h"







brinkDef stBrinkStatus;

LedState _ledDataBuf[MAX_595_NUMS];

KeyLed_StatusDef keyLedStatus[32];

key_statusDef KB_Region_Setting_keyStatus[8] = 
{
    //KEY_MENU
    {
        .key = KEY_DP,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN0),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN0),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN0),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_SCALE
    {
        .key = KEY_VGA,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN1),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN1),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN1),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_SAVE
    {
        .key = KEY_M3,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN2),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN2),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN2),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_SPLIT
    {
        .key = KEY_M4,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN3),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN3),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN3),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LOAD
    {
        .key = KEY_TAKE,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN4),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN4),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN4),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_TAKE
    {
        .key = KEY_LOAD,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN5),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN5),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN5),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },

	//KEY_VGA
    {
        .key = KEY_SAVE,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN6),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN6),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,_74HC595_PIN6),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_SELECTE
    {
        .key = KEY_SEL,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN7),
            .ledId = 0,
        },
        .led.ledId = 0,//_SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN7),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
};



KB_Group_StatusDef KB_Group_Setting_Status = 
{
    .groupId = KEY_GROUP_ID_SETTING,
	.KeyNum = sizeof(KB_Region_Setting_keyStatus)/sizeof(key_statusDef),
	.status = KB_Region_Setting_keyStatus,
};



key_statusDef KB_Region_InputSource_keyStatus[8] = 
{
    //KEY_DVI
    {
        .key = KEY_SCALE,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN0),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN0),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN0),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_HDMI1
    {
        .key = KEY_SPLIT,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN1),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN1),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN1),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_HDMI2
    {
        .key = KEY_MENU,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN2),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN2),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN2),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_DP
    {
        .key = KEY_M2,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN3),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN3),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN3),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_M1
    {
        .key = KEY_M1,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN4),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN4),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN4),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_M2
    {
        .key = KEY_DVI,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN5),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN5),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN5),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_M3
    {
        .key = KEY_HDMI2,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN6),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN6),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN6),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_M4
    {
        .key = KEY_HDMI1,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN7),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN7),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN7),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    
    #if 0
    //KEY_VGA
    {
        .key = KEY_VGA,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN3),
            .ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN3),
        },
        .led.ledId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,_74HC595_PIN3),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    #endif
};




KB_Group_StatusDef KB_Group_InputSource_Status = 
{
    .groupId = KEY_GROUP_ID_INPUT_SOURCE,
	.KeyNum = sizeof(KB_Region_InputSource_keyStatus)/sizeof(key_statusDef),
	.status = KB_Region_InputSource_keyStatus,
};

key_statusDef KB_Region_Layer_keyStatus[8] = 
{
    //KEY_LAYER_A
    {
        .key = KEY_LAYER_A,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN2),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN2),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN2),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_B
    {
        .key = KEY_LAYER_B,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN1),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN1),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN1),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_C
    {
        .key = KEY_LAYER_C,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN0),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN0),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN0),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_D
    {
        .key = KEY_LAYER_D,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN3),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN3),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN3),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_E
    {
        .key = KEY_LAYER_E,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN5),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN5),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN5),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_F
    {
        .key = KEY_LAYER_F,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN6),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN6),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN6),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_G
    {
        .key = KEY_LAYER_G,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_2,HC165_PIN7),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN7),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_2,_74HC595_PIN7),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_H
    {
        .key = KEY_LAYER_H,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN4),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN4),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN4),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
};



KB_Group_StatusDef KB_Group_Layer_Status = 
{
    .groupId = KEY_GROUP_ID_LAYER,
	.KeyNum = sizeof(KB_Region_Layer_keyStatus)/sizeof(key_statusDef),
	.status = KB_Region_Layer_keyStatus,
};

key_statusDef KB_Region_Output_keyStatus[4] = 
{
    //KEY_LAYER_A
    {
        .key = KEY_OUTPUT1,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN2),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN2),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN2),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_B
    {
        .key = KEY_OUTPUT2,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN1),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN1),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN1),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_C
    {
        .key = KEY_OUTPUT3,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN5),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN5),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN5),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
    //KEY_LAYER_D
    {
        .key = KEY_OUTPUT4,
        .id = 
        {
            .keyId = _SET_KB_HW_ID(D74HC165_CHIP_IDX_1,HC165_PIN6),
            .ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN6),
        },
        .led.ledId = _SET_KB_HW_ID(_74HC595_CHIP_IDX_1,_74HC595_PIN6),
        .led.rgb = 0,
		.led.color = 0,
		.led.mode = LED_OFF_ALWAYS,
		.led.bright = 100,
        .key_reserve = 0,
    },
};

KB_Group_StatusDef KB_Group_Output_Status = 
{
    .groupId = KEY_GROUP_ID_OUTPUT,
	.KeyNum = sizeof(KB_Region_Output_keyStatus)/sizeof(key_statusDef),
	.status = KB_Region_Output_keyStatus,
};



KB_Group_StatusDef * KB_KeyGroup_Get(u8 groupId)
{
    KB_Group_StatusDef * keyGroup = NULL;

    switch(groupId)
    {
        case KEY_GROUP_ID_SETTING:
            keyGroup = &KB_Group_Setting_Status;
            break;
        case KEY_GROUP_ID_INPUT_SOURCE:
            keyGroup = &KB_Group_InputSource_Status;
            break;
        case KEY_GROUP_ID_LAYER:
            keyGroup = &KB_Group_Layer_Status;
            break;
        case KEY_GROUP_ID_OUTPUT:
            keyGroup = &KB_Group_Output_Status;
            break;
        default:
            break;
    }
    
    return keyGroup;
}

u8 keyScan_Check_NoZeroData(u8 *data,u8 * idx)
{
    u8 result = 0x00;
    u8 i;

    for(i = 0; i < KEY_GROUP_MAX_CHIP ; i++)
    {
        if(data[i] != 0x00 && data[i] != 0xFF)
        {
            (*idx) = i;
            result = data[i];
            break;
        }
    }

    return result;
}



u16 KB_keyIdToKey(u32 keyId)
{
    KB_Group_StatusDef * KeyGroup;
    key_statusDef * status;	
    u8 i,j;
	//u16 keyval;

    for(i = KEY_GROUP_ID_SETTING; i <= KEY_GROUP_ID_OUTPUT;i++)
    {
 	
        KeyGroup = KB_KeyGroup_Get(i);
    	if(KeyGroup == NULL)
	    {
	        continue;
	    } 
        status = KeyGroup->status;
        if(status == NULL)
	    {
	        continue;
	    } 
        for(j = 0;j < KeyGroup->KeyNum;j++)
        {
            if(keyId == status[j].id.keyId)
            {
				//keyval = status[j].key;
				#if 0
				status[j].led.mode++;
				if(status[j].led.mode >= LED_BLINK_100MS)
					status[j].led.mode = LED_ON_ALWAYS;
				//SetKeyLedMode(key_id,mode);
				//UpdateKeyLedStatus(0,keyval,status[j].led.mode);
				#endif
				
                return status[j].key;
            }
        }

	}

    return KEY_2_4K;  
}



void SetKeyLedMode(u32 keyValue,keyLed_Mode mode)
{
	KB_Group_StatusDef * KeyGroup;
    key_statusDef * status;	
    u8 i,j;
	u32 ledid;
	ledid = KeyVlaueToLedId(keyValue);
	for(i = KEY_GROUP_ID_SETTING; i <= KEY_GROUP_ID_OUTPUT;i++)
    {
 	
        KeyGroup = KB_KeyGroup_Get(i);
    	if(KeyGroup == NULL)
	    {
	        continue;
	    } 
        status = KeyGroup->status;
        if(status == NULL)
	    {
	        continue;
	    } 
        for(j = 0;j < KeyGroup->KeyNum;j++)
        {
            if(ledid == status[j].led.ledId)
            {
                //return status[j].key;
                status[j].led.mode = mode;
								return ;
            }
        }

	}
}



keyLed_Mode GetKeyLedMode(u32 keyValue)
{
	KB_Group_StatusDef * KeyGroup;
    key_statusDef * status;	
    u8 i,j;
	u32 ledid;
	ledid = KeyVlaueToLedId(keyValue);
	for(i = KEY_GROUP_ID_SETTING; i <= KEY_GROUP_ID_OUTPUT;i++)
    {
 	
        KeyGroup = KB_KeyGroup_Get(i);
    	if(KeyGroup == NULL)
	    {
	        continue;
	    } 
        status = KeyGroup->status;
        if(status == NULL)
	    {
	        continue;
	    } 
        for(j = 0;j < KeyGroup->KeyNum;j++)
        {
            if(ledid == status[j].id.ledId)
            {
                return status[j].led.mode;
                //status[j].led.mode = mode;
            }
        }
	}
	return LED_STA_MAX_SIZE;
}


u32 KeyVlaueToLedId(u16 key)
{

	KB_Group_StatusDef * KeyGroup;
    key_statusDef * status;	
    u8 i,j,keyV,k;

	//k = KeyChange(key);
	for(i = KEY_GROUP_ID_SETTING; i <= KEY_GROUP_ID_INPUT_SOURCE;i++)
    {
 	
        KeyGroup = KB_KeyGroup_Get(i);
    	if(KeyGroup == NULL)
	    {
	        continue;
	    } 
        status = KeyGroup->status;
        if(status == NULL)
	    {
	        continue;
	    } 
        for(j = 0;j < KeyGroup->KeyNum;j++)
        {
			keyV = status[j].key;
			if(key == keyV)
				return status[j].id.ledId;
        }
		
	}

	return 0;
	
}



u16 KB_TransTo_Key(u8 chip,u8 pin)
{
    u32 keyId = _SET_KB_HW_ID(chip,pin);

	return KB_keyIdToKey(keyId);
}




LedState* GetKeyLedDataBuf(void)
{
	return _ledDataBuf;
}



void UpdateKeyLedStatus(void)
{
	u8 chip,pin;
	LedState *pdata;
	u8 bitState = 0;
	u8 ledMode;
	u8 bitOffset = 0;
	u32 key_id;

	for(u8 i = 1;i <= 16;i++)
	{
		key_id = KeyVlaueToLedId(i);
		ledMode = GetKeyLedMode(i);
		switch(ledMode)
		{
			case LED_ON_ALWAYS:bitState = LED_ON_ALWAYS;
			break;
			case LED_OFF_ALWAYS:bitState = LED_OFF_ALWAYS;
			break;
			case LED_BLINK_50MS:bitState = stBrinkStatus.brink_500ms;
			break;
			case LED_BLINK_100MS:bitState = stBrinkStatus.brink_500ms;
			break;
			case LED_BLINK_200MS:bitState = LED_ON_ALWAYS;
			break;
			case LED_BLINK_500MS:bitState = LED_ON_ALWAYS;
			break;
			case LED_BLINK_1S:bitState = LED_ON_ALWAYS;
			break;
			default:bitState = LED_ON_ALWAYS;
			break;
			
		}
		_GET_KB_HW_ID(key_id,chip,pin);
		pdata = GetKeyLedDataBuf();
		bitOffset = log2(pin);

		if(bitState & 0x01)
			pdata[chip-1].LedStatus &= ~(1 << bitOffset);
		else
			pdata[chip-1].LedStatus |= (1 << bitOffset);
	}

}




void keyLedDis(void)
{
	
#if defined(USE_HC595_HC165)
	write_74HC595((u8*)_ledDataBuf,MAX_595_NUMS);
#elif defined(USE_TCA9555)
	i2cBurstWriteBytes(TCA9555_I2C_PORT,KEY_LED_ADDRESS,TCA9555_OUTPUT0_REG,(u8*)_ledDataBuf,2);
#endif
	
}




void LedBrink(void)
{
	static vu32 startTiming = 0;//delayTimming;
	if(isSysTickTimeout(startTiming, 500))
	{	
		startTiming = GetSysTick();
		stBrinkStatus.brink_500ms = ~stBrinkStatus.brink_500ms;
	}
}



void TaskKeyLed(void)
{
	LedBrink();
	UpdateKeyLedStatus();
	keyLedDis();
}


void LED_Disp(u16 value)
{
	value = value;
	//value = switchValueTo(value);
	//ResetValue_74HC595(value,MAX_595_NUMS);
	//i2cBurstWriteBytes(TCA9555_I2C_PORT,KEY_LED_ADDRESS,TCA9555_OUTPUT0_REG,(u8*)value,2);
}

//D:\tools\mdk5.14\ARM\ARMCC\bin\fromelf.exe --bin   -o    ..\OBJ\RTC.bin  ..\OBJ\RTC.axf












































