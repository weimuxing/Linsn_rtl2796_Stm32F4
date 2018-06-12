#include "SysTick.h"
#include "I2C.h"

#include "it6662/include/IT6661.h"
#include "it6662/include/iTE_typedef.h"
#include "it6662/include/IT6661_config.h"
#include "it6662/include/ITE_Edid.h"

/*
EdidInfo	g_stEdidInfoAry[4];

iTE_u8 u8I2CAdr = 0;
iTE_u8	CurSysStatus;
iTE_u8 g_u8CurDev = IT6662_ROOT;
IT6662_Var	*stIt6662_A;
IT6662_Var	*stIt6662_B;
extern EdidInfo*	g_stEdidInfo;
EdidInfo*	g_stEdidInfo = g_stEdidInfoAry;
*/

extern iTE_u8 u8I2CBus;

extern IT6662_Var stIt6662Var[2];
extern EdidInfo g_stEdidInfoAry[2][4];
extern IT6662_Var	*stIt6662;
extern EdidInfo*	g_stEdidInfo;



void Write_IT6661(IT6661_TypeDef IT6661_SEL,BYTE REG_ADDR,BYTE REG_DATA)
{
	if(IT6661_SEL == IT6661_A)
		i2cWriteByte(I2C_A,IT6661A_ADD,REG_ADDR,REG_DATA);
	else if(IT6661_SEL == IT6661_B)
		i2cWriteByte(I2C_C,IT6661B_ADD,REG_ADDR,REG_DATA);
}

BYTE Read_IT6661(IT6661_TypeDef IT6661_SEL,BYTE REG_ADDR)
{
	#if 1
	BYTE readData = 0;
	
	if(IT6661_SEL == IT6661_A)
		readData = i2cReadByte(I2C_A,IT6661A_ADD,REG_ADDR);
	else if(IT6661_SEL == IT6661_B)
		readData = i2cReadByte(I2C_C,IT6661B_ADD,REG_ADDR);
	return readData;
	#else
	BYTE readData[1];
	readData[0] = 0;
	
	if(IT6661_SEL == IT6661_A)
		i2cBurstReadBytes2(I2C_A, IT6661A_ADD, REG_ADDR, readData, 1);
	else if(IT6661_SEL == IT6661_B)
		i2cBurstReadBytes2(I2C_C, IT6661B_ADD, REG_ADDR, readData, 1);
	return readData[0];
	
	
	#endif
}

//****************************************************************************
/*
void IT6662_VarInit(IT6662_Var* stCurIt6662Var)
{
	iTE_u16	u16Temp = sizeof(IT6662_Var);
	iTE_pu8	pu8Tmp = (iTE_u8 *)stCurIt6662Var;

	while(u16Temp--){
		*pu8Tmp++ = 0;
	};
	u16Temp = sizeof(EdidInfo)*4;
	pu8Tmp = (iTE_u8 *)g_stEdidInfo;
	while(u16Temp--){
		*pu8Tmp++ = 0;
	};
}
*/
//****************************************************************************

void IT6661SelDevice(IT6661_TypeDef device)
{
	if(device == IT6661_A)
	{
		stIt6662 = &stIt6662Var[0];
		g_stEdidInfo = g_stEdidInfoAry[0];
		u8I2CBus = IT6661_A;
		
	}
	else if(device == IT6661_B)
	{
		stIt6662 = &stIt6662Var[1];
		g_stEdidInfo = g_stEdidInfoAry[1];
		u8I2CBus = IT6661_B;
	}
}

void mDelay(iTE_u16 Delay_Count)
{
	SysDelay1ms(Delay_Count);
}
void mSleep(iTE_u16 Delay_Count)
{
	SysDelay1ms(Delay_Count);
}

void iTE_Sleep_ms(iTE_u16 usMsCnt)
{
	//mSleep(usMsCnt);
	//SysDelay1ms(usMsCnt-1); //@lv 20160318
	SysDelay1ms(usMsCnt); //@lv 20160318
}
iTE_u1 i2c_read_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device )
{
	if(device == IT6661_A)
		i2cBurstReadBytes(I2C_A, address, offset, p_data, byteno);
	else if(device == IT6661_B)
		i2cBurstReadBytes(I2C_C, address, offset, p_data, byteno);
	return 1;
}
iTE_u1 i2c_write_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device )
{
	if(device == IT6661_A)
		i2cBurstWriteBytes(I2C_A, address, offset, p_data, byteno);
	else if(device == IT6661_B)
		i2cBurstWriteBytes(I2C_C, address, offset, p_data, byteno);
	return 1;
}


void iTE_I2C_WriteBurst(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf)
{
#if 0
	return i2c_write_byte( u8Adr + u8I2CAdr, u8Offset, u8Cnt, pu8Buf, u8I2CBus );
#else
	if(i2c_write_byte( u8Adr + u8I2CAdr, u8Offset, u8Cnt, pu8Buf, u8I2CBus ) == 0){
		extern iTE_u8 CurSysStatus;
		CurSysStatus = 0;
	}
#endif
}

void iTE_I2C_ReadBurst(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf)
{
	if(i2c_read_byte( u8Adr + u8I2CAdr, u8Offset, u8Cnt, pu8Buf, u8I2CBus ) == 0){
		extern iTE_u8 CurSysStatus;
		CurSysStatus = 0;
	}
}

void iTE_I2C_SetByte(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8InvMask, iTE_u8 u8Data )
{
	iTE_u8	u8RdData;
	if(u8InvMask){
		if(~u8InvMask){
			if(i2c_read_byte( u8Adr + u8I2CAdr, u8Offset, 1, &u8RdData, u8I2CBus ) == 0){
				extern iTE_u8 CurSysStatus;
				CurSysStatus = 0;
			}
			u8RdData &= ~u8InvMask;
			u8Data &= u8InvMask;
			u8Data |= u8RdData;
		}
		if(i2c_write_byte( u8Adr + u8I2CAdr, u8Offset, 1, &u8Data, u8I2CBus ) == 0){
			extern iTE_u8 CurSysStatus;
			CurSysStatus = 0;
		}
	}else{
		if(u8Offset){
			// ToDo for other feature
		}else{
			iTE_Sleep_ms(u8Data);
		}
	}
}

void iTE_I2C_WriteByte(iTE_u8 u8Adr, iTE_u8 u8Offset, iTE_u8 u8Data)
{
	if(i2c_write_byte( u8Adr + u8I2CAdr, u8Offset, 1, &u8Data, u8I2CBus ) == 0){
		extern iTE_u8 CurSysStatus;
		CurSysStatus = 0;
	}
}

iTE_u8 iTE_I2C_ReadByte(iTE_u8 u8Adr, iTE_u8 u8Offset)   
{
	iTE_u8	u8RdData;
	if(i2c_read_byte( u8Adr + u8I2CAdr, u8Offset, 1, &u8RdData, u8I2CBus ) == 0){
		extern iTE_u8 CurSysStatus;
		CurSysStatus = 0;
	}
	return u8RdData;
}






