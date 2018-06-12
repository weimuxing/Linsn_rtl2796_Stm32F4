#include "IDT5P49V5901.h"
#include "I2C.h"
#include "IDT_Timing_Commander.h"
#include "define.h"
void IDT_FreqRegSetting(IDT5P49V5901_TypeDef IDTDevice,IDTTimming_Typedef IDTTimming);

u8 IDT_Reg[106] = 
{
#if 1
	0x61,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x01,0x00,0x00,0xB6,0xB4,0x92,
	0x80,0x0C,0x01,0x00,0x00,0x03,0x8C,0x06,0x40,0x00,0x00,0x00,0x9F,0x4D,0x92,0x32,
	0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0xA0,0x00,
	0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x00,
	0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x00,
	0x00,0x81,0x00,0x55,0x55,0x54,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x40,0x00,
	0x7B,0x01,0x3B,0x01,0x7B,0x01,0x3B,0x01,0xFF,0xFC   
#endif
#if 0
//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	0x61,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x01,0x00,0x00,0xB6,0xB4,0x92,  //0
	0x80,0x0C,0x01,0x00,0x00,0x03,0x8C,0x05,0x00,0x00,0x00,0x00,0x9F,0x4D,0x92,0x32,  // 1
	0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0xE0,0x00,  // 2
	0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0xE0,0x00,  // 3
	0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0xE0,0x00,  //4// 4
	0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0xE0,0x00,  // 5
	0x7B,0x01,0x7B,0x01,0x7B,0x01,0x7B,0x01,0xFF,0xFC   
#endif


#if 0
//  0    1     2   3    4    5    6    7    8    9    a    b    c    d    e    f
	0x61,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x01,0x00,0x00,0xB6,0xB4,0x92,  //0
	0x80,0x0C,0x01,0x00,0x00,0x03,0x8C,0x05,0x00,0x00,0x00,0x00,0x9F,0x4D,0x92,0x32,  // 1
	0x00,0x81,0x03,0x6B,0xBE,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x20,0x00,  // 2
	0x00,0x81,0x03,0x6B,0xBE,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x20,0x00,  // 3
	0x00,0x81,0x03,0x6B,0xBE,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x20,0x00,  //4// 4
	0x00,0x81,0x03,0x6B,0xBE,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x20,0x00,  // 5
	0x7B,0x01,0x7B,0x01,0x7B,0x01,0x7B,0x01,0xFF,0xFC  
#endif
};



void Write_IDT5P49V5901(IDT5P49V5901_TypeDef IDTDevice, u16 address, u8 value)
{
	switch(IDTDevice)
	{
		case IDT5P49V5901_1:
			i2cWriteByte(I2C_H, IDT5P49V5901_1_I2C_ADDRESS, address, value); 
			break;
		case IDT5P49V5901_2:
			i2cWriteByte(I2C_H, IDT5P49V5901_2_I2C_ADDRESS, address, value); 
			break;
		default: 
			break;
	}
}



int Read_IDT5P49V5901(IDT5P49V5901_TypeDef IDTDevice, u16 address)
{
	int res = 0;
	switch(IDTDevice)
	{
		case IDT5P49V5901_1:
			res = i2cReadByte(I2C_H, IDT5P49V5901_1_I2C_ADDRESS, address);	 
			break;
		case IDT5P49V5901_2:
			res = i2cReadByte(I2C_H, IDT5P49V5901_2_I2C_ADDRESS, address);	 
			break;
		default:
			res = 0;
			break;
	}
	return res;
}


void IDT5P49V5901_Reg_Initial(IDT5P49V5901_TypeDef IDTDevice,u64 Fout0,u64 Fout1,u64 Fout2,u64 Fout3)
{
	int i;

    GPIO_InitTypeDef GPIO_InitStructure;

  // PF2 connect PO1Ext

  /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);

  /* Configure PB0 as GPIO out  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOH, &GPIO_InitStructure);

    GPIO_SetBits(GPIOH, GPIO_Pin_11);  	
	SysDelay1ms(1000);    
    GPIO_ResetBits(GPIOH, GPIO_Pin_11);     

	for(i = 0;i < 106;i ++)
	{
		Write_IDT5P49V5901(IDTDevice,i,IDT_Reg[i]);      
		printf("Read_IDT5P49V5901%d     0x%x\r\n",i,Read_IDT5P49V5901(IDTDevice,i));	
	}
	Write_IDT5P49V5901(IDTDevice,0x73,0x00);  
	Write_IDT5P49V5901(IDTDevice,0x74,0x4E);      
	Write_IDT5P49V5901(IDTDevice,0x75,0x00);  
 	Write_IDT5P49V5901(IDTDevice,0x76,0x61);  
	Write_IDT5P49V5901(IDTDevice,0x77,0x00);     
	Write_IDT5P49V5901(IDTDevice,0x78,0x00);   

	Write_IDT5P49V5901(IDTDevice,0x72,0xF0);    
	Write_IDT5P49V5901(IDTDevice,0x72,0xF8);    
	SysDelay1ms(500);
	Write_IDT5P49V5901(IDTDevice,0x72,0xF0);    
	Write_IDT5P49V5901(IDTDevice,0x72,0xF8);   
	SysDelay1ms(500);
	Write_IDT5P49V5901(IDTDevice,0x72,0xF0);  

	Write_IDT5P49V5901(IDTDevice,0x72,0xF2);    
	Write_IDT5P49V5901(IDTDevice,0x72,0xF0);     

    printf("0x9F = 0x%x \r\n",Read_IDT5P49V5901(IDT5P49V5901_1,0x9F));  
 
    #if 1
	#if 1
	if(IDTDevice == IDT5P49V5901_1)
	{
		Calculate_IDTTimming(&IDTTimming,Fout0,Fout1,Fout2,Fout3);//john@20170110 debug 220000000ll
		//Calculate_IDTTimming(&IDTTimming,74250000ll,125000000ll,64000000ll,13500000ll);//lv@20160801 debug
		//Calculate_IDTTimming(&IDTTimming,74250000ll,74250000ll,74250000ll,74250000ll);//lv@20160801 debug
		IDT_FreqRegSetting(IDTDevice,IDTTimming);
	}
	else if(IDTDevice == IDT5P49V5901_2)
	{
		Calculate_IDTTimming(&IDTTimming,13500000,74250000,64000000,13500000);//lv@20160801 debug
		IDT_FreqRegSetting(IDTDevice,IDTTimming);
	}
	#endif	
    #endif  
}


void Set5901Freq(IDT5P49V5901_TypeDef IDTDevice,u64 Fout0,u64 Fout1,u64 Fout2,u64 Fout3)
{
	if(IDTDevice == IDT5P49V5901_1)
	{
		Calculate_IDTTimming(&IDTTimming,Fout0,Fout1,Fout2,Fout3);//john@20170110 debug
		//Calculate_IDTTimming(&IDTTimming,74250000ll,50000000ll,220000000ll,220000000ll);//john@20170110 debug
		//Calculate_IDTTimming(&IDTTimming,74250000ll,125000000ll,64000000ll,13500000ll);//lv@20160801 debug
		//Calculate_IDTTimming(&IDTTimming,74250000ll,74250000ll,74250000ll,74250000ll);//lv@20160801 debug
		IDT_FreqRegSetting(IDTDevice,IDTTimming);
	}
	else if(IDTDevice == IDT5P49V5901_2)
	{
		Calculate_IDTTimming(&IDTTimming,Fout0,Fout1,Fout2,Fout3);//lv@20160801 debug
		//Calculate_IDTTimming(&IDTTimming,13500000,74250000,64000000,13500000);//lv@20160801 debug
		IDT_FreqRegSetting(IDTDevice,IDTTimming);
	}	
}



#if 1
void IDT_FreqRegSetting(IDT5P49V5901_TypeDef IDTDevice,IDTTimming_Typedef IDTTimming)
{
 
	//=================== Integer
	Write_IDT5P49V5901(IDTDevice,0x2E,(IDTTimming.IDTTimming_Q0.DSMint & 0x00f)<<4);
	Write_IDT5P49V5901(IDTDevice,0x2D,(IDTTimming.IDTTimming_Q0.DSMint >> 4));

	Write_IDT5P49V5901(IDTDevice,0x3E,(IDTTimming.IDTTimming_Q1.DSMint & 0x00f)<<4);
	Write_IDT5P49V5901(IDTDevice,0x3D,(IDTTimming.IDTTimming_Q1.DSMint >> 4)); 
 

	Write_IDT5P49V5901(IDTDevice,0x4E,(IDTTimming.IDTTimming_Q2.DSMint & 0x00f)<<4);
	Write_IDT5P49V5901(IDTDevice,0x4D,(IDTTimming.IDTTimming_Q2.DSMint >> 4)); 

	Write_IDT5P49V5901(IDTDevice,0x5E,(IDTTimming.IDTTimming_Q3.DSMint & 0x00f)<<4);
	Write_IDT5P49V5901(IDTDevice,0x5D,(IDTTimming.IDTTimming_Q3.DSMint >> 4));     
	//================== Fractional
#if 1
	Write_IDT5P49V5901(IDTDevice,0x25,(IDTTimming.IDTTimming_Q0.DSMfrac& 0x0000003f)<<2);
	Write_IDT5P49V5901(IDTDevice,0x24,((IDTTimming.IDTTimming_Q0.DSMfrac >> 6)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x23,((IDTTimming.IDTTimming_Q0.DSMfrac >> 14)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x22,((IDTTimming.IDTTimming_Q0.DSMfrac >> 22)&0x000000ff));    
	
	Write_IDT5P49V5901(IDTDevice,0x35,(IDTTimming.IDTTimming_Q1.DSMfrac& 0x0000003f)<<2);
	Write_IDT5P49V5901(IDTDevice,0x34,((IDTTimming.IDTTimming_Q1.DSMfrac >> 6)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x33,((IDTTimming.IDTTimming_Q1.DSMfrac >> 14)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x32,((IDTTimming.IDTTimming_Q1.DSMfrac >> 22)&0x000000ff));    
	
	Write_IDT5P49V5901(IDTDevice,0x45,(IDTTimming.IDTTimming_Q2.DSMfrac& 0x0000003f)<<2);
	Write_IDT5P49V5901(IDTDevice,0x44,((IDTTimming.IDTTimming_Q2.DSMfrac >> 6)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x43,((IDTTimming.IDTTimming_Q2.DSMfrac >> 14)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x42,((IDTTimming.IDTTimming_Q2.DSMfrac >> 22)&0x000000ff));    
	
	Write_IDT5P49V5901(IDTDevice,0x55,(IDTTimming.IDTTimming_Q3.DSMfrac& 0x0000003f)<<2);
	Write_IDT5P49V5901(IDTDevice,0x54,((IDTTimming.IDTTimming_Q3.DSMfrac >> 6)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x53,((IDTTimming.IDTTimming_Q3.DSMfrac >> 14)&0x000000ff));
	Write_IDT5P49V5901(IDTDevice,0x52,((IDTTimming.IDTTimming_Q3.DSMfrac >> 22)&0x000000ff));    
#endif
	
}
#endif

static void IDT_FreqRegSettingIndependent(IDT5P49V5901_TypeDef IDTDevice,IDTTimming_Typedef IDTTimming,u8 port)
{
	switch(port)
	{
		case 0:
			Write_IDT5P49V5901(IDTDevice,0x2E,(IDTTimming.IDTTimming_Q0.DSMint & 0x00f)<<4);
			Write_IDT5P49V5901(IDTDevice,0x2D,(IDTTimming.IDTTimming_Q0.DSMint >> 4));

			Write_IDT5P49V5901(IDTDevice,0x25,(IDTTimming.IDTTimming_Q0.DSMfrac& 0x0000003f)<<2);
			Write_IDT5P49V5901(IDTDevice,0x24,((IDTTimming.IDTTimming_Q0.DSMfrac >> 6)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x23,((IDTTimming.IDTTimming_Q0.DSMfrac >> 14)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x22,((IDTTimming.IDTTimming_Q0.DSMfrac >> 22)&0x000000ff)); 
		break;
		case 1:
			Write_IDT5P49V5901(IDTDevice,0x3E,(IDTTimming.IDTTimming_Q1.DSMint & 0x00f)<<4);
			Write_IDT5P49V5901(IDTDevice,0x3D,(IDTTimming.IDTTimming_Q1.DSMint >> 4)); 
			
			Write_IDT5P49V5901(IDTDevice,0x35,(IDTTimming.IDTTimming_Q1.DSMfrac& 0x0000003f)<<2);
			Write_IDT5P49V5901(IDTDevice,0x34,((IDTTimming.IDTTimming_Q1.DSMfrac >> 6)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x33,((IDTTimming.IDTTimming_Q1.DSMfrac >> 14)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x32,((IDTTimming.IDTTimming_Q1.DSMfrac >> 22)&0x000000ff)); 
		break;
		case 2:
			Write_IDT5P49V5901(IDTDevice,0x4E,(IDTTimming.IDTTimming_Q2.DSMint & 0x00f)<<4);
			Write_IDT5P49V5901(IDTDevice,0x4D,(IDTTimming.IDTTimming_Q2.DSMint >> 4));
			
			Write_IDT5P49V5901(IDTDevice,0x45,(IDTTimming.IDTTimming_Q2.DSMfrac& 0x0000003f)<<2);
			Write_IDT5P49V5901(IDTDevice,0x44,((IDTTimming.IDTTimming_Q2.DSMfrac >> 6)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x43,((IDTTimming.IDTTimming_Q2.DSMfrac >> 14)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x42,((IDTTimming.IDTTimming_Q2.DSMfrac >> 22)&0x000000ff)); 
		break;
		case 3:
			Write_IDT5P49V5901(IDTDevice,0x5E,(IDTTimming.IDTTimming_Q3.DSMint & 0x00f)<<4);
			Write_IDT5P49V5901(IDTDevice,0x5D,(IDTTimming.IDTTimming_Q3.DSMint >> 4)); 

			Write_IDT5P49V5901(IDTDevice,0x55,(IDTTimming.IDTTimming_Q3.DSMfrac& 0x0000003f)<<2);
			Write_IDT5P49V5901(IDTDevice,0x54,((IDTTimming.IDTTimming_Q3.DSMfrac >> 6)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x53,((IDTTimming.IDTTimming_Q3.DSMfrac >> 14)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x52,((IDTTimming.IDTTimming_Q3.DSMfrac >> 22)&0x000000ff)); 
		break;

		default:
			Write_IDT5P49V5901(IDTDevice,0x4E,(IDTTimming.IDTTimming_Q2.DSMint & 0x00f)<<4);
			Write_IDT5P49V5901(IDTDevice,0x4D,(IDTTimming.IDTTimming_Q2.DSMint >> 4));
			
			Write_IDT5P49V5901(IDTDevice,0x45,(IDTTimming.IDTTimming_Q2.DSMfrac& 0x0000003f)<<2);
			Write_IDT5P49V5901(IDTDevice,0x44,((IDTTimming.IDTTimming_Q2.DSMfrac >> 6)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x43,((IDTTimming.IDTTimming_Q2.DSMfrac >> 14)&0x000000ff));
			Write_IDT5P49V5901(IDTDevice,0x42,((IDTTimming.IDTTimming_Q2.DSMfrac >> 22)&0x000000ff)); 
		break;
	}
}

void Set5901FreqIndependent(IDT5P49V5901_TypeDef IDTDevice,u64 Fout,u8 port)
{
	if(IDTDevice == IDT5P49V5901_1)
	{
		Calculate_IDTTimmingIndependent(&IDTTimming,Fout,port);
		IDT_FreqRegSettingIndependent(IDTDevice,IDTTimming,port);
	}
	else if(IDTDevice == IDT5P49V5901_2)
	{
		Calculate_IDTTimmingIndependent(&IDTTimming,Fout,port);
		IDT_FreqRegSettingIndependent(IDTDevice,IDTTimming,port);
	}	
}


void SetIDT5901Freq(u64 Fout,u8 port)
{
	Set5901FreqIndependent(IDT5P49V5901_1,Fout,port);
}


