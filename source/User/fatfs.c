#include "fatfs.h"
#include "ff.h"
#include "sdcard.h"
#include <stdio.h>
#include "systick.h"

#include "uart.h"

#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"


FATFS fs;

//#define LED0 PCout(3)


void fatfs_init(void)
{
#if 0	// Cancel by hibernate
	u32 total, free;
	int tryCount = 0;

	
	while(SD_USER_Init() != SD_OK)					//���SD��
	{
		printf("SD Card Error!\r\n");
		if(tryCount++ > 3)
		{
			return;
		}
	}
	{
		int buffer[1024/4];
		SD_ReadDisk((uint8_t *) &buffer,0,2);
	}
	printf("SD Card OK!\r\n");

	f_mount(0,&fs); 					 	//����SD�� 
  	printf("f_mount sd\r\n");

	while(fatfs_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
		int error;
		printf("Fatfs Error!\r\n");
		SysDelay1ms(200);
		LED0=!LED0;//DS0��˸
		printf("MakeFs Wait!\r\n");
		error = f_mkfs(0,0,0);
		if(error != FR_OK)
		{
			
			printf("MakeFs Error = %d\r\n", error);
			return;
		}
	}
	printf("FATFS OK!\r\n");	
	printf("SD Total Size: %d MB\r\n", total>>10);	 
	printf("SD  Free Size: %d MB\r\n", free>>10);
#endif
}

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 fatfs_getfree(u8 *drv,u32 *total,u32 *free)
{
	u8 res;

#if 0

	FATFS *fs1;

    DWORD fre_clust=0, fre_sect=0, tot_sect=0;


    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv, &fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
#endif
	return res;
}


