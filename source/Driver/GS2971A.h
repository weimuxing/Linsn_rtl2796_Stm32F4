#ifndef _GS2971A_H_
#define _GS2971A_H_

// For GS2971A
// SDI_GS2971_RST		PE2
// SDOUT --MCU_SPI_DATA0 --PG0
// SDIN --MCU_SPI_ASD0--PF15
// SCLK --MCU_SPI_DCLK --PF14
// CS0 --MCU_SPI_nCSO --PF13

#define GS2971A_RESET_PIN_A 	GPIO_Pin_2
#define GS2971A_RESET_PORT_A 	GPIOE

#define GS2971A_CS_PIN_A 		GPIO_Pin_13
#define GS2971A_CS_PORT_A 		GPIOF
#define GS2971A_CLK_PIN_A 		GPIO_Pin_14
#define GS2971A_CLK_PORT_A 		GPIOF
#define GS2971A_MOSI_PIN_A 		GPIO_Pin_15
#define GS2971A_MOSI_PORT_A 	GPIOF
#define GS2971A_MISO_PIN_A 		GPIO_Pin_0
#define GS2971A_MISO_PORT_A 	GPIOG

//2971A¶Á¼Ä´æÆ÷
#define GS2971A_READ_FLAG		0x8000
#define GS2971A_AUTOINC_FLAG	0x1000

typedef enum 
{
	GS2971A_A,
	GS2971A_MAX,
} GS2971A_Port_TypeDef;

void GS2971A_Reset(GS2971A_Port_TypeDef GS2971A_X);

void taskInputSDI(void);
int GS2971A_ReadInputFormat(GS2971A_Port_TypeDef GS2971A_X);


#endif


