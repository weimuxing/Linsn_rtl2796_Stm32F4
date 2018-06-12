/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */

#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"


/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
#define IMAGE_BUFFER_SIZE    512
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15]  = {0};

FATFS fatfs;
FIL file;
uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
uint8_t line_idx = 0;   

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 


/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/

/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
	printf("USB OTG HS MSC Host\r\n");
	printf("> USB Host library started.\r\n");
	printf("  USB Host Library v2.1.0\r\n\r\n");
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
	printf("检测到USB设备插入!\r\n");  
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 
 // LCD_ErrLog((void *)MSG_UNREC_ERROR); 
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
	printf("USB设备拔出!\r\n");  
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
	printf("复位设备...\r\n");
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
   	printf("高速(HS)USB设备!\r\n");
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
	 printf("全速(FS)USB设备!\r\n"); 
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
	  printf("低速(LS)USB设备!\r\n");  
  }
  else
  {
	  printf("设备错误!\r\n");	
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  
  printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
  printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 

}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
 	printf("从机地址分配成功!\r\n");    
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
	  printf("可移动存储器设备!\r\n"); 
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
  	printf("HID 设备!\r\n"); 
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
	printf("Product: %s\r\n",(char *)ProductString); 
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	printf("Serial Number: %s\r\n",(char *)SerialNumString);  
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
	printf("设备枚举完成!\r\n\r\n");    
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
	printf("无法识别的USB设备!\r\n\r\n");    
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
	return USBH_USR_RESP_OK;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
	printf("端口电流过大!!!\r\n");
}


extern FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
extern char USBDISKPath[4];          /* USB Host logical drive path */
extern BYTE bUsbUpdateAsk;

extern BYTE MSC_Application(void);
extern BYTE UserPerf_Application(void);
extern BYTE MSC_Application_Ask(void);


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
	char name[20];
    BYTE  result;

	switch(USBH_USR_ApplicationState)
	{
		case USH_USR_FS_INIT: 		  
		  if(f_mount(&USBDISKFatFs, "2:", 0) != FR_OK)
		  {
			/* efs initialisation fails*/
		  //  LCD_ErrLog("> Cannot initialize File System.\n");
			return(-1);
		  }
		  
		  	//sprintf(name, "%s%s",USBDISKPath,"VipDrive");
			//f_mkdir(name);
		  	printf("> File System initialized.\r\n");
		  	printf("> Disk capacity : %d M Bytes\r\n", USBH_MSC_Param.MSCapacity * \
			USBH_MSC_Param.MSPageLength/1024/1024); 
		  
		  if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
		  {
			//LCD_ErrLog((void *)MSG_WR_PROTECT);
		  }
		  //MSC_UpdateLogo();
		  //USBH_USR_ApplicationState = USH_USR_FS_UPDATE;
		  USBH_USR_ApplicationState = USH_USR_FS_UPDATE_ASK;
		  bUsbUpdateAsk = 0xFF;
		  
		  break;
		case USH_USR_FS_UPDATE_ASK:
			result = MSC_Application_Ask();
		  	if(result == 0xFF || result == 0xF0)
		  	{
				
		  	}
			else if(result == 0)
		  	{
		  		USBH_USR_ApplicationState = USH_USR_FS_IDLE;
		  	}
			else if(result == 1)
			{
				USBH_USR_ApplicationState = USH_USR_FS_UPDATE;
			}
			break;			
		case USH_USR_FS_UPDATE:
		  	if(	MSC_Application())
		  	{
		  		USBH_USR_ApplicationState = USH_USR_FS_IDLE;
		  	}
			break;
		  case USH_USR_FS_USERPERF:
		  	if(UserPerf_Application())
		  	{
		  		USBH_USR_ApplicationState = USH_USR_FS_IDLE;
		  	}
		  	break;
		case USH_USR_FS_IDLE:
			break;
		default:
			break;
	}
	return(0);
}

extern char bUpdateMCU;


/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
  bUpdateMCU = FALSE;
}




////////////////////////////////////////////////////////////////////////////////////////
//用户定义函数,实现fatfs diskio的接口函数 
extern USBH_HOST              USB_Host;
//static u8 AppState;

//获取U盘状态
//返回值:0,U盘未就绪
//      1,就绪
u8 USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core);//返回U盘状态
}

//读U盘
//buf:读数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;		 
u8 USBH_UDISK_Read(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core))//&&AppState==USH_USR_FS_TEST)//连接还存在,且是APP测试状态
	{  		    
		do
		{
			res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=1;//读写错误
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}
	else 
		res=1;		  
	if(res==USBH_MSC_OK)
		res=0;	
	return res;
}

//写U盘
//buf:写数据缓存区
//sector:扇区地址
//cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;		 
u8 USBH_UDISK_Write(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core))//&&AppState==USH_USR_FS_TEST)//连接还存在,且是APP测试状态
	{  		    
		do
		{
			res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt); 
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=1;//读写错误
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

