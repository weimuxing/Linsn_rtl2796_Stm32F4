#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "SysConfig.h"

#include "usb-rs232.h"	

#include "string.h"

// usb
#if 0
#include  "usbd_cdc_core.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"
#include  "usbd_cdc_vcp.h"
#else
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#endif



/* Private variables ---------------------------------------------------------*/


////////////////////////////////////


void init_usb(void)
{
#if !(CURRENT_DEVICE == _VIP_SMART)
	USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
				USB_OTG_HS_CORE_ID,
#else            
				USB_OTG_FS_CORE_ID,
#endif  
				&USR_desc, 
				&USBD_CDC_cb, 
				&USR_cb);
#endif
}

void task_usb(void)
{
	// do nothing
}

extern void USART_TO_USB_DATA(uint8_t *buf, int len); // Usbd_cdc_vcp.c

void SendStringToUsb(char * str)
{
#if !(CURRENT_DEVICE == _VIP_SMART)
	USART_TO_USB_DATA((uint8_t *)str, strlen(str));
#endif
	
}

void to_pcusb(U8 *buf,int buflen) 
{
	#if 0
	while(buflen > 64)
	{
		USART_TO_USB_DATA(buf, 64);
		buf += 64;
		buflen -= 64;
	}
	#endif

	
  #if !(CURRENT_DEVICE == _VIP_SMART)
	USART_TO_USB_DATA(buf, buflen);
  #endif
  
}





