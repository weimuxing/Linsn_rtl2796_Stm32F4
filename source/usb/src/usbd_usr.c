/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_usr.h"
#include "usbd_ioreq.h"
#include <stdio.h>

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USBD_USR 
* @brief    This file includes the user application layer
* @{
*/ 

/** @defgroup USBD_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Variables
* @{
*/ 

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
};

/**
* @}
*/

/** @defgroup USBD_USR_Private_Constants
* @{
*/ 

/**
* @}
*/



/** @defgroup USBD_USR_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Functions
* @{
*/ 

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{
#if 1
#ifdef USE_USB_OTG_HS 
  printf(" USB OTG HS VCP Device\r\n");
#else
  printf(" USB OTG FS VCP Device\r\n");
#endif
  printf("> USB device library started.\r\n"); 
  printf ("     USB Device Library v1.0.0\r\n" );

#else
  /* Initialize LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);   
  
  /* Initialize the LCD */
#ifdef USE_STM3210C_EVAL
  STM3210C_LCD_Init();  
#else
  STM322xG_LCD_Init();
#endif
  LCD_LOG_Init();
  
#ifdef USE_USB_OTG_HS 
  LCD_LOG_SetHeader(" USB OTG HS VCP Device");
#else
  LCD_LOG_SetHeader(" USB OTG FS VCP Device");
#endif
  LCD_UsrLog("> USB device library started.\n"); 
  LCD_LOG_SetFooter ("     USB Device Library v1.0.0" );
#endif

}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed )
{
#if 1
 switch (speed)
 {
   case USB_OTG_SPEED_HIGH: 
     //printf("     USB Device Library v1.0.0 [HS]\r\n" );
     break;

  case USB_OTG_SPEED_FULL: 
    // printf("     USB Device Library v1.0.0 [FS]\r\n" );
     break;
 default:
     printf("     USB Device Library v1.0.0 [??]\r\n" );	 
 }

#else
 switch (speed)
 {
   case USB_OTG_SPEED_HIGH: 
     LCD_LOG_SetFooter ("     USB Device Library v1.0.0 [HS]" );
     break;

  case USB_OTG_SPEED_FULL: 
     LCD_LOG_SetFooter ("     USB Device Library v1.0.0 [FS]" );
     break;
 default:
     LCD_LOG_SetFooter ("     USB Device Library v1.0.0 [??]" );	 
 }
#endif

}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConfigured (void)
{
  printf("> VCP Interface configured.\r\n");
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
  printf("> USB Device in Suspend Mode.\r\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
    printf("> USB Device in Idle Mode.\r\n");
  /* Users can do their application actions here for the USB-Reset */
}

/**
* @}
*/ 

/**
* @}
*/ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
