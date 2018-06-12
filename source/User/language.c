
#include "language.h"

const char *const langs[][LANG_TOTAL]=
{
	{"--PAGE END--","--结束页--"},
	{"--PAGE END--","--结束页--"},
	{"VSP3600","VSP3600"},
	{"VSP3500","VSP3500"},
	{"DEVINFO","设备信息"},
	{"RESET","复位"},
	{"INPUT CONFIG",	"输入配置"},
	{"LAYER CONFIG",	"图层配置"},
	{"OUTPUT CONFIG",	"输出配置"},
	{"SYSTEM",			"系统设置"},
	{"VIEW",			"视图模式"},
	{"LAYOUT",			"画中画"},
	{"MODE",			"画中画模式"},
	{"SPLIT SCREEN",	"分屏"},
	{"SPLIT MODE",		"分屏模式"},
	{"STRAIGHT",		"直通"},
	{"HORIZONTAL 1/2",	"水平一分二分屏"},
	{"HORIZONTAL 1/4",	"水平一分四分屏"},
	{"FIELD GLYPH",		"田字形分屏"},
	{"VERTICAL 1/2",	"垂直一分二分屏"},
	{"VERTICAL 1/4",	"垂直一分四分屏"},
	{"2 IN 3 OUT",	    "水平二进三出"},
	{"2 IN 4 OUT",		"水平二进四出"},

	{"OUTPUT WIDTH",			"输出宽度"},
	{"OUTPUT HEIGHT",		"输出高度"},
	{"TOTAL W",			"屏幕总宽"},
	{"TOTAL H",			"屏幕总高"},
	{"FIRST W",			"第一块屏宽"},
	{"FIRST H",			"第一块屏高"},
	{"SECOND W",		"第二块屏宽"},
	{"SECOND H",		"第二块屏高"},
	{"THIRD W",			"第三块屏宽"},
	{"THIRD H",			"第三块屏高"},
	{"START X",	        "级联起始X"},
	{"START Y",	        "级联起始Y"},
    
	{"ADVANCE","高级"},
	
	{"SAVE","保存"},
	{"SAVE FINISH","保存完成"},
	{"SAVE FAILED","保存失败"},
	{"SAVE TO 1~10","保存在1~10"},
	{"SAVE TO 1","保存到1"},
	{"SAVE TO 2","保存到2"},
	{"SAVE TO 3","保存到3"},
	{"SAVE TO 4","保存到4"},
	{"SAVE TO 5","保存到5"},
	{"SAVE TO 6","保存到6"},
	{"SAVE TO 7","保存到7"},
	{"SAVE TO 8","保存到8"},
	{"SAVE TO 9","保存到9"},
	{"SAVE TO 10","保存到10"},
	
	{"LOAD","载入"},
	{"LOAD FINISH","载入完成"},
	{"LOAD FAILED","载入失败"},
	{"LOAD FROM 1~10","从1~10载入"},	
	{"LOAD FROM 1","从1 载入"},	
	{"LOAD FROM 2","从2 载入"},
	{"LOAD FROM 3","从3 载入"},
	{"LOAD FROM 4","从4 载入"},
	{"LOAD FROM 5","从5 载入"},
	{"LOAD FROM 6","从6 载入"},
	{"LOAD FROM 7","从7 载入"},
	{"LOAD FROM 8","从8 载入"},
	{"LOAD FROM 9","从9 载入"},
	{"LOAD FROM 10","从10载入"},

	{"NO","否"},
	{"YES","是"},
	{"  NOW RESET...", "  系统复位中..."},
	{"  RESET FINISH!","  复位完成 !"},
	{"FAIL!","失败"},

	{"FADE IN/OUT","淡入淡出"}, // 20130306

	{"LANGUAGE","语言"},
	{"ENGLISH","ENGLISH"},
	{"中文","中文"},

	{"SET INPUT","选择输入"},
	{"SELECT OUTPUT","选择输出"},
	{"PROGRAM LAYER OF","编辑层"},
	
	{"LAYERA OF OUTPUT1","图层A 输出到1"},
	{"LAYERB OF OUTPUT1","图层B 输出到1"},
	{"LAYERC OF OUTPUT1","图层C 输出到1"},
	{"LAYERD OF OUTPUT1","图层D 输出到1"},
	
	{"LAYERA OF OUTPUT2","图层A 输出到2"},
	{"LAYERB OF OUTPUT2","图层B 输出到2"},
	{"LAYERC OF OUTPUT2","图层C 输出到2"},
	{"LAYERD OF OUTPUT2","图层D 输出到2"},

	{"LAYERA OF OUTPUT3","图层A 输出到3"},
	{"LAYERB OF OUTPUT3","图层B 输出到3"},
	{"LAYERC OF OUTPUT3","图层C 输出到3"},
	{"LAYERD OF OUTPUT3","图层D 输出到3"},

	{"LAYERA OF OUTPUT4","图层A 输出到4"},
	{"LAYERB OF OUTPUT4","图层B 输出到4"},
	{"LAYERC OF OUTPUT4","图层C 输出到4"},
	{"LAYERD OF OUTPUT4","图层D 输出到4"},

	{"ADD ABCD LAYERS","添加ABCD图层"},
	{"CLEAR ABCD LAYERS","删除ABCD图层"},

	{"NOTICE:","提示:"},
	{"  PAD LOCKED!","    键盘已锁!"},
	{"  PAD UNLOCK!","    键盘已解锁!"},
	{"PAD LOCKED!   PRESS",     "键盘已锁!   同时按"},
	{"MENU AND NEXT UNLOCK","<MENU>和<NEXT>解锁!"},

	{"PADLOCK SETTING","键盘锁设置"},
	{"PADLOCK","键盘锁"},

	{"SELECT LAYER","选择图层"},
	{"PROGRAM SOURCE OF","编辑输入源"},
	{"LAYER A SOURCE","A 层输入"},
	{"LAYER B SOURCE","B 层输入"},
	{"LAYER C SOURCE","C 层输入"},
	{"LAYER D SOURCE","D 层输入"},
	{"LAYER ABCD SOURCE","ABCD层输入"},

	{"LAYER A FREEZE","A 层冻结"},
	{"LAYER B FREEZE","B 层冻结"},
	{"LAYER C FREEZE","C 层冻结"},
	{"LAYER D FREEZE","D 层冻结"},

	{"LAYERA FULLSCREEN","A 层全屏"},
	{"LAYERB FULLSCREEN","B 层全屏"},
	{"LAYERC FULLSCREEN","C 层全屏"},
	{"LAYERD FULLSCREEN","D 层全屏"},

	{"LAYER A ROTATE","A 层翻转"},
	{"CROP","裁剪"},
	{"CROP X","裁剪X"},
	{"CROP Y","裁剪Y"},
	{"CROP WIDTH","裁剪宽度"},
	{"CROP HIGHT","裁剪高度"},
	
	{"POS X","水平位置"},
	{"POS Y","垂直位置"},
	{"SIZE X","缩放宽度"},
	{"SIZE Y","缩放高度"},

	{"CUT SWITCH TO","直切到"},
	{"TAKE SWITCH TO","切换到"},

	{"COLOR KEY","设置色键"},
	{"KEY ON/OFF","色键开关"},
	{"ON","开"},
	{"OFF","关"},
	{"KEY MODE","色键模式"},
	{"IN","输入"},
	{"OUT","输出"},

	{"LEFT","左翻转"},
	{"RIGHT","右翻转"},

	{"DISPLAY","显示"},
	{"MODE","模式"},
	{"H MIRROR","水平镜像"},	
	{"V MIRROR","垂直镜像"},
	{"LIVE","活动"},
	{"FREEZE","静止"},
	
	{"KEY COLOR","抠像"},
	{"SET R MIN/MAX","红色范围"},
	{"R MIN","红色最小值"},
	{"R MAX","红色最大值"},
	{"SET G MIN/MAX","绿色范围"},
	{"G MIN","绿色最小值"},
	{"G MAX","绿色最大值"},
	{"SET B MIN/MAX","蓝色范围"},
	{"B MIN","蓝色最小值"},
	{"B MAX","蓝色最大值"},
	{"COLOR SYC","键值同步"},
	{"OPACITY","透明度"},

	{"  SET SUCCESS!","设置成功 !"},
	
	{"SET SCALE","设置放大范围"},
	{"SET ZOOM","设置放大范围"},
	{"SET POSITON","设置放大范围"},
	
	{"SET CROP","设置裁剪区域"},
	{"SET ROTATE","设置翻转"},
	{"ROTATE MODE","翻转模式"},

	{"ALPHA","透明度"},
	{"LAYER A ALPHA","A 层透明度"},
	{"LAYER B ALPHA","B 层透明度"},
	{"LAYER C ALPHA","C 层透明度"},
	{"LAYER D ALPHA","D 层透明度"},
	{"ALPHA TIME","透明度时间"},

	{"INPUT 1","输入1"},
	{"INPUT 2","输入2"},
	{"INPUT 3","输入3"},
	{"INPUT 4","输入4"},

	{"OUTPUT 1","输出1"},
	{"OUTPUT 2","输出2"},
	{"OUTPUT 3","输出3"},
	{"OUTPUT 4","输出4"},

	{"LAYER A","图层1"},
	{"LAYER B","图层2"},
	{"LAYER C","图层3"},
	{"LAYER D","图层4"},

	{"SET PICTURE",	"图像调节"},
	{"BRIGHTNESS",	"亮度"},
	{"CONTRAST",	"对比度"},
	{"SATURATION",	"色度"},

	{"SIGNAL BACKUP",	"信号备份"},
	{"BACK UP",	"备份"},
        
	{"DELAY CALL SAVE",	"延时调保存"},
	{"TIME(SEC.)",	"延时时间"},
	{"INDEX",	"调保存编号"},

	{"MODIFY IP", "修改IP地址"},
	{"SET MAX ALPHA", "设置亮度"},
	{"ALPHA", "亮度"},
	
	{"COMMON FORMAT","常用分辨率"},
	{"EXTERNAL SYNC.", "选择外同步"},
	{"OTHER FORMAT","其它分辨率"}, // 20130116

	{"TIMING ADJUST","全局时序调整"},    
	{"TIMING ADJUST","时序调整"},
	{"H TOTAL","H TOTAL"},
	{"V TOTAL","V TOTAL"},
	{"H POS","H POS"},
	{"V POS","V POS"},
	{"H ACTIVE","H ACTIVE"},
	{"V ACTIVE","V ACTIVE"},	
	{"HV POLARITY","水平极性"},
	{"H POLARITY","水平极性"},
	{"V POLARITY","垂直极性"},
	
	{"POS","正向"},
	{"NEG","反向"},
	
	{"HV SYNC","水平垂直同步"},
	{"H SYNC","水平同步"},
	{"V SYNC","垂直同步"},

	{"INPUT FORMAT","输入格式"},
	{"FORMAT","输入分辨率"},
	{"AUTO DETECT","自动侦测"},
	{"COLOR SPACE","色域"},

	{"SAVE CONFIG","保存设置"},
	{"RESET CONFIG","恢复设置"},
	{"  SAVE FINISH","保存完成"},
	{"  SAVE FAIL","保存失败"},

	{"SET EFFECT","设置特效"},
	{"MODE:","特效模式"},
	{"SPEED:","特效速度"},
	
	{"SPLIT SOURCE","分屏输入源"},
	{"VSP3600 VER:","VSP3600版本号"},
	{"VSP3500 VER:","VSP3500版本号"},
	
	{"LAYER A TP","图层A TP设置"},
	{"LAYER B TP","图层B TP设置"},
	{"LAYER C TP","图层C TP设置"},
	{"LAYER D TP","图层D TP设置"},

	{"SELECT LAYER","添加图层"},
	{"SELECT LAYER","删除图层"},

	{"THE SAVE IS BLANK,","该保存是空的,"},
	{"CAN NOT LOAD","不能调保存"},

	{"LAYER ABCD STATUS:","ABCD图层状态:"},
	{"FULL SCREEN","全屏"},

	{"DISABLE","禁止"},
	{"ENABLE","允许"},
	
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



