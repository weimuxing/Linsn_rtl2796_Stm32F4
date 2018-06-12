#include "THS8200.h"
#include "VideoFormatTable.h"
#include "I2c.h"

#ifdef USE_THS8200

////////////////////////////////////////////////////////////////////////////////
// Include File: THS8200_default_settings.inc
// Release Date: 11-03-2011
// For use with the TVP7002+THS8200 EVM Kit
// Default THS8200 control register settings
////////////////////////////////////////////////////////////////////////////////
const char THS8200_default_settings[] =
{
0x03,0x01, // chip_ctl
0x04,0x00, // csc_ric1
0x05,0x00, // csc_rfc1
0x06,0x00, // csc_ric2
0x07,0x00, // csc_rfc2
0x08,0x00, // csc_ric3
0x09,0x00, // csc_rfc3
0x0A,0x00, // csc_gic1
0x0B,0x00, // csc_gfc1
0x0C,0x00, // csc_gic2
0x0D,0x00, // csc_gfc2
0x0E,0x00, // csc_gic3
0x0F,0x00, // csc_gfc3
0x10,0x00, // csc_bic1
0x11,0x00, // csc_bfc1
0x12,0x00, // csc_bic2
0x13,0x00, // csc_bfc2
0x14,0x00, // csc_bic3
0x15,0x00, // csc_bfc3
0x16,0x00, // csc_offset1 
0x17,0x00, // csc_offset12
0x18,0x00, // csc_offset23
0x19,0x03, // csc_offset3 
0x1A,0x00, // tst_cntl
0x1B,0x00, // tst_ramp_cntl 
0x1C,0x03, // dman_cntl 
0x1D,0x00, // dtg_y_sync1 
0x1E,0x00, // dtg_y_sync2 
0x1F,0x00, // dtg_y_sync3 
0x20,0x00, // dtg_cbcr_sync1
0x21,0x00, // dtg_cbcr_sync2
0x22,0x00, // dtg_cbcr_sync3
0x23,0x23, // dtg_y_sync_upper
0x24,0x23, // dtg_cbcr_sync_upper 
0x25,0x2C, // dtg_spec_a
0x26,0x58, // dtg_spec_b
0x27,0x2C, // dtg_spec_c
0x28,0x84, // dtg_spec_d
0x29,0x00, // dtg_spec_d1 
0x2A,0xC0, // dtg_spec_e
0x2B,0x00, // dtg_spec_h_msb
0x2C,0x00, // dtg_spec_h_lsb
0x2D,0x00, // dtg_spec_i_msb
0x2E,0x00, // dtg_spec_i_lsb
0x2F,0x58, // dtg_spec_k_lsb
0x30,0x00, // dtg_spec_k_msb
0x31,0x00, // dtg_spec_k1 
0x32,0x58, // dtg_speg_g_lsb
0x33,0x00, // dtg_speg_g_msb
0x34,0x05, // dtg_total_pixel_msb 
0x35,0x20, // dtg_total_pixel_lsb 
0x36,0x00, // dtg_linecnt_msb 
0x37,0x01, // dtg_linecnt_lsb 
0x38,0x86, // dtg_mode
0x39,0x30, // dtg_frame_field_msb 
0x3A,0x00, // dtg_frame_size_lsb
0x3B,0x20, // dtg_field_size_lsb
0x3C,0x80, // dtg_vesa_cbar_size
0x3D,0x00, // dac_upper 
0x3E,0x00, // dac1_test 
0x3F,0x00, // dac2_test 
0x40,0x00, // dac3_test 
0x41,0x40, // csm_clip_gy_low 
0x42,0x40, // csm_clip_bcb_low
0x43,0x40, // csm_clip_rcr_low
0x44,0x53, // csm_clip_gy_high
0x45,0x3F, // csm_clip_bcb_high 
0x46,0x3F, // csm_clip_rcr_high 
0x47,0x40, // csm_shift_gy
0x48,0x40, // csm_shift_bcb 
0x49,0x40, // csm_shift_rcr 
0x4A,0x08, // csm_mult_gy_msb 
0x4B,0x00, // csm_mult_bcb_rcr_msb
0x4C,0x00, // csm_mult_gy_lsb 
0x4D,0x00, // csm_mult_bcb_lsb
0x4E,0x00, // csm_mult_rcr_lsb
0x4F,0x00, // csm_mode
0x50,0x00, // dtg_bp1_2_msb 
0x51,0x00, // dtg_bp3_4_msb 
0x52,0x00, // dtg_bp5_6_msb 
0x53,0x00, // dtg_bp7_8_msb 
0x54,0x00, // dtg_bp9_10_msb
0x55,0x00, // dtg_bp11_12_msb 
0x56,0x00, // dtg_bp13_14_msb 
0x57,0x00, // dtg_bp15_16_msb 
0x58,0x00, // dtg_bp1_lsb 
0x59,0x00, // dtg_bp2_lsb 
0x5A,0x00, // dtg_bp3_lsb 
0x5B,0x00, // dtg_bp4_lsb 
0x5C,0x00, // dtg_bp5_lsb 
0x5D,0x00, // dtg_bp6_lsb 
0x5E,0x00, // dtg_bp7_lsb 
0x5F,0x00, // dtg_bp8_lsb 
0x60,0x00, // dtg_bp9_lsb 
0x61,0x00, // dtg_bp10_lsb
0x62,0x00, // dtg_bp11_lsb
0x63,0x00, // dtg_bp12_lsb
0x64,0x00, // dtg_bp13_lsb
0x65,0x00, // dtg_bp14_lsb
0x66,0x00, // dtg_bp15_lsb
0x67,0x00, // dtg_bp16_lsb
0x68,0x00, // dtg_linetype1 
0x69,0x00, // dtg_linetype2 
0x6A,0x00, // dtg_linetype3 
0x6B,0x00, // dtg_linetype4 
0x6C,0x00, // dtg_linetype5 
0x6D,0x00, // dtg_linetype6 
0x6E,0x00, // dtg_linetype7 
0x6F,0x00, // dtg_linetype8 
0x70,0x60, // dtg_hlength_lsb 
0x71,0x00, // dtg_hdly_msb
0x72,0x02, // dtg_hdly_lsb
0x73,0x03, // dtg_vlength_lsb 
0x74,0x00, // dtg_vdly_msb
0x75,0x03, // dtg_vdly_lsb
0x76,0x00, // dtg_vlength2_lsb
0x77,0x07, // dtg_vdly2_msb 
0x78,0xFF, // dtg_vdly2_lsb 
0x79,0x00, // dtg_hs_in_dly_msb 
0x7A,0x3D, // dtg_hs_in_dly_lsb 
0x7B,0x00, // dtg_vs_in_dly_msb 
0x7C,0x03, // dtg_vs_in_dly_lsb 
0x82,0x5F, // pol_cntl
0x83,0x00, // cgms_header 
0x84,0x00, // cgms_payload_msb
0x85,0x00, // cgms_payload_lsb
};

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Include File: THS8200_RGB_444_DS_to_VGA_common.inc
// Release Date: 11-08-2011
// For use with the TVP7002+THS8200 EVM Kit
// Common THS8200 control register settings for 
// - Input: 30-bit RGB 4:4:4 with discrete syncs
// - Output: 5-wire VGA (RGBHV)
////////////////////////////////////////////////////////////////////////////////
const char THS8200_common_settings[] =
{
	0x03,0xC1,
	0x19,0x03,
	0x1c,0x78,
	0x23,0x00,
	0x24,0x00,
	0x36,0x80,
	0x37,0x01,
	0x38,0x87,
	0x4a,0x8c,
	0x4b,0x44,
	0x4c,0x00,
	0x4d,0x00,
	0x4e,0x00,
	0x4f,0xc0,
	0x71,0x00,
	0x72,0x01,
	0x74,0x00,
	0x75,0x01,
	0x76,0x00,
	0x77,0x07,
	0x78,0xff,
	0x79,0x00,
	0x7a,0x00,
	0x7b,0x00,
	0x7c,0x00,

};

////////////////////////////////////////////////////////////////////////////////


#define THS8200_I2C_SLAVE_ADDRESS (0x40)
void THS8200_WriteReg(int address, int value)
{
	//i2cWriteByte(I2C_A,THS8200_I2C_SLAVE_ADDRESS,address,value);
}
int THS8200_ReadReg(int address)
{
	return 0;//i2cReadByte(I2C_A,THS8200_I2C_SLAVE_ADDRESS,address);
}

void THS8200_SetDefaultValue(void)
{
	int i;
	int size = sizeof(THS8200_default_settings)/2;
	for(i=0; i<size; i++)
	{
		THS8200_WriteReg(THS8200_default_settings[i*2], THS8200_default_settings[i*2+1]);
	}
}

void THS8200_SetCommonValue(void)
{
	int i;
	int size = sizeof(THS8200_common_settings)/2;
	for(i=0; i<size; i++)
	{
		THS8200_WriteReg(THS8200_common_settings[i*2], THS8200_common_settings[i*2+1]);
	}
}

const char THS8200_1920x1080x60_settings[] =
{
	0x34,0x08, // dtg1_total_pixel_msb
	0x35,0x98, // dtg1_total_pixel_lsb
	0x39,0x44, // dtg1_frame_field_msb
	0x3A,0x65, // dtg1_frame_size_lsb
	0x3B,0x65, // dtg1_field_size_lsb
	0x70,0x2C, // dtg2_hlength_lsb
	0x73,0x06, // dtg2_vlength_lsb
	0x82,0xDB, // dtg2_cntl, RGB mode, discrete syncs
};

void THS8200_SetHTotal(int value)
{
	THS8200_WriteReg(0x35, value);
	THS8200_WriteReg(0x34, value>>8);
}

void THS8200_SetVTotal(int value)
{
	THS8200_WriteReg(0x3A, value);
	THS8200_WriteReg(0x3B, value);
	value = (value>>8)&0x07;
	THS8200_WriteReg(0x39, value|(value<<4));
}

void THS8200_SetHSyncWidth(int value)
{
	THS8200_WriteReg(0x70, value);
}

void THS8200_SetVSyncWidth(int value)
{
	THS8200_WriteReg(0x73, value);
}


void THS8200_SetHVPolarity(int inHs, int inVs, int outHs, int outVs) // 0  -- Neg, 1 -- Pos
{
	int defaultValue = 0xC0;
	if(inHs)
	{
		defaultValue |= 0x01;
	}
	if(inVs)
	{
		defaultValue |= 0x02;
	}
	if(outHs)
	{
		defaultValue |= 0x08;
	}
	if(outVs)
	{
		defaultValue |= 0x10;
	}
	THS8200_WriteReg(0x82, defaultValue);
}

void THS8200_SetOutputFormat(int format)
{
	int hs, vs;
	
	hs = GetHSyncPolarityByVideoFormat(format);
	vs = GetVSyncPolarityByVideoFormat(format);
	
	hs = hs?0:1;
	vs = vs?0:1;
	
	THS8200_SetDefaultValue();
	THS8200_SetCommonValue();

	THS8200_SetHTotal(GetHTotalByVideoFormat(format));
	THS8200_SetVTotal(GetVTotalByVideoFormat(format));
	THS8200_SetHSyncWidth(GetHSyncWidthByVideoFormat(format));
	THS8200_SetVSyncWidth(GetVSyncWidthByVideoFormat(format));
	THS8200_SetHVPolarity(hs, vs, hs, vs);
	
}

char CheckTHS8200(void)
{
	char i,res,readValue,checkValue[5] = {0x3c,0x46,0xaa,0xb8,0xe9};
	for(i = 0; i < 5; i++)
	{
		THS8200_WriteReg(0x35, checkValue[i]);
		readValue = THS8200_ReadReg(0x35);
		if(readValue != checkValue[i])
		{
			res = FALSE;
			return res;
		}
	}
	res = TRUE;
	return res;	
}

#endif		// #ifdef USE_THS8200

