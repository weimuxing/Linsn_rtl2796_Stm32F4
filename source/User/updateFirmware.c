#include <stdio.h>
#include "uart.h"
#include "SysConfig.h"


#include "I2C.h"
#include "spi_flash.h"

#include "FPGA.h"

#include "menuData.h"
#include "menu.h"

#include "updateFirmware.h"
#include "rs232command.h"

#include "systick.h"
#include "input.h"
//#include "output.h"
//#include "WIFI.h"
//#include "frontBoard.h"
#include "ff.h"
#include "fatfs.h"


//FIL file;

//程序打包方式：
//Transform Style		Device ID 	程序文件
//MCU			0		通讯板207程序
//MCU			1		通讯板103输入板程序
//MCU			2		通讯板103输出板程序
//MCU			3		DVI输入板207程序
//MCU			4		DVI输出板207程序
//MCU			5		182输入板103程序
//MCU			6		182输入板207程序
//MCU			7		Double Link DVI输入板
//MCU			8		前面板 103程序
//MCU			9		WIFI 103程序
//MCU			10		HDMI输入直通板103C8程序(0x3206)

extern  void Reboot(void); // rs232command.c


#define UPDATE_BUFFER_SIZE 1024
u8 updateBuffer[UPDATE_BUFFER_SIZE];
int updateSize = 0;
u32 totalTime = 0;
char updateState = 0xff;
char updateGuid = UNDEFINE_GUID;
char updateDeviceId = 0;
int updateFileSize = 0;
int updateFileIndex = 0;
int update_version = 0;

const u8 _guidList[GUID_MAX_SIZE][16] =
{
	{0x57, 0x59, 0xAD, 0xA8, 0x5C, 0xBB, 0x40, 0x77, 0x92, 0xD9, 0x8D, 0xFB, 0x63, 0x14, 0x5A, 0xE7}, // mcu
	{0x85, 0xF7, 0x7B, 0x73, 0x36, 0x7B, 0x24, 0xC9, 0xB9, 0xDA, 0x73, 0x2A, 0xDF, 0x4A, 0x23, 0xAE}, // video
	{0x62, 0x05, 0x7F, 0x97, 0x28, 0x63, 0x45, 0xEC, 0x8A, 0x7B, 0xD2, 0xB3, 0x8D, 0x90, 0x14, 0xEC}, // fpga
	{0x14, 0xDF, 0xFE, 0x0B, 0xC3, 0xCA, 0x4D, 0xCE, 0xB7, 0xD2, 0xAB, 0xA0, 0x38, 0x53, 0x9E, 0x14}, // font
	
};


int GetTypeGUID(u8 * guid)
{
	int i;
	for(i=0; i<GUID_MAX_SIZE; i++)
	{
		int j;
		int isSame = 1;
		
		for(j=0; j<16; j++)
		{
			if(guid[j] != _guidList[i][j])
			{
				isSame = 0;
				break;
			}
		}
		if(isSame)
		{
			return i;
		}
	}
	printf("undefine guid: %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x \r\n", 
		guid[0],guid[1],guid[2],guid[3],guid[4],guid[5],guid[6],guid[7]);
	return UNDEFINE_GUID;
}

void GetFileHeaderInfo(u8 *updateBuffer)
{
	updateDeviceId = updateBuffer[0x34];
	updateFileSize = (updateBuffer[0x30]<<24);
	updateFileSize += (updateBuffer[0x31]<<16);
	updateFileSize += (updateBuffer[0x32]<<8);
	updateFileSize += (updateBuffer[0x33]);	

	update_version =  (updateBuffer[0x40]<<8);
	update_version +=  (updateBuffer[0x41]);
	
}


//static U32 spiIndex = 0;
static U32 fileEraseSize = 0;
//static U32 startFileTime = 0;
//static u16 blockIndex = 0;
//static char isSameTypeDevice = 0;
static char updateError = 0;
extern char bUpdateMCU;


void StartProgramFont(int devId)
{
	printf("%s\r\n", __func__);
	SysDelay1ms(50);

	SoftSpi_UsePort(SPI_PORT_C);

	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_START);
	SPI_FLASH_DisableWriteProtection();
	
	SPI_FLASH_SetType(M25P16);
	SPI_FLASH_SectorErase(0);//fileIndex
	menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, 0);

}

void ProgramFont(int devId, int address, u8 *buffer, int size)
{
	while ((address + size)>fileEraseSize)
	{
		SPI_FLASH_SectorErase(fileEraseSize);
		fileEraseSize += SPI_FLASH_SECTOR_SIZE;
	}
	SPI_FLASH_BufferWrite(buffer,address,size);
	printf("Video update addr=%8d\r",address);
	menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, address*100/updateFileSize);
}

void EndProgramFont(int devId)
{
	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_SUCCESS);

	printf("\n%s\r\n", __func__);

}

void UartPinConfig(u8 uartport);

void EntryProgramMcu(void)
{
	BYTE i;
		
	printf("EntryProgramMcu,Will Reboot!\r\n");

	//UartPinConfig(1);
	resetUserPadLock();
	
	FLASH_Lock();
	/* Flash unlock */
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
		  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	if(FLASH_ProgramWord(0x8000000+0x80000-0x08, 0x19820426) == FLASH_COMPLETE)
	{
		//printf("program success\r\n  ");
	}
	else
	{
		//printf("program error\r\n  ");
	}
	
	// disable all interrupt //防止跳转的时候死机
	for(i=0; i<8; i++)
	{
		NVIC->ICER[i] = 0xffffffff;
	}
//	Jump_To_Bootloader();
	
	printf("\r\nJump to Bootloader failed");
}



void StartProgramFpga(int devId)
{
	int sectorNumber;
	u8 i;

	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_START);

	printf("%s\r\n", __func__);
	FPGA_Program_Start();
	SysDelay1ms(50);

	SoftSpi_UsePort(SPI_PORT_A);

	SPI_FLASH_DisableWriteProtection();
	
	SPI_FLASH_SetType(SST25VF016B);

	sectorNumber = SPI_FLASH_SectorsMask(updateFileSize);
	fileEraseSize = updateFileSize;
	//printf("sectorNumber = %d\r\n",sectorNumber);
	for(i=0; i<sectorNumber; i++)
	{
		//printf("SPI_FLASH_SectorErase(%d)\r\n",i);
		SPI_FLASH_SectorErase(i*SPI_FLASH_SECTOR_SIZE);
		SysDelay1ms(50);
	}	

	//printf("program size = %d\r\n",updateSize);
	menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, 0);

}



void ProgramFpga(int devId, int address, u8 *buffer, int size)
{

	while ((address + size)>fileEraseSize)
	{
		SPI_FLASH_SectorErase(fileEraseSize);
		fileEraseSize += SPI_FLASH_SECTOR_SIZE;
	}
	SPI_FLASH_BufferWrite(buffer,address,size);
	printf("FPGA update addr=%d\r",address);
	//percent = (address+size)*100/updateFileSize;
	menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, address*100/updateFileSize);
}

void EndProgramFpga(int devId)
{
	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_SUCCESS);
	printf("%8x\n\r\n", __func__);
	FPGA_Program_Done();
}

void StartProgramVideo(int devId)
{
	printf("%s\r\n", __func__);
	SysDelay1ms(50);

	SoftSpi_UsePort(SPI_PORT_B);

	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_START);
	SPI_FLASH_DisableWriteProtection();
	
	SPI_FLASH_SetType(M25P16);
	// write file header to spi flash
	//SPI_FLASH_BufferWrite(rxBuffer+128, FPGA_FLASH_ADDRESS_OFFSET, UPDATE_BlOCK_SIZE-128);

	//spiIndex = 0;
	SPI_FLASH_SectorErase(0);//fileIndex
	//fileEraseSize = SPI_FLASH_SECTOR_SIZE;	

	//printf("program size = %d\r\n",updateSize);
	menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, 0);
}

void ProgramVideo(int devId, int address, u8 *buffer, int size)
{
	//static char prePercent = 0;
	//char percent = 0;

	while ((address + size)>fileEraseSize)
	{
		SPI_FLASH_SectorErase(fileEraseSize);
		fileEraseSize += SPI_FLASH_SECTOR_SIZE;
	}
	SPI_FLASH_BufferWrite(buffer,address,size);
	printf("Video update addr=%8d\r",address);
	//percent = (address+size)*100/updateFileSize;

	menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, address*100/updateFileSize);
}

void EndProgramVideo(int devId)
{
	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_SUCCESS);

	printf("\n%s\r\n", __func__);
}



void ProcessUpdateBuffer(void)
{
	switch(updateState)
	{
	case 0:
		// get file header, file size, file type
		if(updateSize < 128)
		{
			return;
		}
		updateGuid = GetTypeGUID(updateBuffer);
		if(updateGuid == UNDEFINE_GUID)
		{
			updateSize = 0;
		}
		else
		{
			GetFileHeaderInfo(updateBuffer);
             
			fileEraseSize = 0;
			updateState = 1;
			ProcessUpdateBuffer();			
		}		
		break;
	case 1:
		// process file header
		updateFileIndex = 0;
		
		if(updateGuid == GUID_IDX_FPGA)
		{
			int i;			
			
			updateSize -= 128; // remove file header
			for(i=0; i<updateSize; i++)
			{
				updateBuffer[i] = updateBuffer[i+128];
			}
			StartProgramFpga(updateDeviceId);
			updateState = 2;	
		}	
		else if(updateGuid == GUID_IDX_MCU)
		{
			if(updateSize == 1024)
			{
				//StartProgramMcu(updateDeviceId);

				if(bUpdateMCU == FALSE)
				{
					bUpdateMCU = TRUE;
				}
				else
				{
					menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_WAIT_AUTO_REBOOT);
					SysDelay1ms(500);
					EntryProgramMcu();
				}				

				updateFileIndex = 1024-128;
				
				updateSize = 0;		
				
				updateState = 3;
			}
		}
		else if(updateGuid == GUID_IDX_FONT)
		{
			if(updateSize == 1024)
			{
				StartProgramFont(updateDeviceId);

				updateFileIndex = 1024-128;
				
				updateSize = 0;		
				
				updateState = 4;
			}
		}
		else if(updateGuid == GUID_IDX_VIDEO)
		{
			int i;			
			
			updateSize -= 128; // remove file header
			for(i=0; i<updateSize; i++)
			{
				updateBuffer[i] = updateBuffer[i+128];
			}
			StartProgramVideo(updateDeviceId);
			updateState = 5;	
		}			
		else 
		{
			updateState = 0;
		}
				
		break;
	case 2:  // process fpga program
		if((updateFileIndex+updateSize) < updateFileSize)
		{
			if((updateSize % 256) == 0)
			{
				ProgramFpga(updateDeviceId, updateFileIndex, updateBuffer, updateSize);
				updateFileIndex += updateSize;
				updateSize = 0;
			}
		}
		else  // get file end
		{
			int remainSize;
			int i;
			
			remainSize = (updateFileSize-updateFileIndex);
			
			ProgramFpga(updateDeviceId, updateFileIndex, updateBuffer, remainSize);

			EndProgramFpga(updateDeviceId);
                                      
			/*************************************************************2015-7-16 徐帆添加***************************************************************/
			updateFileIndex += updateSize;
			/*************************************************************2015-7-16 徐帆添加***************************************************************/

			updateSize -= remainSize;
			
			for(i=0; i<updateSize; i++)
			{
				updateBuffer[i] = updateBuffer[remainSize+i];
			}
			updateState = 0;
			
		}
		break;
	case 3: // process mcu program
		
		if((updateFileIndex+updateSize) < updateFileSize)
		{
			if((updateSize == 1024))
			{
				//ProgramMcu(updateDeviceId, updateFileIndex, updateBuffer, updateSize);
				updateFileIndex += updateSize;
				updateSize = 0;
			}
		}
		else  // get file end
		{
			int remainSize;
			int i;
			
			remainSize = (updateFileSize-updateFileIndex);
			
			//ProgramMcu(updateDeviceId, updateFileIndex, updateBuffer, remainSize);

			//EndProgramMcu(updateDeviceId);
                                     
			/*******************************2015-7-16 徐帆添加***************************************/
			updateFileIndex += updateSize;
			/******************************2015-7-16 徐帆添加******************************************/

			updateSize -= remainSize;

			printf("updateSize = %d\r\n", updateSize);
			
			for(i=0; i<updateSize; i++)
			{
				updateBuffer[i] = updateBuffer[remainSize+i];
			}
			updateState = 0;
			
		}
		break;
	case 4: // process font program
		if((updateFileIndex+updateSize) < updateFileSize)
		{
			if((updateSize % 256) == 0)
			{
				ProgramFont(updateDeviceId, updateFileIndex, updateBuffer, updateSize);
				updateFileIndex += updateSize;
				updateSize = 0;
			}
		}
		else  // get file end
		{
			int remainSize;
			int i;
			
			remainSize = (updateFileSize-updateFileIndex);
			
			ProgramFont(updateDeviceId, updateFileIndex, updateBuffer, remainSize);

			EndProgramFont(updateDeviceId);
                                      
			/*************************************************************2015-7-16 徐帆添加***************************************************************/
			updateFileIndex += updateSize;
			/*************************************************************2015-7-16 徐帆添加***************************************************************/

			updateSize -= remainSize;
			
			for(i=0; i<updateSize; i++)
			{
				updateBuffer[i] = updateBuffer[remainSize+i];
			}
			updateState = 0;
			
		}
		
		break;

	case 5:  // process Video program
		if((updateFileIndex+updateSize) < updateFileSize)
		{
			if((updateSize % 256) == 0)
			{
				ProgramVideo(updateDeviceId, updateFileIndex, updateBuffer, updateSize);
				updateFileIndex += updateSize;
				updateSize = 0;
			}
		}
		else  // get file end
		{
			int remainSize;
			int i;
			
			remainSize = (updateFileSize-updateFileIndex);
			
			ProgramVideo(updateDeviceId, updateFileIndex, updateBuffer, remainSize);

			EndProgramVideo(updateDeviceId);
                                      
			/*************************************************************2015-7-16 徐帆添加***************************************************************/
			updateFileIndex += updateSize;
			/*************************************************************2015-7-16 徐帆添加***************************************************************/

			updateSize -= remainSize;
			
			for(i=0; i<updateSize; i++)
			{
				updateBuffer[i] = updateBuffer[remainSize+i];
			}
			updateState = 0;
			
		}
		break;		
	default:
		updateState = 0;
		break;
	}
}


void StartUpdateProgram(void)
{
	
	updateSize = 0;
	updateState = 0;
	updateGuid = UNDEFINE_GUID;
	updateDeviceId = 0;
	totalTime = GetSysTick();
}


void UpdateProgramData(u8 *buffer, int size)
{
	int i;

	if(size == 0)
	{
		return;
	}
	
	if((updateSize + size) <= UPDATE_BUFFER_SIZE)
	{
		for(i=0; i<size; i++)
		{
			updateBuffer[updateSize++] = buffer[i];
		}
		size = 0;
	}
	else
	{
		int remain = UPDATE_BUFFER_SIZE - updateSize;
		for(i=0; i<remain; i++)
		{
			updateBuffer[updateSize++] = buffer[i];
		}
		size = size - remain;
		buffer += remain;
	}
	ProcessUpdateBuffer();
        
	if(size)
	{
		UpdateProgramData(buffer, size);
	}
}
void EndUpdateProgram(void)
{
	UpdateProgramData(updateBuffer, updateSize);
	updateState = 0xff;
	
}

int GetUpdateState(void)
{
	return updateState;
}

int GetUpdateError(void)
{
	return updateError;
}



