#include "usmart.h"
#include "usmart_str.h"
#include "RTD2796.h"
#include "fpga.h"
#include "menuData.h"
#include "control.h"
////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
//#include "delay.h"		
//#include "sys.h"
//#include "lcd.h"

void SetIDT5901Freq(u64 Fout,u8 port);
void SetOutFormat(BYTE index);
int setValue(int index, int value);

void UpdateInputSource(BYTE sources);



void SetOutput0Map(BYTE value)
{
	setValue(outputPort0Value,value);
}

void SetOutput1Map(BYTE value)
{
	setValue(outputPort1Value,value);
}

void SetOutput2Map(BYTE value)
{
	setValue(outputPort2Value,value);
}

void SetOutput3Map(BYTE value)
{
	setValue(outputPort3Value,value);
}


void Bsp_Soft_Reset(void);

//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   		

	//(void*)GetRtd2796Version,"void GetRtd2796Version(void)",
	//(void*)SetIDT5901Freq,"void SetIDT5901Freq(u64 Fout,u8 port)",
	
	(void*)FPGA_set,"FPGA_set(u8 address, u8 value)",
	(void*)FPGA_get,"FPGA_get(u8 address)",
	
	(void*)SetOutFormat,"void SetOutFormat(BYTE index)",
	
	(void*)SetOutput0Map,"void SetOutput0Map(BYTE value)",
	(void*)SetOutput1Map,"void SetOutput1Map(BYTE value)",
	(void*)SetOutput2Map,"void SetOutput2Map(BYTE value)",
	(void*)SetOutput3Map,"void SetOutput3Map(BYTE value)",
	
	(void*)ResetFactory,"void ResetFactory(void)",
	(void*)UpdateInputSource,"void UpdateInputSource(BYTE sources)",
	(void*)Bsp_Soft_Reset,"void Bsp_Soft_Reset(void)",
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   



















