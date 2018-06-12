#ifndef _SENDCARD_LXY_H_
#define _SENDCARD_LXY_H_


#include "stm32f4xx.h"

void init_sendcard(void);
int getSendCardPort(void);
u8 SendcardCheckSum(u8 *buffer, int size);

bool readsendcard(void);

bool sendcardlxy_brightness_command(u8 value);
bool sendcardlxy_rgb_command(u8 r_value,u8 g_value,u8 b_value);		//1ÁÁ¶Èµ÷½ÚÃüÁî
bool readsendcardlxy(void);
bool readsendcardlxy_check(void);
bool sendcardlxy_write_en_command(u8 cardnumber);
bool SendCardLxyWriteUnknowCmd(u8 cardnumber,u8 cn_type);
bool sendcardlxy_write_command(u8 cardnumber, u16 x, u16 y, u16 width, u16 height);
bool sendcardlxy_write_un_command(u8 cardnumber);
bool sendcardlxy_save_en_command(u8 cardnumber);
bool sendcardlxy_save_clear_command(u8 cardnumber);
bool sendcardlxy_save_command(u8 cardnumber,u16 x, u16 y, u16 width, u16 height);
bool sendcardlxy_save_un_command(u8 cardnumber);
bool sendcardlxy_write_en_commandNew(u8 cmdid);
bool sendcardlxy_write_un_commandNew(u8 cmdid);
bool sendcardlxy_write_command_ColorTem(u32 rcvcardaddr,u8 cmdid);


bool sendcardlxy_xy_offsetcommand(void);
bool sendcardlxy_write_edid(u8 outputformat);
#if 1 //@lv 20150925
bool sendcardlxy_save_edid(u8 outputformat);
#endif


bool Sendcard_area_Command(u16 cardnumber,u16 x, u16 y, u16 endx, u16 endy);

bool sendcardlxy_xy_command(u16 x,u16 y,u16 ux,u16 uy,u16 dx,u16 dy);
bool sendcardlxy_setWidth_command(u16 x,u16 y,u16 w,u16 ux,u16 uy,u16 dx,u16 dy);
bool sendcardlxy_changecard_command(u8 cardnumber);
bool sendcardlxy_command_save(u8 adress);
bool sendcardlxy_command_xy_save(u8 adress,u16 x,u16 y);


bool sendcardlxy_write_en_command_pre(u8 cardnumber);

bool sendcardlxy_write_command_pre(u8 cardnumber, u16 x, u16 y, u16 width, u16 height);

bool sendcardlxy_write_un_command_pre(u8 cardnumber);


#if 1 //@lv 20150924
bool SendCardFactoryReset_Func(void);
#endif

#endif		// #ifndef _SENDCARD_H_


