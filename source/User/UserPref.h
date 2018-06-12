#ifndef _USER_PREF_H_
#define _USER_PREF_H_

#include "define.h"
#include "VideoFormatTable.h"
#include "PCF8563.h"
//#include "RTD2796.h"

#define VENUS_C1   0x1c00
#define VENUS_X1   0x1f00

#define SN_PASSWORD	170726UL

#define DEFAULT_BRIGHTNESS	0X200UL
#define DEFAULT_CONTRAST	0X800UL
#define DEFAULT_GAMMA		1

#define DEFAULT_COLOR_TEMP_RED		127
#define DEFAULT_COLOR_TEMP_GREEN	142
#define DEFAULT_COLOR_TEMP_BLUE		132

//VSP 1U
#define ENABLE_USER_DEFINE 0  //Great Viwe
#define ENABLE_TTL_OUTPUT 1 
#define ENABLE_EXTERN_MATRIX_CVBS 0
#define ENTER_NUMBER_KEY  0
#define DISABLE_INTERNAL_HDCP	1 // 0 //  1
#define ENABLE_TP_SELF	0  //  1
#define ENABLE_AUDIO_TYPE_X1	1
#define ENABLE_SAVE_REAL_TIME   0// <---------�ĳ��ɱ�������                 1 //ʵʱ����������� .ֻ��SCALE,POS,CROPΪ�û�����
#define ENABLE_MENU_CLOSE_LOOP  1 //  �˵��ջ�ʹ��   1- �˵����һ�����һ���ǵ�һ��,��һ�����һ�������һ��    0- �˵��������һ����һ��֮��Ͳ��� ������
#define ENABLE_WIFI_AND_LED_CONTROL  0 // 1-��wifi��LED���Ƶ�100M��
#define DISP_TEST_INFO      1 //  1- ��ӡˢ��8028�������������
#define DEVICE_ID (0x45673600+sizeof(DeviceInfo_TypeDef))
#define DEVICE_TYPE        VENUS_X1 //   0x1f00
#define DEVICE_DEFAULT_SN 0x1234

#define HOT_BACK_NUMBERS	3		//

#define FPGA_SPLIT_PARAM_ADDR_BASD		(0x100000UL)



#pragma pack(1) //�ñ�����������ṹ��1�ֽڶ���

enum
{
	MAIN_WINDOW,
	PIP_WINDOW,
	LAYER_MAX,
};

enum
{
	COLOR_R,
	COLOR_G,
	COLOR_B,
};

typedef struct
{
	BYTE enable;  // 1  -- ����Ȩ��������, 0 -- û��Ȩ
	u32 password;
	u32 canUseTime; // ����
	u32 id;
} License_TypeDef; 

#define WORK_TIME_ID 0x20130415

typedef struct
{
	u32 startAddress;
	u32 endAddress;
	u32 indexAddress;
	u32 bootTimes;  // ��������
	u32 totalWorkTime; // �ܹ���ʱ�䣬��λ����
	u32 lastWorkTime;  // ��һ�ι���ʱ��,��λ���� //added by cyj 
	u32 lastTotalTime; // ��һ���ܹ���ʱ��,��λ���� //added by cyj 
	u32 workTimeListId;	
} WorkTimeList_Typedef; // 20 bytes


typedef enum
{
	USER_SAVE_1,
	USER_SAVE_2,
	USER_SAVE_3,
	USER_SAVE_4,
	USER_SAVE_5,
	USER_SAVE_6,
	USER_SAVE_7,
	USER_SAVE_8,
	USER_SAVE_9,
	USER_SAVE_10,
	USER_SAVE_MAX_SIZE = 10,	//10,		// 36
} UserSaveIndex_TypeDef;

typedef struct
{
	u32 sn;
	u32 deviceType; 			//�豸���� ---VSP1U
	BYTE logoDisMode;			/* bit7=1(��˾��Ϣ), bit7=0(�û�������Ϣ) */
					 			/* [6:4](��һ�е���ʾλ��)*/
								/* [2:0](�ڶ��е���ʾλ��)*/
	BYTE disableDisplayLogo; 	// 0 -- ��ʾlogo , 1 -- ����ʾ
	char keyLock;
	char companyLogoStr[21];
	char deviceTypeStr[21];
	char deviceLogoStr[21];
	char deviceTypeStr2[12];	
	
	//BYTE techSupportDispMode; //bit7 = 1(��˾����֧����Ϣ),bit7 = 0(�û����Ƽ���֧����Ϣ) //added by cyj 
	char saleNumStr[21]; 		//�ͻ������������ߺ���
	char customNumStr[21];		//�ͻ������ۺ����ߺ���
	char emailStr[33]; 			//�ͻ����������ַ
	char websiteStr[33]; 		//�ͻ�������ַ
	u32  extType;   			//������������� b0-b7 ext7 type      b8-b15 ext8 type       b16-b23  ext9 type
	u32  DeviceModel;			//�豸�ͺ�    0x0198----VSP 198              0x0516-----VSP 516S              0x0112-------VSP112U
	u32  deviceId;
} DeviceInfo_TypeDef;

#define USER_PREF_ID 0x20151218
#define USER_PREF_ID_END 0x20150604
#define USER_DEFAULT_LANG 0x00  // 0--English, 1--Chinese

typedef struct 
{
	int artNetStartAddr;
	int artNetUniverse;
	U8  DHCP;				/* DHCP on/off */
	U8  RemoteIpAdr[4];		// remote ip
	U8  IpAdr[4];           /* Local IP Address                        */
  	U8  DefGW[4];           /* Default GateWay                         */
  	U8  NetMask[4];         /* Net Mask                                */
  	U8  PriDNS[4];          /* Primary DNS Server                      */
  	U8  SecDNS[4];          /* Secondary DNS Server                    */
	U8  HwAdr[6];           /* Hardware Address                 */
} NetWorkSetting;

typedef struct
{
	BYTE enableDE;
	BYTE enableHDMI;
	BYTE enableDataRange; // 16-235 To 0-255
	#if 0
	BYTE HsVsPolarity;    // bit 0 -- hs,  bit 1 -- vs,  0 -- positive, 1 -- negative
	#else
	BYTE HPolarity;		//0 -- positive, 1 -- negative
	BYTE VPolarity;		//0 -- positive, 1 -- negative
	#endif
	WORD HStart;
	WORD VStart;
	WORD HActive;
	WORD VActive;
	BYTE colorSpace;
	BYTE bitWidth; 		// 0 -- 8bit, 1 -- 10bit, 2 -- 12bit
} UserDE_Struct;


typedef struct
{
	BYTE hotBackupEn;
	BYTE hotBackupIndex;
	BYTE hotBackupDetail[5];  //0xff: no backup signal
	
}HotBackup_Typedef;

typedef struct
{
	char autoDuty;
	char fanDutyRatio;
}FanControl_Typedef;

typedef struct
{
	BYTE AudioInputDetail[3];  
}AudioInput_Typedef;

typedef struct
{
    u32 padLockId;
	BYTE PadLockEnable; 			// pad lock enable/disable,
	BYTE PadLockState; 				// pad lock state,
	u32 lockPanelPassword;
	char keyLock;	
}PadLock_Typedef;


#define USER_PADLOCK_ID 0x20160918


typedef struct
{
	u32 userPrefId;
	BYTE userLang;					// 0--English, 1--Chinese
	BYTE noUser1;
	BYTE noUser2;
	BYTE LoadFadeInoutOff; 			// 20130306 �������Ƿ��뵭�� 
	NetWorkSetting netWorkSetting;
	BYTE delayCallTime; 			// ��
	BYTE delayCallIndex; 			// ���� 0~9 ����
	//BYTE PadLockEnable; 			// pad lock enable/disable,
	//BYTE PadLockState; 			// pad lock state,
	UserDE_Struct deSetting[4];
	BYTE edidFollowEn[5]; 			//  0 -- disable edid follow  1--enable edid follow,if set output format,edit the edid of HDMI port.   2--enable custom edid
	BYTE enableNetwork;
	BYTE OLEDLum;       			//OLED ����
	BYTE temperatureMode;			// �¶���ʾģʽ
	FanControl_Typedef fanControl;
	//HotBackup_Typedef hotBsckup;
	AudioInput_Typedef audioInput; 	//ֻ���V1.3�������弰����
	BYTE realTimeSaveEn; 			//0- default save  1-save param real tine

    u32 SetWidthH;
    u32 SetWidthV;
    BYTE setWidthEnable;
    BYTE setWidthChoose;

	BYTE output1Config; 			// 0- output  1- dvi loop        //just for mainboard >= V1.3
	u32 userPrefId_end;
} UserPref_TypeDef;

typedef struct
{
	short posX;
	short posY;
	WORD sizeX;
	WORD sizeY;
} PosSizeSetting;

typedef struct
{
	float posX;
	float posY;
	float sizeX;
	float sizeY;
} CropRatioSetting;


// �ṹ����������˳��������ı�
typedef struct
{
	BYTE format;
	BYTE hvSyncPolarity;  		// bit0 -- horizontal sync  bit1 -- vertical sync,  0 -- ��, 1 -- ��
	WORD hTotal;
	WORD vTotal;
	WORD hStart;
	WORD vStart;
	WORD hActive;
	WORD vActive;
	WORD hSyncWidth;
	WORD vSyncWidth;
	BYTE colorSpace;     		// 0 -- RGB ,  1 -- YUV 4:4:4,  2 -- YUV 4:2:2, 3 -- BT656.
	BYTE YUVdelay;
	
}VideoFormatSetting;

typedef struct
{
	BYTE inputPort;				// 0 ~ 3  port
	BYTE freeze;        		// 0 -- off, 1 -- on
	BYTE horizontalFlip; 		// 0 -- off, 1 -- on
	BYTE rotate; 				// 0 -- off, 1 -- left rotation, 2 -- right rotation.
	PosSizeSetting crop;		//�������ü�
	PosSizeSetting cropDot; 	//���ü�,��ʾ���,�߶ȵȲü����ĵ���
	PosSizeSetting scale;	
	PosSizeSetting zoom;     
	PosSizeSetting cropParam;	//0 up  1 down  2 left  3 riight
	PosSizeSetting cropPre;		//Ԥ�ü�,�����EXT7 ��EXT8������ʵ������;  ���� = ����ֱ���
}LayerSetting;

typedef struct
{
	PosSizeSetting scale;	
	PosSizeSetting cropDot; 	//���ü�,��ʾ���,�߶ȵȲü����ĵ���
	PosSizeSetting cropParam;	//0 up  1 down  2 left  3 riight
	PosSizeSetting cropPre;		//Ԥ�ü�,�����EXT7 ��EXT8������ʵ������;  ���� = ����ֱ���
}SourceSeting;

typedef struct
{
	BYTE onOff;
	BYTE mode; 					// 0 -- key out, 1 -- key in
	BYTE Red;
	BYTE Green;
	BYTE Blue;
}ColorKeySetting;

typedef struct
{
	//OutLayerSetting outLayerSetting;
	VideoFormatSetting format;
	int alphaTimeMs;
	ColorKeySetting colorKey;
}OutputSetting;


// 20121029 add
typedef struct
{
	BYTE Brightness;
	BYTE Saturation;
	BYTE Contrast;
}PictureSetting;



typedef struct
{
	float X,Y,Width,Height;
}_CropRatio;



typedef struct
{
	BYTE format;
	BYTE hvSyncPolarity;  // bit0 -- horizontal sync  bit1 -- vertical sync,  0 -- ��, 1 -- ��
	WORD hTotal;
	WORD vTotal;
	WORD hStart;
	WORD vStart;
	WORD hActive;
	WORD vActive;
	WORD hSyncWidth;
	WORD vSyncWidth;
	BYTE colorSpace;     // 0 -- RGB ,  1 -- YUV 4:4:4,  2 -- YUV 4:2:2, 3 -- BT656.
} Timing_TypeDef;



typedef struct
{
	WORD hStart;
	WORD vStart;
	BYTE YUVdelay;
	BYTE id;
} SDI_InputAdjust_Typedef;

typedef struct
{
	BYTE TimingId;
	Timing_TypeDef TimingInput[8];

}UserTiming_TypeDef;


typedef struct
{
	int HorizontalPixel;
	int VerticalPixel;
	int Frequency;
}CUSTOM_RESOLUTION;



#define USER_SETTING_ID (0x19850423+sizeof(UserSettingParam_Typedef))
#define END_ID 0x861210
#define USER_SETTING_NOT_USE    0x55
#define USER_SETTING_USED       0xaa

typedef struct {
	u32 timeControlId;
	BYTE timeControlValid;
	BYTE enableTimeControl;
	BYTE timeControlBlock[128];
} TimeControl_TypeDef;


enum eoutput_prglayer
{
	eoutput_prglayer_end=0,
	eoutput_prglayer_init,
	eoutput_prglayer_LayOut,
	eoutput_prglayer_Split,
};

enum elayer_prgItem
{
	elayer_prgItem_end=0,
	elayer_prgItem_init,
	elayer_prgItem_Program,
	elayer_prgItem_AddLayer,
	elayer_prgItem_ClrLayer,	
	elayer_prgItem_Zoom,	
	elayer_prgItem_Crop,
	elayer_prgItem_Pos,
	elayer_prgItem_TP,	
	elayer_prgItem_Save,	
	elayer_prgItem_Load,
	elayer_prgItem_Dsk,	
	elayer_prgItem_Menu,	

	elayer_prgItem_Out,	
	elayer_prgItem_split, 
	
};

enum ectrl_SplitMode
{
	SPLIT_MODE_H_2 = 0,
	SPLIT_MODE_V_2,
	SPLIT_MODE_HV_4,
	SPLIT_MODE_H_4,
	SPLIT_MODE_V_4,
	SPLIT_MODE_2IN3OUT,
	SPLIT_MODE_2IN4OUT,
	SPLIT_MODE_FULL,
	SPLIT_MODE_MAX
};

enum elayer_prgMenu
{
	elayer_prgMenu_end=0,
	elayer_prgMenu_init,
	elayer_prgMenu_prg,
};

enum elayer_adjust_layer
{
	elayer_ajust1layer =0,
	elayer_ajust4layers,
};

typedef struct {
	u8 prgItem;
	u8 prglayer;
	u8 prginputstatus;
	u8 curoutput;	
	u8 curlayer;
	u8 prgsource;
	u8 FastAdjustLayer; // 0: sclae 4 layers, 1: scale current layer
}TempData_TypeDef;

#define TIME_CONTROL_ID (0x20150822+sizeof(TimeControl_TypeDef))

//#define USER_SAVE_EEPROM_PORT I2CEEPROM_PORTA//I2CEEPROM_PORTI//
#define USER_SAVE_EEPROM_PORT I2C_E


typedef struct {
	u32 timeControlId;
	BYTE enableTimeControl;
} TimeControlOld_TypeDef;

#define TIME_CONTROL_ITEM_SIZE (0x20)
#define TIME_CONTROL_ITEM_NUM (10)
#define TIME_CONTROL_ITEM_BLOCK_LEN  (128)

#define TIME_CONTROL_DATA_SIZE (TIME_CONTROL_ITEM_SIZE*TIME_CONTROL_ITEM_NUM)

//added by @cc20140610
//#define USB_FILESLIST_ITEM_SIZE (0x19)	//ÿ���ļ���ռ25bytes
//#define USB_FILESLIST_ITEM_NUM	(100)	//֧��100���ļ�
//#define USB_PLSYLIST_BLOCK_NUM	(300)	//USB Playlist ʱ����ƿ���Ŀ
#define USB_PLSYLIST_BLOCK_NUM	(1)	//USB Playlist ʱ����ƿ���Ŀ----vip smart


//#define USB_FILESLIST_DATA_SIZE (USB_FILESLIST_ITEM_SIZE*USB_FILESLIST_ITEM_NUM)



//########################################################################################
//������ֱPanel����
typedef struct
{
	U16 vPaneStart0Posy;		//��ֱ������ʼλ��
	U16 vPaneStart1Posy;		//��ֱ������ʼλ��
	U16 vPaneEnd0Posy;		//��ֱ�����е�λ��
	U16 vPaneEnd1Posy;		//��ֱ�������λ��
}vPaneParamSt;


//########################################################################################
//�����Xͼ�����
//����ͼ����ƴ�� layer_A,layer_B
typedef struct 
{
	U16 hLayerStartPos;		//ˮƽ������ʼλ��
	U16 vLayerStartPos;		//��ֱ������ʼλ��
	U16 hLayerWidth;		//ˮƽ������
	U16 vLayerHeight;		//��ֱ����߶�
	//U8  layerAlpha;			//ͼ��alphaֵ
}OutputPortLayerParamSt;

//########################################################################################
//�����Xͼ�����
//����ͼ����ƴ�� layer_A,layer_B
typedef struct 
{
	U8  layerA_Alpha;			//ͼ��alphaֵ
	U8  layerB_Alpha;			//ͼ��alphaֵ
}OutputPorlayerAlphaSt;


//########################################################################################
//�����Xƴ�Ӳ���
typedef struct 
{
	U16 hSplitActiveStartPos;	//ˮƽƴ����Ч��ʼλ��
	U16 hSplitActiveEndPos;		//ˮƽƴ����Ч����λ��
	U16 hSplitCropStartPos;		//ˮƽƴ�Ӳü���ʼλ��
	U16 hSplitCropEndPos;		//ˮƽƴ�Ӳü�����λ��
} OutputPortSplitParamSt;


typedef struct 
{
	u16 gPipMainWinPosX;
	u16 gPipMainWinPosY;
	u16 gPipMainWinSizeX;
	u16 gPipMainWinSizeY;
	
	u16 gPipSubWinPosX;
	u16 gPipSubWinPosY;
	u16 gPipSubWinSizeX;
	u16 gPipSubWinSizeY;

	u16 gPbpTopWinPosX;
	u16 gPbpTopWinPosY;
	u16 gPbpTopWinSizeX;
	u16 gPbpTopWinSizeY;

	u16 gPbpBottomWinPosX;
	u16 gPbpBottomWinPosY;
	u16 gPbpBottomWinSizeX;
	u16 gPbpBottomWinSizeY;

	u16 gPbpLeftWinPosX;
	u16 gPbpLeftWinPosY;
	u16 gPbpLeftWinSizeX;
	u16 gPbpLeftWinSizeY;

	u16 gPbpRightWinPosX;
	u16 gPbpRightWinPosY;
	u16 gPbpRightWinSizeX;
	u16 gPbpRightWinSizeY;

	u8 gPipParamCheckSum;
}PipWinSizeSt,*pPipWinSizeSt;



//########################################################################################
//����ƴ�Ӳ���
typedef struct 
{
	vPaneParamSt vPanelInfo;

	OutputPortLayerParamSt  OutpuPort0tLayerA;		//�����Aͼ�����
	OutputPortLayerParamSt  OutputPort0LayerB;		//�����Bͼ�����
	OutputPorlayerAlphaSt	OutputPort0LayerAlpha;
	OutputPortSplitParamSt  OutputPort0SplitInfo;

	OutputPortLayerParamSt  OutpuPort1tLayerA;		//�����Aͼ�����
	OutputPortLayerParamSt  OutputPort1LayerB;		//�����Bͼ�����
	OutputPorlayerAlphaSt	OutputPort1LayerAlpha;
	OutputPortSplitParamSt  OutputPort1SplitInfo;


	OutputPortLayerParamSt  OutpuPort2tLayerA;		//�����Aͼ�����
	OutputPortLayerParamSt  OutputPort2LayerB;		//�����Bͼ�����
	OutputPorlayerAlphaSt	OutputPort2LayerAlpha;
	OutputPortSplitParamSt  OutputPort2SplitInfo;


	OutputPortLayerParamSt  OutpuPort3tLayerA;		//�����Aͼ�����
	OutputPortLayerParamSt  OutputPort3LayerB;		//�����Bͼ�����
	OutputPorlayerAlphaSt	OutputPort3LayerAlpha;
	OutputPortSplitParamSt  OutputPort3SplitInfo;

}PanelSplitParamSt;

typedef struct
{
	//u32 sendcard_id;
	u16 fieldSplit_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 fieldSplit_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 fieldSplit_FirstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 fieldSplit_FirstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 fieldSplit_SecondDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 fieldSplit_SecondDisplayHeight;		//�ڶ������ܸ�(ƴ��)
	u16 fieldSplit_ThirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 fieldSplit_ThirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 fieldSplit_FourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 fieldSplit_FourthDisplayHeight;		//���Ŀ����ܸ�(ƴ��)

	u16 _PIP_MainWinDisplayWidth;	//�û���ʾ�����ܿ�
	u16 _PIP_MainWinDisplayHeight;	//�û���ʾ�����ܸ�
	u16 _PIP_SubWinDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 _PIP_SubWinDisplayHeight;		//��һ�����ܸ�(ƴ��)
	//u16 fieldSplit_SecondDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	//u16 fieldSplit_SecondDisplayHeight;		//�ڶ������ܸ�(ƴ��)
	//u16 fieldSplit_ThirdDisplayWidth;		//���������ܿ�(ƴ��)
	//u16 fieldSplit_ThirdDisplayHeight;		//���������ܸ�(ƴ��)
	//u16 fieldSplit_FourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	//u16 fieldSplit_FourthDisplayHeight;		//���Ŀ����ܸ�(ƴ��)

	u16 _4P_UserTotalDisplayWidth;			//�û���ʾ�����ܿ�
	u16 _4P_UserTotalDisplayHeight;			//�û���ʾ�����ܸ�
	u16 _4P_FirstDisplayWidth;				//��һ�����ܿ�(ƴ��)
	u16 _4P_FirstDisplayHeight;				//��һ�����ܸ�(ƴ��)
	u16 _4P_SecongDisplayWidth;				//�ڶ������ܿ�(ƴ��)
	u16 _4P_SecongDisplayHeight;			//�ڶ������ܸ�(ƴ��)
	u16 _4P_ThirdDisplayWidth;				//���������ܿ�(ƴ��)
	u16 _4P_ThirdDisplayHeight;				//���������ܸ�(ƴ��)
	u16 _4P_FourthDisplayWidth;				//���Ŀ����ܿ�(ƴ��)
	u16 _4P_FourthDisplayHeight;			//���Ŀ����ܸ�(ƴ��)

	u16 leftRight12_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 leftRight12_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 leftRight12_FirstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 leftRight12_FirstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 leftRight12_SecongDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 leftRight12_SecongDisplayHeight;	//�ڶ������ܸ�(ƴ��)
	u16 leftRight12_ThirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 leftRight12_ThirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 leftRight12_FourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 leftRight12_FourthDisplayHeight;	//���Ŀ����ܸ�(ƴ��)

	u16 topBotton12_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 topBotton12_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 topBotton12_firstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 topBotton12_firstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 topBotton12_secongDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 topBotton12_secongDisplayHeight;	//�ڶ������ܸ�(ƴ��)
	u16 topBotton12_thirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 topBotton12_thirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 topBotton12_fourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 topBotton12_fourthDisplayHeight;	//���Ŀ����ܸ�(ƴ��)

	u16 FourPort12_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 FourPort12_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 FourPort12_FirstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 FourPort12_FirstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 FourPort12_SecongDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 FourPort12_SecongDisplayHeight;		//�ڶ������ܸ�(ƴ��)
	u16 FourPort12_ThirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 FourPort12_ThirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 FourPort12_FourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 FourPort12_FourthDisplayHeight;		//���Ŀ����ܸ�(ƴ��)

	u16 FourPortVer12_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 FourPortVer12_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 FourPortVer12_FirstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 FourPortVer12_FirstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 FourPortVer12_SecongDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 FourPortVer12_SecongDisplayHeight;		//�ڶ������ܸ�(ƴ��)
	u16 FourPortVer12_ThirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 FourPortVer12_ThirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 FourPortVer12_FourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 FourPortVer12_FourthDisplayHeight;		//���Ŀ����ܸ�(ƴ��)
	
	u16 leftRight13_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 leftRight13_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 leftRight13_FirstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 leftRight13_FirstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 leftRight13_SecongDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 leftRight13_SecongDisplayHeight;	//�ڶ������ܸ�(ƴ��)
	u16 leftRight13_ThirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 leftRight13_ThirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 leftRight13_FourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 leftRight13_FourthDisplayHeight;	//���Ŀ����ܸ�(ƴ��)

	u16 leftRight14_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 leftRight14_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 leftRight14_FirstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 leftRight14_FirstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 leftRight14_SecongDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 leftRight14_SecongDisplayHeight;	//�ڶ������ܸ�(ƴ��)
	u16 leftRight14_ThirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 leftRight14_ThirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 leftRight14_FourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 leftRight14_FourthDisplayHeight;	//���Ŀ����ܸ�(ƴ��)

	u16 topBotton14_UserTotalDisplayWidth;	//�û���ʾ�����ܿ�
	u16 topBotton14_UserTotalDisplayHeight;	//�û���ʾ�����ܸ�
	u16 topBotton14_firstDisplayWidth;		//��һ�����ܿ�(ƴ��)
	u16 topBotton14_firstDisplayHeight;		//��һ�����ܸ�(ƴ��)
	u16 topBotton14_secongDisplayWidth;		//�ڶ������ܿ�(ƴ��)
	u16 topBotton14_secongDisplayHeight;	//�ڶ������ܸ�(ƴ��)
	u16 topBotton14_thirdDisplayWidth;		//���������ܿ�(ƴ��)
	u16 topBotton14_thirdDisplayHeight;		//���������ܸ�(ƴ��)
	u16 topBotton14_fourthDisplayWidth;		//���Ŀ����ܿ�(ƴ��)
	u16 topBotton14_fourthDisplayHeight;	//���Ŀ����ܸ�(ƴ��)
	
	BYTE alpha;
	
	BYTE dvi1LayerAlpha_A;
	BYTE dvi1LayerAlpha_B;
	BYTE dvi2LayerAlpha_A;
	BYTE dvi2LayerAlpha_B;
	BYTE dvi3LayerAlpha_A;
	BYTE dvi3LayerAlpha_B;
	BYTE dvi4LayerAlpha_A;
	BYTE dvi4LayerAlpha_B;
	
	BYTE outputFormatIndex;

	BYTE splitMode;
	
	BYTE inputSource1;
	BYTE inputSource2;
	BYTE inputSource3;
	BYTE inputSource4;

	BYTE outputPort1Map;
	BYTE outputPort2Map;
	BYTE outputPort3Map;
	BYTE outputPort4Map;

	VideoDefine_Struct stCustomFormat;
	
}SPLIT_PARAM_Typedef,*pSPLIT_PARAM_Typedef;

typedef struct 
{
	u16 layerAWidth;
	u16 layerAHeight;
	u16 layerAXstart;
	u16 layerAVstart;
	u16 layerASplitStart;
	u16 layerASplitWidth;
	u16 layerACropStart;

	u16 layerBWidth;
	u16 layerBHeight;
	u16 layerBXstart;
	u16 layerBVstart;
	u16 layerBSplitStart;
	u16 layerBSplitWidth;
	u16 layerBCropStart;

	u16 layerCWidth;
	u16 layerCHeight;
	u16 layerCXstart;
	u16 layerCVstart;
	u16 layerCSplitStart;
	u16 layerCSplitWidth;
	u16 layerCCropStart;

	u16 layerDWidth;
	u16 layerDHeight;
	u16 layerDXstart;
	u16 layerDVstart;
	u16 layerDSplitStart;
	u16 layerDSplitWidth;
	u16 layerDCropStart;


	
}LAYER_PARAM_Typedef;


typedef struct
{
	u8 output0Type;
	u8 output0BitWidth;
	u8 output0Range;

	u8 output1Type;
	u8 output1BitWidth;
	u8 output1Range;

	u8 output2Type;
	u8 output2BitWidth;
	u8 output2Range;

	u8 output3Type;
	u8 output3BitWidth;
	u8 output3Range;
	
}OUTPUT_TYPE_PARAM_Typedef;


typedef struct
{
	u32 	SystemPrefId;
	BYTE 	SystemLang;					// 0--English, 1--Chinese
	BYTE    Osd4pSwitch;
	BYTE    PipModeSwitch;
	BYTE    PipLayout;
	BYTE 	LastSplitMode;
	BYTE 	FpgaDisMode;
	BYTE 	CurrentWindow;
	BYTE 	OLEDLum;       				//OLED ����
	BYTE    SysFadeTime;
	BYTE 	gamma;
	WORD 	brightness;
	WORD 	cnntrast;
	BYTE 	colorEffect;
	BYTE 	colorTemper;
	BYTE 	colorTemperRed;
	BYTE 	colorTemperGreen;
	BYTE 	colorTemperBlue;
	
	HotBackup_Typedef hotBsckup;
	
	u32 	SystemPrefId_end;
	
} SystemTypeDef;

typedef enum
{
    _DISPLAY_PATH_NONE = 0,
    _DISPLAY_PATH_M1 = 0x01,
    _DISPLAY_PATH_M2 = 0x02,
    _DISPLAY_PATH_M1_M2 = 0x03,
    _DISPLAY_PATH_S1 = 0x04,
    _DISPLAY_PATH_S2 = 0x08,
    _DISPLAY_PATH_S1_S2 = 0x0C,
    _DISPLAY_PATH_ALL = 0x0F,
} EnumDisplayDataPath;

// Struct for Display Timing Info
typedef struct
{
    EnumDisplayDataPath enumDisplayDataPath; // Display data path
    WORD usHTotal;                // Horizontal Total length (unit: Pixel)
    WORD usHStart;                // Horizontal Start Position
    WORD usHWidth;                // Horizontal Active Width (unit: Pixel)
    WORD usHBackStart;            // Horizontal Back Ground Start Position
    WORD usHBackWidth;            // Horizontal Back Ground Active Width (unit: Pixel)

    WORD usVTotal;                // Vertical Total length (unit: HSync)
    WORD usVStart;                // Vertical Start Position
    WORD usVHeight;               // Vertical Active Height (unit: HSync)
    WORD usVBackStart;            // Vertical Back Ground Start Position
    WORD usVBackHeight;           // Vertical Back Ground Active Height (unit: HSync)
}StructDisplayInfo;


typedef struct 
{
	u16 stTotalWidth;
	u16 stTotalHeight;
	u16 stFirstDisplayWidth;
	u16 stFirstDisplayHeight;
	u16 stSecondDisplayWidth;
	u16 stSecondDisplayHeight;
	u16 stThirdDisplayWidth;
	u16 stThirdDisplayHeight;
	u16 stFourthDisplayWidth;
	u16 stFourthDisplayHeight;

	u16 stFirstRegionWidthStart;
	u16 stFirstRegionHeightStart;
	u16 stSecondRegionWidthStart;
	u16 stSecondRegionHeightStart;

	u16 stThirdRegionWidthStart;
	u16 stThirdRegionHeightStart;
	u16 stFourthRegionWidthStart;
	u16 stFourthRegionHeightStart;

	u8 splitMode;
	u8 splitParamCheckSum;
}FourPortSplitParamSt;

//typedef struct
//{
//	BYTE hotBackupEn;
//	BYTE hotBackupDetail[5];  //0xff: no backup signal
//}HotBackup_Typedef;

typedef struct
{
	u32 userSettingId;
	BYTE userSettingStatus; //0x55: not use 0xaa:used
	SPLIT_PARAM_Typedef userSplitParam;
	OUTPUT_TYPE_PARAM_Typedef stOutputTypeParam;
	SystemTypeDef stSystemParam;
	PipWinSizeSt stPipWinSize;
	u32 endId;
}UserSettingParam_Typedef;

#pragma pack() //ȡ��1�ֽڶ��룬�ָ�ΪĬ��4�ֽڶ���


extern UserSettingParam_Typedef _userSettingParam;
extern UserSettingParam_Typedef userMode;
//extern PipWinSizeSt stPipWinSize;
extern PipWinSizeSt stPipWinSizeBackUp;
extern PanelSplitParamSt _panelSplitInfo;
extern PipWinSizeSt stPipWin;



#define GET_PBP_LEFT_POS_X()		(GetPipParam()->gPbpLeftWinPosX)
#define GET_PBP_LEFT_POS_Y()		(GetPipParam()->gPbpLeftWinPosY)
#define GET_PBP_LEFT_SIZE_X()		(GetPipParam()->gPbpLeftWinSizeX)
#define GET_PBP_LEFT_SIZE_Y()		(GetPipParam()->gPbpLeftWinSizeY)

#define GET_PBP_RIGHT_POS_X()		(GetPipParam()->gPbpRightWinPosX)
#define GET_PBP_RIGHT_POS_Y()		(GetPipParam()->gPbpRightWinPosY)
#define GET_PBP_RIGHT_SIZE_X()		(GetPipParam()->gPbpRightWinSizeX)
#define GET_PBP_RIGHT_SIZE_Y()		(GetPipParam()->gPbpRightWinSizeY)

#define GET_PBP_TOP_POS_X()			(GetPipParam()->gPbpTopWinPosX)
#define GET_PBP_TOP_POS_Y()			(GetPipParam()->gPbpTopWinPosY)
#define GET_PBP_TOP_SIZE_X()		(GetPipParam()->gPbpTopWinSizeX)
#define GET_PBP_TOP_SIZE_Y()		(GetPipParam()->gPbpTopWinSizeY)

#define GET_PBP_BOTTOM_POS_X()		(GetPipParam()->gPbpBottomWinPosX)
#define GET_PBP_BOTTOM_POS_Y()		(GetPipParam()->gPbpBottomWinPosY)
#define GET_PBP_BOTTOM_SIZE_X()		(GetPipParam()->gPbpBottomWinSizeX)
#define GET_PBP_BOTTOM_SIZE_Y()		(GetPipParam()->gPbpBottomWinSizeY)

#define SET_PBP_LEFT_POS_X(X)		(GetPipParam()->gPbpLeftWinPosX = X)
#define SET_PBP_LEFT_POS_Y(X)		(GetPipParam()->gPbpLeftWinPosY = X)
#define SET_PBP_LEFT_SIZE_X(X)		(GetPipParam()->gPbpLeftWinSizeX = X)
#define SET_PBP_LEFT_SIZE_Y(X)		(GetPipParam()->gPbpLeftWinSizeY = X)

#define SET_PBP_RIGHT_POS_X(X)		(GetPipParam()->gPbpRightWinPosX = X)
#define SET_PBP_RIGHT_POS_Y(X)		(GetPipParam()->gPbpRightWinPosY = X)
#define SET_PBP_RIGHT_SIZE_X(X)		(GetPipParam()->gPbpRightWinSizeX = X)
#define SET_PBP_RIGHT_SIZE_Y(X)		(GetPipParam()->gPbpRightWinSizeY = X)

#define SET_PBP_TOP_POS_X(X)		(GetPipParam()->gPbpTopWinPosX = X)
#define SET_PBP_TOP_POS_Y(X)		(GetPipParam()->gPbpTopWinPosY = X)
#define SET_PBP_TOP_SIZE_X(X)		(GetPipParam()->gPbpTopWinSizeX = X)
#define SET_PBP_TOP_SIZE_Y(X)		(GetPipParam()->gPbpTopWinSizeY = X)

#define SET_PBP_BOTTOM_POS_X(X)		(GetPipParam()->gPbpBottomWinPosX = X)
#define SET_PBP_BOTTOM_POS_Y(X)		(GetPipParam()->gPbpBottomWinPosY = X)
#define SET_PBP_BOTTOM_SIZE_X(X)	(GetPipParam()->gPbpBottomWinSizeX = X)
#define SET_PBP_BOTTOM_SIZE_Y(X)	(GetPipParam()->gPbpBottomWinSizeY = X)



typedef enum
{
	DEVICE_INFO_ADDRESS = 0x00,
	PAD_LOCK_ADDRESS = (DEVICE_INFO_ADDRESS + sizeof(DeviceInfo_TypeDef)),

	USER_VIDEO_START_ADDRESS = (PAD_LOCK_ADDRESS + sizeof(PadLock_Typedef)),
	
	USER_PREF_ADDRESS	= (USER_VIDEO_START_ADDRESS + sizeof(FourPortSplitParamSt)),
	
	USER_SETTING_ADDRESS = (USER_PREF_ADDRESS + sizeof(UserPref_TypeDef)),
	
	USER_PANEL_SPLIT_ADDRESS = (USER_SETTING_ADDRESS + sizeof(UserSettingParam_Typedef)\
								- sizeof(OUTPUT_TYPE_PARAM_Typedef) - sizeof(SPLIT_PARAM_Typedef)\
								- sizeof(SystemTypeDef)\
								- sizeof(PipWinSizeSt)
								- sizeof(u32)),
								
	USER_OUTPUT_FORMAT_ADDRESS	 = 	(USER_PANEL_SPLIT_ADDRESS - sizeof(VideoDefine_Struct)),
	
	USER_OUTPUT_TYPE_ADDRESS = (USER_PANEL_SPLIT_ADDRESS + sizeof(SPLIT_PARAM_Typedef)),
	
	USER_SYSTEM_PARAM_ADDRESS = (USER_OUTPUT_TYPE_ADDRESS + sizeof(OUTPUT_TYPE_PARAM_Typedef)),

	USER_PIP_PARAM_ADDRESS = (USER_SYSTEM_PARAM_ADDRESS + sizeof(SystemTypeDef)),

	USER_SETTING_SAVE1_ADDRESS = (USER_PIP_PARAM_ADDRESS + sizeof(PipWinSizeSt) + 4 + (sizeof(UserSettingParam_Typedef) % 2)),
	
	USER_SETTING_SAVE_FLAG_ADDRESS = (USER_SETTING_SAVE1_ADDRESS + USER_SAVE_MAX_SIZE*sizeof(UserSettingParam_Typedef)),

} UserAddress_TypeDef;




char get_save_flag(int value);

u8 GetSendcardChoose(void);
u8 GetSendcardWidthChoose(u8 port);
u8 getCommHardVersion(void);
BYTE getUserLang(void);
BYTE getUserPadLockState(void);
BYTE getUserPadLockEnable(void);
BYTE getDelayCallTime(void);
BYTE getUserEDIDMode(BYTE inputPort);
BYTE getDelayCallIndex(void);
BYTE getIsFirstPowerAfterReset(void);
BYTE UserPerf_Application(void);
BYTE getLoadFadeInOutOff(void);
BYTE getUserOLEDLum(void);
BYTE isUserSettingUsed_ram(UserSaveIndex_TypeDef saveIndex);
BYTE * getIPAddress(void);

u16 GetOutputWidth(void);
u16 GetOutputHeight(void);

int getSizeEEPROM(void);
int GetOutHActive(void);
int GetOutVActive(void);
int getUserSettingStatus(UserSaveIndex_TypeDef saveId);
int getAllUserSettingStatus(void);
int GetLayerInputPort(int layer);
int getOldTimeControlEnable(void);

u32 getDeviceSN(void);
u32 getDeviceModel(void);


CUSTOM_RESOLUTION *GetCustomResolution(void);
CUSTOM_RESOLUTION *GetCustomEDID(void);
UserSaveIndex_TypeDef getCurrentRecordIndex(void);
UserSettingParam_Typedef *GetUserSettingParam(void);
UserTiming_TypeDef *GetUserTiming(void);
TempData_TypeDef *getTempData(void);
WorkTimeList_Typedef *getWorkTimeList(void);
UserPref_TypeDef * GetUserPref(void);
License_TypeDef * getLicense(void);
UserSettingParam_Typedef *GetUserSettingSave(void);
TimeControl_TypeDef *GetTimeControl(void);
SPLIT_PARAM_Typedef * GetPanleAndSplitParam(void);
SPLIT_PARAM_Typedef * GetPanleAndSplitParamRam(void);
OUTPUT_TYPE_PARAM_Typedef* GetOutputTypeParamRam(void);
SystemTypeDef* GetSystemParam(void);
DeviceInfo_TypeDef * getDeviceInfo(void);
PadLock_Typedef * getPadLock(void);
PipWinSizeSt* GetPipParam(void);
PanelSplitParamSt* LoadFpgaSplitFromFlash(UserSaveIndex_TypeDef saveIndex);

void resetWorkTimeList(void);
void saveWorkTimeList(void);
void loadWorkTimeList(void);
void clearWorkTimeList(void);
void savePowerOnTime(Time_TypeDef *value);
void savePowerOffTime(Time_TypeDef *value);
void getWorkTimeData(BYTE *buffer, int address, int size);
void Fac_UserSettingSaveTo(UserSaveIndex_TypeDef saveIndex);
void setCurrentRecordIndex(int value);
void loadUserSetting(UserSaveIndex_TypeDef saveIndex);
void saveUserSeetingToSave1AndNoVideo(void);
void UserSettingSaveTo(UserSaveIndex_TypeDef saveIndex);
void SaveUserSetting(UserSaveIndex_TypeDef saveIndex);
void ResetUserSettingSave(void);
void set_save_flag(int value,int value1);
void saveLoadFadeInOutOff(BYTE value);
//void saveOLEDLuminance(BYTE value);
void loadLicense(void);
void saveLicense(void);
void SaveUserPref(void);
void ResetScreenSettingSave(void);
void ResetScaleSettingSave(void);
void ResetLayerSettingSave(void);
//void getUserMode(UserSaveIndex_TypeDef saveIndex);
bool getUserMode(UserSaveIndex_TypeDef saveIndex);
void setUserMode(UserSaveIndex_TypeDef saveIndex);
void loadTimeControl(int index);
void saveTimeControl(int index);
void resetTimeControl(void);
void setOldTimeControlEnable(int value);
void setOldTimeControlData(int address, u8 *buffer, int size);
void getOldTimeControlData(int address, u8 *buffer, int size);
void CheckBoardVersion(void);
void save_hdmi_edid(u8 *edidData,int size);
void GetEDID(u8 *edidData);
void LoadOutputTypeParam(void);
void SaveOutputTypeParam(void);
void SaveCustomFormatParam(void);
void SaveSystemParam(void);
void LoadSystemParam(void);
void resetsendcard(void);
void savesendcard(void);
void loadSendCard(void);
void SetSendcardChoose(u8 choose);
void SetSendcardWidthChoose(u8 port ,u8 choose);
void resetUserSettingParam(void);
void resetUserPref(void);
void resetUserPrefIP(void);
void resetUserPrefWithoutIP(void);
void loadUserPref(void);
void saveUserPref(void);
void resetUserPadLock(void);
void saveUserPadLock(void);
void setUserPadLock(BYTE enable, BYTE value);
void loadPadLock(void);
void saveUserLang(BYTE lang);
void saveIPAddress(void);
void saveDelayCallTime(BYTE value);
void saveUserEDIDMode(BYTE inputPort,BYTE value);
void loadDeviceInfo(void);
void saveDeviceInfo(void);
void saveDeviceSN(u32 sn);
void saveDeviceModel(u32 sn);
void saveExtInputType(int type);
void saveExtInputPort(char port, char InPort);  //port  0-2   InPort   0-1
void SavePanleAndSplitParam(void);
void LoadPanleAndSplitParam(void);
void SavePipParam(void);
void LoadPipParam(void);
void SaveCurrentUserSettingParam(void);
void LoadCurrentUserSettingParam(void);
void SaveFpgaSplitParamToFlash(UserSaveIndex_TypeDef saveIndex);


#endif // _USER_PREF_H_

































































