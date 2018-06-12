/*
 * bmp.h
 *
 *  Created on: 2010-3-2
 *      Author: Administrator
 */

#ifndef BMP_H_
#define BMP_H_

#include "define.h"
#include "integer.h"

#define BMP_FILE_HEADER_SIZE (512)

#define BMP_FILE_ID (0x4D42)

//BMP自定义文件信息
typedef struct tagBMPINFO
{
	WORD isValid;		// 是否是有效的位图文件
	WORD colorSize; 	// 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一
	WORD width;			// 位图的宽度，以像素为单位
	WORD height;		// 位图的高度，以像素为单位
	DWORD fileSize;		// 位图文件的大小，以字节为单位
	DWORD dataOffset;	// 位图数据的起始位置，以相对于位图
	DWORD dataSize;		// 位图的大小，以字节为单位
	BYTE colorSpace; // 0 -- RGB, 1 -- YUV
} BMPINFO;



//BMP文件头数据结构含有BMP文件的类型、文件大小和位图起始位置等信息。
typedef struct tagBITMAPFILEHEADER
{
	WORD  bfType; // 位图文件的类型，必须为BM
	DWORD bfSize; // 位图文件的大小，以字节为单位
	WORD  bfReserved1; // 位图文件保留字，必须为0
	WORD  bfReserved2; // 位图文件保留字，必须为0
	DWORD bfOffBits; // 位图数据的起始位置，以相对于位图
	// 文件头的偏移量表示，以字节为单位
} BITMAPFILEHEADER;  // 14 BYTE

//BMP位图信息头数据用于说明位图的尺寸等信息。
typedef struct tagBITMAPINFOHEADER {
	DWORD biSize; // 本结构所占用字节数
	LONG  biWidth; // 位图的宽度，以像素为单位
	LONG  biHeight; // 位图的高度，以像素为单位
	WORD biPlanes; // 目标设备的级别，必须为1
	WORD biBitCount; // 每个像素所需的位数，必须是1(双色),
	// 4(16色)，8(256色)或24(真彩色)之一
	DWORD biCompression; // 位图压缩类型，必须是 0(不压缩),
	// 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
	DWORD biSizeImage; // 位图的大小，以字节为单位
	LONG  biXPelsPerMeter; // 位图水平分辨率，每米像素数
	LONG  biYPelsPerMeter; // 位图垂直分辨率，每米像素数
	DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数
	DWORD biClrImportant;// 位图显示过程中重要的颜色数
} BITMAPINFOHEADER;

// 24色数据的排列顺序是BGR

//颜色表用于说明位图中的颜色，它有若干个表项，每一个表项是一个RGBQUAD类型的结构，定义一种颜色。
typedef struct tagRGBQUAD {
	BYTE rgbBlue;// 蓝色的亮度(值范围为0-255)
	BYTE rgbGreen; // 绿色的亮度(值范围为0-255)
	BYTE rgbRed; // 红色的亮度(值范围为0-255)
	BYTE rgbReserved;// 保留，必须为0
} RGBQUAD;

//　颜色表中RGBQUAD结构数据的个数有biBitCount来确定:
//　当biBitCount=1,4,8时，分别有2,16,256个表项;
//　当biBitCount=24时，没有颜色表项。
//　位图信息头和颜色表组成位图信息，BITMAPINFO结构定义如下:
typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader; // 位图信息头
	RGBQUAD bmiColors[1]; // 颜色表
} BITMAPINFO;
//  位图数据
//　位图数据记录了位图的每一个像素值，记录顺序是在扫描行内是从左到右,扫描行之间是从下到上。位图的一个像素值所占的字节数:
//    当biBitCount=1时，8个像素占1个字节;
//　　当biBitCount=4时，2个像素占1个字节;
//　　当biBitCount=8时，1个像素占1个字节;
//　　当biBitCount=24时,1个像素占3个字节;
//　　Windows规定一个扫描行所占的字节数必须是
//　　4的倍数(即以long为单位),不足的以0填充，
//　　一个扫描行所占的字节数计算方法:
//　　DataSizePerLine= (biWidth* biBitCount+31)/8;
//　　// 一个扫描行所占的字节数
//　　DataSizePerLine= DataSizePerLine/4*4; // 字节数必须是4的倍数
//　　位图数据的大小(不压缩情况下):
//　　DataSize= DataSizePerLine* biHeight;

#define BMP_PRE_LINE_SIZE(width, colorSize) ((width*colorSize+31)/8/4*4)
bool BMP_encodeInfo(BYTE *bmpHeaderBuf, int width, int height, int colorSize);
bool BMP_decodeInfo(BYTE *bmpHeaderBuf, BMPINFO *bmpInfo);
bool createBMPFile(const char *fileName);
bool DisplayLogo(char * fileName);
bool BMP_getFileExist(char * fileName);
void SetLogoLayer(int layer);
void DisableLogoDisplay(void);






#endif /* BMP_H_ */
