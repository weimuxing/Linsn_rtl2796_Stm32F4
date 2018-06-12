#include "EDID.h"
#include "i2c.h"
#include "systick.h"

const I2CPort_TypeDef edid_i2c_port[EDID_PORT_MAX_SIZE] =
{
	I2C_EDID_1,			// DVI EDID
	I2C_EDID_2,			// Preview EDID
	I2C_EDID_3,
	//I2C_EDID_4,
};

GPIO_TypeDef * const hpdPort[EDID_PORT_MAX_SIZE] =
{
	GPIOH,
	GPIOI,
	GPIOI,			// As HDMI hpd, Preview Hpd not use
	//GPIOI,
	
};

const u16 hpdPin[EDID_PORT_MAX_SIZE] =
{
	GPIO_Pin_15,
	GPIO_Pin_4,
	GPIO_Pin_5,
	
};

#define PAGE_WRITE_SIZE (4)
#define PAGE_WRITE_DELAY (10)

#define EDID_EEPROM_WRITE_PAGE 16
#define EDID_EEPROM_I2C_ADDRESS 0xA0
#define EDID_EEPROM_WRITE_TIME 10   // ms


void WriteEDID_EEPROM(int EDID_PORT_X, int address, u8 *buffer, int size)
{
	int ucRemainLen = address % EDID_EEPROM_WRITE_PAGE;
	int pageSize;
	int i;
	

	if(EDID_PORT_X >= EDID_PORT_MAX_SIZE)
	{
		return;
	}
#if 0
	int remainLen = size;
	int writeLen = PAGE_WRITE_SIZE;
	while(remainLen)
	{
		
		if(remainLen > PAGE_WRITE_SIZE)
		{
			writeLen = PAGE_WRITE_SIZE;
		}
		else
		{
			writeLen = remainLen;
		}
		i2cBurstWriteBytes(edid_i2c_port[EDID_PORT_X], 0xA0, address, buffer, writeLen);
		SysDelay1ms(PAGE_WRITE_DELAY);
		
		address += writeLen;
		buffer += writeLen;
		remainLen -= writeLen;
	}
#else
	if(ucRemainLen)		
	{
		ucRemainLen = EDID_EEPROM_WRITE_PAGE - ucRemainLen;
		if(ucRemainLen < size)
		{
			i2cBurstWriteBytes(edid_i2c_port[EDID_PORT_X], EDID_EEPROM_I2C_ADDRESS, address, buffer, ucRemainLen);;//i2cBurstWriteBytes2(I2C_x, EEPROM_I2C_ADDRESS, address, buffer, ucRemainLen);
			SysDelay1ms(EDID_EEPROM_WRITE_TIME+1);
			size -= ucRemainLen;
			address += ucRemainLen;
			buffer += ucRemainLen;			
		}
		else
		{
			i2cBurstWriteBytes(edid_i2c_port[EDID_PORT_X], EDID_EEPROM_I2C_ADDRESS,address, buffer, size);//i2cBurstWriteBytes2(I2C_x,EEPROM_I2C_ADDRESS, address, buffer, size);
			SysDelay1ms(EDID_EEPROM_WRITE_TIME+1);
			return;
		}	
	}
	pageSize = size/EDID_EEPROM_WRITE_PAGE;
	for(i=0; i<pageSize; i++)
	{
		i2cBurstWriteBytes(edid_i2c_port[EDID_PORT_X], EDID_EEPROM_I2C_ADDRESS,address, buffer, EDID_EEPROM_WRITE_PAGE); //i2cBurstWriteBytes2(I2C_x,EEPROM_I2C_ADDRESS, address, buffer, EEPROM_WRITE_PAGE);
		SysDelay1ms(EDID_EEPROM_WRITE_TIME+1);
		address += EDID_EEPROM_WRITE_PAGE;
		buffer += EDID_EEPROM_WRITE_PAGE;
		size -= EDID_EEPROM_WRITE_PAGE;		
	}
	if(size)
	{
		i2cBurstWriteBytes(edid_i2c_port[EDID_PORT_X], EDID_EEPROM_I2C_ADDRESS,address,  buffer, size); //i2cBurstWriteBytes2(I2C_x,EEPROM_I2C_ADDRESS, address, buffer, size);
		SysDelay1ms(EDID_EEPROM_WRITE_TIME+1);
	}	
	#endif
}
void ReadEDID_EEPROM(int EDID_PORT_X, int address, u8 *buffer, int size)
{
	if(EDID_PORT_X < EDID_PORT_MAX_SIZE)
	{
		i2cBurstReadBytes(edid_i2c_port[EDID_PORT_X], 0xA0, address, buffer, size);
	}
//	i2cBurstReadBytes(edid_i2c_port[EDID_PORT_X], 0xA0, address, buffer, size);
}


void SetHPD_Pin(int EDID_PORT_X, int highOrLow) // highOrLow: 0 -- Low, 1 -- Hight
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef *gpio_x;
	

	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA, ENABLE);
	
	if(EDID_PORT_X < EDID_PORT_MAX_SIZE)
	{
		if(highOrLow)
		{

			#if 1
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//GPIO_Mode_IN;
			GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
					
			GPIO_InitStruct.GPIO_Pin = hpdPin[EDID_PORT_X];
			gpio_x = hpdPort[EDID_PORT_X];
			
			GPIO_Init(gpio_x,&GPIO_InitStruct);
			#endif
			GPIO_SetBits(gpio_x,GPIO_InitStruct.GPIO_Pin);
		}
		else
		{

			#if 1
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
					
			GPIO_InitStruct.GPIO_Pin = hpdPin[EDID_PORT_X];
			gpio_x = hpdPort[EDID_PORT_X];
			
			GPIO_Init(gpio_x,&GPIO_InitStruct);
			#endif
			
			GPIO_ResetBits(gpio_x,GPIO_InitStruct.GPIO_Pin);
		}
	}
}



void EdidHpInit(void) // highOrLow: 0 -- Low, 1 -- Hight
{
	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_TypeDef *gpio_x;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI, ENABLE);

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Init(GPIOI,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOH,&GPIO_InitStruct);



	GPIO_ResetBits(GPIOH,GPIO_Pin_15);
	GPIO_ResetBits(GPIOI,GPIO_Pin_4|GPIO_Pin_5);
}



