#ifndef _SYSTICK_H
#define _SYSTICK_H

#include "stm32f4xx.h"

void InitSysTick(void);
u32 GetSysTick(void);
void IsrSysTick(void);  // call by interrupt every 1 ms
bool isSysTickTimeout(u32 startTiming, u32 delayTimming);
void SysDelayMs(u32 ms);
void SysDelay1ms(u32 ms);
void delay_ms(u32 ms);
#endif

