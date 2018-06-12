#ifndef _D74HC165_H_
#define _D74HC165_H_

#include "define.h"

//PD0	KP1_ESC
//PE13	KP2_MENU
//PD1	KP3_SOURCE1


#define HC165_PIN0              0x01
#define HC165_PIN1              0x02
#define HC165_PIN2              0x04
#define HC165_PIN3              0x08
#define HC165_PIN4              0x10
#define HC165_PIN5              0x20
#define HC165_PIN6              0x40
#define HC165_PIN7              0x80


typedef enum
{
    D74HC165_CHIP_NONE,
    D74HC165_CHIP_IDX_1,
    D74HC165_CHIP_IDX_2,
    D74HC165_CHIP_IDX_3,
    D74HC165_CHIP_IDX_4,     

    D74HC165_CHIP_IDX_MAX,
}D74HC165_GroupDef;

#define HC165_SEROUT_PIN 	GPIO_Pin_4
#define HC165_SEROUT_PORT 	GPIOE
#define HC165_CLK_PIN		GPIO_Pin_7
#define HC165_CLK_PORT 		GPIOD
#define HC165_LOAD_PIN		GPIO_Pin_5
#define HC165_LOAD_PORT		GPIOE


void init_74HC165(void);
u8 read_74HC165(int countOf,u8 *data);

void test_74HC165(void);

#endif
