#ifndef _UPDATE_FROM_FATFS_H
#define _UPDATE_FROM_FATFS_H


#include "define.h"


#define PROGRAM_SIZE	4096

typedef struct
{
	char 	FileType[12];						//Transform style
	int 	FileId;
	u32  	FileLocation;						//
	u32  	FileLen;							//
	char 	FileVer[12];						//
}FileIndex;




typedef struct
{
	char 		FileType[12];						//
	u32  		FileNum;							//
	FileIndex 	Index[12];
}FileHead;



typedef struct
{
	char fileType[12];	//Transform style
    u32 fileId;			//
    u32 fileSize;		//
    u64 fileOffset;		//
    u16 FileVersion;						//
}fwPackInfo;

typedef enum
{ 
  UPDATE_OK = 0,
  UPDATE_ERROR_LOAD_FILE,
  UPDATE_ERROR_WRITE,
  UPDATE_ERROR_READ,
  UPDATE_ERROR_CMP,
  UPDATE_ERROR_ERASE,
  //UPDATE_ERROR_OPERATION,
  //UPDATE_COMPLETE
}UPDATE_Status;


void usb_disk_init(void);
void usb_disk_task(void);
BYTE setUsbUpdateFlag(BYTE flag);
BYTE getUsbUpdateFlag(void);

BYTE usbSaveUserSetting(int saveIndex);
BYTE usbLoadUserSetting(int saveIndex);


#endif			// _UPDATE_FROM_FATFS_H

