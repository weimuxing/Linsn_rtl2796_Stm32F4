#include "D74HC595.h"
#include "userPref.h"

#define SET_HC595_SH_CLK() GPIO_SetBits(HC595_SH_CLK_PORT,HC595_SH_CLK_PIN)
#define RESET_HC595_SH_CLK() GPIO_ResetBits(HC595_SH_CLK_PORT,HC595_SH_CLK_PIN)
#define SET_HC595_ST_CLK() GPIO_SetBits(HC595_ST_CLK_PORT,HC595_ST_CLK_PIN)
#define RESET_HC595_ST_CLK() GPIO_ResetBits(HC595_ST_CLK_PORT,HC595_ST_CLK_PIN)
#define SET_HC595_SERIN() GPIO_SetBits(HC595_SERIN_PORT,HC595_SERIN_PIN)
#define RESET_HC595_SERIN() GPIO_ResetBits(HC595_SERIN_PORT,HC595_SERIN_PIN)

static u64 hc595_value = 0xFFFFFFFFFFFFFFFF;


static void delay_74HC595(void)
{
	#if 0
	char i;
	for(i = 0; i < 20; i++)
	{
		;
	}
	#endif
}

void init_74HC595(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* configure SH_CLK pin */
	GPIO_InitStruct.GPIO_Pin = HC595_SH_CLK_PIN;
	GPIO_Init(HC595_SH_CLK_PORT,&GPIO_InitStruct);
	
	/* configure ST_CLK pin */
	GPIO_InitStruct.GPIO_Pin = HC595_ST_CLK_PIN;
	GPIO_Init(HC595_ST_CLK_PORT,&GPIO_InitStruct);

	/* configure SERIN pin */
	GPIO_InitStruct.GPIO_Pin = HC595_SERIN_PIN;
	GPIO_Init(HC595_SERIN_PORT,&GPIO_InitStruct);

	//write_74HC595(hc595_value,4);
}

/*
 * Function: write_74HC595
 * Parameter: value -- 要写入的值, 
 *			countOf -- 串联的HC595 的个
 * Return: none
 * 从高位先移动
 */
void write_74HC595(u8* value, u32 countOf)
{
	int i;
	u8 dd;
	//u64 firstBit = (u64)0x01<<(countOf*8-1);
//	hc595_value = value;
	RESET_HC595_ST_CLK();
	delay_74HC595();
	RESET_HC595_SH_CLK();
	delay_74HC595();
	for(int j = 0;j < countOf;j++)
	{
		dd = *(value + 3 - j);
		for(i=0; i < 8; i++)
		{
			if(dd & 0x80)
			{
				SET_HC595_SERIN();
			}
			else
			{
				RESET_HC595_SERIN();
			}
			dd <<= 1;
			delay_74HC595();
			SET_HC595_SH_CLK();
			delay_74HC595();
			RESET_HC595_SH_CLK();
			delay_74HC595();
		}
	}
	SET_HC595_ST_CLK();
	delay_74HC595();
	RESET_HC595_ST_CLK();
	delay_74HC595();
	RESET_HC595_SH_CLK();
	delay_74HC595();
	RESET_HC595_SERIN();
	delay_74HC595();
}




void ResetValue_74HC595(u64 value, u32 countOf)
{
	u8 buf[4];
	hc595_value = (~ value);
	buf[0] = (u8)(hc595_value & 0xff);
	buf[1] = (u8)((hc595_value >> 8) & 0xff);
	buf[2] = (u8)((hc595_value >> 16) & 0xff);
	buf[3] = (u8)((hc595_value >> 24) & 0xff);
	write_74HC595(buf, countOf);
}




#if 0
void write_74HC595_invert(u64 value, u32 countOf)
{
	value = ~ value;
	write_74HC595(value, countOf);
	
}

void writeBits_74HC595(u64 value, u64 mask, u32 countOf)
{
	hc595_value &= (~mask);
	hc595_value |= value;
	write_74HC595(hc595_value, countOf);
}



void SetBits_74HC595(u64 value, u32 countOf)
{
	#if 1
	hc595_value |= value;
	write_74HC595(hc595_value, countOf);
	#else
	BYTE i;
	u64 num = 0x03;
	#if 0
	u32 hc595_h,hc595_l,v_h,v_l;
	hc595_h = hc595_value>>32;
	hc595_l = hc595_value;
	v_h = value>>32;
	v_l = value;
	printf("before set,hc595_value:0x%x%16x,value:0x%x%16x\r\n",hc595_h,hc595_l,v_h,v_l);
	#endif
	for(i = 0; i < 64; i++)
	{
		if((value&num) != 0)
		{
			hc595_value &= (~num);
			hc595_value |= num;
		}
		i++;
		num <<= 2;
	}
	#if 0
	hc595_h = hc595_value>>32;
	hc595_l = hc595_value;
	v_h = value>>32;
	v_l = value;
	printf("after set,hc595_value:0x%x%16x,value:0x%x%16x\r\n",hc595_h,hc595_l,v_h,v_l);
	#endif
	write_74HC595(hc595_value, countOf);
	#endif
}
void ResetBits_74HC595(u64 value, u32 countOf)
{
	#if 1
	//u32 h = 0,l = 0;
	//h = hc595_value>>32;
	//l = hc595_value;
	//printf("\r\nhc595_value_before = %x%x,",h,l);
	hc595_value &= (~value);
	//h = hc595_value>>32;
	//l = hc595_value;
	//printf("\r\nhc595_value_do& = %x%x,",h,l);
	write_74HC595(hc595_value, countOf);
	#else
	BYTE i;
	u64 num = 0x03;
	#if 0
	u32 hc595_h,hc595_l,v_h,v_l;
	hc595_h = hc595_value>>32;
	hc595_l = hc595_value;
	v_h = value>>32;
	v_l = value;
	printf("before reset,hc595_value:0x%08x%08x,value:0x%08x%08x\r\n",hc595_h,hc595_l,v_h,v_l);
	#endif
	for(i = 0; i < 64; i++)
	{
		if((value&num) != 0)
		{
			hc595_value &= (~num);
			hc595_value |= (~(value&num))&num;//&= ~(value&num);
		}
		i++;
		num <<= 2;
	}
	#if 0
	hc595_h = hc595_value>>32;
	hc595_l = hc595_value;
	v_h = value>>32;
	v_l = value;
	printf("after reset,hc595_value:0x%08x%08x,value:0x%08x%08x\r\n",hc595_h,hc595_l,v_h,v_l);
	#endif
	write_74HC595(hc595_value, countOf);
	#endif
}

void setDispAndHide_74HC595(u64 valueDisp,u64 valueHide,u32 countOf)
{
	#if 0
	u32 h = 0,l = 0;
	h = hc595_value>>32;
	l = hc595_value;
	printf("\r\nhc595_value_before = %x%x,",h,l);
	h = valueDisp>>32;
	l = valueDisp;
	printf("valueDisp = %x%x,",h,l);
	#endif
	hc595_value |= valueHide; //先做消影
	#if 0
	h = hc595_value>>32;
	l = hc595_value;
	printf("hc595_value = %x%x\r\n",h,l);
	#endif
	hc595_value &= ~valueDisp;
	#if 0
	h = hc595_value>>32;
	l = hc595_value;
	printf("\r\nhc595_value_do& = %x%x,",h,l);
	#endif
	write_74HC595(hc595_value, countOf);
}

u64 read_74HC595(void)
{
	return hc595_value;
}

#endif

