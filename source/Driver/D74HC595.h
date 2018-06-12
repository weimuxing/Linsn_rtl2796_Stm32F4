#ifndef _D74HC595_H_
#define _D74HC595_H_

#include "define.h"



//PD11	LED_IN
//PD12	LED_SRCK
//PE12	LED_RCK


#define MAX_595_NUMS				4



#define _74HC595_PIN0               0x01
#define _74HC595_PIN1               0x02
#define _74HC595_PIN2               0x04
#define _74HC595_PIN3               0x08
#define _74HC595_PIN4               0x10
#define _74HC595_PIN5               0x20
#define _74HC595_PIN6               0x40
#define _74HC595_PIN7               0x80


typedef enum
{
    _74HC595_CHIP_NONE,
    _74HC595_CHIP_IDX_1,
    _74HC595_CHIP_IDX_2,
    _74HC595_CHIP_IDX_3,
    _74HC595_CHIP_IDX_4,
   
    _74HC595_CHIP_IDX_MAX,
}_74HC595_ChipId_Def;



#define HC595_SERIN_PIN 	GPIO_Pin_1
#define HC595_SERIN_PORT	GPIOD
#define HC595_SH_CLK_PIN	GPIO_Pin_6
#define HC595_SH_CLK_PORT	GPIOE
#define HC595_ST_CLK_PIN	GPIO_Pin_2
#define HC595_ST_CLK_PORT	GPIOD


void init_74HC595(void);
//void write_74HC595(u64 value, u32 countOf);
void write_74HC595_invert(u64 value, u32 countOf);

void writeBits_74HC595(u64 value, u64 mask, u32 countOf);
void SetBits_74HC595(u64 value, u32 countOf);
void ResetBits_74HC595(u64 value, u32 countOf);
void ResetValue_74HC595(u64 value, u32 countOf);
void write_74HC595(u8* value, u32 countOf);

u64 read_74HC595(void);
void setDispAndHide_74HC595(u64 valueDisp,u64 valueHide,u32 countOf);

#endif
