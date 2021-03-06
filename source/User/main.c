/*#########################################################################################

										   _ooOoo_
										  o8888888o
										  88" . "88
										  (| -_- |)
										  O\  =  /O
									   ____/`---'\____
									 .'  \\|     |//  `.
									/  \\|||  :  |||    \
								   /  _||||| -:- |||||-  \
								   |   | \\\  -      |   |
								   | \_|  ''\---/''  |   |
								   \  .-\__  `-`  ___/-. /
								 ___`. .'  /--.--\  `. . __
							  .""'<  `.___\_<|>_/___.'   >'"".
							 | | :  `- \`.;`\ _ /`;.`/ - ` : | |
							 \  \ `-.   \_ __\ /__ _/   .-` /  /
						======`-.____`-.___\_____/___.-`____.-'======
										   `=---='
						^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
									佛祖保佑       永无BUG
###########################################################################################*/

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "SysConfig.h"
#include "uart.h"
#include "mstv.h"
#include "i2c.h"
#include "userpref.h"
#include "control.h"
#include "videoFormatTable.h"
#include "SiI9134.h"
#include "GS2971A.h"
#include "IT6604.h"
#include "rs232command.h"
#include "malloc.h"
#include "fatfs.h"
#include "EDID.h"
#include "soft_spi.h"
#include "spi_flash.h"
#include "Input.h"
//#include "drvSSD1322.h"
#include "lcd.h"
#include "Picture.h"
#include "keyScan.h"
#include "rotaryEncoder.h"
#include "processKey.h"
#include "menu.h"
#include "langStr.h"
#include "FPGA.h"
#include "update_from_fatfs.h"
#include "keyBoardStatus.h"
#include "IDT5P49V5901.h"
#include "RTD2796.h"
#include "UartDebug.h"
#include "menuData.h"
#include "Drv_LCD.h"
#include "Drv_LCD_if.h"
#include "usmart.h"


//###################################################################################################
//modify 20160412

int recv_count;
u8 ledHorseFalg = 1;
u8 sendcardset = 0;
u8 sendCardUsbPlus_old = 0XFF;
BYTE updateFirmware = 0;
BYTE ProductType = 0;//IS_OLD3600;
BYTE IT6604ack = 0;
int ledHorseIndex = 0xff;
int ledHorseCnt = 300000;
int logoDispCnt = 1000;

extern u8 IapBufCmp[PROGRAM_SIZE];
extern int enableMBSourceCnt;
extern u8 IpModifyFlah;	//iP设置标志
extern u8 IpType;
extern char uart0ToUartx;
extern void FpgaDebugTask(void);


//------------------------------------------------------------------------------------------------
void CheckEepromAddr(void)
{
	DebugMessageMain("DEVICE_INFO_ADDRESS = %x\r\n",DEVICE_INFO_ADDRESS);
	DebugMessageMain("PAD_LOCK_ADDRESS = %x\r\n",PAD_LOCK_ADDRESS);
	DebugMessageMain("USER_PREF_ADDRESS = %x\r\n",USER_PREF_ADDRESS);
	DebugMessageMain("USER_SETTING_ADDRESS = %x\r\n",USER_SETTING_ADDRESS);
	DebugMessageMain("USER_PANEL_SPLIT_ADDRESS = %x\r\n",USER_PANEL_SPLIT_ADDRESS);
	DebugMessageMain("USER_OUTPUT_FORMAT_ADDRESS = %x\r\n",USER_OUTPUT_FORMAT_ADDRESS);
	DebugMessageMain("USER_OUTPUT_TYPE_ADDRESS = %x\r\n",USER_OUTPUT_TYPE_ADDRESS);
	DebugMessageMain("USER_SYSTEM_PARAM_ADDRESS = %x\r\n",USER_SYSTEM_PARAM_ADDRESS);
	DebugMessageMain("USER_SETTING_SAVE1_ADDRESS = %x\r\n",USER_SETTING_SAVE1_ADDRESS);
}

//------------------------------------------------------------------------------------------------
void OnInputLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(MCU_STATUS_LED_AHB1PERIPH, ENABLE);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = MCU_STATUS_LED_PIN;
	GPIO_Init(MCU_STATUS_LED_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(MCU_STATUS_LED_PORT, MCU_STATUS_LED_PIN);

	DebugMessageMain("%s\r\n",__func__);
}


void LightInputLed(void)
{
	static char light = 0;
	static u32 testLED_sysTime = 0;
	if(!isSysTickTimeout(testLED_sysTime,1000))
	{
		return;
	}
	testLED_sysTime = GetSysTick();
	if(light)
	{
		GPIO_SetBits(MCU_STATUS_LED_PORT, MCU_STATUS_LED_PIN);
		light = 0;
	}
	else
	{
		GPIO_ResetBits(MCU_STATUS_LED_PORT, MCU_STATUS_LED_PIN);
		light = 1;
	}
}



void SysInit_Rs232Usb(void)
{
	printf("rs232-usb initial...\r\n");
	init_usb();
}



#if 1
void UsbReset(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Configure  GPIO input  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_InitStructure.GPIO_Pin);

	/* Configure SOF VBUS ID DM DP Pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;


	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	GPIO_ResetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);
  
	SysDelay1ms(500);
	// PA8  -- USB_Detect  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);

	/* Configure SOF VBUS ID DM DP Pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);
	
}
#endif

  
void test(void)
{
	static u32 test_sysTime = 0;
	if(!isSysTickTimeout(test_sysTime,5000))
	{
		return;
	}
	test_sysTime = GetSysTick();
}


u8 GetSendCardUsbPlusState(void)
{
	return sendCardUsbPlus_old;
}


void TestLed(void);

int main(void)
{
  	int ledStartSysTime;
	u8 version;
    ledHorseFalg = 1;
	init_SSD1322_rst();
	RTD2796Init();
	FPGA_Program_Start();
	//init_key();	
    InitSysTick();
	InitRtd2796Pipe();
	UARTInit(1,115200);
	UARTInit(3,9600); // 2796
	UARTInit(6,115200); 
	//UARTInit(6,9600); 
	usmart_init(168);
	//UARTSendStr(1,"uart1 ok\r\n");	
	UARTSendStr(6,"uart6 ok\r\n");
	UARTSendStr(3,"uart3 ok\r\n");

	DebugMessageMain("###################################\r\n");
	DebugMessageMain("#-------------$$-4K-Processer---------#\r\n");
	DebugMessageMain("#------------version:%d.%d-----------#\r\n",SOFT_VERSION_HIGH,SOFT_VERSION_LOW);
	DebugMessageMain("#------------%s--------#\r\n",__DATE__);	
	DebugMessageMain("#-------------%s---------------#\r\n",__TIME__);
	DebugMessageMain("###################################\r\n");
	i2c_Init(I2C_A);
	i2c_Init(I2C_B);
	i2c_Init(I2C_C);
	i2c_Init(I2C_D);
	i2c_Init(I2C_E);
	i2c_Init(I2C_F);
	i2c_Init(I2C_G);
	i2c_Init(I2C_H);
	i2c_Init(I2C_I);
	i2c_Init(I2C_J);
	IDT5P49V5901_Reg_Initial(IDT5P49V5901_1,148500000,148500000,148500000,148500000);
	FPGA_Program_Done();
    //CheckBoardVersion();
	//RTD2796StopRun();
	//FPGA_Program_Start();
	//SPI_FLASH_Init(SPI_PORT_A);
    //SPI_FLASH_Init(SPI_PORT_B);
	//SPI_FLASH_Init(SPI_PORT_C);
	
    SoftSpi_UnUsePort(SPI_PORT_A);
    SoftSpi_UnUsePort(SPI_PORT_B);
	SoftSpi_UnUsePort(SPI_PORT_C);
	//FPGA_Program_Done();
	//RTD2796Reset();
	init_key();	

	//TestLed();
	EdidHpInit();
  	OnInputLed();
	init_Pluse(pulse_A);
	InitRs232CommandVariable();
	Drv_LCDInit();
	//Drv_DisplayCharacterReverse(16,0,0,0);
	//while(1);
	//SysDelay1ms(500);
	//LCD_WriteLine1("abcdefghijk");
	//resetEDID(hdmi20PortHdmi20EdidValue);
	//resetEDID(hdmi14PortHdmi14EdidValue);
	//resetEDID(dpPort4k2k60Value);
	//resetEDID(dviPortDviEdidValue);
	//LCD_Init();
	//LCD_Clear();
	//setUserOLEDLum(getUserOLEDLum());
	loadUserPref();
	//SoftSpi_UsePort(SPI_PORT_C);
	//SPI_FLASH_BufferRead(IapBufCmp,SPI_FLASH_FONT_OFFSET,0x800);
	//SoftSpi_UnUsePort(SPI_PORT_C);
	//LcdDispLogo((const char *)IapBufCmp);//LogoGreatVideo
	SysDelay1ms(1000);
	
	loadDeviceInfo();
	loadPadLock();
	//init_key();	
	ledHorseCnt = 300000;	 //这里设置17秒,只是保证在正式设置时间之前跑马灯能正常跑起来
	ledStartSysTime = GetSysTick();

	menuInit();	
	LCD_Clear();
	LCD_WriteLine2("         灵星雨 ");
	setProcessKeyEnable(0);
	//CheckEepromAddr();
	DebugMessageMain("getAllUserSettingStatus() -> %d\r\n", GetSysTick());
	getAllUserSettingStatus();

	if(getCommHardVersion() > 0)
	{
	}
	#if 1
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	SysDelay1ms(2000);
	#endif
	//version = FPGA_GetSoftVersion(NULL,NULL);
	DebugMessageMain("Fpga version = %x\r\n",FPGA_GetSoftVersion(NULL,NULL));
    ledHorseFalg = 2;
    while(ledHorseFalg);
	DebugMessageMain("Enter Main Task!\r\n");
   	setProcessKeyEnable(1);
	menuDisableBusy();
    menuSetFinish(2,8);
	//ResetFactory();
	PanelParamInit();
	menuProcessEvent(MENU_KEY_INFO);
	TIM3_Configuration();
	
	SiI9134_Reset();
	SiI9134_Init(SiI9134_PORT_1);
	SiI9134_Init(SiI9134_PORT_2);
	SiI9134_Init(SiI9134_PORT_3);
	SiI9134_Init(SiI9134_PORT_4);
	SiI9134_Init(SiI9134_PORT_5);
	SiI9134_Init(SiI9134_PORT_6);
	SiI9134_Init(SiI9134_PORT_7);
	SiI9134_Init(SiI9134_PORT_8);
	usb_disk_init();
	//UARTInit(3,9600); // 2796
	#if 0

	while(1)
	{
		task_Key_Scan();
		task_processKey();
		LightInputLed();
		
		//Drv_LCDWriteByte(0xaa, LCD_DATA);
	}
	#else
	//SetKeyLedMode(KEY_HDMI1,LED_ON_ALWAYS);
	while(1)
	{
		usmart_dev.scan();	//执行usmart扫描
		//TaskInputResolution();
		task_2796();
		if(updateFirmware == 0) 
		{
			usb_disk_task();
		}
		task_Key_Scan();
		task_processKey();
		//CheckDoubleKey();
		TaskControl();
		//FpgaDebugTask();
		PcDebugTask();
		menuTask();
		LightInputLed();
		//videoSetInputSource(2);
	}
	#endif
}





