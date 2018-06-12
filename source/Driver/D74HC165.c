
#include <stdio.h>
#include "D74HC165.h"
#include "UartDebug.h"







#define SET_HC165_CLK() GPIO_SetBits(HC165_CLK_PORT, HC165_CLK_PIN)
#define RESET_HC165_CLK() GPIO_ResetBits(HC165_CLK_PORT, HC165_CLK_PIN)
#define SET_HC165_LOAD() GPIO_SetBits(HC165_LOAD_PORT, HC165_LOAD_PIN)
#define RESET_HC165_LOAD() GPIO_ResetBits(HC165_LOAD_PORT, HC165_LOAD_PIN)


static void delay_74HC165(void)
{
	#if 0
	char i;
	for(i = 0; i < 20; i++)
	{
		;
	}
	#endif
}



void init_74HC165(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* configure CLK pin */
	GPIO_InitStruct.GPIO_Pin = HC165_CLK_PIN;
	GPIO_Init(HC165_CLK_PORT,&GPIO_InitStruct);
	
	/* configure LOAD pin */
	GPIO_InitStruct.GPIO_Pin = HC165_LOAD_PIN;
	GPIO_Init(HC165_LOAD_PORT,&GPIO_InitStruct);

	/* configure SEROUT pin */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = HC165_SEROUT_PIN;
	GPIO_Init(HC165_SEROUT_PORT,&GPIO_InitStruct);
	GPIO_SetBits(HC165_SEROUT_PORT, HC165_SEROUT_PIN);
}




/* 
 * Function: read_74HC165
 * Parameter: countOf -- the count of 74HC165 
 * Return: value of 74HC165 
 */
u8 read_74HC165(int countOf,u8 *data)
{
	u8 result;
    u8 *buf = data;
	int  i,j;
	result = 0;

	SET_HC165_LOAD();
	delay_74HC165();
	RESET_HC165_CLK();
	delay_74HC165();
	RESET_HC165_LOAD();
	delay_74HC165();
	SET_HC165_LOAD();
	delay_74HC165();
	
    for(j = 0;j < countOf;j++)
    {
    	for(i=0; i < (8); i++)
    	{
    		result <<= 1;
    		if(GPIO_ReadInputDataBit(HC165_SEROUT_PORT,HC165_SEROUT_PIN))
    		{
    			result++;
    		}
    		RESET_HC165_CLK();
    		delay_74HC165();
    		SET_HC165_CLK();
    		delay_74HC165();
    	}
        if(buf != NULL)
        {
            *(buf + (countOf - 1 - j)) = ~result;
        }
        else
        {
            return FALSE;
        }
    }
	return TRUE;	
}


#define TEST_74HC165
#ifdef TEST_74HC165

void test_74HC165(void)
{
	static u32 preValue = 0;
    u8 buf[4];
	u32 keyValue;
	keyValue = read_74HC165(4,buf);
    keyValue = (u32)buf[0] << 24 | (u32)buf[1] << 16 | (u32)buf[2] << 8 | (u32)buf[3];
	if(keyValue)
	{
		if(keyValue != preValue)
		{
			DebugMessageD74HC165("Down Key: 0x%x\r\n", keyValue);
			preValue = keyValue;
		}
	}
	else
	{
		if(keyValue != preValue)
		{
			DebugMessageD74HC165("Up Key\r\n");
			preValue = keyValue;
		}
	}
}



#endif




