#ifndef __SPI_H
#define __SPI_H
#include "define.h"

#if 0
#define SPI2_DMA_ENABLE 1				// enable dma
#else
//#define SPI2_DMA_ENABLE 1      //disable dma
#endif

// define DMA
#define SPI_MASTER_DMA                DMA1
#define SPI_MASTER_DMA_CLK            RCC_AHB1Periph_DMA1
#define SPI_MASTER_Rx_DMA_Channel     DMA1_Stream4
#define SPI_MASTER_Rx_DMA_FLAG        DMA_FLAG_TCIF4
#define SPI_MASTER_Tx_DMA_Channel     DMA1_Stream5
#define SPI_MASTER_Tx_DMA_FLAG        DMA_FLAG_TCIF5
#define SPI_MASTER_DR_Base            0x4000380C 

 				  	    													  
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void DMA1_To_SPI2_Init( void *txBuffer, int bufferSize);

#define SPI2_CS PBout(12)    //    Usage: SPI2_CS = 1, SPI2_CS = 0;

		 
#endif

