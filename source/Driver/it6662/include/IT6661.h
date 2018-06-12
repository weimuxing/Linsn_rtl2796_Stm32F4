#ifndef _IT6661_H
#define _IT6661_H

#include "define.h"
#include "it6662/include/iTE_typedef.h"



#define IT6661A_ADD 0x90
#define IT6661B_ADD 0x92
/*
#define u8I2CBus		(0)
#define iTE_uDef	(iTE_u16)
*/

typedef enum
{
	IT6661_A = 0,
	IT6661_B = 1,
}IT6661_TypeDef;

//extern iTE_u8 u8I2CAdr;
//extern IT6662_Var	*stIt6662_A;
//extern IT6662_Var	*stIt6662_B;


void Write_IT6661(IT6661_TypeDef IT6661_SEL,BYTE REG_ADDR,BYTE REG_DATA);
BYTE Read_IT6661(IT6661_TypeDef IT6661_SEL,BYTE REG_ADDR);
void IT6662_VarInit(IT6662_Var* stCurIt6662Var);
void IT6661SelDevice(IT6661_TypeDef device);

void iTE_Sleep_ms(iTE_u16 usMsCnt);
void iTE_I2C_WriteBurst(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);
void iTE_I2C_ReadBurst(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);
void iTE_I2C_SetByte(iTE_u8 u8Adr,iTE_u8 u8Offset,iTE_u8 u8InvMask, iTE_u8 u8Data );
void iTE_I2C_WriteByte(iTE_u8 u8Adr, iTE_u8 u8Offset, iTE_u8 u8Data);
iTE_u8 iTE_I2C_ReadByte(iTE_u8 u8Adr, iTE_u8 u8Offset);

#endif




