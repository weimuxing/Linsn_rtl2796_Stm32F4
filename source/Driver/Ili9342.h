#ifndef ILI9342_H
#define ILI9342_H
#include "define.h"

//#define USE_CS_STOP_FLAG

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ILI9342_CMD_NOP					0x00
#define ILI9342_CMD_SOFT_RST				0x01
#define ILI9342_CMD_RD_ID					0x04
#define ILI9342_CMD_RD_STATUS				0x09
#define ILI9342_CMD_RD_POWER_MODE		0x0A
#define ILI9342_CMD_RD_MADCTL				0x0B
#define ILI9342_CMD_RD_PIXEL_FORMAT		0x0C
#define ILI9342_CMD_RD_IMAGE_FORMAT		0x0D
#define ILI9342_CMD_RD_SIGNAL_MODE		0x0E
#define ILI9342_CMD_ENTER_SLEEP			0x10
#define ILI9342_CMD_EXIT_SLEEP				0x11
#define ILI9342_CMD_PARTIAL_MODE_ON		0x12
#define ILI9342_CMD_NORMAL_DISPLAY		0x13
#define ILI9342_CMD_INVERSION_OFF			0x20
#define ILI9342_CMD_INVERSION_ON			0x21
#define ILI9342_CMD_GAMMA_SET				0x26
#define ILI9342_CMD_DISPLAY_OFF			0x28
#define ILI9342_CMD_DISPLAY_ON				0x29
#define ILI9342_CMD_COLUMN_ADDR_SET		0x2A
#define ILI9342_CMD_PAGE_ADDR_SET			0x2B
#define ILI9342_CMD_MEMORY_WR				0x2C
#define ILI9342_CMD_COLOR_SET				0x2D
#define ILI9342_CMD_MEMORY_RD				0x2E
#define ILI9342_CMD_PARTIAL_AREA			0x30

#define ILI9342_CMD_IDLE_MODE_OFF			0x38
#define ILI9342_CMD_IDLE_MODE_ON			0x39

#define ILI9342_CMD_RD_ID1					0xDA
#define ILI9342_CMD_RD_ID2					0xDB
#define ILI9342_CMD_RD_ID3					0xDC
#define ILI9342_CMD_RD_ID4					0xD3

#define ILI9342_CMD_RGB_SIGNAL_CTL		0xB0
#define ILI9342_CMD_INVERSION_CTL			0xB4



/* Exported functions ------------------------------------------------------- */
void ili9342c_Initial(void);
void Ili9342_Check_id(void);

#endif /* ILI9342_H */

