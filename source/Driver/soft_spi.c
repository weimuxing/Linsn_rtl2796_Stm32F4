#include "soft_spi.h"

#if 0
#define SOFT_SPI_DELAY()
#else
#define SOFT_SPI_DELAY() SoftSpi_Delay()
static void SoftSpi_Delay(void)
{
	int i;
	//for(i=0; i<20; i++);
	//for(i=0; i<5; i++);
	for(i=0; i<8; i++);
}
#endif


/*
 * This soft spi driver use CPOL = 0, CPHA = 0;
 *
 */

static SpiPortTypeDef SoftSpiUsedPort = SPI_PORT_B;

void SoftSpi_Init(SpiPortTypeDef SPI_PORT_X)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(SPI_PORT_X == SPI_PORT_A)
	{
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);

		// MISO
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_MISO_PIN;
		GPIO_Init(SOFT_SPIA_MISO_PORT, &GPIO_InitStruct);

		// MOSI
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_MOSI_PIN;
		GPIO_Init(SOFT_SPIA_MOSI_PORT, &GPIO_InitStruct);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_SCLK_PIN;
		GPIO_Init(SOFT_SPIA_SCLK_PORT, &GPIO_InitStruct);
		GPIO_ResetBits(SOFT_SPIA_SCLK_PORT,SOFT_SPIA_SCLK_PIN);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_CS_PIN;
		GPIO_Init(SOFT_SPIA_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIA_CS_PORT,SOFT_SPIA_CS_PIN);

		// WP
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_WP_PIN;
		GPIO_Init(SOFT_SPIA_WP_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIA_WP_PORT,SOFT_SPIA_WP_PIN);
	
	}
	else if(SPI_PORT_X == SPI_PORT_B)
	{
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE, ENABLE);

		// MISO
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_MISO_PIN;
		GPIO_Init(SOFT_SPIB_MISO_PORT, &GPIO_InitStruct);

		// MOSI
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_MOSI_PIN;
		GPIO_Init(SOFT_SPIB_MOSI_PORT, &GPIO_InitStruct);
		
		
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_SCLK_PIN;
		GPIO_Init(SOFT_SPIB_SCLK_PORT, &GPIO_InitStruct);
		GPIO_ResetBits(SOFT_SPIB_SCLK_PORT,SOFT_SPIB_SCLK_PIN);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_CS_PIN;
		GPIO_Init(SOFT_SPIB_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIB_CS_PORT,SOFT_SPIB_CS_PIN);

//		// HOLD
//		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_HOLD_PIN;
//		GPIO_Init(SOFT_SPIB_HOLD_PORT, &GPIO_InitStruct);
//		GPIO_SetBits(SOFT_SPIB_HOLD_PORT,SOFT_SPIB_HOLD_PIN);
		// WP
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_WP_PIN;
		GPIO_Init(SOFT_SPIB_WP_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIB_WP_PORT,SOFT_SPIB_WP_PIN);
	
	}
	else if(SPI_PORT_X == SPI_PORT_C)
	{
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		// MISO
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_MISO_PIN;
		GPIO_Init(SOFT_SPIC_MISO_PORT, &GPIO_InitStruct);

		// MOSI
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_MOSI_PIN;
		GPIO_Init(SOFT_SPIC_MOSI_PORT, &GPIO_InitStruct);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_SCLK_PIN;
		GPIO_Init(SOFT_SPIC_SCLK_PORT, &GPIO_InitStruct);
		GPIO_ResetBits(SOFT_SPIC_SCLK_PORT,SOFT_SPIC_SCLK_PIN);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_CS_PIN;
		GPIO_Init(SOFT_SPIC_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIC_CS_PORT,SOFT_SPIC_CS_PIN);
	
	}
	else if(SPI_PORT_X == SPI_PORT_D)
	{
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOF, ENABLE);

		// MISO
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_MISO_PIN;
		GPIO_Init(SOFT_SPID_MISO_PORT, &GPIO_InitStruct);

		// MOSI
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_MOSI_PIN;
		GPIO_Init(SOFT_SPID_MOSI_PORT, &GPIO_InitStruct);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_SCLK_PIN;
		GPIO_Init(SOFT_SPID_SCLK_PORT, &GPIO_InitStruct);
		GPIO_ResetBits(SOFT_SPID_SCLK_PORT,SOFT_SPID_SCLK_PIN);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_CS_PIN;
		GPIO_Init(SOFT_SPID_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPID_CS_PORT,SOFT_SPID_CS_PIN);
	
	}	
}

void SoftSpi_Deinit(SpiPortTypeDef SPI_PORT_X)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if(SPI_PORT_X == SPI_PORT_A)
	{
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		// MISO
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_MISO_PIN;
		GPIO_Init(SOFT_SPIA_MISO_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIA_MISO_PORT, SOFT_SPIA_MISO_PIN);
		// MOSI
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_MOSI_PIN;
		GPIO_Init(SOFT_SPIA_MOSI_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIA_MOSI_PORT, SOFT_SPIA_MOSI_PIN);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_SCLK_PIN;
		GPIO_Init(SOFT_SPIA_SCLK_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIA_SCLK_PORT, SOFT_SPIA_SCLK_PIN);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_CS_PIN;
		GPIO_Init(SOFT_SPIA_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIA_CS_PORT, SOFT_SPIA_CS_PIN);

		// WP
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIA_WP_PIN;
		GPIO_Init(SOFT_SPIA_WP_PORT, &GPIO_InitStruct);
		GPIO_ResetBits(SOFT_SPIA_WP_PORT,SOFT_SPIA_WP_PIN);
	}
	else if(SPI_PORT_X == SPI_PORT_B)
	{		
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		// MISO
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_MISO_PIN;
		GPIO_Init(SOFT_SPIB_MISO_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIB_MISO_PORT,GPIO_InitStruct.GPIO_Pin);
		
		
		// MOSI
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_MOSI_PIN;
		GPIO_Init(SOFT_SPIB_MOSI_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIB_MOSI_PORT,GPIO_InitStruct.GPIO_Pin);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_SCLK_PIN;
		GPIO_Init(SOFT_SPIB_SCLK_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIB_SCLK_PORT,GPIO_InitStruct.GPIO_Pin);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_CS_PIN;
		GPIO_Init(SOFT_SPIB_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIB_CS_PORT,GPIO_InitStruct.GPIO_Pin);

//		// HOLD
//		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_HOLD_PIN;
//		GPIO_Init(SOFT_SPIB_HOLD_PORT, &GPIO_InitStruct);
//		GPIO_SetBits(SOFT_SPIB_HOLD_PORT,SOFT_SPIB_HOLD_PIN);
		// WP
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIB_WP_PIN;
		GPIO_Init(SOFT_SPIB_WP_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIB_WP_PORT,SOFT_SPIB_WP_PIN);
	}
	else if(SPI_PORT_X == SPI_PORT_C)
	{
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		// MISO
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_MISO_PIN;
		GPIO_Init(SOFT_SPIC_MISO_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIC_MISO_PORT, SOFT_SPIC_MISO_PIN);
		// MOSI
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_MOSI_PIN;
		GPIO_Init(SOFT_SPIC_MOSI_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIC_MOSI_PORT, SOFT_SPIC_MOSI_PIN);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_SCLK_PIN;
		GPIO_Init(SOFT_SPIC_SCLK_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIC_SCLK_PORT, SOFT_SPIC_SCLK_PIN);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPIC_CS_PIN;
		GPIO_Init(SOFT_SPIC_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPIC_CS_PORT, SOFT_SPIC_CS_PIN);
	}
	else if(SPI_PORT_X == SPI_PORT_D)
	{
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

		// MISO
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_MISO_PIN;
		GPIO_Init(SOFT_SPID_MISO_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPID_MISO_PORT, SOFT_SPIA_MISO_PIN);
		// MOSI
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_MOSI_PIN;
		GPIO_Init(SOFT_SPID_MOSI_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPID_MOSI_PORT, SOFT_SPID_MOSI_PIN);
		// SCLK
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_SCLK_PIN;
		GPIO_Init(SOFT_SPID_SCLK_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPID_SCLK_PORT, SOFT_SPID_SCLK_PIN);
		// CS
		GPIO_InitStruct.GPIO_Pin = SOFT_SPID_CS_PIN;
		GPIO_Init(SOFT_SPID_CS_PORT, &GPIO_InitStruct);
		GPIO_SetBits(SOFT_SPID_CS_PORT, SOFT_SPID_CS_PIN);
	}
}

void SoftSpi_UsePort(SpiPortTypeDef SPI_PORT_X)
{
	if(SPI_PORT_MAX <= SPI_PORT_X)
		return;
	
	SoftSpiUsedPort = SPI_PORT_X;
	SoftSpi_Init(SPI_PORT_X);
}

void SoftSpi_UnUsePort(SpiPortTypeDef SPI_PORT_X)
{
	if(SPI_PORT_MAX <= SPI_PORT_X)
		return;

	SoftSpiUsedPort = SPI_PORT_X;
	SoftSpi_Deinit(SPI_PORT_X);
}

BitAction READ_SPI_MISO(void)
{
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		return GPIO_ReadInputDataBit(SOFT_SPIA_MISO_PORT,SOFT_SPIA_MISO_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		#ifdef ENABLE_FAST_SOFT_SPI
		return READ_SOFT_SPIB_MISO?Bit_SET:Bit_RESET;
		#else
		return GPIO_ReadInputDataBit(SOFT_SPIB_MISO_PORT,SOFT_SPIB_MISO_PIN);
		#endif
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		return GPIO_ReadInputDataBit(SOFT_SPIC_MISO_PORT,SOFT_SPIC_MISO_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_D)
	{
		return GPIO_ReadInputDataBit(SOFT_SPID_MISO_PORT,SOFT_SPID_MISO_PIN);
	}

	return Bit_SET;
}


void SPI_MOSI_HIGH(void)
{
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		GPIO_SetBits(SOFT_SPIA_MOSI_PORT,SOFT_SPIA_MOSI_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		#ifdef ENABLE_FAST_SOFT_SPI
		SET_SOFT_SPIB_MOSI;
		#else
		GPIO_SetBits(SOFT_SPIB_MOSI_PORT,SOFT_SPIB_MOSI_PIN);
		#endif
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		GPIO_SetBits(SOFT_SPIC_MOSI_PORT,SOFT_SPIC_MOSI_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_D)
	{
		GPIO_SetBits(SOFT_SPID_MOSI_PORT,SOFT_SPID_MOSI_PIN);
	}	
}



void SPI_MOSI_LOW(void)
{
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		GPIO_ResetBits(SOFT_SPIA_MOSI_PORT,SOFT_SPIA_MOSI_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		#ifdef ENABLE_FAST_SOFT_SPI
		CLEAR_SOFT_SPIB_MOSI;
		#else
		GPIO_ResetBits(SOFT_SPIB_MOSI_PORT,SOFT_SPIB_MOSI_PIN);
		#endif
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		GPIO_ResetBits(SOFT_SPIC_MOSI_PORT,SOFT_SPIC_MOSI_PIN);
}
	else if(SoftSpiUsedPort == SPI_PORT_D)
	{
		GPIO_ResetBits(SOFT_SPID_MOSI_PORT,SOFT_SPID_MOSI_PIN);
	}	
}


void SPI_SCLK_HIGH(void)
{
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		GPIO_SetBits(SOFT_SPIA_SCLK_PORT,SOFT_SPIA_SCLK_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		#ifdef ENABLE_FAST_SOFT_SPI
		SET_SOFT_SPIB_SCLK;
		#else
		GPIO_SetBits(SOFT_SPIB_SCLK_PORT,SOFT_SPIB_SCLK_PIN);
		#endif
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		GPIO_SetBits(SOFT_SPIC_SCLK_PORT,SOFT_SPIC_SCLK_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_D)
	{
		GPIO_SetBits(SOFT_SPID_SCLK_PORT,SOFT_SPID_SCLK_PIN);
	}	
}

void SPI_SCLK_LOW(void)
{
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		GPIO_ResetBits(SOFT_SPIA_SCLK_PORT,SOFT_SPIA_SCLK_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		#ifdef ENABLE_FAST_SOFT_SPI
		CLEAR_SOFT_SPIB_SCLK;
		#else
		GPIO_ResetBits(SOFT_SPIB_SCLK_PORT,SOFT_SPIB_SCLK_PIN);
		#endif
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		GPIO_ResetBits(SOFT_SPIC_SCLK_PORT,SOFT_SPIC_SCLK_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_D)
	{
		GPIO_ResetBits(SOFT_SPID_SCLK_PORT,SOFT_SPID_SCLK_PIN);
	}	
}

void SoftSpi_CS_High(void)
{
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		GPIO_SetBits(SOFT_SPIA_CS_PORT,SOFT_SPIA_CS_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		GPIO_SetBits(SOFT_SPIB_CS_PORT,SOFT_SPIB_CS_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		GPIO_SetBits(SOFT_SPIC_CS_PORT,SOFT_SPIC_CS_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_D)
	{
		GPIO_SetBits(SOFT_SPID_CS_PORT,SOFT_SPID_CS_PIN);
	}	
	SOFT_SPI_DELAY();
}

void SoftSpi_CS_Low(void)
{
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		GPIO_ResetBits(SOFT_SPIA_CS_PORT,SOFT_SPIA_CS_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		GPIO_ResetBits(SOFT_SPIB_CS_PORT,SOFT_SPIB_CS_PIN);
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		GPIO_ResetBits(SOFT_SPIC_CS_PORT,SOFT_SPIC_CS_PIN);
	}	
	else if(SoftSpiUsedPort == SPI_PORT_D)
	{
		GPIO_ResetBits(SOFT_SPID_CS_PORT,SOFT_SPID_CS_PIN);
	}	
	SOFT_SPI_DELAY();
	SOFT_SPI_DELAY();
}


/* CPOL = 0, CPHA = 0 */

u8 SoftSpi_SendData(u8 value)
{
	int i;
	u8 result = 0;
	#ifdef ENABLE_FAST_SOFT_SPI
	if(SoftSpiUsedPort == SPI_PORT_A)
	{
		for(i=0; i<8; ++i)
		{
			result <<= 1; 
			
			if(value & 0x80)
			{
				SET_SOFT_SPIA_MOSI;
			}
			else
			{
				CLEAR_SOFT_SPIA_MOSI;
			}
			SET_SOFT_SPIA_SCLK;
			SOFT_SPI_DELAY();
			if(READ_SOFT_SPIA_MISO)
			{
				result |= 0x01;
			}
			value <<= 1;
			CLEAR_SOFT_SPIA_SCLK;
			SOFT_SPI_DELAY();
			
		}
	}
	else if(SoftSpiUsedPort == SPI_PORT_B)
	{
		for(i=0; i<8; ++i)
		{
			result <<= 1; 
			
			if(value & 0x80)
			{
				SET_SOFT_SPIB_MOSI;
			}
			else
			{
				CLEAR_SOFT_SPIB_MOSI;
			}
			SET_SOFT_SPIB_SCLK;
			SOFT_SPI_DELAY();
			if(READ_SOFT_SPIB_MISO)
			{
				result |= 0x01;
			}
			value <<= 1;
			CLEAR_SOFT_SPIB_SCLK;
			SOFT_SPI_DELAY();
			
		}
	}
	else if(SoftSpiUsedPort == SPI_PORT_C)
	{
		for(i=0; i<8; ++i)
		{
			result <<= 1; 
			
			if(value & 0x80)
			{
				SET_SOFT_SPIC_MOSI;
			}
			else
			{
				CLEAR_SOFT_SPIC_MOSI;
			}
			SET_SOFT_SPIC_SCLK;
			SOFT_SPI_DELAY();
			if(READ_SOFT_SPIC_MISO)
			{
				result |= 0x01;
			}
			value <<= 1;
			CLEAR_SOFT_SPIC_SCLK;
			SOFT_SPI_DELAY();
			
		}
	}
	#else
	for(i=0; i<8; ++i)
	{
		result <<= 1; 
		if(READ_SPI_MISO())
		{
			result |= 0x01;
		}
		if(value & 0x80)
		{
			SPI_MOSI_HIGH();
		}
		else
		{
			SPI_MOSI_LOW();
		}
		SPI_SCLK_HIGH();
		SOFT_SPI_DELAY();
		value <<= 1;
		SPI_SCLK_LOW();
		SOFT_SPI_DELAY();
		
	}
	#endif
	return result;
}

