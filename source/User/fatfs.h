#ifndef _FATFS_H_
#define _FATFS_H_

#include "define.h"


void fatfs_init(void);


u8 fatfs_getfree(u8 *drv,u32 *total,u32 *free);


#endif


