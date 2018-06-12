#ifndef _LCD_H_
#define _LCD_H_

#include "define.h"

void LCD_Init(void);
void LCD_WriteStr(u32 column, u32 row, const char *str);
void LCD_WriteStr_fb(u32 row, u32 column, const char *str);
void LCD_WriteStr2(u32 row, u32 column, const char *str);
void LCD_WriteStr_fb2(u32 row, u32 column, const char *str);

void LCD_WriteLine(int line, const char *str);
void LCD_WriteLine_fb(int line, const char *str);
void LCD_Clear(void);
void LCD_Unicode2Ascii(const char *uniStr, char *ascStr, int sizeByte);
u16 uni_to_gb2312(u16 unicode);
void LCD_ClearRect(int startX,int endX,int startY,int endY);
void LCD_DrawFillRect(int startX,int endX,int startY,int endY);
void LCD_ClearItemBar(void);

#define LCD_BLACK_TIME 100

void LCD_WriteLine1(const char *str);
void LCD_WriteLine2(const char *str);
void LCD_WriteLine3(const char *str);
void LCD_WriteLine4(const char *str);
void LCD_WriteLine1_fb(const char *str);
void LCD_WriteLine2_fb(const char *str);
void LCD_WriteLine3_fb(const char *str);
void LCD_WriteLine4_fb(const char *str);
void LCD_DrawItemBar(int totalItem, int curItem);

void LCD_WriteLine1_L(const char *str);
void LCD_WriteLine2_L(const char *str);
void LCD_WriteLine3_L(const char *str);
void LCD_WriteLine4_L(const char *str);
void dispLineCenter_L(char line,const char* dispData);

#endif // _LCD_H_

