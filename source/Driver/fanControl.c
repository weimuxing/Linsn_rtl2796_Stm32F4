

#include "SysConfig.h"

#include "stm32f4xx_gpio.h"
#include "userPref.h"
#include "fanControl.h"

u8 fan_gpio_light;
void fan_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /*GPIOF_Pin_4, TIM4_Channel4 PWM*/  

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOF, GPIO_Pin_4);
   // GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_TIM4);//GPIOB_Pin9 TIM4_Ch4N,
}

void fan_gpio_set(void)
{
    //printf("fan_gpio_set\r\n");
    if(fan_gpio_light == TRUE)
    {
        fan_gpio_light = FALSE;
        GPIO_ResetBits(GPIOF, GPIO_Pin_4);
    }
    else
    {
        fan_gpio_light = TRUE;
         GPIO_SetBits(GPIOF, GPIO_Pin_4);
    }
}

void Time4_Config(void)  
{
#if 0
    /* ----------------------------------------------------------------------- 
        TIM4 Configuration: generate 4 PWM signals with 4 different duty cycles. 

        In this example TIM4 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
        since APB1 prescaler is different from 1. 
        TIM3CLK = 2 * PCLK1 
        PCLK1 = HCLK / 4 
        => TIM4CLK = HCLK / 2 = SystemCoreClock /2 
        To get TIM4 counter clock at 20 MHz, the prescaler is computed as follows: 
        Prescaler = (TIM4CLK / TIM4 counter clock) - 1 
        Prescaler = ((SystemCoreClock /2) /20 MHz) - 1 
        To get TIM4 output clock at 30 KHz, the period (ARR)) is computed as follows: 
        ARR = (TIM4 counter clock / TIM4 output clock) - 1= 665 
        TIM4 Channel1 duty cycle = (TIM4_CCR1[TIM_Pulse]/ TIM4_ARR[TIM_Period])* 100 = 50% 
        ----------------------------------------------------------------------- */  
        
    u32 PrescalerValue  = 0;  
     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
     TIM_OCInitTypeDef TIM_OCInitStructure;  
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//时能定时器时钟4  
     TIM_DeInit(TIM4); 
     
     /* Compute the prescaler value */  
     PrescalerValue = (uint16_t) ((120000000/2) / 20000000) - 1;  
      
     /* Time base configuration */  
     TIM_TimeBaseStructure.TIM_Period = 665;//ARR 计数初值设置PWM频率计数初值  
     TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;//PSC 设置用来作为TIMx时钟频率除数的预分频值  
     TIM_TimeBaseStructure.TIM_ClockDivision = 0;  
     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
     TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  
     /* PWM1 Mode configuration: Channel4 */  
     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  
     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
     TIM_OCInitStructure.TIM_Pulse = 444;//333;//设置占空比  
     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//设置有效极性，也就是设置比较输出的有效电平。  
     TIM_OC4Init(TIM4, &TIM_OCInitStructure);  
     TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable); 

  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;      //这个对高级定时器很重要
TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;//这个对高级定时器很重要
TIM_CtrlPWMOutputs(TIM4, ENABLE);                                             //这个对高级定时器很重要   
      
     TIM_ARRPreloadConfig(TIM4, ENABLE);  
     /* TIM4 enable counter */  
     TIM_Cmd(TIM4, ENABLE);  
    /* 
     TIM_ClearFlag(TIM4, TIM_FLAG_Update); 
     TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); 
     TIM_Cmd(TIM4, ENABLE); 
     TIM_CtrlPWMOutputs(TIM4, ENABLE); 
     */
#else
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //TIM_OCInitTypeDef  TIM_OCInitStructure ;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);    

	TIM_DeInit(TIM4);   

	//t=1ms     2/t = 2000= 120000000/(1000*60)       
    //TIM_TimeBaseStructure.TIM_Period = 999;       
    //TIM_TimeBaseStructure.TIM_Prescaler = 59;      

	//t=133ms     2/t = 15 = 120000000/(10000*800)       
    TIM_TimeBaseStructure.TIM_Period = 9999;       
    TIM_TimeBaseStructure.TIM_Prescaler = 59;      

	//t= 1s
    //TIM_TimeBaseStructure.TIM_Period = 9999;       
    //TIM_TimeBaseStructure.TIM_Prescaler = 5999;      
	

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);//Clear TIM2 update pending flag
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // Enable TIM2 Update interrupt
    TIM_Cmd(TIM4, ENABLE);//
    
	/* Enable the TIM2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 

#endif
}

void fanControl_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  
    /* PWM信号电平跳变值 */  
    u16 CCR1= 36000;          
    u16 CCR2= 36000;  
    /*PCLK1经过2倍频后作为TIM3的时钟源等于72MHz*/  

    if(getCommHardVersion() == 0)
    {
        return;
    }
    fan_gpio_init();  
    Time4_Config();
}

