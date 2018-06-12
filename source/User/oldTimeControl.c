#include "oldTimeControl.h"
#include "systick.h"
#include "userPref.h"
#include "PCF8563.h"
#include "control.h"
#include "menu.h"
#include "menuData.h"




static char oldTimeControlState = STATE_INIT;
static Time_TypeDef timeBuffer[TIME_CONTROL_ITEM_NUM];


void loadTimeBuffer(void)
{
	int i;
	u8 buffer[7];
	//printf("loadTimeBuffer(),\r\n");
	for(i=0; i<TIME_CONTROL_ITEM_NUM; i++)
	{
		getOldTimeControlData(TIME_CONTROL_ITEM_SIZE*i,buffer,sizeof(buffer));
		timeBuffer[i].year = buffer[0] + (buffer[1]<<8);
		timeBuffer[i].month = buffer[2];
		timeBuffer[i].day = buffer[3];
		timeBuffer[i].hour = buffer[4];
		timeBuffer[i].minute = buffer[5];
		timeBuffer[i].second = buffer[6];
		
		//printf("%d: %x,%x,%x,%x,%x,%x\r\n",i,timeBuffer[i].year,timeBuffer[i].month,timeBuffer[i].day,timeBuffer[i].hour,timeBuffer[i].minute,timeBuffer[i].second);
	}
}

void actionTimeControlData(int index)
{
	UserSetting_TypeDef *pUserSetting = GetUserSetting();
	
	u8 buffer[20];
	
	SaveCurrentSetting();
	
	getOldTimeControlData((TIME_CONTROL_ITEM_SIZE*index)+7,buffer,sizeof(buffer));

	if(buffer[0])
	{
		int channel;
		int address = 1;
		channel = 0;
		
		pUserSetting->pipMode = 1;
		// main channel
		#if 0
		pUserSetting->inputSource[channel] = buffer[address];
		#else
		if(getFrontType() == 2)
		{
			pUserSetting->inputSource[channel] = getSourceToDevice(buffer[address]);
		}
		else
		{
			pUserSetting->inputSource[channel] = buffer[address];
		}
		#endif
		GetUserSetting()->layer[channel].scale.posX = buffer[address+1]+(buffer[address+2]<<8);
		GetUserSetting()->layer[channel].scale.posY = buffer[address+3]+(buffer[address+4]<<8);
		GetUserSetting()->layer[channel].scale.sizeX = buffer[address+5]+(buffer[address+6]<<8);
		GetUserSetting()->layer[channel].scale.sizeY = buffer[address+7]+(buffer[address+8]<<8);

		// pip channel
		channel = 1;
		address = 10;

		#if 0
		pUserSetting->inputSource[channel] = buffer[address];
		#else
		if(getFrontType() == 2)
		{
			pUserSetting->inputSource[channel] = getSourceToDevice(buffer[address]);
		}
		else
		{
			pUserSetting->inputSource[channel] = buffer[address];
		}
		#endif
		GetUserSetting()->layer[channel].scale.posX = buffer[address+1]+(buffer[address+2]<<8);
		GetUserSetting()->layer[channel].scale.posY = buffer[address+3]+(buffer[address+4]<<8);
		GetUserSetting()->layer[channel].scale.sizeX = buffer[address+5]+(buffer[address+6]<<8);
		GetUserSetting()->layer[channel].scale.sizeY = buffer[address+7]+(buffer[address+8]<<8);
		
	}
	else
	{
		int channel;
		int address = 1;
		channel = 0;
		
		pUserSetting->pipMode = 0;
		pUserSetting->currentWindow = 0;
		// main channel
		#if 0
		pUserSetting->inputSource[channel] = buffer[address];
		#else
		if(getFrontType() == 2)
		{
			pUserSetting->inputSource[channel] = getSourceToDevice(buffer[address]);
		}
		else
		{
			pUserSetting->inputSource[channel] = buffer[address];
		}
		#endif
		GetUserSetting()->layer[channel].scale.posX = buffer[address+1]+(buffer[address+2]<<8);
		GetUserSetting()->layer[channel].scale.posY = buffer[address+3]+(buffer[address+4]<<8);
		GetUserSetting()->layer[channel].scale.sizeX = buffer[address+5]+(buffer[address+6]<<8);
		GetUserSetting()->layer[channel].scale.sizeY = buffer[address+7]+(buffer[address+8]<<8);		
	}
	
	SetNoSameSettingAll();
	menuProcessEvent(MENU_KEY_INFO);	
}

static u16 actionIndexMask;

void clearAllActionIndex(void)
{
	actionIndexMask = 0;
}

void setActionIndex(u8 index)
{
	if(index < TIME_CONTROL_ITEM_NUM)
	{
		actionIndexMask |= (1<<index);
	}
}

int isActionIndex(u8 index)
{
	if(actionIndexMask & (1<<index))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void compareTimeAndAction(void)
{
	int i;
	
	Time_TypeDef currentTime;
	
	if(!GetTime_PCF8563(&currentTime))
	{
		init_PCF8563();
		GetTime_PCF8563(&currentTime);
	}
	for(i=0; i<TIME_CONTROL_ITEM_NUM; i++)
	{
		int temp = CompareTime(&currentTime, &timeBuffer[i]);
		if( (0<=temp) && (temp <= 2))
		{			
			if(!isActionIndex(i))
			{
				//printf("compareTimeAndAction(),");
				//printf("timeBuffer is %x,%x,%x,%x,%x,%x\r\n",timeBuffer[i].year,timeBuffer[i].month,timeBuffer[i].day,timeBuffer[i].hour,timeBuffer[i].minute,timeBuffer[i].second);
				//printf("current time is %x,%x,%x,%x:%x:%x\r\n",currentTime.year,currentTime.month,currentTime.day,currentTime.hour,currentTime.minute,currentTime.second);
				printf("task_oldTimeControl()___i=%d, temp=%d\r\n", i, temp);
				actionTimeControlData(i);
				setActionIndex(i);
				printf("end Action\r\n");
			}
			
		}
		
	}
}

void task_oldTimeControl(void)
{
	static u32 _startTime = 0;

	
	if(!getOldTimeControlEnable())
	{
		return;
	}

	if(isSysTickTimeout(_startTime,1000))
	{
		_startTime = GetSysTick();
	}
	else
	{
		return;
	}
	
	switch(oldTimeControlState)
	{
	case STATE_INIT:
		oldTimeControlState = STATE_LOAD_DATA;
		break;
	case STATE_LOAD_DATA:
		oldTimeControlState = STATE_COMPARE_TIME;
		clearAllActionIndex();
		loadTimeBuffer();
		break;
	case STATE_COMPARE_TIME:
		compareTimeAndAction();
		break;
	default:
		oldTimeControlState = STATE_INIT;
		break;
	}

}

void SetTimeControlState(int state)
{
	oldTimeControlState = state;
}






























