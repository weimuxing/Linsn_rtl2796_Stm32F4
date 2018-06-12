

#include "UART.h"
#include "rs232command.h"
//#include "ucos_ii.h"

#ifdef USE_STDIO
#ifdef __GNUC__ 
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf 
     set to 'Yes') calls __io_putchar() */ 
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch) 
#else 
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f) 
#endif /* __GNUC__ */ 

BYTE  UART1Buffer_Receive[512];
//BYTE m_edidRaw[128];
unsigned int ReceiveCount;

extern u8 getCommHardVersion(void);

#define DEBUG_UART_PORT	USART1


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[200];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

/******************************************************************************* 
* Function Name  : PUTCHAR_PROTOTYPE 
* Description    : Retargets the C library printf function to the USART. 
* Input          : None 
* Output         : None 
* Return         : None 
*******************************************************************************/ 
PUTCHAR_PROTOTYPE 
{ 
  /* Write a character to the USART */ 
  USART_SendData(DEBUG_UART_PORT, (u8) ch); 

  /* Loop until the end of transmission */ 
  while(USART_GetFlagStatus(DEBUG_UART_PORT, USART_FLAG_TXE) == RESET) 
  { 
  } 

  return ch; 
} 

#endif // USE_STDIO


/*******************************************************************************
* Function Name  : Str2Int
* Description    : Convert a string to an integer
* Input 1        : - inputstr: The string to be converted
*                  - intnum: The intger value
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u32 Str2Int(u8 *inputstr, s32 *intnum)
{
  u32 i = 0, res = 0;
  u32 val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}

/*******************************************************************************
* Function Name  : Int2Str
* Description    : Convert an Integer to a string
* Input          : - str: The string
*                  - intnum: The intger to be converted
* Output         : None
* Return         : None
*******************************************************************************/
void Int2Str(u8* str, s32 intnum)
{
  u32 i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

/*******************************************************************************
* Function Name  : SerialKeyPressed
* Description    : Test to see if a key has been pressed on the HyperTerminal
* Input          : - key: The key pressed
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u32 SerialKeyPressed(u8 *key)
{

  if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
  {
    *key = (u8)USART1->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}

/*******************************************************************************
* Function Name  : GetKey
* Description    : Get a key from the HyperTerminal
* Input          : None
* Output         : None
* Return         : The Key Pressed
*******************************************************************************/
u8 GetKey(void)
{
  u8 key = 0;

  /* Waiting for user input */
  while (1)
  {
    if (SerialKeyPressed((u8*)&key)) break;
  }
  return key;

}

/*******************************************************************************
* Function Name  : SerialPutChar
* Description    : Print a character on the HyperTerminal
* Input          : - c: The character to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutChar(u8 c)
{
  USART_SendData(DEBUG_UART_PORT, c);
  while (USART_GetFlagStatus(DEBUG_UART_PORT, USART_FLAG_TXE) == RESET);
}

/*******************************************************************************
* Function Name  : Serial_PutString
* Description    : Print a string on the HyperTerminal
* Input          : - s: The string to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void Serial_PutString(u8 *s)
{
  while (*s != '\0')
  {
    SerialPutChar(*s);
    s ++;
  }
}

#ifndef USE_STDIO
void printf(BYTE  *pFmt, WORD wVal)
{
    BYTE ucBff, ucDisp;
    bool bNotZero=FALSE, bHex=FALSE;
    WORD wDivider=10000;

    while( (ucBff = *(pFmt++)) != '\0' )
    {
    #if ENABLE_CEC
        msHDMICecChkBuf();
    #endif
        if(ucBff == '%') // check special case
        {
            switch( *(pFmt++) ) // check next character
            {
                case 'x': // hexadecimal number
                case 'X':
                    wDivider = 0x1000;
                    bHex = TRUE;
                case 'd': // decimal number
                case 'i':
                    if(wVal)
                    {
                        while(wDivider)
                      	{
                      	    ucDisp = wVal/wDivider;
                            wVal = wVal%wDivider;

                            if(ucDisp)
                          	    bNotZero=TRUE;

                            if(bNotZero)
                          	{
                          	    if (ucDisp>9)
                                    putchar(ucDisp-0x0a+'A');
                                else
                                    putchar(ucDisp+'0');
                            }

                            if(bHex)
                          	    wDivider /= 0x10;
                            else
                          	    wDivider /= 10;
                        }
                    }
                    else
      	                putchar('0');

                    break;
            } // switch
        }
        else // general
            putchar(ucBff); // put a character
    } // while
}

#endif // USE_STDIO





//1#######################################################################################################
//1将UART的引脚配置为输入
void UartPinConfig(u8 uartport)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	if(uartport == 1)
	{
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);


		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		USART_Cmd(USART1, DISABLE);	
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	}
	else if(uartport == 2)
	{
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);


		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		USART_Cmd(USART2, DISABLE);	
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}
	else if(uartport == 3)
	{
		//
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);


		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		USART_Cmd(USART3, DISABLE);	
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	}
	else if(uartport == 4)
	{
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);


		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOC, &GPIO_InitStructure);        

		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		
		GPIO_Init(GPIOC, &GPIO_InitStructure);
        
		USART_Cmd(UART4, DISABLE);	
		USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
	}
	else if(uartport == 5)
	{
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);

		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		USART_Cmd(UART5, DISABLE);	
		USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);
	}
	else if(uartport == 6)
	{
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);

#if 1
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;// | GPIO_Pin_7;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif

		USART_Cmd(USART6, DISABLE);	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, DISABLE);
		USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
	}
	
}

void USART_Config(USART_TypeDef* usart, int baudRate);
////////////////////////////////////////////////////////////


void UARTInit(int port, int baudRate) // port = 1, 2, 3, 4, 5
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	if(port == 1)
	{
		 /* Enable GPIO clock */
  		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
		
		 /* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Enable UART clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_Config(USART1, baudRate);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		
	}
	else if(port == 2)
	{
		/* Enable GPIO clock */
  		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		
		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);

		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* Enable UART clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//8;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
				
		USART_Config(USART2, baudRate);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		
	}
	else if(port == 3)
	{
		/* Enable GPIO clock */
  		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
		
		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);

		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Enable UART clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//8;
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		USART_Config(USART3, baudRate);

		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		
	}
	else if(port == 4)
	{
	#if 1
		/* Enable GPIO clock */
  		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
		
		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);

		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* Enable UART clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		USART_Config(UART4, baudRate);

		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	#endif
	}
	else if(port == 5)
	{
		/* Enable GPIO clock */
  		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
		
		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* Enable UART clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//8;
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
				
		USART_Config(UART5, baudRate);

		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
		
	}
	else if(port == 6)
	{
		/* Enable GPIO clock */
  		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
		
		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);

		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

		/* Configure USART Tx as alternate function  */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* Configure USART Rx as alternate function  */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* Enable UART clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

		/* Enable the USART6 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//8;
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
				
		USART_Config(USART6, baudRate);

		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
		
	}
}

void UARTSendStr(int port, char *buffer)
{
	USART_TypeDef * usart;
	switch(port)
	{
	case 1:
		usart = USART1;
		break;
	case 2:
		usart = USART2;
		break;
	case 3:
		usart = USART3;
		break;
	case 4:
		usart = UART4;
		break;
	case 5:
		usart = UART5;
		break;
	case 6:
		usart = USART6;
		break;
	default:
		return;
	}
	while(*buffer != '\0')
	{		
		USART_SendData(usart, *buffer);
		++buffer;			
	}
}

void UARTSendBuffer(int port, char *buffer, int size)
{
 	u32 retry;   
	int i;
	USART_TypeDef * usart;
	switch(port)
	{
	case 1:
		usart = USART1;
		break;
	case 2:
		usart = USART2;
		break;
	case 3:
		usart = USART3;
		break;
	case 4:
		usart = UART4;
		break;
	case 5:
		usart = UART5;
		break;
	case 6:
		usart = USART6;
		break;
		
	default:
		return;
	}
	for(i=0; i<size; i++)
	{		
		USART_SendData(usart, *buffer);
        retry = 0xFFFFFF;
		while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET)
        {
            retry--;
            if(retry == 0)
                break;
        };
		++buffer;			
	}
}


void USART_Config(USART_TypeDef* usart, int baudRate)
{
	USART_InitTypeDef USART_InitStructure;

	USART_DeInit(usart);

	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    USART_InitStructure.USART_StopBits = USART_StopBits_1;  
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USART */
	USART_Init(usart, &USART_InitStructure);	

	USART_Cmd(usart, ENABLE);	
}





#define DEUBG_UART						USART1

uartRxSt uartxRxStatus;

//##############################################################################################################
//
bool UartTranFinishFlag = false;

bool GetUartDataTranFinishFlag(void)
{

	return uartxRxStatus.uart1RxFinishFlag;
}


//##############################################################################################################
//
void SetUartDataTranFinishFlag(bool flag)
{

	uartxRxStatus.uart1RxFinishFlag= flag;
}



//##############################################################################################################
//
u16 GetUartDataTranFinishLen(void)
{

	return (uartxRxStatus.uart1RxStatus & 0X3FFF);
}

//##############################################################################################################
//
u16 GetUart2DataTranFinishLen(void)
{

	return (uartxRxStatus.uart2RxStatus & 0X3FFF);
}



//##############################################################################################################
//
void ClrUartDataTranFinishLen(void)
{
	uartxRxStatus.uart1RxStatus = 0;
}


//##############################################################################################################
//
void ClrUart2DataTranFinishLen(void)
{
	uartxRxStatus.uart2RxStatus = 0;
}



#if 0
//##############################################################################################################
//
void USART1_IRQHandler(void)                	
{
	u8 Res;
	if(USART_GetITStatus(DEUBG_UART, USART_IT_RXNE) != RESET)  
	{
		Res =USART_ReceiveData(DEUBG_UART);
		//DebugUartSendData(Res);
		//USART_SendData(USART1,Res);
		//USART_SendData(USART2,Res);
		
		if(GetUartDataTranFinishFlag() != true)
		{
			#if 0
			if(uartxRxStatus.uart1RxStatus & 0x4000)
			{
				if(Res != 0x0a)
					uartxRxStatus.uart1RxStatus=0;
				else 
				{
					SetUartDataTranFinishFlag(true);
					//USART_RX_STA|=0x8000;	//接收完成了 
				}
			}
			#else
			if(Res == 0x4e)
			{
				uartxRxStatus.uart1RxBuf[uartxRxStatus.uart1RxStatus & 0X3FFF] = Res;
				SetUartDataTranFinishFlag(true);
			}
			#endif
			else //还没收到0X0D
			{	
				//if(Res == 0x0d)
				//	uartxRxStatus.uart1RxStatus |= 0x4000;
				//else
				//{
					uartxRxStatus.uart1RxBuf[uartxRxStatus.uart1RxStatus & 0X3FFF] = Res;
					uartxRxStatus.uart1RxStatus++;
					if(uartxRxStatus.uart1RxStatus >= (sizeof(st_ControlProtocal)))
						uartxRxStatus.uart1RxStatus = 0;//接收数据错误,重新开始接收	  
				//}		 
			}
		}   		 
  } 
} 

#else

void GetDebugUartCmd(u8 value)
{
	if((USART_RX_STA&0x8000)==0)//接收未完成
	{
		if(USART_RX_STA&0x4000)//接收到了0x0d
		{
			if(value != 0x0a)USART_RX_STA=0;//接收错误,重新开始
			else USART_RX_STA|=0x8000;	//接收完成了 
		}
		else //还没收到0X0D
		{	
			if(value==0x0d)USART_RX_STA|=0x4000;
			else
			{
				USART_RX_BUF[USART_RX_STA&0X3FFF]=value ;
				USART_RX_STA++;
				if(USART_RX_STA>(200-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
			}		 
		}
	}  
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		 		 
  } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	




