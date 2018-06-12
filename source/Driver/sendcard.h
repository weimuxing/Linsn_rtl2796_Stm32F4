#ifndef _SENDCARD_H_
#define _SENDCARD_H_


#include "stm32f4xx.h"

#define _SENDCARD_LXY           0
#define _SENDCARD_COLORLIGHT        1
#define _SENDCARD_MOWA       2

typedef struct
{
	BYTE  vline[2][2];
	BYTE  hline[2][2];
	int   vsize[2][2];
	int   hsize[2][2];    
}SendCard_Basic_Param_St;



void init_sendcard(void);
int getSendCardPort(void);


#endif		// #ifndef _SENDCARD_H_


