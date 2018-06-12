#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "define.h"

#define ENABlE_WIFI_VCOM  1  



#define CurCard	  getsendcard()->choosecard
#define CurPort   getsendcard()->lxynetwork

typedef struct 
{
	u8 ver_msb;
	u8 ver_lsb;
}WifiLedControl_Setting_TypeDef;




void init_ledControl(void);
void Isr_receive_led(u8 value);

void task_LED(void);
void led_sendBuffer(u8 *buffer, int size);

int led_goToBoot(void);
int led_sendProgramMcuBuffer(int address, u8 *buffer, int size);

void led_getSoftVersion(void);

void led_setSendCardType(BYTE value);
BYTE led_getSendCardType(void);
void led_setSendCardConnectType(BYTE port,BYTE value);
BYTE led_getSendCardConnectType(BYTE port);
void led_setSendCardBrightness(BYTE value);
BYTE led_getSendCardBrightness(void);
void led_setSendCardGamma(BYTE value);
BYTE led_getSendCardGamma(void);
void led_setSendCardPosX(BYTE port, int value);
int  led_getSendCardPosX(BYTE port);
void led_setSendCardPosY(BYTE port, int value);
int led_getSendCardPosY(BYTE port);
void led_setSendCardSizeX(BYTE port, int value);
int led_getSendCardSizeX(BYTE port);
void led_setSendCardSizeY(BYTE port, int value);
int led_getSendCardSizeY(BYTE port);
void led_setSendCardColorTempRed(BYTE value);
BYTE led_getSendCardColorTempRed(void);
void led_setSendCardColorTempGreen(BYTE value);
BYTE led_getSendCardColorTempGreen(void);
void led_setSendCardColorTempBlue(BYTE value);
BYTE led_getSendCardColorTempBlue(void);
void led_setSendCardColorTemp(BYTE index,BYTE value);
BYTE led_getSendCardColorTemp(BYTE index);
void led_setSendCardHLine(BYTE port, BYTE value);
BYTE led_getSendCardHLine(BYTE port);
void led_setSendCardVLine(BYTE port, BYTE value);
BYTE led_getSendCardVLine(BYTE port);
void led_setSendCardHSize(int value);
int led_getSendCardHSize(void);
void led_setSendCardVSize(int value);
int led_getSendCardVSize(void);
void led_setSendCardChooseCard(BYTE value);
BYTE led_getSendCardChooseCard(void);
void led_setSendCardReceiveNumber(BYTE value);
BYTE led_getSendCardReceiveNumber(void);
void led_setSendCardX(int value);
int led_getSendCardX(void);
void led_setSendCardY(int value);
int led_getSendCardY(void);
void led_setSendCardXOffset(BYTE port,int value);
int led_getSendCardXOffset(BYTE port);
void led_setSendCardYOffset(BYTE port,int value);
int led_getSendCardYOffset(BYTE port);
void led_setSendCardNetWork_LXY(BYTE value);
BYTE led_getSendCardNetWork_LXY(void);
void led_setSendCardCascade(BYTE value);
BYTE led_getSendCardCascade(void);
int led_getSendCardInfo(void);
int led_getReceiveCardInfo(void);

void led_setSendCardFactory(void);
void led_setSendCardConnectParam(void);
void led_setSendCardReceiveCardParam(void);
void led_setSendCardColorTempParam(void);
void led_setSendCardParam(void);


void refleshLEDParam(void); //刷新发送卡控制参数
void setLEDSendCardType(BYTE value);
BYTE getLEDSendCardType(void);
void setLEDSendCardChooseCard(BYTE value);
BYTE getLEDSendCardChooseCard(void);
void setLEDSendCardOutputFormat(BYTE value);
BYTE getLEDSendCardOutputFormat(void);

void setLedSendCardStandOuputFormat(BYTE format);


void setLEDSendCardPosX(BYTE port,int value);
int getLEDSendCardPosX(BYTE port);
void setLEDSendCardPosY(BYTE port,int value);
int getLEDSendCardPosY(BYTE port);
void setLEDSendCardSizeX(BYTE port,int value);
int getLEDSendCardSizeX(BYTE port);
void setLEDSendCardSizeY(BYTE port,int value);
int getLEDSendCardSizeY(BYTE port);
void setLEDSendCardParam(void);
void resetLEDSendCardParam(void);

void setLEDNetworkChoose(BYTE value);
void setSendCardPortSelNoSave(BYTE value);
void setSendCardPortSel(BYTE value);
void setLEDSendCardHLine(BYTE port,BYTE value);
void setLEDSendCardVLine(BYTE port,BYTE value);
void setLEDSendCardHSize(int value);
void setLEDSendCardVSize(int value);
void setLEDSendCardXOffset(BYTE port,int value);
void setLEDSendCardYOffset(BYTE port,int value);
void setLEDSendCardConnectType(BYTE port,BYTE value);
u8 setLEDSendCardAutoConnect(u8 display,int value,u8 card,u8 port);
u8 setLEDSendCardConnect(u8 display,int value);
void sendcard_Connect_init(void);

u8 setLEDSendCardWidth(u8 display,int value);

void setLEDSendCardCascade(BYTE value);

void setLEDSendCardBrightness(BYTE value);
void setLEDGamma(BYTE value);

void setLEDSendCardReceiveNumber(BYTE value);
void setLEDSendcardX(int value);
void setLEDSendCardY(int value);
void setLEDSendCardReceiveCardParam(void);

void setLEDSendCardColorTemp(BYTE index,BYTE value);
void setLEDSendCardColorTempParam(void);
void checkLEDSendCard(void);

#endif
