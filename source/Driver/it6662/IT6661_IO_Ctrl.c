#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "uart.h"

#include "SysConfig.h"

#include "I2C.h"
#include "IT6661_IO_Ctrl.h"

#define _BEBUG
#if defined(DEBUG_PRINT_ENABLE)&&defined(_BEBUG)
//#define IT6661_DBG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"/n", __LINE__, ##__VA_ARGS__)
#define IT6661_DBG(format,...) printf("[IT6661]"format,  ##__VA_ARGS__)
#else
#define IT6661_DBG(format,...)	do{}while(0)
#endif

void IT6661_Reset(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	IT6661_DBG("%s\r\n",__func__);
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* Configure  as GPIO out  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOE,GPIO_InitStructure.GPIO_Pin);
	SysDelay1ms(200);
	GPIO_SetBits(GPIOE,GPIO_InitStructure.GPIO_Pin);
	SysDelay1ms(10);
}

void IT6661_IoCtrl(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	IT6661_DBG("%s\r\n",__func__);
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Configure  as GPIO out  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_InitStructure.GPIO_Pin);
	SysDelay1ms(10);	
}



