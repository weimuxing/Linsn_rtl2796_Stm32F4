
#include "SysConfig.h"

#include "stm32f2xx_gpio.h"
#include "tempSensor.h"

//初始化ADC
//这里我们仅以规则通道为例
//开启温度传感器通道																   
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//使能ADC1时钟

  //先初始化IO口
 // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// 上拉
  //GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
  ADC_TempSensorVrefintCmd(ENABLE);//使能内部温度传感器
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; //ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_ExternalTrigConv = 0;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);
 
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles );	//ADC5,ADC通道,480个周期,提高采样时间可以提高精确度		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );	//ADC16,ADC通道,480个周期,提高采样时间可以提高精确度		
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	 			
 
}				  
//获得ADC值
//ch:通道值 @ref ADC_channels  0~16：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	BYTE retry = 20;
	
	 //设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	

	while((!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )) && retry)//等待转换结束
	{
		retry--;
	}

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);//获取通道转换值
		delay_ms(5);
	}
	return temp_val/times;
} 

//得到温度值
//返回值:温度值(扩大了100倍,单位:℃.)
//    ADC转换结束以后，读取ADC_DR寄存器中的结果，转换温度值计算公式如下：
//          V25 - VSENSE
//  T(℃) = ------------  + 25
//           Avg_Slope
//   V25：  温度传感器在25℃时 的输出电压，典型值1.43 V。
//  VSENSE：温度传感器的当前输出电压，与ADC_DR 寄存器中的结果ADC_ConvertedValue之间的转换关系为： 
//            ADC_ConvertedValue * Vdd
//  VSENSE = --------------------------
//            Vdd_convert_value(0xFFF)
//  Avg_Slope：温度传感器输出电压和温度的关联参数，典型值4.3 mV/℃。
short Get_Temprate(void)
{
	u32 adcx;
	short result;
 	double temperate;
	adcx=Get_Adc_Average(ADC_Channel_16,10);	//读取通道16内部温度传感器通道,10次取平均
	//printf("Get_Temprate = %d\r\n",adcx);
	temperate=(float)adcx*(3.3/4096);		//电压值
	temperate=(temperate-0.76)/0.0025 + 25; //转换为温度值 
	result=temperate*100;					//扩大100倍.
	//printf("Get_Temprate = %d\r\n",result);
	return result;
}
	 

