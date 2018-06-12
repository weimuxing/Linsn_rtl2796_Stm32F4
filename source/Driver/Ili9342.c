#include "SysConfig.h"

#include "Ili9342.h"
#include "soft_spi.h"


// 根据9342C+CPT2.0.pdf文档，确认
// 屏使用的接口是: 6-bit Parallel RGB Interface
// 其中IM0[1],IM1[0],IM2[1],IM3[1] 
//  CSX-连接的就是CS
//  RS  --连接的是SCL
// WR RD 连接到GND

// 硬件连接说明
// CN6 中
// LCD_CS --接到了FPGA E10脚 -MCU PG8
// LCD_RESET 
// LCD_DEN
// LCD2.0_SCL->I2C_SCL3 --MCU(I2C_SCL3_207 PA8 )
// LCD2.0_SDA->I2C_SDA3 --MCU(I2C_SDA3_207 PC9 )


//#define  Ili9342_SendCmd			Soft3wireSpi_SendCmd
//#define  Ili9342_SendData		Soft3wireSpi_SendData

#define ILI9342_CS_HIGH() 		GPIO_SetBits(ILI9342_CS_PORT,ILI9342_CS_PIN)
#define ILI9342_CS_LOW() 		GPIO_ResetBits(ILI9342_CS_PORT,ILI9342_CS_PIN)
#define ILI9342_SCLK_HIGH() 		GPIO_SetBits(ILI9342_SCLK_PORT,ILI9342_SCLK_PIN)
#define ILI9342_SCLK_LOW() 		GPIO_ResetBits(ILI9342_SCLK_PORT,ILI9342_SCLK_PIN)
#define ILI9342_SDA_HIGH()  		GPIO_SetBits(ILI9342_SDA_PORT,ILI9342_SDA_PIN)
#define ILI9342_SDA_LOW() 		GPIO_ResetBits(ILI9342_SDA_PORT,ILI9342_SDA_PIN)	
#define ILI9342_SDA_READ()		GPIO_ReadInputDataBit(ILI9342_SDA_PORT,ILI9342_SDA_PIN)
#define ILI9342_RST_HIGH() 		GPIO_SetBits(ILI9342_RST_PORT,ILI9342_RST_PIN)
#define ILI9342_RST_LOW() 		GPIO_ResetBits(ILI9342_RST_PORT,ILI9342_RST_PIN)

void ILI9342_DELAY(void)
{
	u8 i,j;

	for(i=0;i<1;i++)
		for(j=0;j<1;j++);
}

void Ili9342_init_port(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(ILI9342_SPI_AHB1PERIPH, ENABLE);

	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	/* config SCLK pin */
	GPIO_InitStruct.GPIO_Pin = ILI9342_SCLK_PIN;
	GPIO_Init(ILI9342_SCLK_PORT, &GPIO_InitStruct);
	//ILI9342_SCLK_LOW();
	ILI9342_SCLK_HIGH();	
		
	/* config SDA pin */
	GPIO_InitStruct.GPIO_Pin = ILI9342_SDA_PIN;
	GPIO_Init(ILI9342_SDA_PORT, &GPIO_InitStruct);
	ILI9342_SDA_LOW();

	/* config CS pin */
	GPIO_InitStruct.GPIO_Pin =  ILI9342_CS_PIN;
	GPIO_Init(ILI9342_CS_PORT, &GPIO_InitStruct); 
	ILI9342_CS_HIGH();	
}


void Ili9342_SDA_AsOuput(void)
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	

	GPIO_InitStructure.GPIO_Pin = ILI9342_SDA_PIN;
	GPIO_Init(ILI9342_SDA_PORT, &GPIO_InitStructure);	
	//GPIO_ResetBits(ILI9342_SDA_PORT,SOFT_SPIC_SDA_PIN);
 #endif   
}

void Ili9342_SDA_AsInput(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		

	GPIO_InitStructure.GPIO_Pin = ILI9342_SDA_PIN;
	GPIO_Init(ILI9342_SDA_PORT, &GPIO_InitStructure);	
	GPIO_ResetBits(ILI9342_SDA_PORT,ILI9342_SDA_PIN);
}

/******************************************/	
static void Ili9342_writeByte(BYTE value)
{
	int i;

	for(i = 0;i < 8;i++)
	{
		ILI9342_SCLK_LOW();
		if(value &0x80)
		{
			ILI9342_SDA_HIGH();
		}
		else
		{
			ILI9342_SDA_LOW();
		}
		value <<=1;
 		ILI9342_DELAY();

		ILI9342_SCLK_HIGH();
		ILI9342_DELAY();
	}
}

void Ili9342_SendCmd(u8 cmd)
{
	Ili9342_SDA_AsOuput();
	ILI9342_CS_HIGH();
 	ILI9342_SCLK_LOW();
	ILI9342_SDA_LOW();

	ILI9342_CS_LOW();
	ILI9342_DELAY();
	
	ILI9342_SCLK_HIGH();
	ILI9342_DELAY();
 
	Ili9342_writeByte(cmd);

	ILI9342_CS_HIGH();
	ILI9342_SDA_HIGH();
	ILI9342_SCLK_HIGH();
	ILI9342_DELAY();

}


void Ili9342_SendData(u8 value)
{
	//Ili9342_SDA_AsOuput();
 	ILI9342_SCLK_LOW();
 	ILI9342_SDA_HIGH();
 	ILI9342_CS_LOW();
	ILI9342_DELAY();
	
  	ILI9342_SCLK_LOW();
	//ILI9342_DELAY();
  	ILI9342_SCLK_HIGH();
 	ILI9342_DELAY();

	Ili9342_writeByte(value);

	ILI9342_CS_HIGH();
	ILI9342_SDA_HIGH();
	ILI9342_SCLK_HIGH();
	ILI9342_DELAY();

}

u8 Ili9342_RecvByte(u8 bEnd)    
{  
	u8 i = 0;  
	u8 value = 0;

	Ili9342_SDA_AsInput();
	ILI9342_CS_LOW();
	ILI9342_DELAY();
	
	for(i = 0; i < 8;i++)  
	{		
		ILI9342_SCLK_LOW();//上升沿有效	
		ILI9342_DELAY();	

		ILI9342_SCLK_HIGH();
		//ILI9342_DELAY();
  		value<<=1; 
		if(Bit_SET == ILI9342_SDA_READ())
		{
			value++; 
		}	
		ILI9342_DELAY();		
	} 	

	ILI9342_CS_HIGH();
	ILI9342_SDA_HIGH();
	ILI9342_SCLK_HIGH();
	ILI9342_DELAY();

	return value;
}

void ili9342c_reset(void)
{
    if(getCommHardVersion() > 0)
    {
     	GPIO_InitTypeDef GPIO_InitStruct;

    	//RCC_AHB1PeriphClockCmd(ILI9342_RST_AHB1PERIPH, ENABLE);
    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    	
    	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    		
    	GPIO_InitStruct.GPIO_Pin = ILI9342_RST_PIN;
    	GPIO_Init(ILI9342_RST_PORT, &GPIO_InitStruct);

    	ILI9342_RST_HIGH();
    	SysDelayMs(10);

    	//ILI9342_RST_LOW();	        /* 这里不能拉低，拉低之后不能正常复位V1.34*/
    	//SysDelayMs(1000);

    	ILI9342_RST_HIGH();
    	SysDelayMs(300);       
    }
}

void ili9342c_Initial(void)
{
	ili9342c_reset();
	
	Ili9342_init_port();

#if 0		// sample
	Ili9342_SendCmd(0xB9);		//Set EXTC(External Command)	
	Ili9342_SendData(0xFF);	
	Ili9342_SendData(0x93);	
	Ili9342_SendData(0x42);	

	Ili9342_SendCmd(0x21);		// Display Inversion ON
	
	Ili9342_SendCmd(0x36);		//Memory Access Control
	Ili9342_SendData(0xC0);	//C0//80//00	(MY MX MV ML BGR MH 0 0)

 	Ili9342_SendCmd(0xC0);	//Power Control 1
	Ili9342_SendData(0x25);	// VRH1[5:0] 亮度   越大越暗
	Ili9342_SendData(0x0A);	//  VC[3:0] 对比度      值小对比度越大

	Ili9342_SendCmd(0xC1);	//Power Control 2
	Ili9342_SendData(0x01);	//SAP[2:0] BT[3:0]

	Ili9342_SendCmd(0xC5);	//VCOM Control 1
	Ili9342_SendData(0x2F);	
	Ili9342_SendData(0x27);

	Ili9342_SendCmd(0xC7);//VCOM Control 2	
	Ili9342_SendData(0xC8);	//nVM VMF[6:0]

	//Ili9342_SendCmd(0xB8);	//Oscilator Control
	//Ili9342_SendData(0x0B);	//POSC[3:0]

	//Ili9342_SendCmd(0xF2);	// ???
	//Ili9342_SendData(0x00);	//

	//*****************GAMMA*****************
	Ili9342_SendCmd(0xE0);	//Positive Gamma Correction		
	Ili9342_SendData(0x0F);	
	Ili9342_SendData(0x25);	
	Ili9342_SendData(0x24);	
	Ili9342_SendData(0x0B);	
	Ili9342_SendData(0x0F);	
	Ili9342_SendData(0x09);		
	Ili9342_SendData(0x52);		
	Ili9342_SendData(0x76);	
	Ili9342_SendData(0x40);	
	Ili9342_SendData(0x00);	
	Ili9342_SendData(0x00);	
	Ili9342_SendData(0x00);	
	Ili9342_SendData(0x00);
	Ili9342_SendData(0x00);			
	Ili9342_SendData(0x00);	

	Ili9342_SendCmd(0xE1);	//Negative Gamma Correction
	Ili9342_SendData(0x00);		
	Ili9342_SendData(0x1A);	
	Ili9342_SendData(0x1B);	
	Ili9342_SendData(0x04);	
	Ili9342_SendData(0x10);	
	Ili9342_SendData(0x06);	
	Ili9342_SendData(0x2D);		
	Ili9342_SendData(0x23);		
	Ili9342_SendData(0x3F);	
	Ili9342_SendData(0x0F);	
	Ili9342_SendData(0x1F);	
	Ili9342_SendData(0x0F);	
	Ili9342_SendData(0x3F);	
	Ili9342_SendData(0x3F);			
	Ili9342_SendData(0x0F);	

	Ili9342_SendCmd(0x3A);//Pixel Format Set	
	//Ili9342_SendData(0x66);	 //X DPI[2:0], X DBI[2:0]
	Ili9342_SendData(0x55);	 //X DPI[2:0], X DBI[2:0]	

	Ili9342_SendCmd(0xf6);// Interface Control
	Ili9342_SendData(0x00);	// MY_EOR MX_EOR MV_EOR 0 BGR_EOR 0 0 WEMODE   default:01
	Ili9342_SendData(0x00);	// 0 0 EPF[1:0] 0 0 0 0  default:00
	//Ili9342_SendData(0x03);	// 0 0 ENDIAN 0 DM[1:0] RM RIM default:00
	Ili9342_SendData(0x07);	// 0 0 ENDIAN 0 DM[1:0] RM RIM default:00
	
	Ili9342_SendData(0xB0);// RGB Interface Signal Control
	//Ili9342_SendData(0xB1);	//BYPASS RCM[1:0] 0 VSPL HSPL DPL EPL     --- 40:DE  60:SYNC   
	Ili9342_SendData(0xE0);	//BYPASS RCM[1:0] 0 VSPL HSPL DPL EPL     --- 40:DE  60:SYNC   

	//Ili9342_SendCmd(0x0c);	// Read Display Pixel Format
	//Ili9342_SendData(0x00);
	//Ili9342_SendData(0xe6);	

	Ili9342_SendCmd(0x11);
	SysDelayMs(200);	

	Ili9342_SendCmd(0x29);	//Display On

	SysDelayMs(200);
	//Ili9342_SendCmd(0x2C);// Memory Write
#elif 1
	Ili9342_SendCmd(0x01);
	SysDelayMs(150);

	Ili9342_SendCmd(0x11); //Exit SleepDelay(120);
	SysDelayMs(150);

	Ili9342_SendCmd(0xC8);		// ??
	//Ili9342_SendCmd(0xB9);		//Set EXTC(External Command)	
	Ili9342_SendData (0xFF);
	Ili9342_SendData (0x93);
	Ili9342_SendData (0x42);

	//Ili9342_SendCmd(0x21);		// Display Inversion ON

	Ili9342_SendCmd(0x36);		//Memory Access Control
	Ili9342_SendData (0xC0);	//C0//80//00	(MY MX MV ML BGR MH 0 0) 

	//Ili9342_SendCmd(0xC0);	//Power Control 1
	//Ili9342_SendData (0x0f);	// VRH1[5:0] 亮度	越大越暗
	//Ili9342_SendData (0x0f);	//	VC[3:0] 对比度		值小对比度越大

	Ili9342_SendCmd(0xC1);	//Power Control 2
	Ili9342_SendData (0x06);	//SAP[2:0] BT[3:0]

 	Ili9342_SendCmd(0xC0);	//Power Control 1
	Ili9342_SendData(0x25);	// VRH1[5:0] 亮度   越大越暗
	Ili9342_SendData(0x0A);	//  VC[3:0] 对比度      值小对比度越大

	//Ili9342_SendCmd(0xC1);	//Power Control 2
	//Ili9342_SendData(0x01);	//SAP[2:0] BT[3:0]	

	Ili9342_SendCmd(0xC5);	//VCOM Control 1
	Ili9342_SendData (0xDB);//DF

	Ili9342_SendCmd(0xB4);	
	Ili9342_SendData (0x02); 

#if 0
//*****************GAMMA*****************
Ili9342_SendCmd(0xE0);	//Positive Gamma Correction 	
Ili9342_SendData(0x0F); 
Ili9342_SendData(0x25); 
Ili9342_SendData(0x24); 
Ili9342_SendData(0x0B); 
Ili9342_SendData(0x0F); 
Ili9342_SendData(0x09); 	
Ili9342_SendData(0x52); 	
Ili9342_SendData(0x76); 
Ili9342_SendData(0x40); 
Ili9342_SendData(0x00); 
Ili9342_SendData(0x00); 
Ili9342_SendData(0x00); 
Ili9342_SendData(0x00);
Ili9342_SendData(0x00); 		
Ili9342_SendData(0x00); 

Ili9342_SendCmd(0xE1);	//Negative Gamma Correction
Ili9342_SendData(0x00); 	
Ili9342_SendData(0x1A); 
Ili9342_SendData(0x1B); 
Ili9342_SendData(0x04); 
Ili9342_SendData(0x10); 
Ili9342_SendData(0x06); 
Ili9342_SendData(0x2D); 	
Ili9342_SendData(0x23); 	
Ili9342_SendData(0x3F); 
Ili9342_SendData(0x0F); 
Ili9342_SendData(0x1F); 
Ili9342_SendData(0x0F); 
Ili9342_SendData(0x3F); 
Ili9342_SendData(0x3F); 		
Ili9342_SendData(0x0F); 

#else
	//*****************GAMMA*****************
	Ili9342_SendCmd(0xE0);	//Positive Gamma Correction
	Ili9342_SendData (0x00);
	Ili9342_SendData (0x05);
	Ili9342_SendData (0x08);
	Ili9342_SendData (0x02);
	Ili9342_SendData (0x1A);
	Ili9342_SendData (0x0C);
	Ili9342_SendData (0x42);
	Ili9342_SendData (0x7A);
	Ili9342_SendData (0x54);
	Ili9342_SendData (0x08);
	Ili9342_SendData (0x0D);
	Ili9342_SendData (0x0C);
	Ili9342_SendData (0x23);
	Ili9342_SendData (0x25);
	Ili9342_SendData (0x0F);

	Ili9342_SendCmd(0xE1);	//Negative Gamma Correction
	Ili9342_SendData (0x00);
	Ili9342_SendData (0x29);
	Ili9342_SendData (0x2F);
	Ili9342_SendData (0x03);
	Ili9342_SendData (0x0F);
	Ili9342_SendData (0x05);
	Ili9342_SendData (0x42);
	Ili9342_SendData (0x55);
	Ili9342_SendData (0x53);
	Ili9342_SendData (0x06);
	Ili9342_SendData (0x0F);
	Ili9342_SendData (0x0C);
	Ili9342_SendData (0x38);
	Ili9342_SendData (0x3A);
	Ili9342_SendData (0x0F);
#endif

	Ili9342_SendCmd(0x3A);//Pixel Format Set
	Ili9342_SendData (0x66);	 //X DPI[2:0], X DBI[2:0]	
	//Ili9342_SendData (0x55);	 //X DPI[2:0], X DBI[2:0]	

	Ili9342_SendCmd(0xF6);// Interface Control
	Ili9342_SendData (0x01);// MY_EOR MX_EOR MV_EOR 0 BGR_EOR 0 0 WEMODE   default:01
	Ili9342_SendData (0x00);// 0 0 EPF[1:0] 0 0 0 0  default:00
	//Ili9342_SendData (0x07);// 0 0 ENDIAN 0 DM[1:0] RM RIM default:00
	Ili9342_SendData (0x03);// 0 0 ENDIAN 0 DM[1:0] RM RIM default:00

	Ili9342_SendCmd(0xB0);// RGB Interface Signal Control
	Ili9342_SendData(0xE0);//BYPASS RCM[1:0] 0 VSPL HSPL DPL EPL	 --- 40:DE	60:SYNC   
	//Ili9342_SendData(0xE1);//BYPASS RCM[1:0] 0 VSPL HSPL DPL EPL	 --- 40:DE	60:SYNC   	
	//Ili9342_SendData(0xC0);//BYPASS RCM[1:0] 0 VSPL HSPL DPL EPL	 --- 40:DE	60:SYNC   	

	Ili9342_SendCmd(0xB5);		// Blanking Porch
	Ili9342_SendData(0x02); 		// VFP [6:0]
	Ili9342_SendData(0x02); 		// VBP [6:0]
	Ili9342_SendData(0x1D); 		// HFP [4:0]
	Ili9342_SendData(0x01); 		// HBP [4:0]

	Ili9342_SendCmd(0x11); //Exit SleepDelay(120);
	SysDelayMs(150);
	Ili9342_SendCmd(0x29);	//Display On
	SysDelayMs(10);
	Ili9342_SendCmd(0x29);	//Display On
#else
	Ili9342_SendCmd(0xC8);		// ??
	//Ili9342_SendCmd(0xB9);		//Set EXTC(External Command)	
	Ili9342_SendData (0xFF);
	Ili9342_SendData (0x93);
	Ili9342_SendData (0x42);

	//Ili9342_SendCmd(0x21);		// Display Inversion ON

	Ili9342_SendCmd(0x36);		//Memory Access Control
	Ili9342_SendData (0xC8); 	//C0//80//00	(MY MX MV ML BGR MH 0 0) 

	Ili9342_SendCmd(0xC0);	//Power Control 1
	Ili9342_SendData (0x0f);	// VRH1[5:0] 亮度   越大越暗
	Ili9342_SendData (0x0f);	//  VC[3:0] 对比度      值小对比度越大

	Ili9342_SendCmd(0xC1);	//Power Control 2
	Ili9342_SendData (0x06);	//SAP[2:0] BT[3:0]

	Ili9342_SendCmd(0xC5); 	//VCOM Control 1
	Ili9342_SendData (0xDB);//DF

	Ili9342_SendCmd(0xB4);  
	Ili9342_SendData (0x02); 

	//*****************GAMMA*****************
	Ili9342_SendCmd(0xE0);	//Positive Gamma Correction
	Ili9342_SendData (0x00);
	Ili9342_SendData (0x05);
	Ili9342_SendData (0x08);
	Ili9342_SendData (0x02);
	Ili9342_SendData (0x1A);
	Ili9342_SendData (0x0C);
	Ili9342_SendData (0x42);
	Ili9342_SendData (0x7A);
	Ili9342_SendData (0x54);
	Ili9342_SendData (0x08);
	Ili9342_SendData (0x0D);
	Ili9342_SendData (0x0C);
	Ili9342_SendData (0x23);
	Ili9342_SendData (0x25);
	Ili9342_SendData (0x0F);

	Ili9342_SendCmd(0xE1);	//Negative Gamma Correction
	Ili9342_SendData (0x00);
	Ili9342_SendData (0x29);
	Ili9342_SendData (0x2F);
	Ili9342_SendData (0x03);
	Ili9342_SendData (0x0F);
	Ili9342_SendData (0x05);
	Ili9342_SendData (0x42);
	Ili9342_SendData (0x55);
	Ili9342_SendData (0x53);
	Ili9342_SendData (0x06);
	Ili9342_SendData (0x0F);
	Ili9342_SendData (0x0C);
	Ili9342_SendData (0x38);
	Ili9342_SendData (0x3A);
	Ili9342_SendData (0x0F);

	Ili9342_SendCmd(0x3A);//Pixel Format Set
	Ili9342_SendData (0x66);	 //X DPI[2:0], X DBI[2:0]	
	//Ili9342_SendData (0x55);	 //X DPI[2:0], X DBI[2:0]	

	Ili9342_SendCmd(0xF6);// Interface Control
	Ili9342_SendData (0x01);// MY_EOR MX_EOR MV_EOR 0 BGR_EOR 0 0 WEMODE   default:01
	Ili9342_SendData (0x00);// 0 0 EPF[1:0] 0 0 0 0  default:00
	Ili9342_SendData (0x07);// 0 0 ENDIAN 0 DM[1:0] RM RIM default:00

	Ili9342_SendCmd(0xB0);// RGB Interface Signal Control
	Ili9342_SendData(0xE0);//BYPASS RCM[1:0] 0 VSPL HSPL DPL EPL     --- 40:DE  60:SYNC   

	Ili9342_SendCmd(0xB5);		// Blanking Porch
	Ili9342_SendData(0x02);			// VFP [6:0]
	Ili9342_SendData(0x02);			// VBP [6:0]
	Ili9342_SendData(0x0A);			// HFP [4:0]
	Ili9342_SendData(0x14);			// HBP [4:0]

	Ili9342_SendCmd(0x11); //Exit SleepDelay(120);

	Ili9342_SendCmd(0x29);	//Display On
#endif
}

/**************************************************
*功能:读ID号
*输入:NULL
*输出:16位ID,正确返回值应该为0X9342
**************************************************/
void Ili9342_Check_id(void)
{
#if 0

	int byte1,byte2,byte3,byte4;


	Ili9342_SendCmd(ILI9342_CMD_RD_ID,0);
	byte1 = Ili9342_RecvByte(0);	//dummy read 假读
	byte2 = Ili9342_RecvByte(0);	// LCD module's manufacturer ID.
	byte3 = Ili9342_RecvByte(0);	//LCD module/driver version ID. 0x0093
	byte4 = Ili9342_RecvByte(1);	//LCD module/driver ID.0x0042

	Ili9342_SendCmd(ILI9342_CMD_RD_ID1,0);
	byte1 = Ili9342_RecvByte(0);	//dummy read 假读
	byte2 = Ili9342_RecvByte(1);	// LCD module's manufacturer ID.

	Ili9342_SendCmd(ILI9342_CMD_RD_ID2,0);
	byte1 = Ili9342_RecvByte(0);	//dummy read 假读
	byte2 = Ili9342_RecvByte(1);	//LCD module/driver version ID and the ID parameter range is from 80h to FFh.

	Ili9342_SendCmd(ILI9342_CMD_RD_ID3,0);
	byte1 = Ili9342_RecvByte(0);	//dummy read 假读
	byte2 = Ili9342_RecvByte(1);	//LCD module/driver ID.


	SysDelayMs(1);
#endif

}


