#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#define LANG_TOTAL 2
#define get_lang(x) (*((x)+getUserLang()))
#define get_lang_index() getUserLang()

enum elang_index
{
	elang_pageend_l1,
	elang_pageend_l2,
	
	elang_menu,
	elang_menu_35,
	
	elang_devinfo,
	elang_reset,
	elang_InputCfg,
	elang_LayerCfg,
	elang_OutputCfg,
	elang_System,
	elang_View,
	
	elang_LayOut,
	elang_LayOutMode,
	elang_SplitScreen,
	elang_SplitMode,
	elang_Split_Full,
	elang_Split_H_2,
	elang_Split_H_4,
	elang_Split_HV_4,
	elang_Split_V_2,
	elang_Split_V_4,
	elang_Split_2in3out,
	elang_Split_2in4out,

	elang_output_width,
	elang_output_height,
	elang_Split_totalW,
	elang_Split_totalH,
	elang_Split_firstW,
	elang_Split_firstH,
	elang_Split_secondW,
	elang_Split_secondH,
	elang_Split_thirdW,
	elang_Split_thirdH,

	elang_Split_multiStartX,
	elang_Split_multiStartY,


	elang_Advance,
	
	elang_Save,
	elang_SaveOK,
	elang_SaveFail,
	elang_SaveTo0_9,	
	elang_SaveTo0,	
	elang_SaveTo1,	
	elang_SaveTo2,	
	elang_SaveTo3,	
	elang_SaveTo4,	
	elang_SaveTo5,	
	elang_SaveTo6,	
	elang_SaveTo7,	
	elang_SaveTo8,	
	elang_SaveTo9,	
	
	elang_Load,
	elang_LoadOK,
	elang_LoadFail,
	elang_LoadFrom0_9,
	elang_LoadFrom0,	
	elang_LoadFrom1,
	elang_LoadFrom2,
	elang_LoadFrom3,
	elang_LoadFrom4,
	elang_LoadFrom5,
	elang_LoadFrom6,
	elang_LoadFrom7,
	elang_LoadFrom8,
	elang_LoadFrom9,	

	elang_no,
	elang_yes,
	
	elang_sysReset,
	elang_completed,
	elang_fail,
	elang_LoadFadeInOutOff,  // 调保存淡入淡出开关

	elang_langsel,
	elang_english,
	elang_chinese,

	elang_curinput,
	elang_curoutput,
	elang_prglayer,
	elang_layerAOfOutput1,
	elang_layerBOfOutput1,
	elang_layerCOfOutput1,
	elang_layerDOfOutput1,
	
	elang_layerAOfOutput2,
	elang_layerBOfOutput2,
	elang_layerCOfOutput2,
	elang_layerDOfOutput2,
	
	elang_layerAOfOutput3,
	elang_layerBOfOutput3,
	elang_layerCOfOutput3,
	elang_layerDOfOutput3,

	elang_layerAOfOutput4,
	elang_layerBOfOutput4,
	elang_layerCOfOutput4,
	elang_layerDOfOutput4,

	elang_AddLayerABCD,
	elang_ClearLayerABCD,

	elang_NoticeStr,
	elang_PadLocked,
	elang_PadUnlock,
	elang_PadLockNotice0,
	elang_PadLockNotice1,

	elang_PadLockSetting,
	elang_PadLockEnable,

	elang_curlayer,
	elang_prginput,
	elang_layerAsource,
	elang_layerBsource,
	elang_layerCsource,
	elang_layerDsource,
	elang_layerABCDsource,

	elang_LayerAfreeze,
	elang_LayerBfreeze,
	elang_LayerCfreeze,
	elang_LayerDfreeze,
	elang_LayerAfullscreen,
	elang_LayerBfullscreen,
	elang_LayerCfullscreen,
	elang_LayerDfullscreen,
	elang_layerARotate,
	elang_layercrop,
	elang_layercropX,
	elang_layercropY,
	elang_layercropW,
	elang_layercropH,
	elang_layerposX,
	elang_layerposY,
	elang_layersizeW,
	elang_layersizeH,
	
	elang_outputcut,
	elang_outputtake,

	elang_ColorKey, // long
	elang_key_onoff,
	elang_on,
	elang_off,	
	elang_key_inout,
	elang_in,
	elang_out,

	elang_left,
	elang_right,

	elang_display,
	elang_dispMode,
	elang_dispHmirror,	
	elang_dispVmirror,	
	elang_Live,
	elang_Freeze,
	
	elang_key_color,
	elang_key_color_R,
	elang_key_color_R_min,
	elang_key_color_R_max,
	elang_key_color_G,
	elang_key_color_G_min,
	elang_key_color_G_max,
	elang_key_color_B,
	elang_key_color_B_min,
	elang_key_color_B_max,
	elang_key_color_syc,
	elang_key_color_ALPHA,

	elang_set_success,

	elang_set_scale,
	elang_set_zoom,
	elang_set_positon,
	
	elang_set_crop,
	elang_set_rotate,
	elang_rotate_mode,

	elang_Alpha,
	elang_AlphaA,
	elang_AlphaB,
	elang_AlphaC,
	elang_AlphaD,
	elang_AlphaTime,

	elang_input1,
	elang_input2,
	elang_input3,
	elang_input4,

	elang_Output1,
	elang_Output2,
	elang_Output3,
	elang_Output4,

	elang_LayerA,
	elang_LayerB,
	elang_LayerC,
	elang_LayerD,
	
	elang_Picture,
	elang_Brightness,
	elang_Contrast,
	elang_Saturation,

	elang_SignalBackup,
	elang_SignalBackup_onoff,

	elang_DelayCallSave,
	elang_DelayCallTime,
	elang_DelayCallIndex,

	elang_ModifyIPaddr,
	elang_SetMaxAlpha,
	elang_MaxAlpha,

	elang_OutputFormat,
	elang_OutputFormat_extern,
	elang_OutputFormat_others,

	elang_TimeAdjust_4outputs,
	elang_TimeAdjust,
	elang_AdjustHtotal,
	elang_AdjustVtotal,
	elang_AdjustHpositon,
	elang_AdjustVpositon,
	elang_AdjustHactive,
	elang_AdjustVactive,
	
	elang_HVpolarity,	
	elang_Hpolarity,
	elang_Vpolarity,
	
	elang_Polarity_Pos,
	elang_Polarity_Neg,
	
	elang_HVsync,	
	elang_Hsync,
	elang_Vsync,

	elang_InputFormat,
	elang_FormatVideo,
	elang_FormatAuto,	
	elang_InputColorSpace,

	elang_SaveConfig,
	elang_ResetConfig,
	elang_SaveTimingOK,
	elang_SaveTimingFail,

	elang_SetEffect,
	elang_SetEffectMode,
	elang_SetEffectSpeed,
	elang_SplitSource,	

	elang_Version,	
	elang_Version_35,	

	elang_TP_LayerA,	
	elang_TP_LayerB,	
	elang_TP_LayerC,	
	elang_TP_LayerD,	

	elang_Layer_ADD,	
	elang_Layer_Clr,	

	elang_save_blank1,
	elang_save_blank2,

	elang_LayerABCDstatus,
	elang_FullScreen,

	elang_disabale,	
	elang_enbale,	

};

extern const char *const langs[][LANG_TOTAL];
extern const char *const lang_output[];
extern const char *const lang_layer[];
extern const char *const lang_layeradd[];
extern const char *const lang_input[];
extern const char *const lang_enable[];
extern const char *const lang_freeze[];
extern const char *const lang_fs[];
extern const char *const lang_rotate[];
extern const char *const facInfo_str[];

#endif



