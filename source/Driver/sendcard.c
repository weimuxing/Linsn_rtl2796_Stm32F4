
#include "uart.h"
#include "pipe.h"
#include "rs232command.h"
#include "systick.h"
//#include "vsp320.h"
#include "userpref.h"
#include "control.h"
#include "videoFormatTable.h"
#include "sendcard.h"

#include <stdio.h>
#include "uart.h"
#include <math.h>
#include <string.h>

int UART_PORT = 6;

u8 cardTypeCheckOK =0;
u8 isCheckCardtype =0;



void init_sendcard(void)
{
    if(getCommHardVersion() > 0)
    {
        UART_PORT = 4;
    }
    else
    {
        UART_PORT = 6;
    }
	UARTInit(UART_PORT,115200);
}

int getSendCardPort(void)
{
    return UART_PORT;
}

void setCardTypeOk(u8 value)
{
	cardTypeCheckOK = value;
}

u8 getCardTypeOk(void)
{
	return cardTypeCheckOK;
}
void setIscheckCardtype(u8 value)
{
	isCheckCardtype = value;
}

u8 getIscheckCardtype(void)
{
	return isCheckCardtype;
}


