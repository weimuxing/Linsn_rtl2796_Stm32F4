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
#ifndef _DRV_LCD_IF_H_
#define _DRV_LCD_IF_H_


/**** Definition of constants ****/


/**** Definition of types ****/ 


/**** Definition of macros ****/
#define DISPLAY_ON              0xAF    //显示屏开关
#define DISPLAY_OFF             0xAE    

#define INIT_DISP_LINE          0x40    //初始化显示行

#define PAGE_BASE_ADDR          0xB0    //显示位置设置 
#define COLUMN_BASE_ADDR_MSB    0x10    
#define COLUMN_BASE_ADDR_LSB    0x00    

#define READ_STATUS_BUSY        0x80    //读状态
#define READ_STATUS_ADCB        0x40
#define READ_STATUS_ONOFF       0x20
#define READ_STATUS_RESETB      0x10

#define ADC_SEG_NORMAL          0xA0    //地址方向
#define ADC_SEG_REVERSE         0xA1

#define REVERSE_DISP_ON         0xA7    //反白显示
#define REVERSE_DISP_OFF        0xA6

#define ENTIRE_DISP_ON          0xA5    //整屏显示
#define ENTIRE_DISP_OFF         0xA4

#define LCD_BIAS_ON             0xA3    //LCD偏置
#define LCD_BIAS_OFF            0xA2

#define SET_MODIFY_READ         0xE0    //设置修改读 
#define RESET_MODIFY_READ       0xEE    //复位修改读

#define DISP_RESET              0xE2    //复位

#define SHL_COM_NORMAL          0xC0    //设置COM输出方向
#define SHL_COM_REVERSE         0xC8

#define POWER_CTRL_VC           0x2C    //电源控制 
#define POWER_CTRL_VR           0x2A
#define POWER_CTRL_VF           0x29

#define REG_RESISTOR_R2         0x24    //调压电阻器                 
#define REG_RESISTOR_R1         0x22
#define REG_RESISTOR_R0         0x21
 
#define SET_REF_VOL_MODE        0x81    //设置参考电压
#define SET_REF_VOL_SV5         0x20
#define SET_REF_VOL_SV4         0x10
#define SET_REF_VOL_SV3         0x08
#define SET_REF_VOL_SV2         0x04
#define SET_REF_VOL_SV1         0x02
#define SET_REF_VOL_SV0         0x01

#define SET_STATIC_ON           0xAD    //静态指示寄存器 
#define SET_STATIC_OFF          0xAC
#define SET_STATIC_S1           0x02
#define SET_STATIC_S0           0x01

#define POWER_SAVE_STAND        0xA8    //节能模式
#define POWER_SAVE_SLEEP        0xA9
#define POWER_SAVE_RESET        0xE1    //复位节能模式

#define REVERSAL_DRIVE_REG3     0x38    //设置n行反转驱动寄存器
#define REVERSAL_DRIVE_REG2     0x34 
#define REVERSAL_DRIVE_REG1     0x32 
#define REVERSAL_DRIVE_REG0     0x31 
#define REVERSAL_DRIVE_RESET    0xE4    //n行反转驱动寄存器复位 

#define BUILT_OSC_ON            0xAB    //启动内置的振荡器电路

#define EXTERNAL_CAP_ENABLE     0x70    //外部电容器放电 
#define EXTERNAL_CAP_DISABLE    0x77
  
#define DISP_NOP                0xE3    //空操作
      
#define OTP_PROGRAM             0x90    //OTP编程
#define OTP_OTPADJ_0            0x00    //OTP编程选项
#define OTP_OTPADJ_1            0x80
#define OTP_OTPON_0             0x00
#define OTP_OTPON_1             0x40 

#define LCD_CMD                 0       //写命令
#define LCD_DATA                1	//写数据

#define DISPLAY_NORMAL          0       //正常显示
#define DISPLAY_REVERSE         1       //反色显示             
/**** Declaration of constants ****/


/**** Declaration of variables ****/


/**** Declaration of functions ****/
extern void Drv_LCDInit(void);
extern void Drv_LCDWriteByte(uint8 OutData, uint8 DataType);
extern void Drv_DisplayCharacter(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh);
extern void Drv_DisplayCharacterReverse(uint8 CharacterType, uint8 StartPage, uint8 StartColumn, uint8* pCh);
extern void Drv_DisplayString(uint8 DisplayReverse, uint8 StartPage, uint8 StartColumn, uint8* pCh);
extern void Drv_ScreenClear(void);
extern void Drv_LineClear(uint8 Line);
extern void Drv_LineClearR(uint8 Line);
extern void Drv_LCDGRamRefresh(void);
extern void Drv_LCDScreenWrite(uint8* DispData);
extern void Drv_LCDGRamClear(void);

#endif //_DRV_LCD_IF_H_
/****************************** END OF FILE **********************************/

