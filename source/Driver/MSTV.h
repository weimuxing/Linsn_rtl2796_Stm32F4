
#ifndef _MSTV_H_
#define _MSTV_H_

#include "define.h"

#ifdef _MSTV_C_
    #define _UARTDEC_
#else
    #define _UARTDEC_ extern
#endif

/////////////////////////////////////////////
// Uart Protocol Command Definition
/////////////////////////////////////////////
#define kWrite_MST_Bank_n    0x01
#define kRead_MST_Bank_n     0x02
#define kWrite_MST_SBM       0x1A
#define kRead_MST_SBM        0x1B
#define kWrite_EEPROM        0x12
#define kRead_EEPROM         0x13
#define kWrite_Device        0x10
#define kRead_Device_B8      0x11
#define kRead_Device_B16     0x0F
#define kWrite_VPC32xx_FP    0x0B
#define kRead_VPC32xx_FP     0x0C
#define kWrite_MCU_SFR       0x07
#define kRead_MCU_SFR        0x08
#define kRead_MCU_XDATA      0x04
#define kWrite_MCU_XDATA     0x05
#define kWrite_MCU_sBIT      0x0D // Bruce for simulater 20060914
#define kRead_MCU_sBIT       0x0E // Bruce for simulater 20060914

#define kEnter_ISP           0x00
#define kEnterFlashIsp       0x03

// Test
#define kTest_Command        0x09

#define DEV_MICRONAS_AUDIO   0x01
#define DEV_PHLIPS_TUNER     0x02
#define DEV_EEPROM_HDCP      0x03
#define DEV_EEPROM_NONLINEAR 0x04
#define DEV_MST_DLC_TEST     0x05

#define URCMD_MICRONAS_AUDIO_WRITE      0
#define URCMD_MICRONAS_AUDIO_READ       1

#define URCMD_PHLIPS_TUNER_SET_FREQ     0
#define URCMD_PHLIPS_TUNER_GET_STATUS   1

#define URCMD_EEPROM_HDCP_GET_ADDR      0
#define URCMD_EEPROM_HDCP_WRITE         1
#define URCMD_EEPROM_HDCP_WRITE_END     2
#define URCMD_EEPROM_HDCP_GET_CHECKSUM  3

#define URCMD_MST_DLC_TEST_FORCE_PAUSE  0
#define URCMD_MST_DLC_TEST_FORCE_RUN    1
#define URCMD_MST_DLC_TEST_READ         2
#define URCMD_MST_DLC_TEST_WRITE_1      3
#define URCMD_MST_DLC_TEST_WRITE_2      4
#define URCMD_MST_DLC_TEST_REINIT       5

#define cmdExitFlashIspMode             0x00
#define cmdEraseFlash                   0x01
#define cmdSetFlashRwAddress            0x02
#define cmdCheckSumCompare              0x03
#define cmdFlashCurrentRead             0x04
#define cmdSetFlashType                 0x05
#define cmdFlashCurrentWrite            0x06
#define cmdFlashCurrentWriteOne         0x07

/////////////////////////////////////////////
// Uart Constant
/////////////////////////////////////////////
// [Timer1 Baud Rate] = (1 / 32) * {Fosc / [12 * (256 - [TH1])]}
// [TH1] = 256 - (Fosc / ([Timer1 Baud Rate] * 32 * 12)

// ADCON BIT7=1
// s1rel = 1024-(fclk/baudrate)/32

#define _SMOD     1

#define UART_CMD_LENGTH (g_UartCommand.Buffer[_UART_CMD_CODE_] >> 5)
#define UART_CMD        (g_UartCommand.Buffer[_UART_CMD_CODE_] & 0x1F)
#define UART_CMD_MS_REG (g_UartCommand.Buffer[_UART_CMD_INDEX1_])
#define UART_CMD_MS_DAT (g_UartCommand.Buffer[_UART_CMD_INDEX2_])

// for MST Banks
#define UART_XDATA_BANK      0x73 // VD Dsp xdata
#define UART_MCU2_BANK       0x74
#define UART_DSP_BANK        0x75 // VD Dsp
#define UART_PARA_BANK       0x76 // VD Dsp parameter
#define UART_OSD_BANK        0x77
#define UART_CMD_MS_BANK     (g_UartCommand.Buffer[_UART_CMD_INDEX1_])
#define UART_CMD_MS_REGINDEX (g_UartCommand.Buffer[_UART_CMD_INDEX2_])
#define UART_CMD_MS_REGDATA  (g_UartCommand.Buffer[_UART_CMD_INDEX3_])

#define UART_CMD_XDATA_ADD_H (g_UartCommand.Buffer[_UART_CMD_INDEX2_])
#define UART_CMD_XDATA_ADD_L (g_UartCommand.Buffer[_UART_CMD_INDEX3_])
#define UART_CMD_XDATA_DATA  (g_UartCommand.Buffer[_UART_CMD_INDEX4_])

// for MST Banks
#define UART_CMD_MCU_IDX_H   (g_UartCommand.Buffer[_UART_CMD_INDEX1_])
#define UART_CMD_MCU_IDX_L   (g_UartCommand.Buffer[_UART_CMD_INDEX2_])
#define UART_CMD_MCU_DATA    (g_UartCommand.Buffer[_UART_CMD_INDEX3_])

// for EXT Command
#define UART_EXT             (UART_CMD_LENGTH == 7)
#define UART_CMD_EXT_LENGTH  (g_UartCommand.Buffer[_UART_CMD_INDEX1_])
#define UART_EXT_DEV         (g_UartCommand.Buffer[_UART_CMD_INDEX2_])
#define UART_EXT_CMD         (g_UartCommand.Buffer[_UART_CMD_INDEX3_])
#define UART_EXT_CMD_MS_DAT1 (g_UartCommand.Buffer[_UART_CMD_INDEX4_])
#define UART_EXT_CMD_MS_DAT2 (g_UartCommand.Buffer[_UART_CMD_INDEX5_])
#define UART_EXT_CMD_MS_DAT3 (g_UartCommand.Buffer[_UART_CMD_INDEX6_])
#define UART_EXT_CMD_MS_DAT4 (g_UartCommand.Buffer[_UART_CMD_INDEX7_])
#define UART_EXT_CMD_MS_DAT5 (g_UartCommand.Buffer[_UART_CMD_INDEX8_])
#define UART_EXT_CMD_MS_DAT6 (g_UartCommand.Buffer[_UART_CMD_INDEX9_])
#define UART_EXT_CMD_MS_DAT7 (g_UartCommand.Buffer[_UART_CMD_INDEX10_])
#define UART_EXT_CMD_MS_DAT8 (g_UartCommand.Buffer[_UART_CMD_INDEX11_])
#define UART_EXT_CMD_MS_DAT9 (g_UartCommand.Buffer[_UART_CMD_INDEX12_])
#define UART_EXT_CMD_MS_DAT10 (g_UartCommand.Buffer[_UART_CMD_INDEX13_])
#define UART_EXT_CMD_MS_DAT11 (g_UartCommand.Buffer[_UART_CMD_INDEX14_])
#define UART_EXT_CMD_MS_DAT12 (g_UartCommand.Buffer[_UART_CMD_INDEX15_])
#define UART_EXT_CMD_MS_DAT13 (g_UartCommand.Buffer[_UART_CMD_INDEX16_])

#define UART_CMD_FLASH_LENGTH    (g_UartCommand.Buffer[_UART_CMD_CODE_] >> 3)
#define UART_CMD_FLASH_ADDR0    (g_UartCommand.Buffer[_UART_CMD_INDEX1_])
#define UART_CMD_FLASH_ADDR1    (g_UartCommand.Buffer[_UART_CMD_INDEX2_])
#define UART_CMD_FLASH_ADDR2    (g_UartCommand.Buffer[_UART_CMD_INDEX3_])
#define UART_CMD_FLASH_DATA     (g_UartCommand.Buffer[_UART_CMD_INDEX1_])
#define UART_FLASH_CMD          (g_UartCommand.Buffer[_UART_CMD_CODE_] & 0x07)

#define _EOS_ '\0' // end of string

/////////////////////////////////////////////
// Uart command structure
/////////////////////////////////////////////
typedef enum _UartCommadEnumType
{
    _UART_CMD_CODE_,
    _UART_CMD_INDEX1_,
    _UART_CMD_INDEX2_,
    _UART_CMD_INDEX3_,
    _UART_CMD_INDEX4_,
    _UART_CMD_INDEX5_,
    _UART_CMD_INDEX6_,
    _UART_CMD_INDEX7_,
    _UART_CMD_INDEX8_,
    _UART_CMD_INDEX9_,
    _UART_CMD_INDEX10_,
    _UART_CMD_INDEX11_,
    _UART_CMD_INDEX12_,
    _UART_CMD_INDEX13_,
    _UART_CMD_INDEX14_,
    _UART_CMD_INDEX15_,
    _UART_CMD_INDEX16_,

    _UART_CMD_BUFFER_LENGTH_
}UartCommadEnumType;

typedef struct _UartCommadType
{
    BYTE Buffer[_UART_CMD_BUFFER_LENGTH_];  // command buffer
                                     // [Length+Command] - [Byte 2] - [Byte 3] - [Byte 4] - [Byte 5]
    BYTE Index;                     // buffer index

}UartCommadType;

typedef enum
{
    MCU_P0,
    MCU_P1,
    MCU_P2,
    MCU_P3,
    MCU_P4,
    MCU_TCON,
    MCU_TMOD,
    MCU_PCON,
    MCU_IE,
    MCU_IP,
    MCU_SCON,
    MCU_SP,
    MCU_DPL,
    MCU_DPH,
    MCU_DPL1,
    MCU_DPH1,
    MCU_DPS,
    MCU_TL0,
    MCU_TL1,
    MCU_TH0,
    MCU_TH1,
    MCU_CKCON,
    MCU_SBUF,
    MCU_SCON1,
    MCU_SBUF1,
    MCU_PMR,
    MCU_STATUS,
    MCU_T2CON,
    MCU_T2MOD,
    MCU_RCAP2L,
    MCU_RCAP2H,
    MCU_TL2,
    MCU_TH2,
    MCU_PSW,
    MCU_WDTCON,
    MCU_P4ENON,
    MCU_ACC,
    MCU_PDCON,
    MCU_REP2,
    MCU_B
} MCU_Reg;
/////////////////////////////////////////////
// Uart Variables
/////////////////////////////////////////////


_UARTDEC_ UCHAR TI0_FLAG;
_UARTDEC_ UCHAR TI1_FLAG;


_UARTDEC_ bit g_bUartDetected; // decode command flag
#if(ENABLE_UART1)
_UARTDEC_ bit g_bUart1Detected; // decode command flag
#endif
_UARTDEC_ bit g_bUartOutput;  // output hexidecimal data flag
_UARTDEC_ bit g_bDebugASCIICommandFlag;
_UARTDEC_ bit g_bDebugProgStopFlag;
_UARTDEC_ bit g_bSimulateStopFlag;

_UARTDEC_  UartCommadType g_UartCommand; // Uart command struct

#define ENABLE_UART_CHECKSUM    1
#if( ENABLE_UART_CHECKSUM )
bit IsUartCmdValid(void);
#endif

////////////////////////////////////////////////////////////////
// Debug option.If disable all, please modify project file.
////////////////////////////////////////////////////////////////
#define _DEBUG_PRINT_EN_          1  //For printf,putchar and putstr
#define _DEBUG_EXTDEV_EN_         1  //For extend device
#define _DEBUG_TESTCOMMAND_EN_    1  //For virtual IR and keypad
#define _DEBUG_RW_REG_EN_         1  //For Mstar,EEPROM and MCU
#define _DEBUG_DUMP_DRAM_EN_      0  //For Dump dram, use DramCatcher.exe

#define SiI9134_ADDRESS (0x72)
#define FPGA_ADDRESS (0x80)

void IsrMSTV(BYTE value);
void InitializeMSTV(void);
void uartDecodeCommand(void);
void uartDecodeNormalCommand(void);
void uartDecodeExtCommand(void);

#endif

