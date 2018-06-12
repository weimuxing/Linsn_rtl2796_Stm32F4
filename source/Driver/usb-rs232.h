#ifndef _USB_RS232_H_
#define _USB_RS232_H_

#include "define.h"


#define ENABlE_USB_VCOM 1  // define


void init_usb(void);

void task_usb(void);

void SendStringToUsb(char * str);

void to_pcusb(U8 *buf,int buflen);




#endif


