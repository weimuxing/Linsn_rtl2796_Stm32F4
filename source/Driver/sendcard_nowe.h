#ifndef _SENDCARD_NOWE_H_
#define _SENDCARD_NOWE_H_


#include "stm32f2xx.h"

void init_sendcard(void);
int getSendCardPort(void);
BYTE SendcardCheckSum(BYTE *buffer, int size);
bool readsendcard(void);
bool NW_Brightness_Command_Write(u8 Brightness);
bool NW_Brightness_Command_Save(void);

u8 getIscheckCardtype(void);
void setCardTypeOk(u8 value);

#endif		// #ifndef _SENDCARD_LXY_H_


