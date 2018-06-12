#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

/* Includes ------------------------------------------------------------------*/
#define USE_SOFT_SPI_DRIVER 1


#if USE_SOFT_SPI_DRIVER
#include "Soft_Spi.h"
#else
#include "LPC2368_SSP.h"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */

// spi flash type define
#define M25P40 1
#define M25P16 2
#define M25P64 3
#define M25Q16 4
#define SST25VF016B 5

#define SPI_FLASH_TYPE M25P16   //100M for Venus X1 

#if (SPI_FLASH_TYPE == M25P40)

	#define SPI_FLASH_SECTOR_SIZE (0x10000)
	#define SPI_FLASH_SIZE (0x80000)
	#define SPI_FLASH_ID (0x202013)
	
#elif (SPI_FLASH_TYPE == M25P16)
	
	#define SPI_FLASH_SECTOR_SIZE (0x10000)
	#define SPI_FLASH_SIZE (0x200000)
	#define SPI_FLASH_ID (0x202015)

#elif (SPI_FLASH_TYPE == M25Q16)

	#define SPI_FLASH_SECTOR_SIZE (0x10000)
	#define SPI_FLASH_SIZE (0x200000)
	#define SPI_FLASH_ID (0x202015)	// 0xef4015

#elif (SPI_FLASH_TYPE == M25P64)

	#define SPI_FLASH_SECTOR_SIZE (0x10000)
	#define SPI_FLASH_SIZE (0x800000)
	#define SPI_FLASH_ID (0x202017)

#elif (SPI_FLASH_TYPE == SST25VF016B)
	#define SPI_FLASH_SECTOR_SIZE (0x10000)
	#define SPI_FLASH_SIZE (0x200000)
	#define SPI_FLASH_ID (0xBF2541)

#endif

//25P16 in Venus X1 
//free           0x000000~0x07ffff
//font           0x080000~0x0cffff
//logo          0x0d0000~0x0dffff
//pic            0x0e0000~0x0fffff
//user code  0x100000~0x1fffff

#define SPI_FLASH_FONT_OFFSET     (0x1ff000)
//#define SPI_FLASH_FONT_OFFSET     (0x080000)
#define SPI_FLASH_LOGO_OFFSET     (0x0D0000)
#define SPI_FLASH_PIC_OFFSET      (0x0E0000)
#define SPI_FLASH_USERCODE_OFFSET (0x100000)   


/* Exported macro ------------------------------------------------------------*/
#if USE_SOFT_SPI_DRIVER

/* Select SPI FLASH: Chip Select pin low  */
#define SPI_FLASH_CS_LOW()       SoftSpi_CS_Low()
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()      SoftSpi_CS_High()

#else

/* Select SPI FLASH: Chip Select pin low  */
#define SPI_FLASH_CS_LOW()       SSP0_SEL_LOW()
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()      SSP0_SEL_HIGH()

#endif


/* SPI Flash supported commands */
#define CMD_WRITE     0x02  /* Write to Memory instruction */
#define CMD_WRSR      0x01  /* Write Status Register instruction */
#define CMD_WREN      0x06  /* Write enable instruction */
#define CMD_READ      0x03  /* Read from Memory instruction */
#define CMD_RDSR      0x05  /* Read Status Register instruction  */
#define CMD_RDID      0x9F  /* Read identification */
#define CMD_SE        0xD8  /* Sector Erase instruction */
#define CMD_SE2       0x20  /* Sector Erase instruction */
#define CMD_BE        0xC7  /* Bulk Erase instruction */

#define WIP_FLAG      0x01  /* Write In Progress (WIP) flag */

#define DUMMY_BYTE    0xA5


/* Exported functions --------------------------------------------------------*/
/*----- High layer function -----*/
void SPI_FLASH_Init(SpiPortTypeDef SPI_PORT_X);
void SPI_FLASH_SetType(int type);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint8_t SPI_FLASH_ReadStatus(void);
void SPI_FLASH_WriteStatus(uint8_t status);
void SPI_FLASH_DisableWriteProtection(void);


uint32_t SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);

/*----- Low layer function -----*/
uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
u32 SPI_FLASH_SectorsMask(u32 Size);

char checkFlash(void);
char checkFlash25Q128(void);
void SPI_Flash_Read_Test(void);
void SPI_Flash_EraseA_Test(void);
void SPI_Flash_EraseB_Test(void);


#endif // _SPI_FLASH_H_

