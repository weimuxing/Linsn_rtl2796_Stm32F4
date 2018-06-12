#ifndef _AD4604_H_
#define _AD4604_H_

#include "define.h"

// 使用差分线的线数都是4个通道
#define AD4604_SIGNAL_WIRE		4

#define ADN4604_SALAVE_ADDR0				0x90
#define ADN4604_SALAVE_ADDR1				0x92
#define ADN4604_SALAVE_ADDR2				0x94
#define ADN4604_SALAVE_ADDR3				0x96
#define ADN4604_SALAVE_ADDR_INVALID		0xFF

#define ADN4604_REG_RESET					0x00
#define ADN4604_REG_RX_EQ_CTL0			0x10
#define ADN4604_REG_RX_EQ_CTL1			0x11
#define ADN4604_REG_RX_CTL0				0x12
#define ADN4604_REG_RX_CTL1				0x13

#define ADN4604_REG_TX_CTL_BASE			0x20
#define ADN4604_REG_TX_LOOKUP_BASE		0x60
#define ADN4604_REG_TX_ADVCTL_BASE		0x30
#define ADN4604_REG_UPDATA				0x80
#define ADN4604_REG_MAP_SEL				0x81
#define ADN4604_REG_BROADCAST			0x82
#define ADN4604_REG_MAP0_BASE			0x90
#define ADN4604_REG_MAP1_BASE			0x98
#define ADN4604_REG_STATUS_BASE			0xB0
#define ADN4604_REG_TERMINATION			0xF0
#define ADN4604_REG_REV_ID					0xFE
#define ADN4604_REG_DEV_ID				0xFF

#define ADN4604_MASK_ODD_IDX				0xF0
#define ADN4604_MASK_EVEN_IDX				0x0F

// regsiter bit define 
// TX Basic Control Register
#define ADN4604_TX_EN_MASK				0x30
#define ADN4604_TX_EN_DISABLED			0x00
#define ADN4604_TX_EN_STANDBY				0x10
#define ADN4604_TX_EN_SQUELCH				0x20
#define ADN4604_TX_EN_ENABLED				0x30

typedef enum
{
	ADN4604_IDX_1,	
	ADN4604_IDX_2,	
	ADN4604_IDX_MAX
} adn4604_index;

typedef enum
{
	ADN4604_I2C,	
	ADN4604_SPI
} adn4604_interface;

typedef enum
{
	AD4604_HDMI_IN = 0,
	AD4604_DVI_IN,
	AD4604_SDI_IN,
	AD4604_IN_SIGNAL_MAX
}ad6404_input_signal;


typedef enum
{
	AD4604_IT6604_OUT = 0,
	AD4604_PREVIEW_OUT,			
	AD4604_DVI_OUT,
	AD4604_HDMI_OUT,
	AD4604_OUT_SIGNAL_MAX
}ad6404_ouput_signal;

typedef enum
{
	AD4604_IN_PIN0 = 0,
	AD4604_IN_PIN1,
	AD4604_IN_PIN2,
	AD4604_IN_PIN3,
	AD4604_IN_PIN4,
	AD4604_IN_PIN5,
	AD4604_IN_PIN6,	
	AD4604_IN_PIN7,
	AD4604_IN_PIN8,
	AD4604_IN_PIN9,
	AD4604_IN_PIN10,
	AD4604_IN_PIN11,
	AD4604_IN_PIN12,
	AD4604_IN_PIN13,
	AD4604_IN_PIN14,
	AD4604_IN_PIN15,		
	AD4604_IN_PIN_MAX
}ad6404_input_pin;

typedef enum
{
	AD4604_OUT_PIN0 = 0,
	AD4604_OUT_PIN1,
	AD4604_OUT_PIN2,
	AD4604_OUT_PIN3,
	AD4604_OUT_PIN4,
	AD4604_OUT_PIN5,
	AD4604_OUT_PIN6,	
	AD4604_OUT_PIN7,
	AD4604_OUT_PIN8,
	AD4604_OUT_PIN9,
	AD4604_OUT_PIN10,
	AD4604_OUT_PIN11,
	AD4604_OUT_PIN12,
	AD4604_OUT_PIN13,
	AD4604_OUT_PIN14,
	AD4604_OUT_PIN15,		
	AD4604_OUT_PIN_MAX
}ad6404_ouput_pin;

typedef struct
{
	unsigned char signal;
	unsigned char pin[AD4604_SIGNAL_WIRE];	
}ad4604_hardware_wire;

typedef struct
{
    int     InputCh;
	FunctionalState   EnSta; 
}AD4604OUT;

typedef struct
{
	adn4604_index idx;
	adn4604_interface		interface;
	unsigned char port;
	GPIO_TypeDef * reset_gpiox;
	uint32_t reset_Pin;
	unsigned char slave_addr;
}adn4604_dev_info;

void AD4604_Reset(void);
void AD4604_DisableOutput(u8 output);
void AD4604_EnableOutput(u8 output);
int AD4604_GetEnable(u8 output);
void AD4604_DisableAll(void);
void AD4604_EnableAll(void);

void AD4604_SwitchInputToAll(u8 value);
void AD4604_SwitchInputToOutput(u8 input,u8 output);

u8 AD4604_GetInterInputPort(u8 input);
u8 AD4604_GetInput(u8 output);
void AD4604_SetOutputDriver(int value);
int AD4604_GetOutputDriver(void);

void AD4604EnableInputToOutput(u8 input_ch,u8 output_ch);
void AD4604SwitchInputToOutput(ad6404_input_signal input,ad6404_ouput_signal * outputList);
void AD4604SwitchInputToAll(ad6404_input_signal input);
u8 AD4604_GetInputPort(ad6404_ouput_signal output);

void AD4604_writeTest(void);
//void setSwitchToOutputAndOutputEnable(u8 output,u8 input);// output 0-1   input  0-3 
//void AD4604_writeTest(void);
#endif
