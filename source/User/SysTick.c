/* Includes ------------------------------------------------------------------*/
#include "SysTick.h"
#include "stm32f4xx.h"
#include "keyScan.h"
#include "D74HC595.h"
#include "menu.h"
#include "processKey.h"

/* Private variables ---------------------------------------------------------*/
static vu32 msSysTick;

/* Private functions ---------------------------------------------------------*/



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IS_SYSTICK_RELOAD(RELOAD) (((RELOAD) > 0) && ((RELOAD) <= 0xFFFFFF))

/* ---------------------- SysTick registers bit mask -------------------- */
/* CTRL TICKINT Mask */
#define CTRL_TICKINT_Set      ((u32)0x00000002)
#define CTRL_TICKINT_Reset    ((u32)0xFFFFFFFD)

/* SysTick counter state */
#define SysTick_Counter_Disable        ((u32)0xFFFFFFFE)
#define SysTick_Counter_Enable         ((u32)0x00000001)
#define SysTick_Counter_Clear          ((u32)0x00000000)

/* SysTick Flag */
#define SysTick_FLAG_COUNT             ((u32)0x00000010)
#define SysTick_FLAG_SKEW              ((u32)0x0000001E)
#define SysTick_FLAG_NOREF             ((u32)0x0000001F)

#define IS_SYSTICK_FLAG(FLAG) (((FLAG) == SysTick_FLAG_COUNT) || \
                               ((FLAG) == SysTick_FLAG_SKEW)  || \
                               ((FLAG) == SysTick_FLAG_NOREF))



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if 0
/*******************************************************************************
* Function Name  : SysTick_CLKSourceConfig
* Description    : Configures the SysTick clock source.
* Input          : - SysTick_CLKSource: specifies the SysTick clock source.
*                    This parameter can be one of the following values:
*                       - SysTick_CLKSource_HCLK_Div8: AHB clock divided by 8
*                         selected as SysTick clock source.
*                       - SysTick_CLKSource_HCLK: AHB clock selected as
*                         SysTick clock source.
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_CLKSourceConfig(u32 SysTick_CLKSource)
{
  /* Check the parameters */
  assert_param(IS_SYSTICK_CLK_SOURCE(SysTick_CLKSource));

  if (SysTick_CLKSource == SysTick_CLKSource_HCLK)
  {
    SysTick->CTRL |= SysTick_CLKSource_HCLK;
  }
  else
  {
    SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;
  }
}
#endif

/*******************************************************************************
* Function Name  : SysTick_SetReload
* Description    : Sets SysTick Reload value.
* Input          : - Reload: SysTick Reload new value.
*                    This parameter must be a number between 1 and 0xFFFFFF.
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_SetReload(u32 Reload)
{
  /* Check the parameters */
  assert_param(IS_SYSTICK_RELOAD(Reload));

  SysTick->LOAD = Reload;
}

/*******************************************************************************
* Function Name  : SysTick_CounterCmd
* Description    : Enables or disables the SysTick counter.
* Input          : - SysTick_Counter: new state of the SysTick counter.
*                    This parameter can be one of the following values:
*                       - SysTick_Counter_Disable: Disable counter
*                       - SysTick_Counter_Enable: Enable counter
*                       - SysTick_Counter_Clear: Clear counter value to 0
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_CounterCmd(u32 SysTick_Counter)
{
  if (SysTick_Counter == SysTick_Counter_Enable)
  {
    SysTick->CTRL |= SysTick_Counter_Enable;
  }
  else if (SysTick_Counter == SysTick_Counter_Disable) 
  {
    SysTick->CTRL &= SysTick_Counter_Disable;
  }
  else /* SysTick_Counter == SysTick_Counter_Clear */
  {
    SysTick->VAL = SysTick_Counter_Clear;
  }    
}

/*******************************************************************************
* Function Name  : SysTick_ITConfig
* Description    : Enables or disables the SysTick Interrupt.
* Input          : - NewState: new state of the SysTick Interrupt.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_ITConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    SysTick->CTRL |= CTRL_TICKINT_Set;
  }
  else
  {
    SysTick->CTRL &= CTRL_TICKINT_Reset;
  }
}

/*******************************************************************************
* Function Name  : SysTick_GetCounter
* Description    : Gets SysTick counter value.
* Input          : None
* Output         : None
* Return         : SysTick current value
*******************************************************************************/
u32 SysTick_GetCounter(void)
{
  return(SysTick->VAL);
}

/*******************************************************************************
* Function Name  : SysTick_GetFlagStatus
* Description    : Checks whether the specified SysTick flag is set or not.
* Input          : - SysTick_FLAG: specifies the flag to check.
*                    This parameter can be one of the following values:
*                       - SysTick_FLAG_COUNT
*                       - SysTick_FLAG_SKEW
*                       - SysTick_FLAG_NOREF
* Output         : None
* Return         : None
*******************************************************************************/
FlagStatus SysTick_GetFlagStatus(u8 SysTick_FLAG)
{
  u32 statusreg = 0, tmp = 0 ;
  FlagStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_SYSTICK_FLAG(SysTick_FLAG));

  /* Get the SysTick register index */
  tmp = SysTick_FLAG >> 3;

  if (tmp == 2) /* The flag to check is in CTRL register */
  {
    statusreg = SysTick->CTRL;
  }
  else          /* The flag to check is in CALIB register */
  {
    statusreg = SysTick->CALIB;
  }

  if ((statusreg & ((u32)1 << SysTick_FLAG)) != (u32)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}




void delay_ms(u32 ms)
{
	u32 start = GetSysTick();
	while(!isSysTickTimeout(start, ms));
}

void SysDelayMs(u32 ms)
{
	u32 start = GetSysTick();
	while(!isSysTickTimeout(start, ms));
}

void SysDelay1ms(u32 ms)
{
	u32 start = GetSysTick();
	while(!isSysTickTimeout(start, ms));
}


/*******************************************************************************
* Function Name  : InitSysTick
* Description    : Initialize Sys Tick.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitSysTick(void)
{
	msSysTick = 0;
	
	/* SysTick end of count event each 1ms with input clock equal to 15MHz (HCLK/8, default) */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_SetReload(15000); // 1ms
	//SysTick_SetReload(1500000);//  100ms

	/* Enable SysTick interrupt */
	SysTick_ITConfig(ENABLE);

	/* Enable the SysTick Counter */
	SysTick_CounterCmd(SysTick_Counter_Enable);

	
}

/*******************************************************************************
* Function Name  : GetSysTick
* Description    : return msSysTick value
* Input          : None
* Output         : None
* Return         : msSystick value  ms
*******************************************************************************/
u32 GetSysTick(void)
{
	u32 temp;
	
	temp = msSysTick;
	
	return temp;
}

/*******************************************************************************
* Function Name  : IsrSysTick
* Description    : call by interrupt every 1 ms
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern int ledHorseIndex;
extern int ledHorseCnt;
extern u8 ledHorseFalg;
//extern int ChangeExtInputFormatFlag;
extern int enableMBSourceCnt;
#if 0
extern int logoDispCnt;
extern int deviceDispCnt;
#endif



#if defined(LING_XIN)
#define ledHorseMAX  15 //按键数
#else
#define ledHorseMAX  26 //按键数
#endif
extern void LED_Disp(u64 value);

//LOAD -> SW1
//DVI2 -> SW2
//DVI1 -> SW3
//LAYD -> SW4
//LAYH -> SW5
//DVI3 -> SW6
//DVI4 -> SW7
//TAKE -> SW8
//LAYC -> SW9
//LAYB -> SW10
//LAYA -> SW11
//VGA  -> SW12
//REVS -> SW13
//LAYE -> SW14
//LAYF -> SW15
//LAYG -> SW16
//DP  -> SW17
//HDMI2 -> SW18
//HDMI1 -> SW19
//DVI -> SW20
//M1 -> SW21
//M2 -> SW22
//M3 -> SW23
//M4 -> SW24
//REVS -> SW25
//SCALE -> SW26
//MENU -> SW27
//SAVE -> SW28
//SPLIT -> SW29
//REVS -> SW30
//SELECT -> SW31
//REVS -> SW32


//2-0 dp
//2-1 vga
//2-2 m3
//2-3 m4

//2-4 take
//2-5 load
//2-6 save
//2-7 sel

//1-0 sclse
//1-1 split
//1-2 menu
//1-3 m2

//1-4 m1
//1-5 dvi
//1-6 hdmi2
//1-7 hdmi1

#define LED_MENU	(1 << (0*8 + 3-1))//
#define LED_SCALE	(1 << (0*8 + 1-1))
#define LED_DVI		(1 << (0*8 + 6-1))
#define LED_HDMI1	(1 << (0*8 + 8-1))
#define LED_HDMI2	(1 << (0*8 + 7-1))
#define LED_SPLIT	(1 << (0*8 + 2-1))
#define LED_M1		(1 << (0*8 + 5-1))
#define LED_M2		(1 << (0*8 + 4-1))


#define LED_DP		(1 << (1*8 + 1-1))
#define LED_VGA		(1 << (1*8 + 2-1))
#define LED_M3		(1 << (1*8 + 3-1))
#define LED_M4		(1 << (1*8 + 4-1))
#define LED_TAKE	(1 << (1*8 + 5-1))
#define LED_LOAD	(1 << (1*8 + 6-1))
#define LED_SAVE	(1 << (1*8 + 7-1))



//#define LED_MENU	(1 << (3*8 + 3))
//#define LED_MENU	(1 << (3*8 + 3))
//#define LED_MENU	(1 << (3*8 + 3))
//#define LED_MENU	(1 << (3*8 + 3))
//#define LED_MENU	(1 << (3*8 + 3))



#if defined(LING_XIN)
u16 ledHorseValue[ledHorseMAX] = 
{
	LED_MENU,	
	LED_SPLIT,	
	LED_HDMI1,	
	LED_HDMI2,	
	LED_DVI,	
	LED_M1,	
	LED_M2,		
	LED_LOAD,	
	LED_TAKE,		
	LED_M4,	
	LED_M3,
	LED_VGA,
	LED_DP,
	LED_SCALE,
	LED_SAVE,
	
};
#else
u64 ledHorseValue[ledHorseMAX] = 
{
	0x04000000,0x02000000,0x00080000,0x00040000,0x00020000,0x00010000,0x00000400,0x00000200,0x00000100,
	0x00000008,0x000000004,0x000000002,0x00000001,0x00000080,0x00000040,0x00000020,0x00000010,
	0x00008000,0x00004000,0x00002000,0x00800000,0x00400000,0x00200000,0x00100000,0x10000000,0x08000000
};
#endif

void LED_ON_Index(char index)
{
	u16 value = ledHorseValue[index];

	//printf("%s,index=%d\r\n",__func__,index);
	//LED_ON(value);
	LED_Disp(value);
}
void IsrSysTick(void)  // call by interrupt every 1 ms
{
	++msSysTick;
	#if 1
    if(ledHorseFalg != 0)
	{
        if(ledHorseFalg == 2)
		{
		    
			//menuDisableBusy();			
			//menuSetFinish(1,11);
            ledHorseFalg = 0;
			return;
		}
		if(msSysTick%100 == 0)
		{
			ledHorseIndex++;
			if(ledHorseIndex >= ledHorseMAX)
			{
				ledHorseIndex = 0;
			}
			LED_ON_Index(ledHorseIndex);
		}
	}
	#else
	#endif
	if(enableMBSourceCnt > 0)
	{
		enableMBSourceCnt--;
		if(enableMBSourceCnt == 0)
		{
			//printf("KEY_MB_SOURCE_V\r\n");
			setVirKey(KEY_MB_SOURCE_V);
			return;
		}
	}
}



/*******************************************************************************
* Function Name  : isSysTickTimeout
* Description    : is time out
* Input          : startTiming--start sys tick count
			 delayTimming--delay time   (ms)
* Output         : None
* Return         : TRUE---time out
*******************************************************************************/
bool isSysTickTimeout(vu32 startTiming, vu32 delayTimming)
{
	u32 endTiming;
	endTiming = GetSysTick();
	if(endTiming >= startTiming)
	{
		if( (endTiming-startTiming) <delayTimming)
		{
			return FALSE;
		}		
	}
	else
	{
		if((endTiming+(U32_MAX-startTiming)) < delayTimming)
		{
			return FALSE;
		}
	}

	return TRUE;  // time out
}





