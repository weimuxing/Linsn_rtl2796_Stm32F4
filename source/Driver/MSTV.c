

#define _MSTV_C_

#include "MSTV.h"
#include "I2C.h"
#include "UART.h"
#include "SysTick.h"
#include "pipe.h"

#ifndef _MSTV_C_

#define MSTV_BUFFER_SIZE (sizeof(UartCommadType))

static u32 mstvStartTime;
static u32 mstvDelayTime = 0;

static Pipe_t pipe_of_mstv;
static BYTE mstv_buffer[MSTV_BUFFER_SIZE];
static I2CPort_TypeDef _i2cPort = I2C_A;
#define MSTV_I2C_PORT _i2cPort

static bool ReceiveByteMSTV(BYTE value);

static void disableMstvTimeout(void)
{
	mstvDelayTime = 0;
}
static void setMstvTimeout(u32 timeout)
{
	mstvStartTime = GetSysTick();
	mstvDelayTime = timeout;
}

static bool isMstvTimeout(void)
{
	if(mstvDelayTime == 0)
		return FALSE;

	return isSysTickTimeout(mstvStartTime, mstvDelayTime);
}


#if( ENABLE_UART_CHECKSUM )
bit IsUartCmdValid(void)
{
    BYTE i, ucLen;
    BYTE ucCS = 0;

	if( UART_EXT )
	{
	    ucLen = UART_CMD_EXT_LENGTH;
	}
	else
	{
	    ucLen = UART_CMD_LENGTH;
	}

    ucCS = 0;
    for( i = 0; i < ucLen; ++ i )
    {
        ucCS += g_UartCommand.Buffer[i];
    }

    if( ucCS == 0xFF )
        return (BitAction)1;
    return (BitAction)0;
}
#endif

void uartDecodeCommand(void)
{
	// receive command from pipe
	BYTE value;
	while(pipe_read(&pipe_of_mstv, &value))
	{
		if(ReceiveByteMSTV(value))
		{
			// continue do nothing
		}
		else
		{
			break;  // received data is full command
		}
	}
    
    if (g_bUartDetected) // cehck command flag
    {
    	
        g_bUartDetected = (bit)0;
		//if (g_bFlashIspMode)
		//	uartDecodeFlashIspCommand();
	#if (_DEBUG_EXTDEV_EN_)
		if (UART_EXT)
		    uartDecodeExtCommand();
	#endif
		else
		    uartDecodeNormalCommand();
    } 

    if(isMstvTimeout())
    {
    	g_UartCommand.Index = 0;
    }
}
void uartDecodeExtCommand(void)
{
	return;
}
///////////////////////////////////////
//  Decode Uart command
///////////////////////////////////////
void uartDecodeNormalCommand(void)
{
#if (_DEBUG_RW_REG_EN_)
     //WORD uwEEADDR;
#endif

    if (UART_CMD==kTest_Command && UART_CMD_MS_REG==0x51)
        ;
    else if (g_bSimulateStopFlag)
    {
        if (UART_CMD!=kWrite_MCU_SFR && UART_CMD!=kRead_MCU_SFR
        && UART_CMD!=kWrite_MCU_XDATA && UART_CMD!=kRead_MCU_XDATA
        && UART_CMD!=kWrite_MCU_sBIT && UART_CMD!=kRead_MCU_sBIT)
            return;
    }
    else
    {
    }
    switch(UART_CMD)
    {
    #if (_DEBUG_RW_REG_EN_)
        case kWrite_MST_Bank_n:
        {
            if( UART_CMD_MS_BANK == UART_DSP_BANK )
            {
                
            }
            else if( UART_CMD_MS_BANK == UART_PARA_BANK )
            {
            } 
            else if( UART_CMD_MS_BANK == UART_XDATA_BANK )
            {
                WORD wAddr;
                wAddr = UART_CMD_XDATA_ADD_H;
                wAddr = wAddr*256 + UART_CMD_XDATA_ADD_L;
                //msVdDspWriteByte(3, wAddr, UART_CMD_XDATA_DATA);
            }
            else
            {
                //scWriteByte(SC0_00, UART_CMD_MS_BANK);
                //scWriteByte(UART_CMD_MS_REGINDEX, UART_CMD_MS_REGDATA);
            }
            if (!g_bDebugASCIICommandFlag)
                putchar(0xF1);
            break;
        }
        case kRead_MST_Bank_n:
        {
            if( UART_CMD_MS_BANK == UART_DSP_BANK )
            {
                UART_CMD_MS_REGDATA = 0; //msVdDspReadByte(2, UART_CMD_MS_REGINDEX);
            }
            else if( UART_CMD_MS_BANK == UART_PARA_BANK )
            {
                UART_CMD_MS_REGDATA = 0; //msVdDspReadByte(6, UART_CMD_MS_REGINDEX);
            }
            else if( UART_CMD_MS_BANK == UART_XDATA_BANK )
            {
                //WORD wAddr;
                //wAddr = UART_CMD_XDATA_ADD_H;
                //wAddr = wAddr*256 + UART_CMD_XDATA_ADD_L;
                UART_CMD_XDATA_DATA = 0;//msVdDspReadByte(4, wAddr);
            }
            else
            {
                //scWriteByte(SC0_00, UART_CMD_MS_BANK);
                UART_CMD_MS_REGDATA = 0;//scReadByte(UART_CMD_MS_REGINDEX);
            }
            putchar(0xF2);
            if( UART_CMD_MS_BANK == UART_XDATA_BANK )
                putchar(UART_CMD_XDATA_DATA);
            else
            putchar(UART_CMD_MS_REGDATA);
            if (g_bDebugASCIICommandFlag)
            {
                printf("BK%x", UART_CMD_MS_BANK);
                printf("[0x%x]=", UART_CMD_MS_REGINDEX);
                printf("0x%x\r\n", UART_CMD_MS_REGDATA);
            }
            break;
        }
        case kWrite_MCU_SFR:
        {
            
            if (!g_bDebugASCIICommandFlag)
                putchar(0xF1);
            break;
        }
        case kRead_MCU_SFR:
        {

		UART_CMD_MS_DAT = 0;
            putchar(0xF2);
            putchar(UART_CMD_MS_DAT);
            if (g_bDebugASCIICommandFlag)
            {
                printf("0x%x \r\n", UART_CMD_MS_DAT);
            }
            break;
        }
        case kWrite_MCU_XDATA:
        {
            //XBYTE[MAKEWORD(UART_CMD_MCU_IDX_H, UART_CMD_MCU_IDX_L)] = UART_CMD_MCU_DATA;
            /*if(UART_CMD_MCU_IDX_H == SiI9134_ADDRESS 
				|| UART_CMD_MCU_IDX_H == FPGA_ADDRESS
				|| UART_CMD_MCU_IDX_H == 0x7A
				|| UART_CMD_MCU_IDX_H == 0xA0)
		*/
			if((UART_CMD_MCU_IDX_H == 0x02) && (UART_CMD_MCU_IDX_L == 0x00))
			{
				if(UART_CMD_MCU_DATA < 3)
				{
					_i2cPort = (I2CPort_TypeDef)UART_CMD_MCU_DATA;
				}
			}
			#if 0
			else if(UART_CMD_MCU_IDX_H == 0x10)
			{
				SetC732_Port(0);
				writeRegisterC732(UART_CMD_MCU_IDX_L,UART_CMD_MCU_DATA);
			}
			else if(UART_CMD_MCU_IDX_H == 0x12)
			{
				SetC732_Port(1);
				writeRegisterC732(UART_CMD_MCU_IDX_L,UART_CMD_MCU_DATA);
			}
			#endif
			else
            {
            	i2cWriteByte(MSTV_I2C_PORT, UART_CMD_MCU_IDX_H, UART_CMD_MCU_IDX_L, UART_CMD_MCU_DATA);
            }
		
            if (!g_bDebugASCIICommandFlag)
                putchar(0xF1);
            break;
        }
        case kRead_MCU_XDATA:
        {
			if((UART_CMD_MCU_IDX_H == 0x02) && (UART_CMD_MCU_IDX_L == 0x00))
			{
				UART_CMD_MCU_DATA = MSTV_I2C_PORT;				
			}
			#if 0
			else if(UART_CMD_MCU_IDX_H == 0x10)
			{
				SetC732_Port(0);
				UART_CMD_MCU_DATA = readRegisterC732(UART_CMD_MCU_IDX_L);
			}
			else if(UART_CMD_MCU_IDX_H == 0x12)
			{
				SetC732_Port(1);
				UART_CMD_MCU_DATA = readRegisterC732(UART_CMD_MCU_IDX_L);
			}
			#endif
			else
			{
				UART_CMD_MCU_DATA = i2cReadByte(MSTV_I2C_PORT, UART_CMD_MCU_IDX_H, UART_CMD_MCU_IDX_L);
			}


			
			putchar(0xF2);
			putchar(UART_CMD_MCU_DATA);
			if (g_bDebugASCIICommandFlag)
			{
			    printf("0x%x \r\n", UART_CMD_MCU_DATA);
			}
			break;
        }
        case kWrite_MCU_sBIT:
        {            
            
            if (!g_bDebugASCIICommandFlag)
                putchar(0xF1);
            break;
        }
        case kRead_MCU_sBIT:
        {          
            
            putchar(0xF2);
            putchar(UART_CMD_MS_DAT);
            if (g_bDebugASCIICommandFlag)
            {
                printf("0x%x \r\n", UART_CMD_MS_DAT);
            }
            break;
        }
    #endif
    #if (0)
        case kWrite_EEPROM:
        {
            uwEEADDR = MAKEWORD(g_UartCommand.Buffer[_UART_CMD_INDEX1_],g_UartCommand.Buffer[_UART_CMD_INDEX2_]);
            //uwEEADDR = g_UartCommand.Buffer[_UART_CMD_INDEX1_] & 0x0E;
            //uwEEADDR = (uwEEADDR << 7) + g_UartCommand.Buffer[_UART_CMD_INDEX2_];
        #if ENABLE_HDCP
            if (uwEEADDR>=RM_HDCP_KEY_BASEADDR1)
                g_wHDCP_KeyChkSum += g_UartCommand.Buffer[_UART_CMD_INDEX3_];
        #endif // ENABLE_HDCP
            rmWriteByte(uwEEADDR, g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            if (!g_bDebugASCIICommandFlag)
                putchar(0xF1);
            break;
        }
        case kRead_EEPROM:
        {
            uwEEADDR = MAKEWORD(g_UartCommand.Buffer[_UART_CMD_INDEX1_],g_UartCommand.Buffer[_UART_CMD_INDEX2_]);
            //uwEEADDR = g_UartCommand.Buffer[_UART_CMD_INDEX1_] & 0x0E;
            //uwEEADDR = (uwEEADDR << 7) + g_UartCommand.Buffer[_UART_CMD_INDEX2_];
            g_UartCommand.Buffer[_UART_CMD_INDEX3_] = rmReadByte(uwEEADDR);
            putchar(0xF2);
            putchar(g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            if (g_bDebugASCIICommandFlag)
            {
                printf("EEPROM[0x%x]=", uwEEADDR);
                printf("0x%x \r\n", g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            }
            break;
        }
        case kRead_Device_B8:
        {
            g_UartCommand.Buffer[_UART_CMD_INDEX3_] = i2cReadByte( g_UartCommand.Buffer[_UART_CMD_INDEX1_],  g_UartCommand.Buffer[_UART_CMD_INDEX2_]);
            putchar(0xF2);
            putchar(g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            if (g_bDebugASCIICommandFlag)
            {
                printf("I2C 0x%x ", g_UartCommand.Buffer[_UART_CMD_INDEX1_]);
                printf("[0x%x]=", g_UartCommand.Buffer[_UART_CMD_INDEX2_]);
                printf("0x%x \r\n", g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            }
            break;
        }
        case kRead_Device_B16:
        {
            WORD wData;

            putchar(0xF3);
            i2cBurstReadBytes(g_UartCommand.Buffer[_UART_CMD_INDEX1_], g_UartCommand.Buffer[_UART_CMD_INDEX2_], (BYTE *)&wData, sizeof(wData));
            putchar(HIBYTE(wData));
            putchar(LOBYTE(wData));
            if (g_bDebugASCIICommandFlag)
            {
                printf("I2C 0x%x ", g_UartCommand.Buffer[_UART_CMD_INDEX1_]);
                printf("[0x%x]=", g_UartCommand.Buffer[_UART_CMD_INDEX2_]);
                printf("0x%x \r\n", g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            }
            break;
        }

	 case kWrite_Device:
        {
            if(UART_CMD_LENGTH==5)
            {
                WORD wData;

                wData = ((WORD)(g_UartCommand.Buffer[_UART_CMD_INDEX3_]))<<8;
                wData = wData + (WORD)(g_UartCommand.Buffer[_UART_CMD_INDEX4_]);
                i2cBurstWriteBytes(g_UartCommand.Buffer[_UART_CMD_INDEX1_], g_UartCommand.Buffer[_UART_CMD_INDEX2_], (BYTE *)&wData, sizeof(wData));
                if (!g_bDebugASCIICommandFlag) putchar(0xF1);
            }
            else
            {
                i2cWriteByte( g_UartCommand.Buffer[_UART_CMD_INDEX1_],  g_UartCommand.Buffer[_UART_CMD_INDEX2_],  g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
                if (!g_bDebugASCIICommandFlag) putchar(0xF1);
            }
            break;
        }
        case kWrite_VPC32xx_FP:
        {
            WORD wData,wAddress;

            wAddress = MAKEWORD(g_UartCommand.Buffer[_UART_CMD_INDEX2_] , g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            wData = MAKEWORD(g_UartCommand.Buffer[_UART_CMD_INDEX4_], g_UartCommand.Buffer[_UART_CMD_INDEX5_]);
            i2cBurstWriteBytes(g_UartCommand.Buffer[_UART_CMD_INDEX1_], 0x37, (BYTE *)&wAddress, sizeof(wAddress));
            i2cBurstWriteBytes(g_UartCommand.Buffer[_UART_CMD_INDEX1_], 0x38, (BYTE *)&wData, sizeof(wData));
            if (!g_bDebugASCIICommandFlag)
                putchar(0xF1);
            break;
        }
        case kRead_VPC32xx_FP:
        {
            WORD wData,wAddress;

            wAddress = MAKEWORD(g_UartCommand.Buffer[_UART_CMD_INDEX2_] , g_UartCommand.Buffer[_UART_CMD_INDEX3_]);
            i2cBurstWriteBytes(g_UartCommand.Buffer[_UART_CMD_INDEX1_], 0x36, (BYTE *)&wAddress, sizeof(wAddress));
            i2cBurstReadBytes(g_UartCommand.Buffer[_UART_CMD_INDEX1_], 0x38, (BYTE *)&wData, sizeof(wData));
            putchar(0xF3);
            putchar(HIBYTE(wData));
            putchar(LOBYTE(wData));
            break;
        }
    #endif
        // Test Only
        case kTest_Command:
            //ExecTestCommand();
            break;
    } // switch
    if (UART_CMD==kTest_Command && UART_CMD_MS_REG==0x51)
        ;
    else if (g_bSimulateStopFlag)
    {

    }
}

/*
*  Return: TRUE--Continue, FALSE--not Continue(Receive byte is full)
*/
static bool ReceiveByteMSTV(BYTE value)
{
        BYTE ucTmp = value;

        //g_ucUartCheckTick = 2;
        setMstvTimeout(100); // 100 ms
        if( g_UartCommand.Index < _UART_CMD_BUFFER_LENGTH_ ) // Protect buffer overflow
        {
            g_UartCommand.Buffer[g_UartCommand.Index] = ucTmp; // recieve byte
        }

        if (UART_EXT)
        {
            if (g_UartCommand.Index == 0) // check 1st data
                g_UartCommand.Index++; // for get UART_CMD_EXT_LENGTH
            else
            {
                if (g_UartCommand.Index < UART_CMD_EXT_LENGTH) // still read command
                    g_UartCommand.Index++; // next index of command buffer
                if (g_UartCommand.Index >= UART_CMD_EXT_LENGTH) // read command ok
                {
                #if( ENABLE_UART_CHECKSUM )
                    if( IsUartCmdValid() )
                #endif
                        g_bUartDetected = (bit)TRUE; // command  buffer recieve ok
                    g_UartCommand.Index = 0; // reset index of command buffer
                    //g_ucUartCheckTick = 0;
                    disableMstvTimeout();
			return FALSE;
                }
            }
        }
        else
        {
            if (g_UartCommand.Index < UART_CMD_LENGTH) // still read command
                g_UartCommand.Index++; // next index of command buffer
            if (g_UartCommand.Index >= UART_CMD_LENGTH) // read command ok
            {
            #if( ENABLE_UART_CHECKSUM )
                if( IsUartCmdValid() )
            #endif
                    g_bUartDetected = (bit)TRUE; // command  buffer recieve ok
                g_UartCommand.Index = 0; // reset index of command buffer
                //g_ucUartCheckTick = 0;
                disableMstvTimeout();
		  return FALSE;
            }
        }
	return TRUE;
}

void InitializeMSTV(void)
{
   
    // reset Uart variables
    g_UartCommand.Index = 0;
    g_bDebugASCIICommandFlag = (bit)0;
//    g_bDebugASCIICommandFlag = _ENABLE;
    g_bDebugProgStopFlag = (bit)FALSE;
    g_bSimulateStopFlag = (bit)FALSE;

    mstvDelayTime = 0;
	pipe_init(&pipe_of_mstv, mstv_buffer, MSTV_BUFFER_SIZE);
	
}

void IsrMSTV(BYTE value)
{
	pipe_write(&pipe_of_mstv, value);
}
#endif

#undef _MSTV_C_

