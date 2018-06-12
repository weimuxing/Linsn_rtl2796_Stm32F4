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

//BMP�Զ����ļ���Ϣ
typedef struct tagBMPINFO
{
	WORD isValid;		// �Ƿ�����Ч��λͼ�ļ�
	WORD colorSize; 	// ÿ�����������λ����������1(˫ɫ), 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ
	WORD width;			// λͼ�Ŀ�ȣ�������Ϊ��λ
	WORD height;		// λͼ�ĸ߶ȣ�������Ϊ��λ
	DWORD fileSize;		// λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ
	DWORD dataOffset;	// λͼ���ݵ���ʼλ�ã��������λͼ
	DWORD dataSize;		// λͼ�Ĵ�С�����ֽ�Ϊ��λ
	BYTE colorSpace; // 0 -- RGB, 1 -- YUV
} BMPINFO;



//BMP�ļ�ͷ���ݽṹ����BMP�ļ������͡��ļ���С��λͼ��ʼλ�õ���Ϣ��
typedef struct tagBITMAPFILEHEADER
{
	WORD  bfType; // λͼ�ļ������ͣ�����ΪBM
	DWORD bfSize; // λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ
	WORD  bfReserved1; // λͼ�ļ������֣�����Ϊ0
	WORD  bfReserved2; // λͼ�ļ������֣�����Ϊ0
	DWORD bfOffBits; // λͼ���ݵ���ʼλ�ã��������λͼ
	// �ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ
} BITMAPFILEHEADER;  // 14 BYTE

//BMPλͼ��Ϣͷ��������˵��λͼ�ĳߴ����Ϣ��
typedef struct tagBITMAPINFOHEADER {
	DWORD biSize; // ���ṹ��ռ���ֽ���
	LONG  biWidth; // λͼ�Ŀ�ȣ�������Ϊ��λ
	LONG  biHeight; // λͼ�ĸ߶ȣ�������Ϊ��λ
	WORD biPlanes; // Ŀ���豸�ļ��𣬱���Ϊ1
	WORD biBitCount; // ÿ�����������λ����������1(˫ɫ),
	// 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ
	DWORD biCompression; // λͼѹ�����ͣ������� 0(��ѹ��),
	// 1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ
	DWORD biSizeImage; // λͼ�Ĵ�С�����ֽ�Ϊ��λ
	LONG  biXPelsPerMeter; // λͼˮƽ�ֱ��ʣ�ÿ��������
	LONG  biYPelsPerMeter; // λͼ��ֱ�ֱ��ʣ�ÿ��������
	DWORD biClrUsed;// λͼʵ��ʹ�õ���ɫ���е���ɫ��
	DWORD biClrImportant;// λͼ��ʾ��������Ҫ����ɫ��
} BITMAPINFOHEADER;

// 24ɫ���ݵ�����˳����BGR

//��ɫ������˵��λͼ�е���ɫ���������ɸ����ÿһ��������һ��RGBQUAD���͵Ľṹ������һ����ɫ��
typedef struct tagRGBQUAD {
	BYTE rgbBlue;// ��ɫ������(ֵ��ΧΪ0-255)
	BYTE rgbGreen; // ��ɫ������(ֵ��ΧΪ0-255)
	BYTE rgbRed; // ��ɫ������(ֵ��ΧΪ0-255)
	BYTE rgbReserved;// ����������Ϊ0
} RGBQUAD;

//����ɫ����RGBQUAD�ṹ���ݵĸ�����biBitCount��ȷ��:
//����biBitCount=1,4,8ʱ���ֱ���2,16,256������;
//����biBitCount=24ʱ��û����ɫ���
//��λͼ��Ϣͷ����ɫ�����λͼ��Ϣ��BITMAPINFO�ṹ��������:
typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader; // λͼ��Ϣͷ
	RGBQUAD bmiColors[1]; // ��ɫ��
} BITMAPINFO;
//  λͼ����
//��λͼ���ݼ�¼��λͼ��ÿһ������ֵ����¼˳������ɨ�������Ǵ�����,ɨ����֮���Ǵ��µ��ϡ�λͼ��һ������ֵ��ռ���ֽ���:
//    ��biBitCount=1ʱ��8������ռ1���ֽ�;
//������biBitCount=4ʱ��2������ռ1���ֽ�;
//������biBitCount=8ʱ��1������ռ1���ֽ�;
//������biBitCount=24ʱ,1������ռ3���ֽ�;
//����Windows�涨һ��ɨ������ռ���ֽ���������
//����4�ı���(����longΪ��λ),�������0��䣬
//����һ��ɨ������ռ���ֽ������㷽��:
//����DataSizePerLine= (biWidth* biBitCount+31)/8;
//����// һ��ɨ������ռ���ֽ���
//����DataSizePerLine= DataSizePerLine/4*4; // �ֽ���������4�ı���
//����λͼ���ݵĴ�С(��ѹ�������):
//����DataSize= DataSizePerLine* biHeight;

#define BMP_PRE_LINE_SIZE(width, colorSize) ((width*colorSize+31)/8/4*4)
bool BMP_encodeInfo(BYTE *bmpHeaderBuf, int width, int height, int colorSize);
bool BMP_decodeInfo(BYTE *bmpHeaderBuf, BMPINFO *bmpInfo);
bool createBMPFile(const char *fileName);
bool DisplayLogo(char * fileName);
bool BMP_getFileExist(char * fileName);
void SetLogoLayer(int layer);
void DisableLogoDisplay(void);






#endif /* BMP_H_ */
