#ifndef MENU_H
#define MENU_H


#include "define.h"

enum
{
    MENU_TYPE_LABEL,
	MENU_TYPE_READ,
    MENU_TYPE_DIR,
    MENU_TYPE_VALUE,
    MENU_TYPE_ENTER_NUMBER_VALUE,
    MENU_TYPE_LABEL_WITH_VALUE,
    MENU_TYPE_SALENUM,
    MENU_TYPE_CUSTOMNUM,
    MENU_TYPE_EMAIL,
    MENU_TYPE_WEBSITE,
    MENU_TYPE_NO_DRAW_ITEM_BAR,    
    MENU_TYPE_VALUE_ICON,
};

typedef void (*MenuPrucessFun)(int, int);

typedef void (*Exec_Func)(void);


typedef struct
{
    int type;
    int str;
    int value;
    const MenuPrucessFun pfProcess;

} MenuPage;

enum
{
	// menu key
    MENU_KEY_MENU,
    MENU_KEY_NEXT,
    MENU_KEY_SELECT = MENU_KEY_NEXT,
    MENU_KEY_ESC,
    MENU_KEY_LEFT,
    MENU_KEY_RIGHT,
    MENU_KEY_UP,//5
    MENU_KEY_DOWN,
    MENU_KEY_MENU_ROOT,

	// 
	MENU_KEY_PIP,
	MENU_KEY_LAYOUT,
	MENU_KEY_AB,
	MENU_KEY_OUTPUT,
	MENU_KEY_SCALE,
	MENU_KEY_BRT,
	MENU_KEY_MUTE,
	MENU_KEY_MSA,
	MENU_KEY_VOL_UP,
	MENU_KEY_VOL_DOWN,
	MENU_KEY_FS,
	MENU_KEY_FREEZE,
	MENU_KEY_RATIO,
	MENU_KEY_SPLIT,
	MENU_KEY_COM,
	MENU_KEY_MAP,

	// number key
	MENU_KEY_0,
	MENU_KEY_1,
	MENU_KEY_2,
	MENU_KEY_3,
	MENU_KEY_4,
	MENU_KEY_5,
	MENU_KEY_6,
	MENU_KEY_7,
	MENU_KEY_8,
	MENU_KEY_9,
	
	
    
    
    MENU_KEY_INFO, //33

	MENU_KEY_HSIZE,
	MENU_KEY_VSIZE,

	MENU_KEY_HDMI,
	MENU_KEY_HDMI_TAKE,
    MENU_KEY_CV1,
    MENU_KEY_CV2,
    MENU_KEY_CV3,
    MENU_KEY_SV,
    MENU_KEY_DVI,
    MENU_KEY_DVI_TAKE,   
    MENU_KEY_YPbPr,
    MENU_KEY_VGA,
    MENU_KEY_SDI,
    MENU_KEY_SDI_TAKE,
    MENU_KEY_DP,
    MENU_KEY_BLACK,
    MENU_KEY_GRID,
	MENU_KEY_EXT_7,
	MENU_KEY_EXT_8,
	MENU_KEY_EXT_9,

	// save key
	MENU_KEY_SAVE,
	MENU_KEY_SAVE1,
	MENU_KEY_SAVE2,
	MENU_KEY_SAVE3,

	MENU_KEY_LOAD,

	MENU_KEY_UNLOCK,

	MENU_KEY_SIZE,

	
    
	MENU_EVENT_STEP_1,//56
	MENU_EVENT_STEP_10,
	MENU_EVENT_STEP_100,

    MENU_EVENT_ART_NET_TEST,

	MENU_EVENT_UPDATE_FIRMWARE_START,
	MENU_EVENT_UPDATE_FIRMWARE_SUCCESS,
	MENU_EVENT_UPDATE_FIRMWARE_FAIL,
	MENU_EVENT_UPDATE_FIRMWARE_WAIT_AUTO_REBOOT,
	MENU_EVENT_PROGRESS_BAR,
	MENU_EVENT_UPDATE_FIRMWARE_SKIP,

    MENU_EVENT_SEND_TO_PORT,
    MENU_EVENT_SEND_TO_PORT1,
    MENU_EVENT_SEND_TO_PORT2,
    MENU_EVENT_SEND_TO_PORT3,
    MENU_EVENT_SEND_TO_PORT4,
	

	// menu event
    MENU_EVENT_ENTER,//65
    MENU_EVENT_EXIT,
    MENU_EVENT_PREV_VALUE,
    MENU_EVENT_NEXT_VALUE,
    MENU_EVENT_NEXT_ITEM, //69
    MENU_EVENT_PREV_ITEM,
    MENU_EVENT_DRAW_ITEM, //71
    MENU_EVENT_DRAW_ITEM2,			// 
    MENU_EVENT_DRAW_REFESH,			//     
    MENU_EVENT_CHANGE_VALUE,
    MENU_EVENT_SELECT_ITEM,//73
    MENU_EVENT_SET_VALUE, 
    MENU_EVENT_CHANGE_ITEM,
	MENU_EVENT_DRAW_VALUE,

	//
	MENU_EVENT_PREVIEW_SOURCE,
	MENU_EVENT_INPUT_SOURCE,
	MENU_EVENT_ENTER_PAGE,
	MENU_EVENT_LOAD_SAVE,
	MENU_EVENT_RELOAD_SAVE1,
	MENU_EVENT_FREEZE,
	MENU_EVENT_TP,
	MENU_EVENT_BRIGHTNESS,
	MENU_EVENT_FS,
	MENU_EVENT_AB,

	MENU_EVENT_PIP,
	MENU_EVENT_PBP,	

	//guide
	MENU_EVENT_GUIDE_LANGUAGE,
	
	MENU_KEY_LOCK,
	MENU_KEY_BRIGHTNESS,
	MENU_KEY_LANG,
	MENU_KEY_VGA_AUTO_ADJUST,
	MENU_KEY_EXT7_VGA_AUTO_ADJUST,
	MENU_KEY_EXT8_VGA_AUTO_ADJUST,
	MENU_KEY_SCALE_TAKE,
	MENU_KEY_SD_EXHANCE,
	MENU_KEY_BLACK_KEY_DEFAULT,
	
	MENU_EVENT_WAIT_CALL_DELAY,

	MENU_EVENT_DISP_AUDIO_INPUT_CONFIG,
	
	MENU_EVENT_STUDY_EDID_SUCCESS,
	MENU_EVENT_STUDY_EDID_FAIL,

	//��ݼ�
	MENU_EVENT_LANG,
	MENU_EVENT_VGA_AUTO_ADJUST,
	MENU_EVENT_EXT7_VGA_AUTO_ADJUST,
	MENU_EVENT_EXT8_VGA_AUTO_ADJUST,
	MENU_EVENT_USB_PREVIOUS,
	MENU_EVENT_USB_NEXT,
	MENU_EVENT_USB_PLAY,
	MENU_EVENT_USB_PAUSE,
	MENU_EVENT_SCALE_TAKE,
	MENU_EVENT_BLACK_KEY_DEFAULT,
	
	MENU_EVENT_REFLESH_VIDEO_AND_DISP,

	MENU_KEY_TEST_OPERATION_ON,
	MENU_KEY_TEST_MENU,
	MENU_KEY_TEST_PIP,
	MENU_KEY_TEST_SAVE,
	MENU_KEY_TEST_FS,
	MENU_KEY_TEST_SPLIT,
	MENU_KEY_TEST_LOAD,
	MENU_KEY_TEST_HDMI,
	MENU_KEY_TEST_CVBS,
	MENU_KEY_TEST_VGA,
	MENU_KEY_TEST_DVI,
	MENU_KEY_TEST_MODULE_7,
	MENU_KEY_TEST_MODULE_8,
	MENU_KEY_TEST_MODULE_9,
	MENU_KEY_TEST_BLACK,
	MENU_KEY_TEST_SCALE,
	MENU_KEY_TEST_SELECT,
	MENU_KEY_TEST_LEFT,
	MENU_KEY_TEST_RIGHT,


//#######################################################
//and by zyk@20170829
	MENU_KEY_M1,
	MENU_KEY_M2,
	MENU_KEY_M3,
	MENU_KEY_M4,
	MENU_KEY_HDMI1,
	MENU_KEY_HDMI2,
	MENU_KEY_A,
	MENU_KEY_B,
	MENU_KEY_C,
	MENU_KEY_D,
	MENU_KEY_E,
	MENU_KEY_F,
	MENU_KEY_G,
	MENU_KEY_H,
	MENU_KEY_DVI1,
	MENU_KEY_DVI2,
	MENU_KEY_DVI3,
	MENU_KEY_DVI4,

	MENU_KEY_TAKE,

};

// this order is same menuList[]
typedef enum
{
	MENU_INDEX_MAIN,
	MENU_INDEX_STATUS,
	MENU_INDEX_INPUT,
	MENU_INDEX_INPUT_INFO,
	MENU_INDEX_INPUT_HDMI,
	MENU_INDEX_INPUT_DVI,
	MENU_INDEX_INPUT_SDI,
	MENU_INDEX_INPUT_DETAIL,
	MENU_INDEX_INPUT_CROP,
	MENU_INDEX_ZOOM,
	MENU_INDEX_VGA_ADJUST,
	MENU_INDEX_ADC_ADJUST,
	MENU_INDEX_ADC_AUTO_ADJUST,
	MENU_INDEX_ADC_RESET_ALL,
	MENU_INDEX_SDI_ADJUST,
	MENU_INDEX_HDMI_ADJUST,
	MENU_INDEX_EDID,
	MENU_INDEX_CUSTOM_EDID,
	MENU_INDEX_OUTPUT,
	MENU_INDEX_OUTPUT_DETAIL,
	MENU_INDEX_OUTPUT_DVI_DETAIL,
	MENU_INDEX_CUSTOM_FORMAT,
	MENU_INDEX_STANDARD_FORMAT,
	MENU_INDEX_OTHER_FORMAT,
	MENU_INDEX_OUTPUT_FORMAT,
	MENU_INDEX_OUTPUT_ADJUST,
	MENU_INDEX_OUTPUT_DVI_CONFIG,
	MENU_INDEX_DE_ADJUST,
	MENU_INDEX_OUTPUT_DVI_CONFIG_LOOP,
	MENU_INDEX_SCALE,
	MENU_INDEX_OUTPUT_FIT,
	MENU_INDEX_FIT_ADVANCED,
	MENU_INDEX_FIT_RATIO,
	MENU_INDEX_SCREEN,
	MENU_INDEX_SCREEN_HOTKEY,
	MENU_INDEX_PICTURE,
	MENU_INDEX_IMAGE_QUALITY,
	MENU_INDEX_IMAGE_COLORTEMP,
	MENU_INDEX_PIP,
	MENU_INDEX_TEXT_OVERLAY,
	MENU_INDEX_ALPHA,
	MENU_INDEX_DISPLAY_MODE,
	MENU_INDEX_FLAT_COLOR,
	MENU_INDEX_TEST_PATTERN,
	MENU_INDEX_TP_COLOR,
	MENU_INDEX_TP_PARAM,
	MENU_INDEX_TRANSITION,
	MENU_INDEX_IMAGE_ENHANCE_VERIFY,
	MENU_INDEX_AUDIO,
	MENU_INDEX_AUDIO_CONFIG,
	MENU_INDEX_SPLIT,
	MENU_INDEX_SPLIT_FIXED,
	MENU_INDEX_SPLIT_MODE,
	MENU_INDEX_SAVE_SETUP,
	MENU_INDEX_SAVE_TO,
	MENU_INDEX_SAVE_VERIFY,  
	MENU_INDEX_LOAD_FROM,
	MENU_INDEX_USB_SINGLE_LOAD,
	MENU_INDEX_USB_ALL_LOAD,
	MENU_INDEX_USB_ALL_SAVE,
	MENU_INDEX_SYSTEM,
	MENU_INDEX_SYSTEM_INFO,
	MENU_INDEX_SYSTEM_VERSION_SOFT,
	MENU_INDEX_SYSTEM_VERSION_HARD,
	MENU_INDEX_SYSTEM_VERSION,
	MENU_INDEX_TECH_SUPPORT,
	MENU_INDEX_DATE_TIME,
	MENU_INDEX_WORK_TIME,
	MENU_INDEX_TOTAL_WORK_TIME,
	MENU_INDEX_CHANGE_DATE,
	MENU_INDEX_CHANGE_TIME,
	MENU_INDEX_PRV_DBLTAP_SWITCH,
	MENU_INDEX_PREVIEW_SWITCH,
	MENU_INDEX_ADVANCED,
	MENU_INDEX_ETHERNET,
	MENU_INDEX_ARTNET_ETHADDR,
	MENU_INDEX_REMOTE_IP_ADDRESS,
	MENU_INDEX_LOCAL_IP_ADDRESS,
	MENU_INDEX_SUBNET_MASK,
	MENU_INDEX_GATEWAY,
	MENU_INDEX_LOCK_PANEL,
	MENU_INDEX_LOCKPANEL_PASSWORD,
	MENU_INDEX_LOCKPANEL_ENTER_PASSWORD,
	MENU_INDEX_ENTER_PASSWORD,
	MENU_INDEX_CAN_USE_TIME,
	MENU_INDEX_HOT_BACKUP,
	MENU_INDEX_LICENSE_SETUP,
	MENU_INDEX_CHANGE_LICENSE_TIME,
	MENU_INDEX_CHANGE_PASSWORD,
	MENU_INDEX_FANCONTROL,
	MENU_INDEX_DEVICESTATUS,
	MENU_INDEX_USER_DEFINED_BUTTON,
	MENU_INDEX_FACTORY_RESET,
	MENU_INDEX_FACTORY_RESET_OK,
	MENU_INDEX_SAVE_IP_RESET,
	MENU_INDEX_UPDATE_FIRMWARE,
	MENU_INDEX_SELECT_INPUT,
	MENU_INDEX_ADJUST,
	MENU_INDEX_SYS_WORKTIME,
	MENU_INDEX_ZOOM_NEW,
	MENU_INDEX_OLED_LUMINANCE,
	MENU_INDEX_TEMPERATURE,
	MENU_INDEX_COLORTEMPERATURE,
	MENU_INDEX_GAMMASET,

	MENU_IUNDX_TEST_GRIP,
	MENU_IUNDX_GRIP_DOT,
	MENU_IUNDX_GRIP_DOT_COLOR,
	MENU_IUNDX_GRIP_DOT_MANUAL,

	MENU_INDEX_VIDEO_PANEL_FIRMWARE,
	MENU_INDEX_VIDEO_PANNEL_FW_SEND,
	MENU_INDEX_USB_UPDATE_CONFIRM,
	MENU_INDEX_ARTNET_TEST,
	MENU_INDEX_TWO_PORT_SPLIT,
	MENU_INDEX_FOUR_PORT_SPLIT,
	MENU_INDEX_MIMO,
	MENU_INDEX_PIP_MODE,
	MENU_INDEX_PIPIMAGEA,
	MENU_INDEX_PIPIMAGEB,
	MENU_INDEX_INDEPENDENT,
	MENU_INDEX_H_1_SUB_2,
	MENU_INDEX_V_1_SUB_2,
	MENU_INDEX_TWO_4K_TAKE,
	MENU_INDEX_FIELD_SPLIT,
	MENU_INDEX_HORIZONTAL1_2_SPLIT,
	MENU_INDEX_HORIZONTAL1_3_SPLIT,
	MENU_INDEX_HORIZONTAL1_4_SPLIT,
	MENU_INDEX_VERTICAL1_2_SPLIT,
	MENU_INDEX_VERTICAL1_4_SPLIT,
	MENU_INDEX_EDID_HDMI20_PORT,
	MENU_INDEX_EDID_HDMI14_PORT,
	MENU_INDEX_EDID_DP_PORT,
	MENU_INDEX_EDID_DVI_PORT,
	MENU_INDEX_OUTPUT_PORT_SEL,
	MENU_INDEX_OUTPUT0_SET,
	MENU_INDEX_OUTPUT1_SET,
	MENU_INDEX_OUTPUT2_SET,
	MENU_INDEX_OUTPUT3_SET,
	MENU_INDEX_OUTPUT0_Layer,
	MENU_INDEX_OUTPUT1_Layer,
	MENU_INDEX_OUTPUT2_Layer,
	MENU_INDEX_OUTPUT3_Layer,
	MENU_INDEX_WRITE_SN,
	MENU_INDEX_4PLT,
	MENU_INDEX_4PLB,
	MENU_INDEX_4PRT,
	MENU_INDEX_4PRB,
	MENU_INDEX_NOTHING,
	MENU_INDEX_SIZE,

}MenuPageIndex;

void refleshDispEDIDValue(void);

int  menuIsCurrentPageIndex(int index);
void menuProcessEvent(int key);
void menuProcessKey(int key);
void menuProcessEventValue(int event, int value);
void menuEnterPage(int index);
void menuEnterStatusPage(void);
void menuInit(void);
void menuTask(void);
void menuSetBusy(int row, int column);
void menuSetBusy_Dot(int row, int column);
void menuDisableBusy(void);
void menuSetFinish(char row,char column);

void refleshDispVslue(void);
void setCustomValue(void);
int getDispCustomValue(int index);
//int getCustomHTotal(void);
int getCustomVTotal(void);
//int getCustomHStart(void);
int getCustomVStart(void);
int getCustomVFreq(void);
//int getCustomHSyncWidth(void);
int getCustomVSyncWidth(void);
//int getCustomHVSyncPolarity(void);
int getCustomDisplayWidth(void);
int getCustomDisplayHeight(void);
char setCustomEDIDValue(int h_pixels,int v_lines,int freq);

void setCustomFormat(void);

void setm_customVideo(int type,int value);
void setm_customResolution(int type,int value);
char ifCustomFormat(void);
char ifStandardFormatPage(void);

int CurrentIsSavePage(void);
int CurrentIsLoadFromPage(void);
int currentIsGuidePage(void);
int currentIsGuideScreenPage(void);
int CurrentIsGuideLanguagePage(void);
int CurrentIsGuideOutputFormatPage(void);
int CurrentIsGuideSourcePage(void);
int CurrentIsGuideSetSavePage(void);
int CurrentIsGuideSaveToPage(void);
int CurrentIsEnterNumberPage(void);
int CurrentIsSplitPage(void);
char getCurrentIsEnterNumberFlag(void);
int CurrentIsYesOrNoPage(void);
int CurrentIsSelectInputPage(void);
int CurrentIsUpgradePage(void);
int CurrentIsOutputFormatPage(void);
int CurrentIsSaveOrLoadPage(void);

void setReplaceRecordEn(int value);

void gotoImageEnhanceVerity(void);

void initCustomByComm(void);
void setCustomByComm(int type, int value);
int getCustomByComm(int type);
int setCustomResolution(void);

void USBUdateTips(void);
void defaultMenuPageProcess(int type, int value);

void dispDeviceInfo(void);
void dispDeviceInit(void);


void returnToLastMenu(void);
void dispFactoryReset(void);

void disableDispReflrshInputFormat(void);
void enableDispReflrshInputFormat(void);
char getIsDispReflrshInputFormatEnable(void);

void enableDispErrorInfo(void);
void disableDispErrorInfo(void);
void taskErrorInfo(void);
void SetInputPortLedStatus(u8 PgmInput,u8 PrvInput,u8 disMode);
void TIM3_Configuration(void);

u8 InputSourceConv(u8 x);
u8 InputSourceConv1(u8 x);

void SetEcInputValueNew(void);
void ClearEcInputValueNew(void);
void SetEcInputValueOld(void);
void ClearEcInputValueOld(void);
u16 GetEcInputValueNew(void);
u16 GetEcInputValueOld(void);

void SetIsEcInputFinish(u8 value);
u16 GetIsEcInputFinish(void);


#endif // MENU_H


