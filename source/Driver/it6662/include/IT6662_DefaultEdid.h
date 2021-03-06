///*****************************************
//  Copyright (C) 2009-2015
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_DefaultEdid.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2015/10/28
//   @fileversion: ITE_SPLITER_1.16
//******************************************/
///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6662_DefaultEdid.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2014/12/17
//   @fileversion: ITE_SPLITER_1.01
//******************************************/
#ifndef _IT6662_DEFAULTEDID_H_
#define _IT6662_DEFAULTEDID_H_

#ifdef _MHL_MSC_INIT_TAB_
//#define EDIDContentSel		(3)		// 1: 3D 8bit ,2: Big Pattern gen ,3: 4Kx2K
iTE_u8 _CODE uc3DDtd[]={0x00};		//unsupport 3D DTD information

#if (USING_DEFAULT_3D_VIC == iTE_TRUE)
iTE_u8 _CODE uc3DVic[]={0x00};
#endif

#if (USING_DEFAULT_3D_VIC == iTE_TRUE)
#define USING_CALLOC				(iTE_FALSE)		//calloc will not use when USING_DEFAULT_3D_VIC set
#else
#define USING_CALLOC				(iTE_FALSE)		//set if system support calloc function
#endif

#if (USING_CALLOC==iTE_FALSE)
#define VIC_3D_MAX					(16)
#if(VIC_3D_MAX < 16)
	#pragma message("****ERROR****: VIC_3D_MAX need bigger than 16")
#endif
#endif

#if (USING_CALLOC == iTE_TRUE)
#include <stdlib.h>
#else
iTE_u8	uc3DInfor[VIC_3D_MAX];
#endif
#endif

#ifdef _EDID_TAB_

#if 0
iTE_u8	u8DefDisplayDescriptorPName[13] = {0x69, 0x54, 0x45, 0x2D, 0x43, 0x6F, 0x6D, 0x70, 0x6F, 0x73, 0x65, 0x20, 0x20};

iTE_u8	u8EdidFHD[256] = {
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x26, 0x85, 0x62, 0x66, 0x01, 0x00, 0x00, 0x00,
0x0C, 0x14, 0x01, 0x03, 0x80, 0x3E, 0x22, 0x78, 0x0A, 0x1E, 0xAC, 0x98, 0x59, 0x56, 0x85, 0x28,
0x29, 0x52, 0x57, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
0x45, 0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x1E, 0xD5, 0x09, 0x80, 0xA0, 0x20, 0xE0, 0x2D, 0x10,
0x10, 0x60, 0xA2, 0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x49,
0x54, 0x45, 0x2D, 0x46, 0x48, 0x44, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
0x00, 0x17, 0x3D, 0x0D, 0x2E, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x61,

0x02, 0x03, 0x18, 0xF3, 0x43, 0x90, 0x04, 0x03, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
0x67, 0x03, 0x0C, 0x00, DefaultEdid_PA_AB, DefaultEdid_PA_CD, 0x00, 0x2D, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40,
0x58, 0x2C, 0x45, 0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0,
0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x1E, 0xD5, 0x09, 0x80, 0xA0,
0x20, 0xE0, 0x2D, 0x10, 0x10, 0x60, 0xA2, 0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE - DefaultEdid_PA_AB - DefaultEdid_PA_CD ,
};
iTE_u8	u8Edid4K2K[256] = {
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x26, 0x85, 0x62, 0x66, 0x01, 0x00, 0x00, 0x00,
0x0C, 0x14, 0x01, 0x03, 0x80, 0x3E, 0x22, 0x78, 0x0A, 0x1E, 0xAC, 0x98, 0x59, 0x56, 0x85, 0x28,
0x29, 0x52, 0x57, 0xFF, 0xFF, 0x80, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x04, 0x74, 0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80, 0xB0, 0x58,
0x8A, 0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x1A, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40,
0x58, 0x2C, 0x45, 0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x49,
0x54, 0x45, 0x2D, 0x55, 0x48, 0x44, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
0x00, 0x17, 0x3D, 0x0D, 0x2E, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xC9,

0x02, 0x03, 0x21, 0xF3, 0x46, 0x90, 0x04, 0x03, 0x5F, 0x5E, 0x5D, 0x23, 0x09, 0x07, 0x07, 0x83,
0x01, 0x00, 0x00, 0x6D, 0x03, 0x0C, 0x00, DefaultEdid_PA_AB, DefaultEdid_PA_CD, 0x00, 0x3C, 0x20, 0x00, 0x60, 0x01, 0x02,
0x03, 0x04, 0x74, 0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80, 0xB0, 0x58, 0x8A, 0x00, 0x6D, 0x55, 0x21,
0x00, 0x00, 0x1A, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x6D,
0x55, 0x21, 0x00, 0x00, 0x1E, 0xD5, 0x09, 0x80, 0xA0, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x60, 0xA2,
0x00, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85 - DefaultEdid_PA_AB - DefaultEdid_PA_CD ,
};
#else
iTE_u8	u8DefDisplayDescriptorPName[13] = {0x52,  0x47,  0x42,  0x48,	0x44,  0x4d,  0x49,  0x0a,	0x20,  0x20,  0x20,  0x20,	0x20};

iTE_u8	u8EdidFHD[256] = {
	0x00,  0xff,  0xff,  0xff,	0xff,  0xff,  0xff,  0x00,	0x48,  0xe2,  0x22,  0x38,	0x01,  0x01,  0x01,  0x01, 
		 0x07,	0x13,  0x01,  0x03,  0x80,	0x34,  0x1d,  0x78,  0x2e,	0xee,  0xd5,  0xa5,  0x55,	0x48,  0x9b,  0x26, 
		 0x12,	0x50,  0x54,  0x21,  0x08,	0x00,  0xd1,  0x00,  0xb3,	0x00,  0xa9,  0x40,  0x95,	0x00,  0xe1,  0xc0, 
		 0x81,	0x80,  0x90,  0x40,  0x81,	0x00,  0x02,  0x3a,  0x80,	0x18,  0x71,  0x38,  0x2d,	0x40,  0x58,  0x2c, 
		 0x45,	0x00,  0x08,  0x22,  0x21,	0x00,  0x00,  0x1e,  0x00,	0x00,  0x00,  0xff,  0x00,	0x52,  0x38,  0x57, 
		 0x30,	0x39,  0x30,  0x37,  0x30,	0x31,  0x36,  0x37,  0x30,	0x0a,  0x00,  0x00,  0x00,	0xfd,  0x00,  0x32, 
		 0x4b,	0x0f,  0x52,  0x12,  0x00,	0x0a,  0x20,  0x20,  0x20,	0x20,  0x20,  0x20,  0x00,	0x00,  0x00,  0xfc, 
		 0x00,	0x52,  0x47,  0x42,  0x48,	0x44,  0x4d,  0x49,  0x0a,	0x20,  0x20,  0x20,  0x20,	0x20,  0x01,  0x20, 
		 0x02,	0x03,  0x25,  0xf1,  0x52,	0x90,  0x05,  0x04,  0x03,	0x02,  0x07,  0x05,  0x0f,	0x0e,  0x1f,  0x14, 
		 0x1e,	0x1d,  0x13,  0x12,  0x11,	0x16,  0x01,  0x23,  0x09,	0x07,  0x07,  0x83,  0x01,	0x00,  0x00,  0x65, 
		 0x03,	0x0c,  0x00,  0x10,  0x00,	0x02,  0x3a,  0x80,  0x18,	0x71,  0x38,  0x2d,  0x40,	0x58,  0x2c,  0x45, 
		 0x00,	0x08,  0x22,  0x21,  0x00,	0x00,  0x1e,  0x84,  0x35,	0x00,  0xa0,  0xa0,  0x30,	0x18,  0x30,  0x30, 
		 0x20,	0x3a,  0x00,  0x08,  0x22,	0x21,  0x00,  0x00,  0x1a,	0x01,  0x1d,  0x00,  0x72,	0x51,  0xd0,  0x1e, 
		 0x20,	0x6e,  0x28,  0x55,  0x00,	0x08,  0x22,  0x21,  0x00,	0x00,  0x1e,  0x02,  0x3a,	0x80,  0xd0,  0x72, 
		 0x38,	0x2d,  0x40,  0x10,  0x2c,	0x45,  0x80,  0x08,  0x22,	0x21,  0x00,  0x00,  0x1e,	0x8c,  0x0a,  0xd0, 
		 0x8a,	0x20,  0xe0,  0x2d,  0x10,	0x10,  0x3e,  0x96,  0x00,	0x08,  0x22,  0x21,  0x00,	0x00,  0x18,  0x8f,

};
iTE_u8	u8Edid4K2K[256] = {
	0x00,  0xff,  0xff,  0xff,	0xff,  0xff,  0xff,  0x00,	0x48,  0xe2,  0x22,  0x38,	0x01,  0x01,  0x01,  0x01, 
		 0x07,	0x13,  0x01,  0x03,  0x80,	0x34,  0x1d,  0x78,  0x2e,	0xee,  0xd5,  0xa5,  0x55,	0x48,  0x9b,  0x26, 
		 0x12,	0x50,  0x54,  0x21,  0x08,	0x00,  0xd1,  0x00,  0xb3,	0x00,  0xa9,  0x40,  0x95,	0x00,  0xe1,  0xc0, 
		 0x81,	0x80,  0x90,  0x40,  0x81,	0x00,  0x02,  0x3a,  0x80,	0x18,  0x71,  0x38,  0x2d,	0x40,  0x58,  0x2c, 
		 0x45,	0x00,  0x08,  0x22,  0x21,	0x00,  0x00,  0x1e,  0x00,	0x00,  0x00,  0xff,  0x00,	0x52,  0x38,  0x57, 
		 0x30,	0x39,  0x30,  0x37,  0x30,	0x31,  0x36,  0x37,  0x30,	0x0a,  0x00,  0x00,  0x00,	0xfd,  0x00,  0x32, 
		 0x4b,	0x0f,  0x52,  0x12,  0x00,	0x0a,  0x20,  0x20,  0x20,	0x20,  0x20,  0x20,  0x00,	0x00,  0x00,  0xfc, 
		 0x00,	0x52,  0x47,  0x42,  0x48,	0x44,  0x4d,  0x49,  0x0a,	0x20,  0x20,  0x20,  0x20,	0x20,  0x01,  0x20, 
		 0x02,	0x03,  0x25,  0xf1,  0x52,	0x90,  0x05,  0x04,  0x03,	0x02,  0x07,  0x05,  0x0f,	0x0e,  0x1f,  0x14, 
		 0x1e,	0x1d,  0x13,  0x12,  0x11,	0x16,  0x01,  0x23,  0x09,	0x07,  0x07,  0x83,  0x01,	0x00,  0x00,  0x65, 
		 0x03,	0x0c,  0x00,  0x10,  0x00,	0x02,  0x3a,  0x80,  0x18,	0x71,  0x38,  0x2d,  0x40,	0x58,  0x2c,  0x45, 
		 0x00,	0x08,  0x22,  0x21,  0x00,	0x00,  0x1e,  0x84,  0x35,	0x00,  0xa0,  0xa0,  0x30,	0x18,  0x30,  0x30, 
		 0x20,	0x3a,  0x00,  0x08,  0x22,	0x21,  0x00,  0x00,  0x1a,	0x01,  0x1d,  0x00,  0x72,	0x51,  0xd0,  0x1e, 
		 0x20,	0x6e,  0x28,  0x55,  0x00,	0x08,  0x22,  0x21,  0x00,	0x00,  0x1e,  0x02,  0x3a,	0x80,  0xd0,  0x72, 
		 0x38,	0x2d,  0x40,  0x10,  0x2c,	0x45,  0x80,  0x08,  0x22,	0x21,  0x00,  0x00,  0x1e,	0x8c,  0x0a,  0xd0, 
		 0x8a,	0x20,  0xe0,  0x2d,  0x10,	0x10,  0x3e,  0x96,  0x00,	0x08,  0x22,  0x21,  0x00,	0x00,  0x18,  0x8f,

};

#endif
#endif
#endif
