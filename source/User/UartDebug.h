

#ifndef __UART_DEBUG_H__

#define __UART_DEBUG_H__


#define _ON				1
#define _OFF 			0

//#ifndef SOURCE_RELEASE

#define _DEBUG_MESSAGE_SYSTEM			_ON
#define _DEBUG_MESSAGE_RTD2796			_ON
#define _DEBUG_MESSAGE_KEY_BOARD		_OFF
#define _DEBUG_MESSAGE_KEY_SCAN			_ON
#define _DEBUG_MESSAGE_D74HC165			_OFF
#define _DEBUG_MESSAGE_CONTROL			_ON
#define _DEBUG_MESSAGE_FPGA				_OFF
#define _DEBUG_MESSAGE_AD4604			_OFF
#define _DEBUG_MESSAGE_RS232			_OFF
#define _DEBUG_MESSAGE_MAIN				_ON
#define _DEBUG_MESSAGE_USER_PREF		_OFF
#define _DEBUG_MESSAGE_FATFS			_ON
#define _DEBUG_MESSAGE_IT6604			_OFF
#define _DEBUG_MESSAGE_MENU				_OFF
#define _DEBUG_MESSAGE_FLASH			_OFF
#define _DEBUG_MESSAGE_SIL9134			_OFF




#if(_DEBUG_MESSAGE_SYSTEM == _ON)
#define DebugMessageSystem(format,arg...)  printf("[System]"format"\r\n",##arg)
#else
#define DebugMessageSystem(format,arg...)	;
#endif

#if(_DEBUG_MESSAGE_RTD2796 == _ON)
#define DebugMessageRtd2796(format,arg...)  printf("[Rtd2796]"format"\r\n",##arg)
#else
#define DebugMessageRtd2796(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_KEY_BOARD == _ON)
#define DebugMessageKeyBoard(format,arg...)  printf("[KeyBoard]"format"\r\n",##arg)
#else
#define DebugMessageKeyBoard(format,arg...)	;
#endif

#if(_DEBUG_MESSAGE_KEY_SCAN == _ON)
#define DebugMessageKeyScan(format,arg...) printf("[keyScan]"format,  ##arg)
#else
#define DebugMessageKeyScan(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_D74HC165 == _ON)
#define DebugMessageD74HC165(format,arg...)  printf("[D74HC165]"format"\r\n",##arg)
#else
#define DebugMessageD74HC165(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_CONTROL == _ON)
#define DebugMessageControl(format,arg...) printf("[control]"format,  ##arg)
#else
#define DebugMessageControl(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_FPGA == _ON)
#define DebugMessageFpga(format,arg...) printf("[FPGA]"format,  ##arg)
#else
#define DebugMessageFpga(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_AD4604 == _ON)
#define DebugMessageAD4604(format,arg...) printf("[ADN4604]"format,  ##arg)
#else
#define DebugMessageAD4604(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_RS232 == _ON)
#define DebugMessageRS232(format,arg...) printf("[RS232]"format,  ##arg)
#else
#define DebugMessageRS232(format,arg...)	;
#endif

#if(_DEBUG_MESSAGE_MAIN == _ON)
#define DebugMessageMain(format,arg...) printf("[main]"format,  ##arg)
#else
#define DebugMessageMain(format,arg...)	;
#endif

#if(_DEBUG_MESSAGE_USER_PREF == _ON)
#define DebugMessageUserPref(format,arg...) printf("[UserPref]"format,  ##arg)
#else
#define DebugMessageUserPref(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_FATFS == _ON)
#define DebugMessageFatfs(format,arg...) printf("[fatfs]"format,  ##arg)
#else
#define DebugMessageFatfs(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_IT6604 == _ON)
#define DebugMessageIt6604(format,arg...) printf("[IT6604]"format,  ##arg)
#else
#define DebugMessageIt6604(format,arg...)	;
#endif



#if(_DEBUG_MESSAGE_MENU == _ON)
#define DebugMessageMenu(format,arg...) printf("[menu]"format,  ##arg)
#else
#define DebugMessageMenu(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_FLASH == _ON)
#define DebugMessageFlash(format,arg...) printf("[flash]"format,  ##arg)
#else
#define DebugMessageFlash(format,arg...)	;
#endif


#if(_DEBUG_MESSAGE_SIL9134 == _ON)
#define DebugMessageSil9134(format,arg...) printf("[Sil9134]"format,  ##arg)
#else
#define DebugMessageSil9134(format,arg...)	;
#endif



#endif




