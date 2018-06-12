
#include "language.h"

const char *const langs[][LANG_TOTAL]=
{
	{"--PAGE END--","--����ҳ--"},
	{"--PAGE END--","--����ҳ--"},
	{"VSP3600","VSP3600"},
	{"VSP3500","VSP3500"},
	{"DEVINFO","�豸��Ϣ"},
	{"RESET","��λ"},
	{"INPUT CONFIG",	"��������"},
	{"LAYER CONFIG",	"ͼ������"},
	{"OUTPUT CONFIG",	"�������"},
	{"SYSTEM",			"ϵͳ����"},
	{"VIEW",			"��ͼģʽ"},
	{"LAYOUT",			"���л�"},
	{"MODE",			"���л�ģʽ"},
	{"SPLIT SCREEN",	"����"},
	{"SPLIT MODE",		"����ģʽ"},
	{"STRAIGHT",		"ֱͨ"},
	{"HORIZONTAL 1/2",	"ˮƽһ�ֶ�����"},
	{"HORIZONTAL 1/4",	"ˮƽһ���ķ���"},
	{"FIELD GLYPH",		"�����η���"},
	{"VERTICAL 1/2",	"��ֱһ�ֶ�����"},
	{"VERTICAL 1/4",	"��ֱһ���ķ���"},
	{"2 IN 3 OUT",	    "ˮƽ��������"},
	{"2 IN 4 OUT",		"ˮƽ�����ĳ�"},

	{"OUTPUT WIDTH",			"������"},
	{"OUTPUT HEIGHT",		"����߶�"},
	{"TOTAL W",			"��Ļ�ܿ�"},
	{"TOTAL H",			"��Ļ�ܸ�"},
	{"FIRST W",			"��һ������"},
	{"FIRST H",			"��һ������"},
	{"SECOND W",		"�ڶ�������"},
	{"SECOND H",		"�ڶ�������"},
	{"THIRD W",			"����������"},
	{"THIRD H",			"����������"},
	{"START X",	        "������ʼX"},
	{"START Y",	        "������ʼY"},
    
	{"ADVANCE","�߼�"},
	
	{"SAVE","����"},
	{"SAVE FINISH","�������"},
	{"SAVE FAILED","����ʧ��"},
	{"SAVE TO 1~10","������1~10"},
	{"SAVE TO 1","���浽1"},
	{"SAVE TO 2","���浽2"},
	{"SAVE TO 3","���浽3"},
	{"SAVE TO 4","���浽4"},
	{"SAVE TO 5","���浽5"},
	{"SAVE TO 6","���浽6"},
	{"SAVE TO 7","���浽7"},
	{"SAVE TO 8","���浽8"},
	{"SAVE TO 9","���浽9"},
	{"SAVE TO 10","���浽10"},
	
	{"LOAD","����"},
	{"LOAD FINISH","�������"},
	{"LOAD FAILED","����ʧ��"},
	{"LOAD FROM 1~10","��1~10����"},	
	{"LOAD FROM 1","��1 ����"},	
	{"LOAD FROM 2","��2 ����"},
	{"LOAD FROM 3","��3 ����"},
	{"LOAD FROM 4","��4 ����"},
	{"LOAD FROM 5","��5 ����"},
	{"LOAD FROM 6","��6 ����"},
	{"LOAD FROM 7","��7 ����"},
	{"LOAD FROM 8","��8 ����"},
	{"LOAD FROM 9","��9 ����"},
	{"LOAD FROM 10","��10����"},

	{"NO","��"},
	{"YES","��"},
	{"  NOW RESET...", "  ϵͳ��λ��..."},
	{"  RESET FINISH!","  ��λ��� !"},
	{"FAIL!","ʧ��"},

	{"FADE IN/OUT","���뵭��"}, // 20130306

	{"LANGUAGE","����"},
	{"ENGLISH","ENGLISH"},
	{"����","����"},

	{"SET INPUT","ѡ������"},
	{"SELECT OUTPUT","ѡ�����"},
	{"PROGRAM LAYER OF","�༭��"},
	
	{"LAYERA OF OUTPUT1","ͼ��A �����1"},
	{"LAYERB OF OUTPUT1","ͼ��B �����1"},
	{"LAYERC OF OUTPUT1","ͼ��C �����1"},
	{"LAYERD OF OUTPUT1","ͼ��D �����1"},
	
	{"LAYERA OF OUTPUT2","ͼ��A �����2"},
	{"LAYERB OF OUTPUT2","ͼ��B �����2"},
	{"LAYERC OF OUTPUT2","ͼ��C �����2"},
	{"LAYERD OF OUTPUT2","ͼ��D �����2"},

	{"LAYERA OF OUTPUT3","ͼ��A �����3"},
	{"LAYERB OF OUTPUT3","ͼ��B �����3"},
	{"LAYERC OF OUTPUT3","ͼ��C �����3"},
	{"LAYERD OF OUTPUT3","ͼ��D �����3"},

	{"LAYERA OF OUTPUT4","ͼ��A �����4"},
	{"LAYERB OF OUTPUT4","ͼ��B �����4"},
	{"LAYERC OF OUTPUT4","ͼ��C �����4"},
	{"LAYERD OF OUTPUT4","ͼ��D �����4"},

	{"ADD ABCD LAYERS","���ABCDͼ��"},
	{"CLEAR ABCD LAYERS","ɾ��ABCDͼ��"},

	{"NOTICE:","��ʾ:"},
	{"  PAD LOCKED!","    ��������!"},
	{"  PAD UNLOCK!","    �����ѽ���!"},
	{"PAD LOCKED!   PRESS",     "��������!   ͬʱ��"},
	{"MENU AND NEXT UNLOCK","<MENU>��<NEXT>����!"},

	{"PADLOCK SETTING","����������"},
	{"PADLOCK","������"},

	{"SELECT LAYER","ѡ��ͼ��"},
	{"PROGRAM SOURCE OF","�༭����Դ"},
	{"LAYER A SOURCE","A ������"},
	{"LAYER B SOURCE","B ������"},
	{"LAYER C SOURCE","C ������"},
	{"LAYER D SOURCE","D ������"},
	{"LAYER ABCD SOURCE","ABCD������"},

	{"LAYER A FREEZE","A �㶳��"},
	{"LAYER B FREEZE","B �㶳��"},
	{"LAYER C FREEZE","C �㶳��"},
	{"LAYER D FREEZE","D �㶳��"},

	{"LAYERA FULLSCREEN","A ��ȫ��"},
	{"LAYERB FULLSCREEN","B ��ȫ��"},
	{"LAYERC FULLSCREEN","C ��ȫ��"},
	{"LAYERD FULLSCREEN","D ��ȫ��"},

	{"LAYER A ROTATE","A �㷭ת"},
	{"CROP","�ü�"},
	{"CROP X","�ü�X"},
	{"CROP Y","�ü�Y"},
	{"CROP WIDTH","�ü����"},
	{"CROP HIGHT","�ü��߶�"},
	
	{"POS X","ˮƽλ��"},
	{"POS Y","��ֱλ��"},
	{"SIZE X","���ſ��"},
	{"SIZE Y","���Ÿ߶�"},

	{"CUT SWITCH TO","ֱ�е�"},
	{"TAKE SWITCH TO","�л���"},

	{"COLOR KEY","����ɫ��"},
	{"KEY ON/OFF","ɫ������"},
	{"ON","��"},
	{"OFF","��"},
	{"KEY MODE","ɫ��ģʽ"},
	{"IN","����"},
	{"OUT","���"},

	{"LEFT","��ת"},
	{"RIGHT","�ҷ�ת"},

	{"DISPLAY","��ʾ"},
	{"MODE","ģʽ"},
	{"H MIRROR","ˮƽ����"},	
	{"V MIRROR","��ֱ����"},
	{"LIVE","�"},
	{"FREEZE","��ֹ"},
	
	{"KEY COLOR","����"},
	{"SET R MIN/MAX","��ɫ��Χ"},
	{"R MIN","��ɫ��Сֵ"},
	{"R MAX","��ɫ���ֵ"},
	{"SET G MIN/MAX","��ɫ��Χ"},
	{"G MIN","��ɫ��Сֵ"},
	{"G MAX","��ɫ���ֵ"},
	{"SET B MIN/MAX","��ɫ��Χ"},
	{"B MIN","��ɫ��Сֵ"},
	{"B MAX","��ɫ���ֵ"},
	{"COLOR SYC","��ֵͬ��"},
	{"OPACITY","͸����"},

	{"  SET SUCCESS!","���óɹ� !"},
	
	{"SET SCALE","���÷Ŵ�Χ"},
	{"SET ZOOM","���÷Ŵ�Χ"},
	{"SET POSITON","���÷Ŵ�Χ"},
	
	{"SET CROP","���òü�����"},
	{"SET ROTATE","���÷�ת"},
	{"ROTATE MODE","��תģʽ"},

	{"ALPHA","͸����"},
	{"LAYER A ALPHA","A ��͸����"},
	{"LAYER B ALPHA","B ��͸����"},
	{"LAYER C ALPHA","C ��͸����"},
	{"LAYER D ALPHA","D ��͸����"},
	{"ALPHA TIME","͸����ʱ��"},

	{"INPUT 1","����1"},
	{"INPUT 2","����2"},
	{"INPUT 3","����3"},
	{"INPUT 4","����4"},

	{"OUTPUT 1","���1"},
	{"OUTPUT 2","���2"},
	{"OUTPUT 3","���3"},
	{"OUTPUT 4","���4"},

	{"LAYER A","ͼ��1"},
	{"LAYER B","ͼ��2"},
	{"LAYER C","ͼ��3"},
	{"LAYER D","ͼ��4"},

	{"SET PICTURE",	"ͼ�����"},
	{"BRIGHTNESS",	"����"},
	{"CONTRAST",	"�Աȶ�"},
	{"SATURATION",	"ɫ��"},

	{"SIGNAL BACKUP",	"�źű���"},
	{"BACK UP",	"����"},
        
	{"DELAY CALL SAVE",	"��ʱ������"},
	{"TIME(SEC.)",	"��ʱʱ��"},
	{"INDEX",	"��������"},

	{"MODIFY IP", "�޸�IP��ַ"},
	{"SET MAX ALPHA", "��������"},
	{"ALPHA", "����"},
	
	{"COMMON FORMAT","���÷ֱ���"},
	{"EXTERNAL SYNC.", "ѡ����ͬ��"},
	{"OTHER FORMAT","�����ֱ���"}, // 20130116

	{"TIMING ADJUST","ȫ��ʱ�����"},    
	{"TIMING ADJUST","ʱ�����"},
	{"H TOTAL","H TOTAL"},
	{"V TOTAL","V TOTAL"},
	{"H POS","H POS"},
	{"V POS","V POS"},
	{"H ACTIVE","H ACTIVE"},
	{"V ACTIVE","V ACTIVE"},	
	{"HV POLARITY","ˮƽ����"},
	{"H POLARITY","ˮƽ����"},
	{"V POLARITY","��ֱ����"},
	
	{"POS","����"},
	{"NEG","����"},
	
	{"HV SYNC","ˮƽ��ֱͬ��"},
	{"H SYNC","ˮƽͬ��"},
	{"V SYNC","��ֱͬ��"},

	{"INPUT FORMAT","�����ʽ"},
	{"FORMAT","����ֱ���"},
	{"AUTO DETECT","�Զ����"},
	{"COLOR SPACE","ɫ��"},

	{"SAVE CONFIG","��������"},
	{"RESET CONFIG","�ָ�����"},
	{"  SAVE FINISH","�������"},
	{"  SAVE FAIL","����ʧ��"},

	{"SET EFFECT","������Ч"},
	{"MODE:","��Чģʽ"},
	{"SPEED:","��Ч�ٶ�"},
	
	{"SPLIT SOURCE","��������Դ"},
	{"VSP3600 VER:","VSP3600�汾��"},
	{"VSP3500 VER:","VSP3500�汾��"},
	
	{"LAYER A TP","ͼ��A TP����"},
	{"LAYER B TP","ͼ��B TP����"},
	{"LAYER C TP","ͼ��C TP����"},
	{"LAYER D TP","ͼ��D TP����"},

	{"SELECT LAYER","���ͼ��"},
	{"SELECT LAYER","ɾ��ͼ��"},

	{"THE SAVE IS BLANK,","�ñ����ǿյ�,"},
	{"CAN NOT LOAD","���ܵ�����"},

	{"LAYER ABCD STATUS:","ABCDͼ��״̬:"},
	{"FULL SCREEN","ȫ��"},

	{"DISABLE","��ֹ"},
	{"ENABLE","����"},
	
};

const char *const lang_output[]=
{
	"OUTPUT 1",
	"OUTPUT 2",
	"OUTPUT 3",
	"OUTPUT 4",
};

const char *const lang_output_35[]=
{
	"DVI 1",
	"DVI 2",
	"DVI 3",
	"DVI 4",
};

const char *const lang_layer[]=
{
	"LAYER A",
	"LAYER B",
	"LAYER C",
	"LAYER D",
};

const char *const lang_layerOperate[]=
{
	"LAYER",
	"LAYER",
};
const char *const lang_layeradd[]=
{
	"CLEAR",
	"ADD",	
};
const char *const lang_input[]=
{
	"INPUT 1",
	"INPUT 2",
	"INPUT 3",
	"INPUT 4",
};
const char *const lang_enable[]=
{
	"DISABLE",
	"ENABLE",
};
const char *const lang_freeze[]=
{
	"LIVE",
	"FREEZE",
};
const char *const lang_fs[]=
{
	"FULL SCREEN",
};
const char *const lang_rotate[]=
{
	"OFF",
	"LEFT ROTATE",
	"RIGHT ROTATE",
};

const char *const facInfo_str[]=
{
	"Great Viwe",
	"IP:",
	"   .",
	"COM BAUDRATE:115200",
	"rgblinkcs@gmail.com",
	"www.rgblink.com",
};



