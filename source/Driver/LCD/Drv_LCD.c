/******************************************************************************
*   Copyright (C) Lingxinyu Technology(ShenZhen) Inc.                         *
*                 All Rights Reserved.                                        *
*   Department :  R&D Department                                              *
*   AUTHOR     :  Huang Feineng                                               *
*******************************************************************************
* Project       : 
* Module        : 
* File          : 
* Description   : 
*-----------------------------------------------------------------------
* Version:  V0.01
* Date:     2015/10/30
* Author:   Huang Feineng
***********************************************************************/
/*-History---------------------------------------------------------------------
* Version       Date        Name               Changes and comments
* V0.01         2015/10/30  Huang Feineng      initial version
*============================================================================*/
//#include "INCLUDES.H"
#include "Drv_LCD.h"
#include "Drv_LCD_if.h"
//#include "fonts.h"
#include "stdio.h"
#include "userPref.h"
#include "string.h"
#include "spi_flash.h"
#include "picture.h"
#include "SysTick.h"
#include "Spi_flash.h"
#include "soft_spi.h"

/*===VARIABLES========================================================*/

/*---Global-----------------------------------------------------------*/
static uint8 LCD_GRam[8][COLUMN_NUM];

/*---Private----------------------------------------------------------*/


/*===FUNCTIONS========================================================*/

/*---Global-----------------------------------------------------------*/

/*---Private----------------------------------------------------------*/

/******************************************************************************
**  FUNCTION      : Drv_LCDInit                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : void
**  RETURN        : void                                                          
******************************************************************************/
void Drv_LCDInit(void)
{
    vuint32 i=0;
	
    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);//使能PD,PE,PF,PG时钟  
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);     //使能FSMC时钟  
	
	//BL_ADJ--PE3
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;              //PE3 推挽输出,控制背光
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //普通输出模式
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //100MHz
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
    //GPIO_Init(GPIOE, &GPIO_InitStructure);                  //初始化 //PE3 推挽输出,控制背光
	
	//D2,D3,NOE,NWE,D13,D14,D15,D0,D1--PD
    GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(3<<14);   			//PD0,1,4,5,8,9,10,14,15 AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                    //复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;              //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                    //上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);                          //初始化  
	
	//D4~D12--PE
    GPIO_InitStructure.GPIO_Pin = (0XF<<7);                       	//PE7~10,AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                    //复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;              //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                    //上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);                          //初始化  

	//A6--PF
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                      //PF12,FSMC_A6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                    //复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;              //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                    //上拉
    GPIO_Init(GPIOF, &GPIO_InitStructure);                          //初始化  

	//NE4--PG
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                      	//PG4,FSMC_NE4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                    //复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;              //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                    //上拉
    GPIO_Init(GPIOG, &GPIO_InitStructure);                          //初始化 

	//
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);           //PD0,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);           //PD1,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
    //GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
    //GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
    //GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);          //PD15,AF12
 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);           //PE7,AF12
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
    //GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
    //GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
    //GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
    //GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
    //GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);          //PE15,AF12

    GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);          	//PF12,AF12
    GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);


    readWriteTiming.FSMC_AddressSetupTime = 0XF;	                //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns	
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	                //地址保持时间（ADDHLD）模式A未用到	
    readWriteTiming.FSMC_DataSetupTime = 60;			            //数据保存时间为60个HCLK	=6*60=360ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	        //模式A 
    

    writeTiming.FSMC_AddressSetupTime =9;	                        //地址建立时间（ADDSET）为9个HCLK =54ns 
    writeTiming.FSMC_AddressHoldTime = 0x00;	                    //地址保持时间（A		
    writeTiming.FSMC_DataSetupTime = 8;		                        //数据保存时间为6ns*9个HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	            //模式A 

 
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;                          //  这里我们使用NE4 ，也就对应BTCR[6],[7]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;        // 不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;                    // FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;          //存储器数据宽度为16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;       // FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	        //  存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;             // 读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;            //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                    //写时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);                                       //初始化FSMC配置

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);                                       // 使能BANK1 
		
 	SysDelay1ms(50);                                                                 // Delay 50 ms 
 
 //-----------------------------------------------------------   
    Drv_LCDWriteByte(ADC_SEG_NORMAL, LCD_CMD);
    Drv_LCDWriteByte(SHL_COM_REVERSE, LCD_CMD);
    Drv_LCDWriteByte(LCD_BIAS_ON, LCD_CMD);
    Drv_LCDWriteByte(INIT_DISP_LINE, LCD_CMD);
    Drv_LCDWriteByte(ENTIRE_DISP_OFF, LCD_CMD);
    Drv_LCDWriteByte(REVERSE_DISP_OFF, LCD_CMD);
    Drv_LCDWriteByte(POWER_CTRL_VC|POWER_CTRL_VR|POWER_CTRL_VF, LCD_CMD);
    Drv_LCDWriteByte(BUILT_OSC_ON, LCD_CMD);
    Drv_LCDWriteByte(DISPLAY_ON, LCD_CMD);
    //SysDelay1ms(500);
    Drv_LCDWriteByte(REG_RESISTOR_R1, LCD_CMD);       
    //SysDelay1ms(500);
    Drv_ScreenClear();
}

/******************************************************************************
**  FUNCTION      : Drv_LCDWriteByte                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : uint8 OutData, uint8 DataType
**  RETURN        : void                                                          
******************************************************************************/
/*void Drv_LCDWriteByte(uint8 OutData, uint8 DataType)
{
    DATAOUT(OutData);	    
    LCD_A0   = DataType;
    LCD_CS1_ = 0;	   
    LCD_WR_  = 0;	 
    LCD_WR_  = 1;
    LCD_CS1_ = 1;	  
    LCD_A0   = 1;	  	    	   	
}
*/
void Drv_LCDWriteByte(uint8 OutData, uint8 DataType)
{
	if(DataType == LCD_CMD)
	{
		TFTLCM->LCD_REG = OutData;   // Write CMD
	}
	else if(DataType == LCD_DATA)
	{
		TFTLCM->LCD_RAM = OutData;     // Write Data		
	}
}
extern const unsigned char ASCII8x16_Table [];

/******************************************************************************
**  FUNCTION      : Drv_DisplayCharacter                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh
**  RETURN        : void                                                          
******************************************************************************/
void Drv_DisplayEnCharacter(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh)
{
    uint8 i, j;
    uint8* pMsk;
    pMsk = NULL;//FontFind(pCh);	
    pMsk = (uint8*)&ASCII8x16_Table[(*pCh-32)*16];//[]
    
    for(i=0;i<2;i++)		                //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+StartPage+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB+(StartColumn>>4), LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB+(StartColumn&0x0F), LCD_CMD);
        for(j=0;j<CharacterType;j++)
        {
            Drv_LCDWriteByte(*(pMsk+j+CharacterType*i), LCD_DATA);
        }
    }
    return ;
}

void GetHzMat(const char *code,unsigned char *mat,u8 size) ;
/******************************************************************************
**  FUNCTION      : Drv_DisplayCharacter                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh
**  RETURN        : void                                                          
******************************************************************************/
void Drv_DisplayCnCharacter(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh)
{
    uint8 i, j;
    uint8* pMsk;
	BYTE HZbuf[32] = {	
						0x3F,0xF0,0x20,0x20,0x3F,0xF0,0x00,0x00,0x7F,0xFF,0x42,0x08,0x7D,0xF0,0x00,0x00,
						0x4F,0xF0,0x48,0x20,0x4F,0xF2,0x40,0x01,0x7F,0xFE,0x40,0x00,0x00,0x00,0x00,0x00
					};/*"?",0*/
    pMsk = NULL;//FontFind(pCh);	
    //pMsk = (uint8*)&ASCII8x16_Table[(*pCh-32)*16];//[]
    GetHzMat(pCh, HZbuf, 32);
	pMsk = HZbuf;
    for(i=0;i<2;i++)		                //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+StartPage+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB+(StartColumn>>4), LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB+(StartColumn&0x0F), LCD_CMD);
        for(j=0;j<CharacterType;j++)
        {
            Drv_LCDWriteByte(*(pMsk+j+CharacterType*i), LCD_DATA);
        }
    }
    return ;
}

/******************************************************************************
**  FUNCTION      : Drv_DisplayCharacter                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh
**  RETURN        : void                                                          
******************************************************************************/
void Drv_DisplayCharacter(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh)
{
    uint8 i, j;
    uint8* pMsk;
    pMsk = NULL;//FontFind(pCh);	
    pMsk = (uint8*)&ASCII8x16_Table[(*pCh-32)*16];//[]
    for(i=0;i<2;i++)		                //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+StartPage+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB+(StartColumn>>4), LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB+(StartColumn&0x0F), LCD_CMD);
        for(j=0;j<CharacterType;j++)
        {
            Drv_LCDWriteByte(*(pMsk+j+CharacterType*i), LCD_DATA);
        }
    }
    return ;
}

void Drv_DisplayCnCharacterReverse(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh)
{
    uint8 i, j;
    uint8* pMsk;
	BYTE HZbuf[32] = {	
						0x3F,0xF0,0x20,0x20,0x3F,0xF0,0x00,0x00,0x7F,0xFF,0x42,0x08,0x7D,0xF0,0x00,0x00,
						0x4F,0xF0,0x48,0x20,0x4F,0xF2,0x40,0x01,0x7F,0xFE,0x40,0x00,0x00,0x00,0x00,0x00
					};/*"?",0*/
    pMsk = NULL;//FontFind(pCh);	
    //pMsk = (uint8*)&ASCII8x16_Table[(*pCh-32)*16];//[]
    GetHzMat(pCh, HZbuf, 32);
	pMsk = HZbuf;
    for(i=0;i<2;i++)		                //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+StartPage+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB+(StartColumn>>4), LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB+(StartColumn&0x0F), LCD_CMD);
        for(j=0;j<CharacterType;j++)
        {
            Drv_LCDWriteByte(~(*(pMsk+j+CharacterType*i)), LCD_DATA);
        }
    }
    return ;
}

void Drv_DisplayEnCharacterReverse(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh)
{
    uint8 i, j;
    uint8* pMsk;
	
    pMsk = (uint8*)&ASCII8x16_Table[(*pCh-32)*16];//[]

    for(i=0;i<2;i++)		                //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+StartPage+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB+(StartColumn>>4), LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB+(StartColumn&0x0F), LCD_CMD);
        for(j=0;j<CharacterType;j++)
        {
            Drv_LCDWriteByte(~(*(pMsk+j+CharacterType*i)), LCD_DATA);
        }
    }
    return ;
}


/******************************************************************************
**  FUNCTION      : Drv_DisplayCharacterReverse                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh
**  RETURN        : void                                                          
******************************************************************************/
void Drv_DisplayCharacterReverse(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh)
{
    uint8 i, j;
    uint8* pMsk;
	
    pMsk = 0;//FontFind(pCh);	

    for(i=0;i<2;i++)		                //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+StartPage+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB+(StartColumn>>4), LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB+(StartColumn&0x0F), LCD_CMD);
        for(j=0;j<CharacterType;j++)
        {
            Drv_LCDWriteByte(~(*(pMsk+j+CharacterType*i)), LCD_DATA);
        }
    }
    return ;
}

/******************************************************************************
**  FUNCTION      : Drv_DisplayString                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : uint8 DisplayReverse, uint8 StartPage, uint8 StartColumn, uint8* pCh  
**  RETURN        : void                                                       
******************************************************************************/
void Drv_DisplayString(uint8 DisplayReverse, uint8 StartPage, uint8 StartColumn, uint8* pCh)
{
    uint8 CharacterType;
    uint8 Page, Column;
    uint8* pChTemp;

    Page   = StartPage;
    Column =StartColumn;
    pChTemp = pCh;

    while(*pChTemp != '\0')
    {
        if((*pChTemp&0x80)==0x00)       CharacterType = 8;
        else                            CharacterType = 16;//FONT_MSK_LENTH;	

        if(Column+CharacterType>COLUMN_NUM)	        //换行
        {
            Page  += 2;
            Column = 0;		
        }
        if(Page > 7)		                	//换屏
        {
            Page = 0;
            Drv_ScreenClear();		
        }

	if(DisplayReverse == DISPLAY_REVERSE)
        {
            Drv_DisplayCharacterReverse(CharacterType, Page, Column, pChTemp);
        }
        else
        {
            Drv_DisplayCharacter(CharacterType, Page, Column, pChTemp);
        }

        Column +=CharacterType;			        //改变位置
	
        if(CharacterType == 8)			        //改变字符
        {
            pChTemp +=1;
        }
        else
        {
            pChTemp +=2;
        }
    }	
}

/******************************************************************************
**  FUNCTION      : Drv_ScreenClear                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : void
**  RETURN        : void                                                          
******************************************************************************/
void Drv_ScreenClear(void)
{
    uint8 i, j;	

    for(i=0;i<8;i++)		                        //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB, LCD_CMD);
        for(j=0;j<COLUMN_NUM;j++)
        {
            Drv_LCDWriteByte(0x00, LCD_DATA);
        }
    }	
}

/******************************************************************************
**  FUNCTION      : Drv_LineClear                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : void
**  RETURN        : uint8 Line                                                         
******************************************************************************/
void Drv_LineClear(uint8 Line)
{
    uint8 i, j, Page;
	
    if(Line > 4) return ;
	
    Page = (Line-1)*2;
    for(i=0; i<2; i++)
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+Page+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB, LCD_CMD);
        for(j=0;j<COLUMN_NUM;j++)
        {
            Drv_LCDWriteByte(0x00, LCD_DATA);
        }
    }	
    return ;		
}

/******************************************************************************
**  FUNCTION      : Drv_LineClearR                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : void
**  RETURN        : uint8 Line                                                         
******************************************************************************/
void Drv_LineClearR(uint8 Line)
{
    uint8 i, j, Page;
	
    if(Line > 4) return ;
	
    Page = (Line-1)*2;
    for(i=0; i<2; i++)
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+Page+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB, LCD_CMD);
        for(j=0;j<COLUMN_NUM;j++)
        {
            Drv_LCDWriteByte(0xFF, LCD_DATA);
        }
    }	
    return ;		
}

/******************************************************************************
**  FUNCTION      : Drv_LCDGRamRefresh                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : void
**  RETURN        : void                                                          
******************************************************************************/
void Drv_LCDGRamRefresh(void)
{
    uint8 i, j;	

    for(i=0;i<8;i++)		                        //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB, LCD_CMD);
        for(j=0;j<COLUMN_NUM;j++)
        {
            Drv_LCDWriteByte(LCD_GRam[i][j], LCD_DATA);
        }
    }	
}

/******************************************************************************
**  FUNCTION      : Drv_LCDScreenWrite                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : void
**  RETURN        : void                                                          
******************************************************************************/
void Drv_LCDScreenWrite(uint8* DispData)
{
    uint8 i, j;	

    for(i=0;i<8;i++)		                        //PageNum
    {
        Drv_LCDWriteByte(PAGE_BASE_ADDR+i, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_MSB, LCD_CMD);
        Drv_LCDWriteByte(COLUMN_BASE_ADDR_LSB, LCD_CMD);
        for(j=0;j<COLUMN_NUM;j++)
        {
            Drv_LCDWriteByte(DispData[i*COLUMN_NUM+j], LCD_DATA);
        }
    }	
}

/******************************************************************************
**  FUNCTION      : Drv_LCDGRamClear                                       
**  DESCRIPTION   : NA	
**  PARAMETERS    : void
**  RETURN        : void                                                          
******************************************************************************/
void Drv_LCDGRamClear(void)
{
    uint8 i, j;	

    for(i=0;i<8;i++)		                        //PageNum
    {
        for(j=0;j<COLUMN_NUM;j++)
        {
            LCD_GRam[i][j] = 0x00;
        }
    }
}

/*---Private----------------------------------------------------------*/

/****************************** END OF FILE **********************************/

