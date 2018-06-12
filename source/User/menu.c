#include <stdio.h>
#include <string.h>

#include "uart.h"
#include "SysConfig.h"

#include "lcd.h"
#include "langStr.h"
#include "systick.h"
#include "userPref.h"
#include "menu.h"
#include "menuData.h"
#include "processKey.h"
#include "rs232command.h"
#include "control.h"
#include "keyScan.h"
#include "videoFormatTable.h"
#include "picture.h"
#include "drvSSD1322.h"
#include "input.h"
#include "IT6604.h"
#include "ledcontrol.h"
#include "sendcard.h"
#include "sendcard_colorLight.h"
#include "sendcard_lxy.h"
#include "keyBoardStatus.h"
#include "RTD2796.h"
#include "FPGA.h"
#include "UartDebug.h"


// Define
#define STACK_MENU_SIZE (20)

#define MENU_DISPLAY_LINE_NUM (4)

#define MENU_DIR_MARK 			">>"
#define MENU_SELECT_ITEM 		"->"
#define MENU_NO_SELECT 			"  "

#define MENU_USE_TIMER 2
#define MENU_TIMER_MS 133


u32 SnPassword = 0;





//#define UP_DOWN_CHANGE_ITEM_LEFT_RIGHT_CHANGE_VALUE 1


// Local Type
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void SetCurrentIsEnterNumberFlag(char flag);
void enterNumberPageProcess(int type, int value);
void enterPasswordTask(void);
void resetMenuPageProcess(int type, int value);
void scaleMenuPageConfirmValue(int type, int value);
u8 CalcSendCardOffsetPort(u8 currentItem,u8 * itemMin,u8 * itemMax);
u8 CalcSendCardPositionPort(u8 currentItem,u8 * itemMin,u8 * itemMax);
u8 CalcSendCardPort(u8 currentItem,u8 * itemMin,u8 * itemMax);
extern u8 GetSendCardUsbPlusState(void);


static char isEnterMenuPage = 0;
static char readFansControlEnable = 1;
static char isDispReflrshInputFormatEnable = 1;

extern const MenuPage menuSendCardConfigPage[];


typedef struct
{
    const MenuPage *menuPage;
    int       currentItem;
    int       menuTtemSize;
} MenuItem;

typedef enum 
{
    MENU_STATE_CHANGE_ITEM,
    MENU_STATE_SELECT_ITEM,
    MENU_STATE_CHANGE_AND_SET_VALUE,
    MENU_STATE_SET_VALUE,
    MENU_STATE_CHANGE_VALUE
}MenuStateType;

// same order  MenuStateType
const char * menuStateStr[] =
{
	"->", //MENU_STATE_CHANGE_ITEM,
    " *", //MENU_STATE_SELECT_ITEM,
    " *", //MENU_STATE_CHANGE_AND_SET_VALUE,
    "->", //MENU_STATE_SET_VALUE,
    " *", //MENU_STATE_CHANGE_VALUE
};

extern MenuItem menuList[];

MenuItem *pCurrentMenuPage;
BYTE detailSource = 0;
static u32 menuNoOperationTime = 0;
static Exec_Func menuMainTaskFunc = 0;
static u16 menuMainTimeout = 1000;
static u32 menuTaskTime = 0;
extern MenuItem menuList[];

extern u8 usbconnectflg;


const MenuPage * stackMenuPage[STACK_MENU_SIZE];

int stackSize = 0;

int menuState = MENU_STATE_CHANGE_ITEM;

int MenuDisplayIndex = 0;
u8 MenuFoucsIndex = 0;
int menuValueList[30];
static BYTE menuSubItem = 0;

static BYTE setCustomBeyondFlag = 0;


int dispValue[3];
#if(DEVICE_TYPE == 0x516)
static int dispBlinkValue[4];
static int dispBlinkIndex;
#endif
static char CurrentIsEnterNumberFlag = 0;

int cusEdidWidth = 0,cusEdidHeight = 0,cusEdidFreq = 0;
int cusFormatWidth = 0,cusFormatHeight = 0,cusFormatFreq = 0;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// Local Function
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void changeNoSetProcess(int type, int value);

void menuResetMainTimeout(void)
{
	menuTaskTime = GetSysTick();
}

void menuAddMainFunc(u16 timeout, Exec_Func func)
{
	if(menuMainTaskFunc != 0)
	{
		if(func != menuMainTaskFunc)
		{
			menuMainTaskFunc();
		}		
	}
	menuMainTaskFunc = func;
	menuMainTimeout = timeout;
	menuTaskTime = GetSysTick();
}

void menuRemoveMainFunc(Exec_Func func)
{
	menuMainTaskFunc = 0;
}

void menuDrawItemBar(void)
{
	//LCD_DrawItemBar(pCurrentMenuPage->menuTtemSize,pCurrentMenuPage->currentItem);
}


void menuClearItemBar(void)
{
	//LCD_ClearItemBar();
}



/*
 * Function: menuSetBusy
 * Description: 在LCD 屏上指定位置显示系统忙的图形
 * 
 */

static BYTE _busyRow;
static BYTE _busyColumn;
static volatile Exec_Func menuTimerHandleFunc = 0;

#if (DEVICE_TYPE == 0x198)

#endif

u8 takeStatus = 0;

u8 port1Alpha_a = 0;//GetUserSetting()->userSplitParam.dvi1LayerAlpha_A;
u8 port1Alpha_b = 0;//GetUserSetting()->userSplitParam.dvi1LayerAlpha_B;
u8 port2Alpha_a = 0;//GetUserSetting()->userSplitParam.dvi2LayerAlpha_A;
u8 port2Alpha_b = 0;//GetUserSetting()->userSplitParam.dvi2LayerAlpha_B;
u8 port3Alpha_a = 0;//GetUserSetting()->userSplitParam.dvi3LayerAlpha_A;
u8 port3Alpha_b = 0;//GetUserSetting()->userSplitParam.dvi3LayerAlpha_B;
u8 port4Alpha_a = 0;//GetUserSetting()->userSplitParam.dvi4LayerAlpha_A;
u8 port4Alpha_b = 0;//GetUserSetting()->userSplitParam.dvi4LayerAlpha_B;

extern u8 PgmOldInput,PrvOldInput;
extern u8 PgmCurrentInput,PrvCurrentInput;
extern u8 PgmCurrentRegion,PrvCurrentRegion;
extern u8 CurrentDisplayMode;


vu16 EcInputCountNew = 0;
vu16 EcInputCountOld = 0;
u8 IsEcInputFinish = 0;

	
void SetIsEcInputFinish(u8 value)
{
	IsEcInputFinish = value;
}

u16 GetIsEcInputFinish(void)
{
	return IsEcInputFinish;
}

	
void SetEcInputValueNew(void)
{
	EcInputCountNew++;
}

u16 GetEcInputValueNew(void)
{
	return EcInputCountNew;
}

void ClearEcInputValueNew(void)
{
	EcInputCountNew = 0;
}

void SetEcInputValueOld(void)
{
	EcInputCountOld = GetEcInputValueNew();
}

u16 GetEcInputValueOld(void)
{
	return EcInputCountOld;
}

void ClearEcInputValueOld(void)
{
	EcInputCountOld = 0;
}


void SetPipWinSize(int index,int value)
{
	switch(index)
	{
		case pipMainWinPosXValue:
			GetPipParam()->gPipMainWinPosX = value;
		break;
		case pipMainWinPosYValue:
			GetPipParam()->gPipMainWinPosY = value;
		break;
		case pipMainWinSizeXValue:
			GetPipParam()->gPipMainWinSizeX = value;
		break;
		case pipMainWinSizeYValue:
			GetPipParam()->gPipMainWinSizeY = value;
		break;
		case pipSubWinPosXValue:
			GetPipParam()->gPipSubWinPosX = value;
		break;
		case pipSubWinPosYValue:
			GetPipParam()->gPipSubWinPosY = value;
		break;
		case pipSubWinSizeXValue:
			GetPipParam()->gPipSubWinSizeX = value;
		break;
		case pipSubWinSizeYValue:
			GetPipParam()->gPipSubWinSizeY = value;
		break;
	}
}



void SetInputPortLedStatus(u8 PgmInput,u8 PrvInput,u8 disMode)
{
	switch(PgmInput)
	{
		case _DP_INPUT:
			SetKeyLedMode(LED_DP,LED_ON_ALWAYS);
			SetKeyLedMode(LED_HDMI1,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI2,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_DVI,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_VGA,LED_OFF_ALWAYS);
		break;
		case _HDMI14_INPUT:
			SetKeyLedMode(LED_DP,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI1,LED_ON_ALWAYS);
			SetKeyLedMode(LED_HDMI2,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_DVI,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_VGA,LED_OFF_ALWAYS);
		break;
		case _HDMI20_INPUT:
			SetKeyLedMode(LED_DP,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI1,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI2,LED_ON_ALWAYS);
			SetKeyLedMode(LED_DVI,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_VGA,LED_OFF_ALWAYS);
		break;
		case _DVI_INPUT:
			SetKeyLedMode(LED_DP,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI1,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI2,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_DVI,LED_ON_ALWAYS);
			SetKeyLedMode(LED_VGA,LED_OFF_ALWAYS);
		break;
		case _VGA_INPUT:
			SetKeyLedMode(LED_DP,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI1,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_HDMI2,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_DVI,LED_OFF_ALWAYS);
			SetKeyLedMode(LED_VGA,LED_ON_ALWAYS);
		break;
		default:
		break;
		
	}
	if((disMode == _OSD_DM_2P_LR) || (disMode == _OSD_DM_2P_TB))
	{
		switch(PrvInput)
		{
			case _DP_INPUT:
				SetKeyLedMode(LED_DP,LED_BLINK_100MS);
			break;
			case _HDMI14_INPUT:
				SetKeyLedMode(LED_HDMI1,LED_BLINK_100MS);
			break;
			case _HDMI20_INPUT:
				SetKeyLedMode(LED_HDMI2,LED_BLINK_100MS);
			break;
			case _DVI_INPUT:
				SetKeyLedMode(LED_DVI,LED_BLINK_100MS);
			break;
			case _VGA_INPUT:
				SetKeyLedMode(LED_VGA,LED_BLINK_100MS);
			break;
			default:
			break;
			
		}
	}
	else
	{
		//SetKeyLedMode(KEY_OUTPUT1,LED_ON_ALWAYS);
		//SetKeyLedMode(KEY_OUTPUT2,LED_ON_ALWAYS);
		//SetKeyLedMode(KEY_OUTPUT3,LED_ON_ALWAYS);
		//SetKeyLedMode(KEY_OUTPUT4,LED_ON_ALWAYS);
	}
	printf("PgmCurrentInput = %d\r\n",PgmInput);
	printf("PrvCurrentInput = %d\r\n",PrvInput);
	printf("PgmCurrentRegion = %d\r\n",PgmCurrentRegion);
	printf("PrvCurrentRegion = %d\r\n",PrvCurrentRegion);
	printf("CurrentDisplayMode = %d\r\n",disMode);
}



void FadeInFadeOut0S(void)
{
	if(takeStatus == 0)
	{
		FPGA_set(0x20, 0);
		FPGA_set(0x40, 255);

		FPGA_set(0x60, 255);
		FPGA_set(0x80, 0);
	}
	else
	{
		FPGA_set(0x20, 255);
		FPGA_set(0x40, 0);

		FPGA_set(0x60, 0);
		FPGA_set(0x80, 255);
	}	
}

void FadeInFadeOut05S(int fadeTime)
{
	u8 alphaMax = GetUserSettingParam()->userSplitParam.alpha;
	u8 alphav = 0;
	int i;
//	u8 alphaStep;
	//alphaStep = 500/256;
	for(i = 0;i < 127;i++)
	{
		SysDelay1ms(fadeTime/128);
		alphav += 2;
		if(alphav >= alphaMax)
			alphav = alphaMax;
		if(takeStatus == 0)
		{
			port1Alpha_a = alphaMax - alphav; 
			port1Alpha_b = alphav;

			port3Alpha_a = port1Alpha_b;
			port3Alpha_b = port1Alpha_a;
		}
		else
		{
			port1Alpha_a = alphav; 
			port1Alpha_b = alphaMax - alphav;

			port3Alpha_a = port1Alpha_b;
			port3Alpha_b = port1Alpha_a;
		}	
		FPGA_set(0x20, port1Alpha_a);
		FPGA_set(0x40, port1Alpha_b);

		FPGA_set(0x60, port3Alpha_a);
		FPGA_set(0x80, port3Alpha_b);
	}
}
	
void TakeTask(int fadeTime)
{
//	static BYTE fadeCount = 0;	
//	u8 alphav = 0;
	u8 alphaMax = GetUserSettingParam()->userSplitParam.alpha;
	u8 regionInput,PgmInput;
	fadeTime = fadeTime;
	regionInput = GetPrvCurrentRegion();
	SetPrvCurrentRegion(GetPgmCurrentRegion());
	SetPgmCurrentRegion(regionInput);

	PgmInput = GetPrvCurrentInput();
	SetPrvCurrentInput(GetPgmCurrentInput());
	SetPgmCurrentInput(PgmInput);

	SetInputPortLedStatus(GetPgmCurrentInput(),GetPrvCurrentInput(),GetPgmCurrentDisplayMode());
	switch(fadeTime)
	{
		case 0:
			FadeInFadeOut0S();
		break;
		default:
			FadeInFadeOut05S(fadeTime*100);
		break;
	}

	GetPanleAndSplitParamRam()->inputSource1 = GetPgmCurrentInput();
	GetPanleAndSplitParamRam()->inputSource2 = GetPrvCurrentInput();
	SavePanleAndSplitParam();

	menuEnterStatusPage();
}


void menuBusyTimerTask(void)
{
	static BYTE busyCount = 0;
	switch(busyCount % 10)
	{
		case 0:
			LCD_WriteStr(_busyRow,_busyColumn, "[>    ]");
			break;
		case 1:
			LCD_WriteStr(_busyRow,_busyColumn, "[ >   ]");
			break;
		case 2:
			LCD_WriteStr(_busyRow,_busyColumn, "[  >  ]");
			break;
		case 3:
			LCD_WriteStr(_busyRow,_busyColumn, "[   > ]");
			break;
		case 4:
			LCD_WriteStr(_busyRow,_busyColumn, "[    >]");
			break;
		case 5:
			LCD_WriteStr(_busyRow,_busyColumn, "[    <]");
			break;
		case 6:
			LCD_WriteStr(_busyRow,_busyColumn, "[   < ]");
			break;
		case 7:
			LCD_WriteStr(_busyRow,_busyColumn, "[  <  ]");
			break;
		case 8:
			LCD_WriteStr(_busyRow,_busyColumn, "[ <   ]");
			break;
		case 9:
			LCD_WriteStr(_busyRow,_busyColumn, "[<    ]");
			break;
	}
	busyCount++;
}

void menuBusyTimerTask_fb(void)
{
	static BYTE busyCount = 0;
	switch(busyCount % 10)
	{
		case 0:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[>    ]");
			break;
		case 1:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[ >   ]");
			break;
		case 2:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[  >  ]");
			break;
		case 3:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[   > ]");
			break;
		case 4:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[    >]");
			break;
		case 5:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[    <]");
			break;
		case 6:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[   < ]");
			break;
		case 7:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[  <  ]");
			break;
		case 8:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[ <   ]");
			break;
		case 9:
			LCD_WriteStr_fb(_busyRow,_busyColumn, "[<    ]");
			break;
	}
	busyCount++;
}

void menuBusyTimerDotTask(void)
{
	static BYTE busyCount = 0;
	switch(busyCount%6)
	{
		case 0:
			LCD_WriteStr(_busyRow,_busyColumn, ".     ");
			break;
		case 1:
			LCD_WriteStr(_busyRow,_busyColumn, "..    ");
			break;
		case 2:
			LCD_WriteStr(_busyRow,_busyColumn, "...   ");
			break;
		case 3:
			LCD_WriteStr(_busyRow,_busyColumn, "....  ");
			break;
		case 4:
			LCD_WriteStr(_busyRow,_busyColumn, "..... ");
			break;
		case 5:
			LCD_WriteStr(_busyRow,_busyColumn, "......");
			break;
	}
	busyCount++;
}

void menuSetBusy(int row, int column)
{
	if(row > 3)  //if((row <= 0) || (row > 1))
	{
		return;
	}
	_busyRow = row;
	if((column <= 0) || (column > 13))
	{
		return;
	}
	_busyColumn = column;
	menuTimerHandleFunc = menuBusyTimerTask;
	TIM_Cmd(TIM2, ENABLE);	//enable_timer(MENU_USE_TIMER);
}
void menuSetBusy_Dot(int row, int column)
{
	//printf("menuSetBusy_Dot(%d,%d) ---> %d\r\n", row, column,GetSysTick());
	if(row > 3)  //if((row <= 0) || (row > 1))
	{
		return;
	}
	_busyRow = row;
	if((column <= 0) || (column > 13))
	{
		return;
	}
	_busyColumn = column;
	menuTimerHandleFunc = menuBusyTimerDotTask;
	TIM_Cmd(TIM2, ENABLE);//   enable_timer(MENU_USE_TIMER);
}
void menuSetBusy_fb(int row, int column)
{
	if(row > 3)  //if((row <= 0) || (row > 1))
	{
		return;
	}
	_busyRow = row;
	if((column <= 0) || (column > 13))
	{
		return;
	}
	_busyColumn = column;
	menuTimerHandleFunc = menuBusyTimerTask_fb;
	
    TIM_Cmd(TIM2, ENABLE);//   enable_timer(MENU_USE_TIMER);
}

void menuDisableBusy(void)
{
	//printf("menuDisableBusy() ---> %d\r\n", GetSysTick());
	TIM_Cmd(TIM2, DISABLE);//disable_timer(MENU_USE_TIMER);
	LCD_WriteStr(_busyRow,_busyColumn, "       ");
	menuTimerHandleFunc = 0;
}

void menuSetFinish(char row,char column)
{
	LCD_WriteStr(row,column,langGetStr(finishedStr));
}
void menuIsrTimerHandler(void)
{
	if(menuTimerHandleFunc)
	{
		menuTimerHandleFunc();
	}
}


const MenuPage * popMenuPage(void)
{
    if(stackSize <= 0)
    {
        return 0;
    }
    else
    {
        stackSize--;
        return stackMenuPage[stackSize];
    }
}

void pushMenuPage(const MenuPage * menuPage)
{
	//printf("pushMenuPage\r\n");
    if(stackSize >= STACK_MENU_SIZE)
    {
        //printf("menu stack overflow!\r\n");
        return;
    }
    else
    {
        stackMenuPage[stackSize++] = menuPage;
    }
}

void clearMenuPageStack(void)
{
	stackSize = 0;
}

void FindMenuPageIndexList(void)
{
//	int i;

	
}


MenuItem * findMenuPage(const MenuPage *menuPage)
{
    int i = 0;
    while(1)
    {
        if(menuList[i].menuPage == menuPage)
        {
            return &menuList[i];
        }
        if(menuList[i].menuPage == 0)
        {
            return &menuList[0];
        }
        i++;
    }
}

#define MENU_START_ITEM (1)

BYTE checkIsLock(void) 			//added by cyj
{

	if(getValue(keyLockValue)) 
	{
		menuEnterPage(MENU_INDEX_NOTHING);
		dispLineCenter_L(0,langGetStr(lockPanelTipsStr));	
		dispLineCenter_L(1,langGetStr(pressMenuKeyStr));	
		dispLineCenter_L(2,langGetStr(toUnlockPanelStr));	
		LCD_WriteLine4("");		
		return TRUE;
	}
	else if(getUserPadLockState())
	{
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		dispLineCenter_L(0,langGetStr(deviceInSerialControlStr));	
		dispLineCenter_L(1,langGetStr(lockFrontPanelStr));	
		dispLineCenter_L(2,langGetStr(pressMenuKeyStr));	
		dispLineCenter_L(3,langGetStr(toUnlockPanelStr));	
		return TRUE;
	}
	return FALSE;
}



BYTE checkInputFormatChangedPreview(BYTE *format)
{
//	if(*format == GetInputFormatPreview())
	{
		return TRUE;
	}
	
//	return FALSE;
}



void setLastSource(BYTE *format,BYTE value)
{
	*format = value;
}

//static BYTE lastFormatInput,lastFormatInput_PIP;
static char noOperationFlag = 0;

void menuKeyProcess(int type, int value)
{

    const MenuPage *menuPage;
    int item = 1;
//	static u8 black_flag = 0;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	if(!getValue(keyLockValue)&& getUserPadLockState()&& (type < MENU_KEY_SIZE)&&(type != MENU_KEY_INFO)&&(type != MENU_KEY_UNLOCK))
	{
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();

		dispLineCenter_L(0,langGetStr(deviceInSerialControlStr));	
		dispLineCenter_L(1,langGetStr(lockFrontPanelStr));	
		dispLineCenter_L(2,langGetStr(pressMenuKeyStr));	
		dispLineCenter_L(3,langGetStr(toUnlockPanelStr));	
		return;
	}
	if(type == MENU_KEY_UNLOCK)
	{
		if(getUserPadLockState())
		{
			setUserPadLock(0,0);//手动解除上位机控制
		}
		setValue(keyLockValue, 0);
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_WriteLine1_L("");
		//LCD_WriteLine2_L(langGetStr(unLockPanelStr));
		dispLineCenter_L(1,langGetStr(unLockPanelStr));	
		LCD_WriteLine3_L("");
		LCD_WriteLine4_L("");
		SysDelay1ms(1000);
		menuProcessEvent(MENU_KEY_INFO);
		return;
	}
	if(type == MENU_KEY_LOCK)
	{
		setValue(keyLockValue, 1);
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_WriteLine1_L("");
		#if 0
		LCD_WriteLine2_L(langGetStr(lockPanelTipsStr));
		#else
		dispLineCenter_L(1,langGetStr(lockPanelTipsStr));	
		#endif
		LCD_WriteLine3_L("");
		LCD_WriteLine4_L("");
		return;
	}
	if(getValue(keyLockValue) && (type < MENU_KEY_SIZE)&&(type != MENU_KEY_INFO))
	{

	}  

    switch(type)
    {
    case MENU_KEY_MENU_ROOT:
		isEnterMenuPage = 0;
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);
		//printf("MENU_KEY_MENU_ROOT\r\n");
		clearMenuPageStack();
		pCurrentMenuPage = &menuList[MENU_INDEX_MAIN];

		pCurrentMenuPage->currentItem = MENU_START_ITEM;
	    pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
		break;
    case MENU_KEY_ESC:
		pCurrentMenuPage->menuPage->pfProcess(MENU_KEY_MENU, 0);
		break;

    case MENU_KEY_MENU:
		printf("menu_1,");
		if(checkIsLock())
		{
			return;
		}
		printf("2,");
        if((menuState == MENU_STATE_CHANGE_ITEM) || (menuState == MENU_STATE_SET_VALUE))
        {
			printf("3,");
        	isEnterMenuPage = 0;
            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);
            menuPage = popMenuPage();
            if(menuPage == 0)
            {
				printf("4,");
            	#if 1
            	if(menuIsCurrentPageIndex(MENU_INDEX_MAIN))
				#else
            	if((((menuIsCurrentPageIndex(MENU_INDEX_MAIN))||(menuIsCurrentPageIndex(MENU_INDEX_MAIN_EN_AUDIO)))&&(GetLanguage()== 0))
				 ||(((menuIsCurrentPageIndex(MENU_INDEX_MAIN_CN))||(menuIsCurrentPageIndex(MENU_INDEX_MAIN_CN_AUDIO)))&&(GetLanguage()== 1)))
				 #endif
            	{
					 printf("5,");
					//printf("menuKeyProcess_MENU_KEY_MENU,menuIsCurrentPageIndex(MENU_INDEX_MAIN)\r\n");
	           		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_STATUS);
            	}
				else
				{
					printf("6,");
					//setDispLineEn();//dispLineEn = 1;

					pCurrentMenuPage = &menuList[MENU_INDEX_MAIN];

	                pCurrentMenuPage->currentItem = MENU_START_ITEM;
	                pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
				}
            }
			else if(menuIsCurrentPageIndex(MENU_INDEX_STATUS))
			{
				printf("7,");
				//printf("menuKeyProcess_MENU_KEY_MENU,menuState = %d,menuIsCurrentPageIndex(MENU_INDEX_STATUS)_____",menuState);
				if(noOperationFlag == 1) //长时间不操作,转入待机界面,这时按MENU键时，返回之前菜单项
				{
					printf("8,");
					//printf("findMenuPage\r\n");
					noOperationFlag = 0;
					pCurrentMenuPage = findMenuPage(menuPage);
				}
				else
				{
					printf("9,");
					//printf("enter main page\r\n");
					pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0); //先退出当前菜单<--20151014
					clearMenuPageStack();
					//setDispLineEn();//dispLineEn = 1;
					pCurrentMenuPage = &menuList[MENU_INDEX_MAIN];

					pCurrentMenuPage->currentItem = MENU_START_ITEM;
				}
	            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
			}
			else if(menuIsCurrentPageIndex(MENU_INDEX_CHANGE_DATE)||menuIsCurrentPageIndex(MENU_INDEX_CHANGE_TIME))
			{
				printf("10,");
				//printf("menuKeyProcess_MENU_KEY_MENU,menuPage: %d,menuIsCurrentPageIndex(MENU_INDEX_STATUS)\r\n",menuPage);
                pCurrentMenuPage = findMenuPage(menuPage);
	            pCurrentMenuPage->currentItem = MENU_START_ITEM;
                pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
			}
            else
            {
				printf("11,");
				//printf("menuKeyProcess_MENU_KEY_MENU,menuState = %d,findMenuPage\r\n",menuState);
                pCurrentMenuPage = findMenuPage(menuPage);
                pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
            }
        }
        else
        {
			printf("12,");
            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
			menuState = MENU_STATE_CHANGE_ITEM;
        }
        break;
	case MENU_KEY_BRIGHTNESS:      //added by cyj for 1.50
		if(checkIsLock())
		{
			return;
		}
		//printf("\r\n++++Press key BRIGHTNESS\r\n");
		menuList[MENU_INDEX_IMAGE_QUALITY].currentItem = 1;
		menuEnterPage(MENU_INDEX_IMAGE_QUALITY);
        //menuState = MENU_STATE_CHANGE_VALUE; //只允许先按确认然后再输数字
		break;
    case MENU_KEY_NEXT:
        {
			if(checkIsLock())
			{
				return;
			}
			
            if(menuPage[item].type == MENU_TYPE_DIR)
            {  
                pushMenuPage(menuPage);
                pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);
                menuPage = (MenuPage *)menuPage[item].value;
				//readFansControlEnable = 1;
				#if 0
				if(menuPage == menuList[MENU_INDEX_PIP_MODE].menuPage)
				{
					if(GetSystemParam()->PipModeSwitch == 1)
					{
						pCurrentMenuPage = &menuList[MENU_INDEX_PIPIMAGEA];
					}
					else
					{
						pCurrentMenuPage = findMenuPage(menuPage);
					}
				}
				else
				{
					pCurrentMenuPage = findMenuPage(menuPage);
				}
				#else
				pCurrentMenuPage = findMenuPage(menuPage);
				#endif
				pCurrentMenuPage->currentItem = MENU_START_ITEM;
                pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
                break;
            }
            else if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
            {
				if(menuIsCurrentPageIndex(MENU_INDEX_EDID_HDMI20_PORT) ||
					menuIsCurrentPageIndex(MENU_INDEX_EDID_HDMI14_PORT) || 
					menuIsCurrentPageIndex(MENU_INDEX_EDID_DP_PORT) ||
					menuIsCurrentPageIndex(MENU_INDEX_EDID_DVI_PORT) ||
					menuIsCurrentPageIndex(MENU_INDEX_OUTPUT))
					
				{
					setEDIDDesPort(menuPage[item].value - hdmi20PortCustomEdidValue);
					menuState = MENU_STATE_CHANGE_VALUE;
				}
                switch(menuState)
                {
                case MENU_STATE_CHANGE_ITEM:
					//printf("MENU_STATE_CHANGE_ITEM\r\n");
                    menuState = MENU_STATE_SELECT_ITEM;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SELECT_ITEM, 0);
                    break;
                case MENU_STATE_SELECT_ITEM:
					//printf("MENU_STATE_SELECT_ITEM\r\n");
                    menuState = MENU_STATE_CHANGE_ITEM;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
                    break;
                case MENU_STATE_CHANGE_VALUE:
					//printf("MENU_STATE_CHANGE_VALUE\r\n");
                    menuState = MENU_STATE_SET_VALUE;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
                    break;
                case MENU_STATE_CHANGE_AND_SET_VALUE:
					//printf("MENU_STATE_CHANGE_AND_SET_VALUE\r\n");
					if(menuIsCurrentPageIndex(MENU_INDEX_SCALE) == TRUE)
					{
//						setScaleValueToVideo(menuPage[item].value);
					}
                    else if(menuPage[pCurrentMenuPage->currentItem].value == colorTempValue)
                    {
                        setValue(colorTempValue, menuValueList[item]);
                    }                   
                    
                    menuState = MENU_STATE_CHANGE_ITEM;
                    //pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_ENTER, 0);
                    break;
                case MENU_STATE_SET_VALUE:
					//printf("MENU_STATE_SET_VALUE\r\n");
                    menuState = MENU_STATE_CHANGE_ITEM;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
                    break;
                }
            }
			
			else if((menuPage[item].type == MENU_TYPE_CUSTOMNUM))
			{
				if( menuIsCurrentPageIndex(MENU_INDEX_4PLT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_4PLB) ||\
					menuIsCurrentPageIndex(MENU_INDEX_4PRT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_4PRB) ||\
					menuIsCurrentPageIndex(MENU_INDEX_OUTPUT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_PIP_MODE)||\
					menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) ||\
					menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) ||\
					menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_OUTPUT0_Layer) ||\
					menuIsCurrentPageIndex(MENU_INDEX_OUTPUT1_Layer) ||\
					menuIsCurrentPageIndex(MENU_INDEX_OUTPUT2_Layer) ||\
					menuIsCurrentPageIndex(MENU_INDEX_OUTPUT3_Layer) ||\
					menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) ||\
					menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT)
					)
				{
					menuState = MENU_STATE_CHANGE_VALUE;
				}
				switch(menuState)
                {
                case MENU_STATE_CHANGE_ITEM:
					//printf("MENU_STATE_CHANGE_ITEM\r\n");
                    menuState = MENU_STATE_SELECT_ITEM;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SELECT_ITEM, 0);
                    break;
                case MENU_STATE_SELECT_ITEM:
					//printf("MENU_STATE_SELECT_ITEM\r\n");
                    menuState = MENU_STATE_CHANGE_ITEM;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
                    break;
                case MENU_STATE_CHANGE_VALUE:
					//printf("MENU_STATE_CHANGE_VALUE\r\n");
                    menuState = MENU_STATE_SET_VALUE;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
                    break;
                case MENU_STATE_CHANGE_AND_SET_VALUE:
					//printf("MENU_STATE_CHANGE_AND_SET_VALUE\r\n");
					if(menuIsCurrentPageIndex(MENU_INDEX_SCALE) == TRUE)
					{
//						setScaleValueToVideo(menuPage[item].value);
					}
                    else if(menuPage[pCurrentMenuPage->currentItem].value == colorTempValue)
                    {
                        setValue(colorTempValue, menuValueList[item]);
                    }                   
                    
                    menuState = MENU_STATE_CHANGE_ITEM;
                    //pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_ENTER, 0);
                    break;
                case MENU_STATE_SET_VALUE:
					//printf("MENU_STATE_SET_VALUE\r\n");
                    menuState = MENU_STATE_CHANGE_ITEM;
                    pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
                    break;
                }
			}
        }
        break;
    case MENU_KEY_UP:
		#ifdef UP_DOWN_CHANGE_ITEM_LEFT_RIGHT_CHANGE_VALUE
		menuState = MENU_STATE_CHANGE_ITEM;
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_PREV_ITEM, 0);
		#else
		if(checkIsLock())
		{
			return;
		}
		
        if(menuState == MENU_STATE_CHANGE_ITEM)
        {
			//printf("MENU_KEY_UP_menuState == MENU_STATE_CHANGE_ITEM\r\n");
            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_NEXT_ITEM, 0);

        }
        else
        {
			//printf("MENU_KEY_UP_menuState != MENU_STATE_CHANGE_ITEM\r\n");
        	if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
        	{
            	pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_NEXT_VALUE, 0);
        	}
        }
		#endif
        break;
    case MENU_KEY_DOWN:
		#ifdef UP_DOWN_CHANGE_ITEM_LEFT_RIGHT_CHANGE_VALUE
		menuState = MENU_STATE_CHANGE_ITEM;
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_NEXT_ITEM, 0);
		#else
		if(checkIsLock())
		{
			return;
		}
        if(menuState == MENU_STATE_CHANGE_ITEM)
        {
			//printf("MENU_KEY_DOWN_menuState == MENU_STATE_CHANGE_ITEM\r\n");
            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_PREV_ITEM, 0);

        }
        else
        {
			//printf("MENU_KEY_DOWN_menuState != MENU_STATE_CHANGE_ITEM\r\n");
        	if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
        	{
            	pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_PREV_VALUE, 0);
        	}
        }
		#endif
        break;
    case MENU_KEY_LEFT:
		#ifdef UP_DOWN_CHANGE_ITEM_LEFT_RIGHT_CHANGE_VALUE
		if(menuPage[item].type == MENU_TYPE_DIR)
		{
			pCurrentMenuPage->menuPage[item].pfProcess(MENU_KEY_NEXT, 0);
		}
		else if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE))
		{
        	menuState = MENU_STATE_CHANGE_VALUE;
        	pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_PREV_VALUE, 0);
		}
		#else
		if(checkIsLock())
		{
			return;
		}
		
		if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
		{
        	menuState = MENU_STATE_CHANGE_VALUE;
        	pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_PREV_VALUE, 0);
		}
		#endif
        break;
    case MENU_KEY_RIGHT:
		#ifdef UP_DOWN_CHANGE_ITEM_LEFT_RIGHT_CHANGE_VALUE
		if(menuPage[item].type == MENU_TYPE_DIR)
		{
			pCurrentMenuPage->menuPage[item].pfProcess(MENU_KEY_NEXT, 0);
		}
		else if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE))
		{
        	menuState = MENU_STATE_CHANGE_VALUE;
        	pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_NEXT_VALUE, 0);
		}
		#else
		if(checkIsLock())
		{
			return;
		}
		
		if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
		{
        	menuState = MENU_STATE_CHANGE_VALUE;
        	pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_NEXT_VALUE, 0);
		}
		#endif
        break;
	case MENU_KEY_HDMI_TAKE:
		if(checkIsLock())
		{
			return;
		}
		menuState = MENU_STATE_CHANGE_ITEM;
	#ifdef SOURCE_TAKE_MODE
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_PROGRAM_MASK | VIPDRIVE_INPUT_HDMI);
	#else	
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_HDMI);
	#endif		
		break;		
	case MENU_KEY_HDMI:
		if(checkIsLock())
		{
			return;
		}

//		setCheckSignalDelay(5000);

		menuState = MENU_STATE_CHANGE_ITEM;
	#ifdef SOURCE_TAKE_MODE
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_PREVEW_MASK | VIPDRIVE_INPUT_HDMI);
	#else
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_HDMI);
	#endif
		break;
	case MENU_KEY_SDI_TAKE:
		if(checkIsLock())
		{
			return;
		}
		menuState = MENU_STATE_CHANGE_ITEM;
	#ifdef SOURCE_TAKE_MODE
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_PROGRAM_MASK | VIPDRIVE_INPUT_DP);
	#else	
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_DP);
	#endif		
		break;
	case MENU_KEY_SDI:
		if(checkIsLock())
		{
			return;
		}


//		setCheckSignalDelay(5000);

		menuState = MENU_STATE_CHANGE_ITEM;
	#ifdef SOURCE_TAKE_MODE
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_PREVEW_MASK | VIPDRIVE_INPUT_DP);
	#else	
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_SDI);
	#endif
		break;		
	case MENU_KEY_DVI_TAKE:
		if(checkIsLock())
		{
			return;
		}
		menuState = MENU_STATE_CHANGE_ITEM;
	#ifdef SOURCE_TAKE_MODE
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_PROGRAM_MASK | VIPDRIVE_INPUT_DVI);
	#else	
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_DVI);
	#endif		
		break;	
//	case MENU_KEY_DVI:
//		if(checkIsLock())
//		{
//			return;
//		}

////		setCheckSignalDelay(5000);

//		menuState = MENU_STATE_CHANGE_ITEM;
//	#ifdef SOURCE_TAKE_MODE
//		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_PREVEW_MASK | VIPDRIVE_INPUT_DVI);
//	#else	
//		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_INPUT_SOURCE, VIPDRIVE_INPUT_DVI);
//	#endif
//		break;
//	case MENU_KEY_DP:	
//		break;
	case MENU_KEY_INFO:
		menuState = MENU_STATE_CHANGE_ITEM;
		//menuDisableBusy();
		SetKeyLedMode(KEY_MENU,LED_OFF_ALWAYS);
		if(menuIsCurrentPageIndex(MENU_INDEX_STATUS))
		{

		}
		else
		{
			//menuDisableBusy();
			pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_STATUS);
		}
		break;
	case MENU_KEY_OUTPUT:
		if(checkIsLock())
		{
			return;
		}
		
		menuEnterPage(MENU_INDEX_OUTPUT_FORMAT);
		break;
	case MENU_KEY_SAVE:
		if(checkIsLock())
		{
			return;
		}
		if(CurrentIsEnterNumberFlag == 1)
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		else
		{
			SetCurrentEnterNumberStatus(0xff);
			SetPrvEnterNumberStatus(0xaa);
			SetKeyLedMode(KEY_SAVE,LED_ON_ALWAYS);
			SetCurrentIsEnterNumberFlag(0);
			menuEnterPage(MENU_INDEX_SAVE_TO);
		}
		//menuEnterPage(MENU_INDEX_SAVE_TO);
		break;
	case MENU_KEY_LOAD:
		if(checkIsLock())
		{
			return;
		}
		
		menuEnterPage(MENU_INDEX_LOAD_FROM);
		break;
	case MENU_KEY_SAVE1:
	case MENU_KEY_SAVE2:
	case MENU_KEY_SAVE3:
		menuState = MENU_STATE_CHANGE_ITEM;
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_LOAD_SAVE, type-MENU_KEY_SAVE1);
		break;
	case MENU_KEY_MAP:

		break;
	case MENU_KEY_SCALE:
		if(checkIsLock())
		{
			return;
		}
		menuList[MENU_INDEX_OUTPUT_FIT].currentItem = 1;
		menuEnterPage(MENU_INDEX_OUTPUT_FIT);
		break;
	case MENU_KEY_BRT:
		menuList[MENU_INDEX_PICTURE].currentItem = 1;
		menuEnterPage(MENU_INDEX_PICTURE);
		break;
	
	case MENU_KEY_LAYOUT:
		if(!getValue(pipModeValue))
		{
			setValue(pipModeValue,1);
		}
//		SetPipLayout(LAYOUT_PIP_LT);
		menuList[MENU_INDEX_PIP].currentItem = 2;
		menuEnterPage(MENU_INDEX_PIP);
		break;

	

	case MENU_KEY_SCALE_TAKE:
		if(checkIsLock())
		{
			return;
		}		
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_SCALE_TAKE, 0);
		break;
		
	case MENU_KEY_BLACK_KEY_DEFAULT:
		if(checkIsLock())
		{
			return;
		}		
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_BLACK_KEY_DEFAULT, 0);
		break;	
	case MENU_KEY_HDMI1:
		if(CurrentIsEnterNumberFlag == 1)
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		else
			UpdateInputSource(_HDMI14_INPUT);
	break;
	case MENU_KEY_HDMI2:
		if(CurrentIsEnterNumberFlag == 1)
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		else
			UpdateInputSource(_HDMI20_INPUT);
	break;
	case MENU_KEY_DVI:
		if(CurrentIsEnterNumberFlag == 1)
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		else
			UpdateInputSource(_DVI_INPUT);
	break;
	case MENU_KEY_VGA:
		if(CurrentIsEnterNumberFlag == 1)
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		else
			UpdateInputSource(_VGA_INPUT);
	break;
	case MENU_KEY_DP:
		if(CurrentIsEnterNumberFlag == 1)
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		else
			UpdateInputSource(_DP_INPUT);
	break;
	case MENU_KEY_FREEZE:
		if(CurrentIsEnterNumberFlag == 1)
		{
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		}
		else
		{
			SetFreeze();
		}
	break;
	case MENU_KEY_SPLIT:
		if(CurrentIsEnterNumberFlag == 1)
		{
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		}
		else
		{
			SetKeyLedMode(KEY_SPLIT,LED_ON_ALWAYS);
			menuEnterPage(MENU_INDEX_FOUR_PORT_SPLIT);
		}
		
	break;
	case MENU_KEY_TAKE:
		if(CurrentIsEnterNumberFlag == 1)
		{
			pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
		}
		else
		{
			if((GetPgmCurrentDisplayMode() == _OSD_DM_2P_LR) || (GetPgmCurrentDisplayMode() == _OSD_DM_2P_TB))
			{
				SetKeyLedMode(KEY_TAKE,LED_ON_ALWAYS);
				disableDispReflrshInputFormat();
				TakeTask(GetSystemParam()->SysFadeTime);
				takeStatus = !takeStatus;
				enableDispReflrshInputFormat();
				SetKeyLedMode(KEY_TAKE,LED_OFF_ALWAYS);
			}
			else// if(GetPgmCurrentDisplayMode() == _OSD_DM_2P_PIP)
			{
				menuEnterPage(MENU_INDEX_PIP);
			}
		}
		
	break;
	case MENU_KEY_M1:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		  )
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 1;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_M2:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 2;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_M3:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 3;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_M4:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 4;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_E:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 5;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_F:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 6;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_G:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 7;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_H:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 8;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
	break;
	case MENU_KEY_DVI1:
		menuEnterPage(MENU_INDEX_OUTPUT0_SET);
	break;
	case MENU_KEY_DVI2:
		menuEnterPage(MENU_INDEX_OUTPUT1_SET);
	break;
	case MENU_KEY_DVI3:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 9;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
		else
		{
			menuEnterPage(MENU_INDEX_OUTPUT2_SET);
		}
	break;
	case MENU_KEY_DVI4:
		if( 
			menuIsCurrentPageIndex(MENU_INDEX_4PLT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PLB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_4PRB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_SYSTEM) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_H_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_V_1_SUB_2) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_INDEPENDENT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_VERTICAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_2_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_4_SPLIT) == TRUE ||\
			menuIsCurrentPageIndex(MENU_INDEX_HORIZONTAL1_3_SPLIT) == TRUE
		)
		{
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				if(CurrentIsEnterNumberFlag == 1)
					pCurrentMenuPage->menuPage[item].pfProcess(type, 0);
			}
			SnPassword = SnPassword*10 + 0;
			DebugMessageSystem("SnPassword = %d",SnPassword);
			if(SnPassword == SN_PASSWORD)
			{
				SnPassword = 0;
				menuEnterPage(MENU_INDEX_WRITE_SN);
			}
		}
		else
		{
			menuEnterPage(MENU_INDEX_OUTPUT3_SET);
		}
	break;
    default:		
    break;
    }
}

void MenuPageLockPanelProcess(int type, int value)
{
    const MenuPage *menuPage;
    int item = 1;
	

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
    switch(type)
    {
		case MENU_KEY_MENU:
		case MENU_KEY_SCALE:
			if(menuIsCurrentPageIndex(MENU_INDEX_OUTPUT_FIT) == TRUE)
			{
				setTotaryDelay(100);
				//menuList[MENU_INDEX_STATUS].currentItem = MENU_START_ITEM;
				menuEnterStatusPage();
				//clearMenuPageStack();
			}
			else
			{
				defaultMenuPageProcess(type,value);
			}
			break;
    
		case MENU_EVENT_ENTER:
			if(menuIsCurrentPageIndex(MENU_INDEX_LOCK_PANEL) == TRUE)
			{
				
				pCurrentMenuPage->menuTtemSize = 2;
			}
			defaultMenuPageProcess(type,value);
			break;
		case MENU_KEY_UP:				
			if(menuState == MENU_STATE_CHANGE_ITEM)
			{
				//printf("MENU_KEY_UP_menuState == MENU_STATE_CHANGE_ITEM\r\n");
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_NEXT_ITEM, 0);
			}
			else
			{
				//printf("MENU_KEY_UP_menuState != MENU_STATE_CHANGE_ITEM\r\n");
				if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
				{
					pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_NEXT_VALUE, 0);
				}
			}
			break;
		case MENU_KEY_DOWN:
			if(menuState == MENU_STATE_CHANGE_ITEM)
			{
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_PREV_ITEM, 0);
			}
			else
			{
				if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
				{
					pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_PREV_VALUE, 0);
				}
			}

			break;
		case MENU_KEY_NEXT:
	
			if(menuPage[item].type == MENU_TYPE_DIR)
			{
				pushMenuPage(menuPage);
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);
				menuPage = (MenuPage *)menuPage[item].value;
				readFansControlEnable = 1;
				pCurrentMenuPage = findMenuPage(menuPage);
			
				//if(menuIsCurrentPageIndex(MENU_INDEX_LOCK_PANEL))
				//	pCurrentMenuPage->currentItem = 0;
				//else
				pCurrentMenuPage->currentItem = MENU_START_ITEM;
						
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
				break;
			}
			else if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
			{
				//printf("menuState = ");
				switch(menuState)
				{
					case MENU_STATE_CHANGE_ITEM:
						//printf("MENU_STATE_CHANGE_ITEM\r\n");
						menuState = MENU_STATE_SELECT_ITEM;
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SELECT_ITEM, 0);
						break;
					case MENU_STATE_SELECT_ITEM:
						//printf("MENU_STATE_SELECT_ITEM\r\n");
						menuState = MENU_STATE_CHANGE_ITEM;
						if(menuIsCurrentPageIndex(MENU_INDEX_OUTPUT_FIT) == TRUE)
						{
							menuState = MENU_STATE_CHANGE_AND_SET_VALUE;
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_KEY_NEXT, 0);							
						}
						else
						{
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
						}
						
						break;
					case MENU_STATE_CHANGE_VALUE:
						//printf("MENU_STATE_CHANGE_VALUE\r\n");
						menuState = MENU_STATE_SET_VALUE;
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
						break;
					case MENU_STATE_CHANGE_AND_SET_VALUE:
						//printf("MENU_STATE_CHANGE_AND_SET_VALUE\r\n");
						menuState = MENU_STATE_CHANGE_ITEM;
						//pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
						if(menuIsCurrentPageIndex(MENU_INDEX_LOCK_PANEL) == TRUE && getValue(pCurrentMenuPage->menuPage[item].value) == 1)
						{
							pushMenuPage(menuPage);
							pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);
							menuPage = (MenuPage *)menuPage[item + 1].value;
							readFansControlEnable = 1;
							pCurrentMenuPage = findMenuPage(menuPage);
						
							//if(menuIsCurrentPageIndex(MENU_INDEX_LOCK_PANEL))
							//	pCurrentMenuPage->currentItem = 0;
							//else
							pCurrentMenuPage->currentItem = MENU_START_ITEM;
									
							pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
							break;							
						}
						else if(menuIsCurrentPageIndex(MENU_INDEX_OUTPUT_FIT) == TRUE)
						{
							pushMenuPage(menuPage);
							pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);						
							if(getValue(pCurrentMenuPage->menuPage[item].value) == 1)
							{
								menuPage = (MenuPage *)menuPage[item + 3].value;						
							}
							else
							{
								menuPage = (MenuPage *)menuPage[item + 2].value;
							}
							
							readFansControlEnable = 1;
							pCurrentMenuPage = findMenuPage(menuPage);
						
							//if(menuIsCurrentPageIndex(MENU_INDEX_LOCK_PANEL))
							//	pCurrentMenuPage->currentItem = 0;
							//else
							pCurrentMenuPage->currentItem = MENU_START_ITEM;
									
							pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);							}
						else
						{
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_ENTER, 0);
						}
						break;
					case MENU_STATE_SET_VALUE:
						//printf("MENU_STATE_SET_VALUE\r\n");
						menuState = MENU_STATE_CHANGE_ITEM;
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
						break;
					}
				}
			
			break;
		
    	default:
			defaultMenuPageProcess(type,value);
			break;
    }
}


void defaultMenuPageProcess(int type, int value)
{
    char buffer[40];
    const MenuPage *menuPage;
    int item;
//   int i=0;
//    u8 curport;	
	BYTE fb = 0;
//	int portxItemSize,portx;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

	//printf("defaultMenuPageProcess(),item:%d,type:%d,menuState:%d\r\n",item,type,menuState);
    switch(type)
    {
		case MENU_EVENT_ENTER:
	
		if(menuIsCurrentPageIndex(MENU_INDEX_USER_DEFINED_BUTTON) == TRUE)
		{
			if(pCurrentMenuPage->currentItem <= 2)
			{
				pCurrentMenuPage->currentItem = 2;
			}
		}
        if(!menuIsCurrentPageIndex(MENU_INDEX_TRANSITION))
            menuState = MENU_STATE_CHANGE_ITEM;

        if(menuPage[0].type == MENU_TYPE_LABEL)
        {
            //sprintf(buffer, "  %s", langGetStr(menuPage[0].str));
            //LCD_WriteLine1(buffer);
        }
        else
        {
            //LCD_WriteLine1("NO SUPPORT");
        }
        item = item-1;
        item = item / MENU_DISPLAY_LINE_NUM;
        item = item * MENU_DISPLAY_LINE_NUM+1;
        {
            int size = item+MENU_DISPLAY_LINE_NUM;
            int clearLine;
            if(size > (pCurrentMenuPage->menuTtemSize))
            {
                size = (pCurrentMenuPage->menuTtemSize);
            }
            clearLine = (size - item);
            if(clearLine<MENU_DISPLAY_LINE_NUM)
            {
                int i;
                for(i=MENU_DISPLAY_LINE_NUM-1; i>=clearLine; i--)
                {
                    LCD_WriteLine(i,"");
                }
            }
            for(; item < size; item++)
            {            	
				if((menuPage[item].type == MENU_TYPE_VALUE) || (menuPage[item].type == MENU_TYPE_READ)
					||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
				{                 
                	menuValueList[item] = getValue(menuPage[item].value);
				}
                menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
				#if 1
				if(menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) || menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB))
				{
					if(item == pCurrentMenuPage->currentItem)
					{
						//if(menuPage[item].value != pipSourceValue)
						setValue(menuPage[item].value, menuValueList[item]);
					}
				}
				#endif
            }
        }
    break;
    case MENU_EVENT_CHANGE_ITEM:
    case MENU_EVENT_CHANGE_VALUE:
    case MENU_EVENT_SELECT_ITEM:
        menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
    break;

    case MENU_EVENT_SET_VALUE:
		if(menuIsCurrentPageIndex(MENU_INDEX_FIELD_SPLIT))
		{
			if(menuPage[item].value == FieldSplitParamSendValue)
			{
				setValue(menuPage[item].value,0);
	        	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, item);
			}
			else
			{
				setValue(menuPage[item].value, menuValueList[item]);
	        	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, item);
			}
		}
		else
		{
	        setValue(menuPage[item].value, menuValueList[item]);
	        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, item);
		}
        break;
    case MENU_EVENT_NEXT_ITEM:
		//printf("MENU_EVENT_NEXT_ITEM_item_%d,itemsize_%d\r\n",item,pCurrentMenuPage->menuTtemSize);
        item++;
	
        if(item < 1)
        {
            item = pCurrentMenuPage->menuTtemSize-1;
        }
        if(item > (pCurrentMenuPage->menuTtemSize-1))
        {
        	#if(ENABLE_MENU_CLOSE_LOOP == 0)
            item = pCurrentMenuPage->menuTtemSize-1; //到最后一项就不再动
            #else
			if(menuIsCurrentPageIndex(MENU_INDEX_USER_DEFINED_BUTTON) == TRUE)
			{
				item = 2;        //在最后一项再按下回到第一项
			}
			else
			{
				item = 1;        //在最后一项再按下回到第一项
			}
            
            #endif
        }
        pCurrentMenuPage->currentItem = item;
		if(
			((pCurrentMenuPage->currentItem > 4)&&(menuIsCurrentPageIndex(MENU_INDEX_SCREEN)))
		  ||((pCurrentMenuPage->currentItem > 5)&&(menuIsCurrentPageIndex(MENU_INDEX_SCALE)))
			//((pCurrentMenuPage->currentItem > 4)&&(menuIsCurrentPageIndex(MENU_INDEX_SCREEN)||menuIsCurrentPageIndex(MENU_INDEX_SCALE)))
		  ||((pCurrentMenuPage->currentItem > 6)&&((menuIsCurrentPageIndex(MENU_INDEX_SCREEN_HOTKEY))||(menuIsCurrentPageIndex(MENU_INDEX_IMAGE_QUALITY))))
		  ||((pCurrentMenuPage->currentItem > 7)&&(menuIsCurrentPageIndex(MENU_INDEX_ZOOM_NEW)||menuIsCurrentPageIndex(MENU_INDEX_SPLIT_MODE))))
		{
			CurrentIsEnterNumberFlag = 0;
		}
		else if(menuIsCurrentPageIndex(MENU_INDEX_TEMPERATURE) == TRUE)
		{
			if((pCurrentMenuPage->currentItem) > 1)
			{
				pCurrentMenuPage->currentItem = 1;
			}
		}

        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
        break;
    case MENU_EVENT_PREV_ITEM:
		//printf("MENU_EVENT_PREV_ITEM_item_%d,itemsize_%d\r\n",item,pCurrentMenuPage->menuTtemSize);
        item--;       
        if(item < 1)
        {
			#if(ENABLE_MENU_CLOSE_LOOP == 0)
            item = 1;
			#else
            item = (pCurrentMenuPage->menuTtemSize-1); //在第一项再按向上回到最后一项
			#endif
        }
        if(item > (pCurrentMenuPage->menuTtemSize-1))
        {
        	
            item = 1;   
        }
        pCurrentMenuPage->currentItem = item;
		if((pCurrentMenuPage->currentItem == 1)&&((CurrentIsSplitPage())||(menuIsCurrentPageIndex(MENU_INDEX_SCREEN_HOTKEY))))
		{
			CurrentIsEnterNumberFlag = 0;
		}
		else if(menuIsCurrentPageIndex(MENU_INDEX_TEMPERATURE) == TRUE)
		{
			if((pCurrentMenuPage->currentItem) > 1)
			{
				pCurrentMenuPage->currentItem = 1;
			}
		}	       
       
        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
        break;
    case MENU_EVENT_NEXT_VALUE:
        if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
        {
            int index;
            index = menuPage[item].value;
            
            menuState = MENU_STATE_CHANGE_AND_SET_VALUE;
            menuValueList[item] = nextValue(index,getValue(index));
			if(getValue(index) != menuValueList[item])
			{
				//setValue(index, menuValueList[item]);
				#if 1
				if(!menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) && !menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB))
				{
					setValue(index, menuValueList[item]);
				}
				else
				{
					SetScalerPosSize(index,menuValueList[item]);
					//SetPipWinSize(index, menuValueList[item]);
				}
				#endif
				if(!menuIsCurrentPageIndex(MENU_INDEX_STATUS))
				{
		            menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
				}
			}
        }
        break;
    case MENU_EVENT_PREV_VALUE:
		//printf("defaultMenuPageProcess_MENU_EVENT_PREV_VALUE\r\n");
        if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE)||(menuPage[item].type == MENU_TYPE_VALUE_ICON))
        {
            int index;
            index = menuPage[item].value;
             
            menuState = MENU_STATE_CHANGE_AND_SET_VALUE;
            menuValueList[item] = prevValue(index,getValue(index));
 			if(getValue(index) != menuValueList[item])
			{
				//setValue(index, menuValueList[item]);
				#if 1
				if(!menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEA) && !menuIsCurrentPageIndex(MENU_INDEX_PIPIMAGEB))
				{
					setValue(index, menuValueList[item]);
				}
				else
				{
					SetScalerPosSize(index,menuValueList[item]);
					//SetPipWinSize(index, menuValueList[item]);
				}
				#endif
	            menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
			}
        }
        break;

    case MENU_EVENT_DRAW_ITEM:
        if(menuPage[value].type == MENU_TYPE_LABEL)
        {
        	if(strlen(langGetStr(menuPage[value].str)) > 18)
        	{
        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
        	}
			else
			{
            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
			}
        }
        else if(menuPage[value].type == MENU_TYPE_DIR)
        {
            if(value == item)
            {
				fb = 1;
                sprintf(buffer, "%s%-22s%s", MENU_SELECT_ITEM,langGetStr(menuPage[value].str), MENU_DIR_MARK);
            }
            else
            {
				fb = 0;
                sprintf(buffer, "%s%-22s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str), MENU_DIR_MARK);
            }
			//dispMenuPosBar(item,pCurrentMenuPage->menuTtemSize);
        }
   		 else if((menuPage[value].type == MENU_TYPE_VALUE)||(menuPage[value].type == MENU_TYPE_ENTER_NUMBER_VALUE))
    	{       	
            if(strlen(valueToString(menuPage[value].value,menuValueList[value])) == 10)
        	{
				//printf("strlen == 10\r\n");
        		if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-14s%10s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-14s%10s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
        	}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 14)
			{
				//printf("strlen > 14\r\n");
				if((DEVICE_TYPE == 0x1f00)&&(menuIsCurrentPageIndex(MENU_INDEX_TRANSITION)) )
				{
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-8s%20s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-8s%20s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else
				{
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%s %s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%s %s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
			}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 8)
        	{
				//printf("strlen > 8\r\n");
        		if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-12s%12s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-12s%12s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
        	}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 6)
			{
				//printf("strlen > 6\r\n");
	            if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-14s%8s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-14s%8s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
			}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) < 4)
			{
				//printf("strlen < 4\r\n");
	            if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-18s%4s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-18s%4s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
			}
			else
			{
				//printf("strlen else\r\n");
				if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-18s%6s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-18s%6s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
			}
		
        }
		else if(menuPage[value].type == MENU_TYPE_READ)
		{
			if(menuPage[value].str)
        	{        
        		//printf("strlen(valueToString(menuPage[value].value,menuValueList[value])) = %d\r\n",strlen(valueToString(menuPage[value].value,menuValueList[value])) );
				if(strlen(valueToString(menuPage[value].value,menuValueList[value])) >= 14)
	        	{
					//printf("strlen > 14\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-10s%14s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-10s%14s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) >= 8)
	        	{
					//printf("strlen > 8\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-12s%12s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-12s%12s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) >= 6)
				{
					//printf("strlen > 6\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-14s%8s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-14s%8s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) <= 4)
				{
					//printf("strlen < 4\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-18s%4s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-18s%4s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else
				{
					//printf("strlen: %d\r\n",strlen(valueToString(menuPage[value].value,menuValueList[value])));
					if(value == item)
					{
						fb = 1;
						sprintf(buffer, "%s%-18s%5s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));				  
					}
					else
					{
						fb = 0;
						sprintf(buffer, "%s%-18s%5s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
					}
				}
        	}
			else
			{
				//printf("strlen: %d\r\n",(strlen(valueToString(menuPage[value].value,menuValueList[value])) < 4));
				if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%s", menuStateStr[menuState], valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%s", MENU_NO_SELECT, valueToString(menuPage[value].value,menuValueList[value]));
	            }
			}
		}
        else if(menuPage[value].type == MENU_TYPE_CUSTOMNUM)
        {
            if(strlen(valueToString(menuPage[value].value,menuValueList[value])) == 10)
        	{
				//printf("strlen == 10\r\n");
        		if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-14s%10s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-14s%10s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
        	}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 14)
			{
				//printf("strlen > 14\r\n");
				if((DEVICE_TYPE == 0x1f00)&&(menuIsCurrentPageIndex(MENU_INDEX_TRANSITION)) )
				{
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-8s%20s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-8s%20s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else
				{
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%s %s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%s %s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
			}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 8)
        	{
				//printf("strlen > 8\r\n");
        		if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-12s%12s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-12s%12s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
        	}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 6)
			{
				//printf("strlen > 6\r\n");
	            if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-14s%8s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-14s%8s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
			}
			else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) < 4)
			{
				//printf("strlen < 4\r\n");
	            if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-18s%4s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-18s%4s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
			}
			else
			{
				//printf("strlen else\r\n");
				if(value == item)
	            {
					fb = 1;
	            	sprintf(buffer, "%s%-18s%6s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-18s%6s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
	            }
			}
        }
		else
		{
			sprintf( buffer, "NO SUPPORT");
		}
		if(	fb )
		{
			LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
		}
		else
		{
			LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
		}
		//if((!currentIsGuidePage())&&(!CurrentIsYesOrNoPage())&&(!CurrentIsSelectInputPage())&&(!CurrentIsUpgradePage())&&(!CurrentIsOutputFormatPage())&&(!CurrentIsSaveOrLoadPage()))
		if((!CurrentIsYesOrNoPage())&&(!CurrentIsSelectInputPage())&&(!CurrentIsUpgradePage())&&(!CurrentIsSaveOrLoadPage()))
		{
			if(pCurrentMenuPage->menuTtemSize >= 2)
			{
				dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1));
				//menuDrawItemBar();
			}
		}
	
        break;
		
	case MENU_EVENT_INPUT_SOURCE:

		menuProcessEvent(MENU_KEY_INFO);
		break;
		
	case MENU_EVENT_ENTER_PAGE:
		menuEnterPage(value);
		break;
	case MENU_EVENT_STEP_1:
		setValue(stepValue, 1);
		break;
	case MENU_EVENT_STEP_10:
		setValue(stepValue, 10);
		//setValue(stepValue, 5);
		break;
	case MENU_EVENT_STEP_100:
		setValue(stepValue, 100);
		//setValue(stepValue, 10);
		break;
	case MENU_EVENT_UPDATE_FIRMWARE_START:
		menuEnterPage(MENU_INDEX_UPDATE_FIRMWARE);
		break;
        
    case  MENU_EVENT_SEND_TO_PORT:
        switch(value)
        {
            case MENU_EVENT_SEND_TO_PORT1:
                pCurrentMenuPage->currentItem = 5;
                break;
            case MENU_EVENT_SEND_TO_PORT2:
                pCurrentMenuPage->currentItem = 10;
                break;
            case MENU_EVENT_SEND_TO_PORT3:
                pCurrentMenuPage->currentItem = 15;
                break;
            case MENU_EVENT_SEND_TO_PORT4:
                pCurrentMenuPage->currentItem = 20;
                break;
            default:
                break;
        }
        menuProcessEvent(MENU_KEY_NEXT);
        break;
	case MENU_EVENT_UPDATE_FIRMWARE_SKIP:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		dispLineCenter_L(1,langGetStr(noThisModuleStr));	
		dispLineCenter_L(2,langGetStr(upgradeSkipStr));	
		break;
	case MENU_EVENT_UPDATE_FIRMWARE_SUCCESS:
		setProcessKeyEnable(0);
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();

		dispLineCenter_L(1,langGetStr(updateFirmwareSuccessStr));	
		dispLineCenter_L(2,langGetStr(pleaseRestartStr));	

		break;
	case MENU_EVENT_UPDATE_FIRMWARE_WAIT_AUTO_REBOOT:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "  %s", langGetStr(updateFirmwareStr));
		LCD_WriteLine1(buffer);
		sprintf(buffer, "%s", langGetStr(pleaseDoNotShautDownStr));
		LCD_WriteLine2(buffer);
		sprintf(buffer, "%s", langGetStr(pleaseWaitAutoRebootStr));
		LCD_WriteLine3(buffer);
		break;
	case MENU_EVENT_UPDATE_FIRMWARE_FAIL:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();

		dispLineCenter_L(1,langGetStr(updateFirmwareFailStr));	
		dispLineCenter_L(2,langGetStr(pleaseTryAgainStr));	

		break;
	case MENU_EVENT_STUDY_EDID_SUCCESS:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "  %s", langGetStr(studyEDIDSuccessStr));
		LCD_WriteLine2(buffer);
		break;
	case MENU_EVENT_STUDY_EDID_FAIL:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "  %s", langGetStr(studyEDIDFailStr));
		LCD_WriteLine1(buffer);
		break;

	case MENU_EVENT_LOAD_SAVE:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "  %s", langGetStr(loadFromStr));
		LCD_WriteLine1(buffer);
		sprintf(buffer, "%s%s", MENU_SELECT_ITEM, valueToString(loadFromValue, value));
		LCD_WriteLine2(buffer);	
		
		menuSetBusy(1,5);
		setValue(loadFromValue, value);
		menuDisableBusy();
		
		LCD_WriteStr(1, 5, langGetStr(finishedStr));	
		
		break;

	case MENU_EVENT_RELOAD_SAVE1:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "  %s", langGetStr(loadFromStr));
		LCD_WriteLine1(buffer);
		sprintf(buffer, "%s%s", MENU_SELECT_ITEM, valueToString(loadFromValue, value));
		LCD_WriteLine2(buffer);	
		
		menuSetBusy(1,5);
		loadUserSetting(USER_SAVE_1);
		menuDisableBusy();
		
		LCD_WriteStr(1, 5, langGetStr(finishedStr));
		menuProcessEvent(MENU_KEY_INFO);
		break;
	case MENU_EVENT_WAIT_CALL_DELAY:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "  %s", langGetStr(waitCallDelayStr));
		LCD_WriteLine2(buffer);
		break;
	case MENU_EVENT_DISP_AUDIO_INPUT_CONFIG:
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0); //先退出当前菜单<--20151014
		clearMenuPageStack();
		menuList[MENU_INDEX_AUDIO_CONFIG].currentItem = MENU_START_ITEM;
		menuEnterPage(MENU_INDEX_AUDIO_CONFIG);
		break;
	case MENU_EVENT_FREEZE:
    #if (CURRENT_DEVICE == _VIP_SMART) 
            if(checkIsLock())
            {
                return;
            }
            if(getValue(displayModeValue) != 2)     
            {
                char buffer[40];
                setValue(displayModeValue, 2);
                
                menuEnterPage(MENU_INDEX_NOTHING);
                LCD_Clear();            
                if(GetLanguage() == LANG_ENGLISH)
                {
                    sprintf(buffer, "%21s", langGetStr(freezeStr));
                }
                else
                {
                    sprintf(buffer, "%18s", langGetStr(freezeStr));
                }
                
                LCD_WriteLine2(buffer);                     
            }
            else
            {
                char buffer[40];
                setValue(displayModeValue, 1);
                
                menuEnterPage(MENU_INDEX_NOTHING);
                LCD_Clear();
                //sprintf(buffer, "%18s", langGetStr(liveStr));
                if(GetLanguage() == LANG_ENGLISH)
                {
                    sprintf(buffer, "%21s", langGetStr(liveStr));
                }
                else
                {
                    sprintf(buffer, "%20s", langGetStr(liveStr));
                }
                LCD_WriteLine2(buffer);     
                
            }
            SysDelay1ms(500);
            menuProcessEvent(MENU_KEY_INFO);            
    #else    
		LCD_Clear();
		menuEnterPage(MENU_INDEX_NOTHING);
		if(getValue(freezeValue))
		{
			setValue(freezeValue, 0);
			if(GetLanguage() == LANG_ENGLISH)
			{
				sprintf(buffer, "%21s", langGetStr(liveStr));
			}
			else
			{
				sprintf(buffer, "%20s", langGetStr(liveStr));
			}
		}
		else
		{
			setValue(freezeValue, 1);
			if(GetLanguage() == LANG_ENGLISH)
			{
				sprintf(buffer, "%22s", langGetStr(freezeStr));
			}
			else
			{
				sprintf(buffer, "%20s", langGetStr(freezeStr));
			}
		}
		//sprintf(buffer, "  %s", valueToString(freezeValue,getValue(freezeValue)));
		LCD_WriteLine2(buffer);
		LCD_WriteLine3("");
		SysDelay1ms(500);
		menuProcessEvent(MENU_KEY_INFO);
    #endif        
		break;
	case MENU_EVENT_TP:
		if(getValue(displayModeValue) != 4)		
		{
			{
				//char buffer[40];
				LCD_Clear();
				//sprintf(buffer, "%s", langGetStr(operationReadyTipStr));
				//LCD_WriteLine2(buffer);
				dispLineCenter_L(1,langGetStr(operationReadyTipStr));	
			}
			setValue(displayModeValue, 4);
			menuList[MENU_INDEX_TEST_PATTERN].currentItem = 1;
			menuEnterPage(MENU_INDEX_TEST_PATTERN);
		}
		else
		{
			{
				char buffer[40],len;
				LCD_Clear();
				len = strlen(langGetStr(liveStr));
				len = 16 - (len>>1);//32-((32 - len)>>1);
				if(GetLanguage() == LANG_ENGLISH)
				{
					sprintf(buffer, "%21s", langGetStr(liveStr));
				}
				else
				{
					sprintf(buffer, "%20s", langGetStr(liveStr));
				}
				LCD_WriteLine2(buffer);
			}
			setValue(displayModeValue, 1);
			SysDelay1ms(500);
			menuProcessEvent(MENU_KEY_INFO);
			
		}
		//menuList[MENU_INDEX_TEST_PATTERN].currentItem = 1;
		//menuEnterPage(MENU_INDEX_TEST_PATTERN);
		break;
	case MENU_EVENT_BRIGHTNESS:
		menuList[MENU_INDEX_IMAGE_QUALITY].currentItem = 1;
		menuEnterPage(MENU_INDEX_IMAGE_QUALITY);
        //menuState = MENU_STATE_CHANGE_VALUE;
		break;
	case MENU_EVENT_FS:
		LCD_Clear();
		if(getValue(pipModeValue))
		{
			menuEnterPage(MENU_INDEX_NOTHING);
			dispLineCenter_L(1,langGetStr(invalidOpInPIPModeStr));	
			SysDelay1ms(800);
			menuProcessEvent(MENU_KEY_INFO);
		}
		else if(getValue(splitOnOffValue))
		{
			menuEnterPage(MENU_INDEX_NOTHING);
			dispLineCenter_L(1,langGetStr(invalidOpInSPLITModeStr));	
			SysDelay1ms(800);
			menuProcessEvent(MENU_KEY_INFO);
		}
		else
		{

			LCD_Clear();
			if(getValue(screenModeValue))
			{
				sprintf(buffer, "  %s", langGetStr(FullSizeStr));
				dispLineCenter_L(1,buffer);
				setValue(screenModeValue, 0);
			}
			else
			{
				sprintf(buffer, "  %s", langGetStr(ScreenSizeStr));
				dispLineCenter_L(1,buffer);
				setValue(screenModeValue, 1);
			}
			menuEnterPage(MENU_INDEX_SCREEN_HOTKEY);
		}				
		break;
	case MENU_EVENT_PIP:
		if(!getValue(pipModeValue))
		{
			setValue(pipModeValue,1);
		}
		menuList[MENU_INDEX_PIP].currentItem = MENU_START_ITEM;
		menuEnterPage(MENU_INDEX_PIP);
		break;
	case MENU_EVENT_PBP:
		if(!getValue(pipModeValue))
		{
			setValue(pipModeValue,1);
		}
		menuList[MENU_INDEX_PIP].currentItem = MENU_START_ITEM;
		menuEnterPage(MENU_INDEX_PIP);
		break;

	case MENU_EVENT_AB:
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		if(getValue(currentWindowValue))
		{
			setValue(currentWindowValue, 0);
		}
		else
		{
			setValue(currentWindowValue, 1);
		}
		sprintf(buffer, "%s:%s", langGetStr(selectStr), valueToString(currentWindowValue,getValue(currentWindowValue)));
		dispLineCenter_L(1,buffer);	
		dispLineCenter_L(2,langGetStr(SplitInvalidInPIPModeStr));	
			
		if(getValue(pipModeValue))
		{
			SysDelay1ms(1500);
			menuProcessEvent(MENU_KEY_INFO);//menuEnterPage(MENU_INDEX_PIP);
		}
		break;

	case MENU_EVENT_LANG:
	#if (CURRENT_DEVICE == _VIP_SMART)
		setValue(languageValue, 0);
	#else
		if(getValue(languageValue))
		{
			setValue(languageValue, 0);
		}
		else
		{
			setValue(languageValue, 1);
		}
	#endif
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "%15s", valueToString(languageLongValue,getValue(languageValue)));
		LCD_WriteLine2(buffer);
		LCD_WriteLine3("");
		
		SysDelay1ms(1000);
		menuProcessEvent(MENU_KEY_INFO);//menuEnterPage(MENU_INDEX_PIP);
		break;
	case MENU_EVENT_VGA_AUTO_ADJUST:
		menuState = MENU_STATE_CHANGE_ITEM;
		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_VGA_ADJUST);		
		SysDelay1ms(200);
		resetMenuPageProcess(MENU_EVENT_SELECT_ITEM,1);
		break;	
	case MENU_EVENT_EXT7_VGA_AUTO_ADJUST:
		break;	
	case MENU_EVENT_EXT8_VGA_AUTO_ADJUST:
		break;	
	case MENU_EVENT_SCALE_TAKE:
		if(getValue(scaleKeyTypeValue))
		{
			setValue(scaleKeyTypeValue, 0);
		}
		else
		{
			setValue(scaleKeyTypeValue, 1);
		}
		menuEnterPage(MENU_INDEX_NOTHING);
		LCD_Clear();
		sprintf(buffer, "%15s", valueToString(scaleKeyTypeValue,getValue(scaleKeyTypeValue)));
		LCD_WriteLine2(buffer);
		if(getValue(scaleKeyTypeValue))
		{
			dispLineCenter_L(2,langGetStr(invalidInPIPOrSplitModeStr)); 	
		}
		else
		{
			LCD_WriteLine3(""); 	
		}
		SysDelay1ms(1000);
		menuProcessEvent(MENU_KEY_INFO);//menuEnterPage(MENU_INDEX_PIP);
		break;
		
	case MENU_EVENT_BLACK_KEY_DEFAULT:				
		menuEnterPage(MENU_INDEX_USER_DEFINED_BUTTON);
		break;
	case MENU_KEY_MENU:
		if(menuIsCurrentPageIndex(MENU_INDEX_MAIN))
		{
			//printf("defaultMenuPageProcess_MENU_KEY_MENU_current main\r\n");
			menuEnterStatusPage();
			return;
				
		}
    default:
        menuKeyProcess(type, value);
        break;
    }
}

void MenuPage_Advanced_Process(int type, int value)
{
	int item;
	char buffer[40];
	const MenuPage *menuPage;

	menuPage = pCurrentMenuPage->menuPage;
	item = pCurrentMenuPage->currentItem;
	//printf("systemMenuPageProcess_item = %d\r\n",item);
	switch(type)
	{

		case MENU_EVENT_PREV_ITEM:
            if(item <= 1)   // 20160803 for add version in advanced menu
			{
				item = (pCurrentMenuPage->menuTtemSize-1); //在第一项再按向上回到最后一项
			}
			else
			{
				item--;
			}
			pCurrentMenuPage->currentItem = item;
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
		break;
			
		case MENU_EVENT_ENTER:
			if(menuIsCurrentPageIndex(MENU_INDEX_ADVANCED) == TRUE)
			{
			#if (SUPPORT_ADVANCED_SN_OPTION == 1)
				if(pCurrentMenuPage->currentItem <= 2)
				{
					pCurrentMenuPage->currentItem = 2;
				}
			#endif
			}
			defaultMenuPageProcess(type, value);
		break;

		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_READ)
			{
				sprintf(buffer, "  %-20s%8s", langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));				 
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				break;
			}
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
	
}

void systemMenuPageProcess(int type, int value)
{
	int item;
	char buffer[40];
    const MenuPage *menuPage;

    menuPage = pCurrentMenuPage->menuPage;
	item = pCurrentMenuPage->currentItem;
	//printf("systemMenuPageProcess_item = %d\r\n",item);
	switch(type)
	{
		case MENU_EVENT_PREV_ITEM:
			if(item <= 2)
			{
				item = (pCurrentMenuPage->menuTtemSize-1); //在第一项再按向上回到最后一项
			}
			else
			{
				item--;
			}
			pCurrentMenuPage->currentItem = item;
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
		break;
			
		case MENU_EVENT_ENTER:
			if(pCurrentMenuPage->currentItem <= 2)
			{
				pCurrentMenuPage->currentItem = 2;
			}
			defaultMenuPageProcess(type, value);
			break;

		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_READ)
			{
            	sprintf(buffer, "%-20s%8s", langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				break;
			}
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
	
}

void zoomMenuPageProcess(int type, int value)
{
    const MenuPage *menuPage = pCurrentMenuPage->menuPage;
    int item = pCurrentMenuPage->currentItem;;

	menuPage = menuPage;
	item = item;
	
	switch(type)
	{
	case MENU_KEY_SCALE:
		menuEnterStatusPage();
		break;
	case MENU_KEY_HSIZE:
		if(item != 1)
		{
			item = 1;
		}
		else
		{
			item = 3;
		}
		pCurrentMenuPage->currentItem = item;
		menuPage->pfProcess(MENU_EVENT_ENTER, 0);
		break;
	case MENU_KEY_VSIZE:
		if(item != 2)
		{
			item = 2;
		}
		else
		{
			item = 4;
		}
		pCurrentMenuPage->currentItem = item;
		menuPage->pfProcess(MENU_EVENT_ENTER, 0);
		break;
	default:
		#ifdef ENTER_NUMBER_KEY
		enterNumberPageProcess(type,value);
		#else
		defaultMenuPageProcess(type, value);
		#endif
		break;
	}
}




void enterNumberPageTask(void)
{
	const MenuPage *menuPage;
	int item = 1;

	item = pCurrentMenuPage->currentItem;
	menuPage = pCurrentMenuPage->menuPage;
	
	if(menuPage[item].type != MENU_TYPE_ENTER_NUMBER_VALUE)
	{
		menuRemoveMainFunc(enterNumberPageTask);
		return;
	}
	if(menuValueList[0])
	{
		menuValueList[0] = 0;
	}
	else
	{
		menuValueList[0] = 1;
	}
	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, pCurrentMenuPage->currentItem);
}

#define ENTER_NUMBER_SIZE (4)

int getSubNumber(int value, int sub)
{
	if((sub < 0) || (sub > ENTER_NUMBER_SIZE))
	{
		return 0;
	}
	if(sub && value)
	{
		return getSubNumber(value/10, sub-1);
	}
	else
	{
		return value%10;
	}
}

int setSubNumber(int value, int sub, int number)
{
	int currentNumber = getSubNumber(value,sub);
	if((sub < 0) || (sub > ENTER_NUMBER_SIZE) || (number < -9) || (number > 9))
	{
		return value;
	}
	currentNumber = number-currentNumber;
	while(sub)
	{
		currentNumber *= 10;
		sub--;
	}
	return value+currentNumber;	
}

#ifdef ENTER_NUMBER_KEY

int changeToKeyNumber(int type)
{
	#if 1
	if(menuState == MENU_STATE_CHANGE_ITEM)  //增加这个判断,只在用户按下确认键,由'->'变成'*'之后才允许输入数字
	{
		CurrentIsEnterNumberFlag = 0;
		//printf("MENU_STATE_CHANGE_ITEM,not number keys\r\n");
		return type;
	}
	#endif
#if defined(LING_XIN)
	switch(type)
	{
		case MENU_KEY_HDMI1:
		type = MENU_KEY_1;
		break;
		case MENU_KEY_HDMI2:
		type = MENU_KEY_2;
		break;
		case MENU_KEY_DVI:
		type = MENU_KEY_3;
		break;
		case MENU_KEY_M1:
		type = MENU_KEY_4;
		break;
		case MENU_KEY_M2:
		type = MENU_KEY_5;
		break;
		case MENU_KEY_FREEZE:
		type = MENU_KEY_6;
		break;
		case MENU_KEY_TAKE:
		type = MENU_KEY_7;
		break;
		case MENU_KEY_M4:
		type = MENU_KEY_8;
		break;
		case MENU_KEY_M3:
		type = MENU_KEY_9;
		break;
		case MENU_KEY_SPLIT:
		type = MENU_KEY_0;
		break;
	}
#else
	switch(type)
		{
			case MENU_KEY_M1:
			//case MENU_KEY_AB:
				type = MENU_KEY_1;
				break;
			case MENU_KEY_M2:
				type = MENU_KEY_2;
				break;
			case MENU_KEY_M3:
				type = MENU_KEY_3;
				break;
			case MENU_KEY_M4:
				type = MENU_KEY_4;
				break;
			case MENU_KEY_E:
				type = MENU_KEY_5;
				break;
			case MENU_KEY_F:
				type = MENU_KEY_6;
				break;
			case MENU_KEY_G:
				type = MENU_KEY_7;
				break;
			case MENU_KEY_H:
				type = MENU_KEY_8;
				break;
			case MENU_KEY_DVI3:
				type = MENU_KEY_9;
				break;
			case MENU_KEY_DVI4:
				type = MENU_KEY_0;
				break;
		}
#endif
	return type;
}


void customEDIDPageProcess(int type, int value);

void enterNumberPageProcess(int type, int value)
{
	const MenuPage *menuPage;
	int item = 1;
	item = pCurrentMenuPage->currentItem;
	menuPage = pCurrentMenuPage->menuPage;
	CurrentIsEnterNumberFlag = 1;
	if((menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE) 
		|| (type == MENU_EVENT_DRAW_ITEM))
	{
		type = changeToKeyNumber(type);
		switch(type)
		{
		
			case MENU_KEY_0:
			case MENU_KEY_1:
			case MENU_KEY_2:
			case MENU_KEY_3:
			case MENU_KEY_4:
			case MENU_KEY_5:
			case MENU_KEY_6:
			case MENU_KEY_7:
			case MENU_KEY_8:
			case MENU_KEY_9:
			if((pCurrentMenuPage == &menuList[MENU_INDEX_CUSTOM_FORMAT])||(pCurrentMenuPage == &menuList[MENU_INDEX_CUSTOM_EDID]))
			{
				menuValueList[item] = menuValueList[item]*10+type-MENU_KEY_0;
				if(menuValueList[1] > 3840 || menuValueList[2] > 3840)
					menuValueList[item] = type-MENU_KEY_0;
				if(menuValueList[3] > 60)
					menuValueList[3] = type-MENU_KEY_0;
				customEDIDPageProcess(MENU_EVENT_DRAW_ITEM, item);
			}
			else
			{
				if(menuSubItem == 0)
				{
					menuValueList[item] = 0;
					menuSubItem++;
				}
				menuState = MENU_STATE_CHANGE_VALUE;

				if(menuIsCurrentPageIndex(MENU_INDEX_SPLIT_FIXED))
				{
					if(item < 4)
					{
						if(type != MENU_KEY_0)
						{
							menuValueList[item] = type-MENU_KEY_0;
						}
						else
						{
							menuValueList[item] = 10;
						}
					}
					else if(item == 4)
					{
						char max = getValue(totalWidthValue)*getValue(totalHeighValue);
						if(max < 10)
						{
							if((type == MENU_KEY_0)||((type-MENU_KEY_0) > max ))
							{
								break;
							}
							menuValueList[item] = type-MENU_KEY_0;
						}
						else
						{
							menuValueList[item] = menuValueList[item]*10+type-MENU_KEY_0;
						}						
					}
					else
					{
						menuValueList[item] = menuValueList[item]*10+type-MENU_KEY_0;
					}
				}
				else
				{
					menuValueList[item] = menuValueList[item]*10+type-MENU_KEY_0;
				}
				if(nextValue(menuPage[item].value,menuValueList[item]) <= menuValueList[item])
				{
					menuSubItem = 0;
					defaultMenuPageProcess(MENU_EVENT_SET_VALUE, item);
				}
				defaultMenuPageProcess(MENU_EVENT_DRAW_ITEM, item);
			}
			break;
			
			case MENU_EVENT_ENTER:
				if(pCurrentMenuPage == &menuList[MENU_INDEX_CUSTOM_FORMAT])
				{
					printf("enters number page MENU_EVENT_ENTER");
					dispValue[menuSubItem - 1] = menuValueList[item];
					menuValueList[item] = 0;
					menuSubItem++;
					menuState = MENU_STATE_SELECT_ITEM;
					CurrentIsEnterNumberFlag = 1;
					if(menuSubItem > 3)
					{
						menuSubItem = 0;
						CurrentIsEnterNumberFlag = 0;
						menuState = MENU_STATE_CHANGE_ITEM;
						defaultMenuPageProcess(type,value);
					}
				}
				else
				{
					setTotaryDelay(100);
					if((menuState == MENU_STATE_CHANGE_ITEM)||(menuState == MENU_STATE_SET_VALUE))  //增加这个判断,只在用户按下确认键,由'->'变成'*'之后才允许输入数字
					{
						CurrentIsEnterNumberFlag = 0;
					}
					else
					{
						CurrentIsEnterNumberFlag = 1;
					}
					menuSubItem = 0;
					defaultMenuPageProcess(type,value);
				}
			break;
			case MENU_EVENT_EXIT:
				menuSubItem = 0;
				CurrentIsEnterNumberFlag = 0;
			break;
			case MENU_EVENT_SELECT_ITEM:  //只在用户按下确认键,由'->'变成'*'之后才允许输入数字
				menuSubItem = 0;
				CurrentIsEnterNumberFlag = 1;
				defaultMenuPageProcess(type, value);
			break;	
			case MENU_EVENT_CHANGE_ITEM:
			case MENU_EVENT_SET_VALUE:
			case MENU_KEY_MENU:
				menuSubItem = 0;
		    case MENU_KEY_SCALE:
		    case MENU_KEY_SAVE:
		    case MENU_KEY_LOAD:
			case MENU_KEY_OUTPUT:
			case MENU_KEY_INFO:
			case MENU_KEY_PIP:
				CurrentIsEnterNumberFlag = 0;
			default:
				defaultMenuPageProcess(type, value);
			break;
		}
	
	}
	else
	{
		switch(type)
		{
			case MENU_EVENT_ENTER:
				if(menuIsCurrentPageIndex(MENU_INDEX_PIP))
				{
					if(GetSystemParam()->PipModeSwitch == 0)
					{
						LCD_Clear();
						pCurrentMenuPage->menuTtemSize = 2;
					}
					else
					{
						pCurrentMenuPage->menuTtemSize = 4;//sizeof(pipModePage)/sizeof(MenuPage);
					}
				}

			default:
				defaultMenuPageProcess(type,value);
				break;
		}
	}
}


#else


void enterNumberPageProcess(int type, int value)
{
	
	const MenuPage *menuPage;
	int item = 1;
	char buffer[40];
	char fb;

	item = pCurrentMenuPage->currentItem;
	menuPage = pCurrentMenuPage->menuPage;
	if((menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE) 
		|| (type == MENU_EVENT_DRAW_ITEM))
	{
		switch(type)
		{
		case MENU_EVENT_ENTER:
			menuSubItem = 0;
			defaultMenuPageProcess(type,value);
			break;
		case MENU_EVENT_EXIT:
			menuSubItem = 0;
			break;
		case MENU_KEY_MENU:
			menuSubItem = 0;
			menuRemoveMainFunc(enterNumberPageTask);
	        defaultMenuPageProcess(type,value);
	        break;
	    case MENU_KEY_NEXT:
	        {
	           switch(menuState)
	            { 
	            case MENU_STATE_CHANGE_ITEM:
					menuSubItem = 1;
					menuState = MENU_STATE_SELECT_ITEM;
					pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					break;
	            case MENU_STATE_SELECT_ITEM:
					if(menuSubItem)
					{
						menuSubItem++;
						if(menuSubItem > ENTER_NUMBER_SIZE)
						{
							menuSubItem = 0;
							menuState = MENU_STATE_CHANGE_ITEM;
	                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
						}
						else
						{
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
						}
					}
					else
					{	menuSubItem = 1;
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					}
	                break;
	            case MENU_STATE_CHANGE_VALUE:
					if(menuSubItem)
					{
						
						{
							menuSubItem = 0;
							menuState = MENU_STATE_SET_VALUE;
	                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
						}
						
					}
					else
					{
						defaultMenuPageProcess(type, value);
					}
	                break; 
				default:
					defaultMenuPageProcess(type, value);
					break;
	            }
	        }
	        break;
	    case MENU_KEY_UP:
			if(menuSubItem)
			{
				int currentNum;
				menuState = MENU_STATE_CHANGE_VALUE;
				currentNum = getSubNumber(menuValueList[item],ENTER_NUMBER_SIZE-menuSubItem);
				currentNum++;
				if((currentNum<-9) || (currentNum>9))
				{
					currentNum = 9;
				}
				menuValueList[item] = setSubNumber(menuValueList[item], ENTER_NUMBER_SIZE-menuSubItem, currentNum);
				
				menuResetMainTimeout();
				menuValueList[0] = 1;
				menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
			}
			else
			{
				defaultMenuPageProcess(type, value);
			}
	        break;
	    case MENU_KEY_DOWN:
			if(menuSubItem)
			{
				int currentNum;
				menuState = MENU_STATE_CHANGE_VALUE;
				currentNum = getSubNumber(menuValueList[item],ENTER_NUMBER_SIZE-menuSubItem);
				currentNum--;
				if((currentNum<-9) || (currentNum>9))
				{
					currentNum = -9;
				}
				menuValueList[item] = setSubNumber(menuValueList[item], ENTER_NUMBER_SIZE-menuSubItem, currentNum);
				
				menuResetMainTimeout();
				menuValueList[0] = 1;
				menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
			}
			else
			{
				defaultMenuPageProcess(type, value);
			}
	        break;
	    case MENU_KEY_LEFT:
			{
	           switch(menuState)
	            {
	            case MENU_STATE_CHANGE_ITEM:
					pCurrentMenuPage->menuPage[item].pfProcess(MENU_KEY_NEXT, item);
					break;
	            case MENU_STATE_SELECT_ITEM:
					if(menuSubItem)
					{
						menuSubItem--;
						if(menuSubItem < 1)
						{
							menuSubItem = 0;
							menuState = MENU_STATE_CHANGE_ITEM;
	                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
						}
						else
						{
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
						}
					}
					else
					{
						defaultMenuPageProcess(type, value);
					}
	                break;
	            case MENU_STATE_CHANGE_VALUE:
					if(menuSubItem)
					{
						menuSubItem--;
						if(menuSubItem < 1)
						{
							menuSubItem = 0;
							menuState = MENU_STATE_SET_VALUE;
	                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
						}
						else
						{
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
						}
					}
					else
					{
						defaultMenuPageProcess(type, value);
					}
	                break; 
				default:
					defaultMenuPageProcess(type, value);
					break;
	            }
	        }
	        break;
	    case MENU_KEY_RIGHT:
			{
	           switch(menuState)
	           { 
	            case MENU_STATE_CHANGE_ITEM:
					pCurrentMenuPage->menuPage[item].pfProcess(MENU_KEY_NEXT, item);
					break;
	            case MENU_STATE_SELECT_ITEM:
					if(menuSubItem)
					{
						menuSubItem++;
						if(menuSubItem > ENTER_NUMBER_SIZE)
						{
							menuSubItem = 0;
							menuState = MENU_STATE_CHANGE_ITEM;
	                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
						}
						else
						{
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
						}
					}
					else
					{
						defaultMenuPageProcess(type, value);
					}
	                break;
	            case MENU_STATE_CHANGE_VALUE:
					if(menuSubItem)
					{
						menuSubItem++;
						if(menuSubItem > ENTER_NUMBER_SIZE)
						{
							menuSubItem = 0;
							menuState = MENU_STATE_SET_VALUE;
	                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
						}
						else
						{
							pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
						}
					}
					else
					{
						defaultMenuPageProcess(type, value);
					}
	                break; 
				default:
					defaultMenuPageProcess(type, value);
					break;
	            }
	        }
	        break;
		case MENU_EVENT_DRAW_ITEM:
			#ifdef ENABLE_HD44780_LCD
			#if 1
			if(value == item)
	        {
	        	if(menuValueList[value]<0)
	        	{
	        		sprintf(buffer, "%s%-9s%+0.4d", menuStateStr[menuState],langGetStr(menuPage[value].str),menuValueList[value]); 
	        	}
				else
				{
					sprintf(buffer, "%s%-10s%0.4d", menuStateStr[menuState],langGetStr(menuPage[value].str),menuValueList[value]); 
				}
	        	
				if(menuValueList[0] == 0)
				{
					if(menuSubItem)
					{
						buffer[14-(ENTER_NUMBER_SIZE-menuSubItem)] = ' ';
					}
				}
				menuAddMainFunc(500,enterNumberPageTask);
	        }
	        else
	        {
	        	if(menuValueList[value]<0)
	        	{
	            	sprintf(buffer, "%s%-9s%+0.4d", MENU_NO_SELECT,langGetStr(menuPage[value].str),menuValueList[value]);
	        	}
				else
				{
					sprintf(buffer, "%s%-10s%0.4d", MENU_NO_SELECT,langGetStr(menuPage[value].str),menuValueList[value]);
				}
	        }
			#else

			#endif
			#else
			#if 1  // 可以调节负值
			if(value == item)
	        {
				fb = 1;
	        	if(menuValueList[value]<0)
	        	{
	        		sprintf(buffer, "%s%-13s%+0.4d", menuStateStr[menuState],langGetStr(menuPage[value].str),menuValueList[value]); 
	        	}
				else
				{
					sprintf(buffer, "%s%-14s%0.4d", menuStateStr[menuState],langGetStr(menuPage[value].str),menuValueList[value]); 
				}
				if(menuValueList[0] == 0)
				{
					if(menuSubItem)
					{
						buffer[19-(ENTER_NUMBER_SIZE-menuSubItem)] = ' ';
					}
				}
				menuAddMainFunc(500,enterNumberPageTask);
	        }
	        else
	        {
				fb = 0;
	        	if(menuValueList[value]<0)
	        	{
	            	sprintf(buffer, "%s%-13s%+0.4d", MENU_NO_SELECT,langGetStr(menuPage[value].str),menuValueList[value]);
	        	}
				else
				{
					sprintf(buffer, "%s%-14s%0.4d", MENU_NO_SELECT,langGetStr(menuPage[value].str),menuValueList[value]);
				}
	        }
			#else

			#endif
			#endif
			
			if(	fb )
			{
				LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			else
			{
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			//LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			
			break;
		default:
			defaultMenuPageProcess(type, value);
			break;
		}
	
	}
	else
	{
		defaultMenuPageProcess(type, value);
	}
}
#endif

int getSubIpNumber(int value,int index)// index = 1~4
{
	if((1<=index)&&(index<=4))
	{
		return (value>>(8*(4-index)))&0xff;
	}
	return 0;
}

int setSubIpNumber(int value, int index, int subValue)
{
	if((1<=index)&&(index<=4))
	{
		index = ((4-index)*8);
		
		value = value & (~(0xff<<index));
		value |= subValue <<index;
		return value;
	}
	return 0;
}

void menuPageBlinkTask(void)
{
	//const MenuPage *menuPage;
	//int item = 1;

	//item = pCurrentMenuPage->currentItem;
	//menuPage = pCurrentMenuPage->menuPage;
	
	
	if(menuValueList[0])
	{
		menuValueList[0] = 0;
	}
	else
	{
		menuValueList[0] = 1;
	}
	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, pCurrentMenuPage->currentItem);
}

void IpAddressMenuPageProcess(int type, int value)
{
	const MenuPage *menuPage = pCurrentMenuPage->menuPage;
    int item = pCurrentMenuPage->currentItem;
	char buffer[40];
//	char fb;
	
	menuPage = menuPage;
	item = item;
	switch(type)
	{
	case MENU_EVENT_ENTER:
		menuSubItem = 0;
		pCurrentMenuPage->currentItem = 2;
		defaultMenuPageProcess(type,value);		
		break;
	case MENU_EVENT_EXIT:
		menuSubItem = 0;
		menuRemoveMainFunc(menuPageBlinkTask); //cyj 20130624
		break;
	case MENU_KEY_MENU:
		menuSubItem = 0;
		menuRemoveMainFunc(menuPageBlinkTask);
        defaultMenuPageProcess(type,value);
        break;
	case MENU_KEY_NEXT:
        {
           switch(menuState)
            { 
            case MENU_STATE_CHANGE_ITEM:
				menuSubItem = 1;
				menuState = MENU_STATE_SELECT_ITEM;
				pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
				break;
            case MENU_STATE_SELECT_ITEM:
				if(menuSubItem)
				{
					menuSubItem++;
					if(menuSubItem > ENTER_NUMBER_SIZE)
					{
						menuSubItem = 0;
						menuState = MENU_STATE_CHANGE_ITEM;
                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
					}
					else
					{
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					}
				}
				else
				{	menuSubItem = 1;
					pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
				}
                break;
            case MENU_STATE_CHANGE_VALUE:
				if(menuSubItem)
				{
					menuSubItem++;
					if(menuSubItem > 4)
					{
						menuSubItem = 0;
						menuState = MENU_STATE_SET_VALUE;
                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
					}
					else
					{
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					}
					
				}
				else
				{
					defaultMenuPageProcess(type, value);
				}
                break; 
			default:
				defaultMenuPageProcess(type, value);
				break;
            }
        }
        break;
    case MENU_KEY_UP:
		if(menuSubItem)
		{
			int currentNum;
			menuState = MENU_STATE_CHANGE_VALUE;
			currentNum = getSubIpNumber(menuValueList[item],menuSubItem);
			currentNum += getValue(stepValue);
			if((currentNum<0) || (currentNum>255))
			{
				currentNum = 255;
			}
			menuValueList[item] = setSubIpNumber(menuValueList[item], menuSubItem, currentNum);
			
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
		}
		else
		{
			defaultMenuPageProcess(type, value);
		}
        break;
    case MENU_KEY_DOWN:
		if(menuSubItem)
		{
			int currentNum;
			menuState = MENU_STATE_CHANGE_VALUE;
			currentNum = getSubIpNumber(menuValueList[item], menuSubItem);
			currentNum -= getValue(stepValue);
			if((currentNum<0) || (currentNum>255))
			{
				currentNum = 0;
			}
			menuValueList[item] = setSubIpNumber(menuValueList[item], menuSubItem, currentNum);
			
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
		}
		else
		{
			defaultMenuPageProcess(type, value);
		}
        break;
    case MENU_KEY_LEFT:
		{
           switch(menuState)
            {
            case MENU_STATE_CHANGE_ITEM:
				pCurrentMenuPage->menuPage[item].pfProcess(MENU_KEY_NEXT, item);
				break;
            case MENU_STATE_SELECT_ITEM:
				if(menuSubItem)
				{
					menuSubItem--;
					if(menuSubItem < 1)
					{
						menuSubItem = 0;
						menuState = MENU_STATE_CHANGE_ITEM;
                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
					}
					else
					{
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					}
				}
				else
				{
					defaultMenuPageProcess(type, value);
				}
                break;
            case MENU_STATE_CHANGE_VALUE:
				if(menuSubItem)
				{
					menuSubItem--;
					if(menuSubItem < 1)
					{
						menuSubItem = 0;
						menuState = MENU_STATE_SET_VALUE;
                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
					}
					else
					{
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					}
				}
				else
				{
					defaultMenuPageProcess(type, value);
				}
                break; 
			default:
				defaultMenuPageProcess(type, value);
				break;
            }
        }
        break;
    case MENU_KEY_RIGHT:
		{
           switch(menuState)
           { 
            case MENU_STATE_CHANGE_ITEM:
				pCurrentMenuPage->menuPage[item].pfProcess(MENU_KEY_NEXT, item);
				break;
            case MENU_STATE_SELECT_ITEM:
				if(menuSubItem)
				{
					menuSubItem++;
					if(menuSubItem > ENTER_NUMBER_SIZE)
					{
						menuSubItem = 0;
						menuState = MENU_STATE_CHANGE_ITEM;
                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_CHANGE_ITEM, 0);
					}
					else
					{
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					}
				}
				else
				{
					defaultMenuPageProcess(type, value);
				}
                break;
            case MENU_STATE_CHANGE_VALUE:
				if(menuSubItem)
				{
					menuSubItem++;
					if(menuSubItem > ENTER_NUMBER_SIZE)
					{
						menuSubItem = 0;
						menuState = MENU_STATE_SET_VALUE;
                		pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, 0);
					}
					else
					{
						pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
					}
				}
				else
				{
					defaultMenuPageProcess(type, value);
				}
                break; 
			default:
				defaultMenuPageProcess(type, value);
				break;
            }
        }
        break;
	case MENU_EVENT_DRAW_ITEM:
		if(menuPage[value].type == MENU_TYPE_LABEL)
        {
        	if(strlen(langGetStr(menuPage[value].str)) > 18)
        	{
        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
        	}
			else
			{
            	sprintf(buffer, "%s%s:", MENU_NO_SELECT,langGetStr(menuPage[value].str));
			}
        }
		else if(menuPage[value].type == MENU_TYPE_VALUE)
		{
			if(value == item)
	        {
	        	sprintf(buffer, "%s%s", menuStateStr[menuState],valueToString(menuPage[value].value,menuValueList[value]));   
				if(menuValueList[0] == 0)
				{
					if(menuSubItem)
					{
						int index = (menuSubItem-1)*4 + 2;
						buffer[index]   = ' ';
						buffer[index+1] = ' ';
						buffer[index+2] = ' ';
					}
				}
				menuAddMainFunc(500,menuPageBlinkTask);
	        }
	        else
	        {
	            sprintf(buffer, "%s%s", MENU_NO_SELECT, valueToString(menuPage[value].value,menuValueList[value]));
	        }
		}
		LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
		break;
	default:
		defaultMenuPageProcess(type,value);
		break;
	}
}

void scaleMenuPageProcess(int type, int value)
{
	const MenuPage *menuPage;
	int item = 1;

	item = pCurrentMenuPage->currentItem;
	menuPage = pCurrentMenuPage->menuPage;

	item = item;
	menuPage = menuPage;
	
	switch(type)
	{

		case MENU_KEY_HSIZE:
			if(item != 1)
			{
				item = 1;
			}
			else
			{
				item = 3;
			}
			pCurrentMenuPage->currentItem = item;
			menuPage->pfProcess(MENU_EVENT_ENTER, 0);
			break;
		case MENU_KEY_VSIZE:
			if(item != 2)
			{
				item = 2;
			}
			else
			{
				item = 4;
			}
			pCurrentMenuPage->currentItem = item;
			menuPage->pfProcess(MENU_EVENT_ENTER, 0);
			break;
			
		case MENU_EVENT_EXIT:
			setTotaryDelay(100);
			CurrentIsEnterNumberFlag = 0;
			break;
		case MENU_EVENT_PREV_VALUE:
			printf("scaleMenuPageProcess()_MENU_EVENT_PREV_VALUE\r\n");
			break;
		case MENU_EVENT_NEXT_VALUE:
			printf("scaleMenuPageProcess()_MENU_EVENT_NEXT_VALUE\r\n");
			break;
		case MENU_EVENT_ENTER:
			{
				setTotaryDelay(100);//(300);(300);
				CurrentIsEnterNumberFlag = 0;
			}
		default: 
			enterNumberPageProcess(type,value);
			break;
	}
}

static u8 saveUserSettingIndex = 0;
static u8 returnFromReplace = 0;

void USBUdateTips(void)
{
	menuEnterPage(MENU_INDEX_NOTHING);
	LCD_WriteStr(0,0, langGetStr(bootCopyCodestr));
	LCD_WriteStr(1,0, langGetStr(bootRestartstr));
}


extern void Bsp_Soft_Reset(void);
void dispFactoryReset(void)
{
	menuEnterPage(MENU_INDEX_FACTORY_RESET_OK);
	//printf("MENU_INDEX_FACTORY_RESET_OK_MENU_KEY_SELECT\r\n");
	LCD_WriteStr(0,11, "		");
	LCD_WriteLine(1, "");
	menuSetBusy(0,9);
	ResetFactory();
	menuDisableBusy();
	LCD_WriteStr(0,11, langGetStr(finishedStr));
	dispLineCenter_L(2,langGetStr(pleaseRestartStr));
	SysDelay1ms(2000);
	Bsp_Soft_Reset();
}

void menuPage_UsbUpdateConfirm_Process(int type, int value)
	{
		char buffer[40];
		const MenuPage *menuPage;
		int item;
//		char fb;
	
		item = pCurrentMenuPage->currentItem;
		menuPage = pCurrentMenuPage->menuPage;
		switch(type)
		{		
		case MENU_KEY_SELECT:		
			//setValue(menuPage[3].value,1);
			if(setUsbUpdateFlag(1) == TRUE)
			{
				menuPage->pfProcess(MENU_KEY_MENU, 0);				
			}
			else
			{
				LCD_WriteStr(1,9, langGetStr(updateFirmwareFailStr));
				SysDelay1ms(1000);
				menuPage->pfProcess(MENU_KEY_MENU, 0);				
			}
		
			break;
			
		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_LABEL)
			{
				sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
			}
			else if(menuPage[value].type == MENU_TYPE_DIR)
			{
				if(value == item)
				{
					//fb = 1;
					sprintf(buffer, "%s%-15s%s", MENU_SELECT_ITEM,langGetStr(menuPage[value].str), MENU_DIR_MARK);
				}
				else
				{
					//fb = 0;
					sprintf(buffer, "%s%-15s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str), MENU_DIR_MARK);
				}
			}
			else if(menuPage[value].type == MENU_TYPE_VALUE)
			{
				if(menuValueList[value])
				{
					sprintf(buffer,"%s", langGetStr(menuPage[value].str));
				}
				else
				{
					sprintf(buffer,"%s", langGetStr(menuPage[value].str));
				}			
			}
			else if(menuPage[value].type == MENU_TYPE_LABEL_WITH_VALUE)
			{
				if(menuIsCurrentPageIndex(MENU_INDEX_SAVE_VERIFY))
				{
					sprintf(buffer, "%s%d%s",langGetStr(menuPage[value].str),(saveUserSettingIndex+1),langGetStr(menuPage[value].str+1));
				}
			}
			else
			{
				sprintf( buffer, "NO SUPPORT");
			}
			LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			break; 
	
		case MENU_KEY_MENU:
			printf("noYesPageProcess_MENU_KEY_MENU\r\n");
			if(!menuIsCurrentPageIndex(MENU_INDEX_SAVE_VERIFY))
			{
				if(menuIsCurrentPageIndex(MENU_INDEX_IMAGE_ENHANCE_VERIFY))
				{				
					pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_TRANSITION);
				}
				else
				{
					defaultMenuPageProcess(type,value);
				}
			}	
			else
			{
				SysDelay1ms(200);
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_SAVE_TO);
			}
		break;
		
		case MENU_EVENT_ENTER:
			menuClearItemBar();
			LCD_WriteLine3_L("");
			LCD_WriteLine4_L("");
			defaultMenuPageProcess(type,value);
			break;
				
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
}


void noYesPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;
//	char fb;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	switch(type)
	{
	
	case MENU_KEY_SELECT:
		if(menuIsCurrentPageIndex(MENU_INDEX_FACTORY_RESET_OK))
		{
			setValue(factoryResetValue,1);
		}
		else if(menuIsCurrentPageIndex(MENU_INDEX_SAVE_VERIFY))
		{
			LCD_WriteStr(1,12, langGetStr(finishedStr));
			setValue(saveToValue,saveUserSettingIndex);
			returnFromReplace = 1;
			SysDelay1ms(1500);
			//SetInputPortLedStatus(GetPgmCurrentInput(),GetPrvCurrentInput(),GetPgmCurrentDisplayMode());
			//CurrentIsEnterNumberFlag = 0;		
			menuProcessEvent(MENU_KEY_INFO);

		}
		break;
		
	case MENU_EVENT_DRAW_ITEM:
		if(menuPage[value].type == MENU_TYPE_LABEL)
        {
            if(menuIsCurrentPageIndex(MENU_INDEX_IMAGE_ENHANCE_VERIFY) == TRUE)
            {
                sprintf(buffer, "%s",langGetStr(menuPage[value].str));
            }
            else
            {
                sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
            }
        }
        else if(menuPage[value].type == MENU_TYPE_DIR)
        {
            if(value == item)
            {
				//fb = 1;
                sprintf(buffer, "%s%-15s%s", MENU_SELECT_ITEM,langGetStr(menuPage[value].str), MENU_DIR_MARK);
            }
            else
            {
				//fb = 0;
                sprintf(buffer, "%s%-15s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str), MENU_DIR_MARK);
            }
        }
        else if(menuPage[value].type == MENU_TYPE_VALUE)
        {
        	if(menuValueList[value])
        	{
        		sprintf(buffer,"%s", langGetStr(menuPage[value].str));
        	}
			else
			{
				sprintf(buffer,"%s", langGetStr(menuPage[value].str));
			}			
        }
		else if(menuPage[value].type == MENU_TYPE_LABEL_WITH_VALUE)
		{
			if(menuIsCurrentPageIndex(MENU_INDEX_SAVE_VERIFY))
			{
	            sprintf(buffer, "%s%d%s",langGetStr(menuPage[value].str),(saveUserSettingIndex+1),langGetStr(menuPage[value].str+1));
			}
		}
        else
        {
            sprintf( buffer, "NO SUPPORT");
        }
        LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
        break; 

	case MENU_KEY_MENU:
		printf("noYesPageProcess_MENU_KEY_MENU\r\n");
		if(!menuIsCurrentPageIndex(MENU_INDEX_SAVE_VERIFY))
		{
			if(menuIsCurrentPageIndex(MENU_INDEX_IMAGE_ENHANCE_VERIFY))
			{				
//				GetUserSetting()->imageEnhance = 0;
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_TRANSITION);
			}
			else
			{
				defaultMenuPageProcess(type,value);
			}
		}	
		else
		{
			SysDelay1ms(200);
			//menuPage->pfProcess(MENU_KEY_MENU, 0);
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_SAVE_TO);
		}
		break;
	
    case MENU_EVENT_ENTER:
		//no break
		//LCD_Clear();
		menuClearItemBar();
		LCD_WriteLine3_L("");
		LCD_WriteLine4_L("");
		if(CurrentIsSavePage())
		{
			if(getFrontType() == 0)			
			{
			}
			else
			{
			}
		}
		defaultMenuPageProcess(type,value);
		break;
		
	default:
		defaultMenuPageProcess(type,value);
		break;
	}
}

void testPatternMenuTask(void)
{
	const MenuPage *menuPage;
    int item;
	int drawItem = 1;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	
	menuValueList[drawItem] = getValue(menuPage[drawItem].value);

	if(((item-1)/MENU_DISPLAY_LINE_NUM) == ((drawItem-1)/MENU_DISPLAY_LINE_NUM))
	{
	
		menuProcessEventValue(MENU_EVENT_DRAW_ITEM,drawItem);
	}
}

void testPatternMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;
	char fb = 0;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	
    switch(type)
    {
    case MENU_EVENT_ENTER:
		menuClearItemBar();
		if(getValue(tpSwitchValue) == 0)
        {
			if(pCurrentMenuPage != &menuList[MENU_INDEX_TEST_PATTERN])
        	{
        		pCurrentMenuPage = &menuList[MENU_INDEX_TEST_PATTERN];
				pCurrentMenuPage->currentItem = 1;
				defaultMenuPageProcess(type,value);	
				return;
        	}        
        }
        else
        {		
	       // if(getValue(testPatternModeValue) == 65)
	        if(getValue(testPatternModeValue) == 19)    
	        {
	        	if(pCurrentMenuPage != &menuList[MENU_INDEX_TP_COLOR])
	        	{
	        		pCurrentMenuPage = &menuList[MENU_INDEX_TP_COLOR];
					pCurrentMenuPage->currentItem = 1;
					defaultMenuPageProcess(type,value);	
					return;
	        	}
	        }
			//else if(getValue(testPatternModeValue) == 66)
	        else if(getValue(testPatternModeValue) == 20)    
			{
				if(pCurrentMenuPage != &menuList[MENU_INDEX_TP_PARAM])
	        	{
	        		pCurrentMenuPage = &menuList[MENU_INDEX_TP_PARAM];
					pCurrentMenuPage->currentItem = 1;
					defaultMenuPageProcess(type,value);	
					return;
	        	}
			}
			else
			{
				if(pCurrentMenuPage != &menuList[MENU_INDEX_TEST_PATTERN])
	        	{
	        		pCurrentMenuPage = &menuList[MENU_INDEX_TEST_PATTERN];
					pCurrentMenuPage->currentItem = 1;
					defaultMenuPageProcess(type,value);	
					return;
	        	}
			}
        }
		defaultMenuPageProcess(type,value);	
		dispMenuPosBar(1,(pCurrentMenuPage->menuTtemSize - 1));
    break;
	case MENU_EVENT_EXIT:
	break;
	case MENU_EVENT_NEXT_VALUE:
        {
            int index;
            index = menuPage[item].value;
            menuState = MENU_STATE_CHANGE_AND_SET_VALUE;
            menuValueList[item] = nextValue(index,getValue(index));
			if(getValue(index) != menuValueList[item])
			{
	            setValue(index, menuValueList[item]);
	            menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
			}
        }
    break;
    case MENU_EVENT_PREV_VALUE:
	   {
		   int index;
		   index = menuPage[item].value;
		   menuState = MENU_STATE_CHANGE_AND_SET_VALUE;
		   menuValueList[item] = prevValue(index,getValue(index));
		   if(getValue(index) != menuValueList[item])
		   {
			   setValue(index, menuValueList[item]);
			   menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
		   }
		   //setValue(index, menuValueList[item]);
		   //menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
	   }
    break;	
	case MENU_EVENT_DRAW_ITEM:
	        if(menuPage[value].type == MENU_TYPE_LABEL)
	        {
	        	if(strlen(langGetStr(menuPage[value].str)) > 18)
	        	{
	        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
	        	}
				else
				{
	            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
				}
	        }
	        else if(menuPage[value].type == MENU_TYPE_VALUE)
	        {
	        	if(strlen(valueToString(menuPage[value].value,menuValueList[value])) == 10)
	        	{
					//printf("strlen == 10\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-18s%10s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-18s%10s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 14)
				{
					//printf("strlen > 14\r\n");
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%s %s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%s %s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 8)
	        	{
					//printf("strlen > 8\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-14s%14s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-14s%14s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 6)
				{
					//printf("strlen > 6\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-20s%8s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-20s%8s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) < 4)
				{
					//printf("strlen < 4\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-24s%4s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-24s%4s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else
				{
					//printf("strlen else\r\n");
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-22s%6s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-22s%6s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
	        }
				if(	fb )
				{
					LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				else
				{
					LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
	break;

	case MENU_EVENT_NEXT_ITEM: 
        item++;
        if(item < 1)
        {
            item = pCurrentMenuPage->menuTtemSize-1;
        }
        if(item > (pCurrentMenuPage->menuTtemSize-1))
        {
            item = pCurrentMenuPage->menuTtemSize-1;
        }
        pCurrentMenuPage->currentItem = item;
        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);

		//menuClearItemBar();
		if(pCurrentMenuPage->menuTtemSize > 2)
		{
			dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1));
		}
		break;
		
	case MENU_EVENT_PREV_ITEM:
        item--;
        if(item < 1)
        {
            item = 1;
        }
        if(item > (pCurrentMenuPage->menuTtemSize-1))
        {
            item = 1;
        }
        pCurrentMenuPage->currentItem = item;
        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
		//menuClearItemBar();
		if(pCurrentMenuPage->menuTtemSize > 2)
		{
				dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1));
          }
		break;

	case MENU_KEY_SELECT:
		if((menuPage[item].type == MENU_TYPE_VALUE)&&(menuState == MENU_STATE_CHANGE_AND_SET_VALUE))
		{

			switch(menuPage[item].value)
			{
				case testPatternModeValue:
					break;
				case tpColorRedValue:
				case tpColorGreenValue:
				case tpColorBlueValue:
					break;
				case tpHorStepValue:
				case tpVerStepValue:
				case tpStepColorValue:
					break;	
                default:
                    break;
			}
		}
	default:
		defaultMenuPageProcess(type,value);
		break;
    }
}
void resetMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;
	char fb = 0;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	switch(type)
	{
	case MENU_EVENT_DRAW_ITEM:
        if(menuPage[value].type == MENU_TYPE_VALUE)
        {
            if(value == item)
            {
				fb = 1;
                if(menuState == MENU_STATE_CHANGE_ITEM)
                {
                    sprintf(buffer, "%s%-28s", MENU_SELECT_ITEM,langGetStr(menuPage[value].str));
                }
                else
                {
                    sprintf(buffer, "%s%-28s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
                }
            }
            else
            {
				fb= 0;
                sprintf(buffer, "%s%-28s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
            }
        }
        else
        {
            sprintf( buffer, "NO SUPPORT");
        }
		if(	fb )
		{
			LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
		}
		else
		{
			LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
		}
		dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1)); //added by cyj
        break;
	case MENU_EVENT_SELECT_ITEM:
		if(menuIsCurrentPageIndex(MENU_INDEX_VGA_ADJUST))
		{

		}

		setValue(menuPage[item].value, menuValueList[item]);
		if(menuIsCurrentPageIndex(MENU_INDEX_VGA_ADJUST))
		{
			menuDisableBusy();
		}

        menuState = MENU_STATE_CHANGE_ITEM;
        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
        break;
	case MENU_EVENT_EXIT:		
		if(menuIsCurrentPageIndex(MENU_INDEX_SCALE))
		{
		}
	default:
		defaultMenuPageProcess(type,value);
		break;
	}
}


void nothingMenuPageProcess(int type, int value)
{
	switch(type)
	{
	case MENU_KEY_UP:
	case MENU_KEY_DOWN:
	case MENU_KEY_LEFT:
	case MENU_KEY_RIGHT:
	case MENU_KEY_NEXT:
		break;
	default:
		defaultMenuPageProcess(type,value);
		break;
	}
}

void statusMenuTask(void)
{
	//printf("statusMenuTask()_  ");
	if(pCurrentMenuPage == &menuList[MENU_INDEX_STATUS])
	{
		menuProcessEvent(MENU_KEY_INFO);
	}
	else
	{
		menuRemoveMainFunc(statusMenuTask);
	}
	//printf("\r\n");
}
#define ICONLocalX1 0
#define ICONLocalX2 7//6+1
#define ICONLocalX3 14//12+2
#define ICONLocalX4 21//18+3
#define ICONLocalX5 28//24+4
#define ICONLocalX6 35//30+5
#define ICONLocalX7 42//36+6
#define ICONLocalX8 49//42+7
#define ICONLocalX9 56//48+8
static PIC_FLASH_INDEX statusIconList[7] = {PIC_H_2_1,PIC_V_2_1,PIC_V_4_1,PIC_FILD,PIC_4P,PIC_PIP_OFF,PIC_UN_LOCK};

void scanStatusAndRefleshIconList(void)
{
    int temp;

	temp = GetPgmCurrentDisplayMode();
	switch(temp)
	{
		case _OSD_DM_1P:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD_F);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case _OSD_DM_4P:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P_F);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case HORIZONTAL_1_4_MODE:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case VERICAL_1_4_MODE:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1_F);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case _OSD_DM_2P_TB:
		case _OSD_DM_2P_TOW_PORT_TB:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1_F);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case _OSD_DM_2P_LR:
		case _OSD_DM_2P_TOW_PORT_LR:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1_F);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case _OSD_DM_2P_SACLE_TB:
		case _OSD_DM_3P_SACLE_TB:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_H_4_1_F);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case _OSD_DM_2P_SACLE_LR:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_OFF);
		break;
		case _OSD_DM_2P_PIP:
			statusIconList[0] = (PIC_FLASH_INDEX)(PIC_H_2_1);
			statusIconList[1] = (PIC_FLASH_INDEX)(PIC_V_2_1);
			statusIconList[2] = (PIC_FLASH_INDEX)(PIC_V_4_1);
			statusIconList[3] = (PIC_FLASH_INDEX)(PIC_FILD);
			statusIconList[4] = (PIC_FLASH_INDEX)(PIC_4P);
			switch(GetSystemParam()->PipLayout)
			{
				case LAYOUT_PIP_LT:
					statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_LT);
				break;
				case LAYOUT_PIP_RT:	 //added by cyj
					statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_RT);
				break;
				case LAYOUT_PIP_LB:	 //added by cyj
					statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_LB);
				break;
				case LAYOUT_PIP_RB:	 //added by cyj
					statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_RB);
				break;
				case LAYOUT_PIP_CENT: //added by cyj
					statusIconList[5] = (PIC_FLASH_INDEX)(PIC_PIP_CNTER);
				break;
				case LAYOUT_PBP_LR:
					statusIconList[5] = (PIC_FLASH_INDEX)(PIC_H_2_1);
				break;
				case LAYOUT_PBP_TB:
					statusIconList[5] = (PIC_FLASH_INDEX)(PIC_V_2_1);
				break;
			}
		break;
	}
	if(getValue(keyLockValue))
	{
		statusIconList[6] = (PIC_FLASH_INDEX)(PIC_LOCK);
	}
	else
	{
		statusIconList[6] = (PIC_FLASH_INDEX)(PIC_UN_LOCK);
	}
	
}

extern unsigned char gImage[];

void dispIconList(void)
{
	u8 i = 0;
//    u8  index;
	for(i= 0; i < sizeof(statusIconList); i++)
	{
		if(statusIconList[i] != NumOfPic)
		{
			LcdDispBMP(ICONLocalX9 - 7*i,32 ,statusIconList[i]);
		}
	}
}

void disableDispReflrshInputFormat(void)
{
	//printf("disableDispReflrshInputFormat--------->%d\r\n",GetSysTick());
	isDispReflrshInputFormatEnable = 0;
}
void enableDispReflrshInputFormat(void)
{
	//printf("enableDispReflrshInputFormat--------->%d\r\n",GetSysTick());
	isDispReflrshInputFormatEnable = 1;
}
char getIsDispReflrshInputFormatEnable(void)
{
	return isDispReflrshInputFormatEnable;
}


extern const char * inputSourceStr_List[];
u8 InputSourceConv(u8 x)
{
	switch(x)
	{
		case _DP_INPUT:
			return 0;
		case _HDMI14_INPUT:
			return 1;
		case _HDMI20_INPUT:
			return 2;
		case _DVI_INPUT:
			return 3;

		#ifdef _VGA_INPUT_ENABLE
		case _VGA_INPUT:
			return 4;
		#endif
		default:
			break;
	}
	return _HDMI20_INPUT;
}

u8 InputSourceConv1(u8 x)
{
	switch(x)
	{
		case 0:
			return _DP_INPUT;
		case 1:
			return _HDMI14_INPUT;
		case 2:
			return _HDMI20_INPUT;
		case 3:
			return _DVI_INPUT;

		#ifdef _VGA_INPUT_ENABLE
		case 4:
			return _VGA_INPUT;
		#endif
		default:
			break;
	}
	return _HDMI20_INPUT;
}


void statusMenuPageProcess(int type, int value)
{
	
	char buffer[40];
	int item;
	//InputResolution_Struct *inputRes;
	item = pCurrentMenuPage->currentItem;
	switch(type)
	{
	    case MENU_KEY_UP:
	    case MENU_KEY_DOWN:
	    break;
		case MENU_KEY_LEFT:
		case MENU_KEY_RIGHT:
		case MENU_KEY_NEXT:
		case MENU_EVENT_ENTER:
		//printf("statusMenuPageProcess\r\n");
		enableDispErrorInfo();
		enableDispReflrshInputFormat();
		LCD_Clear();
		menuState = MENU_STATE_CHANGE_ITEM;
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 0);
		break;
		case MENU_EVENT_DRAW_REFESH:
		enableDispErrorInfo();
		enableDispReflrshInputFormat();
		menuState = MENU_STATE_CHANGE_ITEM;
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 0);        
        break;
		case MENU_EVENT_EXIT:
		//printf("statusMenuPageProcess_MENU_EVENT_EXIT\r\n");
		disableDispErrorInfo();
		disableDispReflrshInputFormat();
		break;
		case MENU_EVENT_NEXT_ITEM:
			//printf("statusMenuPageProcess_MENU_EVENT_NEXT_ITEM\r\n");
			item++;
	        if(item < 1)
	        {
	            item = pCurrentMenuPage->menuTtemSize-1;
	        }
	        if(item > (pCurrentMenuPage->menuTtemSize-1))
	        {
	            item = 1;
	        }
	        pCurrentMenuPage->currentItem = item;
	        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
        break;
		case MENU_EVENT_PREV_ITEM:
	        item--;
	        if(item < 1)
	        {
	            item = pCurrentMenuPage->menuTtemSize-1;
	        }
	        if(item > (pCurrentMenuPage->menuTtemSize-1))
	        {
	            item = 1;
	        }
	        pCurrentMenuPage->currentItem = item;
	        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
        break;
		case MENU_EVENT_DRAW_ITEM:
			menuResetMainTimeout();
			SetCurrentIsEnterNumberFlag(0);
			LCD_Clear();
			switch(item)
			{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:	
				{
					SnPassword = 0;
					SetKeyLedMode(KEY_MENU,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_SPLIT,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_SAVE,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_LOAD,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_SCALE,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_M1,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_M2,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_M3,LED_OFF_ALWAYS);
					SetKeyLedMode(KEY_M4,LED_OFF_ALWAYS);
					//SetKeyLedMode(KEY_LAYER_A,LED_OFF_ALWAYS);
					//SetKeyLedMode(KEY_LAYER_B,LED_OFF_ALWAYS);
					//SetKeyLedMode(KEY_LAYER_C,LED_OFF_ALWAYS);
					//SetKeyLedMode(KEY_LAYER_D,LED_OFF_ALWAYS);
					SetInputPortLedStatus(GetPgmCurrentInput(),GetPrvCurrentInput(),GetPgmCurrentDisplayMode());
					if(GetPgmCurrentDisplayMode() == _OSD_DM_4P)
					{
						sprintf(buffer, "%-2s               %-2s", "A-","B-");
						LCD_WriteLine1_L(buffer);
						sprintf(buffer, "%-2s               %-2s", "C-","D-");
						LCD_WriteLine2_L(buffer);
					}
					else if(GetPgmCurrentDisplayMode() == _OSD_DM_2P_PIP)
					{
						sprintf(buffer, "%-5s %-8s", "A:",\
							inputSourceStr_List[InputSourceConv(GetPgmCurrentInput()&0x07)]);
						LCD_WriteLine1_L(buffer);
						sprintf(buffer, "%-5s %-8s", "B:",\
						inputSourceStr_List[InputSourceConv(GetPrvCurrentInput()&0x07)]
						   );
						LCD_WriteLine2_L(buffer);
					}
					else
					{
						sprintf(buffer, "%-5s %-8s", "PGM:",\
							inputSourceStr_List[InputSourceConv(GetPgmCurrentInput()&0x07)]);
						LCD_WriteLine1_L(buffer);

						if( 
							GetPgmCurrentDisplayMode() == _OSD_DM_2P_LR ||\
							GetPgmCurrentDisplayMode() == _OSD_DM_2P_TB)
							sprintf(buffer, "%-5s %-8s", "PRV:",
							inputSourceStr_List[InputSourceConv(GetPrvCurrentInput()&0x07)]
						   );
						else
							sprintf(buffer, "%s", "                                ");
						LCD_WriteLine2_L(buffer);
					}

					DisplayInputInfo();
					scanStatusAndRefleshIconList();
					dispIconList();
					
					switch(GetPgmCurrentDisplayMode())
					{
						case _OSD_DM_1P:
						case _OSD_DM_2P_SACLE_LR:
						case _OSD_DM_2P_SACLE_TB:
						case _OSD_DM_4P:
						case _OSD_DM_2P_PIP:
							sprintf(buffer, "%-5s %s %s", "OUT:",valueToString(outputFormatIndex,getValue(outputFormatIndex)),"x 4");
						break;
						case _OSD_DM_3P_SACLE_TB:
							sprintf(buffer, "%-5s %s %s", "OUT:",valueToString(outputFormatIndex,getValue(outputFormatIndex)),"x 3");
						break;
						case _OSD_DM_2P_LR:
						case _OSD_DM_2P_TB:
						case _OSD_DM_2P_TOW_PORT_TB:
						case _OSD_DM_2P_TOW_PORT_LR:
							sprintf(buffer, "%-5s %s %s", "OUT:",valueToString(outputFormatIndex,getValue(outputFormatIndex)),"x 2");
						break;
					}
				
					LCD_WriteLine4_L(buffer);
				}
				break;
			}
		break;
		default:
			defaultMenuPageProcess(type,value);
		break;
	}
}

u8 preBarValue = 0xff;

#define PROGRESS_BAR_START_X   (0)
#define PROGRESS_BAR_END_X     (60)
#define PROGRESS_BAR_START_Y   (32)
#define PROGRESS_BAR_END_Y     (40)

void manuDrawProgressBar(int percent)
{
	static char preValue = 0;
	int total;
	if((percent < 0)||(percent > 100))
	{
		return;
	}
	if(percent == 0)
	{
		LCD_ClearRect(PROGRESS_BAR_START_X,PROGRESS_BAR_END_X,PROGRESS_BAR_START_Y,PROGRESS_BAR_END_Y);
		preValue = PROGRESS_BAR_START_X;
		return ;
	}
	total = PROGRESS_BAR_END_X - PROGRESS_BAR_START_X;
	percent = percent*total/100 + PROGRESS_BAR_START_X;
	if(percent == preValue)
	{
		return;
	}

	LCD_DrawFillRect(preValue,percent,PROGRESS_BAR_START_Y,PROGRESS_BAR_END_Y);
	preValue = percent;
}



static char griDPreValue = 0;
void manuDrawGridProgressBar(int percent)
{
	int total;
	if((percent < 0)||(percent > 100))
	{
		return;
	}
	if(percent == 0)
	{
		LCD_ClearRect(PROGRESS_BAR_START_X,PROGRESS_BAR_END_X,PROGRESS_BAR_START_Y,PROGRESS_BAR_END_Y);
		griDPreValue = PROGRESS_BAR_START_X;
		return ;
	}
	total = PROGRESS_BAR_END_X - PROGRESS_BAR_START_X;
	percent = percent*total/100 + PROGRESS_BAR_START_X;
	if(percent == griDPreValue)
	{
		return;
	}

	LCD_DrawFillRect(griDPreValue,percent,PROGRESS_BAR_START_Y,PROGRESS_BAR_END_Y);
	griDPreValue = percent;
}



void firmwareUpdatePageProcess(int type, int value)
{
	char buffer[40];
	const MenuPage *menuPage;
	//int item;

	//item = pCurrentMenuPage->currentItem;
	menuPage = pCurrentMenuPage->menuPage;
	switch(type)
	{
	case MENU_EVENT_PROGRESS_BAR:
		//if(value != preBarValue)
		{
            if(menuIsCurrentPageIndex(MENU_INDEX_ARTNET_TEST))
            {
    			sprintf(buffer, "%3d", value);
    			LCD_WriteStr(1, 0, buffer);
    			preBarValue = value; 
                value = (value * 100) / 255;
                if(griDPreValue > value)
                {
                    manuDrawGridProgressBar(0);
                }
                manuDrawGridProgressBar(value);
            }
            else
            {
    			sprintf(buffer, "%2d%%", value);
    			LCD_WriteStr(1, 0, buffer);
    			preBarValue = value;  
                manuDrawProgressBar(value);
            }			
		}
		break;
	case MENU_EVENT_ENTER:	
		lang_SetLang(LANG_ENGLISH);
		LCD_Clear();
		preBarValue = 0xff;
		defaultMenuPageProcess(type,value);
		break;
	case MENU_EVENT_EXIT:
		//menuRemoveMainFunc(menuBusyTimerTask);
		lang_SetLang((Lang_TypeDef)getUserLang());
		break;
	case MENU_KEY_MENU:
	case MENU_KEY_ESC:
		defaultMenuPageProcess(type,value);
		break;

	case MENU_EVENT_DRAW_ITEM:
		if(menuPage[value].type == MENU_TYPE_LABEL)
        {
			if(menuPage[value].str != spaceStr)
			{
	        	if(strlen(langGetStr(menuPage[value].str)) > 18)
	        	{
	        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
	        	}
				else
				{
	            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
				}
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			else
			{
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, "");
			}
        }
		break;
		
	default:
        if(menuIsCurrentPageIndex(MENU_INDEX_ARTNET_TEST))
        {
            defaultMenuPageProcess(type,value);
        }
        else
        {
    		if(type > MENU_KEY_SIZE)
    		{
    			defaultMenuPageProcess(type,value);
    		}
        }
		// nothing
		break;
	}
}
void changeNoSetProcess(int type, int value)
{
    char buffer[40];
    const MenuPage *menuPage;
    int item;
	char fb = 0;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
    switch(type)
    {
    case MENU_EVENT_NEXT_VALUE:
        if(menuPage[item].type == MENU_TYPE_VALUE)
        {
            int index;
            index = menuPage[item].value;
            menuState = MENU_STATE_CHANGE_VALUE;
            menuValueList[item] = nextValue(index,menuValueList[item]);
			if(menuIsCurrentPageIndex(MENU_INDEX_HOT_BACKUP))
			{
				if(menuPage[item].value == hotBackupEnableValue)
				{
					GetSystemParam()->hotBsckup.hotBackupEn = menuValueList[item];
				}
			}
			
            menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
        }
        break;
    case MENU_EVENT_PREV_VALUE:
        if(menuPage[item].type == MENU_TYPE_VALUE)
        {
            int index;
            index = menuPage[item].value;
            menuState = MENU_STATE_CHANGE_VALUE;
            menuValueList[item] = prevValue(index,menuValueList[item]);
            menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
        }
        break;
    case MENU_EVENT_SELECT_ITEM:
        if(menuState == MENU_STATE_CHANGE_VALUE)
        {
            setValue(menuPage[item].value, menuValueList[item]);
            menuState = MENU_STATE_SET_VALUE;
        }
        menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
        break;
	case MENU_EVENT_EXIT:
        defaultMenuPageProcess(type, value);
		break;
	case MENU_EVENT_DRAW_ITEM:
		if(menuIsCurrentPageIndex(MENU_INDEX_DISPLAY_MODE))
		{
        	if((menuPage[value].type == MENU_TYPE_VALUE))
	        {
	        	if(strlen(valueToString(menuPage[value].value,menuValueList[value])) == 10)
	        	{
					//printf("strlen == 10\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-18s%10s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-18s%10s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 14)
				{
					//printf("strlen > 14\r\n");
					if((DEVICE_TYPE == 0x198)&&(menuIsCurrentPageIndex(MENU_INDEX_TRANSITION)) )
					{
						if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%-8s%20s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%-8s%20s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
					}
					else
					{
						if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%s %s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%s %s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
					}
				}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 8)
	        	{
					//printf("strlen > 8\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-15s%13s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-15s%13s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 6)
				{
					//printf("strlen > 6\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-20s%8s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-20s%8s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) < 4)
				{
					//printf("strlen < 4\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-24s%4s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-24s%4s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else
				{
					//printf("strlen else\r\n");
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-22s%6s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-22s%6s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
	        }
			if(	fb )
			{
				LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			else
			{
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			if(pCurrentMenuPage->menuTtemSize < 3)
			{
				LcdFillBlock_clear(0x3F,0x3F,0,64);
			}
			break;
		}
    default:
        defaultMenuPageProcess(type, value);
        break;
    }
}


int getCustomValue(int index)// index = 1~3
{
	int res = 0;
	switch(index)
	{
		case 1: //h
			res = getValue(customHValue); 
			break;
		case 2: //v
			res = getValue(customVValue); 
			break;
		case 3: //f
			res = getValue(customFValue); 
			break;
		case 4: //edid h
			res = getValue(customEDIDHValue); 
			break;
		case 5: //edid v
			res = getValue(customEDIDVValue); 
			break;
		case 6: //edid f
			res = getValue(customEDIDFValue); 
			break;
		
		default:
			break;		
	}
	return res;
}

VideoDefine_Struct m_customVideo;  
Resolution_Struct m_customResolution;

u8 getCustomTimming(int h_pixels,int v_lines,int freq)
{
	// CONSTANTS:
	const int CELL_GRAN_RND = 8; 
	const int MARGIN_PER = 2;
	const int MIN_V_PORCH_RND = 3;
	const int INT_RQD = 0;
	//const int IP_FREQ_RQD = 60;
	const int MARGINS_RQD = 0;
	//const int H_PIXELS = 1024;
	//const int V_LINES = 768;
	
	// COMMON TIMING PARAMETERS:
	int V_FIELD_RATE_RQD, H_PIXELS_RND, LEFT_MARGIN = 0, RIGHT_MARGIN = 0;
	int TOTAL_ACTIVE_PIXELS, V_LINES_RND, TOP_MARGIN, BOT_MARGIN;
	float INTERLACE;

	// Computation of CVT Timing Parameters
	// CONSTANTS
	//const int V_SYNC_RND = 3;
	const int MIN_VSYNC_BP = 550;
	//const float C_PRIME = 30;
	//const float M_PRIME = 300;

	float H_PERIOD_EST; // (kHz)	
	float TOTAL_V_LINES;
	//float V_FIELD_RATE_EST, IDEAL_DUTY_CYCLE;
	int V_BACK_PORCH;// V_SYNC_BP, 
	//int H_BLANK;
	int TOTAL_PIXELS;
	float ACT_PIXEL_FREQ; // (MHz)
	float ACT_H_FREQ; // (kHz)
	//float ACT_FIELD_RATE; // (Hz)
	float ACT_FRAME_RATE; // (Hz)

	// Computation of Reduced Blanking Timing Parameters
	const float RB_MIN_V_BLANK = 460;
	const int   RB_H_BLANK = 160;
	const int RB_H_SYNC = 32;
	const int RB_V_SYNC = 4;
	const int RB_V_FPORCH = 3;

	int VBI_LINES;
	int H_BACK_PORCH;

	// Computation of Common Parameters
	V_FIELD_RATE_RQD = (INT_RQD)?(freq*2):(freq);//(INT_RQD)?(IP_FREQ_RQD*2):(IP_FREQ_RQD);
	H_PIXELS_RND = h_pixels/CELL_GRAN_RND*CELL_GRAN_RND;//H_PIXELS/CELL_GRAN_RND*CELL_GRAN_RND;

	if(MARGINS_RQD)
	{
		LEFT_MARGIN = (H_PIXELS_RND*MARGIN_PER/100/CELL_GRAN_RND*CELL_GRAN_RND);
	}
	else
	{
		LEFT_MARGIN = 0;
	}
	RIGHT_MARGIN = LEFT_MARGIN;

	TOTAL_ACTIVE_PIXELS = H_PIXELS_RND + LEFT_MARGIN + RIGHT_MARGIN;

	V_LINES_RND = (INT_RQD)?(v_lines/2):(v_lines);//(INT_RQD)?(V_LINES/2):(V_LINES);

	if(MARGINS_RQD)
	{
		TOP_MARGIN = v_lines*MARGIN_PER/100;//V_LINES*MARGIN_PER/100;
	}
	else
	{
		TOP_MARGIN = 0;
	}
	BOT_MARGIN = TOP_MARGIN;

	INTERLACE = (INT_RQD)?0.5:0;

	// Computation of CVT Timing Parameters
	H_PERIOD_EST = ((1.0/V_FIELD_RATE_RQD)-MIN_VSYNC_BP/1000000);
	H_PERIOD_EST = H_PERIOD_EST/(V_LINES_RND+(2*TOP_MARGIN)+MIN_V_PORCH_RND+INTERLACE)*1000000;
	
	//V_SYNC_BP = MIN_VSYNC_BP/H_PERIOD_EST;
	//V_BACK_PORCH = V_SYNC_BP-V_SYNC_RND;
	//TOTAL_V_LINES = V_LINES_RND+TOP_MARGIN+BOT_MARGIN+V_SYNC_BP;
	//printf("V Total = %f\n", TOTAL_V_LINES);
	//TOTAL_V_LINES = TOTAL_V_LINES+INTERLACE+MIN_V_PORCH_RND;
	//V_FIELD_RATE_EST = 1000000/(H_PERIOD_EST*TOTAL_V_LINES);
	//IDEAL_DUTY_CYCLE = C_PRIME-(M_PRIME*H_PERIOD_EST/1000); // (%)

	//if(IDEAL_DUTY_CYCLE < 0.2)
	//{
	//	H_BLANK = TOTAL_ACTIVE_PIXELS*20/(100-20)/(2*CELL_GRAN_RND);
	//	H_BLANK = H_BLANK*(2*CELL_GRAN_RND);
	//}
	//else
	//{
	//	H_BLANK = TOTAL_ACTIVE_PIXELS*IDEAL_DUTY_CYCLE/(100-IDEAL_DUTY_CYCLE)/(2*CELL_GRAN_RND);
	//	H_BLANK = H_BLANK*(2*CELL_GRAN_RND);
	//}

	//TOTAL_PIXELS = TOTAL_ACTIVE_PIXELS + H_BLANK;
	//ACT_PIXEL_FREQ = ((int)(TOTAL_PIXELS/H_PERIOD_EST/0.125))*0.125;
	//ACT_H_FREQ = 1000*ACT_PIXEL_FREQ/TOTAL_PIXELS;
	//ACT_FIELD_RATE = 1000*ACT_H_FREQ/TOTAL_V_LINES;
	//ACT_FRAME_RATE = (INT_RQD)?ACT_FIELD_RATE/2:ACT_FIELD_RATE;

	//printf("Pixel Clock (MHz) = %f\n", ACT_PIXEL_FREQ);
	//printf("Frame Rate (Hz) = %f\n", ACT_FRAME_RATE);
	//printf("Line Freq (kHz) = %f\n", ACT_H_FREQ);
	//printf("H Total = %d\n", TOTAL_PIXELS);
	//printf("V Total = %f\n", TOTAL_V_LINES);

	// Computation of Reduced Blanking Timing Parameters
	H_PERIOD_EST = ((1000000.0/V_FIELD_RATE_RQD)-RB_MIN_V_BLANK)/(V_LINES_RND+TOP_MARGIN+BOT_MARGIN);

	VBI_LINES = (RB_MIN_V_BLANK+H_PERIOD_EST-1)/H_PERIOD_EST;

	TOTAL_V_LINES = VBI_LINES + V_LINES_RND+TOP_MARGIN+BOT_MARGIN;

	TOTAL_PIXELS = RB_H_BLANK + TOTAL_ACTIVE_PIXELS;

	ACT_PIXEL_FREQ = ((int)(V_FIELD_RATE_RQD*TOTAL_V_LINES*TOTAL_PIXELS)/1000000/0.125)*0.125;

	ACT_H_FREQ = 1000*ACT_PIXEL_FREQ/TOTAL_PIXELS;
	ACT_FRAME_RATE = (1000000*ACT_PIXEL_FREQ/(TOTAL_V_LINES*TOTAL_PIXELS));

	if(INT_RQD)
	{
		ACT_FRAME_RATE = ACT_FRAME_RATE/2;
	}
	H_BACK_PORCH = RB_H_BLANK/2;
	V_BACK_PORCH = VBI_LINES - RB_V_FPORCH - RB_V_SYNC;

	m_customVideo.htotal = TOTAL_PIXELS;
	m_customVideo.vtotal = (int)TOTAL_V_LINES;
	m_customVideo.hstart = RB_H_SYNC+H_BACK_PORCH;
	m_customVideo.vstart = RB_V_SYNC+V_BACK_PORCH;
	m_customVideo.hSyncWidth = RB_H_SYNC;
	m_customVideo.vSyncWidth = RB_V_SYNC;
	m_customVideo.vfreq = freq;
	m_customVideo.pClk = ACT_PIXEL_FREQ;
	m_customVideo.hvSyncPolarity = 0; //默认正极性
	m_customVideo.hActive = h_pixels;
	m_customVideo.vActive = v_lines;
	m_customResolution.DisplayWidth = h_pixels;
	m_customResolution.DisplayHeight = v_lines; 

	#if 1
	printf("\nReduced Blanking Timing\r\n");
	printf("Pixel Clock (MHz) = %f\r\n", ACT_PIXEL_FREQ);
	printf("Frame Rate (Hz) = %f\r\n", ACT_FRAME_RATE);
	printf("Line Freq (kHz) = %f\r\n", ACT_H_FREQ);
	printf("H Total  = %d\r\n", TOTAL_PIXELS);
	printf("V Total  = %d\r\n", (int)TOTAL_V_LINES);
	printf("H Active = %d\r\n", h_pixels);//H_PIXELS);
	printf("V Active = %d\r\n", v_lines);//V_LINES);
	printf("H Sync Width = %d\r\n", RB_H_SYNC);
	printf("V Sync Width = %d\r\n", RB_V_SYNC);
	printf("H Back Porch = %d\r\n", H_BACK_PORCH);
	printf("V Back Porch = %d\r\n", V_BACK_PORCH);
	printf("H Start = %d\r\n", RB_H_SYNC+H_BACK_PORCH);	
	printf("V Start = %d\r\n", RB_V_SYNC+V_BACK_PORCH);
	#endif
	return TRUE;
}


void setCustomParameter(void)
{
//	rgb_SetCustomParameter(0,m_customVideo.htotal); //htotal
//	rgb_SetCustomParameter(1,m_customVideo.vtotal); //vtotal
//	rgb_SetCustomParameter(2,m_customVideo.hstart); //hstart
//	rgb_SetCustomParameter(3,m_customVideo.vstart); //vstart
//	rgb_SetCustomParameter(4,m_customVideo.vfreq); //vfreq
//	rgb_SetCustomParameter(5,m_customVideo.hSyncWidth); //hSyncWidth
//	rgb_SetCustomParameter(6,m_customVideo.vSyncWidth); //vSyncWidth
//	rgb_SetCustomParameter(7,m_customVideo.hvSyncPolarity); //hvSyncPolarity
//	rgb_SetCustomParameter(8,m_customResolution.DisplayWidth); //DisplayWidth
//	rgb_SetCustomParameter(9,m_customResolution.DisplayHeight); //DisplayHeight
}


#include "IDT5P49V5901.h"
extern VideoDefine_Struct tVideoDefineCustom;
//extern void Set5901FreqIndependent(IDT5P49V5901_TypeDef IDTDevice,u64 Fout,u8 port);
void setCustomFormat(void)
{
	printf("MD_CUSTOM_%dx%dx%d\r\n",getValue(customHValue),getValue(customVValue),getValue(customFValue));
	if(getCustomTimming(getValue(customHValue),getValue(customVValue),getValue(customFValue)))
	{
		if(m_customVideo.pClk > 180)
		{
			LCD_Clear();
			if(GetLanguage() == 1)
			{
				LCD_WriteLine1("            时钟过高");
				LCD_WriteLine2("          请尝试降低帧频");
				LCD_WriteLine3("        或者减小有效宽,高");
			}
			else
			{
				LCD_WriteLine1("      The clock is too high !");
				LCD_WriteLine2("  Please try to reduce the frame");
				LCD_WriteLine3("  frequency Or reduces the ");
				LCD_WriteLine4("  effective width, the height.");
			}
			SysDelay1ms(3000);
			return ;
		}
		memcpy((u8*)&tVideoDefineCustom,(u8*)&m_customVideo,sizeof(VideoDefine_Struct));
		tVideoDefineCustom.pClk *= 1000000;
		memcpy((u8*)&(GetPanleAndSplitParamRam()->stCustomFormat),(u8*)&tVideoDefineCustom,sizeof(VideoDefine_Struct));
		SaveCustomFormatParam();
		Set5901FreqIndependent(IDT5P49V5901_1,tVideoDefineCustom.pClk,2);
		SetFpgaOutputFormat(18,1);
	}
}

int getCustomHTotal(void)
{
	return m_customVideo.htotal;
}
int getCustomVTotal(void)
{
	return m_customVideo.vtotal;
}
int getCustomHStart(void)
{
	return m_customVideo.hstart;
}
int getCustomVStart(void)
{
	return m_customVideo.vstart;
}
int getCustomVFreq(void)
{
	return m_customVideo.vfreq;
}
int getCustomHSyncWidth(void)
{
	return m_customVideo.hSyncWidth;
}
int getCustomVSyncWidth(void)
{
	return m_customVideo.vSyncWidth;
}
int getCustomHVSyncPolarity(void)
{
	return m_customVideo.hvSyncPolarity;
}
int getCustomDisplayWidth(void)
{
	return m_customResolution.DisplayWidth;
}
int getCustomDisplayHeight(void)
{
	return m_customResolution.DisplayHeight;
}



void setCustomHTotal(int value)
{
	m_customVideo.htotal = value;
}


void setCustomVTotal(int value)
{
	m_customVideo.vtotal = value;
}


void setCustomHStart(int value)
{
	m_customVideo.hstart = value;
}


void setCustomVStart(int value)
{
	m_customVideo.vstart = value;
}


void setCustomVFreq(int value)
{
	GetPanleAndSplitParamRam()->stCustomFormat.vfreq = value;
}


void setCustomHSyncWidth(int value)
{
	m_customVideo.hSyncWidth = value;
}


void setCustomVSyncWidth(int value)
{
	m_customVideo.vSyncWidth = value;
}


void setCustomHVSyncPolarity(int value)
{
	m_customVideo.hvSyncPolarity = value;
}


void setCustomDisplayWidth(int value)
{
	GetPanleAndSplitParamRam()->stCustomFormat.hActive = value;
}


void setCustomDisplayHeight(int value)
{
	GetPanleAndSplitParamRam()->stCustomFormat.vActive = value;
}



void setm_customVideo(int type,int value)
{
	WORD *p_word = &m_customVideo.htotal;
	BYTE *p_byte = &m_customVideo.vfreq;
	
	if((type > 7)||(type < 0))
	{
		return;
	}
	if(type < 4)
	{
		*(p_word + type) = value;
	}
	else
	{
		*(p_byte + type - 4) = (BYTE)value;
	}
}


void setm_customResolution(int type,int value)
{
	WORD *p_word = &m_customResolution.DisplayWidth;
	if((type > 2)||(type < 0))
	{
		return;
	}
	*(p_word + type) = value;
}



void setCustomValue(void)
{
//	char buffer[40];
	setCustomBeyondFlag = 0;
	printf("custom resolution---%dx%d@%d\n",dispValue[0],dispValue[1],dispValue[2]);
	if(getCustomTimming(dispValue[0],dispValue[1],dispValue[2]))
	{
		setValue(customHValue,dispValue[0]); //*(subValue++)
		setValue(customVValue,dispValue[1]); //*(subValue++)
		setValue(customFValue,dispValue[2]); //*(subValue++)
		initCustomByComm();
	}
	else
	{
		setCustomBeyondFlag = 1;	
	}
}



static CUSTOM_RESOLUTION customByComm;
void initCustomByComm(void)
{
//	customByComm.HorizontalPixel = 	getCustomH();
//	customByComm.VerticalPixel = 	getCustomV();
//	customByComm.Frequency = 		getCustomF();
}



void setCustomByComm(int type, int value)
{
	//printf("setCustomByComm(%d,%d)\r\n",type,value);
	switch(type)
	{
		case 0x00:
			customByComm.HorizontalPixel = value;
			break;
		case 0x01:
			customByComm.VerticalPixel = value;
			break;
		case 0x02:
			customByComm.Frequency = value;
			break;
		default:
			break;
	}
}


int getCustomByComm(int type)
{
	switch(type)
	{
		case 0x00:
			return customByComm.HorizontalPixel;
		case 0x01:
			return customByComm.VerticalPixel;
		case 0x02:
			return customByComm.Frequency;
		default:
			return 0;
	}
}



int setCustomResolution(void)
{
	if(getCustomTimming(customByComm.HorizontalPixel,customByComm.VerticalPixel,customByComm.Frequency))
	{
		setValue(customHValue,customByComm.HorizontalPixel); //*(subValue++)
		setValue(customVValue,customByComm.VerticalPixel); //*(subValue++)
		setValue(customFValue,customByComm.Frequency); //*(subValue++)
		setCustomParameter();
		SetOutputFormat(MD_CUSTOM);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



#define ENTER_CUSTOM_FORMAT_SIZE (3)
int getDispCustomValue(int index)// index = 0~2
{
	return dispValue[index];
}


void refleshDispVslue(void)
{
	dispValue[0] = getCustomValue(1);
	dispValue[1] = getCustomValue(2);
	dispValue[2] = getCustomValue(3);	
	return;
}


char ifCustomFormat(void)
{
//	if(GetUserSetting()->outputFormat == MD_CUSTOM)
	{
		return TRUE;
	}
	return FALSE;

}


char ifStandardFormatPage(void)
{
	return menuIsCurrentPageIndex(MENU_INDEX_STANDARD_FORMAT);
}

extern unsigned char step;
void customFormatPageProcess(int type, int value)
{
	const MenuPage *menuPage = pCurrentMenuPage->menuPage;
    int item = pCurrentMenuPage->currentItem;
	char buffer[40],buffer1[40],buffer2[40];
	menuPage = menuPage;
	item = item;
	switch(type)
	{
	case MENU_EVENT_ENTER:
		LCD_Clear();
		menuClearItemBar();
		menuSubItem = 0;
		refleshDispEDIDValue();
		#ifdef ENTER_NUMBER_KEY
		CurrentIsEnterNumberFlag = 1;
		#endif
		pCurrentMenuPage->currentItem = 2;
		if(setCustomBeyondFlag)
		{
			pCurrentMenuPage->menuTtemSize = 4;
		}
		else
		{
			pCurrentMenuPage->menuTtemSize = 3;
		}

		if(GetLanguage())
		{
			sprintf(buffer, "%s","自定义分辨率");
			LCD_WriteLine1(buffer);
			LCD_WriteLine2("宽度");
			LCD_WriteLine3("高度");
			LCD_WriteLine4("频率");
		}
		else
		{
			sprintf(buffer, "%s","Customized format");
			LCD_WriteLine1(buffer);
			LCD_WriteLine2("hWidth");
			LCD_WriteLine3("vHeight");
			LCD_WriteLine4("vFreq");
		}

		menuValueList[1] = 0;//GetPanleAndSplitParamRam()->stCustomFormat.hActive;
		menuValueList[2] = 0;//GetPanleAndSplitParamRam()->stCustomFormat.vActive;
		menuValueList[3] = 0;//GetPanleAndSplitParamRam()->stCustomFormat.vfreq;
		menuState = MENU_STATE_CHANGE_ITEM;
		menuValueList[0] = 1; // 数字显示方式
		pCurrentMenuPage->currentItem = 1;
		menuAddMainFunc(500,enterPasswordTask);
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
	case MENU_EVENT_EXIT:
		menuSubItem = 0;
		CurrentIsEnterNumberFlag = 0;
		setCustomBeyondFlag = 0;
		menuRemoveMainFunc(enterPasswordTask);	
		break;
	case MENU_KEY_MENU:
		menuSubItem = 0;
		menuRemoveMainFunc(enterPasswordTask);
        defaultMenuPageProcess(type,value);
        break;
	case MENU_KEY_NEXT:
		if(item >= 3)
        {
// 			int i;
			if(menuIsCurrentPageIndex(MENU_INDEX_CUSTOM_FORMAT) == TRUE)
			{
				setValue(customHValue,cusFormatWidth); 
				setValue(customVValue,cusFormatHeight);
				setValue(customFValue,cusFormatFreq);
				setCustomFormat();
				menuEnterPage(MENU_INDEX_OUTPUT);
			}
        }
		else
		{
			menuPage->pfProcess(MENU_KEY_RIGHT, 0);
		}
        break;

	  	case MENU_KEY_LEFT:
			item--;
			if((item < 1) || (item > 10))
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
			break;
		case MENU_KEY_RIGHT:
			item++;
			if((item < 1) || (item > 3))
			{
				item = 3;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
			break;				


	case MENU_KEY_UP:
		menuValueList[item]++;
		if((menuValueList[item]<0) || (menuValueList[item]>3840))
		{
			menuValueList[item] = 3840;
		}
		menuResetMainTimeout();
		menuValueList[0] = 1;
		if(pCurrentMenuPage->currentItem == 1)
		{
			cusFormatWidth = menuValueList[pCurrentMenuPage->currentItem];
			cusFormatHeight = 0;
			cusFormatFreq = 0;
			cusFormatWidth+=step;
			if(cusFormatWidth > 3840)
			{
				cusFormatWidth = 3840;
			}
		}
		else if(pCurrentMenuPage->currentItem == 2)
		{
			cusFormatHeight = menuValueList[pCurrentMenuPage->currentItem];
			cusFormatHeight += step;
			if(cusFormatHeight > 2160)
			{
				cusFormatHeight = 2160;
			}
		}
		else if(pCurrentMenuPage->currentItem == 3)
		{
			cusFormatFreq = menuValueList[pCurrentMenuPage->currentItem];
			cusFormatFreq+=step;
			if(cusFormatFreq > 60)
			{
				cusFormatFreq = 60;
			}
		}
		
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		
		break;
	case MENU_KEY_DOWN:
		menuValueList[item]--;
		if((menuValueList[item]<0) || (menuValueList[item]>3840))
		{
			menuValueList[item] = 0;
		}


		if(pCurrentMenuPage->currentItem == 1)
		{
			cusFormatWidth = menuValueList[pCurrentMenuPage->currentItem];
			cusFormatHeight = 0;
			cusFormatFreq = 0;
			cusFormatWidth-=step;
			if(cusFormatWidth < 0)
			{
				cusFormatWidth = 3840;
			}
		}
		else if(pCurrentMenuPage->currentItem == 2)
		{
			cusFormatHeight = menuValueList[pCurrentMenuPage->currentItem];
			cusFormatHeight-=step;
			if(cusFormatHeight < 0)
			{
				cusFormatHeight = 2160;
			}
		}
		else if(pCurrentMenuPage->currentItem == 3)
		{
			cusFormatFreq = menuValueList[pCurrentMenuPage->currentItem];
			cusFormatFreq-=step;
			if(cusFormatFreq < 0)
			{
				cusFormatFreq = 60;
			}
		}
		
		menuResetMainTimeout();
		menuValueList[0] = 1;
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;


	case MENU_EVENT_DRAW_ITEM:
		if(pCurrentMenuPage->currentItem == 1)
		{
			cusFormatWidth = menuValueList[pCurrentMenuPage->currentItem];
			cusFormatHeight = 0;
			cusFormatFreq = 0;
			if(cusFormatWidth > 3840)
			{
				cusFormatWidth = 3840;
			}
		}
		else if(pCurrentMenuPage->currentItem == 2)
		{
			cusFormatHeight = menuValueList[pCurrentMenuPage->currentItem];
			if(cusFormatHeight > 2160)
			{
				cusFormatHeight = 2160;
			}
		}
		else if(pCurrentMenuPage->currentItem == 3)
		{
			cusFormatFreq = menuValueList[pCurrentMenuPage->currentItem];
			if(cusFormatFreq > 60)
			{
				cusFormatFreq = 60;
			}
		}
		sprintf(buffer, 	"%04d",cusFormatWidth);	
		sprintf(buffer1, 	"%04d",cusFormatHeight);
		sprintf(buffer2, 	"%02d",cusFormatFreq);
		if(menuValueList[0])
		{

		}
		else
		{	
			switch(pCurrentMenuPage->currentItem)
			{
				case 1:
					buffer[0] = ' ';
					buffer[1] = ' ';
					buffer[2] = ' ';
					buffer[3] = ' ';
				break;
				case 2:
					buffer1[0] = ' ';
					buffer1[1] = ' ';
					buffer1[2] = ' ';
					buffer1[3] = ' ';
				break;
				case 3:
					buffer2[0] = ' ';
					buffer2[1] = ' ';
				break;
				default:
				break;
			}		
		}
		LCD_WriteStr(1,4,buffer);
		LCD_WriteStr(2,4,buffer1);
		LCD_WriteStr(3,4,buffer2);
		break;
	default:
		menuState = MENU_STATE_CHANGE_VALUE;
		enterNumberPageProcess(type,value);
		break;
	}

}




char setCustomEDIDValue(int h_pixels,int v_lines,int freq)
{
//	char buffer[40];
	printf("custom edid resolution---%dx%d@%d\n",h_pixels,v_lines,freq);
	//menuSetBusy(2,11);			
	//cal and sent to 8028
	if(getCustomTimming(h_pixels,v_lines,freq))
	{
		setValue(customEDIDHValue,h_pixels); //*(subValue++)
		setValue(customEDIDVValue,v_lines); //*(subValue++)
		setValue(customEDIDFValue,freq); //*(subValue++)
		initCustomByComm();
		editCustomEDID(m_customVideo,m_customResolution,getEDIDDesPort());
		return TRUE;
	}
	else
	{
		menuSetBusy(2,11);
		setCustomBeyondFlag = 1;
		SysDelay1ms(500);
		menuDisableBusy();
		LCD_WriteLine3(langGetStr(studyEDIDFailStr));
		SysDelay1ms(1500);
		return FALSE;
	}
	//getCustomTimming(getValue(customHValue),getValue(customVValue),getValue(customFValue));
}

void refleshDispEDIDValue(void)
{
	dispValue[0] = getCustomValue(4);
	dispValue[1] = getCustomValue(5);
	dispValue[2] = getCustomValue(6);	
	return;
}



void customEDIDPageProcess(int type, int value)
{
	const MenuPage *menuPage = pCurrentMenuPage->menuPage;
    int item = pCurrentMenuPage->currentItem;
	char buffer[40],buffer1[40],buffer2[40];
	menuPage = menuPage;
	item = item;
	switch(type)
	{
	case MENU_EVENT_ENTER:
		LCD_Clear();
		menuClearItemBar();
		menuSubItem = 0;
		refleshDispEDIDValue();
		#ifdef ENTER_NUMBER_KEY
		CurrentIsEnterNumberFlag = 1;
		#endif
		pCurrentMenuPage->currentItem = 2;
		if(setCustomBeyondFlag)
		{
			pCurrentMenuPage->menuTtemSize = 4;
		}
		else
		{
			pCurrentMenuPage->menuTtemSize = 3;
		}
		if(getEDIDDesPort() == 0)
		{
			if(GetLanguage())
			{
				sprintf(buffer, "%s","自定义EDID     HDMI2.0 端口");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("宽度");
				LCD_WriteLine3("高度");
				LCD_WriteLine4("频率");
			}
			else
			{
				sprintf(buffer, "%s","Customized     HDMI2.0 PORT");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("hWidth");
				LCD_WriteLine3("vHeight");
				LCD_WriteLine4("vFreq");
			}
			
		}
		else if(getEDIDDesPort() == 1)
		{
			if(GetLanguage())
			{
				sprintf(buffer, "%s","自定义EDID     HDMI1.4 端口");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("宽度");
				LCD_WriteLine3("高度");
				LCD_WriteLine4("频率");
			}
			else
			{
				sprintf(buffer, "%s","Customized     HDMI1.4 PORT");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("hWidth");
				LCD_WriteLine3("vHeight");
				LCD_WriteLine4("vFreq");
			}
			
		}
		else if(getEDIDDesPort() == 2)
		{
			if(GetLanguage())
			{
				sprintf(buffer, "%s","自定义EDID          DP 端口");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("宽度");
				LCD_WriteLine3("高度");
				LCD_WriteLine4("频率");
			}
			else
			{
				sprintf(buffer, "%s","Customized          DP PORT");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("hWidth");
				LCD_WriteLine3("vHeight");
				LCD_WriteLine4("vFreq");
			}
			//LCD_WriteLine1(buffer);
		}
		else if(getEDIDDesPort() == 3)
		{
			if(GetLanguage())
			{
				sprintf(buffer, "%s","自定义EDID         DVI 端口");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("宽度");
				LCD_WriteLine3("高度");
				LCD_WriteLine4("频率");
			}
			else
			{
				sprintf(buffer, "%s","Customized         DVI PORT");
				LCD_WriteLine1(buffer);
				LCD_WriteLine2("hWidth");
				LCD_WriteLine3("vHeight");
				LCD_WriteLine4("vFreq");
			}
			
		}
		
		menuValueList[1] = cusEdidWidth;
		menuValueList[2] = cusEdidHeight;
		menuValueList[3] = cusEdidFreq;
		menuState = MENU_STATE_CHANGE_ITEM;

		menuValueList[0] = 1; // 数字显示方式
		pCurrentMenuPage->currentItem = 1;
		menuAddMainFunc(500,enterPasswordTask);
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
	case MENU_EVENT_EXIT:
		menuSubItem = 0;
		CurrentIsEnterNumberFlag = 0;
		setCustomBeyondFlag = 0;
		menuRemoveMainFunc(enterPasswordTask);	
		break;
	case MENU_KEY_MENU:
		menuSubItem = 0;
		menuRemoveMainFunc(enterPasswordTask);
        defaultMenuPageProcess(type,value);
        break;
	case MENU_KEY_NEXT:
		if(item >= 3)
        {
// 			int i;
			if(menuIsCurrentPageIndex(MENU_INDEX_CUSTOM_EDID) == TRUE)
			{
				setValue(customEDIDHValue,cusEdidWidth); 
				setValue(customEDIDVValue,cusEdidHeight);
				setValue(customEDIDFValue,cusEdidFreq);
	        	refleshDispEDIDValue();
				setCustomEDIDValue(cusEdidWidth,cusEdidHeight,cusEdidFreq);
				//menuRemoveMainFunc(enterPasswordTask);	
				//defaultMenuPageProcess(MENU_EVENT_SET_VALUE, value);
				//menuProcessEvent(MENU_INDEX_EDID_HDMI20_PORT);
			}
        }
		else
		{
			menuPage->pfProcess(MENU_KEY_RIGHT, 0);
		}
        break;

	  	case MENU_KEY_LEFT:
			item--;
			if((item < 1) || (item > 10))
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
			break;
		case MENU_KEY_RIGHT:
			//menuState = MENU_STATE_CHANGE_VALUE;
			item++;
			if((item < 1) || (item > 3))
			{
				item = 3;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
			break;				


	case MENU_KEY_UP:
		menuValueList[item]++;
		if((menuValueList[item]<0) || (menuValueList[item]>3840))
		{
			menuValueList[item] = 3840;
		}
		menuResetMainTimeout();
		menuValueList[0] = 1;

		if(pCurrentMenuPage->currentItem == 1)
		{
			cusEdidWidth = menuValueList[pCurrentMenuPage->currentItem];
			cusEdidHeight = 0;
			cusEdidFreq = 0;
			cusEdidWidth+=step;
			if(cusEdidWidth > 3840)
			{
				cusEdidWidth = 3840;
			}
		}
		else if(pCurrentMenuPage->currentItem == 2)
		{
			cusEdidHeight = menuValueList[pCurrentMenuPage->currentItem];
			cusEdidHeight += step;
			if(cusEdidHeight > 2160)
			{
				cusEdidHeight = 2160;
			}
		}
		else if(pCurrentMenuPage->currentItem == 3)
		{
			cusEdidFreq = menuValueList[pCurrentMenuPage->currentItem];
			cusEdidFreq+=step;
			if(cusEdidFreq > 60)
			{
				cusEdidFreq = 60;
			}
		}
		
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		
		break;
	case MENU_KEY_DOWN:
		menuValueList[item]--;
		if((menuValueList[item]<0) || (menuValueList[item]>3840))
		{
			menuValueList[item] = 0;
		}
		menuResetMainTimeout();
		menuValueList[0] = 1;

		if(pCurrentMenuPage->currentItem == 1)
		{
			cusEdidWidth = menuValueList[pCurrentMenuPage->currentItem];
			cusEdidHeight = 0;
			cusEdidFreq = 0;
			cusEdidWidth-=step;
			if(cusEdidWidth < 0)
			{
				cusEdidWidth = 3840;
			}
		}
		else if(pCurrentMenuPage->currentItem == 2)
		{
			cusEdidHeight = menuValueList[pCurrentMenuPage->currentItem];
			cusEdidHeight-=step;
			if(cusEdidHeight < 0)
			{
				cusEdidHeight = 2160;
			}
		}
		else if(pCurrentMenuPage->currentItem == 3)
		{
			cusEdidFreq = menuValueList[pCurrentMenuPage->currentItem];
			cusEdidFreq-=step;
			if(cusEdidFreq < 0)
			{
				cusEdidFreq = 60;
			}
		}

		
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;


	case MENU_EVENT_DRAW_ITEM:
		if(pCurrentMenuPage->currentItem == 1)
		{
			cusEdidWidth = menuValueList[pCurrentMenuPage->currentItem];
			if(cusEdidWidth > 3840)
			{
				cusEdidWidth = 3840;
			}
		}
		else if(pCurrentMenuPage->currentItem == 2)
		{
			cusEdidHeight = menuValueList[pCurrentMenuPage->currentItem];
			if(cusEdidHeight > 2160)
			{
				cusEdidHeight = 2160;
			}
		}
		else if(pCurrentMenuPage->currentItem == 3)
		{
			cusEdidFreq = menuValueList[pCurrentMenuPage->currentItem];
			if(cusEdidFreq > 60)
			{
				cusEdidFreq = 60;
			}
		}
		sprintf(buffer, 	"%04d",cusEdidWidth);	
		sprintf(buffer1, 	"%04d",cusEdidHeight);
		sprintf(buffer2, 	"%02d",cusEdidFreq);
		if(menuValueList[0])
		{

		}
		else
		{	
			switch(pCurrentMenuPage->currentItem)
			{
				case 1:
					buffer[0] = ' ';
					buffer[1] = ' ';
					buffer[2] = ' ';
					buffer[3] = ' ';
				break;
				case 2:
					buffer1[0] = ' ';
					buffer1[1] = ' ';
					buffer1[2] = ' ';
					buffer1[3] = ' ';
				break;
				case 3:
					buffer2[0] = ' ';
					buffer2[1] = ' ';
				break;
				default:
				break;
			}		
		}
		LCD_WriteStr(1,4,buffer);
		LCD_WriteStr(2,4,buffer1);
		LCD_WriteStr(3,4,buffer2);
		break;
	default:
		menuState = MENU_STATE_CHANGE_VALUE;
		enterNumberPageProcess(type,value);
		break;
	}

}



void standardFormatPageProcess(int type, int value)
{
    char buffer[40];
    const MenuPage *menuPage;
    int item;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

	//printf("outFormatPageProcess,type: %d\r\n",type);

    switch(type)
    {
    	case MENU_EVENT_ENTER:
			LCD_Clear();
			menuClearItemBar();
			pCurrentMenuPage->currentItem = 2;
			defaultMenuPageProcess(type, value);
			menuState = MENU_STATE_SET_VALUE;
			menuClearItemBar();
		break;
		case MENU_KEY_MENU:
			if((menuState == MENU_STATE_SELECT_ITEM) || (menuState == MENU_STATE_SET_VALUE))
			{
				menuState = MENU_STATE_CHANGE_ITEM;
			}
			defaultMenuPageProcess(type, value);
		break;
		
    	case MENU_EVENT_DRAW_ITEM:
	        if(menuPage[value].type == MENU_TYPE_VALUE)
	        {
	            if(value == item)
	            {
	            	sprintf(buffer, "%s%s", menuStateStr[menuState],valueToString(menuPage[value].value,menuValueList[value]));                
	            }           
	            else
	            {
	                sprintf(buffer, "%s%s", MENU_NO_SELECT,valueToString(menuPage[value].value,menuValueList[value]));
	            }
	        }
			else if(menuPage[value].type == MENU_TYPE_LABEL)
	        {
	        	if(strlen(langGetStr(menuPage[value].str)) > 18)
	        	{
	        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
	        	}
				else
				{
	            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
				}
	        }
	        LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
        break;

		case MENU_EVENT_SET_VALUE:
			setValue(menuPage[item].value, menuValueList[item]);
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
	    break;
	    default:
	        changeNoSetProcess(type, value);
	        break;
    }
}


//static u8 saveUserSettingIndex = 0;
//static u8 returnFromReplace = 0;
void saveToMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

	#ifdef ENTER_NUMBER_KEY
	type = changeToKeyNumber(type);
	//printf("Press key: 0x%x\r\n",type);
	#endif

    switch(type)
    {
	
		case MENU_KEY_SAVE:
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_STATUS);
		break;
		#ifdef ENTER_NUMBER_KEY
		case MENU_KEY_0:
		case MENU_KEY_1:
		case MENU_KEY_2:
		case MENU_KEY_3:
		case MENU_KEY_4:
		case MENU_KEY_5:
		case MENU_KEY_6:
		case MENU_KEY_7:
		case MENU_KEY_8:
		case MENU_KEY_9:
			menuState = MENU_STATE_SET_VALUE;
			if(type == MENU_KEY_0)
			{
				type = MENU_KEY_9+1;
			}
			menuValueList[item] = type - MENU_KEY_1;
			if(isUserSettingUsed_ram((UserSaveIndex_TypeDef)menuValueList[item])) //usersetting is not empty
			{
				printf("record %d is not empty\r\n",menuValueList[item]);
				saveUserSettingIndex = menuValueList[item];
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_SAVE_VERIFY);
				break;
			}
	        pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, item);
			//added bu julia for 1.57  save完成后退出到显示系统信息
			SysDelay1ms(1500);  
			menuProcessEvent(MENU_KEY_INFO);
        break;
		#endif
	    case MENU_KEY_SAVE1:
		case MENU_KEY_SAVE2:
		case MENU_KEY_SAVE3:
			menuState = MENU_STATE_SET_VALUE;
			menuValueList[item] = type-MENU_KEY_SAVE1;
	        pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, item);
			menuEnterPage(MENU_INDEX_NOTHING);
	    break;
	    case MENU_EVENT_EXIT:
		break;
	    case MENU_EVENT_ENTER:
			LCD_Clear();
			if(!returnFromReplace)
			{
				pCurrentMenuPage->currentItem = 2;
				menuState = MENU_STATE_SET_VALUE;
				menuValueList[2] = getValue(menuPage[2].value);
				pCurrentMenuPage->menuPage[1].pfProcess(MENU_EVENT_DRAW_ITEM, 1);
				pCurrentMenuPage->menuPage[2].pfProcess(MENU_EVENT_DRAW_ITEM, 2);
				pCurrentMenuPage->menuPage[3].pfProcess(MENU_EVENT_DRAW_ITEM, 3);//added by cyj 
				pCurrentMenuPage->menuPage[4].pfProcess(MENU_EVENT_DRAW_ITEM, 4);//added by cyj 
			}
			else
			{
				returnFromReplace = 0;
				pCurrentMenuPage->currentItem = 2;
				menuState = MENU_STATE_SET_VALUE;
				menuValueList[2] = saveUserSettingIndex;//getValue(menuPage[2].value);
				pCurrentMenuPage->menuPage[1].pfProcess(MENU_EVENT_DRAW_ITEM, 1);
				pCurrentMenuPage->menuPage[2].pfProcess(MENU_EVENT_DRAW_ITEM, 2);
				pCurrentMenuPage->menuPage[3].pfProcess(MENU_EVENT_DRAW_ITEM, 3); 
				pCurrentMenuPage->menuPage[4].pfProcess(MENU_EVENT_DRAW_ITEM, 4);
				LCD_WriteStr(1,5,langGetStr(finishedStr));
			}
			CurrentIsEnterNumberFlag = 1; 
		break;
		case MENU_KEY_MENU:
			menuState = MENU_STATE_CHANGE_ITEM;
			defaultMenuPageProcess(type, value);
		break;
		case MENU_KEY_NEXT:
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_SET_VALUE, item);
		break;
    	case MENU_EVENT_DRAW_ITEM:
	        if(menuPage[value].type == MENU_TYPE_VALUE)
	        {
	            if(value == item)
	            {
	            	sprintf(buffer, "%s%s", menuStateStr[menuState],valueToString(menuPage[value].value,menuValueList[value]));                
	            }           
	            else
	            {
	                sprintf(buffer, "%s%s", MENU_NO_SELECT,valueToString(menuPage[value].value,menuValueList[value]));
	            }
	        }
			else if(menuPage[value].type == MENU_TYPE_LABEL)
	        {
	        	if(strlen(langGetStr(menuPage[value].str)) > 18)
	        	{
	        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
	        	}
				else
				{
	            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
				}
	        }
			if(value > 2)
			{
		        LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			else
			{
		        LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
        break;
		case MENU_EVENT_SET_VALUE:
			menuState = MENU_STATE_SET_VALUE;
	        value = setValue(menuPage[item].value, menuValueList[item]);
	        if((value == FALSE) && (menuPage[item].value == usbAllSaveValue))
	        {
	            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
	            LCD_WriteStr(1,7,langGetStr(saveFailStr));
	        }
	        else
	        {
	            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
			    LCD_WriteStr(1,7,langGetStr(finishedStr));
	        }       
        break;
    	default:
        	defaultMenuPageProcess(type, value);
        break;
    }
}

void setReplaceRecordEn(int value)
{
	//replaceRecordEn = value;
	if(value)
	{
		UserSettingSaveTo((UserSaveIndex_TypeDef)saveUserSettingIndex);
	}
}

void loadFromMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

	#ifdef ENTER_NUMBER_KEY
	type = changeToKeyNumber(type);
	#endif

    switch(type)
    {
		case MENU_KEY_LOAD:
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_STATUS);
		break;

		#ifdef ENTER_NUMBER_KEY

		case MENU_KEY_0:
		case MENU_KEY_1:
		case MENU_KEY_2:
		case MENU_KEY_3:
		case MENU_KEY_4:
		case MENU_KEY_5:
		case MENU_KEY_6:
		case MENU_KEY_7:
		case MENU_KEY_8:
		case MENU_KEY_9:
			menuState = MENU_STATE_SET_VALUE;
			if(type == MENU_KEY_0)
			{
				type = MENU_KEY_9+1;
			}
			menuValueList[item] = type-MENU_KEY_1;
	        pCurrentMenuPage->menuPage[item].pfProcess(MENU_EVENT_SET_VALUE, item);
		
        break;
		#endif
    	case MENU_EVENT_ENTER:
			menuClearItemBar();
			LCD_Clear();
			pCurrentMenuPage->currentItem = 2;
			defaultMenuPageProcess(type, value);
			menuState = MENU_STATE_SET_VALUE;
			CurrentIsEnterNumberFlag = 1; 
		break;
		case MENU_EVENT_EXIT:
		break;
		case MENU_KEY_MENU:
			if((menuState == MENU_STATE_SELECT_ITEM) || (menuState == MENU_STATE_SET_VALUE))
			{
				menuState = MENU_STATE_CHANGE_ITEM;
			}
			defaultMenuPageProcess(type, value);
		break;
		case MENU_KEY_NEXT:		
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_SET_VALUE, item);
			menuEnterStatusPage();		
		break;

    	case MENU_EVENT_DRAW_ITEM:
	        if(menuPage[value].type == MENU_TYPE_VALUE)
	        {
	            if(value == item)
	            {
	            	sprintf(buffer, "%s%s", menuStateStr[menuState],valueToString(menuPage[value].value,menuValueList[value]));                
	            }           
	            else
	            {
	                sprintf(buffer, "%s%s", MENU_NO_SELECT,valueToString(menuPage[value].value,menuValueList[value]));
	            }
	        }
			else if(menuPage[value].type == MENU_TYPE_LABEL)
	        {
	        	if(strlen(langGetStr(menuPage[value].str)) > 18)
	        	{
	        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
	        	}
				else
				{
	            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
				}
	        }
	 		if(value > 2)
			{
		        LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			else
			{
		        LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
        break;

		case MENU_EVENT_SET_VALUE:
			menuState = MENU_STATE_SET_VALUE;
			pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);

			if(!isUserSettingUsed_ram((UserSaveIndex_TypeDef) (menuValueList[item])) 
				&& (!(menuIsCurrentPageIndex(MENU_INDEX_USB_SINGLE_LOAD) || menuIsCurrentPageIndex(MENU_INDEX_USB_ALL_LOAD) 
				|| menuIsCurrentPageIndex(MENU_INDEX_USB_ALL_SAVE))))
			{
				//printf("recoed %d is not used\r\n", menuValueList[item]);
				LCD_WriteStr(1,5,langGetStr(recordFreeStr));
			}
			else
			{
				value = setValue(menuPage[item].value, menuValueList[item]); 
	            if(value == FALSE && ((menuPage[item].value == usbSingleLoadValue) 
	                 || (menuPage[item].value == usbAllLoadValue)))
	            {
	                LCD_WriteStr(1,5,langGetStr(loadFailStr));
	            }
	            else
	            {
				    LCD_WriteStr(1,5,langGetStr(finishedStr));
	            }
			}
	        SysDelay1ms(2000);
			menuEnterStatusPage();
        break;
		default:
			changeNoSetProcess(type, value);
		break;
    }
}

void outputDetailPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;
	
    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	switch(type)
	{
		case MENU_EVENT_DRAW_ITEM:
		if(menuPage[value].type == MENU_TYPE_READ)
		{
			if(value == item)
			{
				sprintf(buffer, "%s%s: %s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			}
			else
			{
				sprintf(buffer, "%s%s: %s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			}
			LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1));
			break;
		}
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
	
}

void systemInfoPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;
//	char fb;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	
	switch(type)
	{
		case MENU_EVENT_ENTER:
			menuClearItemBar();
			defaultMenuPageProcess(type, value);
		break;

		case MENU_EVENT_NEXT_ITEM:
			if(pCurrentMenuPage->menuTtemSize > 5)
			{
				//if(item < 5)
				//{
				item = 5;
				//}
				//defaultMenuPageProcess(type, value);
				pCurrentMenuPage->currentItem = item;
				LCD_WriteLine2_L("");
				LCD_WriteLine3_L("");
				LCD_WriteLine4_L("");
				defaultMenuPageProcess(MENU_EVENT_ENTER, 0);
			}
		break;
			
		case MENU_EVENT_PREV_ITEM: 
			if(pCurrentMenuPage->menuTtemSize > 5)
			{
				//if(item == 5)
				//{
				item = 1;
				//}
				//defaultMenuPageProcess(type, value);
				pCurrentMenuPage->currentItem = item;
				menuClearItemBar();
				defaultMenuPageProcess(MENU_EVENT_ENTER, 0);
			}
		break;

		case MENU_EVENT_EXIT:
			defaultMenuPageProcess(type, value);
		break;
			
		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_READ)
			{
            	sprintf(buffer, "%-20s: %-6s", langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				break;
			}
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
}



void MenuPage_Version_Process(int type, int value)
{
	char buffer[40];
	const MenuPage *menuPage;
//	int item;
//	char fb;

//	item = pCurrentMenuPage->currentItem;
	menuPage = pCurrentMenuPage->menuPage;
	
	switch(type)
	{
		case MENU_EVENT_ENTER:
			LCD_Clear();
			menuClearItemBar();
			defaultMenuPageProcess(type, value);
			break;

		case MENU_EVENT_EXIT:
			defaultMenuPageProcess(type, value);
			break;
			
		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_READ)
			{
				sprintf(buffer, "%-22s:%9s", langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));				
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				break;
			}
		default:
			defaultMenuPageProcess(type, value);
			break;
	}
}


void hardVersionMenuPageProcess(int type, int value)
{
	char buffer[40];
	const MenuPage *menuPage;
//	int item;

//	item = pCurrentMenuPage->currentItem;
	menuPage = pCurrentMenuPage->menuPage;
	
	switch(type)
	{
		case MENU_EVENT_ENTER:
			LCD_Clear();
			menuClearItemBar();
			defaultMenuPageProcess(type, value);
		break;

		case MENU_EVENT_EXIT:
			defaultMenuPageProcess(type, value);
		break;
			
		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_READ)
			{
				sprintf(buffer, "%-22s:%9s", langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));				
				LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				break;
			}
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
}


void dviSelectMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;
	char fb;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

    switch(type)
    {  
    	case MENU_KEY_NEXT:
			setValue(dviSelectValue,(item -1));
			defaultMenuPageProcess(type, value);
		break;
	
    	case MENU_EVENT_DRAW_ITEM:
	        if(menuPage[value].type == MENU_TYPE_DIR)
	        {
	        	char buffer2[6];
				sprintf(buffer2, "%s%d", langGetStr(menuPage[value].str), value);
	            if(value == item)
	            {
	            	fb = 1;
	                sprintf(buffer, "%s%-26s%s", MENU_SELECT_ITEM,buffer2, MENU_DIR_MARK);
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-26s%s", MENU_NO_SELECT,buffer2, MENU_DIR_MARK);
	            }
				//printf("draw %d\r\n", value);
				if(	fb )
				{
					LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				else
				{
					LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				//LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1));
	        }
			else
			{
				defaultMenuPageProcess(type, value);
				//printf("default\r\n");
			}
        break;
	
    	default:
        	defaultMenuPageProcess(type, value);
        break;
    }
}


void vgaSelectMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage;
    int item;
	char fb;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

    switch(type)
    {  
    	case MENU_KEY_NEXT:
			setValue(vgaSelectValue, item & 1);
			defaultMenuPageProcess(type, value);
		break;
	
    	case MENU_EVENT_DRAW_ITEM:
	        if(menuPage[value].type == MENU_TYPE_DIR)
	        {
	        	char buffer2[6];
				sprintf(buffer2, "%s%d", langGetStr(menuPage[value].str), value);
	            if(value == item)
	            {
	            	fb = 1;
	                sprintf(buffer, "%s%-26s%s", MENU_SELECT_ITEM,buffer2, MENU_DIR_MARK);
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-26s%s", MENU_NO_SELECT,buffer2, MENU_DIR_MARK);
	            }
				//printf("draw %d\r\n", value);
				if(	fb )
				{
					LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				else
				{
					LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				//LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1));
	        }
			else
			{
				defaultMenuPageProcess(type, value);
				//printf("default\r\n");
			}
        break;
	
    	default:
        	defaultMenuPageProcess(type, value);
        break;
    }
}


void selectInputPageProcess(int type, int value)
{
    switch(type)
    {
    
    case MENU_EVENT_ENTER:
		pCurrentMenuPage->currentItem = 2;
		defaultMenuPageProcess(type, value);
		//ledOnSelectInput();
		break;
	case MENU_EVENT_EXIT:
		//ledOffSelectInput();
		break;
    default:
        defaultMenuPageProcess(type, value);
        break;
    }
}



void MenuPage_TechSupport_Process(int type, int value)
{
    char buffer[40];
    const MenuPage *menuPage;
	int item;

	item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	
	switch(type)
	{
		case MENU_EVENT_NEXT_ITEM:
	        item+=4;
	        if(item < 1)
	        {
	            item = pCurrentMenuPage->menuTtemSize-1;
	        }
	        if(item > (pCurrentMenuPage->menuTtemSize-1))
	        {
	            item = pCurrentMenuPage->menuTtemSize-1;
	        }
			//printf("next item = %d\r\n", item);
	        pCurrentMenuPage->currentItem = item;
	        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
        break;
    	case MENU_EVENT_PREV_ITEM:
	        item-=4;
	        if(item < 1)
	        {
	            item = 1;
	        }
	        if(item > (pCurrentMenuPage->menuTtemSize-1))
	        {
	            item = 1;
	        }
			//printf("prev item = %d\r\n", item);
	        pCurrentMenuPage->currentItem = item;
	        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
        break;
		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_LABEL)
	        {
	        	if(strlen(langGetStr(menuPage[value].str)) > 18)
	        	{
	        		sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
	        	}
				else
				{
	            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
				}
	        }
			else if(menuPage[value].type == MENU_TYPE_READ)
			{
	            sprintf(buffer, "%s%s", MENU_NO_SELECT, valueToString(menuPage[value].value,menuValueList[value]));
			}
			LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			if(value > 4)
			{
				dispMenuPosBar(2, 2);
			}
			else
			{
				dispMenuPosBar(1, 2);
			}
		break;
		
		case MENU_EVENT_ENTER:
			if(getDeviceInfo()->logoDisMode & 0x80)
			{
				pCurrentMenuPage->menuTtemSize = 8;
			}
			else
			{
				pCurrentMenuPage->menuTtemSize = 7;
			}
		break;
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
}


void dateTimeMenuPageTask(void)
{
	if((pCurrentMenuPage->currentItem == 1) 
		|| (pCurrentMenuPage->currentItem == 2))
	{
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 2);
	}
}

void dateTimeMenuPageProcess(int type, int value)
{
    char buffer[40];
    const MenuPage *menuPage;
	int item;
	char fb;

	item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	//printf("dateTimeMenuPageProcess,type: %d\r\n",type);
	switch(type)
	{
		case MENU_EVENT_NEXT_ITEM:
			if(!menuIsCurrentPageIndex(MENU_INDEX_SYS_WORKTIME))
			{
				item++;
				//if(item >= 2)
				//{
				//	item = 2;
				//}
		        pCurrentMenuPage->currentItem = item;
				//printf("pCurrentMenuPage->currentItem = %d\r\n",pCurrentMenuPage->currentItem);
		        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
			}
		break;
			
		case MENU_EVENT_PREV_ITEM:
			if(!menuIsCurrentPageIndex(MENU_INDEX_SYS_WORKTIME))
			{
				item--;
				if(item < 2)
				{
					item = 1;
				}
		        pCurrentMenuPage->currentItem = item;
		        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
			}
		break;
			
		case MENU_EVENT_ENTER:
			//printf("MENU_EVENT_ENTER\r\n");
			if(menuIsCurrentPageIndex(MENU_INDEX_SYS_WORKTIME))
			{
				LCD_Clear();
			}
			menuAddMainFunc(500,dateTimeMenuPageTask);
			if(pCurrentMenuPage->currentItem < 3)
			{

				pCurrentMenuPage->currentItem = 3;
			}
			//pCurrentMenuPage->currentItem += 2;
			defaultMenuPageProcess(type, value);
		break;
		case MENU_EVENT_EXIT:
			menuRemoveMainFunc(dateTimeMenuPageTask);
			break;
		case MENU_EVENT_DRAW_ITEM:
			//printf("MENU_EVENT_DRAW_ITEM\r\n");
			if(menuPage[value].type == MENU_TYPE_READ)
			{
				if(menuIsCurrentPageIndex(MENU_INDEX_SYS_WORKTIME))
				{
					//printf("strlen: %d\r\n",strlen(valueToString(menuPage[value].value,menuValueList[value])) );
	                sprintf(buffer, "%-14s%18s",langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
				}
				else
				{
	                sprintf(buffer, "%-14s%16s",langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
				}
			}
			else if(menuPage[value].type == MENU_TYPE_DIR)
	        {
	            if(value == item)
	            {
					fb = 1;
	                sprintf(buffer, "%s%-26s%s", MENU_SELECT_ITEM,langGetStr(menuPage[value].str), MENU_DIR_MARK);
	            }
	            else
	            {
					fb = 0;
	                sprintf(buffer, "%s%-26s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str), MENU_DIR_MARK);
	            }
				if(	fb )
				{
					LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				else
				{
					LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				//LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				dispMenuPosBar(item - 2,(pCurrentMenuPage->menuTtemSize - 1 - 5));
				//break;
	        }
	        else if(menuPage[value].type == MENU_TYPE_VALUE)
	        {
		        	if(strlen(valueToString(menuPage[value].value,menuValueList[value])) == 10)
		        	{
						//printf("strlen == 10\r\n");
		        		if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%-18s%10s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%-18s%10s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
		        	}
					else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 14)
					{
						//printf("strlen > 14\r\n");
						if((DEVICE_TYPE == 0x1f00)&&(menuIsCurrentPageIndex(MENU_INDEX_TRANSITION)) )
						{
							if(value == item)
				            {
								fb = 1;
				            	sprintf(buffer, "%s%-8s%20s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
				            }
				            else
				            {
								fb = 0;
				                sprintf(buffer, "%s%-8s%20s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
				            }
						}
						else
						{
							if(value == item)
				            {
								fb = 1;
				            	sprintf(buffer, "%s%s %s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
				            }
				            else
				            {
								fb = 0;
				                sprintf(buffer, "%s%s %s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
				            }
						}
					}
					else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 8)
		        	{
						//printf("strlen > 8\r\n");
		        		if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%-14s%14s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%-14s%14s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
		        	}
					else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 6)
					{
						//printf("strlen > 6\r\n");
			            if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%-20s%8s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%-20s%8s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
					}
					else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) < 4)
					{
						//printf("strlen < 4\r\n");
			            if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%-24s%4s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%-24s%4s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
					}
					else
					{
						//printf("strlen else\r\n");
						if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%-22s%6s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%-22s%6s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
					}
			
				dispMenuPosBar(item - 2,(pCurrentMenuPage->menuTtemSize - 1 - 5));
	        }
		break;
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
}



void enterPasswordTask(void)
{
	if(menuValueList[0])
	{
		menuValueList[0] = 0;
	}
	else
	{
		menuValueList[0] = 1;
	}
	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
}



void enterSendCardConfigTask(void)
{
	if(menuValueList[0])
	{
		menuValueList[0] = 0;
	}
	else
	{
		menuValueList[0] = 1;
	}
	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_REFESH, 1);
}


void menuPanelPasswordPageProcess(int type, int value)
{
	char buffer[40];
	const MenuPage *menuPage = pCurrentMenuPage->menuPage;
	int item = pCurrentMenuPage->currentItem;

	switch(type)
	{  
		case MENU_KEY_NEXT:
			if(item >= 4)
			{
				int i;
				value = 0;
				for(i = 0; i < 4; i++)
				{
					value |= menuValueList[i+1]<<(12 - 4*i);
				}
				if(menuValueList[5] == 1)
				{
					menuValueList[6] = value;
					
					menuValueList[5] = 2; // 第二次输入密码
					menuValueList[0] = 1; // 密码显示方式
					menuValueList[1] = 0; // 存储密码第1位
					menuValueList[2] = 0; // 存储密码第2位
					menuValueList[3] = 0; // 存储密码第3位
					menuValueList[4] = 0; // 存储密码第4位
					
					pCurrentMenuPage->currentItem = 1;
					sprintf(buffer, "%s", langGetStr(inputPasswordAgainStr));
					LCD_WriteLine1(buffer);
					menuAddMainFunc(500,enterPasswordTask);
					menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
				}
				else
				{
					if(menuValueList[6] == value)
					{
						setValue(lockPanelPasswordValue,value);
						setValue(keyLockValue,1);
						sprintf(buffer, "  %s", langGetStr(inputNewPasswordStr));
						LCD_WriteLine1(buffer);
						sprintf(buffer, "  %s", langGetStr(successStr));
						LCD_WriteLine2(buffer);
						SysDelay1ms(1800);
						menuEnterStatusPage();					
					}
					else
					{
						sprintf(buffer, "  %s", langGetStr(twoPasswordsStr));
						LCD_WriteLine1(buffer);
						sprintf(buffer, "  %s", langGetStr(doNotMatchStr));
						LCD_WriteLine2(buffer);
						SysDelay1ms(1800);
						menuPage->pfProcess(MENU_EVENT_ENTER, 1);
					}
				}
			}
			else
			{
				menuPage->pfProcess(MENU_KEY_RIGHT, 0);
				
			}
			break;
		case MENU_EVENT_EXIT:
			menuRemoveMainFunc(enterPasswordTask);
		break;
			
		case MENU_EVENT_ENTER:
			setValue(keyLockValue, 0);
			menuState = MENU_STATE_CHANGE_ITEM;
			menuValueList[5] = 1; // 第一次输入密码
			menuValueList[0] = 1; // 密码显示方式
			menuValueList[1] = 0; // 存储密码第1位
			menuValueList[2] = 0; // 存储密码第2位
			menuValueList[3] = 0; // 存储密码第3位
			menuValueList[4] = 0; // 存储密码第4位
			LCD_Clear();
			pCurrentMenuPage->currentItem = 1;
			sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(menuPage[1].str));
			LCD_WriteLine1(buffer);
			menuAddMainFunc(500,enterPasswordTask);
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
			
		case MENU_EVENT_DRAW_ITEM:
			sprintf(buffer, "      %d%d%d%d", menuValueList[1],
				menuValueList[2], menuValueList[3], menuValueList[4]);			
			if(menuValueList[0] == 0)
			{
				buffer[5 + pCurrentMenuPage->currentItem] = ' ';
			}

			LCD_WriteLine2(buffer);
		break;
		
		case MENU_KEY_LEFT:
			item--;
			if((item < 1) || (item > 4))
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_RIGHT:
			item++;
			if((item < 1) || (item > 4))
			{
				item = 4;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_UP:
			menuValueList[item]++;
			if((menuValueList[item] < 0) || (menuValueList[item] > 9))
			{
				menuValueList[item] = 9;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
			
		break;
		case MENU_KEY_DOWN:
			menuValueList[item]--;
			if((menuValueList[item] < 0) || (menuValueList[item] > 9))
			{
				menuValueList[item] = 0;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
}

void changePasswordMenuPageProcess(int type, int value)
{
	char buffer[40];
	const MenuPage *menuPage = pCurrentMenuPage->menuPage;
	int item = pCurrentMenuPage->currentItem;

	switch(type)
	{  
		case MENU_KEY_NEXT:
			if(item >= 6)
			{
				int i;
				value = 0;
				for(i=0; i<6; i++)
				{
					value |= menuValueList[i+1]<<(20-4*i);
				}
				if(menuValueList[7] == 1)
				{
					menuValueList[8] = value;
					
					menuValueList[7] = 2; // 第二次输入密码
					menuValueList[0] = 1; // 密码显示方式
					menuValueList[1] = 0; // 存储密码第1位
					menuValueList[2] = 0; // 存储密码第2位
					menuValueList[3] = 0; // 存储密码第3位
					menuValueList[4] = 0; // 存储密码第4位
					menuValueList[5] = 0; // 存储密码第5位
					menuValueList[6] = 0; // 存储密码第6位
					
					pCurrentMenuPage->currentItem = 1;
					sprintf(buffer, "%s", langGetStr(inputPasswordAgainStr));
					LCD_WriteLine1(buffer);
					menuAddMainFunc(500,enterPasswordTask);
					menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
				}
				else
				{
					if(menuValueList[8] == value)
					{
						setValue(licensePasswordValue,value);
						sprintf(buffer, "  %s", langGetStr(inputNewPasswordStr));
						LCD_WriteLine1(buffer);
						sprintf(buffer, "  %s", langGetStr(successStr));
						LCD_WriteLine2(buffer);
						SysDelay1ms(1800);
						menuPage->pfProcess(MENU_KEY_ESC, 1);
					}
					else
					{
						sprintf(buffer, "  %s", langGetStr(twoPasswordsStr));
						LCD_WriteLine1(buffer);
						sprintf(buffer, "  %s", langGetStr(doNotMatchStr));
						LCD_WriteLine2(buffer);
						SysDelay1ms(1800);
						menuPage->pfProcess(MENU_EVENT_ENTER, 1);
					}
				}
				
			}
			else
			{
				menuPage->pfProcess(MENU_KEY_RIGHT, 0);
				
			}
		break;
		case MENU_EVENT_EXIT:
			menuRemoveMainFunc(enterPasswordTask);
		break;
		
		case MENU_EVENT_ENTER:
			menuState = MENU_STATE_CHANGE_ITEM;
			menuValueList[7] = 1; // 第一次输入密码
			menuValueList[0] = 1; // 密码显示方式
			menuValueList[1] = 0; // 存储密码第1位
			menuValueList[2] = 0; // 存储密码第2位
			menuValueList[3] = 0; // 存储密码第3位
			menuValueList[4] = 0; // 存储密码第4位
			menuValueList[5] = 0; // 存储密码第5位
			menuValueList[6] = 0; // 存储密码第6位
			LCD_Clear();
			pCurrentMenuPage->currentItem = 1;
			sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(menuPage[1].str));
			LCD_WriteLine1(buffer);
			menuAddMainFunc(500,enterPasswordTask);
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		
		case MENU_EVENT_DRAW_ITEM:
			#ifdef ENABLE_HD44780_LCD
			if(menuValueList[0])
			{
				sprintf(buffer, "    %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
			}
			else
			{
				sprintf(buffer, "    %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
				buffer[3+pCurrentMenuPage->currentItem] = ' ';
			}
			LCD_WriteLine2(buffer);
			#else
			if(menuValueList[0])
			{
				sprintf(buffer, "      %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
			}
			else
			{
				sprintf(buffer, "      %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
				buffer[5+pCurrentMenuPage->currentItem] = ' ';
			}
			LCD_WriteLine2(buffer);
			#endif
		break;
	
		case MENU_KEY_LEFT:
			item--;
			if((item < 1) || (item > 6))
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_RIGHT:
			item++;
			if((item < 1) || (item > 6))
			{
				item = 6;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_UP:
			menuValueList[item]++;
			if((menuValueList[item]<0) || (menuValueList[item]>9))
			{
				menuValueList[item] = 9;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		
		break;
		case MENU_KEY_DOWN:
			menuValueList[item]--;
			if((menuValueList[item]<0) || (menuValueList[item]>9))
			{
				menuValueList[item] = 0;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
}

void menuPanelEnterPasswordProcess(int type, int value)
{
	char buffer[40];
	const MenuPage *menuPage = pCurrentMenuPage->menuPage;
	int item = pCurrentMenuPage->currentItem;

	switch(type)
	{  
		case MENU_KEY_NEXT:
			if(item >= 4)
			{
				int i;
				value = 0;
				for(i=0; i<4; i++)
				{
					value |= menuValueList[i+1]<<(12-4*i);
				}
				if((value == getValue(lockPanelPasswordValue))||(value == DEFAULT_LOCKPANEL_CRACK))
				{
					setValue(keyLockValue,0);
					setUserPadLock(0,0);
					//menuList[MENU_INDEX_LICENSE_SETUP].currentItem = 1;
					menuEnterStatusPage();
					popMenuPage();
				}
				else
				{
					sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(passwordErrorStr));
					LCD_WriteLine1(buffer);
					sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(retypePasswordStr));
					LCD_WriteLine2(buffer);
					SysDelay1ms(1800);
					menuPage->pfProcess(MENU_EVENT_ENTER, 0);
				}
				
			}
			else
			{
				menuPage->pfProcess(MENU_KEY_RIGHT, 0);
				
			}
			break;
		case MENU_EVENT_EXIT:
			menuRemoveMainFunc(enterPasswordTask);
		break;
			
		case MENU_EVENT_ENTER:
			menuState = MENU_STATE_CHANGE_ITEM;
			menuValueList[0] = 1; // 密码显示方式
			menuValueList[1] = 0; // 存储密码第1位
			menuValueList[2] = 0; // 存储密码第2位
			menuValueList[3] = 0; // 存储密码第3位
			menuValueList[4] = 0; // 存储密码第4位
			LCD_Clear();
			pCurrentMenuPage->currentItem = 1;
			sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(menuPage[1].str));
			LCD_WriteLine1(buffer);
			menuAddMainFunc(500,enterPasswordTask);
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
			
		case MENU_EVENT_DRAW_ITEM:
			sprintf(buffer, "      %d%d%d%d", menuValueList[1],
				menuValueList[2], menuValueList[3], menuValueList[4]);			
			if(menuValueList[0] == 0)
			{
				buffer[5 + pCurrentMenuPage->currentItem] = ' ';
			}
				
			LCD_WriteLine2(buffer);
		break;
	
		case MENU_KEY_LEFT:
			item--;
			if((item < 1) || (item > 4))
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_RIGHT:
			item++;
			if((item < 1) || (item > 4))
			{
				item = 4;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_UP:
			menuValueList[item]++;
			if((menuValueList[item]<0) || (menuValueList[item]>9))
			{
				menuValueList[item] = 9;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
			
		break;
		case MENU_KEY_DOWN:
			menuValueList[item]--;
			if((menuValueList[item]<0) || (menuValueList[item]>9))
			{
				menuValueList[item] = 0;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		default:
			defaultMenuPageProcess(type, value);
		break;
	}
}



void enterPasswordMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage = pCurrentMenuPage->menuPage;
    int item = pCurrentMenuPage->currentItem;

    switch(type)
    {  
    	case MENU_KEY_NEXT:
			if(item >= 6)
			{
				int i;
				value = 0;
				for(i=0; i<6; i++)
				{
					value |= menuValueList[i+1]<<(20-4*i);
				}
				if((value == getLicense()->password)||(value == 0x245880))
				{
					menuList[MENU_INDEX_LICENSE_SETUP].currentItem = 1;
					menuEnterPage(MENU_INDEX_LICENSE_SETUP);
					popMenuPage();
				}
				else
				{
					sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(passwordErrorStr));
					LCD_WriteLine1(buffer);
					sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(retypePasswordStr));
					LCD_WriteLine2(buffer);
					SysDelay1ms(1800);
					menuPage->pfProcess(MENU_EVENT_ENTER, 0);
				}
				
			}
			else
			{
				menuPage->pfProcess(MENU_KEY_RIGHT, 0);
				
			}
		break;
		case MENU_EVENT_EXIT:
			menuRemoveMainFunc(enterPasswordTask);
		break;
		
		case MENU_EVENT_ENTER:
			menuState = MENU_STATE_CHANGE_ITEM;
			menuValueList[0] = 1; // 密码显示方式
			menuValueList[1] = 0; // 存储密码第1位
			menuValueList[2] = 0; // 存储密码第2位
			menuValueList[3] = 0; // 存储密码第3位
			menuValueList[4] = 0; // 存储密码第4位
			menuValueList[5] = 0; // 存储密码第5位
			menuValueList[6] = 0; // 存储密码第6位
			LCD_Clear();
			pCurrentMenuPage->currentItem = 1;
			sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(menuPage[1].str));
			LCD_WriteLine1(buffer);
			menuAddMainFunc(500,enterPasswordTask);
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		
		case MENU_EVENT_DRAW_ITEM:
			#ifdef ENABLE_HD44780_LCD
			if(menuValueList[0])
			{
				sprintf(buffer, "    %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
			}
			else
			{
				sprintf(buffer, "    %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
				buffer[3+pCurrentMenuPage->currentItem] = ' ';
			}
			#else
			if(menuValueList[0])
			{
				sprintf(buffer, "      %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
			}
			else
			{
				sprintf(buffer, "      %d%d%d%d%d%d", menuValueList[1],
					menuValueList[2], menuValueList[3], menuValueList[4], menuValueList[5], menuValueList[6]);
				buffer[5+pCurrentMenuPage->currentItem] = ' ';
			}
			#endif
			LCD_WriteLine2(buffer);
		break;
	
  		case MENU_KEY_LEFT:
			item--;
			if((item < 1) || (item > 6))
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_RIGHT:
			item++;
			if((item < 1) || (item > 6))
			{
				item = 6;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_UP:
			menuValueList[item]++;
			if((menuValueList[item]<0) || (menuValueList[item]>9))
			{
				menuValueList[item] = 9;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		
		break;
		case MENU_KEY_DOWN:
			menuValueList[item]--;
			if((menuValueList[item]<0) || (menuValueList[item]>9))
			{
				menuValueList[item] = 0;
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		
	
    	default:
        	defaultMenuPageProcess(type, value);
        break;
    }
}

int BCD2INT(int value)
{
	int i, result = 0;
	for(i=0; i<8; i++)
	{
		result *= 10;
		result += ((value>>(28-i*4))&0xf);
	}
	return result;
}


int  INT2BCD(int value)
{
	int i, result = 0;
	for(i=0; i<8; i++)
	{
		result += ((value%10)<<(i*4));
		value = value/10;
		if(value == 0)
		{
			break;
		}
	}
	return result;
}


void changeDateMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage = pCurrentMenuPage->menuPage;
    int item = pCurrentMenuPage->currentItem;

    switch(type)
    {  
    	case MENU_KEY_NEXT:
			if(item >= 3)
			{
				value = INT2BCD(menuValueList[1])<<16;
				value += INT2BCD(menuValueList[2])<<8;
				value += INT2BCD(menuValueList[3]);
				setValue(dateValue, value);
				menuPage->pfProcess(MENU_KEY_MENU, 0);		
				
			}
			else
			{
				menuPage->pfProcess(MENU_KEY_RIGHT, 0);
				
			}
		break;
		case MENU_EVENT_EXIT:
			menuRemoveMainFunc(enterPasswordTask);
		break;
		
		case MENU_EVENT_ENTER:
			menuState = MENU_STATE_CHANGE_ITEM;
			menuValueList[0] = 1; // 密码显示方式
			menuValueList[1] = BCD2INT(getValue(dateValue)>>16); 		// year
			menuValueList[2] = BCD2INT((getValue(dateValue)>>8)&0xff); 	// month
			menuValueList[3] = BCD2INT(getValue(dateValue)&0xff); 		// day
			LCD_Clear();
			pCurrentMenuPage->currentItem = 1;
			sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(menuPage[1].str));
			LCD_WriteLine1(buffer);
			menuAddMainFunc(500,enterPasswordTask);
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		
		case MENU_EVENT_DRAW_ITEM:
			if(menuValueList[0])
			{
					sprintf(buffer, "      %04d-%02d-%02d", menuValueList[1],menuValueList[2], menuValueList[3]);
			}
			else
			{
				if(pCurrentMenuPage->currentItem == 1)
				{
					sprintf(buffer, "          -%02d-%02d",
					menuValueList[2], menuValueList[3]);
				}
				else if(pCurrentMenuPage->currentItem == 2)
				{
					sprintf(buffer, "      %04d-  -%02d", menuValueList[1],menuValueList[3]);
				}
				else if(pCurrentMenuPage->currentItem == 3)
				{
					sprintf(buffer, "      %04d-%02d-  ", menuValueList[1],menuValueList[2]);
				}
				
			}
			LCD_WriteLine2(buffer);
			//dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1)); //added by cyj
		break;
	
  		case MENU_KEY_LEFT:
			item--;
			if((item < 1) || (item > 3))
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_RIGHT:
			item++;
			if((item < 1) || (item > 3))
			{
				item = 3;
			}
			pCurrentMenuPage->currentItem = item;
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		case MENU_KEY_UP:
			menuValueList[item]++;
			if(item == 1)
			{
				if((menuValueList[item]<1900) || (menuValueList[item]>2099))
				{
					menuValueList[item] = 2099;
				}
			}
			else if(item == 2)
			{
				if((menuValueList[item]<1) || (menuValueList[item]>12))
				{
					menuValueList[item] = 12;
				}
			}
			else if(item == 3)
			{
				if((menuValueList[item]<1) || (menuValueList[item]>31))
				{
					menuValueList[item] = 31;
				}
			}
			
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		
		break;
		case MENU_KEY_DOWN:
			menuValueList[item]--;
			if(item == 1)
			{
				if((menuValueList[item]<1900) || (menuValueList[item]>2099))
				{
					menuValueList[item] = 1900;
				}
			}
			else if(item == 2)
			{
				if((menuValueList[item]<1) || (menuValueList[item]>12))
				{
					menuValueList[item] = 1;
				}
			}
			else if(item == 3)
			{
				if((menuValueList[item]<1) || (menuValueList[item]>31))
				{
					menuValueList[item] = 1;
				}
			}
			menuResetMainTimeout();
			menuValueList[0] = 1;
			menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		
	
    	default:
        	defaultMenuPageProcess(type, value);
        break;
    }
}

void changeTimeMenuPageProcess(int type, int value)
{
	char buffer[40];
    const MenuPage *menuPage = pCurrentMenuPage->menuPage;
    int item = pCurrentMenuPage->currentItem;

    switch(type)
    {  
    case MENU_KEY_NEXT:
		if(item >= 3)
		{
			value = INT2BCD(menuValueList[1])<<16;
			value += INT2BCD(menuValueList[2])<<8;
			value += INT2BCD(menuValueList[3]);
			setValue(timeValue, value);
			menuPage->pfProcess(MENU_KEY_MENU, 0);		
			
		}
		else
		{
			menuPage->pfProcess(MENU_KEY_RIGHT, 0);
			
		}
		break;
	case MENU_EVENT_EXIT:
		menuRemoveMainFunc(enterPasswordTask);
		break;
		
	case MENU_EVENT_ENTER:
		menuState = MENU_STATE_CHANGE_ITEM;
		menuValueList[0] = 1; // 密码显示方式
		menuValueList[1] = BCD2INT(getValue(timeValue)>>16); 		// year
		menuValueList[2] = BCD2INT((getValue(timeValue)>>8)&0xff); 	// month
		menuValueList[3] = BCD2INT(getValue(timeValue)&0xff); 		// day
		LCD_Clear();
		pCurrentMenuPage->currentItem = 1;
		sprintf(buffer, "%s%s", MENU_NO_SELECT, langGetStr(menuPage[1].str));
		LCD_WriteLine1(buffer);
		menuAddMainFunc(500,enterPasswordTask);
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		
	case MENU_EVENT_DRAW_ITEM:
		if(menuValueList[0])
		{
				sprintf(buffer, "      %02d:%02d:%02d", menuValueList[1],menuValueList[2], menuValueList[3]);
		}
		else
		{
			if(pCurrentMenuPage->currentItem == 1)
			{
				sprintf(buffer, "        :%02d:%02d",
				menuValueList[2], menuValueList[3]);
			}
			else if(pCurrentMenuPage->currentItem == 2)
			{
				sprintf(buffer, "      %02d:  :%02d", menuValueList[1],menuValueList[3]);
			}
			else if(pCurrentMenuPage->currentItem == 3)
			{
				sprintf(buffer, "      %02d:%02d:  ", menuValueList[1],menuValueList[2]);
			}
			
		}
		LCD_WriteLine2(buffer);
		//dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1)); //added by cyj
		break;
	
  	case MENU_KEY_LEFT:
		item--;
		if((item < 1) || (item > 3))
		{
			item = 1;
		}
		pCurrentMenuPage->currentItem = item;
		menuResetMainTimeout();
		menuValueList[0] = 1;
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
	case MENU_KEY_RIGHT:
		item++;
		if((item < 1) || (item > 3))
		{
			item = 3;
		}
		pCurrentMenuPage->currentItem = item;
		menuResetMainTimeout();
		menuValueList[0] = 1;
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
	case MENU_KEY_UP:
		menuValueList[item]++;
		if(item == 1)
		{
			if((menuValueList[item]<0) || (menuValueList[item]>23))
			{
				menuValueList[item] = 23;
			}
		}
		else if(item == 2)
		{
			if((menuValueList[item]<0) || (menuValueList[item]>59))
			{
				menuValueList[item] = 59;
			}
		}
		else if(item == 3)
		{
			if((menuValueList[item]<0) || (menuValueList[item]>59))
			{
				menuValueList[item] = 59;
			}
		}
		
		menuResetMainTimeout();
		menuValueList[0] = 1;
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		
		break;
	case MENU_KEY_DOWN:
		menuValueList[item]--;
		if(item == 1)
		{
			if((menuValueList[item]<0) || (menuValueList[item]>23))
			{
				menuValueList[item] = 0;
			}
		}
		else if(item == 2)
		{
			if((menuValueList[item]<0) || (menuValueList[item]>59))
			{
				menuValueList[item] = 0;
			}
		}
		else if(item == 3)
		{
			if((menuValueList[item]<0) || (menuValueList[item]>59))
			{
				menuValueList[item] = 0;
			}
		}
		menuResetMainTimeout();
		menuValueList[0] = 1;
		menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, 1);
		break;
		
	
    default:
        defaultMenuPageProcess(type, value);
        break;
    }
}


void changeLicenseTimeMenuPageProcess(int type, int value)
{
	switch(type)
	{
	case MENU_EVENT_ENTER:
		pCurrentMenuPage->currentItem = 2;
		defaultMenuPageProcess(type,value);
		break;
	default:
		defaultMenuPageProcess(type,value);
		break;
	}
}


void hotbackupPageProcess(int type, int value)
{
	
	switch(type)
	{			
		case MENU_EVENT_ENTER:
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
}



void displayModePageProcess(int type, int value)
{
    const MenuPage *menuPage;
    int item;
	char buffer[40];
	char fb = 0;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	//printf("displayModePageProcess,type: %d,item: %d\r\n",type,item);
    switch(type)
    {
	    case MENU_EVENT_NEXT_ITEM:
	        item++;
			if(getValue(displayModeValue) == 3)   //added by cyj
			{
	        	item++;
			}
	        if(item < 1)
	        {
	            item = pCurrentMenuPage->menuTtemSize-1;
	        }
	        if(item > (pCurrentMenuPage->menuTtemSize-1))
	        {
	            item = pCurrentMenuPage->menuTtemSize-1;
	        }
	        pCurrentMenuPage->currentItem = item;
	        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
	        break;
	    case MENU_EVENT_PREV_ITEM:
	        item--;
			if(getValue(displayModeValue) == 3) //added by cyj
			{
	        	item--;
			}
	        if(item < 1)
	        {
	            item = 1;
	        }
	        if(item > (pCurrentMenuPage->menuTtemSize-1))
	        {
	            item = 1;
	        }
	        pCurrentMenuPage->currentItem = item;
	        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
	        break;

		case MENU_EVENT_DRAW_ITEM:
	        if(menuPage[value].type == MENU_TYPE_LABEL)
	        {
	        	if(strlen(langGetStr(menuPage[value].str)) > 18)
	        	{
	        		sprintf(buffer, "%s", langGetStr(menuPage[value].str));
	        	}
				else
				{
	            	sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
				}
	        }
	        else if(menuPage[value].type == MENU_TYPE_DIR)
	        {
	            if(value == item)
	            {
					fb = 1;
	                sprintf(buffer, "%s%-26s%s", MENU_SELECT_ITEM,langGetStr(menuPage[value].str), MENU_DIR_MARK);
	            }
	            else
	            { 
					fb = 0;
	                sprintf(buffer, "%s%-26s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str), MENU_DIR_MARK);
	            }
				//dispMenuPosBar(item,pCurrentMenuPage->menuTtemSize);
	        }
	        else if((menuPage[value].type == MENU_TYPE_VALUE)||(menuPage[value].type == MENU_TYPE_ENTER_NUMBER_VALUE))
	        {
	        	if(strlen(valueToString(menuPage[value].value,menuValueList[value])) == 10)
	        	{
					//printf("strlen == 10\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-18s%10s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-18s%10s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 14)
				{
					//printf("strlen > 14\r\n");
					if((DEVICE_TYPE == 0x198)&&(menuIsCurrentPageIndex(MENU_INDEX_TRANSITION)) )
					{
						if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%-9s%19s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%-9s%19s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
					}
					else
					{
						if(value == item)
			            {
							fb = 1;
			            	sprintf(buffer, "%s%s %s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
			            }
			            else
			            {
							fb = 0;
			                sprintf(buffer, "%s%s %s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
			            }
					}
				}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 8)
	        	{
					//printf("strlen > 8\r\n");
	        		if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-14s%14s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-14s%14s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
	        	}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) > 6)
				{
					//printf("strlen > 6\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-20s%8s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-20s%8s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else if(strlen(valueToString(menuPage[value].value,menuValueList[value])) < 4)
				{
					//printf("strlen < 4\r\n");
		            if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-24s%4s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-24s%4s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
				else
				{
					//printf("strlen else\r\n");
					if(value == item)
		            {
						fb = 1;
		            	sprintf(buffer, "%s%-22s%6s", menuStateStr[menuState],langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));                
		            }
		            else
		            {
						fb = 0;
		                sprintf(buffer, "%s%-22s%6s", MENU_NO_SELECT,langGetStr(menuPage[value].str),valueToString(menuPage[value].value,menuValueList[value]));
		            }
				}
	        }
			if(value == 3)
			{
				if(	fb )
				{
					LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1-1, buffer);
				}
				else
				{
					LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1-1, buffer);
				}
				//LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1-1, buffer);
			}
			else
			{
				if(	fb )
				{
					LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				else
				{
					LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				//LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
			}
			//printf("diaplay mode page,itemSize: %d\r\n",pCurrentMenuPage->menuTtemSize);
			if(pCurrentMenuPage->menuTtemSize > 2)
			{
				if(getValue(displayModeValue) == 3) 
				{
					if(item < 3)
					{
						dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 2));//(1,2);//
					}
					else
					{
						dispMenuPosBar(item - 1,(pCurrentMenuPage->menuTtemSize - 2));//(2,2);//
					}
				}
				else
				{
					dispMenuPosBar(item,(pCurrentMenuPage->menuTtemSize - 1));
				}
			}
			break;

	    case MENU_EVENT_ENTER:
			menuClearItemBar();
			LCD_WriteLine3_L("");
			LCD_WriteLine4_L("");
			if(getValue(displayModeValue) == 3)
			{
				pCurrentMenuPage->menuTtemSize = 4;//sizeof(menuDisplayModePage)/sizeof(MenuPage) - 1;//2;
				
		        menuState = MENU_STATE_CHANGE_ITEM;

		        item = item-1;
		        item = item/MENU_DISPLAY_LINE_NUM;
		        item = item*MENU_DISPLAY_LINE_NUM+1;
		        {
		            int size = item+MENU_DISPLAY_LINE_NUM;
		            int clearLine;
		            if(size > (pCurrentMenuPage->menuTtemSize))
		            {
		                size = (pCurrentMenuPage->menuTtemSize);
		            }
		            clearLine = (size - item)-1;
		            if(clearLine<MENU_DISPLAY_LINE_NUM)
		            {
		                int i;
		                for(i=MENU_DISPLAY_LINE_NUM-1; i>=clearLine; i--)
		                {
		                    LCD_WriteLine(i, "");
		                }
		            }
		            for(; item<size; item++)
		            {            	
						if(item == (size - 2))
						{
							continue;
						}
						if((menuPage[item].type == MENU_TYPE_VALUE) || (menuPage[item].type == MENU_TYPE_READ)
							||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE))
						{
		                	menuValueList[item] = getValue(menuPage[item].value);
						}
		                menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
		            }

		        }
				break;
			}
			else if(getValue(displayModeValue) == 4)
			{
				pCurrentMenuPage->menuTtemSize = 3;//sizeof(menuDisplayModePage)/sizeof(MenuPage) - 1;//2;
			}
			else if(getValue(displayModeValue) < 3)
			{
				pCurrentMenuPage->menuTtemSize = 2;
			}
	    default:
			defaultMenuPageProcess(type, value);
	        break;
    }
}


void inputDetailPageProcess(int type, int value)
{
    const MenuPage *menuPage;
    //int item;
	char buffer[40];
	#if 0
	char buffer2[40],buffer3[80];
	#endif

    //item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	//printf("inputDetailPageProcess,type: %d\r\n",type);
    switch(type)
    {
		case MENU_EVENT_NEXT_ITEM:
		case MENU_EVENT_PREV_ITEM: 
			break;
			
		case MENU_EVENT_DRAW_ITEM:
			if(detailSource == 1)
			{
				sprintf(buffer, "Input Source:%15s", langGetStr(hdmiStr));
				LCD_WriteLine1(buffer);
				{
					sprintf(buffer, "Input Format:%15s", langGetStr(autoStr));
				}
				LCD_WriteLine2(buffer);	

				break;
			}
			else if(detailSource == 2)
			{
				sprintf(buffer, "Input Source:%15s", langGetStr(dviStr));
				LCD_WriteLine1(buffer);

				sprintf(buffer, "Input Format:%15s", langGetStr(autoStr));
				LCD_WriteLine2(buffer);			
				break;
			}
			else if(detailSource == 3)
			{
				sprintf(buffer, "Input Source:%15s", langGetStr(sdiStr));
				LCD_WriteLine1(buffer);

				sprintf(buffer, "Input Format:%15s", langGetStr(autoStr));
				LCD_WriteLine2(buffer);		
				
				break;
			}
			else
			{
				if(menuPage[value].type == MENU_TYPE_READ)
				{
				}
			}
		break;

		case MENU_EVENT_ENTER:
			menuClearItemBar();
			
		default:
			defaultMenuPageProcess(type, value);
			break;
    }
}


void ethernetPageProcess(int type, int value)
{
	switch(type)
	{
		case MENU_EVENT_ENTER:
		default:
			defaultMenuPageProcess(type,value);
		break;
	}
}

extern BYTE OLEDLum;
void oledLuminanceAdjustPageProcess(int type, int value)
{
    const MenuPage *menuPage;
    int item;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	printf("oledLuminanceAdjustPageProcess,type: %d\r\n",type);
    switch(type)
    {
	    case MENU_EVENT_NEXT_VALUE:
	        {
				int index;
		        index = menuPage[item].value;
		        menuState = MENU_STATE_CHANGE_VALUE;		        
				menuValueList[item] = nextValue(index,menuValueList[item]);
		        menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
	        }
	        break;
			
	    case MENU_EVENT_PREV_VALUE:
	        {
				int index;
		        index = menuPage[item].value;
		        menuState = MENU_STATE_CHANGE_VALUE;
		        menuValueList[item] = prevValue(index,menuValueList[item]);
		        menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
	        }
	        break;		
		
		case MENU_EVENT_DRAW_ITEM:
			if(menuPage[value].type == MENU_TYPE_VALUE)
			{
				defaultMenuPageProcess(type,value);
				LcdFillBlock_clear(7*2+(OLEDLum*2), 7*2+(15*2), 35, 44);	
				LcdFillBlock(7*2, 7*2+(OLEDLum*2), 35, 44);
				break;
			}
		default:
			defaultMenuPageProcess(type,value);
			break;
    }
}
void transitionPageProcess(int type, int value)
{
    const MenuPage *menuPage;
    int item;
	//char fb;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
	//printf("transitionPageProcess(%d)\r\n",type);
    switch(type)
    {			
    	case MENU_EVENT_SET_VALUE:
			//printf("MENU_EVENT_SET_VALUE,");
			if(menuValueList[item])
			{
			    menuState = MENU_STATE_CHANGE_ITEM;
				//printf("image enhance enable\r\n");
				pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER_PAGE, MENU_INDEX_IMAGE_ENHANCE_VERIFY);
			}
			else
			{
			    menuState = MENU_STATE_CHANGE_ITEM;
				//printf("menuValueList[%d] == 0\r\n",item);
				setValue(imageEnhanceValue, menuValueList[item]);
				menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
			}
			break;
			
		case MENU_EVENT_PREV_VALUE:
			if(menuPage[item].type == MENU_TYPE_VALUE)
			{
				int index;
				index = menuPage[item].value;
				menuState = MENU_STATE_CHANGE_VALUE;//MENU_STATE_CHANGE_AND_SET_VALUE;
				menuValueList[item] = prevValue(index,getValue(index));
				#if 1
                if(menuPage[item].value == imageEnhanceValue)
                    {
                    menuPage[item].pfProcess(MENU_EVENT_ENTER, item);
                    menuState = MENU_STATE_CHANGE_VALUE;
                    break;
                    }
                
				menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
				#else
				#endif
			}
			break;
		case MENU_EVENT_NEXT_VALUE:
			if((menuPage[item].type == MENU_TYPE_VALUE)||(menuPage[item].type == MENU_TYPE_ENTER_NUMBER_VALUE))
			{
				int index;
				index = menuPage[item].value;
				menuState = MENU_STATE_CHANGE_VALUE;//MENU_STATE_CHANGE_AND_SET_VALUE;
				menuValueList[item] = nextValue(index,getValue(index));
				#if 1
                if(menuPage[item].value == imageEnhanceValue)
                    {
                    menuPage[item].pfProcess(MENU_EVENT_ENTER, item);
                    menuState = MENU_STATE_CHANGE_VALUE;
                    break;
                    }                
				menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
				#else
				#endif
			}
			break;
			
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
}

void edidControlPageProcess(int type, int value)
{
	switch(type)
	{
		case MENU_EVENT_ENTER:
			refleshEDIDSourcePort();
			
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
	
}

void extVGAAdjustPageProcess(int type, int value)
{
	switch(type)
	{
			
		case MENU_EVENT_ENTER:
        	{
				defaultMenuPageProcess(type,value);
        	}
			break;
		case MENU_EVENT_EXIT:
			{
				defaultMenuPageProcess(type,value);
			}
			break;
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
	
}

void splitModePageProcess(int type, int value)
{
    const MenuPage *menuPage;
    int item;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

    switch(type)
    {
	    case MENU_KEY_NEXT:
            pushMenuPage(menuPage);
            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);
            menuPage = (MenuPage *)menuPage[item].value;
            pCurrentMenuPage = findMenuPage(menuPage);
			pCurrentMenuPage->currentItem = MENU_START_ITEM;
            pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
            break;

		case MENU_EVENT_ENTER:

		break;
			
		case MENU_EVENT_PREV_ITEM:
			//if(item == 2)
			{
				item = 1;
			}
			pCurrentMenuPage->currentItem = item;
			menuClearItemBar();
			defaultMenuPageProcess(MENU_EVENT_ENTER, 0);
		break;
		case MENU_EVENT_NEXT_ITEM:
			{
				item = 2;
			}
			pCurrentMenuPage->currentItem = item;
			menuClearItemBar();
			defaultMenuPageProcess(MENU_EVENT_ENTER, 0);
			break;
		case MENU_EVENT_DRAW_ITEM:
			{
				char fb,buffer[40];
				if(menuPage[value].type == MENU_TYPE_LABEL)
				{
					if(strlen(langGetStr(menuPage[value].str)) > 18)
					{
						sprintf(buffer, "%s", langGetStr(menuPage[value].str));
					}
					else
					{
						sprintf(buffer, "%s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str));
					}
				}
				else if(menuPage[value].type == MENU_TYPE_DIR)
				{
					if(value == item)
					{
						fb = 1;
						sprintf(buffer, "%s%-26s%s", MENU_SELECT_ITEM,langGetStr(menuPage[value].str), MENU_DIR_MARK);
					}
					else
					{
						fb = 0;
						sprintf(buffer, "%s%-26s%s", MENU_NO_SELECT,langGetStr(menuPage[value].str), MENU_DIR_MARK);
					}
					//dispMenuPosBar(item,pCurrentMenuPage->menuTtemSize);
				}
				
						
				if( fb )
				{
					LCD_WriteLine_fb(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				else
				{
					LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
				}
				if(item <= 2)
				{
					dispMenuPosBar(item,2);
					//menuDrawItemBar();
				}
			}
			break;
		default:
			defaultMenuPageProcess(type,value);
			break;
    }
}

void splitDetailPageProcess(int type, int value)
{
	switch(type)
	{
		case MENU_EVENT_EXIT:
//			if(GetUserSetting()->splitOnOff == 0) //拼接关闭的状态,返回到选择拼接类型的菜单
			{
				if(pCurrentMenuPage != &menuList[MENU_INDEX_SPLIT_MODE])
				{
					pCurrentMenuPage = &menuList[MENU_INDEX_SPLIT_MODE];
					pCurrentMenuPage->currentItem = 1;
					defaultMenuPageProcess(type,value); 
					return;
				}
			}
		default:
			enterNumberPageProcess(type,value);
			break;
	}
}

void mainMenuPageProcess(int type, int value)
{
	switch(type)
	{
		case MENU_EVENT_ENTER:
			if(pCurrentMenuPage != &menuList[MENU_INDEX_MAIN])
			{
				pCurrentMenuPage = &menuList[MENU_INDEX_MAIN];
				pCurrentMenuPage->currentItem = 1;
			}		
			defaultMenuPageProcess(type,value);	
	        break;
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
	
}

void fanControlPageProcess(int type, int value)
{
	//printf("fanControlPageProcess(),type:%d,menuState:%d\r\n",type,menuState);
	switch(type)
	{
		case MENU_EVENT_ENTER:
			if(readFansControlEnable)
			{
				//refleshMainBoardInfo_fans();
				readFansControlEnable = 0;
			}
			defaultMenuPageProcess(type,value);	
	        break;
		case MENU_EVENT_EXIT:
			defaultMenuPageProcess(type,value);	
	        break;
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
}

void inputMenuPageProcess(int type, int value)
{
	switch(type)
	{
		case MENU_EVENT_ENTER:

			defaultMenuPageProcess(type,value);	
	    break;
		case MENU_KEY_NEXT:
            if(menuIsCurrentPageIndex(MENU_INDEX_INPUT_INFO) == TRUE)
            {
                switch(pCurrentMenuPage->currentItem)
                {
                    case 1:
                        detailSource = 1;
                        break;
                    case 2:
                        detailSource = 2;
                        break;
                    case 3:
                        detailSource = 3;
                        break;
                    default:
                        break;
                }
                defaultMenuPageProcess(type,value);
                break;
            }
			else if(menuIsCurrentPageIndex(MENU_INDEX_INPUT_HDMI) == TRUE)
			{	
				detailSource = 1;
				setValue(edidDetinationPortValue,EDID_DES_PORT_HDMI);				
			}
			else if(menuIsCurrentPageIndex(MENU_INDEX_INPUT_DVI)== TRUE)
			{
				detailSource = 2;
				setValue(edidDetinationPortValue,EDID_DES_PORT_DVI);
			}
			else if(menuIsCurrentPageIndex(MENU_INDEX_INPUT_SDI)== TRUE)
			{
				detailSource = 3;
			}
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
}

void audioMenuPageProcess(int type, int value)
{
	//printf("audioMenuPageProcess(),type:%d,menuState:%d\r\n",type,menuState);
	switch(type)
	{
		case MENU_EVENT_ENTER:
			//printf("MENU_EVENT_ENTER\r\n");
			pCurrentMenuPage->menuTtemSize = 5;

		default:
			defaultMenuPageProcess(type,value);
			break;
	}
}

void audioInputConfigPageProcess(int type, int value)//只针对外扩主板V1.3及以上
{
	//printf("audioInputConfigPageProcess(),type:%d,menuState:%d\r\n",type,menuState);
	switch(type)
	{
		case MENU_EVENT_ENTER:
			defaultMenuPageProcess(type,value);	
	        break;
			
		case MENU_KEY_NEXT:
			if(menuState == MENU_STATE_CHANGE_AND_SET_VALUE)
			{
			}
			else if(menuState == MENU_STATE_CHANGE_ITEM)
			{
				//printf("__MENU_STATE_CHANGE_ITEM,currentItem = %d",pCurrentMenuPage->currentItem);
				refleshAudioInputEnable(pCurrentMenuPage->currentItem-1);
			}
			defaultMenuPageProcess(type,value);	
	        break;
		case MENU_EVENT_EXIT:
			defaultMenuPageProcess(type,value);	
	        break;
		default:
			defaultMenuPageProcess(type,value);
			break;
	}
}


void outputFormatPageProcess(int type, int value)
{
	switch(type)
	{
		case MENU_EVENT_ENTER:
		default:
			defaultMenuPageProcess(type,value);
		break;
	}
}	


void outFormatPageProcess(int type, int value)
{
    char buffer[40];
    const MenuPage *menuPage;
    int item;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;

    switch(type)
    {
    case MENU_EVENT_ENTER:
		pCurrentMenuPage->currentItem = 2;
		defaultMenuPageProcess(type, value);
		if(ifCustomFormat())
		{
			menuState = MENU_STATE_CHANGE_VALUE;
		}
		else
		{
			menuState = MENU_STATE_SET_VALUE;
		}
		break;
	case MENU_KEY_MENU:
		if((menuState == MENU_STATE_SELECT_ITEM) || (menuState == MENU_STATE_SET_VALUE))
		{
			menuState = MENU_STATE_CHANGE_ITEM;
		}
		defaultMenuPageProcess(type, value);
		break;
    case MENU_EVENT_DRAW_ITEM:
        if(menuPage[value].type == MENU_TYPE_VALUE)
        {
            if(value == item)
            {
            	sprintf(buffer, "%s%s", menuStateStr[menuState],valueToString(menuPage[value].value,menuValueList[value]));                
            }           
            else
            {
                sprintf(buffer, "%s%s", MENU_NO_SELECT,valueToString(menuPage[value].value,menuValueList[value]));
            }
            LCD_WriteLine(value-((value-1)/MENU_DISPLAY_LINE_NUM*MENU_DISPLAY_LINE_NUM)-1, buffer);
        }
		else
		{
			defaultMenuPageProcess(type, value);
		}
        break;
	case MENU_EVENT_SET_VALUE:
        setValue(menuPage[item].value, menuValueList[item]);
        pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_DRAW_ITEM, item);
        break;
    default:
        changeNoSetProcess(type, value);
        break;
    }
}

u8 CalcSendCardOffsetPort(u8 currentItem,u8 * itemMin,u8 * itemMax)
{
    u8 portx = 0;
    u8 min = 0;
    u8 max = 0;

    switch(currentItem)
    {
        case 0:     // MENU_TYPE_LABEL
            break;
        case 1:     // xposvalue
        case 2:     // yposvalue
            portx = 0;
            min = 1;
            max = 2;
            break;
            
        case 3:     // xposvalue
        case 4:     // yposvalue
            portx = 2;
            min = 3;
            max = 4;           
            break;      
        default:
            break;
    }

    if(itemMin != NULL)
    {
        *itemMin = min;
    }
    if(itemMax != NULL)
    {
        *itemMax = max;
    }

    return portx;
}


u8 CalcSendCardPositionPort(u8 currentItem,u8 * itemMin,u8 * itemMax)
{
    u8 portx = 0;
    u8 min = 0;
    u8 max = 0;

    switch(currentItem)
    {
        case 0:     // MENU_TYPE_LABEL
            break;
        case 1:     // sendCardPortHPosValue
        case 2:     // sendCardPortVPosValue
            portx = 0;
            min = 1;
            max = 2;
            break;
        
        case 3:     // sendCardPortHPosValue
        case 4:     // sendCardPortVPosValue
            portx = 1;
            min = 3;
            max = 4;           
            break;  
        case 5:     // sendCardPortHPosValue
        case 6:     // sendCardPortVPosValue
            portx = 2;
            min = 5;
            max = 6;           
            break;  
        case 7:     // sendCardPortHPosValue
        case 8:     // sendCardPortVPosValue
            portx = 3;
            min = 7;
            max = 8;           
            break;              
        default:
            break;
    }

    if(itemMin != NULL)
    {
        *itemMin = min;
    }
    if(itemMax != NULL)
    {
        *itemMax = max;
    }

    return portx;
}

u8 CalcSendCardPort(u8 currentItem,u8 * itemMin,u8 * itemMax)
{

	return currentItem;
}

u8 CalcSendCardNormalPort(u8 currentItem,u8 * itemMin,u8 * itemMax)
{
	return currentItem;
}

u8 CalcSendCardAbNormalPort(u8 currentItem,u8 * itemMin,u8 * itemMax)
{

	return currentItem;
}



void MenuPage_testGrid_Process(int type, int value)
{
 
}



void MenuPageProcesschange(int type, int value)
{

	
}

void changeNoSetProcess_II(int type, int value)
{
    const MenuPage *menuPage;
    int item;

    item = pCurrentMenuPage->currentItem;
    menuPage = pCurrentMenuPage->menuPage;
    switch(type)
    {
    case MENU_EVENT_NEXT_VALUE:
        if(menuPage[item].type == MENU_TYPE_VALUE)
        {
            int index;
            index = menuPage[item].value;
            menuState = MENU_STATE_CHANGE_VALUE;
            menuValueList[item] = nextValue(index,menuValueList[item]);
            menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
        }
        break;
    case MENU_EVENT_PREV_VALUE:
        if(menuPage[item].type == MENU_TYPE_VALUE)
        {
            int index;
            index = menuPage[item].value;
            menuState = MENU_STATE_CHANGE_VALUE;
            menuValueList[item] = prevValue(index,menuValueList[item]);
            menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
        }
        break;
    case MENU_EVENT_SELECT_ITEM:
        if(menuState == MENU_STATE_CHANGE_VALUE)
        {
            setValue(menuPage[item].value, menuValueList[item]);
            menuState = MENU_STATE_SET_VALUE;
        }
        menuPage[item].pfProcess(MENU_EVENT_DRAW_ITEM, item);
        break;
    default:
        defaultMenuPageProcess(type, value);
        break;
    }
}



const MenuPage menuNaPage[] =
{
	{MENU_TYPE_LABEL,   spaceStr,   	0,             			noYesPageProcess},
	{MENU_TYPE_LABEL,   naStr,    		0,   					noYesPageProcess},
};

const MenuPage menuInputDetailPage[] =
{
	{MENU_TYPE_LABEL,   inputDetailStr,   	0,                	inputDetailPageProcess},
	{MENU_TYPE_READ,   	spaceStr,  			inputSourceValue,   inputDetailPageProcess},
};


const MenuPage menuZoomPage[] =
{
	{MENU_TYPE_LABEL,   			 zoomStr,   		0,                	zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hSizeStr,			zoomWidthValue,		zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vSizeStr,			zoomHeightValue,	zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hPosStr,			zoomPosxValue,		zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vPosStr,			zoomPosyValue,		zoomMenuPageProcess},
	{MENU_TYPE_VALUE,   			 resetStr,			resetZoomValue,   	resetMenuPageProcess},
};

const MenuPage menuInputCropPage[] =
{
	{MENU_TYPE_LABEL,   			 zoomStr,   		0,                	 enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hSizeStr,			cropInputWidthValue, enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vSizeStr,			cropInputHeightValue,enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hPosStr,			cropInputPosxValue,	 enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vPosStr,			cropInputPosyValue,	 enterNumberPageProcess},
	{MENU_TYPE_VALUE,   			 resetStr,			resetCropInputValue, resetMenuPageProcess},
};

const MenuPage menuVgaAdjustPage[] =
{
	{MENU_TYPE_LABEL,   vgaAdjustStr,   			0,          defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	autoAdjustStr,		vgaAutoAdjustValue, resetMenuPageProcess},
	{MENU_TYPE_VALUE,   hPosStr,			vgaPosxValue,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   vPosStr,			vgaPosyValue,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   clockStr,			vgaClockValue,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   phaseStr,			vgaPhaseValue,		defaultMenuPageProcess},
};

const MenuPage menuSelectVGAAdjustPage[] =
{
	{MENU_TYPE_LABEL,	vgaAdjustStr,  		(int)0,					vgaSelectMenuPageProcess},
};

const MenuPage menuAdcAutoAdjustPage[] =
{
	{MENU_TYPE_LABEL,	adcAutoAdjustStr,		0,						noYesPageProcess},
	{MENU_TYPE_LABEL,	adcAutoAdjustStr,		0,						noYesPageProcess},
	{MENU_TYPE_VALUE,	yesOrNoStr,				adcAutoAdjustValue,		noYesPageProcess},
};

const MenuPage menuAdcResetAllPage[] =
{
	{MENU_TYPE_LABEL,	adcResetAllStr,		0,						noYesPageProcess},
	{MENU_TYPE_LABEL,	adcResetAllStr,		0,						noYesPageProcess},
	{MENU_TYPE_VALUE,	yesOrNoStr,			resetAdcSettingValue,	noYesPageProcess},
};

const MenuPage menuAdcAdjustPage[] =
{
	{MENU_TYPE_LABEL,	adcAdjustStr,		0,								defaultMenuPageProcess},
};



const MenuPage menuSdiAdjustPage[] =
{
	{MENU_TYPE_LABEL,	SdiAdjustStr,		0,						defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	hPosStr,			inputHStartValue,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	vPosStr,			inputVStartValue,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	resetStr,			resetSdiInputValue,		resetMenuPageProcess},
};

const MenuPage menuHdmiAdjustPage[] = 
{
	{MENU_TYPE_LABEL,	SdiAdjustStr,		0,						defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	ttlInterlaveModeStr,ttlInterlaveModeValue,	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	resetStr,			resetSdiInputValue,		resetMenuPageProcess},
};

const MenuPage menuZoomNewPage[] =
{

	{MENU_TYPE_LABEL,   			 zoomStr,   				0,                	zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   zoomNewVUpStr,				cropUpValue,		zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   zoomNewVDownStr,			cropDownValue,		zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   zoomNewVUpAndDownStr,		cropUpDownValue,	zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   zoomNewHLeftStr,			cropLeftValue,		zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 	 zoomNewHRightStr,			cropRightValue,   	zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   zoomNewHLeftAndRightStr,	cropLeftRightValue,	zoomMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   zoomNewCenterStr,			cropCenterValue,	zoomMenuPageProcess},
	{MENU_TYPE_VALUE,   			 resetStr,			 		resetCropValue,   	resetMenuPageProcess},
};


#if 0
const MenuPage menuEDIDPageHdmi2_0[] = 
{
	{MENU_TYPE_LABEL,						edidManageStr,		0,						 		defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,			customerEdidStr,	hdmi20PortCustomEdidValue,	 	enterNumberPageProcess},
	{MENU_TYPE_VALUE,						hdmi20EdidStr, 		hdmi20PortHdmi20EdidValue, 		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,						hdmi20VideoEdidStr, hdmi20PortHdmi20VideoEdidValue, defaultMenuPageProcess},
	{MENU_TYPE_VALUE,						hdmi14EdidStr,		hdmi20PortHdmi14EdidValue,	 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,						hdmi13EdidStr,		hdmi20PortHdmi13EdidValue,	 	defaultMenuPageProcess},
	
};
#else
const MenuPage menuEDIDPageHdmi2_0[] = 
{
	{MENU_TYPE_LABEL,						edidManageStr,				0,						 			defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,			customerEdidStr,			hdmi20PortCustomEdidValue,	 		enterNumberPageProcess},
	{MENU_TYPE_VALUE,						hdmi20EdidStr, 				hdmi20PortHdmi20EdidValue, 			defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,						hdmi20VideoEdidStr, 		hdmi20PortHdmi20VideoEdidValue, 	defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,						hdmi20NoHfvsdbNo420, 		hdmi20PortHdmi20NoHfvsdbNo420, 		defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,						hdmi20NoHfvsdbWith420, 		hdmi20PortHdmi20NoHfvsdbWith420, 	defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,						hdmi20WithHfvsdbNo420, 		hdmi20PortHhdmi20WithHfvsdbNo420, 	defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,						hdmi20WithHfvsdbNo420Hdr, 	hdmi20PortHdmi20WithHfvsdbNo420Hdr, defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,						hdmi20WithHfvsdbWith420, 	hdmi20PortHdmi20WithHfvsdbWith420, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,						hdmi14EdidStr,				hdmi20PortHdmi14EdidValue,	 		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,						hdmi13EdidStr,				hdmi20PortHdmi13EdidValue,	 		defaultMenuPageProcess},
	
};
#endif

const MenuPage menuEDIDPageHdmi1_4[] = 
{
	{MENU_TYPE_LABEL,	edidManageStr,		0,						 defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	customerEdidStr, hdmi14PortCustomEdidValue, enterNumberPageProcess},
	{MENU_TYPE_VALUE,	hdmi14EdidStr,		hdmi14PortHdmi14EdidValue,	 defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	hdmi13EdidStr,		hdmi14PortHdmi13EdidValue,	 defaultMenuPageProcess},
};

const MenuPage menuEDIDPageDp[] = 
{
	{MENU_TYPE_LABEL,						edidManageStr,		0,						 	defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,			customerEdidStr, 	dpPortCustomEdidValue, 		enterNumberPageProcess},
	{MENU_TYPE_VALUE,						dp4k2k60EdidStr,	dpPort4k2k60Value,	 		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,						dp4k2k30EdidStr,	dpPort4k2k30Value,	 		defaultMenuPageProcess},
};

const MenuPage menuEDIDPageDvi[] = 
{
	{MENU_TYPE_LABEL,				edidManageStr,			0,						 		defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	customerEdidStr, 		dviPortCustomEdidValue, 		enterNumberPageProcess},
	{MENU_TYPE_VALUE,				dualLinkDviEdidStr,		dviPortDualLinkDviEdidValue,	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,				dviEdidStr,				dviPortDviEdidValue,	 		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,				hdmi13EdidStr,			dviPortHdmi13EdidValue,	 		defaultMenuPageProcess},
	
};		


#if 1
const MenuPage menuEDIDPage[] = 
{
	{MENU_TYPE_LABEL,	edidManageStr,		0,						 		defaultMenuPageProcess},
	{MENU_TYPE_DIR,		hdmi20PortStr, 		(int)menuEDIDPageHdmi2_0, 		defaultMenuPageProcess},
	{MENU_TYPE_DIR,		hdmi14PortStr,		(int)menuEDIDPageHdmi1_4,	 	defaultMenuPageProcess},
	{MENU_TYPE_DIR,		dpPortStr,			(int)menuEDIDPageDp,	 		defaultMenuPageProcess},
	{MENU_TYPE_DIR,		dviPortStr,			(int)menuEDIDPageDvi,	 		defaultMenuPageProcess},
};

#else
#endif

const MenuPage customEDIDPage[] =
{
	{MENU_TYPE_LABEL,   customFormatStr,    0,       							customEDIDPageProcess},
	//{MENU_TYPE_LABEL,   customFormatStr,    0,       							customEDIDPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   nullStr,       customEDIDValue,           	customEDIDPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   nullStr,       customEDIDValue,           	customEDIDPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   nullStr,       customEDIDValue,           	customEDIDPageProcess},
	//{MENU_TYPE_ENTER_NUMBER_VALUE,   nullStr,       customEDIDValue,           	customEDIDPageProcess},
	//{MENU_TYPE_LABEL,	beyondCustomFormatStr,	 0, 							customEDIDPageProcess },
};


const MenuPage menuInputHdmiPage[] = 
{
    {MENU_TYPE_LABEL,   hdmiStr,   	0,                			inputMenuPageProcess},
	{MENU_TYPE_DIR, 	autoStr, (int)menuInputDetailPage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	edidManageStr,	(int)menuEDIDPage,			defaultMenuPageProcess},   
};


const MenuPage menuInputDviPage[] = 
{
    {MENU_TYPE_LABEL,   dviStr,   	0,                			inputMenuPageProcess},
	{MENU_TYPE_DIR, 	autoStr, (int)menuInputDetailPage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	edidManageStr,	(int)menuEDIDPage,			defaultMenuPageProcess},   
};

const MenuPage menuInputSdiPage[] = 
{
    {MENU_TYPE_LABEL,   sdiStr,   	0,                			inputMenuPageProcess},
	{MENU_TYPE_DIR, 	autoStr, (int)menuInputDetailPage,	defaultMenuPageProcess},
};


const MenuPage menuInputInfoPage[] =
{
    {MENU_TYPE_LABEL,   inputDetailStr,   	0,                			inputMenuPageProcess},//inputMenuPageProcess
	{MENU_TYPE_DIR,   	hdmiStr,	(int)menuInputDetailPage,  	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	dviStr,		(int)menuInputDetailPage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR,   	sdiStr,		(int)menuInputDetailPage,  	defaultMenuPageProcess},
};

const MenuPage menuInputPage[] =
{
    {MENU_TYPE_LABEL,   inputStr,   	0,                			inputMenuPageProcess},//inputMenuPageProcess
	{MENU_TYPE_DIR,   	inputDetailStr,	(int)menuInputInfoPage,  	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	zoomStr,		(int)menuZoomNewPage,		defaultMenuPageProcess},//lv@20160715
};


const MenuPage menuTemperaturePage[] =
{
    {MENU_TYPE_LABEL,   temperatureStr,   	0,                			defaultMenuPageProcess},//inputMenuPageProcess
	{MENU_TYPE_VALUE,   	temperatureStr,	(int)temperatureModeValue,  	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   	temperatureStr,	(int)temperatureValue,  	defaultMenuPageProcess},	
};

// OUTPUT

const MenuPage menuDviOutDetailPage[] =
{
	{MENU_TYPE_LABEL,   dviStr,    		0,       			defaultMenuPageProcess},
	{MENU_TYPE_READ,   	formatStr, 		standardFormatValue, outputDetailPageProcess },//defaultMenuPageProcess//outputFormatValue
	{MENU_TYPE_READ,   	dviModeStr,    	dviHdmiValue,  		outputDetailPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ,	bitDepthStr,	bitDepthValue,		outputDetailPageProcess},   //added by julia 
	{MENU_TYPE_READ,   	dataRangeStr,   dataRangeValue,  	outputDetailPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ,   	deStr,    		deOnOffValue,  		outputDetailPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ,   	deHPosStr,    	dePosxValue,  		outputDetailPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ,   	deVPosStr,    	dePosyValue,  		outputDetailPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ,   	deHSizeStr,    	deWidthValue,  		outputDetailPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ,   	deVSizeStr,    	deHeightValue,  	outputDetailPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ,    hPolarityStr,  	deHPalarityValue, 	outputDetailPageProcess},
	{MENU_TYPE_READ,    vPolarityStr,  	deVPalarityValue, 	outputDetailPageProcess},
};

const MenuPage menuOutputDetailPage[] =
{
	{MENU_TYPE_LABEL,   outputDetailStr,    0,       			dviSelectMenuPageProcess},
	{MENU_TYPE_DIR,   	dviStr,    (int)menuDviOutDetailPage,   dviSelectMenuPageProcess},
	{MENU_TYPE_DIR,   	dviStr,    (int)menuDviOutDetailPage,   dviSelectMenuPageProcess},
};


#if 0

#else

const MenuPage customFormatPage[] =
{
	{MENU_TYPE_LABEL,   			customFormatStr,    0,       					customFormatPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   nullStr,       	customHValue,           	customFormatPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   nullStr,       	customVValue,           	customFormatPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   nullStr,       	customFValue,           	customFormatPageProcess},

};

#endif

const MenuPage standardFormatPage[] =
{
	{MENU_TYPE_LABEL,   outputFormatStr,    0,       			standardFormatPageProcess},
	{MENU_TYPE_LABEL,   outputFormatStr,	0,   				standardFormatPageProcess},
	{MENU_TYPE_VALUE,   spaceStr,		standardFormatValue,  	standardFormatPageProcess},
};
const MenuPage otherFormatPage[] =
{
	{MENU_TYPE_LABEL,   outputFormatStr,    0,       			standardFormatPageProcess},
	{MENU_TYPE_LABEL,   outputFormatStr,	0,   				standardFormatPageProcess},
	{MENU_TYPE_VALUE,   spaceStr,		otherFormatValue,  		standardFormatPageProcess},
};
#if 1
const MenuPage outputFormatPage[] =
{
	{MENU_TYPE_LABEL,   outputFormatStr,    0,       			defaultMenuPageProcess},
	{MENU_TYPE_DIR,     standardFormatStr,	(int)standardFormatPage,defaultMenuPageProcess},
	{MENU_TYPE_DIR,     customFormatStr,	(int)customFormatPage,  defaultMenuPageProcess},
};
#else

#endif
const MenuPage menuDeAdjustPage[] =
{
	{MENU_TYPE_LABEL,     spaceStr,      	0,       			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,     deStr,  	 		deOnOffValue, 		changeNoSetProcess},    
    {MENU_TYPE_VALUE,     hPosStr,  		dePosxValue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,     vPosStr,  		dePosyValue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,     hSizeStr,  		deWidthValue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,     vSizeStr,  		deHeightValue, 		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,    hPolarityStr,  	deHPalarityValue, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,    vPolarityStr,  	deVPalarityValue, 	defaultMenuPageProcess},
};

const MenuPage menuDviOutConfigPage[] =
{
	{MENU_TYPE_LABEL,     spaceStr,    		0,       				defaultMenuPageProcess},
	{MENU_TYPE_VALUE,     dviModeStr,  		dviHdmiValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	  bitDepthStr, 		bitDepthValue, 			defaultMenuPageProcess},   //added by julia 
	{MENU_TYPE_VALUE,     dataRangeStr,  	dataRangeValue, 		defaultMenuPageProcess},
	{MENU_TYPE_DIR,       deAdjustStr,  	(int)menuDeAdjustPage, 	defaultMenuPageProcess}, 
    {MENU_TYPE_VALUE,     resetStr,  		resetDviValue, 			resetMenuPageProcess},    
};

const MenuPage menuDviOutLoopConfigPage[] =
{
	{MENU_TYPE_LABEL,     spaceStr,    		0,       				defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   dvi1OutputStr,		dvi1OutputValue,		changeNoSetProcess},//defaultMenuPageProcess
	{MENU_TYPE_VALUE,     dviModeStr,  		dviHdmiValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	  bitDepthStr, 		bitDepthValue, 			defaultMenuPageProcess},   //added by julia 
	{MENU_TYPE_VALUE,     dataRangeStr,  	dataRangeValue, 		defaultMenuPageProcess},
	{MENU_TYPE_DIR,       deAdjustStr,  	(int)menuDeAdjustPage, 	defaultMenuPageProcess}, 
    {MENU_TYPE_VALUE,     resetStr,  		resetDviValue, 			resetMenuPageProcess},    
};

const MenuPage menuOutputAdjustPage[] =
{
	{MENU_TYPE_LABEL,	outputAdjustStr,  		0, 						dviSelectMenuPageProcess},
	{MENU_TYPE_DIR, 	dviStr, 			(int)menuDviOutLoopConfigPage,	dviSelectMenuPageProcess},
	{MENU_TYPE_DIR,   	dviStr,    			(int)menuDviOutConfigPage,  dviSelectMenuPageProcess},
};
#if 0//lv@20160710

#else
const MenuPage menuScalePage[] =
{
	{MENU_TYPE_LABEL,   				scaleStr,    	0,       			scaleMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	hSizeStr,			scaleWidthValue,   	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	vSizeStr,			scaleHeightValue,   enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	 	HVSizeStr,			scaleWidthAndHeightValue,	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	hPosStr,			scalePosxValue,   	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	vPosStr,			scalePosyValue,   	enterNumberPageProcess},
	{MENU_TYPE_VALUE,   				windowRatioStr,		windowRatioValue,   	changeNoSetProcess},
	{MENU_TYPE_VALUE,   				resetStr,			resetScaleValue,   	resetMenuPageProcess},
};
#endif

const MenuPage menuFitAdvancedPage[] =
{
	{MENU_TYPE_LABEL,   scaleStr,    	0,       			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   scaleModeStr,	scaleModeValue,   	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	zoomStr,		(int)menuZoomNewPage,		defaultMenuPageProcess},//lv@20160715
};


const MenuPage menuFitRatioPage[] =
{
	{MENU_TYPE_LABEL,   windowRatioStr,    	0,       			MenuPageLockPanelProcess},
	{MENU_TYPE_VALUE,   windowRatioStr,		windowRatioValue,   	MenuPageLockPanelProcess},
};
#if 0
#else
const MenuPage menuOutputFitPage[] =
{
	{MENU_TYPE_LABEL,   scaleStr,    	0,       			MenuPageLockPanelProcess},
	{MENU_TYPE_DIR, 	scaleStr,		(int)menuScalePage,			MenuPageLockPanelProcess},
};
#endif

const MenuPage menuScreenPage[] =
{
	{MENU_TYPE_LABEL,   screenStr,    	0,       						enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hSizeStr,		screenWidthValue,   enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vSizeStr,		screenHeightValue,  enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hPosStr,		screenPosxValue,   	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vPosStr,		screenPosyValue,   	enterNumberPageProcess},
	{MENU_TYPE_VALUE,   			modeStr,		screenModeValue,   	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,				screenFollowStr,screenFollowValue,	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   			resetStr,		resetScreenValue,   	resetMenuPageProcess},
};
const MenuPage menuScreenHotKeyPage[] =
{
	{MENU_TYPE_LABEL,   			 screenStr,    	0,       			enterNumberPageProcess},
	{MENU_TYPE_VALUE,				 modeStr,		screenModeValue,	defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hSizeStr,		screenWidthValue,   enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vSizeStr,		screenHeightValue,  enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   hPosStr,		screenPosxValue,   	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   vPosStr,		screenPosyValue,   	enterNumberPageProcess},
	{MENU_TYPE_VALUE,				screenFollowStr,screenFollowValue,	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,  				 resetStr,		resetScreenValue,  	resetMenuPageProcess},
};

const MenuPage menuImageColorTempPage[] =
{
	{MENU_TYPE_LABEL,   pictureStr,   	0,                	enterNumberPageProcess},
	{MENU_TYPE_VALUE,   colorTempStr,	colorTempValue,     	enterNumberPageProcess},
    {MENU_TYPE_ENTER_NUMBER_VALUE,   colorTempRedStr,	colorTempRedValue,	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   colorTempGreenStr,	colorTempGreenValue,enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   colorTempBlueStr,	colorTempBlueValue,	enterNumberPageProcess},
};


const MenuPage menuImageQualityPage[] =
{
	{MENU_TYPE_LABEL,   				pictureStr,   		0,                		defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	brightnessStr,		brightnessValue,		enterNumberPageProcess},
    {MENU_TYPE_ENTER_NUMBER_VALUE,   	contrastStr,		contrastValue,     		enterNumberPageProcess},
    {MENU_TYPE_ENTER_NUMBER_VALUE,   	colorEffectStr,		colorEffectValue,   	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   				colorTempStr,		colorTempValue,			defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	colorTempRedStr,	colorTempRedValue,		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	colorTempGreenStr,	colorTempGreenValue,	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	colorTempBlueStr,	colorTempBlueValue,		enterNumberPageProcess},
	{MENU_TYPE_VALUE,					gammaStr,			gammaValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   				resetStr,			resetPictureValue,		resetMenuPageProcess},
};


const MenuPage menuTextOverlayPage[] =
{
	{MENU_TYPE_LABEL,   textOverlayStr,   	0,                	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   textOverlayStr,   	textEnableValue,    defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   presetStr,   		textPresetValue,    defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   blendModeStr,   	textModeValue,    	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   blendLevelStr,   	textAlphaValue,    	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   aboveBelowStr,   	textAboveValue,    	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   andOrStr,   		textAndOrValue,    	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   redStr,   			textRedValue,    	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   greenStr,   		textGreenValue,    	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   blueStr,   			textBlueValue,    	defaultMenuPageProcess},
};


const MenuPage menuAlphaPage[] = 
{
	{MENU_TYPE_LABEL,   alphaStr,      	0,       				defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   alphaStr,  		alphaValue, 			defaultMenuPageProcess},
};

const MenuPage menuFlatColorPage[] = 
{
	{MENU_TYPE_LABEL,   flatColorStr,      	0,       		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   redStr,  			flatRedValue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   greenStr,  			flatGreenValue, defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   blueStr,  			flatBlueValue, 	defaultMenuPageProcess},
};


const MenuPage menuTestPatternPage[] =
{
	{MENU_TYPE_LABEL,   testPatternStr,     0,       				testPatternMenuPageProcess},
    {MENU_TYPE_VALUE,   tpSwitchStr,  	tpSwitchValue, 	testPatternMenuPageProcess},         
    {MENU_TYPE_VALUE,   testPatternStr,  	testPatternModeValue, 	testPatternMenuPageProcess},
    {MENU_TYPE_VALUE,   autoTpTimeStr,  	autoTpIimeValue, 		testPatternMenuPageProcess},
    
};

const MenuPage menuTpColorPage[] =
{
	{MENU_TYPE_LABEL,   testPatternStr,     0,       				testPatternMenuPageProcess},
     {MENU_TYPE_VALUE,   tpSwitchStr,  	tpSwitchValue, 	testPatternMenuPageProcess},            
	{MENU_TYPE_VALUE,   testPatternStr,  	testPatternModeValue, 	testPatternMenuPageProcess},
	{MENU_TYPE_VALUE,   autoTpTimeStr,  	autoTpIimeValue, 		changeNoSetProcess},//testPatternMenuPageProcess
    {MENU_TYPE_VALUE,   redStr,  			tpColorRedValue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   greenStr,  			tpColorGreenValue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   blueStr,  			tpColorBlueValue, 		defaultMenuPageProcess},
};

const MenuPage menuTpParamPage[] =
{
	{MENU_TYPE_LABEL,   testPatternStr,     0,       				testPatternMenuPageProcess},
    {MENU_TYPE_VALUE,   tpSwitchStr,  	tpSwitchValue, 	testPatternMenuPageProcess},             
	{MENU_TYPE_VALUE,   testPatternStr,  	testPatternModeValue, 	testPatternMenuPageProcess},
	{MENU_TYPE_VALUE,   autoTpTimeStr,  	autoTpIimeValue, 		testPatternMenuPageProcess},
    {MENU_TYPE_VALUE,   tpHorStepStr,  		tpHorStepValue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   tpVerStepStr,  		tpVerStepValue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   tpStepColorStr,  	tpStepColorValue, 		defaultMenuPageProcess},
};

const MenuPage menuDisplayModePage[] =
{
	{MENU_TYPE_LABEL,   displayModeStr,     0,       				displayModePageProcess},//defaultMenuPageProcess
    {MENU_TYPE_VALUE,   modeStr,  			displayModeValue, 		changeNoSetProcess},
    {MENU_TYPE_DIR,   	testPatternStr,  	(int)menuTestPatternPage, 	displayModePageProcess},//defaultMenuPageProcess
    {MENU_TYPE_DIR,     flatColorStr,  		(int)menuFlatColorPage, displayModePageProcess},//defaultMenuPageProcess
};	

#if 0
#else
const MenuPage menuPicturePage[] = 
{
	{MENU_TYPE_LABEL,   pictureStr,   	0,                			defaultMenuPageProcess},
	//{MENU_TYPE_DIR,   displayModeStr,	(int)menuDisplayModePage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR,   imageQualityStr,	(int)menuImageQualityPage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR,	  textOverlayStr,	(int)menuTextOverlayPage,	defaultMenuPageProcess},	
};
#endif

#if 0

#else

#if 0

#endif

const MenuPage menuOutputPage[] =
{
    {MENU_TYPE_LABEL,   	outputFormatStr,      	0,       												defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1280x720_50Str,  	output1280x720x50Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1360x768_60Str,  	output1360x768x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1400x1050_60Str,  output1400x1050x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1440x900_60Str,  	output1440x900x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1600x1200_60Str,  output1600x1200x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1680x1050_60Str,  output1680x1050x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1920x1080_60Str,  output1920x1080x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output1920x1200_60Str,  output1920x1200x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output2048x1152_60Str,  output2048x1152x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,     	output2560x812_60Str,  	output2560x812x60Value, 								defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,     	outputCustomStr,  		outputCustomValue, 								defaultMenuPageProcess},
};



const MenuPage menuLEDPage[] =
{
    {MENU_TYPE_LABEL,   outputStr,      0,       						defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	standardFormatStr,	(int)standardFormatPage,defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	customFormatStr,	(int)customFormatPage,	defaultMenuPageProcess},
};
#endif





// TRANSITION

const MenuPage menuTransitionPage[] =
{

	{MENU_TYPE_LABEL,   transitionStr,      0,       			transitionPageProcess},
	{MENU_TYPE_VALUE,	interlaceSwitchStr, interlaceSwitchValue,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	ImageEnhanceStr, 	imageEnhanceValue,		transitionPageProcess},
    {MENU_TYPE_VALUE,   fadeTypeStr,  			switchModeValue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   fadeTimeStr,  		fadeTimeValue, 		defaultMenuPageProcess},

};






const MenuPage menuImageEnhanceVerityPage[] =
{
	{MENU_TYPE_LABEL,   interlaceSwitchStr, 0,             		noYesPageProcess},
	{MENU_TYPE_LABEL,	imageEnhanceWarnStr1, 0,					noYesPageProcess},
    {MENU_TYPE_LABEL,   imageEnhanceWarnStr2, 0,   				noYesPageProcess},
    {MENU_TYPE_LABEL,   imageEnhanceWarnStr3, 0,   				noYesPageProcess},   
    
    {MENU_TYPE_VALUE,   yesOrNoStr,imageEnhanceVerifyValue,  noYesPageProcess},
};





// AUDIO
const MenuPage menuAudioInputConfigPage[] =  ////只针对外扩主板V1.3及以上
{
	{MENU_TYPE_LABEL,   audioInputConfigStr,           0,       				audioInputConfigPageProcess},//defaultMenuPageProcess
    {MENU_TYPE_VALUE,   audioInput1Str,  			audioInput1Value, 			audioInputConfigPageProcess},//defaultMenuPageProcess//changeNoSetProcess    
    {MENU_TYPE_VALUE,   audioInput2Str,  			audioInput2Value, 			audioInputConfigPageProcess},
    {MENU_TYPE_VALUE,   audioInput3Str,  			audioInput3Value, 			audioInputConfigPageProcess},
};




const MenuPage menuAudioPage[] =
{
	{MENU_TYPE_LABEL,   audioStr,           0,       				audioMenuPageProcess},//defaultMenuPageProcess
    {MENU_TYPE_VALUE,   muteStr,  			muteOnOffValue, 		changeNoSetProcess},    
    {MENU_TYPE_VALUE,   volumeStr,  		volumeValue, 			defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   audioInStr,  		audioModeValue, 		changeNoSetProcess},
    {MENU_TYPE_VALUE,   hdmiAndExtAudioStr,  hdmiAndExtAudioValue,changeNoSetProcess},
	{MENU_TYPE_DIR, 	audioInputConfigStr,(int)menuAudioInputConfigPage,defaultMenuPageProcess},	
};




// SPLIT
const MenuPage menuSplitPage[] =
{
	{MENU_TYPE_LABEL,	splitStr,			0,						splitDetailPageProcess},//enterNumberPageProcess
	{MENU_TYPE_VALUE,	splitStr,			splitOnOffValue, 		changeNoSetProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	hTotalSplitStr,			splitHTotalValue,			enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	vTotalSplitStr,			splitVTotalValue,			enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	hPosSplitStr,			splitHPosValue,				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	vPosSplitStr,			splitVPosValue,				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	hSizeSplitStr,			splitHSizeValue,			enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	vSizeSplitStr,			splitVSizeValue,			enterNumberPageProcess},
	{MENU_TYPE_DIR, 				advanceStr,				(int)menuInputCropPage, 	defaultMenuPageProcess},	
	{MENU_TYPE_VALUE,   			resetStr,				resetSplitValue,			resetMenuPageProcess},

};





const MenuPage menuSplitFixedPage[] =
{
	{MENU_TYPE_LABEL,	splitStr,			0,						splitDetailPageProcess},//enterNumberPageProcess
	{MENU_TYPE_VALUE,	splitStr,			splitOnOffValue,		changeNoSetProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	HorizontalNumberStr,splitFixedHNumberValue,	enterNumberPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_ENTER_NUMBER_VALUE,	VerticalNumberStr,	splitFixedVNumberValue,	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	splitIndexStr,		splitFixedIndexValue,	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	hSizeSplitStr,		scaleWidthValue,		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,	vSizeSplitStr,		scaleHeightValue,		enterNumberPageProcess},
	{MENU_TYPE_DIR, 				advanceStr,			(int)menuInputCropPage, defaultMenuPageProcess},	

};


const MenuPage menuSplitModePage[] =
{
	{MENU_TYPE_LABEL,	splitStr,			0,							splitModePageProcess},
	{MENU_TYPE_DIR, 	fixedSplitStr,	(int)menuSplitFixedPage,	    splitModePageProcess},
	{MENU_TYPE_DIR, 	normalSplitStr,	(int)menuSplitPage,				splitModePageProcess},
};


const MenuPage menuSaveToPage[] =
{
	{MENU_TYPE_LABEL,   saveToStr,   	0,             		saveToMenuPageProcess},
    {MENU_TYPE_LABEL,   saveToStr,    	0,   				saveToMenuPageProcess},
    {MENU_TYPE_VALUE,   spaceStr,   	saveToValue,   		saveToMenuPageProcess},
    {MENU_TYPE_LABEL,   saveToPageTipsStr1,    	0,   		saveToMenuPageProcess}, //added by cyj 
    {MENU_TYPE_LABEL,   saveToPageTipsStr2,    	0,   		saveToMenuPageProcess}, //added by cyj
};



const MenuPage menuUsbAllSavePage[] =
{
	{MENU_TYPE_LABEL,   usbAllSaveStr,   	0,             		saveToMenuPageProcess},
    {MENU_TYPE_LABEL,   usbAllSaveStr,    	0,   				saveToMenuPageProcess},
    {MENU_TYPE_VALUE,   spaceStr,   	usbAllSaveValue,   		saveToMenuPageProcess},
    {MENU_TYPE_LABEL,   saveToPageTipsStr1,    	0,   		saveToMenuPageProcess}, //added by cyj 
    {MENU_TYPE_LABEL,   saveToPageTipsStr2,    	0,   		saveToMenuPageProcess}, //added by cyj
};




const MenuPage menuSaveVerifyPage[] =
{
	{MENU_TYPE_LABEL,   			saveToRecoededWarnStr_beforeNum, 0,             	noYesPageProcess},
    {MENU_TYPE_LABEL_WITH_VALUE,   	saveToRecoededWarnStr_beforeNum, 0,   				noYesPageProcess},
    {MENU_TYPE_VALUE,   			yesOrNoStr,replaceRecoedValue,  					noYesPageProcess},
};


const MenuPage menuLoadFromPage[] =
{
	{MENU_TYPE_LABEL,	loadFromStr,	0,						loadFromMenuPageProcess},
	{MENU_TYPE_LABEL,	loadFromStr,	0,						loadFromMenuPageProcess},
	{MENU_TYPE_VALUE,	spaceStr,		loadFromValue,			loadFromMenuPageProcess},
    {MENU_TYPE_LABEL,   loadFromPageTipsStr1,    	0,   		loadFromMenuPageProcess}, //added by cyj
    {MENU_TYPE_LABEL,   loadFromPageTipsStr2,    	0,   		loadFromMenuPageProcess}, //added by cyj
};

const MenuPage menuUsbSingleLoadPage[] =
{
	{MENU_TYPE_LABEL,	usbLoadSingleStr,	0,					loadFromMenuPageProcess},
	{MENU_TYPE_LABEL,	usbLoadSingleStr,	0,					loadFromMenuPageProcess},
	{MENU_TYPE_VALUE,	spaceStr,		usbSingleLoadValue,		loadFromMenuPageProcess},
    {MENU_TYPE_LABEL,   loadFromPageTipsStr1,    	0,   		loadFromMenuPageProcess}, //added by cyj
    {MENU_TYPE_LABEL,   loadFromPageTipsStr2,    	0,   		loadFromMenuPageProcess}, //added by cyj
};

const MenuPage menuUsbAllLoadPage[] =
{
	
	{MENU_TYPE_LABEL,	usbLoadAllStr,	0,					loadFromMenuPageProcess},
	{MENU_TYPE_LABEL,	usbLoadAllStr,	0,					loadFromMenuPageProcess},
	{MENU_TYPE_VALUE,	spaceStr,		usbAllLoadValue,		loadFromMenuPageProcess},
    {MENU_TYPE_LABEL,   loadFromPageTipsStr1,    	0,   		loadFromMenuPageProcess}, //added by cyj
    {MENU_TYPE_LABEL,   loadFromPageTipsStr2,    	0,   		loadFromMenuPageProcess}, //added by cyj
};




const MenuPage menuSavePage[] =
{

	{MENU_TYPE_LABEL,   saveSetupStr,   	0,             		defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	saveToStr,    		(int)menuSaveToPage,   	defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	loadFromStr,    	(int)menuLoadFromPage,  defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	usbLoadSingleStr,    (int)menuUsbSingleLoadPage,defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	usbAllSaveStr,    	(int)menuUsbAllSavePage,defaultMenuPageProcess},     
};

const MenuPage menuSystemInfoPage[] =
{
	{MENU_TYPE_LABEL,	systemInfoStr,		0,					systemInfoPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ, 	mcuVersionStr,		mcuVersionValue,	systemInfoPageProcess},
	{MENU_TYPE_READ, 	videoVersionStr,	videoVersionValue,	systemInfoPageProcess},
	{MENU_TYPE_READ,	snStr,				snValue,			systemInfoPageProcess},
	{MENU_TYPE_READ,	ipStr,				ipAddress3Value,	systemInfoPageProcess},
};


const MenuPage menuSoftVersionPage[] =
{
	{MENU_TYPE_LABEL,		softVersionStr,			0,							systemInfoPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_READ, 		mcuVersionStr,				mcuVersionValue,			systemInfoPageProcess},
	{MENU_TYPE_READ, 		videoVersionStr,			videoVersionValue,			systemInfoPageProcess},
	{MENU_TYPE_READ,		fpgaVersionStr,				fpgaVersionValue,			systemInfoPageProcess},
	//{MENU_TYPE_READ,		ext1MainBoardSoftVersionStr,ext1MainBoardSoftVersionValue,			systemInfoPageProcess},
	//{MENU_TYPE_READ,		ext2MainBoardSoftVersionStr,ext2MainBoardSoftVersionValue,			systemInfoPageProcess},
};

const MenuPage menuHardVersionPage[] =
{
	{MENU_TYPE_LABEL,		hardVersionStr,			0,								MenuPage_Version_Process},//defaultMenuPageProcess
	{MENU_TYPE_READ,		mainBoaedVersionStr,		mainBoardHardVersionValue,		systemInfoPageProcess},	
	//{MENU_TYPE_READ,		extMainBoardVersionStr,		ext1BoardHardVersionValue,		systemInfoPageProcess},	
	//{MENU_TYPE_READ,		ext2MainBoardVersionStr,	ext2BoardHardVersionValue,		systemInfoPageProcess},	

};

const MenuPage menuVersionPage[] =
{
	{MENU_TYPE_LABEL,	versionStr,				0,					defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	softVersionStr,	(int)menuSoftVersionPage, 	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	hardVersionStr,	(int)menuHardVersionPage,	defaultMenuPageProcess},
};


const MenuPage menuTechSupportPage[] =
{
	{MENU_TYPE_LABEL,	techSupportStr,		0,					MenuPage_TechSupport_Process},
	{MENU_TYPE_LABEL, 	salesHotlineStr,	0,					MenuPage_TechSupport_Process},
	{MENU_TYPE_READ, 	0,					saleNumberValue,	MenuPage_TechSupport_Process},
	{MENU_TYPE_LABEL, 	afterSaleHotlineStr,0,					MenuPage_TechSupport_Process},
	{MENU_TYPE_READ, 	0,		    		customNumberValue,	MenuPage_TechSupport_Process},
	{MENU_TYPE_READ, 	0,					emailValue,			MenuPage_TechSupport_Process},	//emailValue
	{MENU_TYPE_READ, 	0,					webSiteValue,		MenuPage_TechSupport_Process},//webSiteValue
	{MENU_TYPE_LABEL, 	phoneStr,			0,					MenuPage_TechSupport_Process},
};


const MenuPage menuWorkTimePage[] =
{
	{MENU_TYPE_LABEL,	workTimeStr,		0,						defaultMenuPageProcess},
	{MENU_TYPE_LABEL,	workTimeStr,		0,						defaultMenuPageProcess},
	{MENU_TYPE_READ,	0,					workTimeValue,			defaultMenuPageProcess},
};


const MenuPage menuTotalWorkTimePage[] =
{
	{MENU_TYPE_LABEL,	workTimeStr,		0,						defaultMenuPageProcess},
	{MENU_TYPE_LABEL,	totalWorkTimeStr,	0,						defaultMenuPageProcess},
	{MENU_TYPE_READ,	0,					totalWorkTimeValue,		defaultMenuPageProcess},
};

const MenuPage menuChangeDatePage[] =
{
	{MENU_TYPE_LABEL,   changeDateStr,   		0,             			changeDateMenuPageProcess},
	{MENU_TYPE_VALUE,   changeDateStr,   			dateValue,          changeDateMenuPageProcess},
};

const MenuPage menuChangeTimePage[] =
{
	{MENU_TYPE_LABEL,   changeTimeStr,   		0,             			changeTimeMenuPageProcess},
	{MENU_TYPE_VALUE,   changeTimeStr,   			timeValue,          changeTimeMenuPageProcess},
};

const MenuPage menuDateTimePage[] =
{
	{MENU_TYPE_LABEL,   dateStr,   		0,             				dateTimeMenuPageProcess},
    {MENU_TYPE_READ,    dateStr,    	dateValue,   				dateTimeMenuPageProcess},
    {MENU_TYPE_READ,   	timeStr,    	timeValue,   				dateTimeMenuPageProcess},
    {MENU_TYPE_DIR,     changeDateStr,    (int)menuChangeDatePage, 	dateTimeMenuPageProcess},
    {MENU_TYPE_DIR,     changeTimeStr,    (int)menuChangeTimePage, 	dateTimeMenuPageProcess},
#if 1
	{MENU_TYPE_READ,	currentTimeStr, 	workTimeValue,				dateTimeMenuPageProcess},
	{MENU_TYPE_READ,	lastTimeStr,		lastWorkTimeValue,				dateTimeMenuPageProcess},
	{MENU_TYPE_READ,	totalWorkTimeStr,	totalWorkTimeValue, 		dateTimeMenuPageProcess},
	{MENU_TYPE_READ,	bootTimesStr,		bootTimesValue, 			dateTimeMenuPageProcess},//defaultMenuPageProcess
#endif
};

const MenuPage menuSysWorkTimePage[] =
{
	{MENU_TYPE_LABEL,   dateStr,   			0,             				dateTimeMenuPageProcess},
    {MENU_TYPE_READ,    currentTimeStr,    	workTimeValue, 				dateTimeMenuPageProcess},
    {MENU_TYPE_READ,    lastTimeStr,    	lastWorkTimeValue, 				dateTimeMenuPageProcess},
    {MENU_TYPE_READ,    totalWorkTimeStr,	totalWorkTimeValue, 		dateTimeMenuPageProcess},
    {MENU_TYPE_READ,    bootTimesStr,    	bootTimesValue, 			dateTimeMenuPageProcess},//defaultMenuPageProcess
};

const MenuPage menuOLEDLuminancePage[] =
{
	{MENU_TYPE_LABEL,   luminanceStr,   	0,             			oledLuminanceAdjustPageProcess},//defaultMenuPageProcess
	{MENU_TYPE_VALUE,   luminanceStr,   	luminanceValue, 		oledLuminanceAdjustPageProcess},//defaultMenuPageProcess
};	


//#################################################################################################################################
//设置远程IP
const MenuPage menuRemoteIpAddressPage[] =
{
	{MENU_TYPE_LABEL,   setRemoteIpAddress,   	0,             			IpAddressMenuPageProcess},
	{MENU_TYPE_LABEL,   setRemoteIpAddress,   	0, 						IpAddressMenuPageProcess},
	{MENU_TYPE_VALUE,   nullStr,    		RemoteIpAddressValue, 		IpAddressMenuPageProcess},
};



//#################################################################################################################################
//设置本地IP
const MenuPage menuLocalIpAddressPage[] =
{
	{MENU_TYPE_LABEL,   setLocalIpAddress,   	0,             			IpAddressMenuPageProcess},
	{MENU_TYPE_LABEL,   setLocalIpAddress,   	0, 						IpAddressMenuPageProcess},
	{MENU_TYPE_VALUE,   nullStr,    		LocalIpAddressValue, 		IpAddressMenuPageProcess},
};	


//#################################################################################################################################
//设置子网掩码
const MenuPage menuSubnetMaskPage[] =
{
	{MENU_TYPE_LABEL,   setSubnetMaskStr,   0,             			IpAddressMenuPageProcess},
	{MENU_TYPE_LABEL,   setSubnetMaskStr,   0, 						IpAddressMenuPageProcess},
	{MENU_TYPE_VALUE,   nullStr,    		subnetMaskValue, 		IpAddressMenuPageProcess},
};	



//#################################################################################################################################
//设置默认网关
const MenuPage menuGatewayPage[] =
{
	{MENU_TYPE_LABEL,   setGatewayStr,   	0,             			IpAddressMenuPageProcess},
	{MENU_TYPE_LABEL,   setGatewayStr,   	0, 						IpAddressMenuPageProcess},
	{MENU_TYPE_VALUE,   nullStr,    		gatewayAddressValue, 	IpAddressMenuPageProcess},
};	


//#################################################################################################################################
//ART-NET IP地址设置菜单
const MenuPage menuArtNetEthAddrPage[] =
{
	{MENU_TYPE_LABEL,   setRemoteIpAddress,   	0,             			defaultMenuPageProcess},
	//{MENU_TYPE_DIR,   	setRemoteIpAddress,   	(int)menuRemoteIpAddressPage,   	defaultMenuPageProcess},	//modify by zhang
	{MENU_TYPE_DIR,   	setLocalIpAddress,   	(int)menuLocalIpAddressPage,   	defaultMenuPageProcess},
	{MENU_TYPE_DIR,   	setSubnetMaskStr,   (int)menuSubnetMaskPage,   	defaultMenuPageProcess},
	//{MENU_TYPE_DIR,   	setGatewayStr,   	(int)menuGatewayPage,   	defaultMenuPageProcess},		
};


const MenuPage menuEthernetPage[] =
{
	{MENU_TYPE_LABEL,   ethernetStr,   		0,             				ethernetPageProcess},//defaultMenuPageProcess   
	{MENU_TYPE_VALUE,   artNetStartAddrStr,    		artNetStartAddrValue,   		changeNoSetProcess},
	{MENU_TYPE_VALUE,   artNetUniverseStr,    		artNetUniverseValue,   		changeNoSetProcess},
   	{MENU_TYPE_DIR,   	setIpAddressStr,   	(int)menuArtNetEthAddrPage,   	defaultMenuPageProcess},			
};

const MenuPage menuLockPanelPasswordPage[] = 
{
	{MENU_TYPE_LABEL,   lockPanelPasswordStr,   	0,             			menuPanelPasswordPageProcess},
    {MENU_TYPE_LABEL,   inputNewPasswordStr,  	lockPanelPasswordValue,   menuPanelPasswordPageProcess},
};

const MenuPage menuLockPanelEnterPasswordPage[] =
{
    {MENU_TYPE_LABEL,   enterPasswordStr,   	0,             	menuPanelEnterPasswordProcess},
    {MENU_TYPE_LABEL,   enterPasswordStr,   	0,             	menuPanelEnterPasswordProcess},
};


const MenuPage menuLockPanelPage[] =
{
#if (CURRENT_DEVICE == _VIP_SMART)
	{MENU_TYPE_LABEL,	lockPanelStr,		0,						MenuPageLockPanelProcess},
	{MENU_TYPE_VALUE,	lockPanelStr, 		keyLockValue,			MenuPageLockPanelProcess},
	{MENU_TYPE_DIR,   	lockPanelPasswordStr,  (int)menuLockPanelPasswordPage,MenuPageLockPanelProcess},
#else
	{MENU_TYPE_LABEL,   lockPanelStr,   	0,             			noYesPageProcess},
    {MENU_TYPE_LABEL,   lockPanelStr,    	0,   					noYesPageProcess},
    {MENU_TYPE_VALUE,   yesOrNoStr,    		keyLockValue,   		noYesPageProcess},
 #endif
};

const MenuPage menuChangePasswordPage[] = 
{
	{MENU_TYPE_LABEL,   changePasswordStr,   	0,             			changePasswordMenuPageProcess},
    {MENU_TYPE_LABEL,   inputNewPasswordStr,  	licensePasswordValue,   changePasswordMenuPageProcess},
};

const MenuPage menuChangeLicenseTimePage[] =
{
	{MENU_TYPE_LABEL,   changeTimeStr,    	licenseTimeValue,   	changeLicenseTimeMenuPageProcess},
	{MENU_TYPE_LABEL,   changeTimeStr,    	licenseTimeValue,   	changeLicenseTimeMenuPageProcess},
    {MENU_TYPE_VALUE,   spaceStr,    		licenseTimeValue,   	changeLicenseTimeMenuPageProcess},
};

const MenuPage menuLicenseSetupPage[] =
{
	{MENU_TYPE_LABEL,   licenseSetupStr,   	0,             			defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   licenseStateStr,    licenseStateValue,   	defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	changeTimeStr,    	(int)menuChangeLicenseTimePage,defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	changePasswordStr,  (int)menuChangePasswordPage,defaultMenuPageProcess},
};

const MenuPage menuEnterPasswordPage[] =
{
    {MENU_TYPE_LABEL,   enterPasswordStr,   	0,             	enterPasswordMenuPageProcess},
    {MENU_TYPE_LABEL,   enterPasswordStr,   	0,             	enterPasswordMenuPageProcess},
};

const MenuPage menuCanUseTimePage[] =
{
	{MENU_TYPE_LABEL,	canUseTimeStr,		0,						defaultMenuPageProcess},
	{MENU_TYPE_LABEL,	canUseTimeStr,		0,						defaultMenuPageProcess},
	{MENU_TYPE_READ,	0,					canUseTimeValue,		defaultMenuPageProcess},
};	
const MenuPage menuHotBackupPage[] =
{
	{MENU_TYPE_LABEL,	hotBackUpStr,		0,						defaultMenuPageProcess},//hotbackupPageProcess
	{MENU_TYPE_VALUE,	hotBackUpStr,		hotBackupEnableValue,	changeNoSetProcess},//defaultMenuPageProcess
	{MENU_TYPE_VALUE,	hotBackUpClass1Str,	hotBackupClass1Value,	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	hotBackUpClass2Str, hotBackupClass2Value,	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	hotBackUpClass3Str, hotBackupClass3Value,	defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,	hotBackUpClass4Str, hotBackupClass4Value,	defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,	hotBackUpClass5Str, hotBackupClass5Value,	defaultMenuPageProcess},
};	



const MenuPage menuDeviceStatusPage[] = 
{
	{MENU_TYPE_LABEL,	deviceStatusStr,		        0,				defaultMenuPageProcess},
	{MENU_TYPE_READ,	videoProcessStatusStr,	videoStatusValue,	    defaultMenuPageProcess},
	{MENU_TYPE_READ,	mbProcessStatusStr,		mainBoardStatusValue,	defaultMenuPageProcess},
};

const MenuPage menuUserDefinedButtonPage[] = 
{
	{MENU_TYPE_LABEL,	userDefinedButtonStr,		0,					defaultMenuPageProcess},
	{MENU_TYPE_LABEL,	userDefinedButtonStr,		0,					defaultMenuPageProcess},		
	{MENU_TYPE_VALUE,	mapKeyStr, 					mapKeyTypeValue,	changeNoSetProcess},//defaultMenuPageProcess		
};


const MenuPage menuFactoryResetOkPage[] =
{
	{MENU_TYPE_LABEL,   factoryResetStr,   		0,             			noYesPageProcess},
    {MENU_TYPE_LABEL,   factoryResetStr,    	0,   					noYesPageProcess},
    {MENU_TYPE_VALUE,   yesOrNoStr,    		    factoryResetValue,   	noYesPageProcess},
};

const MenuPage menuSaveIpResetPage[] =
{
	{MENU_TYPE_LABEL,   saveIpResetStr,   	0,             			noYesPageProcess},
    {MENU_TYPE_LABEL,   saveIpResetStr,     0,   					noYesPageProcess},
    {MENU_TYPE_VALUE,   yesOrNoStr,  		saveIpResetValue,  	 noYesPageProcess},
};


const MenuPage menuFactoryResetPage[] =
{
	{MENU_TYPE_LABEL,   factoryResetStr,   	0,             					defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	factoryResetStr,    (int)menuFactoryResetOkPage,   	defaultMenuPageProcess},
    //{MENU_TYPE_DIR,   	saveIpResetStr,     (int)menuSaveIpResetPage,  		defaultMenuPageProcess},
};

const MenuPage menuPreviewDblTapSwitchPage[] = 
{
	{MENU_TYPE_LABEL,	PreviewDblTapSwitchStr,		0,									defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	PreviewDblTapSwitchStr,		(int)previewDblTapSwitchValue,			changeNoSetProcess},			
};

const MenuPage menuPreviewSwitchPage[] = 
{
	{MENU_TYPE_LABEL,	PreviewSwitchStr,		0,									defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	PreviewSwitchStr,		(int)previewSwitchValue,			changeNoSetProcess},			
};


const MenuPage menuFanControlPage[] = 
{
	{MENU_TYPE_LABEL,	fanControlStr,		        0,					fanControlPageProcess},//hotbackupPageProcess
	{MENU_TYPE_VALUE,	automaticSpeedControlStr,	autoSpeedValue,	    changeNoSetProcess},
	{MENU_TYPE_VALUE,	fanSpeedStr,		        currentSpeedValue,	changeNoSetProcess},
};

const MenuPage menuWriteSnPage[] =
{
    {MENU_TYPE_LABEL,   systemStr,   		0,             							defaultMenuPageProcess},//
	{MENU_TYPE_VALUE,	snStr,				WriteSnValue,							defaultMenuPageProcess},//systemInfoPageProcess
};

const MenuPage menuSystemPage[] =
{
    {MENU_TYPE_LABEL,   systemStr,   		0,             					systemMenuPageProcess},//
	{MENU_TYPE_READ,	snStr,				snValue,						systemInfoPageProcess},//systemInfoPageProcess
	
	{MENU_TYPE_DIR, 	softVersionStr,		(int)menuSoftVersionPage, 		defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	hardVersionStr,		(int)menuHardVersionPage,		defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,	luminanceStr,		luminanceValue, 				defaultMenuPageProcess},
};





const MenuPage outputPortSelPage[] =
{
    {MENU_TYPE_LABEL,   outputPortSelSrt,   		0,             			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	outputPort0Srt,				outputPort0Value,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	outputPort1Srt,				outputPort1Value, 		defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	outputPort2Srt,				outputPort2Value,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	outputPort3Srt,				outputPort3Value, 		defaultMenuPageProcess},
};


const MenuPage menuAdvancedPage[] = 
{
	{MENU_TYPE_LABEL,   			advancedStr,   			0,             					defaultMenuPageProcess},//
	{MENU_TYPE_ENTER_NUMBER_VALUE,	diaplayAlphaStr,		displayAlphaValue, 				enterNumberPageProcess},//
	{MENU_TYPE_DIR,					hotBackUpStr,			(int)menuHotBackupPage, 		defaultMenuPageProcess},//
	{MENU_TYPE_DIR,					outputPortSelSrt,		(int)outputPortSelPage, 		defaultMenuPageProcess},//
	{MENU_TYPE_DIR,   				imageQualityStr,		(int)menuImageQualityPage,		defaultMenuPageProcess},
	{MENU_TYPE_DIR,   				systemInfoStr,			(int)menuSystemPage,			defaultMenuPageProcess},
	{MENU_TYPE_DIR,     			factoryResetStr,  		(int)menuFactoryResetOkPage, 	defaultMenuPageProcess},
};


const MenuPage menuAdjustPage[] =
{
	{MENU_TYPE_LABEL,   spaceStr,   	0,             				defaultMenuPageProcess},
    {MENU_TYPE_VALUE,  	vga1KeyModeStr,		vga1KeyModeValue,  	   	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,  	vga2KeyModeStr,		vga2KeyModeValue,  	   	defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	vgaAdjustStr,	(int)menuVgaAdjustPage,  	defaultMenuPageProcess},
    {MENU_TYPE_DIR,   	SdiAdjustStr,	(int)menuSdiAdjustPage,  	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	HdmiAdjustStr,	(int)menuHdmiAdjustPage,	defaultMenuPageProcess},  //added by julia for V1.62
    {MENU_TYPE_DIR,   	adcAdjustStr,	(int)menuAdcAdjustPage,  	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   gammaStr,  		gammaValue, 				defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   windowRatioStr, windowRatioValue, 			defaultMenuPageProcess},
    {MENU_TYPE_DIR,     outputAdjustStr,(int)menuOutputAdjustPage, 	defaultMenuPageProcess},
};


const MenuPage menuGammaSetmPage[] =
{   
	{MENU_TYPE_LABEL,   spaceStr,       	   0,      defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   gammaStr,	       gamma_sendcard_Value,	   changeNoSetProcess},	//defaultMenuPageProcess
};


const MenuPage systemInfoPage[] = 	//@cc
{	 
	{MENU_TYPE_LABEL,	 	systemInfoStr,		  0,	                    		defaultMenuPageProcess},	
	{MENU_TYPE_READ, 		sendcardinfostr,		    sendcardVersionValue,		defaultMenuPageProcess},
    {MENU_TYPE_READ, 		receivecardinfostr,	        receivecardVersionValue,	defaultMenuPageProcess},
};


const MenuPage menuColorTemSetmPage[] =
{   
	{MENU_TYPE_LABEL,   spaceStr,       	   0,      enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   colorTempRedStr,	    RedColorTemSet_Value,	   enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   colorTempGreenStr,	    GreenColorTemSet_Value,	   enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   colorTempBlueStr,	    BlueColorTemSet_Value,	   enterNumberPageProcess},
	{MENU_TYPE_VALUE,   saveToSenderCardstr,	    ColorTemSet_Value,	   resetMenuPageProcess},
};


const MenuPage menuCardStandardFormatPage[]=
{
	{MENU_TYPE_LABEL,   outputFormatStr,    0,       			outFormatPageProcess},	//outFormatPageProcess
	{MENU_TYPE_LABEL,   outputFormatStr,	0,   				outFormatPageProcess},
	{MENU_TYPE_VALUE,   spaceStr,		cardoutputFormatValue,  	outFormatPageProcess},
};



const MenuPage menuCardCustomFormatPage[]=
{	
	{MENU_TYPE_LABEL,	customFormatStr,			   0,	   			 defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	widthStr,		   	  widthValue,	   				defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	HeightStr,		   	  heightValue,	   				defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	savetosendercardstr,  sendcardsetvalue,				resetMenuPageProcess},
};



const MenuPage menuchoosecard[] =
{
	{MENU_TYPE_LABEL,     choosecard12str,        0,       			defaultMenuPageProcess},
    {MENU_TYPE_VALUE,     sendcardstr,  	choosecard12value, 	defaultMenuPageProcess},
    
};

const MenuPage menuPosition[] =
{
	{MENU_TYPE_LABEL,   positionstr,        0,       			MenuPageProcesschange},
    {MENU_TYPE_VALUE,   hPosStr1,  			xposvalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   vPosStr1,  		    yposvalue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   hSizeStr1,  		xsizevalue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   vSizeStr1,  		ysizevalue, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   savetosendercardstr,		sendcardsetvalue,   	resetMenuPageProcess},
};


const MenuPage menuGamma[] =
{
	{MENU_TYPE_LABEL,   spaceStr,      0,       			defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   gammaStr,  	 gamma_sendcard_Value, 	changeNoSetProcess},
    
};

const MenuPage menuConnect[] =
{
	{MENU_TYPE_LABEL,   connectstr,      0,       			MenuPageProcesschange},
    {MENU_TYPE_VALUE,   screenconnectstr,  	 connectvalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   sendtoreceivestr,		setconnectvalue,   	resetMenuPageProcess},
    
};

const MenuPage parameterstr_lxy[] =
{
	{MENU_TYPE_LABEL,   connectstr,      0,       			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,     sendcardstr,  	choosecard12value, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   networkchoosestr,  	networkchoosevalue, 	defaultMenuPageProcess},
	
    {MENU_TYPE_VALUE,   xoffsetstr,  	xoffsetvalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   yoffsetstr,  	yoffsetvalue, 	defaultMenuPageProcess}, 
};

const MenuPage menuConnect_lxy_son[] =
{
	{MENU_TYPE_LABEL,   connectstr,      0,       			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   vlinestr,  	 vlinevalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   hlinestr,  	 hlinevalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   vsize_str,  	 vsizevalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   hsize_str,  	 hsizevalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   screenconnectstr,  	 connectvalue, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   sendtoreceivestr,		setconnectvalue,   	resetMenuPageProcess},
    
};

const MenuPage menuConnect_lxy[] =
{
	{MENU_TYPE_LABEL,   connectstr,      0,       			MenuPageProcesschange},
	{MENU_TYPE_DIR,     choosecard12str,  	(int)parameterstr_lxy, 	defaultMenuPageProcess},
	{MENU_TYPE_DIR,     connectstr,   	(int)menuConnect_lxy_son,          defaultMenuPageProcess},
    
};


const MenuPage menureceivePosition_lxy[] =
{
	{MENU_TYPE_LABEL,   receivecardstr,        0,       			MenuPageProcesschange},
	{MENU_TYPE_VALUE,     sendcardstr,  	choosecard12value, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   networkchoosestr,  	networkchoosevalue, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   receivecard_number_str,  			receivecard_number_value, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   hPosStr1,  			xpos_receive_value, 	defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   vPosStr1,  		    ypos_receive_value, 		defaultMenuPageProcess},
    {MENU_TYPE_VALUE,   sendtoreceivestr,		sendreceive_cardsetvalue,   	resetMenuPageProcess},
    
};


#if ENABLE_TEST_GRID_CONTROL
const MenuPage menuGridDotColorPage[] =
{
	{MENU_TYPE_LABEL,	gridColorStr,			0,							MenuPage_testGrid_Process},

    {MENU_TYPE_VALUE,   redStr,  			(int)gridDotColorRValue, 	MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   greenStr,  			(int)gridDotColorGValue, 	MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   blueStr,  			(int)gridDotColorBValue, 	MenuPage_testGrid_Process}, 
};


const MenuPage menuGridDotManualPage[] =
{
	{MENU_TYPE_LABEL,	gridDotManualStr,			0,							MenuPage_testGrid_Process},

	{MENU_TYPE_VALUE,   hPosStr,			(int)gridDotPosXValue,   	MenuPage_testGrid_Process},
	{MENU_TYPE_VALUE,   vPosStr,			(int)gridDotPosYValue,   	MenuPage_testGrid_Process}, 
};


const MenuPage menuGridDotPage[] =
{
	{MENU_TYPE_LABEL,	gridDotStr,			0,							MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotStr,  		(int)gridDotValue, 			MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotSpeedStr,  (int)gridDotSpeedValue, 	    MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotColorStr,  (int)gridDotColorValue, 	    MenuPage_testGrid_Process}, 
    {MENU_TYPE_VALUE,   gridDotColorStr,  (int)menuGridDotColorPage, 	    MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotManualStr,  (int)menuGridDotManualPage,      MenuPage_testGrid_Process},
};


const MenuPage menuGridTestPage[] =
{
	{MENU_TYPE_LABEL,	GridTestStr,		0,							MenuPage_testGrid_Process},
	{MENU_TYPE_VALUE,	sendCardPortSelStr,		(int)GridPortValue,			MenuPage_testGrid_Process},        
	{MENU_TYPE_VALUE,	GridTestStr,		(int)GridTestValue,			MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridColorStr,  		(int)gridColorVaule, 			defaultMenuPageProcess},	
    {MENU_TYPE_VALUE,   gridModeStr,  		(int)gridModeValue, 			MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotStr,  		(int)gridDotValue, 			MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotSpeedStr,  (int)gridDotSpeedValue, 	    MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotColorStr,  (int)gridDotColorValue, 	    MenuPage_testGrid_Process}, 
    {MENU_TYPE_VALUE,   gridDotColorStr,  (int)menuGridDotColorPage, 	    MenuPage_testGrid_Process},
    {MENU_TYPE_VALUE,   gridDotManualStr,  (int)menuGridDotManualPage,      MenuPage_testGrid_Process},    
};

#endif


const MenuPage menuUsbUpdateConfirmPage[] =
{
	{MENU_TYPE_LABEL,   usbUpdateFirmwareStr,   	0,             			menuPage_UsbUpdateConfirm_Process},
	{MENU_TYPE_LABEL,   usbUpdateFirmwareStr,   	0,             			menuPage_UsbUpdateConfirm_Process},		
    {MENU_TYPE_LABEL,   updateFile1Str,    	0,   							menuPage_UsbUpdateConfirm_Process},
    {MENU_TYPE_VALUE,   yesOrNoStr,    		usbUpdateConfirmValue,   		menuPage_UsbUpdateConfirm_Process},	
};

const MenuPage menuVideoPanelFwSendPage[] =
{
	{MENU_TYPE_LABEL,        sendStr,       	        0, 					 defaultMenuPageProcess},
	//{MENU_TYPE_DIR,        fromUSBStr,       	        (int)menuUsbUpdateConfirmPage, 					 defaultMenuPageProcess},
	{MENU_TYPE_DIR,        fromUSBStr,       	        (int)menuNaPage, 					 defaultMenuPageProcess},	
	{MENU_TYPE_VALUE,        fromInternalMemStr,       	 (int)naValue, 					 defaultMenuPageProcess},
};

const MenuPage menuVideoPanelFirmwarePage[] =
{
	{MENU_TYPE_LABEL,        videoPanelFirmwareStr,       	        0, 					 defaultMenuPageProcess},	//defaultMenuPageProcess
	//{MENU_TYPE_DIR, 		sendStr,		(int)menuVideoPanelFwSendPage,		defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 		sendStr,		(int)videoPanelFWSaveValue,	 defaultMenuPageProcess},	
	{MENU_TYPE_VALUE, 		saveStr,		(int)videoPanelFWSaveValue,	 defaultMenuPageProcess},
	{MENU_TYPE_READ, 		mcuVersionStr,			mcuVersionValue,			defaultMenuPageProcess},
};

const MenuPage menuHorizontalOneSubTwoModePage[] =	//水平一分二
{
	{MENU_TYPE_LABEL,					splitStr,					0,									defaultMenuPageProcess},	
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					prvInputSourceStr,			(int)prvSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)h12TotalWidthValue,					enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)h12TotalHeighValue,					enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisWidth,				(int)h12FirstDisWidthValue,					enterNumberPageProcess},//第一块屏宽
	//{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisHeigh,				(int)h12FirstDisHeighValue,					enterNumberPageProcess},//第二块屏宽
	//{MENU_TYPE_ENTER_NUMBER_VALUE,   	thirdDisWidth,				thirdDisWidthValue,			enterNumberPageProcess},//第三块屏宽
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)Horizontal2SplitParamSendValue,		defaultMenuPageProcess},
};


const MenuPage menuVerticalOneSubTwoModePage[] =	//垂直一分二
{
	{MENU_TYPE_LABEL,					splitStr,					0,									defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					prvInputSourceStr,			(int)prvSourceValue,			defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)v12TotalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)v12TotalHeighValue,				enterNumberPageProcess},//总高
	//{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisWidth,				(int)v12FirstDisWidthValue,			enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisHeigh,				(int)v12FirstDisHeighValue,			enterNumberPageProcess},//第二块屏宽
	//{MENU_TYPE_ENTER_NUMBER_VALUE,   	thirdDisWidth,				thirdDisWidthValue,				enterNumberPageProcess},//第三块屏宽
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)Vertical2SplitParamSendValue,	defaultMenuPageProcess},
};



const MenuPage menuTwo4kTakePage[] =	//垂直一分二
{
	{MENU_TYPE_LABEL,					splitStr,					0,									defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					prvInputSourceStr,			(int)prvSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)v12TotalWidthValue,			enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)v12TotalHeighValue,			enterNumberPageProcess},//总高
	//{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisWidth,				(int)v12FirstDisWidthValue,			enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisHeigh,				(int)v12FirstDisHeighValue,			enterNumberPageProcess},//第二块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	thirdDisWidth,				thirdDisWidthValue,				enterNumberPageProcess},//第三块屏宽
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)two4kTakeModeSendValue,		defaultMenuPageProcess},
};

const MenuPage menuTwoPortSplitModePage[] =
{
	{MENU_TYPE_LABEL,	splitStr,			0,											defaultMenuPageProcess},
	{MENU_TYPE_VALUE,	fadeTimeStr,		fadeTimeValue, 								defaultMenuPageProcess},//
	{MENU_TYPE_DIR, 	Horizontal2Str,		(int)menuHorizontalOneSubTwoModePage,	  	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	vertical2Str,		(int)menuVerticalOneSubTwoModePage,			defaultMenuPageProcess},
	//{MENU_TYPE_DIR, 	vertical2Str,		(int)menuTwo4kTakePage,						defaultMenuPageProcess},
};

#if 1

const MenuPage menuFourPortHorizontal2SplitModePage[] =
{	
	{MENU_TYPE_LABEL,					splitStr,					0,										defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,					defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)hor12TotalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)hor12TotalHeighValue,				enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisWidth,				(int)hor12FirstDisWidthValue,			enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)Horl2SplitParamSendValue,			defaultMenuPageProcess},

};

const MenuPage menuFourPortVertical2SplitModePage[] =
{	
	{MENU_TYPE_LABEL,					splitStr,					0,										defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,					defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)ver12TotalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)ver12TotalHeighValue,				enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisHeigh,				(int)ver12FirstDisHeighValue,			enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)Verl2SplitParamSendValue,			defaultMenuPageProcess},

};

#endif

const MenuPage menuFourPortHorizontal3SplitModePage[] =
{
	{MENU_TYPE_LABEL,					splitStr,					0,										defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,					defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)h13TotalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)h13TotalHeighValue,				enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisWidth,				(int)h13FirstDisWidthValue,				enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	secondDisWidth,				(int)h13SecondDisWidthValue,			enterNumberPageProcess},//第二块屏宽
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)Horizontal3SplitParamSendValue,	defaultMenuPageProcess},

};


#if 1
const MenuPage menuFourPortHorizontal4SplitModePage[] =
{	
	{MENU_TYPE_LABEL,					splitStr,					0,										defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,					defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)h14TotalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)h14TotalHeighValue,				enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisWidth,				(int)h14FirstDisWidthValue,				enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	secondDisWidth,				(int)h14SecondDisWidthValue,			enterNumberPageProcess},//第二块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	thirdDisWidth,				(int)h14ThirdDisWidthValue,				enterNumberPageProcess},//第三块屏宽
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)Horizontal4SplitParamSendValue,	defaultMenuPageProcess},

};
#endif



const MenuPage menuFourPortvertical4SplitModePage[] =
{
	{MENU_TYPE_LABEL,					splitStr,					0,										defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,					defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)v14TotalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)v14TotalHeighValue,				enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisHeigh,				(int)v14FirstDisHeighValue,				enterNumberPageProcess},//第一块屏高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	secondDisHeigh,				(int)v14SecondDisHeighValue,			enterNumberPageProcess},//第二块屏高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	thirdDisHeigh,				(int)v14ThirdDisHeighValue,				enterNumberPageProcess},//第三块屏高
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)Vertical4SplitParamSendValue,		defaultMenuPageProcess},

};


const MenuPage menuFourPortFieldSplitModePage[] =
{
	{MENU_TYPE_LABEL,					splitStr,					0,									defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalWidth,					(int)totalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	totalHeigh,					(int)totalHeighValue,				enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisWidth,				(int)firstDisWidthValue,			enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	firstDisHeigh,				(int)firstDisHeighValue,			enterNumberPageProcess},//第一块屏高
	
	//{MENU_TYPE_ENTER_NUMBER_VALUE,   	alphaStr,					(int)userAlphaValue,				enterNumberPageProcess},//第一块屏高
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)FieldSplitParamSendValue,		defaultMenuPageProcess},//第一块屏高
};


#if 0	
const MenuPage pipModePage[] =
{
	{MENU_TYPE_LABEL,					splitStr,					0,									defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)pgmSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					prvInputSourceStr,			(int)prvSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	mainWinWidth,				(int)totalWidthValue,				enterNumberPageProcess},//总宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	mainWinHeigh,				(int)totalHeighValue,				enterNumberPageProcess},//总高
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	subWinWidth,				(int)firstDisWidthValue,			enterNumberPageProcess},//第一块屏宽
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	subWinHeigh,				(int)firstDisHeighValue,			enterNumberPageProcess},//第一块屏高
	{MENU_TYPE_CUSTOMNUM,   			sendCardSendToPortStr,		(int)PipModeParamSendValue,		defaultMenuPageProcess},//第一块屏高
};

#else

const MenuPage pipModePage[] =
{
	{MENU_TYPE_LABEL,					pipStr,						0,									enterNumberPageProcess},
	//{MENU_TYPE_VALUE, 					pipMainInputSrt,			(int)pgmSourceValue,				defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 					pipSubInputSrt,				(int)prvSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pipStr,						(int)pipModeValue,					defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		pipSubPosXSrt,				(int)pipSubWinPosXValue,	    	enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		pipSubPosYSrt,				(int)pipSubWinPosYValue,			enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		pipSubSizeXSrt,				(int)pipSubWinSizeXValue,			enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		pipSubSizeYSrt,				(int)pipSubWinSizeYValue,			enterNumberPageProcess},
};


const MenuPage menuPipPage[] = 
{
	{MENU_TYPE_LABEL,   pipStr,      	0,       				defaultMenuPageProcess},//defaultMenuPageProcess
    {MENU_TYPE_VALUE,   pipStr,  		pipModeValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layoutStr,  	layoutValue, 			changeNoSetProcess},
	{MENU_TYPE_VALUE,   selectStr,  	currentWindowValue, 	changeNoSetProcess},
};

const MenuPage menuPipImageBL1Page[] =
{
	{MENU_TYPE_LABEL,   				imageBStr,    					0,       				defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   				inputSourceSelectStr,    		inputSourceValue,       	changeNoSetProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	hPosStr,						scalePosxValue,   		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	vPosStr,						scalePosyValue,   		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	hSizeStr,						scaleWidthValue,   		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	vSizeStr,						scaleHeightValue,   	enterNumberPageProcess},
	//{MENU_TYPE_VALUE,   				resetStr,						resetScaleValue,   		resetMenuPageProcess},
};

const MenuPage menuPipImageAL1Page[] =
{
	{MENU_TYPE_LABEL,   				imageAStr,    		0,       							defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   				inputSourceSelectStr,    		inputSourceValue,       	changeNoSetProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	hPosStr,						scalePosxValue,   		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	vPosStr,						scalePosyValue,   		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	hSizeStr,						scaleWidthValue,   		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	vSizeStr,						scaleHeightValue,   	enterNumberPageProcess},
	//{MENU_TYPE_VALUE,   				resetStr,						resetScaleValue,   		resetMenuPageProcess},
};



#endif


const MenuPage menuPipSubWinAdj[] =
{
	{MENU_TYPE_LABEL,	splitStr,						0,										defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 	pgmInputSourceStr,	(int)pgmSourceValue,				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	pipSubInputSrt,					(int)pipSourceValue,					defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 	pipStr,				(int)PipModeParamSendValue,			defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 	pipSubPosXSrt,		(int)pipSubWinPosXValue,	    		enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 	pipSubPosYSrt,		(int)pipSubWinPosYValue,				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 	pipSubSizeXSrt,		(int)pipSubWinSizeXValue,				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 	pipSubSizeYSrt,		(int)pipSubWinSizeYValue,				enterNumberPageProcess},
};


const MenuPage menuFourPortSplitModePage[] =
{
	{MENU_TYPE_LABEL,	splitStr,			0,											defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	fieldSplitStr,		(int)menuFourPortFieldSplitModePage,		defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	Horizontal2Str,		(int)menuFourPortHorizontal2SplitModePage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	Horizontal3Str,		(int)menuFourPortHorizontal3SplitModePage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	Horizontal4Str,		(int)menuFourPortHorizontal4SplitModePage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	vertical2Str,		(int)menuFourPortVertical2SplitModePage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	pipStr,				(int)menuPipPage,							defaultMenuPageProcess},
	//{MENU_TYPE_DIR, 	pictureModeStr,		(int)menuPipSubWinAdj,						defaultMenuPageProcess},
};


const MenuPage menuMIMOPage[] =
{
	{MENU_TYPE_LABEL,	splitStr,			0,				defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	twoInThreeOutSrt,	0,	    		defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	twoInFourOutSrt,	0,				defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	threeInThreeOutSrt,	0,				defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	threeInFourOutSrt,	0,				defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	fourInFourOutSrt,	0,				defaultMenuPageProcess},
};

const MenuPage menuIndependentPage[] =
{
	{MENU_TYPE_LABEL,					independentModeStr,			0,										defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   				independentModeStr,			(int)mode4PSplitParamSendValue,			defaultMenuPageProcess},//第一块屏高
	{MENU_TYPE_READ,   					outPutDvi1srt,				(int)outPutDvi1Value,					defaultMenuPageProcess},//第一块屏宽
	{MENU_TYPE_READ,   					outPutDvi2srt,				(int)outPutDvi2Value,					defaultMenuPageProcess},//第一块屏高
	{MENU_TYPE_READ,   					outPutDvi3srt,				(int)outPutDvi3Value,					defaultMenuPageProcess},//第二块屏宽
	{MENU_TYPE_READ,   					outPutDvi4srt,				(int)outPutDvi4Value,					defaultMenuPageProcess},//第二块屏高
};



const MenuPage menuOutput0Page[] =
{
	{MENU_TYPE_LABEL,   outputPortSelSrt,   		0,             				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	outputPort0Srt,			(int)outputPort0Value, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   outputTpyeStr,      	(int)outputPort0TpyeValue,		defaultMenuPageProcess},	
	{MENU_TYPE_VALUE,   bitWidthSrt,  			(int)outputPort0BitWidthValue, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   rangeStr,  				(int)outputPort0RangeValue, 	defaultMenuPageProcess},
};

const MenuPage menuOutput1Page[] =
{
	{MENU_TYPE_LABEL,   outputPortSelSrt,   		0,             				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	outputPort1Srt,			(int)outputPort1Value, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   outputTpyeStr,      	(int)outputPort1TpyeValue,		defaultMenuPageProcess},	
	{MENU_TYPE_VALUE,   bitWidthSrt,  			(int)outputPort1BitWidthValue, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   rangeStr,  				(int)outputPort1RangeValue, 	defaultMenuPageProcess},
};

const MenuPage menuOutput2Page[] =
{
	{MENU_TYPE_LABEL,   outputPortSelSrt,   		0,             				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	outputPort2Srt,			(int)outputPort2Value, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   outputTpyeStr,      	(int)outputPort2TpyeValue,		defaultMenuPageProcess},	
	{MENU_TYPE_VALUE,   bitWidthSrt,  			(int)outputPort2BitWidthValue, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   rangeStr,  				(int)outputPort2RangeValue, 	defaultMenuPageProcess},
};

const MenuPage menuOutput3Page[] =
{
	{MENU_TYPE_LABEL,   outputPortSelSrt,   		0,             				defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	outputPort3Srt,			(int)outputPort3Value, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   outputTpyeStr,      	(int)outputPort3TpyeValue,		defaultMenuPageProcess},	
	{MENU_TYPE_VALUE,   bitWidthSrt,  			(int)outputPort3BitWidthValue, 	defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   rangeStr,  				(int)outputPort3RangeValue, 	defaultMenuPageProcess},
};



const MenuPage menu4pLTPage[] =
{
	{MENU_TYPE_LABEL,   				layerParamSrt,   			0,             					defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)_4pLTSourceValue,			defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 				layerAHPosSrt,				(int)_4pLTXValue, 				defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   				layerAVPosSrt,  			(int)_4pLTYValue, 				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		layerAHSizeSrt,				(int)_4pLTWValue, 				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	layerAVSizeSrt,  			(int)_4pLTHValue, 				enterNumberPageProcess},
	{MENU_TYPE_CUSTOMNUM, 				sendCardSendToPortStr,		(int)_4pLTSendValue, 			defaultMenuPageProcess},
};

const MenuPage menu4pLBPage[] =
{
	{MENU_TYPE_LABEL,   				layerParamSrt,   			0,             					defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)_4pLBSourceValue,			defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 				layerAHPosSrt,				(int)_4pLBXValue, 				defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   				layerAVPosSrt,  			(int)_4pLBYValue, 				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		layerCHSizeSrt,				(int)_4pLBWValue, 				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	layerCVSizeSrt,  			(int)_4pLBHValue, 				enterNumberPageProcess},
	{MENU_TYPE_CUSTOMNUM, 				sendCardSendToPortStr,		(int)_4pLBSendValue, 			defaultMenuPageProcess},
};

const MenuPage menu4pRTPage[] =
{
	{MENU_TYPE_LABEL,   				layerParamSrt,   			0,             					defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)_4pRTSourceValue,			defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 				layerAHPosSrt,				(int)_4pRTXValue, 				defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   				layerAVPosSrt,  			(int)_4pRTYValue, 				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		layerBHSizeSrt,				(int)_4pRTWValue, 				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	layerBVSizeSrt,  			(int)_4pRTHValue, 				enterNumberPageProcess},
	{MENU_TYPE_CUSTOMNUM, 				sendCardSendToPortStr,		(int)_4pRTSendValue, 			defaultMenuPageProcess},
};

const MenuPage menu4pRBPage[] =
{
	{MENU_TYPE_LABEL,   				layerParamSrt,   			0,             					defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 					pgmInputSourceStr,			(int)_4pRBSourceValue,			defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 				layerAHPosSrt,				(int)_4pRBXValue, 				defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   				layerAVPosSrt,  			(int)_4pRBYValue, 				defaultMenuPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE, 		layerDHSizeSrt,				(int)_4pRBWValue, 				enterNumberPageProcess},
	{MENU_TYPE_ENTER_NUMBER_VALUE,   	layerDVSizeSrt,  			(int)_4pRBHValue, 				enterNumberPageProcess},
	{MENU_TYPE_CUSTOMNUM, 				sendCardSendToPortStr,		(int)_4pRBSendValue, 			defaultMenuPageProcess},
};


const MenuPage menuOutput0LayerPage[] =
{
	
	{MENU_TYPE_LABEL,   layerParamSrt,   		0,             					defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 	pgmInputSourceStr,		(int)layerASourceValue,			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerAHPosSrt,			(int)layerAHPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerAVPosSrt,  		(int)layerAVPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerAHSizeSrt,			(int)layerAHSizeValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerAVSizeSrt,  		(int)layerAVSizeValue, 			defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM, 	resetStr,			(int)layerAResetValue, 			defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   layerACropStartSrt,  	(int)layerACropStartValue, 			defaultMenuPageProcess},
};


const MenuPage menuOutput1LayerPage[] =
{
	
	{MENU_TYPE_LABEL,   layerParamSrt,   		0,             					defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 	pgmInputSourceStr,		(int)layerBSourceValue,			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerBHPosSrt,			(int)layerBHPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerBVPosSrt,  		(int)layerBVPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerBHSizeSrt,			(int)layerBHSizeValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerBVSizeSrt,  		(int)layerBVSizeValue, 			defaultMenuPageProcess},
		
	{MENU_TYPE_VALUE, 	FineSrt,				(int)layerBFineValue, 			defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,   resetStr,  			(int)layerBResetValue, 			defaultMenuPageProcess},
};

const MenuPage menuOutput2LayerPage[] =
{
	
	{MENU_TYPE_LABEL,   layerParamSrt,   		0,             					defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 	pgmInputSourceStr,		(int)layerCSourceValue,			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerCHPosSrt,			(int)layerCHPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerCVPosSrt,  		(int)layerCVPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerCHSizeSrt,			(int)layerCHSizeValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerCVSizeSrt,  		(int)layerCVSizeValue, 			defaultMenuPageProcess},
		
	{MENU_TYPE_CUSTOMNUM, 	resetStr,			(int)layerCResetValue, 			defaultMenuPageProcess},
	//{MENU_TYPE_VALUE,   layerCCropStartSrt,  	(int)layerCCropStartValue, 			defaultMenuPageProcess},
};

const MenuPage menuOutput3LayerPage[] =
{
	
	{MENU_TYPE_LABEL,   layerParamSrt,   		0,             					defaultMenuPageProcess},
	//{MENU_TYPE_VALUE, 	pgmInputSourceStr,		(int)layerDSourceValue,			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerDHPosSrt,			(int)layerDHPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerDVPosSrt,  		(int)layerDVPosValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE, 	layerDHSizeSrt,			(int)layerDHSizeValue, 			defaultMenuPageProcess},
	{MENU_TYPE_VALUE,   layerDVSizeSrt,  		(int)layerDVSizeValue, 			defaultMenuPageProcess},
		
	{MENU_TYPE_VALUE, 	FineSrt,				(int)layerDFineValue, 			defaultMenuPageProcess},
	{MENU_TYPE_CUSTOMNUM,   resetStr,  			(int)layerDResetValue, 			defaultMenuPageProcess},
};



const MenuPage menuMainPage[] =
{
	{MENU_TYPE_LABEL,   spaceStr,       	0,                  			mainMenuPageProcess},
	{MENU_TYPE_DIR, 	outputStr,			(int)menuOutputPage, 			defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	FourPortSplitStr, 	(int)menuFourPortSplitModePage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR,     splitStr,          	(int)menuTwoPortSplitModePage,	defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	independentModeStr, (int)menuIndependentPage,		defaultMenuPageProcess},
	{MENU_TYPE_DIR, 	edidManageStr,		(int)menuEDIDPage,				defaultMenuPageProcess},
	{MENU_TYPE_DIR,     systemStr,      	(int)menuAdvancedPage,			defaultMenuPageProcess},	
	{MENU_TYPE_VALUE,   languageStr,  		(int)languageValue, 			defaultMenuPageProcess},
};




const MenuPage menuNothingPage[] =
{
	{MENU_TYPE_LABEL,	spaceStr,		0,		nothingMenuPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,	   	0,	   	nothingMenuPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,		0,		nothingMenuPageProcess},
};

const MenuPage menuStatusPage[] =
{
	{MENU_TYPE_LABEL,	spaceStr,		0,		statusMenuPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,	   	0,	   	statusMenuPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,		0,		statusMenuPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,		0,		statusMenuPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,		0,		statusMenuPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,		0,		statusMenuPageProcess},
};

const MenuPage firmwareUpdatePage[] =
{
	{MENU_TYPE_LABEL,	spaceStr,				0,		firmwareUpdatePageProcess},
	{MENU_TYPE_LABEL,	updateFirmwareStr,		0,		firmwareUpdatePageProcess},
	{MENU_TYPE_LABEL,	spaceStr,				0,		firmwareUpdatePageProcess},
};

const MenuPage menuSelectInputPage[] =
{
	{MENU_TYPE_LABEL,	inputSourceSelectStr,	0,		selectInputPageProcess},
	{MENU_TYPE_LABEL,	inputSourceSelectStr,	0,		selectInputPageProcess},
	{MENU_TYPE_LABEL,	spaceStr,				0,		selectInputPageProcess},
};

const MenuPage artnet_testPage[] =
{
	{MENU_TYPE_LABEL,	spaceStr,		0,		firmwareUpdatePageProcess},
	{MENU_TYPE_LABEL,	artnetTestStr,		0,		firmwareUpdatePageProcess},
	{MENU_TYPE_LABEL,	spaceStr,		0,		firmwareUpdatePageProcess},
};


#define _MENU_LIST_ADD(x) {x, 1, sizeof(x)/sizeof(MenuPage)},

MenuItem menuList[] =
{
    _MENU_LIST_ADD(menuMainPage)						// MENU_INDEX_MAIN
	_MENU_LIST_ADD(menuStatusPage)						// MENU_INDEX_STATUS
	_MENU_LIST_ADD(menuInputPage)						// MENU_INDEX_INPUT
	_MENU_LIST_ADD(menuInputInfoPage)					// MENU_INDEX_INPUT_INFO
	_MENU_LIST_ADD(menuInputHdmiPage)					// MENU_INDEX_INPUT_HDMI
	_MENU_LIST_ADD(menuInputDviPage)					// MENU_INDEX_INPUT_DVI
	_MENU_LIST_ADD(menuInputSdiPage)					// MENU_INDEX_INPUT_SDI
	_MENU_LIST_ADD(menuInputDetailPage)
	_MENU_LIST_ADD(menuInputCropPage)
	_MENU_LIST_ADD(menuZoomPage)
	_MENU_LIST_ADD(menuVgaAdjustPage)
	_MENU_LIST_ADD(menuAdcAdjustPage)
	_MENU_LIST_ADD(menuAdcAutoAdjustPage)
	_MENU_LIST_ADD(menuAdcResetAllPage)
	_MENU_LIST_ADD(menuSdiAdjustPage)
	_MENU_LIST_ADD(menuHdmiAdjustPage)
	_MENU_LIST_ADD(menuEDIDPage)						// MENU_INDEX_EDID
	_MENU_LIST_ADD(customEDIDPage)						// MENU_INDEX_CUSTOM_EDID
    _MENU_LIST_ADD(menuOutputPage)						//MENU_INDEX_OUTPUT
    _MENU_LIST_ADD(menuOutputDetailPage) 
    _MENU_LIST_ADD(menuDviOutDetailPage)
	_MENU_LIST_ADD(customFormatPage)
	_MENU_LIST_ADD(standardFormatPage)
	_MENU_LIST_ADD(otherFormatPage)
    _MENU_LIST_ADD(outputFormatPage)
    _MENU_LIST_ADD(menuOutputAdjustPage)
    _MENU_LIST_ADD(menuDviOutConfigPage)
    _MENU_LIST_ADD(menuDeAdjustPage)
    _MENU_LIST_ADD(menuDviOutLoopConfigPage)
	_MENU_LIST_ADD(menuScalePage)						// MENU_INDEX_SCALE
	_MENU_LIST_ADD(menuOutputFitPage)					// MENU_INDEX_OUTPUT_FIT
	_MENU_LIST_ADD(menuFitAdvancedPage)					// MENU_INDEX_FIT_ADVANCED
	_MENU_LIST_ADD(menuFitRatioPage)					// MENU_INDEX_FIT_RATIO
	_MENU_LIST_ADD(menuScreenPage)
	_MENU_LIST_ADD(menuScreenHotKeyPage)
	_MENU_LIST_ADD(menuPicturePage)
	_MENU_LIST_ADD(menuImageQualityPage)				// MENU_INDEX_IMAGE_QUALITY
	_MENU_LIST_ADD(menuImageColorTempPage)              // MENU_INDEX_IMAGE_COLORTEMP
	_MENU_LIST_ADD(menuPipPage)							// MENU_INDEX_PIP
	_MENU_LIST_ADD(menuTextOverlayPage)
	_MENU_LIST_ADD(menuAlphaPage)
	_MENU_LIST_ADD(menuDisplayModePage)
	_MENU_LIST_ADD(menuFlatColorPage)
	_MENU_LIST_ADD(menuTestPatternPage)
	_MENU_LIST_ADD(menuTpColorPage)
	_MENU_LIST_ADD(menuTpParamPage)
	_MENU_LIST_ADD(menuTransitionPage)	            	// MENU_INDEX_TRANSITION
	_MENU_LIST_ADD(menuImageEnhanceVerityPage)      	// MENU_INDEX_IMAGE_ENHANCE_VERIFY
	_MENU_LIST_ADD(menuAudioPage)
	_MENU_LIST_ADD(menuAudioInputConfigPage)
	_MENU_LIST_ADD(menuSplitPage)
	_MENU_LIST_ADD(menuSplitFixedPage)
	_MENU_LIST_ADD(menuSplitModePage)
	_MENU_LIST_ADD(menuSavePage)						// MENU_INDEX_SAVE_SETUP
	_MENU_LIST_ADD(menuSaveToPage)						// MENU_INDEX_SAVE_TO
	_MENU_LIST_ADD(menuSaveVerifyPage)					// MENU_INDEX_SAVE_VERIFY
	_MENU_LIST_ADD(menuLoadFromPage)					// MENU_INDEX_LOAD_FROM
	_MENU_LIST_ADD(menuUsbSingleLoadPage)				// MENU_INDEX_USB_SINGLE_LOAD
	_MENU_LIST_ADD(menuUsbAllLoadPage)					// MENU_INDEX_USB_ALL_LOAD
	_MENU_LIST_ADD(menuUsbAllSavePage)					// MENU_INDEX_USB_ALL_SAVE
	_MENU_LIST_ADD(menuSystemPage)						// MENU_INDEX_SYSTEM
	_MENU_LIST_ADD(menuSystemInfoPage)					// MENU_INDEX_SYSTEM_INFO
	_MENU_LIST_ADD(menuSoftVersionPage)					// MENU_INDEX_SYSTEM_VERSION_SOFT
	_MENU_LIST_ADD(menuHardVersionPage)					// MENU_INDEX_SYSTEM_VERSION_HARD
	_MENU_LIST_ADD(menuVersionPage)
	_MENU_LIST_ADD(menuTechSupportPage)					// MENU_INDEX_TECH_SUPPORT
	_MENU_LIST_ADD(menuDateTimePage)
	_MENU_LIST_ADD(menuWorkTimePage)
	_MENU_LIST_ADD(menuTotalWorkTimePage)
	_MENU_LIST_ADD(menuChangeDatePage)
	_MENU_LIST_ADD(menuChangeTimePage)
	_MENU_LIST_ADD(menuPreviewDblTapSwitchPage)				// MENU_INDEX_PRV_DBLTAP_SWITCH
	_MENU_LIST_ADD(menuPreviewSwitchPage)					// MENU_INDEX_PREVIEW_SWITCH
	_MENU_LIST_ADD(menuAdvancedPage)						// MENU_INDEX_ADVANCED	
	_MENU_LIST_ADD(menuEthernetPage)						// MENU_INDEX_ETHERNET
	_MENU_LIST_ADD(menuArtNetEthAddrPage)					// MENU_INDEX_ARTNET_ETHADDR
	_MENU_LIST_ADD(menuRemoteIpAddressPage)					// MENU_INDEX_REMOTE_IP_ADDRESS
	_MENU_LIST_ADD(menuLocalIpAddressPage)					// MENU_INDEX_LOCAL_IP_ADDRESS
	_MENU_LIST_ADD(menuSubnetMaskPage)						// MENU_INDEX_SUBNET_MASK			
	_MENU_LIST_ADD(menuGatewayPage)							// MENU_INDEX_GATEWAY
	_MENU_LIST_ADD(menuLockPanelPage)						// MENU_INDEX_LOCK_PANEL
	_MENU_LIST_ADD(menuLockPanelPasswordPage)				// MENU_INDEX_LOCKPANEL_PASSWORD	
	_MENU_LIST_ADD(menuLockPanelEnterPasswordPage)			// MENU_INDEX_LOCKPANEL_ENTER_PASSWORD
	_MENU_LIST_ADD(menuEnterPasswordPage)					// MENU_INDEX_ENTER_PASSWORD
	_MENU_LIST_ADD(menuCanUseTimePage)
	_MENU_LIST_ADD(menuHotBackupPage)
	_MENU_LIST_ADD(menuLicenseSetupPage)
	_MENU_LIST_ADD(menuChangeLicenseTimePage)
	_MENU_LIST_ADD(menuChangePasswordPage)
	_MENU_LIST_ADD(menuFanControlPage)
	_MENU_LIST_ADD(menuDeviceStatusPage)
	_MENU_LIST_ADD(menuUserDefinedButtonPage)				// MENU_INDEX_USER_DEFINED_BUTTON
	_MENU_LIST_ADD(menuFactoryResetPage)					// MENU_INDEX_FACTORY_RESET
	_MENU_LIST_ADD(menuFactoryResetOkPage)					// MENU_INDEX_FACTORY_RESET_OK
    _MENU_LIST_ADD(menuSaveIpResetPage)						// MENU_INDEX_SAVE_IP_RESET
    _MENU_LIST_ADD(firmwareUpdatePage)						// MENU_INDEX_UPDATE_FIRMWARE
	_MENU_LIST_ADD(menuSelectInputPage)						// MENU_INDEX_SELECT_INPUT
	_MENU_LIST_ADD(menuAdjustPage)							// MENU_INDEX_ADJUST
	_MENU_LIST_ADD(menuSysWorkTimePage)						// MENU_INDEX_SYS_WORKTIME
	_MENU_LIST_ADD(menuZoomNewPage)							// MENU_INDEX_ZOOM_NEW
	_MENU_LIST_ADD(menuOLEDLuminancePage)					// MENU_INDEX_OLED_LUMINANCE
	_MENU_LIST_ADD(menuTemperaturePage)						// MENU_INDEX_TEMPERATURE
	_MENU_LIST_ADD(menuColorTemSetmPage)					//130
	_MENU_LIST_ADD(menuGammaSetmPage)						// MENU_INDEX_GAMMASET
#if ENABLE_TEST_GRID_CONTROL
	_MENU_LIST_ADD(menuGridTestPage)						// MENU_IUNDX_TEST_GRIP
	_MENU_LIST_ADD(menuGridDotPage)							// MENU_IUNDX_GRIP_DOT
	_MENU_LIST_ADD(menuGridDotColorPage)					// MENU_IUNDX_GRIP_DOT_COLOR
	_MENU_LIST_ADD(menuGridDotManualPage)					// MENU_IUNDX_GRIP_DOT_MANUAL
#endif	
	_MENU_LIST_ADD(menuVideoPanelFirmwarePage)				// MENU_INDEX_VIDEO_PANEL_FIRMWARE
	_MENU_LIST_ADD(menuVideoPanelFwSendPage)				// MENU_INDEX_VIDEO_PANNEL_FW_SEND
	_MENU_LIST_ADD(menuUsbUpdateConfirmPage)				// MENU_INDEX_USB_UPDATE_CONFIRM
	_MENU_LIST_ADD(artnet_testPage)			        		// MENU_INDEX_ARTNET_TEST
	_MENU_LIST_ADD(menuTwoPortSplitModePage)
	_MENU_LIST_ADD(menuFourPortSplitModePage)
	_MENU_LIST_ADD(menuMIMOPage)
	_MENU_LIST_ADD(pipModePage)
	_MENU_LIST_ADD(menuPipImageAL1Page)
 	_MENU_LIST_ADD(menuPipImageBL1Page)
	//_MENU_LIST_ADD(menuPipSubWinAdj)
	_MENU_LIST_ADD(menuIndependentPage)						//MENU_INDEX_INDEPENDENT
	_MENU_LIST_ADD(menuHorizontalOneSubTwoModePage)			// MENU_INDEX_H_1_SUB_2
	_MENU_LIST_ADD(menuVerticalOneSubTwoModePage)			// MENU_INDEX_V_1_SUB_2
	_MENU_LIST_ADD(menuTwo4kTakePage)						// MENU_INDEX_TWO_4K_TAKE
	_MENU_LIST_ADD(menuFourPortFieldSplitModePage)			// MENU_INDEX_FIELD_SPLIT
	_MENU_LIST_ADD(menuFourPortHorizontal2SplitModePage)	// MENU_INDEX_HORIZONTAL1_2_SPLIT
	_MENU_LIST_ADD(menuFourPortHorizontal3SplitModePage)	// MENU_INDEX_HORIZONTAL1_3_SPLIT
	_MENU_LIST_ADD(menuFourPortHorizontal4SplitModePage)	// MENU_INDEX_HORIZONTAL1_4_SPLIT
	_MENU_LIST_ADD(menuFourPortVertical2SplitModePage)		// MENU_INDEX_VERTICAL1_2_SPLIT
	_MENU_LIST_ADD(menuFourPortvertical4SplitModePage)		// MENU_INDEX_VERTICAL1_4_SPLIT
	_MENU_LIST_ADD(menuEDIDPageHdmi2_0)						//MENU_INDEX_EDID_HDMI20_PORT
	_MENU_LIST_ADD(menuEDIDPageHdmi1_4)						//MENU_INDEX_EDID_HDMI14_PORT
	_MENU_LIST_ADD(menuEDIDPageDp)							//MENU_INDEX_EDID_DP_PORT
	_MENU_LIST_ADD(menuEDIDPageDvi)							//MENU_INDEX_EDID_DVI_PORT
	_MENU_LIST_ADD(outputPortSelPage)						//MENU_INDEX_OUTPUT_PORT_SEL
	_MENU_LIST_ADD(menuOutput0Page)							//MENU_INDEX_OUTPUT0_SET
	_MENU_LIST_ADD(menuOutput1Page)							//MENU_INDEX_OUTPUT1_SET
	_MENU_LIST_ADD(menuOutput2Page)							//MENU_INDEX_OUTPUT2_SET
	_MENU_LIST_ADD(menuOutput3Page)							//MENU_INDEX_OUTPUT3_SET
	_MENU_LIST_ADD(menuOutput0LayerPage)					//MENU_INDEX_OUTPUT0_Layer
	_MENU_LIST_ADD(menuOutput1LayerPage)					//MENU_INDEX_OUTPUT1_Layer
	_MENU_LIST_ADD(menuOutput2LayerPage)					//MENU_INDEX_OUTPUT2_Layer
	_MENU_LIST_ADD(menuOutput3LayerPage)					//MENU_INDEX_OUTPUT3_Layer
	_MENU_LIST_ADD(menuWriteSnPage)							//MENU_INDEX_WRITE_SN
	_MENU_LIST_ADD(menu4pLTPage)							//MENU_INDEX_4PLT
	_MENU_LIST_ADD(menu4pLBPage)							//MENU_INDEX_4PLB
	_MENU_LIST_ADD(menu4pRTPage)							//MENU_INDEX_4PRT
	_MENU_LIST_ADD(menu4pRBPage)							//MENU_INDEX_4PRB
	_MENU_LIST_ADD(menuNaPage)								// MENU_INDEX_NA
    {0,      0,      0},
};


//#endif
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //TIM_OCInitTypeDef  TIM_OCInitStructure ;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    
	TIM_DeInit(TIM2);   
    TIM_TimeBaseStructure.TIM_Period = 9999;       
    TIM_TimeBaseStructure.TIM_Prescaler = 799;      
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//Clear TIM2 update pending flag
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // Enable TIM2 Update interrupt
    TIM_Cmd(TIM2, DISABLE);//
    
	/* Enable the TIM2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //TIM_OCInitTypeDef  TIM_OCInitStructure ;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    
	TIM_DeInit(TIM3);       
    TIM_TimeBaseStructure.TIM_Period = 9999;       
    TIM_TimeBaseStructure.TIM_Prescaler = 799;      
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);//Clear TIM2 update pending flag
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // Enable TIM2 Update interrupt
    TIM_Cmd(TIM3, ENABLE);//
    
	/* Enable the TIM2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


void menuInit(void)
{
	TIM2_Configuration();
    pCurrentMenuPage = &menuList[0];
    pushMenuPage(menuList[0].menuPage);
}

void menuProcessKey(int key)
{
	menuNoOperationTime = GetSysTick();
    pCurrentMenuPage->menuPage->pfProcess(key, 0);
    return;
}

void menuProcessEventValue(int event, int value)
{
	menuNoOperationTime = GetSysTick();
	pCurrentMenuPage->menuPage->pfProcess(event, value);
}

void menuProcessEvent(int key)
{	
	menuProcessKey(key);
}

void returnToLastMenu(void)
{
	menuNoOperationTime = GetSysTick();
    pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);	
	return;
}


//static char dispErrorEnable = 0;
void enableDispErrorInfo(void)
{
//	dispErrorEnable = 1;
}
void disableDispErrorInfo(void)
{
//	dispErrorEnable = 0;
}

u8 bArtNetConnect_old = 0xFF;
u8 oldSendCardConnect = 0xFF;

void taskErrorInfo(void)
{

}



#if 1
int menuIsCurrentPageIndex(int index)
{
	if(pCurrentMenuPage == &menuList[index])
	{
		return TRUE;
	}

	return FALSE;
}


int CurrentIsSavePage(void)
{
	//return(menuIsCurrentPageIndex(MENU_INDEX_SAVE_TO)||menuIsCurrentPageIndex(MENU_INDEX_SAVE_VERIFY));
	return menuIsCurrentPageIndex(MENU_INDEX_SAVE_TO);
}


int CurrentIsLoadFromPage(void)
{
	return menuIsCurrentPageIndex(MENU_INDEX_LOAD_FROM);
}


int CurrentIsEnterNumberPage(void)
{
	return (menuIsCurrentPageIndex(MENU_INDEX_SCREEN)||menuIsCurrentPageIndex(MENU_INDEX_SCALE));
}


int CurrentIsSplitPage(void)
{
	return menuIsCurrentPageIndex(MENU_INDEX_SPLIT);
}

int CurrentIsYesOrNoPage(void)
{
	return (menuIsCurrentPageIndex(MENU_INDEX_SAVE_VERIFY)||menuIsCurrentPageIndex(MENU_INDEX_ADC_AUTO_ADJUST)||menuIsCurrentPageIndex(MENU_INDEX_ADC_RESET_ALL)||menuIsCurrentPageIndex(MENU_INDEX_LOCK_PANEL)||menuIsCurrentPageIndex(MENU_INDEX_FACTORY_RESET_OK)||menuIsCurrentPageIndex(MENU_INDEX_SAVE_IP_RESET));

}


int CurrentIsSelectInputPage(void)
{
	return menuIsCurrentPageIndex(MENU_INDEX_SELECT_INPUT);
}


int CurrentIsUpgradePage(void)
{
	return menuIsCurrentPageIndex(MENU_INDEX_UPDATE_FIRMWARE);
}


int CurrentIsOutputFormatPage(void)
{
	return (menuIsCurrentPageIndex(MENU_INDEX_CUSTOM_FORMAT)||menuIsCurrentPageIndex(MENU_INDEX_STANDARD_FORMAT)||menuIsCurrentPageIndex(MENU_INDEX_OUTPUT_FORMAT));
}


int CurrentIsSaveOrLoadPage(void)
{
	return (menuIsCurrentPageIndex(MENU_INDEX_SAVE_TO)||menuIsCurrentPageIndex(MENU_INDEX_LOAD_FROM));
}


void menuEnterStatusPage(void)
{
	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0); //先退出当前菜单<--20151014
	clearMenuPageStack();
	menuList[MENU_INDEX_STATUS].currentItem = MENU_START_ITEM;
	menuEnterPage(MENU_INDEX_STATUS);
}


void menuEnterPage(int index)
{
	const MenuPage *menuPage;

	if(index >= (sizeof(menuList)/sizeof(MenuItem) - 1))
	{
		return;
	}
	
	menuPage = pCurrentMenuPage->menuPage;
	
	if(!isEnterMenuPage)
	{
		if(menuPage != menuNothingPage)
		{
			pushMenuPage(menuPage);       
		}
	}
	
	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_EXIT, 0);
    pCurrentMenuPage = &menuList[index];

	if(index == MENU_INDEX_NOTHING)
	{
		menuState = MENU_STATE_CHANGE_ITEM;
	}
	else
	{	
    	pCurrentMenuPage->menuPage->pfProcess(MENU_EVENT_ENTER, 0);
	}
	
	isEnterMenuPage = 1;
}


bool isMenuNoOperation30s(void)
{	
	if(menuIsCurrentPageIndex(MENU_INDEX_LOAD_FROM)||menuIsCurrentPageIndex(MENU_INDEX_SAVE_TO)
	 ||menuIsCurrentPageIndex(MENU_INDEX_FACTORY_RESET_OK)||menuIsCurrentPageIndex(MENU_INDEX_UPDATE_FIRMWARE)
	 ||menuIsCurrentPageIndex(MENU_EVENT_UPDATE_FIRMWARE_SUCCESS)||menuIsCurrentPageIndex(MENU_INDEX_TEST_PATTERN)
	 ||menuIsCurrentPageIndex(MENU_INDEX_SPLIT)||menuIsCurrentPageIndex(MENU_INDEX_SPLIT_FIXED)||menuIsCurrentPageIndex(MENU_INDEX_INPUT_CROP)
	 ||menuIsCurrentPageIndex(MENU_INDEX_TP_COLOR)||menuIsCurrentPageIndex(MENU_INDEX_TP_PARAM)) //added by cyj  LOAD和SAVE页不计时
	{
		menuNoOperationTime = GetSysTick();
		return FALSE;
	}
	return isSysTickTimeout(menuNoOperationTime, 30000);
}

char getCurrentIsEnterNumberFlag(void)
{
	return CurrentIsEnterNumberFlag;
}


void SetCurrentIsEnterNumberFlag(char flag)
{
	CurrentIsEnterNumberFlag = flag;
}

void menuTask(void)
{
	if((isMenuNoOperation30s())&&(getProcessKeyEnable() == 1))
	{
		CurrentIsEnterNumberFlag = 0;
		noOperationFlag = 1;
		//printf("menuTask()_MENU_KEY_INFO---->%d",GetSysTick());
		SetKeyLedMode(KEY_MENU,LED_OFF_ALWAYS);
		SetKeyLedMode(KEY_SPLIT,LED_OFF_ALWAYS);
		SetKeyLedMode(KEY_SAVE,LED_OFF_ALWAYS);
		SetKeyLedMode(KEY_LOAD,LED_OFF_ALWAYS);
		SetKeyLedMode(KEY_SCALE,LED_OFF_ALWAYS);
		menuProcessEvent(MENU_KEY_INFO);
	}
	
	if(menuMainTaskFunc == 0)
	{
		return;
	}
	if(isSysTickTimeout(menuTaskTime,menuMainTimeout))
	{
		menuTaskTime = GetSysTick();
		menuMainTaskFunc();
	}
}

int deviceDispCnt = 1000;
void dispDeviceInfo(void)
{
	LCD_Clear();
	if(getDeviceInfo()->logoDisMode & 0x80)
	{
		LCD_WriteLine2("          亮彩系列");
		{
			LCD_WriteLine3("    AVDSP SERIES VSP 198S");
		}
	}
	else
	{
		LCD_WriteLine2("          亮彩系列");
		LCD_WriteLine3("    AVDSP SERIES VSP 198S");
	}
	deviceDispCnt = 1000;
	
}

void dispDeviceInit(void)
{
	#if 1
	{
		LCD_Clear();
		LCD_WriteLine2(langGetStr(initDeviceStr));
		menuSetBusy(1,12);
		setProcessKeyEnable(0);
	}
	#endif
}


#endif


