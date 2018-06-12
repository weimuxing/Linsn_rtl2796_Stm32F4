#include "spi.h"


//SPI口初始化
//这里针是对SPI2的初始化



void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOB, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能 

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB13/14/15上拉
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //PB12   SPI2_CS
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_12); 

 	

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	
	SPI2_ReadWriteByte(0xff);//启动传输	

	/* Enable SPI_MASTER DMA Tx request */
  	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);  

	
	/* Enable SPI_MASTER DMA Rx request */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE); 

}   
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI2,ENABLE); 

} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据					    
}






void DMA1_To_SPI2_Init( void *txBuffer, int bufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_ClearFlag(SPI_MASTER_Tx_DMA_Channel, SPI_MASTER_Tx_DMA_FLAG);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,  ENABLE);

	/* SPI_MASTER_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = SPI_MASTER_DR_Base;
	DMA_InitStructure.DMA_BufferSize = bufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	/* SPI_MASTER_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(SPI_MASTER_Tx_DMA_Channel);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SPI_MASTER_DR_Base;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)txBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(SPI_MASTER_Tx_DMA_Channel, &DMA_InitStructure);

}

void SPI2_To_DMA1_Init(void *rxBuffer, int bufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_ClearFlag(SPI_MASTER_Tx_DMA_Channel,SPI_MASTER_Rx_DMA_FLAG);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,  ENABLE);

	/* SPI_MASTER_Rx_DMA_Channel configuration ---------------------------------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = SPI_MASTER_DR_Base;
	DMA_InitStructure.DMA_BufferSize = bufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	/* SPI_MASTER_Rx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(SPI_MASTER_Rx_DMA_Channel);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SPI_MASTER_DR_Base;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)rxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(SPI_MASTER_Rx_DMA_Channel, &DMA_InitStructure);
	
}



























