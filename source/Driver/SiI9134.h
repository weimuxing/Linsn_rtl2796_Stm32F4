#ifndef SII9134_H
#define SII9134_H
#include "define.h"
#include "userPref.h"






#define _9134_HC595_SERIN_PIN 	GPIO_Pin_8
#define _9134_HC595_SERIN_PORT	GPIOA
#define _9134_HC595_SH_CLK_PIN	GPIO_Pin_9
#define _9134_HC595_SH_CLK_PORT	GPIOC
#define _9134_HC595_ST_CLK_PIN	GPIO_Pin_8
#define _9134_HC595_ST_CLK_PORT	GPIOC

#define SET_9134_HC595_SH_CLK() 	(GPIO_SetBits(_9134_HC595_SH_CLK_PORT,_9134_HC595_SH_CLK_PIN))
#define RESET_9134_HC595_SH_CLK() 	(GPIO_ResetBits(_9134_HC595_SH_CLK_PORT,_9134_HC595_SH_CLK_PIN))
#define SET_9134_HC595_ST_CLK() 	(GPIO_SetBits(_9134_HC595_ST_CLK_PORT,_9134_HC595_ST_CLK_PIN))
#define RESET_9134_HC595_ST_CLK() 	(GPIO_ResetBits(_9134_HC595_ST_CLK_PORT,_9134_HC595_ST_CLK_PIN))
#define SET_9134_HC595_SERIN() 		(GPIO_SetBits(_9134_HC595_SERIN_PORT,_9134_HC595_SERIN_PIN))
#define RESET_9134_HC595_SERIN() 	(GPIO_ResetBits(_9134_HC595_SERIN_PORT,_9134_HC595_SERIN_PIN))


/* Exported types ------------------------------------------------------------*/
typedef enum
{
	SiI9134_PORT_1, 
	SiI9134_PORT_2, 
	SiI9134_PORT_3,
	SiI9134_PORT_4,
	SiI9134_PORT_5,
	SiI9134_PORT_6, 
	SiI9134_PORT_7, 
	SiI9134_PORT_8,
	SiI9134_PORT_MAX_SIZE,
} SiI9134PortTypeDef;

// Ouput Timing descriptors supported for this Application
typedef enum
{
	OutputMode_480p = 0x00,
	OutputMode_576p,
	OutputMode_720p,
	OutputMode_1080i,
	OutputMode_576i100,
	OutputMode_768i75,
	OutputMode_xga = 0x06,
	OutputMode_1080i50,
	OutputMode_576i75,
	OutputMode_1080i50a,
	OutputMode_720p50a,
	OutputMode_800X600 = 11,
	OutputMode_1080p60,
	OutputMode_1080p50,
	OutputMode_1280X768,
	OutputMode_1280X1024 = 15,
	OutputMode_576i50,
	OutputMode_480i60,
} OutputMode_t;

	
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define SiI9134_VIDEO_ADDRESS 0x72
#define SiI9134_AUDIO_ADDRESS 0x7A

/* Exported functions ------------------------------------------------------- */
void SiI9134_Reset(void);
void SiI9134_PowerDown(SiI9134PortTypeDef portx);
void SiI9134_Init(SiI9134PortTypeDef portx);
void SiI9134_SetVideoFormat(SiI9134PortTypeDef portx, int format);
void SiI9134_SetAudioFormat(SiI9134PortTypeDef portx, int format);
void SiI9134_SetOutputIsHDMI(SiI9134PortTypeDef portx, bool isHdmi);
bool SiI9134_GetOutputIsHDMI(SiI9134PortTypeDef portx);
BYTE SiI9134ReadVideoReg(SiI9134PortTypeDef portx, BYTE ucSubAdr);
void SiI9134WriteVideoReg(SiI9134PortTypeDef portx, BYTE ucSubAdr, BYTE ucVal);
void SiI9134WriteAudioReg(SiI9134PortTypeDef portx, BYTE ucSubAdr, BYTE ucVal);


// DE adjust
void SiI9134_SetEnableDE(SiI9134PortTypeDef portx, bool enable);
bool SiI9134_GetEnableDE(SiI9134PortTypeDef portx);
void SiI9134_SetDE_TOP(SiI9134PortTypeDef portx, int DE_TOP);
int SiI9134_GetDE_TOP(SiI9134PortTypeDef portx);
void SiI9134_SetDE_DLY(SiI9134PortTypeDef portx, int DE_DLY);
int SiI9134_GetDE_DLY(SiI9134PortTypeDef portx);
void SiI9134_SetDE_WIDTH(SiI9134PortTypeDef portx, int DE_WIDTH);
int SiI9134_GetDE_WIDTH(SiI9134PortTypeDef portx);
void SiI9134_SetDE_HEIGHT(SiI9134PortTypeDef portx, int DE_HEIGHT);
int SiI9134_GetDE_HEIGHT(SiI9134PortTypeDef portx);
void SiI9134_SetHsVs(SiI9134PortTypeDef portx, int HsVs);
int SiI9134_GetHsVs(SiI9134PortTypeDef portx);
void SiI9134_SetDataRange(SiI9134PortTypeDef portx, int enable); // Data Range 16-235 To 0-255
void SiI9134_SoftReset(SiI9134PortTypeDef portx);

void SiI9134_Set_AVI_Info(SiI9134PortTypeDef portx,u8 Format);


void SiI9134_SetOutput8Bits(SiI9134PortTypeDef portx);
void SiI9134_SetOutput10Bits(SiI9134PortTypeDef portx);
void SiI9134_SetOutput12Bits(SiI9134PortTypeDef portx);

void SiI9134_SetBitDepth(SiI9134PortTypeDef portx, int bitWidth); // bitWidth: 0 -- 8bit, 1 -- 10bit, 2 -- 12bit
void OutputTypeInit(OUTPUT_TYPE_PARAM_Typedef *stOutputType);


char seleCheck_Si9134(SiI9134PortTypeDef portx);


#endif /* SII9134_H */

