#ifndef IDT5P49V5901_H
#define IDT5P49V5901_H
#include "define.h"
#include "I2C.h"
#include "IDT_Timing_Commander.h"

#if 1
#define IDT5P49V5901_1_I2C_ADDRESS 0xD4
#define IDT5P49V5901_2_I2C_ADDRESS 0xD0
#else
#define IDT8T49N241_1_I2C_ADDRESS 0xF8
#define IDT8T49N241_2_I2C_ADDRESS 0xFC

#endif

typedef enum
{
	IDT5P49V5901_1,
	IDT5P49V5901_2,
}IDT5P49V5901_TypeDef;

int Read_IDT5P49V5901(IDT5P49V5901_TypeDef IDTDevice, u16 address);

void Write_IDT5P49V5901(IDT5P49V5901_TypeDef IDTDevice, u16 address, u8 value);
void IDT5P49V5901_Reg_Initial(IDT5P49V5901_TypeDef IDTDevice,u64 Fout0,u64 Fout1,u64 Fout2,u64 Fout3);
void Set5901Freq(IDT5P49V5901_TypeDef IDTDevice,u64 Fout0,u64 Fout1,u64 Fout2,u64 Fout3);
void Set5901FreqIndependent(IDT5P49V5901_TypeDef IDTDevice,u64 Fout,u8 port);
//void IDT_FreqRegSetting(IDT5P49V5901_TypeDef IDTDevice,IDTTimming_Typedef IDTTimming);

#endif

