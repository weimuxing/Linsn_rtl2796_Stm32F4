#ifndef _SENDCARD_COLORLIGHT_H_
#define _SENDCARD_COLORLIGHT_H_


#include "stm32f4xx.h"

void init_sendcard(void);
int getSendCardPort(void);
BYTE SendcardCheckSum(BYTE *buffer, int size);
bool erase_flash(u32 adress);
bool Sendcard_Brightness_Command(BYTE value);
bool Sendcard_jizai_Command(u32 adress);
bool Sendcard_position_Command(u32 adress, u16 x,u16 y, u16 xsize, u16 ysize);
bool Sendcard_position_Command1(u32 adress, u16 x,u16 y, u16 xsize, u16 ysize);
bool readsendcard(void);


bool Sendcard_Command_jiazai(void);
bool Sendcard_gamma_Command(u8 value);

bool Sendcard_area_Command(u16 cardnumber,u16 x, u16 y, u16 endx, u16 endy);

bool Sendcard_Brightness_Command_temporary(BYTE value);


u8 getIscheckCardtype(void);
void setCardTypeOk(u8 value);

bool SendCard_Colorlight_Write_Edid(int width,int height);

bool Sendcard_EraseFlash_Gamma(void);

bool sendcard_Set_Gamma(u8 gammaval);


#endif		// #ifndef _SENDCARD_COLORLIGHT_H_


