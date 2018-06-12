#include <stdio.h>
#include "uart.h"
#include "SysConfig.h"
#include "spi_flash.h"
#include "UartDebug.h"


/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PAGESIZE    256
#define SPI_FLASH_SSP_PORT SSP0

/* Private define ------------------------------------------------------------*/
#if !USE_SOFT_SPI_DRIVER
9
#define ENABLE_FAST_SPI  1
#endif

#ifdef ENABLE_FAST_SPI
u
#define SPI_FLASH_OnlySend(value) SSP0_OnlySendData(value)
#define SPI_FLASH_OnlyRead(value) SSP0_OnlyReceiveData(value)
#endif


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
static char spi_flash_type = SPI_FLASH_TYPE;

void SPI_FLASH_Init(SpiPortTypeDef SPI_PORT_X)
{
  u32 id;
#if USE_SOFT_SPI_DRIVER
  SoftSpi_UsePort(SPI_PORT_X);
#else  
  SSP_Config(SPI_FLASH_SSP_PORT);
#endif
  id = SPI_FLASH_ReadID();
  if(id != SPI_FLASH_ID)
  {
  	printf("SPI Flash ID error\r\n");
	printf("SPI Flash ID = %x",(WORD)(id>>16));
	printf("%x\r\n",(WORD)(id));
  }
}

void SPI_FLASH_SetType(int type)
{
	spi_flash_type = type;
}


/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  SPI_FLASH_WaitForWriteEnd();

  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Sector Erase instruction */
  //if(spi_flash_type == M25Q16)
  //{
  //	SPI_FLASH_SendByte(CMD_SE2);
  //}
  //else 
  //{
  SPI_FLASH_SendByte(CMD_SE);
  //}
  /* Send SectorAddr high nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  //printf("%x\r\n", SectorAddr);

  //printf("Wait SectorErase\r\n");

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();

  //printf("End SectorErase\r\n");
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval None
  */
void SPI_FLASH_BulkErase(void)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  /* Bulk Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Bulk Erase instruction  */
  SPI_FLASH_SendByte(CMD_BE);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_ByteWrite(uint32_t WriteAddr, uint8_t value)
{
	 /* Enable the write access to the FLASH */
	SPI_FLASH_WriteEnable();

	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send "Write to Memory " instruction */
	SPI_FLASH_SendByte(CMD_WRITE);
	/* Send WriteAddr high nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	/* Send WriteAddr medium nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	/* Send WriteAddr low nibble address byte to write to */
	SPI_FLASH_SendByte(WriteAddr & 0xFF);


	/* Send the current byte */
	SPI_FLASH_SendByte(value);

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

void SST25VF016B_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	 /* Enable the write access to the FLASH */
	SPI_FLASH_WriteEnable();

	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send "Write to Memory " instruction */
	SPI_FLASH_SendByte(0xAD);
	/* Send WriteAddr high nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	/* Send WriteAddr medium nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	/* Send WriteAddr low nibble address byte to write to */
	SPI_FLASH_SendByte(WriteAddr & 0xFF);

	
		/* Send the current byte */
		//SPI_FLASH_SendByte(value);

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *  (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *   to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *   or less than "SPI_FLASH_PAGESIZE" value.
  * @retval None
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	if(spi_flash_type == SST25VF016B)
	{
		int i;
		for(i=0; i<NumByteToWrite; i++)
		{
			SPI_FLASH_ByteWrite(WriteAddr,pBuffer[i]);
		}
		return;
	}

#ifdef ENABLE_FAST_SPI
	uint8_t value;
	value = value;
	// use spi fifo to faster send data
	/* Enable the write access to the FLASH */
  	SPI_FLASH_WriteEnable();

	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();

	/* Send "Write to Memory " instruction */
	SPI_FLASH_OnlySend(CMD_WRITE);
	/* Send WriteAddr high nibble address byte to write to */
	SPI_FLASH_OnlySend((WriteAddr & 0xFF0000) >> 16);
	/* Send WriteAddr medium nibble address byte to write to */
	SPI_FLASH_OnlySend((WriteAddr & 0xFF00) >> 8);
	/* Send WriteAddr low nibble address byte to write to */
	SPI_FLASH_OnlySend(WriteAddr & 0xFF);

	
	SPI_FLASH_OnlyRead(value);
	SPI_FLASH_OnlyRead(value);
	SPI_FLASH_OnlyRead(value);
	SPI_FLASH_OnlyRead(value);
	
	if(NumByteToWrite < 8)
	{
		int i;
		for(i=0; i<NumByteToWrite; i++)
		{
			SPI_FLASH_OnlySend(*pBuffer++);
		}
		for(i=0; i<NumByteToWrite; i++)
		{
			SPI_FLASH_OnlyRead(value);
		}
	}
	else
	{
		int i;
		SPI_FLASH_OnlySend(*pBuffer++);
		SPI_FLASH_OnlySend(*pBuffer++);
		SPI_FLASH_OnlySend(*pBuffer++);
		SPI_FLASH_OnlySend(*pBuffer++);
		SPI_FLASH_OnlySend(*pBuffer++);
		SPI_FLASH_OnlySend(*pBuffer++);
		SPI_FLASH_OnlySend(*pBuffer++);
		for(i=0; i<(NumByteToWrite-7); i++)
		{
			SPI_FLASH_OnlySend(*pBuffer++);
			SPI_FLASH_OnlyRead(value);
		}
		SPI_FLASH_OnlyRead(value);
		SPI_FLASH_OnlyRead(value);
		SPI_FLASH_OnlyRead(value);
		SPI_FLASH_OnlyRead(value);
		SPI_FLASH_OnlyRead(value);
		SPI_FLASH_OnlyRead(value);
		SPI_FLASH_OnlyRead(value);
	}
	
	 /* Deselect the FLASH: Chip Select high */
	 SPI_FLASH_CS_HIGH();
	
	 /* Wait the end of Flash writing */
	 SPI_FLASH_WaitForWriteEnd();

	

#else
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(CMD_WRITE);
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  /* while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
    SPI_FLASH_SendByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();

#endif
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *   WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *   to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PAGESIZE;
  count = SPI_FLASH_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PAGESIZE;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PAGESIZE;

  if (Addr == 0) /* WriteAddr is SPI_FLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PAGESIZE */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PAGESIZE);
        WriteAddr +=  SPI_FLASH_PAGESIZE;
        pBuffer += SPI_FLASH_PAGESIZE;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not SPI_FLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PAGESIZE;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PAGESIZE;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PAGESIZE);
        WriteAddr +=  SPI_FLASH_PAGESIZE;
        pBuffer += SPI_FLASH_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
#ifdef ENABLE_FAST_SPI
	uint8_t value;
	value = value;
	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();

	/* Send "Read from Memory " instruction */
	SPI_FLASH_OnlySend(CMD_READ);

	/* Send ReadAddr high nibble address byte to read from */
	SPI_FLASH_OnlySend((ReadAddr & 0xFF0000) >> 16);
	/* Send ReadAddr medium nibble address byte to read from */
	SPI_FLASH_OnlySend((ReadAddr& 0xFF00) >> 8);
	/* Send ReadAddr low nibble address byte to read from */
	SPI_FLASH_OnlySend(ReadAddr & 0xFF);
	

	SPI_FLASH_OnlyRead(value);
	SPI_FLASH_OnlyRead(value);
	SPI_FLASH_OnlyRead(value);
	SPI_FLASH_OnlyRead(value);
	if(NumByteToRead < 8)
	{
		int i;
		for(i=0; i<NumByteToRead; i++)
		{
			SPI_FLASH_OnlySend(DUMMY_BYTE);
		}
		for(i=0; i<NumByteToRead; i++)
		{
			SPI_FLASH_OnlyRead(*pBuffer++);
		}
	}
	else
	{
		int i;
		SPI_FLASH_OnlySend(DUMMY_BYTE);
		SPI_FLASH_OnlySend(DUMMY_BYTE);
		SPI_FLASH_OnlySend(DUMMY_BYTE);
		SPI_FLASH_OnlySend(DUMMY_BYTE);
		SPI_FLASH_OnlySend(DUMMY_BYTE);
		SPI_FLASH_OnlySend(DUMMY_BYTE);
		SPI_FLASH_OnlySend(DUMMY_BYTE);
		for(i=0; i<(NumByteToRead-7); i++)
		{
			SPI_FLASH_OnlySend(DUMMY_BYTE);
			SPI_FLASH_OnlyRead(*pBuffer++);
		}
		SPI_FLASH_OnlyRead(*pBuffer++);
		SPI_FLASH_OnlyRead(*pBuffer++);
		SPI_FLASH_OnlyRead(*pBuffer++);
		SPI_FLASH_OnlyRead(*pBuffer++);
		SPI_FLASH_OnlyRead(*pBuffer++);
		SPI_FLASH_OnlyRead(*pBuffer++);
		SPI_FLASH_OnlyRead(*pBuffer++);
	}

	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();

#else

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(CMD_READ);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(DUMMY_BYTE);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
#endif
}




/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(0x9F);

  /* Read a byte from the FLASH */
  Temp0 = SPI_FLASH_SendByte(DUMMY_BYTE);

  /* Read a byte from the FLASH */
  Temp1 = SPI_FLASH_SendByte(DUMMY_BYTE);

  /* Read a byte from the FLASH */
  Temp2 = SPI_FLASH_SendByte(DUMMY_BYTE);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/**
  * @brief  Initiates a read data byte (READ) sequence from the Flash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the Flash still being selected. With this
  *   technique the whole content of the Flash is read with a single READ instruction.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @retval None
  */
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(CMD_READ);

  /* Send the 24-bit address of the address to read from ---------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *   has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
uint8_t SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(DUMMY_BYTE));
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *   from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
#if USE_SOFT_SPI_DRIVER
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  return (uint8_t) SoftSpi_SendData(byte);
}

#else
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  return (uint8_t) SSP_SendData(SPI_FLASH_SSP_PORT, byte);
}
#endif



/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(CMD_WREN);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *   status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  //uint16_t dummy_time = 0xFFFF;
  uint8_t flashstatus = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(CMD_RDSR);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = SPI_FLASH_SendByte(DUMMY_BYTE);
	//dummy_time--;
  }  while ((flashstatus & WIP_FLAG) == SET); /* Write in progress */
  //while (dummy_time && (flashstatus & WIP_FLAG) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

uint8_t SPI_FLASH_ReadStatus(void)
{
  uint8_t flashstatus = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(CMD_RDSR);
  
  flashstatus = SPI_FLASH_SendByte(DUMMY_BYTE);
  SPI_FLASH_CS_HIGH();
  return flashstatus;
  
}

void SPI_FLASH_WriteStatus(uint8_t status)
{
	SPI_FLASH_WriteEnable();
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "Read Status Register" instruction */
	SPI_FLASH_SendByte(CMD_WRSR);

	SPI_FLASH_SendByte(status);
	
	SPI_FLASH_CS_HIGH();
	return ;
}

void SPI_FLASH_DisableWriteProtection(void)
{
	// disable write protection
	int status;
	status = SPI_FLASH_ReadStatus();
	if(status & (0x07<<2))
	{
		status &= (~(0x07<<2));
		SPI_FLASH_WriteStatus(status);
	}
}




/*******************************************************************************
* Function Name  : SPI_FLASH_SectorsMask
* Description    : Calculate the number of sectors
* Input          : - Size: The image size
* Output         : None
* Return         : The number of sectors
*******************************************************************************/
u32 SPI_FLASH_SectorsMask(u32 Size)
{
  u32 pagenumber = 0x0;
  u32 size = Size;

  if ((size % SPI_FLASH_SECTOR_SIZE) != 0)
  {
    pagenumber = (size / SPI_FLASH_SECTOR_SIZE) + 1;
  }
  else
  {
    pagenumber = size / SPI_FLASH_SECTOR_SIZE;
  }
  return pagenumber;

}

char checkFlash(void)
{
	char i,res = FALSE,rxBuffer[6] = {0xaa,0x35,0x36,0x8f,0xc2,0xba},read[6];
	
	SoftSpi_UsePort(SPI_PORT_B);
	SPI_FLASH_SetType(M25P16);
	u32 id = SPI_FLASH_ReadID();
	SPI_FLASH_BufferWrite(rxBuffer, 0x00, sizeof(rxBuffer));

	SPI_FLASH_BufferRead(read, 0x00, sizeof(rxBuffer));
	for(i = 0; i < sizeof(rxBuffer); i++)
	{
		if(read[i] != rxBuffer[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}

u32 id;

char checkFlash25Q128(void)
{
	char i,res = FALSE,rxBuffer[6] = {0xaa,0x35,0x36,0x8f,0xc2,0xba},read[6];
	
	SoftSpi_UsePort(SPI_PORT_C);
	SPI_FLASH_DisableWriteProtection();
	//SPI_FLASH_SetType(M25Q16);
	id = SPI_FLASH_ReadID();
	SPI_FLASH_SectorErase(0);
	SPI_FLASH_BufferWrite(rxBuffer, 0x00, sizeof(rxBuffer));

	SPI_FLASH_BufferRead(read, 0x00, sizeof(rxBuffer));
	for(i = 0; i < sizeof(rxBuffer); i++)
	{
		if(read[i] != rxBuffer[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}



void SPI_Flash_Read_Test(void)
{
	u8 Buffer[1024];
	u16 sectorNumber;
	int i,ii;

	DebugMessageFlash("%s\r\n",__func__);	
	
	DebugMessageFlash("FPGA M25P16 Flash addr 0x00 Check:\r\n");

#if 0
	SoftSpi_UsePort(SPI_PORT_A);//20140806
	FPGA_Program_Start(); //20140729
	SysDelay1ms(10);
	
	sectorNumber = SPI_FLASH_SectorsMask(1024*10);
	for(i=0; i<sectorNumber; i++)
	{
		SPI_FLASH_SectorErase(i*SPI_FLASH_SECTOR_SIZE);
	}
	
	
	for(i = 0;i < 10;i ++)
	{
		for(ii = 0;ii < 1024;ii ++)
			Buffer[ii] = ii;
		SPI_FLASH_BufferWrite(Buffer, i*1024,1024); //20140729
	}
	for(ii = 0;ii < 1024;ii ++)
		Buffer[ii] = 0;
	
	for(i = 0;i < 10;i ++)
	{
		SPI_FLASH_BufferRead(Buffer,i*1024,1024);
				
		for(ii = 0;ii < 1024;ii ++)
		{
			if(i%16 == 15)
				printf("\r\n");
			else
				printf("%x ",Buffer[ii]);
		}
		DebugMessageFlash("\r\n");
	}
	FPGA_Program_Done();	
#endif

	DebugMessageFlash("9320 M25P16 Flash addr 0x00 Check:\r\n");
	SoftSpi_UsePort(SPI_PORT_B);
	SysDelay1ms(10);
#if 0
	sectorNumber = SPI_FLASH_SectorsMask(1024*10);
	for(i=0; i<sectorNumber; i++)
	{
		SPI_FLASH_SectorErase(i*SPI_FLASH_SECTOR_SIZE);
	}
	
	for(i = 0;i < 10;i ++)
	{
		for(ii = 0;ii < 1024;ii ++)
			Buffer[ii] = ii;
		SPI_FLASH_BufferWrite(Buffer, i*1024,1024); //20140729
	}
#endif
	for(ii = 0;ii < 1024;ii ++)
		Buffer[ii] = 0;
	
	for(i = 0;i < 5;i ++)
	{
		SPI_FLASH_BufferRead(Buffer,i*1024,1024);
//		for(ii = 0;ii < 1024;ii ++)
//		{
//			if(ii%16 == 15)
//				printf("02X\r\n");
//			else
//				printf("%02X ",Buffer[ii]);
//		}
		DebugMessageFlash("\r\n");
	}

}

void SPI_Flash_EraseA_Test(void)
{
	u8 Buffer[1024];
	u16 sectorNumber;
	int i,ii;

	DebugMessageFlash("%s\r\n",__func__);	
	
	DebugMessageFlash("FPGA M25P16 Flash addr 0x00 Check:\r\n");

#if 1
	SoftSpi_UsePort(SPI_PORT_A);//20140806
	FPGA_Program_Start(); //20140729
	SPI_FLASH_SetType(M25P16);
	SysDelay1ms(10);
	
	sectorNumber = SPI_FLASH_SectorsMask(1024*10);
	for(i=0; i<sectorNumber; i++)
	{
		SPI_FLASH_SectorErase(i*SPI_FLASH_SECTOR_SIZE);
	}
		
		
	for(i = 0;i < 10;i ++)
	{
		for(ii = 0;ii < 1024;ii ++)
			Buffer[ii] = ii;
		SPI_FLASH_BufferWrite(Buffer, i*1024,1024); //20140729
	}
	for(ii = 0;ii < 1024;ii ++)
		Buffer[ii] = 0;
	
	for(i = 0;i < 10;i ++)
	{
		SPI_FLASH_BufferRead(Buffer,i*1024,1024);
		for(ii = 0;ii < 1024;ii ++)
		{
//			if(ii%16 == 15)
//				printf("%02X\r\n",Buffer[ii]);
//			else
//				printf("%02X ",Buffer[ii]);
		}
		DebugMessageFlash("\r\n");

	}
	FPGA_Program_Done();	
#endif

#if 0
		DebugMessageFlash("9320 M25P16 Flash addr 0x00 Check:\r\n");
		SoftSpi_UsePort(SPI_PORT_B);
		SysDelay1ms(10);

		sectorNumber = SPI_FLASH_SectorsMask(1024*10);
		for(i=0; i<sectorNumber; i++)
		{
			SPI_FLASH_SectorErase(i*SPI_FLASH_SECTOR_SIZE);
		}
		
		for(i = 0;i < 10;i ++)
		{
			for(ii = 0;ii < 1024;ii ++)
				Buffer[ii] = ii;
			SPI_FLASH_BufferWrite(Buffer, i*1024,1024); //20140729
		}

		for(ii = 0;ii < 1024;ii ++)
			Buffer[ii] = 0;
		
		for(i = 0;i < 10;i ++)
		{
			SPI_FLASH_BufferRead(Buffer,i*1024,1024);
			for(ii = 0;ii < 1024;ii ++)
			{
//				if(ii%16 == 15)
//					printf("02X\r\n",Buffer[ii]);
//				else
//					printf("%02X ",Buffer[ii]);
			}
			DebugMessageFlash("\r\n");
		}
	#endif
	
}

void SPI_Flash_EraseB_Test(void)
{
	u8 Buffer[1024];
	u16 sectorNumber;
	int i,ii;

	DebugMessageFlash("%s\r\n",__func__);	
			

	DebugMessageFlash("2796 28Q80 Flash addr 0x00 Check:\r\n");
	SoftSpi_UsePort(SPI_PORT_C);
	SPI_FLASH_SetType(M25Q16);
	SysDelay1ms(10);

	sectorNumber = 5;//SPI_FLASH_SectorsMask(1024*10);
	for(i=0; i<sectorNumber; i++)
	{
		SPI_FLASH_SectorErase(i*SPI_FLASH_SECTOR_SIZE);
	}
	
	for(i = 0;i < 5;i ++)
	{
		for(ii = 0;ii < 1024;ii ++)
			Buffer[ii] = ii;
		SPI_FLASH_BufferWrite(Buffer, i*1024,1024); //20140729
	}

	for(ii = 0;ii < 5;ii ++)
		Buffer[ii] = 0;
	
	for(i = 0;i < 5;i ++)
	{
		SPI_FLASH_BufferRead(Buffer,i*1024,1024);
		for(ii = 0;ii < 1024;ii ++)
		{
//			if(ii%16 == 15)
//				printf("%02X\r\n",Buffer[ii]);
//			else
//				printf("%02X ",Buffer[ii]);
		}
		DebugMessageFlash("\r\n");
	}

}



