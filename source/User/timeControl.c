#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "uart.h"

#include "timeControl.h"
#include "PCF8563.h"
#include "rs232command.h"
#include "stdp9320.h"
//#include "mst6m48.h"
#include "userpref.h"
#include "input.h"
#include "control.h"

static u8 processed_index = 0; // 
static TimeControl_TypeDef * pTimeControl;
static Command_TypeDef conCmd;

static u8 curtimeCtrBlock =0;	
static u8 timeCtrBlockMAX =0;

#define CONTROL_CMD_TOTAL (14)  //  TimeControlBlock[128]里面共包含14条命令

static Time_TypeDef conTime;
static Time_TypeDef currentTime;
static u8 delay_count = 0;


//added by @cc20140618
u8 timeControl_getCurBlocknum(void)
{
	return curtimeCtrBlock;
}

void timeControl_setCurBlocknum(u8 index)
{
	curtimeCtrBlock = index;
}

u8 timeControl_getMaxBlocknum(void)
{
	return timeCtrBlockMAX;
}

void timeControl_setMaxBlocknum(u8 index)
{
	timeCtrBlockMAX = index;
}


//added by @cc for usbplaylist control
void timeControl_readBlock(int index)
{
	u8 i;
	printf("timeControl_readBlock(%d)\r\n",index);
	loadTimeControl(index);	
	pTimeControl = GetTimeControl();
	processed_index = 0;
	delay_count = 0;
	curtimeCtrBlock = index;
	#if 0
	if(index !=0)	//@cctest
	{
		printf("\r\n");
		for(i=0;i<128;i++)
		{
			printf("%x ",pTimeControl->timeControlBlock[i]);
		}
	}
	#endif
}



void timeControl_init(void)
{
	loadTimeControl(0);	
	pTimeControl = GetTimeControl();
	processed_index = 0;
	delay_count = 0;
	curtimeCtrBlock =0;
}

BYTE timeControl_TestgetStartMonth(void)
{
	return (pTimeControl->timeControlBlock[2]);
}


BYTE timeControl_TestgetStartDay(void)
{
	return (pTimeControl->timeControlBlock[3]);
}

BYTE timeControl_TestgetStartHour(void)
{
	return (pTimeControl->timeControlBlock[12]);
}

BYTE timeControl_TestgetStartMinute(void)
{
	return (pTimeControl->timeControlBlock[13]);
}

static int timeControl_getStartYear(void)
{
	return (pTimeControl->timeControlBlock[0]<<8) + pTimeControl->timeControlBlock[1];
}
static int timeControl_getEndYear(void)
{
	return (pTimeControl->timeControlBlock[4]<<8) + pTimeControl->timeControlBlock[5];
}

#define timeControl_getStartMonth() (pTimeControl->timeControlBlock[2])
#define timeControl_getEndMonth() (pTimeControl->timeControlBlock[6])
#define timeControl_getStartDay() (pTimeControl->timeControlBlock[3])
#define timeControl_getEndDay() (pTimeControl->timeControlBlock[7])
#define timeControl_getStartWeekday() (pTimeControl->timeControlBlock[8])  // 0 -- monday, 6 -- sunday
#define timeControl_getEndWeekday() (pTimeControl->timeControlBlock[9])
#define timeControl_getStartEveryDay() (pTimeControl->timeControlBlock[10])
#define timeControl_getEndEveryDay() (pTimeControl->timeControlBlock[11])
#define timeControl_getStartHour() (pTimeControl->timeControlBlock[12])
#define timeControl_getEndHour() (pTimeControl->timeControlBlock[14])
#define timeControl_getStartMinute() (pTimeControl->timeControlBlock[13])
#define timeControl_getEndMinute() (pTimeControl->timeControlBlock[15])

#if 0
static bool haveCmdProcess(void)
{
	if(!GetTime_PCF8563(&currentTime))
	{
		//printf("get time falil\r\n");
		return FALSE;
	}
	
		
		
		
	// compare year
	if((currentTime.year < timeControl_getStartYear()) || (currentTime.year > timeControl_getEndYear()))
	{
		//printf("year fail:start:%x - end:%x, current:%x\r\n",timeControl_getStartYear(), timeControl_getEndYear(),  currentTime.year );
		return FALSE;
	}
	// compare month
	if((currentTime.month < timeControl_getStartMonth()) || (currentTime.month > timeControl_getEndMonth()))
	{
		//printf("year month\r\n");
		return FALSE;
	}
	if(currentTime.month == timeControl_getStartMonth())
	{
		if(currentTime.day < timeControl_getStartDay())
		{
			//printf("day fail\r\n");
			return FALSE;
		}
	}
	if(currentTime.month == timeControl_getEndMonth())
	{
		if(currentTime.day > timeControl_getEndDay())
		{
			//printf("day fail\r\n");
			return FALSE;
		}
	}
	// compare day
	if((currentTime.day < timeControl_getStartEveryDay()) || (currentTime.day> timeControl_getEndEveryDay()))
	{
		//printf("every day fail\r\n");
		return FALSE;
	}
	// compare weekday
	if(currentTime.weekday == 0)
	{
		currentTime.weekday = 7;
	}
	currentTime.weekday -= 1;
	if((currentTime.weekday < timeControl_getStartWeekday()) || (currentTime.weekday > timeControl_getEndWeekday()))
	{
		//printf("weekday fail\r\n");
		return FALSE;
	}
	// compare hour
	if((currentTime.hour < timeControl_getStartHour()) || (currentTime.hour > timeControl_getEndHour()))
	{
		//printf("hour fail\r\n");
		return FALSE;
	}
	// compare minute
	if((currentTime.hour == timeControl_getStartHour()) && (currentTime.minute < timeControl_getStartMinute()))
	{
		//printf("start minute fail\r\n");
		return FALSE;
	}
	if((currentTime.hour == timeControl_getEndHour()) && (currentTime.minute > timeControl_getEndMinute()))
	{
		//printf("end minute fail\r\n");
		return FALSE;
	}
	return TRUE;
	
}
#else
static int haveCmdProcess(void)	//changed by @cc20140707
{
	if(!GetTime_PCF8563(&currentTime))
	{
		//printf("get time falil\r\n");
		return -1;
	}
	
		
		
		
	// compare year
	if(currentTime.year < timeControl_getStartYear())	
	{
		printf("year fail:start:%x - end:%x, current:%x\r\n",timeControl_getStartYear(), timeControl_getEndYear(),  currentTime.year );
		return -2;
	}
	else if(currentTime.year > timeControl_getEndYear())
	{
		printf("year fail:start:%x - end:%x, current:%x\r\n",timeControl_getStartYear(), timeControl_getEndYear(),  currentTime.year );
		return 2;
	}
	
	// compare month
	if(currentTime.month < timeControl_getStartMonth())
	{
		//printf("year month\r\n");
		return -3;
	}
	else if(currentTime.month > timeControl_getEndMonth())
	{
		return 3;
	}
	if(currentTime.month == timeControl_getStartMonth())
	{
		if(currentTime.day < timeControl_getStartDay())
		{
			//printf("day fail\r\n");
			return -4;
		}
	}
	if(currentTime.month == timeControl_getEndMonth())
	{
		if(currentTime.day > timeControl_getEndDay())
		{
			//printf("day fail\r\n");
			return 4;
		}
	}
	// compare day
	if(currentTime.day < timeControl_getStartEveryDay())
	{
		//printf("every day fail\r\n");
		return -5;
	}
	else if(currentTime.day> timeControl_getEndEveryDay())
	{
		return 5;
	}
	
	// compare weekday
	if(currentTime.weekday == 0)
	{
		currentTime.weekday = 7;
	}
	currentTime.weekday -= 1;
	if(currentTime.weekday < timeControl_getStartWeekday()) 
	{
		//printf("weekday fail\r\n");
		return -6;
	}
	else if(currentTime.weekday > timeControl_getEndWeekday())
	{
		return 6;
	}
	// compare hour
	if(currentTime.hour < timeControl_getStartHour())
	{
		//printf("hour fail\r\n");
		return -7;
	}
	else if(currentTime.hour > timeControl_getEndHour())
	{
		return 7;
	}
	// compare minute
	if((currentTime.hour == timeControl_getStartHour()) && (currentTime.minute < timeControl_getStartMinute()))
	{
		//printf("start minute fail\r\n");
		return -8;
	}
	if((currentTime.hour == timeControl_getEndHour()) && (currentTime.minute > timeControl_getEndMinute()))
	{
		//printf("end minute fail\r\n");
		return 8;
	}
	return 1;
	
}
#endif


static bool timeControl_getCmd(int index)
{
	if((index < 1) || (index > CONTROL_CMD_TOTAL))
	{
		return FALSE;
	}
	conTime = currentTime;
	conTime.hour = pTimeControl->timeControlBlock[16+(index-1)*8];
	conTime.minute = pTimeControl->timeControlBlock[16+(index-1)*8+1];
	conTime.second = pTimeControl->timeControlBlock[16+(index-1)*8+2];
	conCmd.command = pTimeControl->timeControlBlock[16+(index-1)*8+3];
	conCmd.data_1 =  pTimeControl->timeControlBlock[16+(index-1)*8+4];
	conCmd.data_2 =  pTimeControl->timeControlBlock[16+(index-1)*8+5];
	conCmd.data_3 =  pTimeControl->timeControlBlock[16+(index-1)*8+6];
	conCmd.data_4 =  pTimeControl->timeControlBlock[16+(index-1)*8+7];
	#if 0
	printf("current:%x:%x:%x\r\n", currentTime.hour,currentTime.minute, currentTime.second);
	printf("cmdtime:%x:%x:%x\r\n", conTime.hour,conTime.minute, conTime.second);
	printf("cmd:%d,%x %x %x %x\r\n",conCmd.command, conCmd.data_1,conCmd.data_2, conCmd.data_3,conCmd.data_4);
	#endif
	if((conCmd.command == 0))
	{
		printf("no cmd\r\n");
		return FALSE;
	}
	if( CompareTime(&currentTime, &conTime) >= 10)
	{
		printf("T:getcmdErr cmpErr;\r\n");
		printf("time out:%d\r\n", CompareTime(&currentTime, &conTime));
		printf("current:%x:%x:%x\r\n", currentTime.hour,currentTime.minute, currentTime.second);
		printf("cmdtime:%x:%x:%x\r\n", conTime.hour,conTime.minute, conTime.second);
		return FALSE;
	}
	return TRUE;
}

static bool timeControl_canProcessCmd(void)
{
	int compare;
	compare = CompareTime(&currentTime, &conTime);
	if((-1 < compare) && (compare < 10))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#if 0
void timeControl_task(void) // every second called
{
	
	
	if(delay_count > 0)
	{
		delay_count--;
		return;
	}
	
	if(GetTimeControl()->enableTimeControl == 0)// no function
	{
		return;
	}
	if(!pTimeControl->timeControlValid)
	{
		return;
	}
	if(!haveCmdProcess())
	{
		//printf("no cmd process\r\n");
		return;
	}
	while(processed_index < CONTROL_CMD_TOTAL)
	{
		if(!timeControl_getCmd(processed_index+1))
		{
			++processed_index;
			//printf("get next %d\r\n", processed_index);
		}
		else
		{
			break;
		}
	}
	if(processed_index >= CONTROL_CMD_TOTAL)
	{
		processed_index = 0;
		delay_count = 60;
		//printf("processed_index>= CONTROL_CMD_TOTAL\r\n");
		return;
	}
	if(timeControl_canProcessCmd())
	{
		++processed_index;
		processCmdFromLocal(conCmd.command,conCmd.data_1,conCmd.data_2,conCmd.data_3,conCmd.data_4);		
		timeControl_task();
	}
	
}
#else
void timeControl_task(void) // every second called
{
#if 0
	char value;
	if(delay_count > 0)
	{
		delay_count--;
		return;
	}
	
	if(GetTimeControl()->enableTimeControl == 0)// no function
	{
		return;
	}
	if(!pTimeControl->timeControlValid)
	{
		return;
	}
	value = haveCmdProcess();		//changed by @cc20140707
	if(value != 1)
	{
		printf("no cmd process:%d;curBlck:%d;maxBlck:%d;\r\n",value,timeControl_getCurBlocknum(),timeControl_getMaxBlocknum());
		if((value ==4)&&(timeControl_getCurBlocknum()< timeControl_getMaxBlocknum()))	//added by @cc20140707
		{
			printf("TTPlaylist...\r\n");
			timeControl_readBlock(timeControl_getCurBlocknum()+1);
			GetTimeControl()->enableTimeControl = 1;	//added by @cc20140702
			saveTimeControl(timeControl_getCurBlocknum());
		}
		return;
	}
	while(processed_index < CONTROL_CMD_TOTAL)
	{
		if(!timeControl_getCmd(processed_index+1))
		{
			++processed_index;
			printf("get next %d\r\n", processed_index);
		}
		else
		{
			printf("processed_index = %d\r\n", processed_index);
			break;
		}
	}
	if(processed_index >= CONTROL_CMD_TOTAL)
	{
		printf("processed_index >= CONTROL_CMD_TOTAL;\r\n");
		if(getUSBControlSetting()->USBPortParam[getUSBControlSetting()->operatBoard - 1].playOrder == PLAYLIST)		
		{				
			if(timeControl_getCurBlocknum()< timeControl_getMaxBlocknum())
			{
				printf("TTPlaylist...\r\n");
				timeControl_readBlock(timeControl_getCurBlocknum()+1);
				GetTimeControl()->enableTimeControl = 1;	
				saveTimeControl(timeControl_getCurBlocknum());
			}
			#if 0		
			else
			{
				timeControl_readBlock(TIMECOTROL_USBPLSYLISTMINBLOCK);
				GetTimeControl()->enableTimeControl = 1;	//added by @cc20140702
				saveTimeControl(timeControl_getCurBlocknum());
			}
			#endif
		}
		else
		{
			processed_index = 0;
			delay_count = 60;
			//printf("processed_index>= CONTROL_CMD_TOTAL\r\n");
			//return;
		}
		return ;
	}
	if(timeControl_canProcessCmd())
	{
		++processed_index;
		#if 0
		processCmdFromLocal(conCmd.command,conCmd.data_1,conCmd.data_2+1,conCmd.data_3,conCmd.data_4);
		#else
		processCmdFromLocal_USB_X1(getUSBControlSetting()->operatBoard,conCmd.command,conCmd.data_1,conCmd.data_2,conCmd.data_3,conCmd.data_4);
		#endif
		printf("\r\n=TIME CTR=\r\n");
		printf("sendCmd:cmd=%x;d1=%x;d2=%x;dx=%d;d4=%d",conCmd.command,conCmd.data_1,conCmd.data_2+1,conCmd.data_3,conCmd.data_4);
		//timeControl_task();	//commented by @cc20140618
	}
	#endif
}

#endif






















