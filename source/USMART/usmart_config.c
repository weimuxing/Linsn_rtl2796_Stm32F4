#include "usmart.h"
#include "usmart_str.h"
#include "RTD2796.h"
#include "fpga.h"
#include "menuData.h"
#include "control.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ�����) 
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

//�������б���ʼ��(�û��Լ�����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
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
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   


















