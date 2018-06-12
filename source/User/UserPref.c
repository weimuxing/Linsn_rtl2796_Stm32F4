#include <stdio.h>
#include "systick.h"
#include <string.h>
#include "SysConfig.h"
#include "userPref.h"
#include "I2CEEPROM.h"
#include "videoFormatTable.h"
#include "i2c.h"
#include "Control.h"
#include "input.h"
#include "RTD2796.h"
#include "fpga.h"
#include "menu.h"
#include "UartDebug.h"
#include "soft_spi.h"
#include "Spi_flash.h"


static TempData_TypeDef   _tempdata;
static UserPref_TypeDef _userPref;
static DeviceInfo_TypeDef _deviceInfo;
UserSettingParam_Typedef _userSettingParam;
PanelSplitParamSt _panelSplitInfo; 

PipWinSizeSt stPipWin;
//pPipWinSizeSt pstPipWin;

typedef struct
{
	PanelSplitParamSt stFpgaSplitParam;
	u8 FpgaDisplayModeValue;
}STFPGASPLIEPARAM,*pSTFPGASPLIEPARAM;


PipWinSizeSt stPipWinSizeBackUp;

u8 sendcardChoose = 0;
u8 sendcardSetWidth = 0;
u8 sendcardWidth1 = 0;
u8 sendcardWidth2 = 0;
u8 sendcardWidth3 = 0;
u8 sendcardWidth4 = 0;

#define LICENSE_ID (0x20090721)

License_TypeDef _license;



static int eepromSize = 0;

void checkSizeEEPROM(void)
{
	eepromSize = 0;
	eepromSize = EEPROM_SIZE;
	DebugMessageUserPref("EEPROM size = %d,", EEPROM_SIZE);
	DebugMessageUserPref("DEVICE_INFO_ADDRESS=%d\r\n",DEVICE_INFO_ADDRESS);
	DebugMessageUserPref("USER_SETTING_SAVE1_ADDRESS=%d\r\n",USER_SETTING_SAVE1_ADDRESS);
	DebugMessageUserPref("USER_SETTING_SAVE_FLAG_ADDRESS=%d\r\n",USER_SETTING_SAVE_FLAG_ADDRESS);	
}

int getSizeEEPROM(void)
{
	return eepromSize;
}


#define TIME_LIST_SIZE (16)

u32 currentIndexAddress = 0;

void saveValueToEEPROM(u32 address, u32 value, u32 size)
{
	int i;
	BYTE buffer[4];
	if(size > 4)
	{
		size = 4;
	}
	for(i=0; i<size; i++)
	{
		buffer[i] = value>>(8*i);		
	}
	eeprom_write(USER_SAVE_EEPROM_PORT,address,buffer,size);
}



void loadUserPref(void)
{
	DebugMessageUserPref("%s\r\n",__func__);
	eeprom_read(USER_SAVE_EEPROM_PORT,USER_PREF_ADDRESS,(BYTE * )&_userPref, sizeof(_userPref));
	if(_userPref.userLang > 1)
		_userPref.userLang = 0;
	lang_SetLang(_userPref.userLang&0x1);
}



void saveUserPref(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_PREF_ADDRESS,(BYTE * )&_userPref, sizeof(_userPref));
}



static PadLock_Typedef padLockControl = {USER_PADLOCK_ID,0,0,0,0};

PadLock_Typedef * getPadLock(void)
{
	return &padLockControl;
}

void resetUserPadLock(void)
{
	padLockControl.padLockId = USER_PADLOCK_ID;
	padLockControl.PadLockEnable = 0;
	padLockControl.PadLockState = 0;	
	
	getPadLock()->lockPanelPassword = 0;
	getPadLock()->keyLock = 0;

	saveUserPadLock();
}


BYTE getUserPadLockState(void)
{
	return padLockControl.PadLockState;
}


BYTE getUserPadLockEnable(void)
{
	return padLockControl.PadLockEnable;
}




void setUserPadLock(BYTE enable, BYTE value)
{
	padLockControl.PadLockEnable = enable;
	padLockControl.PadLockState = enable ? value : 0;
}


void saveUserPadLock(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,PAD_LOCK_ADDRESS,(BYTE * )&padLockControl, sizeof(padLockControl));
}


void loadPadLock(void)
{
	DebugMessageUserPref("%s\r\n",__func__);
	eeprom_read(USER_SAVE_EEPROM_PORT,PAD_LOCK_ADDRESS,(BYTE * )&padLockControl, sizeof(padLockControl));

	padLockControl.PadLockEnable = 0;
	padLockControl.PadLockState = 0;
    if(padLockControl.padLockId != USER_PADLOCK_ID)
    {
        resetUserPadLock();
    }
}


BYTE getUserLang(void)
{
	return _userPref.userLang;
}

void saveUserLang(BYTE lang)
{
	_userPref.userLang = lang;
	DebugMessageUserPref("language = %d\r\n", lang);
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_PREF_ADDRESS,(BYTE * )&_userPref, sizeof(_userPref));
}




BYTE getUserOLEDLum(void)
{
	return _userPref.OLEDLum;
}




BYTE getUserEDIDMode(BYTE inputPort)
{
	if(inputPort > 4)
	{
		return 0;
	}
	return _userPref.edidFollowEn[inputPort];
}

void saveUserEDIDMode(BYTE inputPort,BYTE value)
{
	if((inputPort > 3)||(value > 2))
	{
		return;
	}
	_userPref.edidFollowEn[inputPort] = value;
}

static void resetDeviceInfo(void)
{
	DebugMessageUserPref("reset Device info\r\n");
	_deviceInfo.deviceId = DEVICE_ID;
	_deviceInfo.deviceType = DEVICE_TYPE;
#if !(CURRENT_DEVICE == _VIP_SMART)
	_deviceInfo.sn = DEVICE_DEFAULT_SN;		// SN只是一个可读的，写是一个特殊命令
#endif
	//_deviceInfo.SoftVersion = 0;//SOFT_VERSION; // add by long

	strcpy(_deviceInfo.companyLogoStr, "GreatView");
	strcpy(_deviceInfo.deviceLogoStr,  "远大视讯");

	
	strcpy(_deviceInfo.deviceTypeStr,  " >VIP Drive");
	strcpy(_deviceInfo.deviceTypeStr2, "VIP Drive");

	strcpy(_deviceInfo.saleNumStr,   "  4008-592-114");
	strcpy(_deviceInfo.customNumStr, "	4008-592-315");

	strcpy(_deviceInfo.emailStr,   "	rgblinkcs@gmail.com");
	strcpy(_deviceInfo.websiteStr, "  www.rgblink.com");

	_deviceInfo.disableDisplayLogo = 0;
	_deviceInfo.logoDisMode = 0xB3;
	_deviceInfo.DeviceModel = 0x0198; //VSP 198
	_deviceInfo.extType = 0; //no ext board
	
}

char save_flag[36];
#define Readsaveflag()   eeprom_read(USER_SAVE_EEPROM_PORT,USER_SETTING_SAVE_FLAG_ADDRESS,(BYTE *) save_flag,36)
#define Writesaveflag(value) eeprom_write(USER_SAVE_EEPROM_PORT,USER_SETTING_SAVE_FLAG_ADDRESS+value,(BYTE *) (&save_flag[value]),1)

void loadDeviceInfo(void)
{
	DebugMessageUserPref("%s\r\n",__func__);
	eeprom_read(USER_SAVE_EEPROM_PORT,DEVICE_INFO_ADDRESS,(BYTE * )&_deviceInfo, sizeof(_deviceInfo));
	DebugMessageUserPref("----Device Info----\r\n");
 	DebugMessageUserPref("Device SN = 0x%x,", _deviceInfo.sn);
	DebugMessageUserPref("Device ID = 0x%x,", _deviceInfo.deviceId);
	DebugMessageUserPref("Device TYPE = 0x%x\r\n", _deviceInfo.deviceType);	
	DebugMessageUserPref("Device deviceTypeStr = 0x%s\r\n", _deviceInfo.deviceTypeStr);
	DebugMessageUserPref("Device DeviceModel = 0x%x\r\n", _deviceInfo.DeviceModel);
	DebugMessageUserPref("------------------------\r\n");		
}

void saveDeviceInfo(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,DEVICE_INFO_ADDRESS,(BYTE * )&_deviceInfo, sizeof(_deviceInfo));
}

u32 getDeviceSN(void)
{
	return _deviceInfo.sn;
}

void saveDeviceSN(u32 sn)
{
	_deviceInfo.sn = sn;
	eeprom_write(USER_SAVE_EEPROM_PORT,DEVICE_INFO_ADDRESS,(BYTE * )&_deviceInfo, sizeof(_deviceInfo));
}

u32 getDeviceModel(void)
{
	return _deviceInfo.DeviceModel;
}

void saveDeviceModel(u32 sn)
{
	_deviceInfo.DeviceModel = sn;
	eeprom_write(USER_SAVE_EEPROM_PORT,DEVICE_INFO_ADDRESS,(BYTE * )&_deviceInfo, sizeof(_deviceInfo));
}

DeviceInfo_TypeDef * getDeviceInfo(void)
{
	return &_deviceInfo;
}


#if 0

#else
#define ReadUserSetting(saveIndex)   eeprom_read(USER_SAVE_EEPROM_PORT,USER_SETTING_SAVE1_ADDRESS + saveIndex * sizeof(UserSettingParam_Typedef),(BYTE *) &_userSettingParam,sizeof(UserSettingParam_Typedef))
#define WriteUserSetting(saveIndex)  eeprom_write(USER_SAVE_EEPROM_PORT,USER_SETTING_SAVE1_ADDRESS + saveIndex * sizeof(UserSettingParam_Typedef),(BYTE *) &_userSettingParam,sizeof(UserSettingParam_Typedef))
#endif

static BYTE RecordStatus[USER_SAVE_MAX_SIZE] = {0};
//static UserSaveIndex_TypeDef CurrentRecordIndex = USER_SAVE_1; 
static UserSaveIndex_TypeDef CurrentRecordIndex = USER_SAVE_MAX_SIZE; 
static BYTE isFirstPowerAfterReset = 0;

BYTE getIsFirstPowerAfterReset(void)
{
	return isFirstPowerAfterReset;
}

int getUserSettingStatus(UserSaveIndex_TypeDef saveId)
{
}


int getAllUserSettingStatus(void)
{
	BYTE i;
	int non_usedCnt = 0;
	UserSettingParam_Typedef read_buffer;
	for(i = 0; i < USER_SAVE_MAX_SIZE; i++)
	{
		eeprom_read(USER_SAVE_EEPROM_PORT,USER_SETTING_SAVE1_ADDRESS + i * sizeof(UserSettingParam_Typedef),(BYTE *) &read_buffer,sizeof(UserSettingParam_Typedef)) ;
		DebugMessageUserPref("recoed %d is %d\r\n",i,read_buffer.userSettingStatus);
		if(read_buffer.userSettingStatus == USER_SETTING_USED)
		{
			RecordStatus[i] = TRUE;
		}
		else
		{
			non_usedCnt++;
			RecordStatus[i] = FALSE;
		}
	}
	return non_usedCnt;
}

BYTE isUserSettingUsed_ram(UserSaveIndex_TypeDef saveIndex)
{
	return RecordStatus[saveIndex];
}

UserSaveIndex_TypeDef getCurrentRecordIndex(void)
{
	return CurrentRecordIndex;
}

void setCurrentRecordIndex(int value)
{
	CurrentRecordIndex = (UserSaveIndex_TypeDef)value;
}

void loadUserSetting(UserSaveIndex_TypeDef saveIndex)
{
	CurrentRecordIndex = saveIndex;
	ReadUserSetting(saveIndex);
	DebugMessageUserPref("_userSettingParam.userSettingId = %x,_userSettingParam.endId= %x\r\n",_userSettingParam.userSettingId,_userSettingParam.endId);
	DebugMessageUserPref("sizeof(UserSetting_TypeDef) = %x\r\n",sizeof(UserSettingParam_Typedef));
	if((_userSettingParam.userSettingId != USER_SETTING_ID)||(_userSettingParam.endId != END_ID))
	{
		DebugMessageUserPref("Reset User Setting\r\n");
		resetUserSettingParam();
		WriteUserSetting((int)saveIndex);
	}
}


UserSettingParam_Typedef userMode;


UserSettingParam_Typedef *GetUserSettingSave(void)
{
	return &userMode;
}

bool getUserMode(UserSaveIndex_TypeDef saveIndex)
{
	u16 addr_beg = USER_SETTING_SAVE1_ADDRESS;
	u16 param_len = sizeof(UserSettingParam_Typedef);
	eeprom_read(USER_SAVE_EEPROM_PORT,addr_beg + saveIndex * param_len,(BYTE *) &userMode,sizeof(UserSettingParam_Typedef));
	if(userMode.userSettingId != USER_SETTING_ID)
	{
		DebugMessageUserPref("Reset User Setting\r\n");
		return false;
	}
	return true;
}



void setUserMode(UserSaveIndex_TypeDef saveIndex)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_SETTING_SAVE1_ADDRESS + saveIndex * sizeof(UserSettingParam_Typedef),(BYTE *) &userMode,sizeof(UserSettingParam_Typedef));
}



void Fac_UserSettingSaveTo(UserSaveIndex_TypeDef saveIndex)
{
	WriteUserSetting((int)saveIndex);
}



void saveUserSeetingToSave1AndNoVideo(void)
{
	WriteUserSetting(USER_SAVE_1);
}


void UserSettingSaveTo(UserSaveIndex_TypeDef saveIndex)
{
	u16 addr_beg = USER_SETTING_SAVE1_ADDRESS;
	u16 param_len = sizeof(UserSettingParam_Typedef);
	//WriteUserSetting((int)saveIndex);
	
	eeprom_write(USER_SAVE_EEPROM_PORT,addr_beg + saveIndex * param_len,(BYTE *) &_userSettingParam,sizeof(UserSettingParam_Typedef));
}


u8* GetWriteBufAddr(void);
u8* GetReadBufAddr(void);


void SaveFpgaSplitParamToFlash(UserSaveIndex_TypeDef saveIndex)
{
	//STFPGASPLIEPARAM stfpgaSplitData;
	PanelSplitParamSt *stfpgaSplitData;
	u8* writeBufAddr;
	u8* readBufAddr;
	writeBufAddr = GetWriteBufAddr();
	readBufAddr = GetReadBufAddr();
	stfpgaSplitData = &_panelSplitInfo;
	SoftSpi_UsePort(SPI_PORT_C);
	SPI_FLASH_BufferRead(readBufAddr,FPGA_SPLIT_PARAM_ADDR_BASD,4096);
	memcpy((readBufAddr + saveIndex*sizeof(PanelSplitParamSt)),\
			(u8*)stfpgaSplitData,sizeof(PanelSplitParamSt));
	writeBufAddr = readBufAddr;
	stfpgaSplitData = (PanelSplitParamSt *)readBufAddr;
	SPI_FLASH_SectorErase(FPGA_SPLIT_PARAM_ADDR_BASD);
	SPI_FLASH_BufferWrite(writeBufAddr,FPGA_SPLIT_PARAM_ADDR_BASD,4096);
	SoftSpi_UnUsePort(SPI_PORT_C);	
}



PanelSplitParamSt* LoadFpgaSplitFromFlash(UserSaveIndex_TypeDef saveIndex)
{
	u32 addr;
	addr = FPGA_SPLIT_PARAM_ADDR_BASD + sizeof(PanelSplitParamSt)*saveIndex;
	SoftSpi_UsePort(SPI_PORT_C);
	SPI_FLASH_BufferRead((u8*)&_panelSplitInfo,addr,sizeof(PanelSplitParamSt));
	SoftSpi_UnUsePort(SPI_PORT_C);
	return &_panelSplitInfo;
}



void SaveUserSetting(UserSaveIndex_TypeDef saveIndex)
{
	_userSettingParam.userSettingStatus = USER_SETTING_USED;
	_userSettingParam.userSettingId = USER_SETTING_ID;
	CurrentRecordIndex = saveIndex;
	RecordStatus[saveIndex] = TRUE;
	
	UserSettingSaveTo(saveIndex);
	SaveFpgaSplitParamToFlash(saveIndex+1);
	SysDelay1ms(100);
	SetInputPortLedStatus(GetPgmCurrentInput(),GetPrvCurrentInput(),GetPgmCurrentDisplayMode());
}


char get_save_flag(int value)
{
	return save_flag[value];
}



void set_save_flag(int value,int value1)
{
	save_flag[value] = value1;
	Writesaveflag((int)value);
}


UserSettingParam_Typedef *GetUserSettingParam(void)
{
	return &_userSettingParam;
}



UserPref_TypeDef * GetUserPref(void)
{
	return &_userPref;
}


void SaveUserPref(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_PREF_ADDRESS,(BYTE * )&_userPref, sizeof(_userPref));
}


void resetLicense(void)
{
	_license.canUseTime = 30*24*60;
	_license.enable = 1;  
	_license.id = LICENSE_ID;
	_license.password = 0x090721;
}

void resetUserPref(void)
{
	char i;
	
	_userPref.userPrefId = USER_PREF_ID;
	_userPref.userLang = 0; // english
	_userPref.noUser1 = 0;
	_userPref.noUser2 = 0;
	
	if(_userPref.LoadFadeInoutOff > 1)
	{
		_userPref.LoadFadeInoutOff = 1; // on
	}
	_userPref.netWorkSetting.artNetStartAddr = 1;
	_userPref.netWorkSetting.artNetUniverse = 0;	
	_userPref.netWorkSetting.DHCP = 0;// off dhcp           //复位默认关闭DHCP
	_userPref.netWorkSetting.IpAdr[0] = 2;//192;
	_userPref.netWorkSetting.IpAdr[1] = 0;//168;
	_userPref.netWorkSetting.IpAdr[2] = 0;
	_userPref.netWorkSetting.IpAdr[3] = 11;//100;
	_userPref.netWorkSetting.NetMask[0] = 255;
	_userPref.netWorkSetting.NetMask[1] = 0;//255;
	_userPref.netWorkSetting.NetMask[2] = 0;//255;
	_userPref.netWorkSetting.NetMask[3] = 0;
	_userPref.netWorkSetting.DefGW[0] = 192;
	_userPref.netWorkSetting.DefGW[1] = 168;
	_userPref.netWorkSetting.DefGW[2] = 0;
	_userPref.netWorkSetting.DefGW[3] = 1;
	_userPref.delayCallTime = 0;
	_userPref.delayCallIndex = 0; // save 1
	
	for(i=0; i<4; i++)
	{
	
		_userPref.deSetting[i].enableDE = 0; // disable DE
		_userPref.deSetting[i].enableHDMI = 0;
		_userPref.deSetting[i].enableDataRange = 0;
		_userPref.deSetting[i].bitWidth = 0;
		_userPref.deSetting[i].HPolarity = 0;
		_userPref.deSetting[i].VPolarity = 0;
		_userPref.deSetting[i].colorSpace = 0;   // 0 -- RGB
		_userPref.edidFollowEn[i] = 0; //disable edid follow
	}
	_userPref.edidFollowEn[4] = 0; //disable edid follow
	_userPref.enableNetwork = 0;
	_userPref.OLEDLum = 12;
	_userPref.temperatureMode = 0;
	_userPref.fanControl.autoDuty = 1;
	_userPref.fanControl.fanDutyRatio = 50;
	
	_userPref.audioInput.AudioInputDetail[0] = VIPDRIVE_INPUT_DVI;
	_userPref.audioInput.AudioInputDetail[1] = VIPDRIVE_INPUT_HDMI;
	_userPref.audioInput.AudioInputDetail[2] = VIPDRIVE_INPUT_HDMI2;
	_userPref.realTimeSaveEn = 1;
    _userPref.SetWidthH = 1920;
    _userPref.SetWidthV = 1080;
    _userPref.setWidthEnable = FALSE;
	_userPref.output1Config = 0;
	_userPref.userPrefId_end = USER_PREF_ID_END;	
}


License_TypeDef * getLicense(void)
{
	if(_license.id != LICENSE_ID)
	{
		if(_license.id != LICENSE_ID)
		{
			resetLicense();
		}
	}
	return &_license;
}


void resetUserSettingParam(void)
{
	u8 i;
    UserSettingParam_Typedef *  userSettingParam = GetUserSettingParam(); 
    SPLIT_PARAM_Typedef * SplitParam; 
	OUTPUT_TYPE_PARAM_Typedef * OutputTypeParam;
	SystemTypeDef* SystemParam;
	pPipWinSizeSt stPipParam;

	SplitParam = &(userSettingParam->userSplitParam);
	OutputTypeParam = &(userSettingParam->stOutputTypeParam);
	SystemParam = &(userSettingParam->stSystemParam);
	stPipParam = &(userSettingParam->stPipWinSize);

	userSettingParam->userSettingId = USER_SETTING_ID;
	userSettingParam->endId = END_ID;
	
	SplitParam->fieldSplit_UserTotalDisplayWidth = 3840;	//用户显示区域总宽
	SplitParam->fieldSplit_UserTotalDisplayHeight = 2160;	//用户显示区域总高
	SplitParam->fieldSplit_FirstDisplayWidth = 1920;		//第一块屏总宽(拼接)
	SplitParam->fieldSplit_FirstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->fieldSplit_SecondDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->fieldSplit_SecondDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->fieldSplit_ThirdDisplayWidth = 1920;		//第三块屏总宽(拼接)
	SplitParam->fieldSplit_ThirdDisplayHeight = 1080;		//第三块屏总高(拼接)
	SplitParam->fieldSplit_FourthDisplayWidth = 1920;		//第四块屏总宽(拼接)
	SplitParam->fieldSplit_FourthDisplayHeight = 1080;		//第四块屏总高(拼接)

	SplitParam->leftRight12_UserTotalDisplayWidth = 3840;	//用户显示区域总宽
	SplitParam->leftRight12_UserTotalDisplayHeight = 1080;	//用户显示区域总高
	SplitParam->leftRight12_FirstDisplayWidth = 1920;		//第一块屏总宽(拼接)
	SplitParam->leftRight12_FirstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->leftRight12_SecongDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->leftRight12_SecongDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->leftRight12_ThirdDisplayWidth = 1920;		//第三块屏总宽(拼接)
	SplitParam->leftRight12_ThirdDisplayHeight = 1080;		//第三块屏总高(拼接)
	SplitParam->leftRight12_FourthDisplayWidth = 1920;		//第四块屏总宽(拼接)
	SplitParam->leftRight12_FourthDisplayHeight = 1080;		//第四块屏总高(拼接)

	SplitParam->topBotton12_UserTotalDisplayWidth = 1920;	//用户显示区域总宽
	SplitParam->topBotton12_UserTotalDisplayHeight = 2160;	//用户显示区域总高
	SplitParam->topBotton12_firstDisplayWidth = 1920;		//第一块屏总宽(拼接)
	SplitParam->topBotton12_firstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->topBotton12_secongDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->topBotton12_secongDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->topBotton12_thirdDisplayWidth = 1920;		//第三块屏总宽(拼接)
	SplitParam->topBotton12_thirdDisplayHeight = 1080;		//第三块屏总高(拼接)
	SplitParam->topBotton12_fourthDisplayWidth = 1920;		//第四块屏总宽(拼接)
	SplitParam->topBotton12_fourthDisplayHeight = 1080;		//第四块屏总高(拼接)

	SplitParam->FourPort12_UserTotalDisplayWidth = 3840;	//用户显示区域总宽
	SplitParam->FourPort12_UserTotalDisplayHeight = 1080;	//用户显示区域总高
	SplitParam->FourPort12_FirstDisplayWidth = 1920;		//第一块屏总宽(拼接)
	SplitParam->FourPort12_FirstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->FourPort12_SecongDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->FourPort12_SecongDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->FourPort12_ThirdDisplayWidth = 0;			//第三块屏总宽(拼接)
	SplitParam->FourPort12_ThirdDisplayHeight = 0;			//第三块屏总高(拼接)
	SplitParam->FourPort12_FourthDisplayWidth = 0;			//第四块屏总宽(拼接)
	SplitParam->FourPort12_FourthDisplayHeight = 0;			//第四块屏总高(拼接)

	SplitParam->FourPortVer12_UserTotalDisplayWidth = 1920;		//用户显示区域总宽
	SplitParam->FourPortVer12_UserTotalDisplayHeight = 2160;	//用户显示区域总高
	SplitParam->FourPortVer12_FirstDisplayWidth = 1920;			//第一块屏总宽(拼接)
	SplitParam->FourPortVer12_FirstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->FourPortVer12_SecongDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->FourPortVer12_SecongDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->FourPortVer12_ThirdDisplayWidth = 0;			//第三块屏总宽(拼接)
	SplitParam->FourPortVer12_ThirdDisplayHeight = 0;			//第三块屏总高(拼接)
	SplitParam->FourPortVer12_FourthDisplayWidth = 0;			//第四块屏总宽(拼接)
	SplitParam->FourPortVer12_FourthDisplayHeight = 0;			//第四块屏总高(拼接)

	SplitParam->leftRight13_UserTotalDisplayWidth = 5760;	//用户显示区域总宽
	SplitParam->leftRight13_UserTotalDisplayHeight = 1080;	//用户显示区域总高
	SplitParam->leftRight13_FirstDisplayWidth = 1920;		//第一块屏总宽(拼接)
	SplitParam->leftRight13_FirstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->leftRight13_SecongDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->leftRight13_SecongDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->leftRight13_ThirdDisplayWidth = 1920;		//第三块屏总宽(拼接)
	SplitParam->leftRight13_ThirdDisplayHeight = 1080;		//第三块屏总高(拼接)
	SplitParam->leftRight13_FourthDisplayWidth = 0;			//第四块屏总宽(拼接)
	SplitParam->leftRight13_FourthDisplayHeight = 0;		//第四块屏总高(拼接)

	SplitParam->leftRight14_UserTotalDisplayWidth = 7680;	//用户显示区域总宽
	SplitParam->leftRight14_UserTotalDisplayHeight = 1080;	//用户显示区域总高
	SplitParam->leftRight14_FirstDisplayWidth = 1920;		//第一块屏总宽(拼接)
	SplitParam->leftRight14_FirstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->leftRight14_SecongDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->leftRight14_SecongDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->leftRight14_ThirdDisplayWidth = 1920;		//第三块屏总宽(拼接)
	SplitParam->leftRight14_ThirdDisplayHeight = 1080;		//第三块屏总高(拼接)
	SplitParam->leftRight14_FourthDisplayWidth = 1920;		//第四块屏总宽(拼接)
	SplitParam->leftRight14_FourthDisplayHeight = 1080;		//第四块屏总高(拼接)

	SplitParam->topBotton14_UserTotalDisplayWidth = 1920;	//用户显示区域总宽
	SplitParam->topBotton14_UserTotalDisplayHeight = 4320;	//用户显示区域总高
	SplitParam->topBotton14_firstDisplayWidth = 1920;		//第一块屏总宽(拼接)
	SplitParam->topBotton14_firstDisplayHeight = 1080;		//第一块屏总高(拼接)
	SplitParam->topBotton14_secongDisplayWidth = 1920;		//第二块屏总宽(拼接)
	SplitParam->topBotton14_secongDisplayHeight = 1080;		//第二块屏总高(拼接)
	SplitParam->topBotton14_thirdDisplayWidth = 1920;		//第三块屏总宽(拼接)
	SplitParam->topBotton14_thirdDisplayHeight = 1080;		//第三块屏总高(拼接)
	SplitParam->topBotton14_fourthDisplayWidth = 1920;		//第四块屏总宽(拼接)
	SplitParam->topBotton14_fourthDisplayHeight = 1080;		//第四块屏总高(拼接)

	SplitParam->_4P_UserTotalDisplayWidth = 3840;			//用户显示区域总宽
	SplitParam->_4P_UserTotalDisplayHeight = 2160;			//用户显示区域总高
	SplitParam->_4P_FirstDisplayWidth = 1920;				//第一块屏总宽(拼接)
	SplitParam->_4P_FirstDisplayHeight = 1080;				//第一块屏总高(拼接)
	SplitParam->_4P_SecongDisplayWidth = 1920;				//第二块屏总宽(拼接)
	SplitParam->_4P_SecongDisplayHeight = 1080;				//第二块屏总高(拼接)
	SplitParam->_4P_ThirdDisplayWidth = 1920;				//第三块屏总宽(拼接)
	SplitParam->_4P_ThirdDisplayHeight = 1080;				//第三块屏总高(拼接)
	SplitParam->_4P_FourthDisplayWidth = 1920;				//第四块屏总宽(拼接)
	SplitParam->_4P_FourthDisplayHeight = 1080;				//第四块屏总高(拼接)
	
	SplitParam->alpha = 0xff;
	
	SplitParam->dvi1LayerAlpha_A = 0xff;
	SplitParam->dvi1LayerAlpha_B = 0xff;
	SplitParam->dvi2LayerAlpha_A = 0xff;
	SplitParam->dvi2LayerAlpha_B = 0xff;
	SplitParam->dvi3LayerAlpha_A = 0xff;
	SplitParam->dvi3LayerAlpha_B = 0xff;
	SplitParam->dvi4LayerAlpha_A = 0xff;
	SplitParam->dvi4LayerAlpha_B = 0xff;
	
	SplitParam->outputFormatIndex = 10;

	SplitParam->splitMode = _OSD_DM_1P;
	
	SplitParam->inputSource1 = _HDMI20_INPUT;
	SplitParam->inputSource2 = _DP_INPUT;
	SplitParam->inputSource3 = _HDMI14_INPUT;
	SplitParam->inputSource4 = _DVI_INPUT;

	SplitParam->outputPort1Map = DVI0_OUTPUT;
	SplitParam->outputPort2Map = DVI1_OUTPUT;
	SplitParam->outputPort3Map = DVI2_OUTPUT;
	SplitParam->outputPort4Map = DVI3_OUTPUT;

	SplitParam->stCustomFormat.hActive = 1920;
	SplitParam->stCustomFormat.vActive = 1080;
	SplitParam->stCustomFormat.htotal = 2200;
	SplitParam->stCustomFormat.vtotal = 1125;
	SplitParam->stCustomFormat.hstart = 132;
	SplitParam->stCustomFormat.vstart = 8;
	SplitParam->stCustomFormat.pClk = 148500000;
	SplitParam->stCustomFormat.hSyncWidth = 44;
	SplitParam->stCustomFormat.vSyncWidth = 5;
	SplitParam->stCustomFormat.hvSyncPolarity = 2;

	OutputTypeParam->output0Type = 0;
	OutputTypeParam->output1Type = 0;
	OutputTypeParam->output2Type = 0;
	OutputTypeParam->output3Type = 0;

	OutputTypeParam->output0BitWidth = 0;
	OutputTypeParam->output1BitWidth = 0;
	OutputTypeParam->output2BitWidth = 0;
	OutputTypeParam->output3BitWidth = 0;

	OutputTypeParam->output0Range = 0;
	OutputTypeParam->output1Range = 0;
	OutputTypeParam->output2Range = 0;
	OutputTypeParam->output3Range = 0;

	SystemParam->brightness = DEFAULT_BRIGHTNESS;
	SystemParam->cnntrast = DEFAULT_CONTRAST;
	SystemParam->colorEffect = 0;
	SystemParam->colorTemper = 5;
	SystemParam->colorTemperRed = DEFAULT_COLOR_TEMP_RED;
	SystemParam->colorTemperGreen = DEFAULT_COLOR_TEMP_GREEN;
	SystemParam->colorTemperBlue = DEFAULT_COLOR_TEMP_BLUE;
	SystemParam->gamma = DEFAULT_GAMMA;
	GetSystemParam()->Osd4pSwitch = 0;
	GetSystemParam()->PipModeSwitch = 0;
	SystemParam->SysFadeTime = 10;

	SystemParam->hotBsckup.hotBackupEn = 0;
	SystemParam->hotBsckup.hotBackupDetail[0] = 1;
	SystemParam->hotBsckup.hotBackupDetail[1] = 1;
	SystemParam->hotBsckup.hotBackupDetail[2] = 1;
	SystemParam->hotBsckup.hotBackupDetail[3] = 1;
	SystemParam->hotBsckup.hotBackupDetail[4] = 1;
	SystemParam->hotBsckup.hotBackupIndex = 0;

	SystemParam->CurrentWindow = IMAGE_B;
	SystemParam->PipLayout = LAYOUT_PIP_LT;
	SystemParam->PipModeSwitch = 0;
	SystemParam->LastSplitMode = _OSD_DM_1P;
	SystemParam->FpgaDisMode = _BEYOND_HEIGHT_WIDTH_SPLIT;

	CalcPipLayoutSize(LAYOUT_PIP_LT,stPipParam);

	memset((u8*)&userMode,0xff,sizeof(UserSettingParam_Typedef));
	for(i = 0;i < USER_SAVE_MAX_SIZE;i++)
	{
		eeprom_write(USER_SAVE_EEPROM_PORT,USER_SETTING_SAVE1_ADDRESS + i * sizeof(UserSettingParam_Typedef),(BYTE *) &userMode,sizeof(UserSettingParam_Typedef));
	}
	
}

void SaveVideoStart(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_VIDEO_START_ADDRESS,(BYTE * )&FourPortSplitPara, sizeof(FourPortSplitParamSt));
}

void LoadVideoStart(void)
{
	eeprom_read(USER_SAVE_EEPROM_PORT,USER_VIDEO_START_ADDRESS,(BYTE * )&FourPortSplitPara, sizeof(FourPortSplitParamSt));
}

void SaveCustomFormatParam(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_OUTPUT_FORMAT_ADDRESS,(BYTE * )&(GetPanleAndSplitParamRam()->stCustomFormat), sizeof(VideoDefine_Struct));
}


void SaveCurrentUserSettingParam(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_SETTING_ADDRESS,(BYTE * )(GetUserSettingParam()), sizeof(UserSettingParam_Typedef));
}

void SavePanleAndSplitParam(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_PANEL_SPLIT_ADDRESS,(BYTE * )&(GetUserSettingParam()->userSplitParam), sizeof(SPLIT_PARAM_Typedef));
	SaveVideoStart();
}


void SaveOutputTypeParam(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_OUTPUT_TYPE_ADDRESS,(BYTE * )&(GetUserSettingParam()->stOutputTypeParam), sizeof(OUTPUT_TYPE_PARAM_Typedef));
}



	
void SaveSystemParam(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_SYSTEM_PARAM_ADDRESS,(BYTE * )&(GetUserSettingParam()->stSystemParam), sizeof(SystemTypeDef));
}

void SavePipParam(void)
{
	eeprom_write(USER_SAVE_EEPROM_PORT,USER_PIP_PARAM_ADDRESS,(BYTE * )&(GetUserSettingParam()->stPipWinSize), sizeof(PipWinSizeSt));
}


void LoadPanleAndSplitParam(void)
{
	eeprom_read(USER_SAVE_EEPROM_PORT,USER_PANEL_SPLIT_ADDRESS,(BYTE * )&(GetUserSettingParam()->userSplitParam), sizeof(SPLIT_PARAM_Typedef));
	LoadVideoStart();
}

void LoadOutputTypeParam(void)
{
	eeprom_read(USER_SAVE_EEPROM_PORT,USER_OUTPUT_TYPE_ADDRESS,(BYTE * )&(GetUserSettingParam()->stOutputTypeParam), sizeof(OUTPUT_TYPE_PARAM_Typedef));
}


void LoadSystemParam(void)
{
	eeprom_read(USER_SAVE_EEPROM_PORT,USER_SYSTEM_PARAM_ADDRESS,(BYTE * )&(GetUserSettingParam()->stSystemParam), sizeof(SystemTypeDef));
}

void LoadPipParam(void)
{
	eeprom_read(USER_SAVE_EEPROM_PORT,USER_PIP_PARAM_ADDRESS,(BYTE * )&(GetUserSettingParam()->stPipWinSize), sizeof(PipWinSizeSt));
	LoadVideoStart();
}

void LoadCurrentUserSettingParam(void)
{
	eeprom_read(USER_SAVE_EEPROM_PORT,USER_SETTING_ADDRESS,(BYTE * )(GetUserSettingParam()), sizeof(UserSettingParam_Typedef));
	LoadVideoStart();
}


SPLIT_PARAM_Typedef * GetPanleAndSplitParam(void)
{
	LoadPanleAndSplitParam();
	LoadOutputTypeParam();
	LoadSystemParam();
	LoadPipParam();
	return &(GetUserSettingParam()->userSplitParam);
}


SPLIT_PARAM_Typedef * GetPanleAndSplitParamRam(void)
{
	return &(GetUserSettingParam()->userSplitParam);
}


OUTPUT_TYPE_PARAM_Typedef* GetOutputTypeParamRam(void)
{
	return &(GetUserSettingParam()->stOutputTypeParam);
}


SystemTypeDef* GetSystemParam(void)
{
	return &(GetUserSettingParam()->stSystemParam);
}


PipWinSizeSt* GetPipParam(void)
{
	return &(GetUserSettingParam()->stPipWinSize);
}


static u8 hardVersion = 0; // 0- V1.0  1- V1.1   2-V 1.2


void CheckBoardVersion(void)
{
	BYTE writeData[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};
	BYTE receiveData[16];
	
	hardVersion = 0;

	setEepromAdr(EEPROM_I2C_ADDRESS_V11);
	eeprom_write((eepromPort_TypeDef)USER_SAVE_EEPROM_PORT,(EEPROM_SIZE - 16),writeData,16);
	eeprom_read((eepromPort_TypeDef)USER_SAVE_EEPROM_PORT,(EEPROM_SIZE - 16),receiveData,16);
	if(receiveData[0] == 0x01)
	{
		hardVersion = 1;
		DebugMessageUserPref("check communication board hard version:V1.%d\r\n",hardVersion);
		return;
	}
	setEepromAdr(EEPROM_I2C_ADDRESS_V10);
	eeprom_write((eepromPort_TypeDef)USER_SAVE_EEPROM_PORT,(EEPROM_SIZE - 16),writeData,16);
	eeprom_read((eepromPort_TypeDef)USER_SAVE_EEPROM_PORT,(EEPROM_SIZE - 16),receiveData,16);
	if(receiveData[0] == 0x01)
	{
		hardVersion = 0;
		DebugMessageUserPref("check communication board hard version:V1.%d\r\n",hardVersion);
		return;
	}
	setEepromAdr(EEPROM_I2C_ADDRESS);
	DebugMessageUserPref("check communication board hard version:V1.%d\r\n",hardVersion);
}



u8 getCommHardVersion(void)
{
	return hardVersion;
}

