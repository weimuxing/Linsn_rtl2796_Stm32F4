#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "uart.h"

#include "SysConfig.h"

#include "PCF8563.h"
#include "i2c.h"

void readReg_PCF8563(u8 address, u8 * buffer, u8 size)
{
	i2cBurstReadBytes(PCF8563_I2C_PORT,PCF8563_I2C_ADDRESS,address,buffer,size);
}
void writeReg_PCF8563(u8 address, u8 * buffer, u8 size)
{
	i2cBurstWriteBytes(PCF8563_I2C_PORT,PCF8563_I2C_ADDRESS,address,buffer,size);
}


void init_PCF8563(void)
{
	u8 buffer;
	Time_TypeDef time;
	buffer = PCF8563_Control1_value;
	writeReg_PCF8563(PCF8563_Control1_address,&buffer,1);
	buffer = 0x00;
	writeReg_PCF8563(0x0d, &buffer, 1); // disable clkout 
	time.day = 0x01;
	time.hour = 0;
	time.minute = 0;
	time.month = 1;
	time.second = 0;
	time.weekday = 2;
	time.year = 0x2013;
	SetTime_PCF8563(&time);
	printf("Init PCF8563\r\n");
	
}
bool SetTime_PCF8563(Time_TypeDef * time)
{
	u8 buffer[7];
	Time_TypeDef time1;
	buffer[0] = time->second;
	buffer[1] = time->minute;
	buffer[2] = time->hour;
	buffer[3] = time->day;
	buffer[4] = time->weekday;
	buffer[5] = time->month;
	if(time->year < 0x2000)
	{
		buffer[5] |= 0x80;
	}
	buffer[6] =(u8) (time->year & 0xFF);
	writeReg_PCF8563(0x02, buffer, sizeof(buffer));
	// enable CLKOUT for test
	//buffer[0] = 0x80;
	//writeReg_PCF8563(0x0D, buffer, 1);
	
	if(GetTime_PCF8563(&time1))
	{
		int offset;
		offset = CompareTime(time,&time1);
		if((-2 < offset) && (offset < 2))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}	
	else
	{
		return FALSE;
	}
}

bool SetCalendar_PCF8563(Time_TypeDef * time)
{
	u8 buffer[4];
	Time_TypeDef time1;
	buffer[0] = time->day;
	buffer[1] = time->weekday;
	buffer[2] = time->month;
	if(time->year < 0x2000)
	{
		buffer[2] |= 0x80;
	}
	buffer[3] =(u8) (time->year & 0xFF);
	writeReg_PCF8563(0x05, buffer, sizeof(buffer));
	if(GetTime_PCF8563(&time1))
	{
		
		if( (time1.day == time->day) &&
			(time1.month == time->month) &&
			(time1.weekday == time->weekday) &&
			(time1.year == time->year))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}	
	else
	{
		return FALSE;
	}
}


bool GetTime_PCF8563(Time_TypeDef *time)
{
	u8 buffer[7];
	readReg_PCF8563(0x02,buffer,sizeof(buffer));
	if(buffer[0] & 0x80)
	{
		return FALSE;
	}
	time->second = buffer[0];
	time->minute = buffer[1] & 0x7F;
	time->hour = buffer[2] & 0x3F;
	time->day = buffer[3] & 0x3F;
	time->weekday = buffer[4] & 0x07;
	time->month = buffer[5] & 0x1F;
	if(buffer[5] & 0x80)
	{
		time->year = 0x1900 + buffer[6];
	}
	else
	{
		time->year = 0x2000 + buffer[6];
	}
	
	return TRUE;
}



int CompareTime(Time_TypeDef *time1, Time_TypeDef *time2) // return time1-time2  seconds  
{
	if(time1->year == time2->year)
	{
		
	}
	else if(time1->year > time2->year)
	{
		return 10000;
	}
	else
	{
		return -10000;
	}

	if(time1->month == time2->month)
	{
		
	}
	else if(time1->month > time2->month)
	{
		return 1000;
	}
	else
	{
		return -1000;
	}

	if(time1->day == time2->day)
	{
		
	}
	else
	{
		int second1,  second2;
		int day1, day2;
		
		day1 = (time1->day>>4)*10 + (time1->day& 0x0F);
		day2 = (time2->day>>4)*10 + (time2->day& 0x0F);
		
		second1 = (time1->hour>>4)*10 + (time1->hour & 0x0F);
		second1 *= 60;
		second1 += (time1->minute>>4)*10 + (time1->minute & 0x0F);
		second1 *= 60;
		second1 += (time1->second>>4)*10 + (time1->second & 0x0F);

		second2 = (time2->hour>>4)*10 + (time2->hour & 0x0F);
		second2 *= 60;
		second2 += (time2->minute>>4)*10 + (time2->minute & 0x0F);
		second2 *= 60;
		second2 += (time2->second>>4)*10 + (time2->second & 0x0F);
		
		return (second1-second2+(day1 - day2)*24*60*60);
	}

	if(time1->hour == time2->hour)
	{
		
	}
	else
	{
		int second1, second2;
		second1 = (time1->hour>>4)*10 + (time1->hour & 0x0F);
		second1 *= 60;
		second1 += (time1->minute>>4)*10 + (time1->minute & 0x0F);
		second1 *= 60;
		second1 += (time1->second>>4)*10 + (time1->second & 0x0F);

		second2 = (time2->hour>>4)*10 + (time2->hour & 0x0F);
		second2 *= 60;
		second2 += (time2->minute>>4)*10 + (time2->minute & 0x0F);
		second2 *= 60;
		second2 += (time2->second>>4)*10 + (time2->second & 0x0F);
		return (second1-second2);
	}

	if(time1->minute == time2->minute)
	{
		
	}
	else
	{
		int second1, second2;
		second1 = (time1->minute>>4)*10 + (time1->minute & 0x0F);
		second1 *= 60;
		second1 += (time1->second>>4)*10 + (time1->second & 0x0F);
		
		second2 = (time2->minute>>4)*10 + (time2->minute & 0x0F);
		second2 *= 60;
		second2 += (time2->second>>4)*10 + (time2->second & 0x0F);
		return (second1-second2);
	}

	if(time1->second == time2->second)
	{
		
	}
	else 
	{
		int second1, second2;
		second1 = (time1->second>>4)*10 + (time1->second & 0x0F);
		second2 = (time2->second>>4)*10 + (time2->second & 0x0F);
		return (second1-second2);
	}
	return 0;
}

bool isEqualTime(Time_TypeDef *time1, Time_TypeDef *time2)
{
	if( (time1->second == time2->second) &&
		(time1->minute == time2->minute) &&
		(time1->hour == time2->hour) &&
		(time1->day == time2->day) &&
		(time1->month == time2->month) &&
		(time1->year == time2->year) &&
		(time1->weekday == time2->weekday))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CopyTime(Time_TypeDef *from, Time_TypeDef *to)
{
	to->day = from->day;
	to->minute = from->minute;
	to->month = from->month;
	to->second = from->second;
	to->weekday = from->weekday;
	to->year = from->year;
}

char CheckPCF8563(void)
{
	Time_TypeDef getTime;
	char res = GetTime_PCF8563(&getTime);
	return res;	
}

void pcf8563_test(void)
{
#if defined(COLOK_CHIP_TEST)
	static u32 	time_UpdataTime;
	Time_TypeDef getTime;
		
	if(isSysTickTimeout(time_UpdataTime,1000))
	{
		GetTime_PCF8563(&getTime);
		time_UpdataTime = GetSysTick();
		printf("Time:%4x-%02x-%02x-%02x-%02x-%02x\r\n", getTime.year,getTime.month, getTime.day,getTime.hour,getTime.minute,getTime.second);
	}
#endif
}



