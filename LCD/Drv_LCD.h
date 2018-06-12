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
#ifndef _DRV_LCD_H_
#define _DRV_LCD_H_


/**** Definition of constants ****/


/**** Definition of types ****/ 
//LCD地址结构体
typedef struct
{
	vuint8 LCD_REG;
	vuint8 LCD_RAM;
} LCD_TypeDef;

/**** Definition of macros ****/
//PB0~7,作为数据线
//#define DATAOUT(DataValue)    {GPIO_Write(GPIOB,(GPIO_ReadOutputData(GPIOB)&0x0fff)|((uint16)(DataValue&0x000F)<<12));  \
//                              GPIO_Write(GPIOC,(GPIO_ReadOutputData(GPIOC)&0xfc3f)|((uint16)(DataValue&0x00F0)<<2));}  
//#define DATAOUT(DataValue)      {GPIOB->ODR = (GPIOB->ODR & 0x0fff)|((uint16)(DataValue & 0x000F)<<12); \
//                                GPIOC->ODR = (GPIOC->ODR & 0xfc3f)|((uint16)(DataValue & 0x00F0)<<2);}

//PB8~PB11,作为控制线
//#define LCD_CS1_        PBout(8)	
//#define LCD_A0          PBout(9)
//#define LCD_WR_         PBout(10)		  
//#define LCD_RD_         PAout(8)

#define COLUMN_NUM      216

//使用NOR/SRAM的Bank1.sector4,地址位HADDR[27,26]=11 A6作为数据命令区分线
//注意设置时STM32内部会右移一位对齐！ 111 1110=0X7E			    
#define LCD_BASE        ((uint32)(0x6C000000 | 0x0000003F))
#define TFTLCM             ((LCD_TypeDef *) LCD_BASE)

/**** Declaration of constants ****/


/**** Declaration of variables ****/


/**** Declaration of functions ****/


#endif //_DRV_LCD_H_
/****************************** END OF FILE **********************************/

