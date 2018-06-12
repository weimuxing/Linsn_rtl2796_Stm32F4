#include "SysConfig.h"

#include "langStr.h"
#include "userPref.h"
//#include "keyScan.h"



//#define SUPPORT_LANG_CHINESE
static Lang_TypeDef _lang = LANG_ENGLISH;
//static Lang_TypeDef _lang = LANG_ZH_CN;

#ifdef SUPPORT_LANG_CHINESE
extern const char * const _langStr_Cn[];
#endif
extern const char * const _langStr_En[];

void lang_SetLang(Lang_TypeDef lang)
{	
	_lang = lang;
}



Lang_TypeDef lang_GetLang(void)
{
	return _lang;
}

char * langGetStr(int index)
{
	char * str;

	lang_GetLang();
	switch(_lang)
	{
		case LANG_ENGLISH:
			str = lang_GetEnStrByIndex(index);
			break;

	#ifdef SUPPORT_LANG_CHINESE
		case LANG_ZH_CN:
			str = lang_GetCnStrByIndex(index);
			break;
	#endif
			
		default:
			break;
	}
	return str;
}

char * lang_GetEnStrByIndex(int index)
{
	//int size = sizeof(_langStr_En)/8;
	if(index < maxStrSize)
	{
		
		if(index == Menu0Line1Str)
		{
			return getDeviceInfo()->deviceTypeStr;
		}
		
		if(index == companyInfoStr)
		{
			if(getDeviceInfo()->logoDisMode & 0x80)
			{
				if(_lang == LANG_ENGLISH)
				{
					return getDeviceInfo()->companyLogoStr;
				}
				else
				{
					return getDeviceInfo()->deviceLogoStr;
				}
			}			
		}
		
		return (char * )_langStr_En[index];
	}

	return (char * )_langStr_En[spaceStr];
}

#ifdef SUPPORT_LANG_CHINESE
char * lang_GetCnStrByIndex(int index)
{
	//int size = sizeof(_langStr_Cn)/8;
	if(index < maxStrSize)
	{
		
		if(index == Menu0Line1Str)
		{
			return getDeviceInfo()->deviceTypeStr;
		}
		
		if(index == companyInfoStr)
		{
			if(getDeviceInfo()->logoDisMode & 0x80)
			{
				if(_lang == LANG_ENGLISH)
				{
					return getDeviceInfo()->companyLogoStr;
				}
				else
				{
					return getDeviceInfo()->deviceLogoStr;
				}
			}			
		}	
		return (char * )_langStr_Cn[index];
	}

	return (char * )_langStr_Cn[spaceStr];
}
#endif

