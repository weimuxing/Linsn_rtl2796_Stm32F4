#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "rs232command.h"
#include "ff.h"
#include "update_from_fatfs.h"
#include "updateFirmware.h"
#include "menu.h"
#include "systick.h"
#include "control.h"
//#include "ff_gen_drv.h"
#include "diskio.h"	
#include "fatfs.h"
//#include "usbh_diskio.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "userPref.h"
#include "I2CEEPROM.h"
#include "spi_flash.h"
#include "fpga.h"
#include "RTD2796.h"
#include "UartDebug.h"






//#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
//  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
//    #pragma data_alignment=4   
//  #endif
//#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
//__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

//#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
//  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
//    #pragma data_alignment=4   
//  #endif
//#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
//__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;

extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
USBH_HOST  USB_Host;



char USBDISKPath[4];          /* USB Host logical drive path */
char *updatePath = NULL;
char bUpdateMCU = false;
u8 IapBuf[PROGRAM_SIZE];
u8 IapBufCmp[PROGRAM_SIZE];
BYTE bUsbUpdateAsk = 0xFF;
BYTE bUsbUpdateFlag = FALSE;
FileIndex fileIndex;
FileHead fileHead;
fwPackInfo fwPack_Info[10];
FIL my_FIL;
FIL my_toFIL;
FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
FILINFO fileinfo;
DIR dir;

int cmpExtensionName(const char *fileName, const char * extensionName)  // return 0 -- same, 1 -- different
{
	int nameLength, extensionLength;
	extensionLength = strlen(extensionName);
	nameLength = strlen(fileName);
	return strcmp(fileName+nameLength-extensionLength, extensionName);
}

char updateFileName[16];
//遍历文件
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */	  

    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出						   
        	fn = fileinfo.fname;                                             /* It is a file. */
			if(cmpExtensionName(fn, ".PKG") == 0)
			{
				//printf("%s/", path);//打印路径	
				printf("%s\r\n",  fn);//打印文件名	
				sprintf(updateFileName, "%s", fn);
				f_closedir(&dir);
				return 1;
			}
		} 
    }
	
	f_closedir(&dir);
    return 0;	  
}

bool hasUpdateFile(char *Path)
{
	updatePath = Path;
	
	return (bool)mf_scan_files((u8*)Path);
}

u8* GetWriteBufAddr(void)
{
	return IapBuf;
}

u8* GetReadBufAddr(void)
{
	return IapBufCmp;
}



int copyFile(const char * from, const char * to)
{
	int res;
	char buffer[1024*4];
	UINT readSize;
	UINT writeSize;
	// 1. open file
	res = f_open(&my_FIL, from, FA_READ);
	if(res != FR_OK)
	{
		printf("open file %s error\r\n", from);
		return 1;
	}
	res = f_open(&my_toFIL, to, FA_WRITE|FA_CREATE_ALWAYS);
	if(res != FR_OK)
	{
		f_close(&my_FIL);
		printf("open file %s error\r\n", to);
		return 2;
	}
	// 2. read and write file
	while(1)
	{
	if(f_read(&my_FIL, buffer, sizeof(buffer), &readSize) == FR_OK)
	{
		//printf("read size = %d\r\n", readSize);
		if(readSize == 0)
		{
			f_close(&my_FIL);
			f_close(&my_toFIL);
			return FR_OK;
		}
		res = f_write(&my_toFIL, buffer, readSize, &writeSize);
		if(res == FR_OK)
		{
		}
		else
		{
			f_close(&my_FIL);
			f_close(&my_toFIL);
			printf("Write Error %d\r\n", res);
			return 3;
		}
	}
	else
	{
		f_close(&my_FIL);
		f_close(&my_toFIL);
		return FR_OK;
	}
	}
	
}


//#############################################################################################################
//
//char GetFwPackInfo(char *updateFileName,fwPackInfo *packInfo,const char * FileType)
char GetFwPackInfo(void)
{

   // FIL f_file;
	UINT bw;
    FRESULT res;
	u8 i;
//    u64 FileHeadAddr;
    BYTE verh,verl;    
    memset((u8*)&fileHead,0,sizeof(fileHead));
	res = f_read(&my_FIL,(u8*)&fileHead,sizeof(fileHead), &bw);
	if (res != FR_OK)
	{
		f_close(&my_FIL);
		//STMFLASH_Lock();
		DebugMessageFatfs("Read file error !\r\n");
	  	return UPDATE_ERROR_LOAD_FILE;
	}

	for(i = 0;i < fileHead.FileNum;i++)
	{

		fwPack_Info[i].fileId = fileHead.Index[i].FileId;
		fwPack_Info[i].fileSize = fileHead.Index[i].FileLen;
		strcpy(fwPack_Info[i].fileType,fileHead.Index[i].FileType);
		fwPack_Info[i].fileOffset = fileHead.Index[i].FileLocation;
		verh = fileHead.Index[i].FileVer[0]-'0';
		verl = fileHead.Index[i].FileVer[2]-'0';
		verl = (verl << 4) + (fileHead.Index[i].FileVer[3]-'0');
		verl = verl/16*10 + verl%16;
		fwPack_Info[i].FileVersion = verh;
		fwPack_Info[i].FileVersion = (fwPack_Info[i].FileVersion << 8) | verl;
	}
    return TRUE;
    
}


//#############################################################################################################
//
//系统软复位   
void Bsp_Soft_Reset(void)
{	  
	__set_FAULTMASK(1);   /* 关闭所有中断 */  
	NVIC_SystemReset();   /* 系统复位 */
} 

#define USER_APP_UPDATE_FLAG_ADDR ((u32)0x08080000 - 4)
#define USER_APP_UPDATE_FLAG 0xcccccccc


//#############################################################################################################
//UpdateAppFile
//UPDATE_Status UpdateAppFile(char *updateFileName,fwPackInfo *packInfo,const char * FileType)
UPDATE_Status UpdateAppFile(char *updateFileName)
{
	//FIL f_file;
	UINT bw;
	FRESULT res;
//	FLASH_Status f_status;
	vu32 ProgAddr = 0;
    vu32 BaseAddr = 0;
	int i;
//	u32 updateFinishId = 0;
	u32 sectorSize;
	int fileNums = 0;
	u16 ver;
	u8 verh;
	u8 updateFlag = 0;

	res = f_open(&my_FIL,updateFileName, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (res != FR_OK)
	{
		f_close(&my_FIL);
		DebugMessageFatfs("Open file error !\r\n");
	  	return UPDATE_ERROR_LOAD_FILE;
	}

	GetFwPackInfo();
	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_START);
	for(fileNums = 0;fileNums < fileHead.FileNum;fileNums++)
	{
		if(strcmp(fwPack_Info[fileNums].fileType,"FPGA") == 0)
		{
			verh = FPGA_get(FPGA_REG_VersionNum);
			ver = (verh >> 4) & 0xf;
			ver = (ver << 8) | (verh & 0xf);
			if(fwPack_Info[fileNums].FileVersion == ver)
			{
				DebugMessageFatfs("Fpga Firmware no find !\r\n");
				//updateFlag = 0;
				continue;
			}
			DebugMessageFatfs("USBH Update fpga...\r\n");
			updateFlag = 1;
			FPGA_Program_Start();
			RTD2796StopRun();
			SPI_FLASH_Init(SPI_PORT_A);
			
			sectorSize = 65536;
		}
		else if(strcmp(fwPack_Info[fileNums].fileType,"Video") == 0)
		{
			//ver = RTDVIDEO_VERSION_HIGH;
			//ver = (ver << 8) | RTDVIDEO_VERSION_LOW;
			ver = GetRtd2796Ver();
			if(fwPack_Info[fileNums].FileVersion == ver)
			{
				DebugMessageFatfs("Video Firmware no find !\r\n");
				//updateFlag = 0;
				continue;
			}
			DebugMessageFatfs("USBH Update video...\r\n");
			updateFlag = 1;
			RTD2796StopRun();
			FPGA_Program_Start();
			SPI_FLASH_Init(SPI_PORT_B);
			SPI_FLASH_DisableWriteProtection();
			sectorSize = 65536;
		}
		else if(strcmp(fwPack_Info[fileNums].fileType,"Font") == 0)
		{
			#if 0
			ver = FONT_VERSION_HIGH;
			ver = (ver << 8) | FONT_VERSION_LOW;
			if(fwPack_Info[fileNums].FileVersion == ver)
			{
				DebugMessageFatfs("Font Firmware no find !\r\n");
				//updateFlag = 0;
				continue;
			}
			#endif
			DebugMessageFatfs("USBH Update font...\r\n");
			updateFlag = 1;
			SPI_FLASH_Init(SPI_PORT_C);
			sectorSize = 65536;
		}
		else if(strcmp(fwPack_Info[fileNums].fileType,"MCU") == 0)
		{
			#if 0
			ver = SOFT_VERSION_HIGH;
			ver = (ver << 8) | SOFT_VERSION_LOW;
			if(fwPack_Info[fileNums].FileVersion == ver)
			{
				DebugMessageFatfs("Mcu Firmware no find !\r\n");
				//updateFlag = 0;
				continue;
			}
			#endif
			DebugMessageFatfs("USBH Update MCU...\r\n");
			updateFlag = 1;
			if(FLASH_ProgramWord(USER_APP_UPDATE_FLAG_ADDR,USER_APP_UPDATE_FLAG) == FLASH_COMPLETE)
			{
				DebugMessageFatfs("System restart...\r\n");
			}
			
			LCD_Clear();
			LCD_WriteLine2("Update firmwaer ...");
			LCD_WriteLine3("Please wait the device reboot ...");
			Bsp_Soft_Reset();
			
			//Bsp_Soft_Reset();
		}
		else
		{
			memset((u8*)fwPack_Info,0,sizeof(fwPack_Info));
			DebugMessageFatfs("No update fw find...\r\n");
			return UPDATE_ERROR_LOAD_FILE;
		}

		f_lseek(&my_FIL,0);
	    f_lseek(&my_FIL,fwPack_Info[fileNums].fileOffset);
		
		//#####################################################################
		//擦除扇区
		for(i = 0;i < ((fwPack_Info[fileNums].fileSize)/sectorSize)+1;i++)
		{
			SPI_FLASH_SectorErase(i*sectorSize);
			DebugMessageFatfs("Erase sector %d ok ..\r\n",i);
			menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, i*sectorSize*100/fwPack_Info[fileNums].fileSize);

		}
		
		DebugMessageFatfs("Program start ...\r\n");
		ProgAddr = 0;
		BaseAddr = 0;
		menuProcessEventValue(MENU_EVENT_PROGRESS_BAR,0);
		while((ProgAddr - BaseAddr) < (fwPack_Info[fileNums].fileSize))
		{
			memset(IapBuf,0,PROGRAM_SIZE);
			//res = f_read(&f_file,IapBuf,PROGRAM_SIZE, &bw);
			res = f_read(&my_FIL,IapBuf,sizeof(IapBuf), &bw);
			if (res != FR_OK)
			{
				f_close(&my_FIL);
				DebugMessageFatfs("Read file error !\r\n");
			  	return UPDATE_ERROR_ERASE;
			}
			if(bw == 4096)
			{

				SPI_FLASH_BufferWrite(IapBuf,ProgAddr,PROGRAM_SIZE);	
				memset(IapBufCmp,0,4096);
				SPI_FLASH_BufferRead(IapBufCmp,ProgAddr,PROGRAM_SIZE);

				if(memcmp(IapBuf,IapBufCmp,PROGRAM_SIZE))
				{
					f_close(&my_FIL);
					DebugMessageFatfs("Compare data error ! \r\n");
					return UPDATE_ERROR_CMP;
				}
				
				ProgAddr += PROGRAM_SIZE;
				DebugMessageFatfs("Program addr %8x ok ..\r\n",ProgAddr);
				menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, ProgAddr*100/fwPack_Info[fileNums].fileSize);

			}
			else
			{

				SPI_FLASH_BufferWrite(IapBuf,ProgAddr,PROGRAM_SIZE);	
				memset(IapBufCmp,0,4096);
				SPI_FLASH_BufferRead(IapBufCmp,ProgAddr,PROGRAM_SIZE);

				if(memcmp(IapBuf,IapBufCmp,PROGRAM_SIZE))
				{
					f_close(&my_FIL);
					DebugMessageFatfs("Compare data error ! \r\n");
					return UPDATE_ERROR_CMP;
				}
				ProgAddr += bw;
				DebugMessageFatfs("Program addr %8x ok ..\r\n",ProgAddr);
				menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, ProgAddr*100/fwPack_Info[fileNums].fileSize);
			}
		
		}

		if(strcmp(fwPack_Info[fileNums].fileType,"FPGA") == 0)
		{
			FPGA_Program_Done();
			SoftSpi_UnUsePort(SPI_PORT_B);
		}
		else if(strcmp(fwPack_Info[fileNums].fileType,"Video") == 0)
		{
			RTD2796Reset();
			SoftSpi_UnUsePort(SPI_PORT_A);
		}
		else if(strcmp(fwPack_Info[fileNums].fileType,"Font") == 0)
		{
			SoftSpi_UnUsePort(SPI_PORT_C);
		}

	}
	
	f_close(&my_FIL);
	memset((u8*)fwPack_Info,0,sizeof(fwPack_Info));
	if(updateFlag)
	{
		dispFactoryReset();
	}
	else
	{
		DebugMessageFatfs("No update fw find...\r\n");
		LCD_Clear();
		if(GetLanguage()== 0)
		{
			LCD_WriteLine2("       No update fw find");
			
		}
		else
		{
			LCD_WriteLine2("       没有找到升级文件");
		}
		SysDelay1ms(2000);
		menuEnterStatusPage();
		return UPDATE_ERROR_LOAD_FILE;
	}
    //################################################################################################
    //ID
	return UPDATE_OK;
}

//#############################################################################################################
//UpdateAppFile
//UPDATE_Status UpdateAppFile(char *updateFileName,fwPackInfo *packInfo,const char * FileType)
UPDATE_Status UpdateLogoFile(char *updateFileName)
{
	UINT bw;
	FRESULT res;
	vu32 ProgAddr = 0;
    vu32 BaseAddr = 0;
	int i;
	u32 sectorSize;
	int fileNums = 0;
	u16 ver;
	u8 verh;
	u8 updateFlag = 0;

	res = f_open(&my_FIL,updateFileName, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (res != FR_OK)
	{
		f_close(&my_FIL);
		DebugMessageFatfs("Open Logo file error !\r\n");
	  	return UPDATE_ERROR_LOAD_FILE;
	}
	menuProcessEvent(MENU_EVENT_UPDATE_FIRMWARE_START);

	DebugMessageFatfs("USBH Update Logo...\r\n");
	updateFlag = 1;
	SPI_FLASH_Init(SPI_PORT_C);
	sectorSize = 65536;
	f_lseek(&my_FIL,0);
    //f_lseek(&my_FIL,fwPack_Info[fileNums].fileOffset);
	
	//#####################################################################
	//擦除扇区
	//for(i = 0;i < (my_FIL.fsize/sectorSize)+1;i++)
	//{
		SPI_FLASH_SectorErase(0x1ff000);
		DebugMessageFatfs("Erase sector %d ok ..\r\n",i);
		menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, i*sectorSize*100/fwPack_Info[fileNums].fileSize);
	//}
	
	DebugMessageFatfs("Program start ...\r\n");
	ProgAddr = SPI_FLASH_FONT_OFFSET;
	BaseAddr = SPI_FLASH_FONT_OFFSET;
	menuProcessEventValue(MENU_EVENT_PROGRESS_BAR,0);
	while((ProgAddr - BaseAddr) < my_FIL.fsize)
	{
		memset(IapBuf,0,PROGRAM_SIZE);
		//res = f_read(&f_file,IapBuf,PROGRAM_SIZE, &bw);
		res = f_read(&my_FIL,IapBuf,sizeof(IapBuf), &bw);
		if (res != FR_OK)
		{
			f_close(&my_FIL);
			DebugMessageFatfs("Read file error !\r\n");
		  	return UPDATE_ERROR_ERASE;
		}
		if(bw == 4096)
		{

			SPI_FLASH_BufferWrite(IapBuf,ProgAddr,PROGRAM_SIZE);	
			memset(IapBufCmp,0,4096);
			SPI_FLASH_BufferRead(IapBufCmp,ProgAddr,PROGRAM_SIZE);

			if(memcmp(IapBuf,IapBufCmp,PROGRAM_SIZE))
			{
				f_close(&my_FIL);
				DebugMessageFatfs("Compare data error ! \r\n");
				return UPDATE_ERROR_CMP;
			}
			
			ProgAddr += PROGRAM_SIZE;
			DebugMessageFatfs("Program addr %8x ok ..\r\n",ProgAddr);
			menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, ProgAddr*100/fwPack_Info[fileNums].fileSize);

		}
		else
		{

			SPI_FLASH_BufferWrite(IapBuf,ProgAddr,bw);	
			memset(IapBufCmp,0,4096);
			SPI_FLASH_BufferRead(IapBufCmp,ProgAddr,bw);

			if(memcmp(IapBuf,IapBufCmp,bw))
			{
				f_close(&my_FIL);
				DebugMessageFatfs("Compare data error ! \r\n");
				return UPDATE_ERROR_CMP;
			}
			ProgAddr += bw;
			DebugMessageFatfs("Program addr %8x ok ..\r\n",ProgAddr);
			menuProcessEventValue(MENU_EVENT_PROGRESS_BAR, ProgAddr*100/my_FIL.fsize);
		}
	
	}

	SoftSpi_UnUsePort(SPI_PORT_C);
	
	f_close(&my_FIL);

	DebugMessageFatfs("Update Logo Sucess !\r\n");
	LCD_Clear();
	if(GetLanguage()== 0)
	{
		LCD_WriteLine2("       Update Logo Sucess !");
		
	}
	else
	{
		LCD_WriteLine2("       更新 LOGO 成功!");
	}
	SysDelay1ms(2000);
		
	menuEnterStatusPage();
	//Bsp_Soft_Reset();
    //################################################################################################
    //ID
	return UPDATE_OK;
}


extern int update_version;
extern int FPGA_GetSoftVersion(u8 * msb,u8 * lsb);
extern int getValue(int index);
extern int GetVideoVersion(void);


bool update_mcu_program_from_fatfs(char *updateFileName)
{
	int res;
	unsigned int readSize;
	UpdateAppFile(updateFileName);
	
	printf("Auto update End，sysTick = %d\r\n", GetSysTick());
	SysDelay1ms(100);
	return TRUE;
}

bool update_Logo_from_fatfs(char *updateFileName)
{
	int res;
	unsigned int readSize;
	UpdateLogoFile(updateFileName);
	
	printf("Auto update End，sysTick = %d\r\n", GetSysTick());
	SysDelay1ms(100);
	return TRUE;
}

BYTE MSC_Application_Ask(void)
{
	if(bUsbUpdateAsk == 0xFF)
	{
		bUsbUpdateAsk = 0xF0;
		menuEnterPage(MENU_INDEX_USB_UPDATE_CONFIRM);	

		return 0;			
	}
	return 0;
}

BYTE MSC_UpdateLogo(void)
{
	char name[16];
	sprintf(name, "%s%s",USBDISKPath,  "logo.bin");
	if(update_Logo_from_fatfs(name))
	{
		return 1;
	}
	return 0;	
}

BYTE MSC_Application(void)
{
	
	char name[16];
	sprintf(name, "%s%s","2:","update.yds");
	if(update_mcu_program_from_fatfs(name))
	{
		// 升级程序完成
		if(bUpdateMCU == TRUE)
		{
			printf("Have MCU update,Need Program again\r\n");
			return 0;
		}

		bUsbUpdateFlag = FALSE;
		return 1;
	}
	return 0;
}


int BackUpUserSettingToUDisk(int set_ID)
{

	return TRUE;
}

int LoadUserSettingFromUDisk(int set_ID)
{
	return TRUE;
}


void usb_disk_init(void)
{
	//FATFS_LinkDriver(&USBH_Driver, USBDISKPath);
		
	/* Init Host Library */
	USBH_Init(
				&USB_OTG_Core, 
#ifdef USE_USB_OTG_FS  
				USB_OTG_FS_CORE_ID,
#else 
				USB_OTG_HS_CORE_ID,
#endif 
				&USB_Host,
				&USBH_MSC_cb, 
				&USR_cb
			);

}

void usb_disk_task(void)
{
	USBH_Process(&USB_OTG_Core, &USB_Host);
}

BYTE setUsbUpdateFlag(BYTE flag)
{
	bUsbUpdateFlag = flag;

	if(bUsbUpdateFlag == TRUE)
	{
		if(USBH_USR_ApplicationState == USH_USR_FS_IDLE)
		{
			USBH_USR_ApplicationState = USH_USR_FS_UPDATE;
			return TRUE;
		}
	}

	return FALSE;
}

BYTE getUsbUpdateFlag(void)
{
	return bUsbUpdateFlag;
}

BYTE UserPerf_Application(void)
{
	printf("%s\r\n",__func__);
	return 0;
}

BYTE usbSaveUserSetting(int saveIndex)
{
	BYTE usbSaveIdx = 0;
	
	if(USBH_USR_ApplicationState == USH_USR_FS_IDLE)
	{
		USBH_USR_ApplicationState = USH_USR_FS_USERPERF;
		
		if(BackUpUserSettingToUDisk(usbSaveIdx) == FALSE)
        {
    		USBH_USR_ApplicationState = USH_USR_FS_IDLE;

    		return FALSE;            
        }
		
		USBH_USR_ApplicationState = USH_USR_FS_IDLE;

		return TRUE;
	}

	return FALSE;
}

BYTE usbLoadUserSetting(int saveIndex)
{
	BYTE usbLoadIdx = 0;

	if(saveIndex == 0xFF)
	{
		usbLoadIdx = USER_SAVE_MAX_SIZE;
	}
	else
	{
		if(saveIndex < USER_SAVE_MAX_SIZE)
		{
			usbLoadIdx = saveIndex;
		}
	}
	
	if(USBH_USR_ApplicationState == USH_USR_FS_IDLE)
	{
		USBH_USR_ApplicationState = USH_USR_FS_USERPERF;
		
		if(LoadUserSettingFromUDisk(usbLoadIdx) == FALSE)
        {
            USBH_USR_ApplicationState = USH_USR_FS_IDLE;
            
            return FALSE;
        }
		
		USBH_USR_ApplicationState = USH_USR_FS_IDLE;

		return TRUE;		
	}

	return FALSE;
}


