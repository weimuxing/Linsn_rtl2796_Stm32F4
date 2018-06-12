#ifndef _UART1_H
#define _UART1_H

#define USE_STDIO 1// commented to unuse stdio.h

#ifdef USE_STDIO
#include <stdio.h>
#endif

#include "stm32f4xx.h"
#include "systick.h"
#include "rs232command.h"

extern u8 USART_RX_BUF[200];
extern u16 USART_RX_STA;       //接收状态标记	

typedef enum 
{
  COM1 = 1,
  COM2 = 2,
  COM3 = 3,
  COM4 = 4,
  COM5 = 5,
} COM_TypeDef; 



typedef struct 
{
	u8 uart1RxBuf[sizeof(stControlProtocal)];     //接收缓冲,最大USART_REC_LEN个字节.
	u8 uart2RxBuf[sizeof(stControlProtocal)];     //接收缓冲,最大USART_REC_LEN个字节.
	u16 uart1RxStatus;
	u16 uart2RxStatus;
	bool uart1RxFinishFlag;
	bool uart2RxFinishFlag;
	
}uartRxSt;


extern uartRxSt uartxRxStatus;

typedef u8 BYTE;
typedef u16 WORD;
//typedef u32 DWORD;

/* Exported macro ------------------------------------------------------------*/
#ifndef USE_STDIO
#define putchar(c) SerialPutChar(c)
#endif
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x) Serial_PutString((u8*)(x))
#define DEBUG_PRINT 0
#if DEBUG_PRINT 
#define DebugPutString(x) Serial_PutString((u8*)(x))
#define DebugPrintf printf
#else
#define DebugPutString(x) //Serial_PutString((u8*)(x))
#define DebugPrintf(str, val) //printf((u8*)(str), (u16)(val))
#endif

/* Exported functions ------------------------------------------------------- */
u32 SerialKeyPressed(u8 *key);
u8 GetKey(void);
void SerialPutChar(u8 c);
void Serial_PutString(u8 *s);

#ifndef USE_STDIO
void printf(u8 *pFmt, u16 wVal);
#endif

void Int2Str(u8* str, s32 intnum);
u32 Str2Int(u8 *inputstr, s32 *intnum);
void UARTInit(int port, int baudrate);
void UARTSendStr(int port, char *buffer);
void UARTSendBuffer(int port, char *buffer, int size);

void UartPinConfig(u8 uartport);	//1 20160415

extern bool GetUartDataTranFinishFlag(void);
extern void SetUartDataTranFinishFlag(bool flag);
extern u16 GetUartDataTranFinishLen(void);
extern u16 GetUart2DataTranFinishLen(void);
extern void ClrUartDataTranFinishLen(void);		
extern void ClrUart2DataTranFinishLen(void);



#endif

