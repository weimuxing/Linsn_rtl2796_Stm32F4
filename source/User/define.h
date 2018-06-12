#ifndef _DEFINE_H

#define _DEFINE_H

#include "stm32f4xx.h"


//#define DEBUG




#define U32_MAX ((u32)4294967295uL)

typedef u8 BYTE;
typedef u16 WORD;
typedef u8 UCHAR;
typedef BitAction bit;
//typedef U64 u64;
typedef unsigned __int64 u64;

typedef   signed       __int64 int64_t;
typedef unsigned       __int64 uint64_t;

typedef u8 uint8_t;
typedef u8 U8;
typedef u16 uint16_t;
typedef u16 U16;
typedef u32 uint32_t;
typedef u32 U32;
//typedef enum{false = 0,true = 1,TRUE = 1}bool;

#define BIT0 (0x01)
#define BIT1 (0x02)
#define BIT2 (0x04)
#define BIT3 (0x08)
#define BIT4 (0x10)
#define BIT5 (0x20)
#define BIT6 (0x40)
#define BIT7 (0x80)


#define ABS(x) ((x)<0?-(x):(x))

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//此设置用于STM32F207, 不能用于STM32F103
//IO口地址映射

#define GPIOA_ODR_Addr    (GPIOA_BASE+0x14) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x14) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x14) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x14) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x14) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x14) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x14) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+0x10) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x10) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x10) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x10) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x10) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x10) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x10) //0x40011E08 
 
//IO口操作,只对单一的IO口
//确保n的值小于16
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


#define LED0	(PCout(3))



#endif /*end _DEFINE_H */
