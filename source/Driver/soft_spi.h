#ifndef SOFT_SPI_H
#define SOFT_SPI_H

#include "define.h"

typedef enum
{
	SPI_PORT_A,
	SPI_PORT_B,
	SPI_PORT_C,
	SPI_PORT_D,
	SPI_PORT_MAX,
} SpiPortTypeDef;

#define ENABLE_FAST_SOFT_SPI 1


#ifdef VIP_43S
//SPIA 连接到FPGA的Flash,用于存储FPGA的主程序
//PA6 -- SPI1_MISO--FPGA_AS_ASDO(Flash pin 5:ASDI )
//PA5 -- SPI1_SCK -- FPGA_AS_DCLK (Flash pin 6:DCLK )
//PA4 -- SPI1_NSS --FPGA_AS_nCSO (Flash pin 1:NSC )
//PB6 -- SPI1_MOSI -- FPGA_AS_DATA0((Flash pin 2:DATA))
#define SOFT_SPIA_MISO_PIN 		GPIO_Pin_5
#define SOFT_SPIA_MISO_PORT 	GPIOB
#define SOFT_SPIA_MOSI_PIN 		GPIO_Pin_6
#define SOFT_SPIA_MOSI_PORT 	GPIOA
#define SOFT_SPIA_SCLK_PIN 		GPIO_Pin_5
#define SOFT_SPIA_SCLK_PORT 	GPIOA
#define SOFT_SPIA_CS_PIN 		GPIO_Pin_4
#define SOFT_SPIA_CS_PORT 		GPIOA

#ifdef ENABLE_FAST_SOFT_SPI
#define READ_SOFT_SPIA_MISO (PBin(5))
#define SET_SOFT_SPIA_MOSI   (PAout(6) = 1)
#define CLEAR_SOFT_SPIA_MOSI (PAout(6) = 0)
#define SET_SOFT_SPIA_SCLK   (PAout(5) = 1)
#define CLEAR_SOFT_SPIA_SCLK (PAout(5) = 0)
#else
#define READ_SOFT_SPIA_MISO 	GPIO_ReadInputDataBit(SOFT_SPIA_MISO_PORT,SOFT_SPIA_MISO_PIN)
#define SET_SOFT_SPIA_MOSI  	GPIO_SetBits(SOFT_SPIA_MOSI_PORT,SOFT_SPIA_MOSI_PIN) 
#define CLEAR_SOFT_SPIA_MOSI 	GPIO_ResetBits(SOFT_SPIA_MOSI_PORT,SOFT_SPIA_MOSI_PIN) 
#define SET_SOFT_SPIA_SCLK 		GPIO_SetBits(SOFT_SPIA_SCLK_PORT,SOFT_SPIA_SCLK_PIN) 
#define CLEAR_SOFT_SPIA_SCLK 	GPIO_ResetBits(SOFT_SPIA_SCLK_PORT,SOFT_SPIA_SCLK_PIN)
#endif

#else

#define SOFT_SPIA_MISO_PIN 	GPIO_Pin_2
#define SOFT_SPIA_MISO_PORT GPIOC
#define SOFT_SPIA_MOSI_PIN 	GPIO_Pin_3
#define SOFT_SPIA_MOSI_PORT GPIOC
#define SOFT_SPIA_SCLK_PIN 	GPIO_Pin_13
#define SOFT_SPIA_SCLK_PORT GPIOB
#define SOFT_SPIA_CS_PIN 	GPIO_Pin_0
#define SOFT_SPIA_CS_PORT 	GPIOC
#define SOFT_SPIA_WP_PIN	GPIO_Pin_0
#define SOFT_SPIA_WP_PORT 	GPIOB



#define READ_SOFT_SPIA_MISO (PCin(2))
#define SET_SOFT_SPIA_MOSI   (PCout(3) = 1)
#define CLEAR_SOFT_SPIA_MOSI (PCout(3) = 0)
#define SET_SOFT_SPIA_SCLK   (PBout(13) = 1)
#define CLEAR_SOFT_SPIA_SCLK (PBout(13) = 0)

#define SPIA_MOSI 	PCout(3)
#define SPIA_SCLK 	PBout(13)
#define SPIA_CS 	PCout(0)

#endif


//SPIB 用于烧写2796
// W25Q16BVSSIG
// PB15 --SPI_MOSI_9320--SPI_DI(Flash pin 2:SO )
// PB14 --SPI_MISO_9320--SPI_DO(Flash pin 5:SI )
// PB13 --SPI2_SCK_9320--SPI_CLK
// PB12 --SPI_NSS_9320 -SPI_CS

#ifdef VIP_43S

#define SOFT_SPIB_MISO_PIN 		GPIO_Pin_15
#define SOFT_SPIB_MISO_PORT 	GPIOB
#define SOFT_SPIB_MOSI_PIN 		GPIO_Pin_14
#define SOFT_SPIB_MOSI_PORT 	GPIOB
#define SOFT_SPIB_SCLK_PIN 		GPIO_Pin_13
#define SOFT_SPIB_SCLK_PORT 	GPIOB
#define SOFT_SPIB_CS_PIN 		GPIO_Pin_12
#define SOFT_SPIB_CS_PORT 		GPIOB

#ifdef ENABLE_FAST_SOFT_SPI
#define READ_SOFT_SPIB_MISO (PBin(15))
#define SET_SOFT_SPIB_MOSI  (PBout(14) = 1)
#define CLEAR_SOFT_SPIB_MOSI (PBout(14) = 0)
#define SET_SOFT_SPIB_SCLK (PBout(13) = 1)
#define CLEAR_SOFT_SPIB_SCLK (PBout(13) = 0)
#else
#define READ_SOFT_SPIB_MISO 	GPIO_ReadInputDataBit(SOFT_SPIB_MISO_PORT,SOFT_SPIB_MISO_PIN)
#define SET_SOFT_SPIB_MOSI  	GPIO_SetBits(SOFT_SPIB_MOSI_PORT,SOFT_SPIB_MOSI_PIN) 
#define CLEAR_SOFT_SPIB_MOSI 	GPIO_ResetBits(SOFT_SPIB_MOSI_PORT,SOFT_SPIB_MOSI_PIN) 
#define SET_SOFT_SPIB_SCLK 		GPIO_SetBits(SOFT_SPIB_SCLK_PORT,SOFT_SPIB_SCLK_PIN) 
#define CLEAR_SOFT_SPIB_SCLK 	GPIO_ResetBits(SOFT_SPIB_SCLK_PORT,SOFT_SPIB_SCLK_PIN)
#endif

#else

#define SOFT_SPIB_MISO_PIN 	GPIO_Pin_2
#define SOFT_SPIB_MISO_PORT GPIOC
#define SOFT_SPIB_MOSI_PIN 	GPIO_Pin_3
#define SOFT_SPIB_MOSI_PORT GPIOC
#define SOFT_SPIB_SCLK_PIN 	GPIO_Pin_13
#define SOFT_SPIB_SCLK_PORT GPIOB
#define SOFT_SPIB_CS_PIN 	GPIO_Pin_12
#define SOFT_SPIB_CS_PORT 	GPIOB

//#define SOFT_SPIB_HOLD_PIN 	GPIO_Pin_2
//#define SOFT_SPIB_HOLD_PORT 	GPIOB
#define SOFT_SPIB_WP_PIN 	GPIO_Pin_14
#define SOFT_SPIB_WP_PORT 	GPIOH



#define ENABLE_FAST_SOFT_SPI 1

#ifdef ENABLE_FAST_SOFT_SPI

#define READ_SOFT_SPIB_MISO 	(PCin(2))
#define SET_SOFT_SPIB_MOSI  	(PCout(3) = 1)
#define CLEAR_SOFT_SPIB_MOSI 	(PCout(3) = 0)
#define SET_SOFT_SPIB_SCLK 		(PBout(13) = 1)
#define CLEAR_SOFT_SPIB_SCLK 	(PBout(13) = 0)


#endif

// SPIC连接到FPGA的25Q40,用于存储图片,字库
#ifdef VIP_43S

#define SOFT_SPIC_MISO_PIN 		GPIO_Pin_0
#define SOFT_SPIC_MISO_PORT 	GPIOC
#define SOFT_SPIC_MOSI_PIN 		GPIO_Pin_7
#define SOFT_SPIC_MOSI_PORT 	GPIOB
#define SOFT_SPIC_SCLK_PIN 		GPIO_Pin_6
#define SOFT_SPIC_SCLK_PORT 	GPIOB
#define SOFT_SPIC_CS_PIN 		GPIO_Pin_13
#define SOFT_SPIC_CS_PORT 		GPIOE

#define READ_SOFT_SPIC_MISO 	(PCin(0))
#define SET_SOFT_SPIC_MOSI   	(PBout(7) = 1)
#define CLEAR_SOFT_SPIC_MOSI 	(PBout(7) = 0)
#define SET_SOFT_SPIC_SCLK   	(PBout(6) = 1)
#define CLEAR_SOFT_SPIC_SCLK 	(PBout(6) = 0)

#else

#define SOFT_SPIC_MISO_PIN 	GPIO_Pin_4
#define SOFT_SPIC_MISO_PORT GPIOB
#define SOFT_SPIC_MOSI_PIN 	GPIO_Pin_5
#define SOFT_SPIC_MOSI_PORT GPIOB
#define SOFT_SPIC_SCLK_PIN 	GPIO_Pin_3
#define SOFT_SPIC_SCLK_PORT GPIOB
#define SOFT_SPIC_CS_PIN 	GPIO_Pin_2
#define SOFT_SPIC_CS_PORT 	GPIOE



#define READ_SOFT_SPIC_MISO 	(PBin(4))
#define SET_SOFT_SPIC_MOSI  	(PBout(5) = 1)
#define CLEAR_SOFT_SPIC_MOSI 	(PBout(5) = 0)
#define SET_SOFT_SPIC_SCLK 		(PBout(3) = 1)
#define CLEAR_SOFT_SPIC_SCLK 	(PBout(3) = 0)

#endif

#endif

// SPID连接到GS7921A
// For GS2971A
// SDI_GS2971_RST		PE2
// SDOUT --MCU_SPI_DATA0 --PG0
// SDIN --MCU_SPI_ASD0--PF15
// SCLK --MCU_SPI_DCLK --PF14
// CS0 --MCU_SPI_nCSO --PF13
#define SOFT_SPID_MISO_PIN GPIO_Pin_0
#define SOFT_SPID_MISO_PORT GPIOG
#define SOFT_SPID_MOSI_PIN GPIO_Pin_15
#define SOFT_SPID_MOSI_PORT GPIOF
#define SOFT_SPID_SCLK_PIN GPIO_Pin_14
#define SOFT_SPID_SCLK_PORT GPIOF
#define SOFT_SPID_CS_PIN GPIO_Pin_13
#define SOFT_SPID_CS_PORT GPIOF

#define READ_SOFT_SPID_MISO (PGin(0))
#define SET_SOFT_SPID_MOSI   (PFout(15) = 1)
#define CLEAR_SOFT_SPID_MOSI (PFout(15) = 0)
#define SET_SOFT_SPID_SCLK   (PFout(14) = 1)
#define CLEAR_SOFT_SPID_SCLK (PFout(14) = 0)

void SoftSpi_Init(SpiPortTypeDef SPI_PORT_X);

void SoftSpi_UsePort(SpiPortTypeDef SPI_PORT_X);
void SoftSpi_UnUsePort(SpiPortTypeDef SPI_PORT_X);
u8 SoftSpi_SendData(u8 value);
void SoftSpi_CS_High(void);
void SoftSpi_CS_Low(void);

#endif
