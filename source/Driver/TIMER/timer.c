/*#########################################################################################
									   _ooOoo_
									  o8888888o
									  88" . "88
									  (| -_- |)
									  O\  =  /O
								   ____/`---'\____
								 .'  \\|     |//  `.
								/  \\|||  :  |||    \
							   /  _||||| -:- |||||-  \
							   |   | \\\  -      |   |
							   | \_|  ''\---/''  |   |
							   \  .-\__  `-`  ___/-. /
							 ___`. .'  /--.--\  `. . __
						  .""'<  `.___\_<|>_/___.'   >'"".
						 | | :  `- \`.;`\ _ /`;.`/ - ` : | |
						 \  \ `-.   \_ __\ /__ _/   .-` /  /
					======`-.____`-.___\_____/___.-`____.-'======
									   `=---='
					^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
							 佛祖保佑       永无BUG
#########################################################################################*/



#include "SysConfig.h"

#include "timer.h"
#include "lwip_comm.h"
 

extern u32 lwip_localtime;
char flag = 0;

#ifdef SUPPORT_DOUBLE_TRIP_KEY
extern u8 SdiDoubleKeyCount;
extern u8 DviDoubleKeyCount;
extern u8 HdmiDoubleKeyCount;


__IO u8 TimeCount = 0;
__IO u8 sdikeytype = 0;
__IO u8 dvikeytype = 0;
__IO u8 hdmikeytype = 0;
#endif


//###################################################################################################################################
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


//###################################################################################################################################
//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		lwip_localtime +=10; //加10

	#ifdef SUPPORT_DOUBLE_TRIP_KEY
		TimeCount++;
		if(TimeCount <= 50)
		{
			if(SdiDoubleKeyCount >= 2 || DviDoubleKeyCount >= 2 || HdmiDoubleKeyCount >= 2)
			{
				
				if(SdiDoubleKeyCount == 2)
					sdikeytype = 2;
				else
					sdikeytype = 0;
				if(DviDoubleKeyCount == 2)
					dvikeytype = 2;
				else
					dvikeytype = 0;
				if(HdmiDoubleKeyCount == 2)
					hdmikeytype = 2;
				else
					hdmikeytype = 0;
				
				TimeCount = 0;
				SdiDoubleKeyCount = 0;
				DviDoubleKeyCount = 0;
				HdmiDoubleKeyCount = 0;
			}
			
		}
		else
		{
			if(SdiDoubleKeyCount == 1)
					sdikeytype = 1;
				else
					sdikeytype = 0;
				if(DviDoubleKeyCount == 1)
					dvikeytype = 1;
				else
					dvikeytype = 0;
				if(HdmiDoubleKeyCount == 1)
					hdmikeytype = 1;
				else
					hdmikeytype = 0;
			
			TimeCount = 0;
			SdiDoubleKeyCount = 0;
			DviDoubleKeyCount = 0;
			HdmiDoubleKeyCount = 0;
		}
	#endif
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
