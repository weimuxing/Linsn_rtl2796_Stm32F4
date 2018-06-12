#ifndef _PROCESS_KEY_H_
#define _PROCESS_KEY_H_

#include "define.h"
#include "RTD2796.h"






void task_processKey(void);
void setTotaryDelay(int delay);


void setProcessKeyEnable(int value);
int getProcessKeyEnable(void);
void setFrontType(char type);  //  1- new   0- old
char getFrontType(void);


void SetVideoInputSource(EnumDisplayRegion enumRegion,u8 pgmInput,u8 prvInput,u8 input3,u8 input4);


#endif // _PROCESS_KEY_H_

