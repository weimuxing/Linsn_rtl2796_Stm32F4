#include "I2C.h"
#include "define.h"

#define ENABLE_DEBUG_I2C 1
#if ENABLE_DEBUG_I2C
#include "uart.h"
#endif

#include "stm32f4xx_gpio.h"

extern BYTE IT6604ack;


void SET_SDA(void);
void CLEAR_SDA(void);
void SET_SCL(void);
void CLEAR_SCL(void);
void i2c_Delay(void);
bool i2c_Start(void);
void i2c_Stop(void);
BYTE i2c_ReceiveByte(bool bAck);
BitAction i2c_SendByte(BYTE ucVal);
bool i2c_AccessStart(BYTE ucSlaveAdr, I2cIoTransType trans_t);
static I2CPort_TypeDef i2cPort = I2C_A;

GPIO_TypeDef * const sclPort[I2C_MAX_SIZE] =
{
	I2C_A_SCL_PORT,
	I2C_B_SCL_PORT,
	I2C_C_SCL_PORT,
	I2C_D_SCL_PORT,
	I2C_E_SCL_PORT,
	I2C_F_SCL_PORT,
	I2C_G_SCL_PORT,
	I2C_H_SCL_PORT,
	I2C_I_SCL_PORT,
	I2C_J_SCL_PORT,
};
GPIO_TypeDef * const sdaPort[I2C_MAX_SIZE] =
{
	I2C_A_SDA_PORT,
	I2C_B_SDA_PORT,
	I2C_C_SDA_PORT,
	I2C_D_SDA_PORT,
	I2C_E_SDA_PORT,
	I2C_F_SDA_PORT,
	I2C_G_SDA_PORT,
	I2C_H_SDA_PORT,
	I2C_I_SDA_PORT,
	I2C_J_SDA_PORT,
};

const u16 sclPin[I2C_MAX_SIZE] =
{
	I2C_A_SCL_PIN,
	I2C_B_SCL_PIN,
	I2C_C_SCL_PIN,
	I2C_D_SCL_PIN,
	I2C_E_SCL_PIN,
	I2C_F_SCL_PIN,
	I2C_G_SCL_PIN,
	I2C_H_SCL_PIN,
	I2C_I_SCL_PIN,
	I2C_J_SCL_PIN,
};

const u16 sdaPin[I2C_MAX_SIZE] =
{
	I2C_A_SDA_PIN,
	I2C_B_SDA_PIN,
	I2C_C_SDA_PIN,
	I2C_D_SDA_PIN,
	I2C_E_SDA_PIN,
	I2C_F_SDA_PIN,
	I2C_G_SDA_PIN,
	I2C_H_SDA_PIN,
	I2C_I_SDA_PIN,
	I2C_J_SDA_PIN,
};


BitAction GetPinI2cSCL()
{
	GPIO_TypeDef *gpio_x;
	u32 gpio_pin;
	if(i2cPort < I2C_MAX_SIZE)
	{
		gpio_x = sclPort[i2cPort];
		gpio_pin = sclPin[i2cPort];
	}	
	else
	{
		return Bit_SET;
	}
	return (BitAction) GPIO_ReadInputDataBit(gpio_x, gpio_pin);

}
BitAction GetPinI2cSDA()
{
	
	GPIO_TypeDef *gpio_x;
	u32 gpio_pin;
	if(i2cPort < I2C_MAX_SIZE)
	{
		gpio_x = sdaPort[i2cPort];
		gpio_pin = sdaPin[i2cPort];
	}
	else
	{
		return Bit_SET;
	}
	return (BitAction)GPIO_ReadInputDataBit(gpio_x, gpio_pin);
}

void SET_SDA(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef *gpio_x;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	if(i2cPort < I2C_MAX_SIZE)
	{
		GPIO_InitStruct.GPIO_Pin = sdaPin[i2cPort];
		gpio_x = sdaPort[i2cPort];
	}
	else
	{
		return;
	}
	GPIO_Init(gpio_x,&GPIO_InitStruct);
	
}
 void CLEAR_SDA(void)
 { 	
	
 	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef *gpio_x;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	if(i2cPort < I2C_MAX_SIZE)
	{		
		GPIO_InitStruct.GPIO_Pin = sdaPin[i2cPort];
		gpio_x = sdaPort[i2cPort];
	}
	else
	{
		return;

	}
	GPIO_Init(gpio_x,&GPIO_InitStruct);
	GPIO_ResetBits(gpio_x,GPIO_InitStruct.GPIO_Pin);
 }
 void SET_SCL(void)
 {
 	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef *gpio_x;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	if(i2cPort < I2C_MAX_SIZE)
	{
		GPIO_InitStruct.GPIO_Pin = sclPin[i2cPort];
		gpio_x = sclPort[i2cPort];
	}
	else
	{
		return;
	}
	GPIO_Init(gpio_x,&GPIO_InitStruct);
 }
 void CLEAR_SCL(void)
 {
 
 	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef *gpio_x;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	if(i2cPort < I2C_MAX_SIZE)
	{		
		GPIO_InitStruct.GPIO_Pin = sclPin[i2cPort];
		gpio_x = sclPort[i2cPort];
	}
	else
	{
		return;

	}
	GPIO_Init(gpio_x,&GPIO_InitStruct);
	GPIO_ResetBits(gpio_x,GPIO_InitStruct.GPIO_Pin);
	
	
 }

// 
// SDA---IOA0   SCL---IOA6    CLOCK = ?kHz
//

void i2c_Init(I2CPort_TypeDef I2C_x)
{
	
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD |
						RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI, ENABLE);
	
	i2cPort = I2C_x;

	SET_SCL();
	SET_SDA();
	
}

/*
	i = 10 的工作频率为70 kHz
 */
void i2c_Delay()
{
	u16 i;
	i = 50;
    //i = 1;
	for(; i > 0; i--){	}
}


 
 //////////////////////////////////////////////////////
// I2C start signal.
// <comment>
//  SCL ________
//              \_________
//  SDA _____
//           \____________
//
// Return value: None
//////////////////////////////////////////////////////
bool i2c_Start(void)
{
    bool bStatus = TRUE; // success status

    //mcuClrIntEn(); // disable all interrupt

    SET_SDA();
    i2c_Delay();
    SET_SCL();
    i2c_Delay();

    // check pin error
    //SetI2cSCL_In();
    //SetI2cSDA_In();
    if ((GetPinI2cSCL() == Bit_RESET || (GetPinI2cSDA() == Bit_RESET)))
    {
        //SetI2cSCL_Out();
        //SetI2cSDA_Out();
        bStatus = FALSE;
    }
    else // success
    {
        
        CLEAR_SDA();
        i2c_Delay();
        CLEAR_SCL();
    }

    //mcuSetIntEn(); // release EA bit

    return bStatus;
}

/////////////////////////////////////////
// I2C stop signal.
// <comment>
//              ____________
//  SCL _______/
//                 __________
//  SDA __________/
/////////////////////////////////////////
void i2c_Stop(void)
{
    //mcuClrIntEn(); // disable all interrupt

    CLEAR_SCL();
    i2c_Delay();
    CLEAR_SDA();
    i2c_Delay();
    SET_SCL();
    i2c_Delay();
    SET_SDA();
    i2c_Delay();
	
    //mcuSetIntEn(); // release EA bit
    //Delay1ms(1);//caucy 071016
}

//////////////////////////////////////////////////////////////////////////
// I2C receive byte from device.
//
// Return value: receive byte
//////////////////////////////////////////////////////////////////////////
BYTE i2c_ReceiveByte(bool bAck)
{
    BYTE ucReceive = 0;
    BYTE ucMask = 0x80;

    //mcuClrIntEn();

    while(ucMask)
    {
        SET_SDA();
       SET_SCL();
        i2c_Delay();
        
        if(GetPinI2cSDA() == Bit_SET)
            ucReceive |= ucMask;
        
        CLEAR_SCL();
        i2c_Delay();

        ucMask >>= 1; // next
    } // while

    if (bAck) // acknowledge
        CLEAR_SDA();
    else // non-acknowledge
       SET_SDA();

    i2c_Delay();
    SET_SCL();
    i2c_Delay();
    CLEAR_SCL();
    i2c_Delay();

    //mcuSetIntEn(); // release EA bit

    return ucReceive;
}

//////////////////////////////////////////////////////////////////////////
// I2C send byte to device.
//
// Arguments: ucVal - send byte
// Return value: I2C acknowledge bit
//               I2C_ACKNOWLEDGE/I2C_NON_ACKNOWLEDGE
//////////////////////////////////////////////////////////////////////////
BitAction i2c_SendByte(BYTE ucVal)
{
    BYTE ucMask = 0x80,cout=I2C_ACCESS_DUMMY_TIME;
    BitAction bAck= Bit_SET; // acknowledge bit

    //mcuClrIntEn();

    while(ucMask)
    {
        if (ucVal & ucMask)
            SET_SDA();
        else
            CLEAR_SDA();
        i2c_Delay();
        SET_SCL(); // clock
        i2c_Delay();
       CLEAR_SCL();
        i2c_Delay();

        ucMask >>= 1; // next
    } // while

    // recieve acknowledge
    SET_SDA();
    i2c_Delay();
    SET_SCL();
    i2c_Delay();
    
    bAck = (BitAction) GetPinI2cSDA(); // recieve acknowlege
    while(cout&&bAck)
	{
    	bAck = (BitAction) GetPinI2cSDA(); // recieve acknowlege
    	cout--;
	
	}
   
    CLEAR_SCL();
    i2c_Delay();

    //mcuSetIntEn(); // release EA bit

    return (bAck);
}

//////////////////////////////////////////////////////////////////////////
// I2C access start.
//
// Arguments: ucSlaveAdr - slave address
//            trans_t - I2C_TRANS_WRITE/I2C_TRANS_READ
//////////////////////////////////////////////////////////////////////////
bool i2c_AccessStart(BYTE ucSlaveAdr, I2cIoTransType trans_t)
{
    BYTE ucDummy; // loop dummy



    if (trans_t == I2C_TRANS_READ) // check i2c read or write
        ucSlaveAdr = I2C_DEVICE_ADR_READ(ucSlaveAdr); // read
    else
        ucSlaveAdr = I2C_DEVICE_ADR_WRITE(ucSlaveAdr); // write

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while (ucDummy--)
    {
        if (i2c_Start() == FALSE)
        {
        	#if ENABLE_DEBUG_I2C
			DebugPutString("i2c_Start error\r\n");
			#endif
            continue;
        }

        if (i2c_SendByte(ucSlaveAdr) == I2C_ACKNOWLEDGE) // check acknowledge
            return TRUE;

        i2c_Stop();

        
        i2c_Delay();
    } // while

    return FALSE;
}

void i2cBurstReadBytes(I2CPort_TypeDef I2C_x,BYTE ucSlaveAdr, BYTE ucSubAdr, BYTE *pBuf, u16 ucBufLen)
{
    BYTE ucDummy; // loop dummy

	i2cPort = I2C_x;    	

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while(ucDummy--)
    {
        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_WRITE) == FALSE)
        {
        	#if ENABLE_DEBUG_I2C
	        DebugPutString("I2C start error\r\n");
			#endif
            continue;
        }
        if (i2c_SendByte(ucSubAdr) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
        {
        	#if ENABLE_DEBUG_I2C
	        DebugPutString("I2C non ack error\r\n");
			#endif
            continue;
        }
        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_READ) == FALSE)
            continue;

        while(ucBufLen) // loop to burst read
        {
            ucBufLen--;
            *pBuf = i2c_ReceiveByte((bool)ucBufLen); // receive byte

            pBuf++; // next byte pointer
        } // while

        break;
    } // while

	if(ucDummy == 255) // IT6604 no ack.
	{
		IT6604ack = 0;
	}

    i2c_Stop();

    while(ucBufLen--) // if read error, fill 0xff
    {
        *pBuf = 0xff; // clear
        pBuf++; // next byte pointer
        #if ENABLE_DEBUG_I2C
        DebugPutString("I2C read error\r\n");
		#endif
    } // while
}
void i2cBurstWriteBytes(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, BYTE ucSubAdr, BYTE *pBuf, u16 ucBufLen)
{
    BYTE ucDummy; // loop dummy

	i2cPort = I2C_x;

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while(ucDummy--)
    {
        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_WRITE) == FALSE)
		{
			i2c_Delay();
            continue;
        }
        if (i2c_SendByte(ucSubAdr) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
        {
        	i2c_Delay();
            continue;
        }

        while(ucBufLen--) // loop of writting data
        {
            i2c_SendByte(*pBuf); // send byte

            pBuf++; // next byte pointer
        } // while

        break;
    } // while

    i2c_Stop();
}

BYTE i2cReadByte(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, BYTE ucSubAdr)
{
    BYTE ucBuf; // byte buffer

    i2cBurstReadBytes(I2C_x, ucSlaveAdr, ucSubAdr, &ucBuf, 1);
    return ucBuf;
}

void i2cWriteByte(I2CPort_TypeDef I2C_x,BYTE ucSlaveAdr, BYTE ucSubAdr, BYTE ucVal)
{
    i2cBurstWriteBytes(I2C_x, ucSlaveAdr, ucSubAdr, &ucVal, 1);        // wz@ fpga hang
}

void i2cBurstWriteBytes2(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, u16 ucSubAdr, BYTE *pBuf, u16 ucBufLen)
{
	BYTE ucDummy; // loop dummy
	i2cPort = I2C_x;
	ucDummy = I2C_ACCESS_DUMMY_TIME;
	while(ucDummy--)
	{
		if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_WRITE) == FALSE)
		{
			i2c_Delay();
			continue;
		}
		if (i2c_SendByte((ucSubAdr>>8)) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
		{
			i2c_Delay();
			continue;
		}
		if (i2c_SendByte((ucSubAdr&0xFF)) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
		{
			i2c_Delay();
			continue;
		}

		while(ucBufLen--) // loop of writting data
		{
			i2c_SendByte(*pBuf); // send byte
			//SysDelay1ms(5);
			pBuf++; // next byte pointer
		} // while

		break;
	} // while

	i2c_Stop();
}

void i2cBurstReadBytes2(I2CPort_TypeDef I2C_x, BYTE ucSlaveAdr, u16 ucSubAdr, BYTE *pBuf, u16 ucBufLen)
{
    BYTE ucDummy; // loop dummy
    i2cPort = I2C_x;

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while(ucDummy--)
    {
        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_WRITE) == FALSE)
            continue;
        if (i2c_SendByte(ucSubAdr>>8) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
            continue;
		if (i2c_SendByte(ucSubAdr&0xFF) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
            continue;
        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_READ) == FALSE)
            continue;

        while(ucBufLen) // loop to burst read
        {
            ucBufLen--;
            *pBuf = i2c_ReceiveByte((bool)ucBufLen); // receive byte

            pBuf++; // next byte pointer
        } // while

        break;
    } // while

    i2c_Stop();

    while(ucBufLen--) // if read error, fill 0xff
    {
        *pBuf = 0xff; // clear
        pBuf++; // next byte pointer
    } // while
}


