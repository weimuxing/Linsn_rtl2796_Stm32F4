#ifndef _SYSCONFIG_H
#define _SYSCONFIG_H
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "uart.h"
#include "define.h"

#include "stm32f4xx_gpio.h"


#define DEBUG_PRINT_ENABLE
//#define COLOK_CHIP_TEST

#define _VIP_SMART			0x0520
#define _VENUS_C1   		0x1c00
#define _VENUS_X1   		0x1f00

#define CURRENT_DEVICE	_VIP_SMART

#define USE_ILI9342C
#define USE_FPGA_MODULE

#define GPIO_Pin_INVALIDE		0x0000


// I2C Port Define,must same to I2CPort_TypeDef
#define I2C_PORT_A				0x00
#define I2C_PORT_B				0x01
#define I2C_PORT_C				0x02
#define I2C_PORT_D				0x03
#define I2C_PORT_E				0x04
#define I2C_PORT_F				0x05
#define I2C_PORT_G				0x06
#define I2C_PORT_H				0x07
#define I2C_PORT_I				0x08
#define I2C_PORT_MAX			0x09

// SPI Port Define,must same to SpiPortTypeDef
#define SPI_A					0x00
#define SPI_B					0x01
#define SPI_C					0x02
#define SPI_D					0x03
#define SPI_MAX					0x04

#if (CURRENT_DEVICE == _VIP_SMART)
#include "SysConfig_vipSmart.h"
#else
error!
#endif


#endif

