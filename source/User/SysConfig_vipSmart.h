#ifndef _SYSCFG_VIPSMART_H
#define _SYSCFG_VIPSMART_H

#define DISPLSY_CHAUVET_LOGO	0

//#define SUPPORT_DOUBLE_TRIP_KEY
#define SUPOORT_SPLIT_FUNCTION

#define ENABLE_TEST_GRID_CONTROL  1 // FPGA的网格测试控制
#define SOURCE_TAKE_MODE			// 如果不定义，是直接切换通道，
#define ENABLE_SEND_CARD_LED_CONTROL	1
#define ENABLE_VGA_ADJUST_MENU_ITEM		0

//#define SUPPORT_SD_MODULE

#define SUPPORT_LANG_ENGLISH
//#define SUPPORT_LANG_CHINESE


#define SUPPORT_ADVANCED_SN_OPTION			1		// 打开这个选项后会显示SN,Firmware,Tech Support选择

#define DEFAULT_LOCKPANEL_CRACK				0x1212


// Define the Use Chip List
#define USE_ILI9342C			// TFT2.0 Driver IC
#define USE_SSD1322				// OLED Driver IC
//#define USE_HD44780
#define USE_SIL9134
//#define USE_SIL9135
//#define USE_SIL7172
#define USE_STDP9320
//#define USE_STDP8028
#define USE_ADN4604
#define USE_GS2971A
//#define USE_GS2972
#define USE_IT6604
//#define USE_IT6505
#define USE_IT6661
#define USE_PCF8563				// Clock Chip
#define USE_FPGA_MODULE
#define USE_FPGA
//#define USE_THC63LVD1024
#define USE_D74HC165			// 串转并 For Key 
#define USE_D74HC595			// 串转并 For Key LED
//#define USE_NH16032C
//#define USE_PCA8574D
//#define USE_TVP7002
///-----------------------------------------------------------------------//

#define  MCU_STATUS_LED_AHB1PERIPH			( RCC_AHB1Periph_GPIOI)
#define MCU_STATUS_LED_PORT		GPIOI
#define MCU_STATUS_LED_PIN			GPIO_Pin_10

#define REMOTE_DETECT_AHB1ERIPH_V1_0     (RCC_AHB1Periph_GPIOF)
#define REMOTE_DETECT_PORT_V1_0         GPIOF
#define REMOTE_DETECT_PIN_V1_0         GPIO_Pin_4

#define REMOTE_DETECT_AHB1ERIPH_V1_1     (RCC_AHB1Periph_GPIOA)
#define REMOTE_DETECT_PORT_V1_1         GPIOA
#define REMOTE_DETECT_PIN_V1_1         GPIO_Pin_3


#ifdef USE_ILI9342C
// 硬件连接说明
// CN6 中
// LCD_CS --接到了FPGA E10脚 -MCU PG8
// LCD_RESET 
// LCD_DEN
// LCD2.0_SCL->I2C_SCL3 --MCU(I2C_SCL3_207 PA8 )
// LCD2.0_SDA->I2C_SDA3 --MCU(I2C_SDA3_207 PC9 )
// old
#define ILI9342_SPI_AHB1PERIPH	(RCC_AHB1Periph_GPIOA  | RCC_AHB1Periph_GPIOC)
#define ILI9342_SCLK_PIN 		GPIO_Pin_8
#define ILI9342_SCLK_PORT 		GPIOA
#define ILI9342_CS_PIN 			GPIO_Pin_2
#define ILI9342_CS_PORT 		GPIOC
#define ILI9342_SDA_PIN 		GPIO_Pin_9
#define ILI9342_SDA_PORT 		GPIOC

#define ILI9342_RST_AHB1PERIPH	(RCC_AHB1Periph_GPIOE)
#define ILI9342_RST_PIN 		GPIO_Pin_6
#define ILI9342_RST_PORT 		GPIOE

// new
//#define ILI9342_SPI_AHB1PERIPH	(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOC)
//#define ILI9342_SCLK_PIN 		GPIO_Pin_8
//#define ILI9342_SCLK_PORT 		GPIOG
//#define ILI9342_CS_PIN 			GPIO_Pin_8
//#define ILI9342_CS_PORT 		GPIOA
//#define ILI9342_SDA_PIN 		GPIO_Pin_9
//#define ILI9342_SDA_PORT 		GPIOG
//#define ILI9342_RST_PIN 			GPIO_Pin_9
//#define ILI9342_RST_PORT 		GPIOC
#endif

#ifdef USE_SSD1322
//PG5	LCD_CLK	LCD/RD
//PG4	LCD_CS	LCD/CS
//PG3	SDA	LCD/WR
//PG2	RST	LCD/RESET
#define SSD1322_SPI_AHB1PERIPH		(RCC_AHB1Periph_GPIOG)
#define SSD1322_SCLK_PIN 		GPIO_Pin_5
#define SSD1322_SCLK_PORT 		GPIOG
// CS No Use
#define SSD1322_CS_PIN 			GPIO_Pin_4
#define SSD1322_CS_PORT 		GPIOG
#define SSD1322_SID_PIN 		GPIO_Pin_3
#define SSD1322_SID_PORT 		GPIOG
#define SSD1322_RST_PIN 		GPIO_Pin_2
#define SSD1322_RST_PORT 		GPIOG
#define SSD1322_DC_PIN 			GPIO_Pin_4
#define SSD1322_DC_PORT 		GPIOG

#define SSD1322_FONT_RD_SPI		SPI_A
#endif

#ifdef USE_SIL9134
#define SiI9134_1_I2C			I2C_A
#define SiI9134_2_I2C			I2C_A
#define SiI9134_3_I2C			I2C_C

#define ILI9342_1_RST_AHB1PERIPH	(RCC_AHB1Periph_GPIOF)
#define ILI9342_1_RST_PIN 			GPIO_Pin_2
#define ILI9342_1_RST_PORT 		GPIOF

#define ILI9342_2_RST_AHB1PERIPH	(RCC_AHB1Periph_GPIOF)
#define ILI9342_2_RST_PIN 			GPIO_Pin_3
#define ILI9342_2_RST_PORT 		GPIOF

#define ILI9342_3_RST_AHB1PERIPH	(RCC_AHB1Periph_GPIOF)
#define ILI9342_3_RST_PIN 			GPIO_Pin_5
#define ILI9342_3_RST_PORT 		GPIOF

#endif

#ifdef USE_STDP9320

#endif

#ifdef USE_ADN4604
#define AD4604_I2C_PORT 		I2C_PORT_D

#define AD4604_RESET_AHB1PERIPH		(RCC_AHB1Periph_GPIOE)
#define AD4604_RESET_PORT		GPIOE
#define AD4604_RESET_PIN		GPIO_Pin_3
#endif
#ifdef  USE_GS2971A
// For GS2971A
// SDI_GS2971_RST		PE2
// SDOUT --MCU_SPI_DATA0 --PG0
// SDIN --MCU_SPI_ASD0--PF15
// SCLK --MCU_SPI_DCLK --PF14
// CS0 --MCU_SPI_nCSO --PF13

#define GS2971A_RESET_AHB1PERIPH
#define GS2971A_RESET_PIN_A 	GPIO_Pin_2
#define GS2971A_RESET_PORT_A 	GPIOE

#define GS2971A_SPI_PORT		SPI_PORT_B
#define GS2971A_CS_PIN_A 		GPIO_Pin_13
#define GS2971A_CS_PORT_A 		GPIOF
#define GS2971A_CLK_PIN_A 		GPIO_Pin_14
#define GS2971A_CLK_PORT_A 		GPIOF
#define GS2971A_MOSI_PIN_A 		GPIO_Pin_15
#define GS2971A_MOSI_PORT_A 	GPIOF
#define GS2971A_MISO_PIN_A 		GPIO_Pin_0
#define GS2971A_MISO_PORT_A 	GPIOG
#endif
#ifdef USE_IT6604
#endif
#ifdef USE_IT6661
#endif
#ifdef USE_PCF8563				// Clock Chip
#endif
#ifdef USE_FPGA
#endif
#ifdef USE_D74HC165			// 串转并 For Key 
#endif
#ifdef USE_D74HC595			// 串转并 For Key LED
#endif

// I2C Port Define

// SPI Port Define



#endif			// _SYSCFG_VIPSMART_H

