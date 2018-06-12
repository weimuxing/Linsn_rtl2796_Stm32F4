#include "rotaryEncoder.h"
#include "define.h"


static volatile int pluseValue[pulse_Max];

#define info(format,...)

static GPIO_TypeDef *PULSE_INT_PORT[pulse_Max]={PULSEA_INT_PORT,PULSEB_INT_PORT};
const  u32  PULSE_INT_PIN[pulse_Max]={PULSEA_INT_PIN,PULSEB_INT_PIN};

static GPIO_TypeDef *PULSE_DIR_PORT[pulse_Max]={PULSEA_DIR_PORT,PULSEB_DIR_PORT};
const  u32  PULSE_DIR_PIN[pulse_Max]={PULSEA_DIR_PIN,PULSEB_DIR_PIN};

const  u32  PULSE_EXTI_PORT[pulse_Max]={PULSEA_EXTI_PORT,PULSEB_EXTI_PORT};
const  u32  PULSE_EXTI_SRC[pulse_Max]={PULSEA_EXTI_SRC,PULSEB_EXTI_SRC};

const  u32  PULSE_EXTI_LINE[pulse_Max]={PULSEA_EXTI_LINE,PULSEB_EXTI_LINE};
const  u32  PULSE_EXTI_IRQ[pulse_Max]={PULSEA_EXTI_IRQ,PULSEB_EXTI_IRQ};
const  u32  PULSE_EXTI_TRG[pulse_Max]={PULSEA_EXTI_TRG,PULSEB_EXTI_TRG};

static bool rise[pulse_Max] = {FALSE, FALSE};
static bool readPulse[pulse_Max] = {FALSE, FALSE};
extern BYTE ProductType;

//bool breadIngPulse[pulse_Max] = {FALSE, FALSE};


static u8 READ_PLUSE_INT(u8 Pluse)
{
	return GPIO_ReadInputDataBit(PULSE_INT_PORT[Pluse], PULSE_INT_PIN[Pluse]);
}

static u8 READ_PLUSE_DIR(u8 Pluse)
{
	return GPIO_ReadInputDataBit(PULSE_DIR_PORT[Pluse], PULSE_DIR_PIN[Pluse]);
}

static void readPulseData(u8 Pluse)
{
	char tmp_pulse_Dir,i;
	char nn;
	char KeyOldInt=0,KeyNewInt,KeyOldDir = 0,KeyNewDir;
	char KeyCount = 0;
	char tmp_pulseB, temp_value;

    for(i = 0; i < 10; i++)     
	{			
		for(nn = 0; nn < 50; nn++);
		{
			KeyNewInt = READ_PLUSE_INT(Pluse);
		}
		
		KeyNewDir = READ_PLUSE_DIR(Pluse);
		if((KeyNewInt == KeyOldInt)&&(KeyNewDir == KeyOldDir))
		{
            if(++KeyCount > 20)    
				break;
		}
		else 
		{
			KeyOldInt = KeyNewInt;
			KeyOldDir = KeyNewDir;
			KeyCount = 0;
		}
	}
	temp_value = READ_PLUSE_INT(Pluse);		
	tmp_pulseB = READ_PLUSE_DIR(Pluse);
 	//if (temp_value && !rise[Pluse])
	if (!rise[Pluse])
	{
		readPulse[Pluse] = TRUE;
		//pluseValue[Pluse] = 0;
		return;
	}    
	if((tmp_pulseB && !rise[Pluse]) || (!tmp_pulseB && rise[Pluse])) 
	{
		printf("RIGHT\r\n");
		if(pluseValue[Pluse] <= 0)
		{
			pluseValue[Pluse]--;
		}
		else
		{
			pluseValue[Pluse] =- 1;
		}
		//while((tmp_pulseB && !rise[Pluse]) || (!tmp_pulseB && rise[Pluse]));
	} 
	else 
	{
	    printf("left\r\n");
		if(pluseValue[Pluse] >= 0)
		{
			pluseValue[Pluse]++;
		}
		else
		{
			pluseValue[Pluse] = 1;
		} 
		//while((tmp_pulseB && !rise[Pluse]) || (!tmp_pulseB && rise[Pluse]));
	}

	readPulse[Pluse] = TRUE;	
	
}
void PLUSE_VNIC(u8 pluse)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	EXTITrigger_TypeDef trigger_flag = EXTI_Trigger_Falling;
	
	readPulseData(pluse);

	#if 1
	if (readPulse[pluse]) 
	{
	    if(READ_PLUSE_INT(pluse))
		{
			trigger_flag = EXTI_Trigger_Falling;
			rise[pluse] = FALSE;
		} 
		else 
		{
			trigger_flag = EXTI_Trigger_Rising;
			rise[pluse] = TRUE;
		}
	}
	
	// Configure EXTI Line
	EXTI_InitStructure.EXTI_Line = PULSE_EXTI_LINE[pluse];
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = trigger_flag;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	#endif
}

void PLUSE_VNIC_ENABLE(u8 Pluse)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = PULSE_EXTI_LINE[Pluse];
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)PULSE_EXTI_TRG[Pluse];  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_DeInit();
}


void PLUSE_VNIC_DISABLE(u8 Pluse)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = PULSE_EXTI_LINE[Pluse];
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)PULSE_EXTI_TRG[Pluse];  
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
}
/*pulse*/
void init_Pluse(u8 Pluse)
{
#if 1
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
  /* Configure PULSE pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = PULSE_INT_PIN[Pluse];
  GPIO_Init(PULSE_INT_PORT[Pluse], &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = PULSE_DIR_PIN[Pluse];
  GPIO_Init(PULSE_DIR_PORT[Pluse], &GPIO_InitStructure);

  /* Connect EXTI Line to  pin */
  SYSCFG_EXTILineConfig(PULSE_EXTI_PORT[Pluse], PULSE_EXTI_SRC[Pluse]);

  /* Configure EXTI Line*/
  EXTI_InitStructure.EXTI_Line = PULSE_EXTI_LINE[Pluse];
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)PULSE_EXTI_TRG[Pluse];  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = PULSE_EXTI_IRQ[Pluse];
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#else
    //u16 CCR1_Val = 2500;
    //u16 CCR2_Val = 1000;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    //TIM_OCInitTypeDef  TIM_OCInitStructure;


    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);


    /* Configure PULSE pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = PULSE_INT_PIN[Pluse];
    GPIO_Init(PULSE_INT_PORT[Pluse], &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PULSE_DIR_PIN[Pluse];
    GPIO_Init(PULSE_DIR_PORT[Pluse], &GPIO_InitStructure);

        
    /*----------------------------------------------------------------*/    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //使能ＴＩＭ３
    TIM_DeInit(TIM5);
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Period =0xffff;       //
    TIM_TimeBaseStructure.TIM_Prescaler =0;       //设置预分频：
    TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;   //设置时钟分频系数：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
    //TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1; 
    /*初始化TIM2定时器 */
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    /*-----------------------------------------------------------------*/
    //编码配置                        编码模式
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, 
                 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);  //TIM_ICPolarity_Rising上升沿捕获
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 6;         //比较滤波器
    TIM_ICInit(TIM5, &TIM_ICInitStructure);

    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);   //使能中断
    //Reset counter
    TIM5->CNT =0;


    TIM_Cmd(TIM5, ENABLE);   //使能定时器3
#endif
}

int GetpluseValue(u8 Pluse)
{
	int value;
    
    //if(breadIngPulse[Pluse] == FALSE)
    //{
     //   return 0;
    //}
	value = pluseValue[Pluse];
	pluseValue[Pluse] = 0;
	return value;	
}

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */


void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(PULSEA_EXTI_LINE))
	{
		PLUSE_VNIC(pulse_A);
		EXTI_ClearITPendingBit(PULSEA_EXTI_LINE);
	}
}


void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(PULSEB_EXTI_LINE))
	{
		PLUSE_VNIC(pulse_B);
		EXTI_ClearITPendingBit(PULSEB_EXTI_LINE);
	}
}



