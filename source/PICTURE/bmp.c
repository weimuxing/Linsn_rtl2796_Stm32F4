/*
 ============================================================================
 Name        : BMP.c
 Author      : anve
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "ff.h"
#include "malloc.h"
#include "spi.h"
#include "define.h"
#include "systick.h"

#include "control.h"
#include "bmp.h"

#ifdef SPI2_DMA_ENABLE
#define ENABLE_DMA_DOUBLE_BUFFER 1
#else
#define ENABLE_DMA_DOUBLE_BUFFER 0

#endif

static int memOffsetAddress = 0;
static int logoLayer = 0;



DWORD BYTE2DWORD(BYTE *buf, int size)
{
	DWORD result = 0;
	int i;
	for(i=0; i<size; i++)
	{
		result <<= 8;
		result |= buf[size-1-i];
	}
	return result;
}

void DWORD2BYTE(BYTE *buf, DWORD value, int size)
{
	int i;
	for(i=0; i<size; i++)
	{
		buf[i] = value;
		value >>= 8;
	}
}

DWORD getDataSize(int width, int height, int colorSize)
{
	DWORD DataSizePerLine;
	DataSizePerLine= (width*colorSize+31)/8;	// 一个扫描行所占的字节数
	DataSizePerLine= DataSizePerLine/4*4; // 字节数必须是4的倍数
	return DataSizePerLine*height;
}

bool BMP_encodeInfo(BYTE *bmpHeaderBuf, int width, int height, int colorSize)
{
	DWORD fileSize, dataSize, headerSize;
	if(colorSize != 24)
	{
		return FALSE;
	}
	headerSize = BMP_FILE_HEADER_SIZE;
	dataSize = getDataSize(width, height, colorSize);
	fileSize = dataSize+headerSize;
	
	DWORD2BYTE(bmpHeaderBuf+0, BMP_FILE_ID, 2);
	DWORD2BYTE(bmpHeaderBuf+2, fileSize, 4);
	DWORD2BYTE(bmpHeaderBuf+6, 0, 4);
	DWORD2BYTE(bmpHeaderBuf+0x0A, headerSize, 4);
	DWORD2BYTE(bmpHeaderBuf+0x0E, 0x28, 4);
	DWORD2BYTE(bmpHeaderBuf+0x12, width, 4);
	DWORD2BYTE(bmpHeaderBuf+0x16, height, 4);
	DWORD2BYTE(bmpHeaderBuf+0x1A, 1, 2);
	DWORD2BYTE(bmpHeaderBuf+0x1C, colorSize, 2);
	DWORD2BYTE(bmpHeaderBuf+0x1E, 0, 4);
	DWORD2BYTE(bmpHeaderBuf+0x22, dataSize, 4);
	DWORD2BYTE(bmpHeaderBuf+0x26, 0, 4);
	DWORD2BYTE(bmpHeaderBuf+0x2A, 0, 4);
	DWORD2BYTE(bmpHeaderBuf+0x2E, 0, 4);
	DWORD2BYTE(bmpHeaderBuf+0x32, 0, 4);
	return TRUE;
	
}

bool BMP_decodeInfo(BYTE *bmpHeaderBuf, BMPINFO *bmpInfo)
{
	if(BYTE2DWORD(bmpHeaderBuf, 2) == BMP_FILE_ID)
	{
		bmpInfo->isValid = 1;
	}
	else
	{
		bmpInfo->isValid = 0;
		{
			int i;
			for(i=0; i<16; i++)
			{
				printf("%02x ", bmpHeaderBuf[i]);
			}
		}
		return FALSE;
	}
	bmpInfo->fileSize = BYTE2DWORD(bmpHeaderBuf+2, 4);
	bmpInfo->dataOffset = BYTE2DWORD(bmpHeaderBuf+0x0a, 4);
	bmpInfo->width = BYTE2DWORD(bmpHeaderBuf+0x12, 4);
	bmpInfo->height = BYTE2DWORD(bmpHeaderBuf+0x16, 4);
	bmpInfo->colorSize = BYTE2DWORD(bmpHeaderBuf+0x1c, 2);
	bmpInfo->dataSize = BYTE2DWORD(bmpHeaderBuf+0x22, 4);
	bmpInfo->colorSpace = BYTE2DWORD(bmpHeaderBuf+0x32, 4);
	if(bmpInfo->dataSize != bmpInfo->fileSize-bmpInfo->dataOffset)
	{
		//printf("dataSize = %d   error1\r\n", bmpInfo->dataSize);
		bmpInfo->dataSize = bmpInfo->fileSize-bmpInfo->dataOffset;
	}
	if(bmpInfo->dataSize != getDataSize(bmpInfo->width, bmpInfo->height, bmpInfo->colorSize))
	{
		//printf("dataSize = %d   error2\r\n", bmpInfo->dataSize);
		bmpInfo->dataSize = getDataSize(bmpInfo->width, bmpInfo->height, bmpInfo->colorSize);
	}
	return TRUE;
}

bool BMP_getFileExist(char * fileName)
{
	
	DWORD readSize;
	BMPINFO bmpInfo;
	int result;
	FIL *fp = mymalloc(SRAMIN,sizeof(FIL));
	
	BYTE buffer[0x40];
	f_open(fp, fileName, FA_READ);
	if(fp == NULL)
	{
		printf("can't open file:%s\r\n", fileName);
		myfree(SRAMIN, fp);
		return FALSE;
	}
	// get file info
	f_read(fp, buffer, sizeof(buffer), (unsigned int *) &readSize);
	if(readSize != sizeof(buffer))
	{
		printf("readSize = %d\r\n", readSize);
	}

	result = BMP_decodeInfo(buffer, &bmpInfo);
	f_close(fp);
	
	myfree(SRAMIN, fp);
	
	fp = NULL;
	
	return (bool) result;
	
	
	
}


bool getFileInfo(char *fileName, BMPINFO *bmpInfo)
{
	DWORD readSize;
	FIL *fp = mymalloc(SRAMIN,sizeof(FIL));
	
	BYTE buffer[0x40];
	f_open(fp, fileName, FA_READ);
	if(fp == NULL)
	{
		printf("can't open file:%s\r\n", fileName);
		myfree(SRAMIN, fp);
		return FALSE;
	}
	// get file info
	f_read(fp, buffer, sizeof(buffer), (unsigned int *)&readSize);
	if(readSize != sizeof(buffer))
	{
		printf("readSize = %d\r\n", readSize);
	}

	
	f_close(fp);
	
	myfree(SRAMIN, fp);
	
	fp = NULL;
	return BMP_decodeInfo(buffer, bmpInfo);

}

void printFileInfo(BMPINFO *bmpInfo)
{
	
	{
		printf("File Size: %d\r\n", bmpInfo->fileSize);
		printf("Picture Size: %dX%d\r\n", bmpInfo->width, bmpInfo->height);
		printf("Color Deep: %d\r\n", bmpInfo->colorSize);
		printf("Data Size: %d\r\n", bmpInfo->dataSize);
		printf("Data Offset: %d\r\n", bmpInfo->dataOffset);

	}
	
}

#define LOGO_SPI_CS SPI2_CS

// G6 = SPI_CS
// B13 = SPI_SCL
// B14 = SPI_MISO
// B15 = SPI_MOSI

void SetLogoLayer(int layer)
{
	int ch_offset = layer;
	logoLayer = layer;
	memOffsetAddress = readC732(B22_OSFLD0CH1, ch_offset);
	
}

void DisableLogoDisplay(void)
{
	int inPort;
	
	SetImageMemoryC732(logoLayer);
	
	
	inPort = GetUserSetting()->layer[logoLayer].inputPort;

	GetUserSetting()->autoSetInput[inPort] = 1;

	GetUserSetting()->layer[logoLayer].freeze = 0;
	
	SetColorSpaceAuto(inPort);
	SetInputFormat(inPort,GetUserSetting()->input[inPort].format);
}
void writeLogoStart(void)
{
	int ch_offset = logoLayer;
	
	printf("Start Write Logo\r\n");

#if 1
	SetLayerFreezeC732(logoLayer,1);
	writeC732(B22_OSFLD0CH1,ch_offset,memOffsetAddress);
	writeC732(B22_OSFLD1CH1,ch_offset,memOffsetAddress);
	writeC732(B22_OSFLD2CH1,ch_offset,memOffsetAddress);
	writeC732(B22_OSFLD3CH1,ch_offset,memOffsetAddress);
	writeC732(B0_OSDCT, 0, 0x00);
#else
			 
	SPI2_Init();
	LOGO_SPI_CS = 1;
	LOGO_SPI_CS = 0;
#endif
}

#define LOGO_DELAY() do{int _i; for(_i=0; _i<120; _i++){}}while(0)

void writeLogoLine(int lineNumber, BYTE *buffer, int width)
{
	int startAddress = memOffsetAddress;
	int imw;
	lineNumber = lineNumber;
	buffer = buffer;
	width = width;

#if 1
	imw = readC732(B22_IMWICH1, logoLayer)*128;
	startAddress += imw*4*lineNumber;
	// Specify the start address for write
	writeC732(B0_CPUWAD,0,startAddress);
	#if 1
	// 优化代码1920x1080的图像从17秒降到3秒
	WriteRegisterC732_ImageLineBuffer(0x32, buffer, width );
	#else
	for(i=0; i<width; i++)
	{
		#if 1
		writeRegisterC732(0x32,*buffer++);
		writeRegisterC732(0x32,*buffer++);
		writeRegisterC732(0x32,*buffer++);
		writeRegisterC732(0x32,0);
		#else
		writeRegisterC732(0x32,*buffer++);
		writeC732(B0_CPUDT, 0, *buffer++);
		writeC732(B0_CPUDT, 0, *buffer++);
		writeC732(B0_CPUDT, 0, *buffer++);
		writeC732(B0_CPUDT, 0, 0);
		#endif
		
	}
	#endif
	
#else
	
	#if 1
	#ifdef SPI2_DMA_ENABLE
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	
	DMA1_To_SPI2_Init(buffer,width*3);
	
	/* Enable DMA channels */
	DMA_Cmd(SPI_MASTER_Tx_DMA_Channel, ENABLE);

	#if !ENABLE_DMA_DOUBLE_BUFFER

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	#endif
	
	#else
	
	{
		int i;
		if(lineNumber == 0)
		{
			buffer += 4*3;
			for(i=4; i<width; i++)
			{
				#if 0
				SPI2_ReadWriteByte(i);
				SPI2_ReadWriteByte(i);
				SPI2_ReadWriteByte(i);
				#else
				SPI2_ReadWriteByte(*buffer++);
				SPI2_ReadWriteByte(*buffer++);
				SPI2_ReadWriteByte(*buffer++);
				//LOGO_DELAY();
				
				#endif
			}
		}
		else
		{
			for(i=0; i<width; i++)
			{
				#if 0
				SPI2_ReadWriteByte(i);
				SPI2_ReadWriteByte(i);
				SPI2_ReadWriteByte(i);
				#else
				SPI2_ReadWriteByte(*buffer++);
				SPI2_ReadWriteByte(*buffer++);
				SPI2_ReadWriteByte(*buffer++);
				//LOGO_DELAY();
				
				#endif
			}
		}
	}
	#endif
	#endif
#endif
	
}
void writeLogoEnd(void)
{
#if 0
#if 1
    int i;
    for(i=0; i<100; i++)
    {
		readC732(B0_OSDCT,0);
    }
#else
	#if ENABLE_DMA_DOUBLE_BUFFER
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	#endif
	
	LOGO_SPI_CS = 1;
#endif
#endif
	printf("End Write Logo\r\n");
}

void setLogoDisplayScale(WORD width, WORD height, BYTE colorSpace)
{
	int inPort;
	
	inPort = GetUserSetting()->layer[logoLayer].inputPort;
	GetUserSetting()->input[inPort].colorSpace = colorSpace;
	GetUserSetting()->input[inPort].hActive = width;
	GetUserSetting()->input[inPort].vActive = height;
	GetUserSetting()->autoSetInput[inPort] = 0;
	GetUserSetting()->layer[logoLayer].crop.posX = 0;
	GetUserSetting()->layer[logoLayer].crop.posY = 0;
	GetUserSetting()->layer[logoLayer].crop.sizeX = width;
	GetUserSetting()->layer[logoLayer].crop.sizeY = height;
	GetUserSetting()->layer[logoLayer].freeze = 1;
	SetLayerSetting(logoLayer);
	SetBlackScreen(inPort,0);
	
	
}

void BMP_Display(BMPINFO *fileInfo, char * fileName)
{
	DWORD readSize;

	DWORD preLineSize;

	BYTE *buffer;
	#if ENABLE_DMA_DOUBLE_BUFFER
	BYTE *buffer1, *buffer2;
	#endif

	

	int err;

	FIL *fp = mymalloc(SRAMIN,sizeof(FIL));

	int i;

	if(fileInfo == NULL)
	{
		return;
	}

	preLineSize = BMP_PRE_LINE_SIZE(fileInfo->width,fileInfo->colorSize);
	
	#if ENABLE_DMA_DOUBLE_BUFFER
	buffer1 = mymalloc(SRAMIN,preLineSize);
	if(buffer1 == NULL)
	{
		printf("malloc error\r\n");
		myfree(SRAMIN,fp);
		return;
	}
	buffer2 = mymalloc(SRAMIN,preLineSize);
	if(buffer2 == NULL)
	{
		printf("malloc error\r\n");
		myfree(SRAMIN,fp);
		return;
	}
	buffer = buffer1;
	#else
	buffer = mymalloc(SRAMIN,preLineSize);
	#endif
	if(buffer == NULL)
	{
		myfree(SRAMIN,fp);
		return;
	}
	
	f_open(fp, fileName, FA_READ);
	if(fp == NULL)
	{
		printf("can't open file:%s\r\n", fileName);
		myfree(SRAMIN, fp);
		return;
	}
	// get file info
	err = f_read(fp,buffer, fileInfo->dataOffset, (unsigned int *)&readSize);

	if(err)
	{
		printf("readSize = %d\r\n", readSize);
	}

	
	writeLogoStart();

	setLogoDisplayScale(fileInfo->width, fileInfo->height, fileInfo->colorSpace);
	
	for(i=0; i<fileInfo->height; i++)
	{
		//preLineSize = 512;
		#if ENABLE_DMA_DOUBLE_BUFFER
		
		if(buffer == buffer1)
		{
			buffer = buffer2;
		}
		else
		{
			buffer = buffer1;
		}
		#endif
		
		if(fileInfo->dataOffset & 0x03)
		{
			DWORD * tempBuffer[512/4];
			int offset = 512-fileInfo->dataOffset;
			int j;
			err = f_read(fp, tempBuffer, offset, (unsigned int *)&readSize);
			err = f_read(fp, buffer, preLineSize-offset, (unsigned int *)&readSize);
			for(j=preLineSize-1; j>=offset; j--)
			{
				buffer[j] = buffer[j-offset];
			}
			for(j=0; j<offset; j++)
			{
				buffer[j] = ((BYTE *)tempBuffer)[j];
			}
			
		}
		else
		{
			err = f_read(fp, buffer, preLineSize, (unsigned int *)&readSize);
		}
		if(err)
		{
			printf("preLineSize = %d, readSize = %d, err = %d\r\n", preLineSize, readSize, err);
			break;
		}
		else
		{
			writeLogoLine(fileInfo->height-i-1, buffer, fileInfo->width);
			if((fileInfo->height-i-1) == 0)
			{
				writeLogoLine(fileInfo->height-i-1, buffer, fileInfo->width);
			}
		}
	}
	writeLogoEnd();

	
	
	f_close(fp);
	
	myfree(SRAMIN, fp);
	#if ENABLE_DMA_DOUBLE_BUFFER
	myfree(SRAMIN, buffer1);
	myfree(SRAMIN, buffer2);
	#else
	myfree(SRAMIN, buffer);
	#endif
	fp = NULL;
	return;
}




void writePixel(u8 *picture, u8 r, u8 g, u8 b, int pixelSize)
{
	int i;
	for(i=0; i<pixelSize; i++)
	{
		picture[0] = b;
		picture[1] = g;
		picture[2] = r;
		picture += 3;
	}
}

void createTpLine(u8 *preLine, int width)
{
	int colorBarWidth;
	colorBarWidth = width/8;
	writePixel(preLine, 255, 255, 255, colorBarWidth); 	// white
	preLine += colorBarWidth*3;
	writePixel(preLine, 255, 255, 0,   colorBarWidth);	// yellow
	preLine += colorBarWidth*3;
	writePixel(preLine, 0, 255, 255, colorBarWidth);	// cyan
	preLine += colorBarWidth*3;
	writePixel(preLine, 0, 255, 0, colorBarWidth);	// green
	preLine += colorBarWidth*3;
	writePixel(preLine, 255, 0, 255, colorBarWidth);	
	preLine += colorBarWidth*3;
	writePixel(preLine, 255, 0, 0, colorBarWidth);	
	preLine += colorBarWidth*3;
	writePixel(preLine, 0, 0, 255, colorBarWidth);	
	preLine += colorBarWidth*3;
	writePixel(preLine, 0, 0, 0, colorBarWidth);	
	preLine += colorBarWidth*3;
	
}

#define WRITE_BUF_SIZE (512*2*4)
static int myWriteBuf[WRITE_BUF_SIZE/4];
static int remain = 0;

void myCloseFile(FIL *fp)
{
	int rwSize;
	u8 * buf = (u8 *)myWriteBuf;
	if(remain)
	{
		f_write(fp, buf, remain, (unsigned int *)&rwSize);
		if(rwSize != remain)
		{
			printf("write size = %d\r\n", rwSize);
			return;
		}
	}
	f_close(fp);
}
void myWriteFile(FIL *fp, u8 *buffer, int size)
{
	int rwSize;
	//int res;
	//u8 * buf = (u8 *)myWriteBuf;
	#if 1
	f_write(fp, buffer, size, (unsigned int *)&rwSize);
	return;
	#else
	
	//printf("remain = %d, size = %d\r\n", remain, size);
	if((remain == 0) && (((int)buffer & 0x03) == 0) && (size >= WRITE_BUF_SIZE))
	{
		res = f_write(fp, buffer, WRITE_BUF_SIZE, &rwSize);
		if(rwSize != WRITE_BUF_SIZE)
		{
			printf("write size = %d, error = %d\r\n", rwSize, res);
			return;
		}
		size = size-(WRITE_BUF_SIZE);
		remain = 0;
		myWriteFile(fp, buffer+WRITE_BUF_SIZE, size);
		return;
	}
	else if((size + remain) >= WRITE_BUF_SIZE)
	{
		int i;
		for(i=remain; i < WRITE_BUF_SIZE; i++)
		{
			buf[i] = *buffer++;
		}
		res = f_write(fp, buf, WRITE_BUF_SIZE, &rwSize);
		if(rwSize != WRITE_BUF_SIZE)
		{
			printf("write size = %d, error = %d\r\n", rwSize, res);
			return;
		}
		size = size-(WRITE_BUF_SIZE-remain);
		remain = 0;
		myWriteFile(fp, buffer, size);
		return;
	}
	else
	{
		int i;
		for(i=0; i < (size); i++)
		{
			buf[remain++] = buffer[i];
		}
		return;
	}
	#endif
}


void readLogoStart(void)
{
#if 0
//	GPIO_InitTypeDef  GPIO_InitStructure;
	printf("Start Read Logo\r\n");
#if 1
	SetLayerFreezeC732(logoLayer,1);
	
#else
	
 
 	
			 
	SPI2_Init();
	LOGO_SPI_CS = 1;
	LOGO_SPI_CS = 0;
#endif
#endif
}

void readLogoLine(int lineNumber, BYTE *buffer, int width)
{
#if 0
	int imw;
	int startAddress = memOffsetAddress;
	
	lineNumber = lineNumber;
	buffer = buffer;
	width = width;
	

#if 0
	if(lineNumber == 10)
	{
		int i;
		for(i=0; i<width*3; i++)
		{
			printf("%02x ", *buffer++);
		}
	}
#endif
	
#if 1

	imw = readC732(B22_IMWICH1, logoLayer)*128;
	startAddress += imw*4*lineNumber;
	startAddress += -1;

	// Change OSD access to IDLE
	writeC732(B0_OSDCT,  0, 0x00);
	// Specify the start address for reading
	writeC732(B0_CPURAD, 0, startAddress);
	writeC732(B0_CPUDTCTL, 0, 0x01);
	// Wait for the reading end
	while((readC732(B0_CPUDTCTL,0) & 0x08));
	//  Read B0_CPUDT register
	#if 1  // 1920x1080 优化代码从21秒降到9秒
	ReadRegisterC732_ImageLineBuffer(0x32, buffer, width );
	#else
	for(i=0; i<width; i++)
	{
		*buffer++ = readC732(B0_CPUDT,0);  // R
		*buffer++ = readC732(B0_CPUDT,0);  // G
		*buffer++ = readC732(B0_CPUDT,0);  // B
		readC732(B0_CPUDT,0);  // {00, B[21:20], G[11:10], R[1:0]}
		
	}
	#endif
	writeC732(B0_CPUDTCTL,0,0);
	
	

	
#else
	#ifdef SPI2_DMA_ENABLE
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	
	SPI2_To_DMA1_Init(buffer,width*3);
	
	/* Enable DMA channels */
	DMA_Cmd(SPI_MASTER_Rx_DMA_Channel, ENABLE);

	#if !ENABLE_DMA_DOUBLE_BUFFER

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	#endif
	
	#else
	
	{
		int i;
		for(i=0; i<width; i++)
		{
			
			*buffer++ = SPI2_ReadWriteByte(i);
			*buffer++ = SPI2_ReadWriteByte(i);
			*buffer++ = SPI2_ReadWriteByte(i);	
		}
	}
	#endif
#endif
#endif
}

void readLogoEnd(void)
{
#if 0
#if 1
	SetLayerFreezeC732(logoLayer,GetUserSetting()->layer[logoLayer].freeze);
#else
	#if ENABLE_DMA_DOUBLE_BUFFER
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	#endif
	LOGO_SPI_CS = 1;
#endif
#endif
	printf("End Read Logo\r\n");
}

bool DisplayLogo(char * fileName)
{
	BMPINFO fileInfo;
	u32 startTime;

	startTime = GetSysTick();
	
	if(getFileInfo(fileName, &fileInfo) == FALSE)
	{
		return FALSE;
	}
	printFileInfo( &fileInfo);
	BMP_Display(&fileInfo, fileName);

	printf("display logo time = %ds\r\n", (GetSysTick()-startTime)/1000);

	return TRUE;
}

void GetInputLayerSize(int logoLayer, int *width, int *height)
{
	int iWidth, iHeight, oWidth, oHeight;
	iWidth = GetUserSetting()->layer[logoLayer].crop.sizeX;
	iHeight = GetUserSetting()->layer[logoLayer].crop.sizeY;
	oWidth = GetUserSetting()->layer[logoLayer].scale.sizeX;
	oHeight = GetUserSetting()->layer[logoLayer].scale.sizeY;

	if(iWidth < oWidth)
	{
		*width = iWidth;
	}
	else
	{
		*width = oWidth;
	}
	if(iHeight < oHeight)
	{
		*height = iHeight;
	}
	else
	{
		*height = oHeight;
	}
	
}


bool createBMPFile(const char *fileName)
{
	int width=1920, height=1080, colorSize = 24,  headSize = BMP_FILE_HEADER_SIZE;
	FIL *fp = mymalloc(SRAMIN,sizeof(FIL));
	u8 * bmpHeader = mymalloc(SRAMIN,headSize);
	u8 * preLine;
	int i;
	int preLineSize, lines = 1;
	u32 startTime;
	int colorSpace = 0;

	startTime = GetSysTick();
	// get picture size
	GetInputLayerSize(logoLayer,&width,&height);
	
	if(fp == NULL)
	{
		printf("malloc fp error\r\n");
		return FALSE;
	}
	if(f_open(fp,fileName,FA_WRITE|FA_OPEN_ALWAYS) != FR_OK)
	{
		printf("f_open error\r\n");
		myfree(SRAMIN,fp);
		return FALSE;
	}
	
	// write file header
	printf("start write header!\r\n");
	BMP_encodeInfo(bmpHeader, width, height, colorSize);
	// store colorspace 0 -- RGB, 1 -- YUV
	if(GetUserSetting()->input[GetUserSetting()->layer[logoLayer].inputPort].colorSpace == 0)
	{
		colorSpace = 0;  // RGB
	}
	else
	{
		colorSpace = 1; // YUV
	}
	DWORD2BYTE(bmpHeader+0x32, colorSpace, 4);
	
	myWriteFile(fp, bmpHeader, headSize);
	myfree(SRAMIN, bmpHeader);
	printf("write header!\r\n");

	// write file image data
	preLineSize = BMP_PRE_LINE_SIZE(width,colorSize);
	preLine = mymalloc(SRAMIN, preLineSize*lines);
	if(preLine == NULL)
	{
		return FALSE;
	}
	for(i=0; i<lines; i++)
	{
		createTpLine(preLine+i*preLineSize, width);
	}
	printf("pre line size = %d\r\n", preLineSize*lines);
	
	readLogoStart();
	for(i=0; i<height; i++)
	{
		#if 1
		readLogoLine(height-i-1,preLine,width);
		#endif
		
		//printf("size = %d\r\n", preLineSize*(i+1));
		//while(f_sync(fp) != FR_OK);
		myWriteFile(fp, preLine, preLineSize);
		
		//delay_ms(100);
	}
	readLogoEnd();
	
	printf("write data end\r\n");
	printf("write time = %ds\r\n", (GetSysTick()-startTime)/1000);
	myCloseFile(fp);
	myfree(SRAMIN, preLine);
	myfree(SRAMIN,fp);

	return TRUE;
	
}



