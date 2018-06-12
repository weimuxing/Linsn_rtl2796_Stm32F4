

#ifndef MENUDATA_H
#define MENUDATA_H



enum
{
	naValue,
	pgmSourceValue,
	prvSourceValue,
    inputTypeValue,
    inputFormatValue,
    outputTypeValue,
    outputFormatValue,
    colorTempValue,
    colorTempRedValue,
    colorTempGreenValue,
    colorTempBlueValue,
    resetColorTempValue,
    contrastValue,
    colorEffectValue,
    brightnessValue,
    sharpnessValue,
    saturationValue,
	inputSourceValue,
	stepValue,
	autoFitValue,
	scaleModeValue,
	scalePosxValue,
	scalePosyValue,
	scaleWidthValue,
	scaleHeightValue,
	scaleWidthAndHeightValue,
	resetScaleValue,
	zoomWidthValue,
	zoomHeightValue,
	zoomPosxValue,
	zoomPosyValue,
	
	zoomUpValue,
	zoomDownValue,
	zoomUpDownValue,
	zoomLeftValue,
	zoomRightValue,
	zoomLeftRightValue,
	zoomCenterValue,
	resetZoomValue,

	cropUpValue,
	cropDownValue,
	cropUpDownValue,
	cropLeftValue,
	cropRightValue,
	cropLeftRightValue,
	cropCenterValue,
	resetCropValue,

	vgaPosxValue,
	vgaPosyValue,
	vgaClockValue,
	vgaPhaseValue,
	vgaAutoAdjustValue,
	resetInputConfigValue,
	resetPictureValue,
	screenWidthValue,
	screenHeightValue,
	screenPosxValue,
	screenPosyValue,
	screenModeValue,
	screenFollowValue,
	resetScreenValue,
	dviSelectValue,
	deOnOffValue,
	dePosxValue,
	dePosyValue,
	deWidthValue,
	deHeightValue,
	deHPalarityValue,
	deVPalarityValue,
	dataRangeValue,
	dviHdmiValue,
	resetDviValue,
	gammaValue,
	switchModeValue,
	fadeTimeValue,
	alphaValue,
	saveToValue,
	loadFromValue,
	usbSingleLoadValue,
	usbAllLoadValue,
	usbAllSaveValue,
	freezeValue,
	pipModeValue,
	pipSourceValue,
	swapMainPipValue,
	layoutValue,
	currentWindowValue,
	languageValue,
	languageLongValue,
	windowRatioValue,
	displayModeValue,
	flatRedValue,
	flatGreenValue,
	flatBlueValue,
	muteOnOffValue,
	audioModeValue,
	volumeValue,
	hdmiAudioValue,
	factoryResetValue,
	saveIpResetValue,

	snValue,
	WriteSnValue,
	mcuVersionValue,
	videoVersionValue,
	fpgaVersionValue,
	ext1MainBoardSoftVersionValue,
	ext2MainBoardSoftVersionValue,
	commBoardHardVersionValue,
	mainBoardHardVersionValue,
	ext1BoardHardVersionValue,
	ext2BoardHardVersionValue,
	extInputHardVersionValue,
	mainBoardVersionValue,
	
	ipAddress3Value,
	keyLockValue,
	lockPanelPasswordValue,
	networkEnableValue,
	dhcpEnableValue,
	adcAutoAdjustValue,
	resetAdcSettingValue,
	lockModeValue,
	lockSourceValue,
	dateValue,
	timeValue,
	workTimeValue,
	delayCallSaveValue,
	timingControlValue,
	canUseTimeValue,
	licenseStateValue,
	licenseTimeValue,
	licensePasswordValue,
	tpSwitchValue,
	testPatternModeValue,
	tpColorRedValue,
	tpColorGreenValue,
	tpColorBlueValue,
	tpHorStepValue,
	tpVerStepValue,
	tpStepColorValue,
	inputHStartValue,
	inputVStartValue,
	inputHActValue,
	inputVActValue,
	textEnableValue,
	textModeValue,
	textAboveValue,
	textAndOrValue,
	textAlphaValue,
	textRedValue,
	textGreenValue,
	textBlueValue,
	textPresetValue,
	splitOnOffValue,
	splitSyncModeValue,
	splitHTotalValue,
	splitVTotalValue,
	splitHSizeValue,
	splitVSizeValue,
	splitHPosValue,
	splitVPosValue,
	totalWorkTimeValue,
	bootTimesValue,
	outputColorSpaceValue,
	interlaceSwitchValue, 
	imageEnhanceValue, //added by cyj
	resetSdiInputValue,
	RemoteIpAddressValue,
	LocalIpAddressValue,
	subnetMaskValue,
	gatewayAddressValue,
	vLockLineValue,
	autoTpIimeValue,

	customFormatValue,
	customHValue,  //custom resolution Horizontal pixel
	customVValue,  //custom resolution Verical Line
	customFValue,  //custom resolution Frequency

	replaceRecoedValue, //repalce a recorded record

	standardFormatValue,
	otherFormatValue,

	saleNumberValue,
	customNumberValue,
	emailValue,
	webSiteValue,
	vgaSelectValue, //added by cyj 

	resetSplitValue, //added by cyj 

	lastWorkTimeValue, //added by cyj 

	vga1KeyModeValue,
	vga2KeyModeValue,

	luminanceValue,

	inputHActiveValue,
	inputVActiveValue,
	inputFreqValue,
	inputHTotalValue,
	inputVTotalValue,

	imageEnhanceVerifyValue,

 	bitDepthValue,

	bootloaderValue,
	
	ttlInterlaveModeValue,
		
	hotBackupEnableValue,
	hotBackupClass1Value,
	hotBackupClass2Value,
	hotBackupClass3Value,
	hotBackupClass4Value,
	hotBackupClass5Value,	
	
	edidSourcePortValue,
	edidDetinationPortValue,

	autoSpeedValue,
	currentSpeedValue,
	currentTempValue,

	temperatureModeValue,
	temperatureValue,
	
	customEDIDValue,
	customEDIDHValue,  //custom resolution Horizontal pixel
	customEDIDVValue,  //custom resolution Verical Line
	customEDIDFValue,  //custom resolution Frequency
	
	videoStatusValue,
	mainBoardStatusValue,

	blackKeyTypeValue,
	mapKeyTypeValue,
	scaleKeyTypeValue,
		
	realTimeSaveValue,
	
	splitFixedTypeValue,
	splitFixedIndexValue,
	splitFixedHNumberValue,
	splitFixedVNumberValue,	
	splitFixedHSizeValue,
	splitFixedVSizeValue,

	switchTypeValue,

	previewDblTapSwitchValue,
	previewSwitchValue,
	previewDVISwitchValue,	

	extInputComfigValue,

	dviAndExtAudioValue,
	audioTypeValue,
	hdmiAndExtAudioValue,

	AnotherFormatValue,
	AnothersourceValue,
	dvi1OutputValue,
	dvi1InputValue,

	audioInput1Value,
	audioInput2Value,
	audioInput3Value,

	cropInputWidthValue,
	cropInputHeightValue,
	cropInputPosxValue,
	cropInputPosyValue,
	resetCropInputValue,
	
	sendCardTypeValue,
	LEDoutputFormatValue,

	brightness_sendcard_value,
	xposvalue,
	yposvalue,
	xsizevalue,
	ysizevalue,
	sendcardsetvalue,
	gamma_sendcard_Value,
	connectvalue,
	
	xposvalue1,
	yposvalue1,
	xsizevalue1,
	ysizevalue1,

    sendCardPortSetWidthValue,
    sendCardPort1WidthValue,
    sendCardPort2WidthValue,
    sendCardPort3WidthValue,
    sendCardPort4WidthValue,   

	// ∑¢ÀÕø®œ‡πÿµƒ…Ë÷√œÓ
	// ’‚–©œÓ’Î∂‘∑¢ÀÕø®µƒΩ” ’ø
	sendCardPortSelValue,
	sendCardWallHPanelsValue,					
	sendCardWallVPanelsValue,	
	sendCardHPanelsValue,					
	sendCardVPanelsValue,
	sendCardWallHLedsValue,					
	sendCardWallVLedsValue,		
	sendCardHLedsValue,	
	sendCardVLedsValue,
	sendCardPortHPosValue,	
	sendCardPortVPosValue,	
	sendCardPanelNoValue,
	sendCardPanelHOffsetValue,	
	sendCardPanelVOffsetValue,
	sendCardWallConectValue,
	sendCardPanelConectValue,
	sendCardSendToPortValue,
	sendCardSendToAutoPortValue,
	sendCardSendAllPortValue,
	sendCardSendToPanelValue,
	sendCardResetPortValue,	
	sendCardSetWidthValue,
	sendCardSetWidthHValue,
	sendCardSetWidthVValue,
	
	// ’‚–©œÓ’Î∂‘∑¢ÀÕø®µƒ±æ…Ì
	sendCardResetCardValue,
	sendCardFactoryReset,				// ’‚∏ˆ—°‘Ò «∞—À˘”–µƒø®µƒ≤Œ ˝ƒ¨»œ	

	sendcardnetworkchoosevalue,
	
	sendcardchoosevalue,
	vlinevalue,
	hlinevalue,
	vsizevalue,
	hsizevalue,
	choosecard12value,
	
	xpos_receive_value,
	ypos_receive_value,
	sendreceive_cardsetvalue,
	
	receivecard_number_value,
	
	sendcardVersionValue,
	receivecardVersionValue,
	
	savebrightnessvalue,
	setconnectvalue,
	
	networkchoosevalue,
	
	xoffsetvalue,
	yoffsetvalue,
	brightness,
	edidoutputvalue,
	edidreadvalue,	
	edidwritevalue,
	edidresetvalue,
	cascadevalue,	
	softvervalue,	
	sendtimeoutshow1value,
	sendtimeoutshow2value,
	sendtimeoutshow3value,
	
	RedColorTemSet_Value,
	GreenColorTemSet_Value,
	BlueColorTemSet_Value,
	ColorTemSet_Value,

	cardoutputFormatValue,	//20160413
	
	widthValue,
	heightValue,
	
	hlinevalue1,
	vlinevalue1,
	
	xposvaluea,
	yposvaluea,
	
	networkvalue1,
	networkvalue2,
	
	hlinevalue11,
	hlinevalue12,
	vlinevalue11,
	vlinevalue12,
	
	vsizevalue1,
	vsizevalue2,
	hsizevalue1,
	hsizevalue2,
	
	setconnectvalue1,
	setconnectvalue2,
	
	customerDevTypevalue,
	
	hdmiColorSpaceAdjustValue,

#if ENABLE_TEST_GRID_CONTROL
    GridPortValue,
	GridTestValue,
	gridBackgroudVaule,
	gridModeValue,
	gridColorVaule,
	gridHSpaceVaule,
	gridVSpaceVaule,
	gridHPosVaule,
	gridVPosVaule,	
	gridHWidthVaule,
	gridVHeightVaule,	
	gridDotValue,
	gridDotSpeedValue,
	gridDotColorValue,
	gridDotColorRValue,	
	gridDotColorGValue,
	gridDotColorBValue,	
	gridDotPosXValue,
	gridDotPosYValue,
#endif

	videoPanelFWSaveValue,
	usbUpdateConfirmValue,

	artNetStartAddrValue,
	artNetUniverseValue,

	totalWidthValue,
	totalHeighValue,
	firstDisWidthValue,
	firstDisHeighValue,
	secondDisWidthValue,
	secondDisHeighValue,
	thirdDisWidthValue,
	thirdDisHeighValue,
	fourDisWidthValue,
	fourDisHeighValue,

	mode4PTotalWidthValue,
	mode4PTotalHeighValue,
	mode4PFirstDisWidthValue,
	mode4PFirstDisHeighValue,
	mode4PSecondDisWidthValue,
	mode4PSecondDisHeighValue,
	mode4PThirdDisWidthValue,
	mode4PThirdDisHeighValue,
	mode4PFourDisWidthValue,
	mode4PFourDisHeighValue,

	h12TotalWidthValue,
	h12TotalHeighValue,
	h12FirstDisWidthValue,
	h12FirstDisHeighValue,
	h12SecondDisWidthValue,
	h12SecondDisHeighValue,
	h12ThirdDisWidthValue,
	h12ThirdDisHeighValue,
	h12FourDisWidthValue,
	h12FourDisHeighValue,

	v12TotalWidthValue,
	v12TotalHeighValue,
	v12FirstDisWidthValue,
	v12FirstDisHeighValue,
	v12SecondDisWidthValue,
	v12SecondDisHeighValue,
	v12ThirdDisWidthValue,
	v12ThirdDisHeighValue,
	v12FourDisWidthValue,
	v12FourDisHeighValue,

	ver12TotalWidthValue,
	ver12TotalHeighValue,
	ver12FirstDisWidthValue,
	ver12FirstDisHeighValue,
	ver12SecondDisWidthValue,
	ver12SecondDisHeighValue,
	ver12ThirdDisWidthValue,
	ver12ThirdDisHeighValue,
	ver12FourDisWidthValue,
	ver12FourDisHeighValue,

	hor12TotalWidthValue,
	hor12TotalHeighValue,
	hor12FirstDisWidthValue,
	hor12FirstDisHeighValue,
	hor12SecondDisWidthValue,
	hor12SecondDisHeighValue,
	hor12ThirdDisWidthValue,
	hor12ThirdDisHeighValue,
	hor12FourDisWidthValue,
	hor12FourDisHeighValue,
	
	h13TotalWidthValue,
	h13TotalHeighValue,
	h13FirstDisWidthValue,
	h13FirstDisHeighValue,
	h13SecondDisWidthValue,
	h13SecondDisHeighValue,
	h13ThirdDisWidthValue,
	h13ThirdDisHeighValue,
	h13FourDisWidthValue,
	h13FourDisHeighValue,

	h14TotalWidthValue,
	h14TotalHeighValue,
	h14FirstDisWidthValue,
	h14FirstDisHeighValue,
	h14SecondDisWidthValue,
	h14SecondDisHeighValue,
	h14ThirdDisWidthValue,
	h14ThirdDisHeighValue,
	h14FourDisWidthValue,
	h14FourDisHeighValue,

	v14TotalWidthValue,
	v14TotalHeighValue,
	v14FirstDisWidthValue,
	v14FirstDisHeighValue,
	v14SecondDisWidthValue,
	v14SecondDisHeighValue,
	v14ThirdDisWidthValue,
	v14ThirdDisHeighValue,
	v14FourDisWidthValue,
	v14FourDisHeighValue,


	userAlphaValue,
	mode4PSplitParamSendValue,
	//PipModeParamSendValue,
	FieldSplitParamSendValue,
	Horizontal2SplitParamSendValue,
	Vertical2SplitParamSendValue,
	two4kTakeModeSendValue,
	Horl2SplitParamSendValue,
	Verl2SplitParamSendValue,
	Horizontal3SplitParamSendValue,
	Horizontal4SplitParamSendValue,
	Vertical4SplitParamSendValue,

	
	
	//###################################################################
	// ‰≥ˆ∑÷±Ê¬ 
	outputFormatIndex,
	output800x600x60Value,
	output1024x768x60Value,
	output1280x720x60Value,
	output1280x720x50Value,
	output1280x768x60Value,
	output1280x800x60Value,
	output1280x1024x60Value,
	output1360x768x60Value,
	output1366x768x60Value,
	output1400x1050x60Value,
	output1440x900x60Value,
	output1600x1200x60Value,
	output1680x1050x60Value,
	output1920x1080x60Value,
	output1920x1080x50Value,
	output1920x1200x60Value,
	output2048x1152x60Value,
	output2560x812x60Value,
	//output2560x816x60Value,
	//output3840x1080x60Value,
	//output3840x2160x30Value,
	outputCustomValue,

	//###################################################################
	//EDID
	hdmi20PortHdmi20EdidValue,
	hdmi20PortHdmi20VideoEdidValue,
	hdmi20PortHdmi20NoHfvsdbNo420, 		
	hdmi20PortHdmi20NoHfvsdbWith420, 	
	hdmi20PortHhdmi20WithHfvsdbNo420, 	
	hdmi20PortHdmi20WithHfvsdbNo420Hdr, 
	hdmi20PortHdmi20WithHfvsdbWith420, 	
	hdmi20PortHdmi14EdidValue,
	hdmi20PortHdmi13EdidValue,
	hdmi14PortHdmi14EdidValue,
	hdmi14PortHdmi13EdidValue,
	dpPortDpEdidValue,
	dpPort4k2k60Value,
	dpPort4k2k30Value,
	dviPortDualLinkDviEdidValue,
	dviPortDviEdidValue,
	dviPortHdmi13EdidValue,

	hdmi20PortCustomEdidValue,
	hdmi14PortCustomEdidValue,
	dpPortCustomEdidValue,
	dviPortCustomEdidValue,

	//###################################################################
	//Alpha
	displayAlphaValue,

	outputPort0Value,
	outputPort1Value,
	outputPort2Value,
	outputPort3Value,

	outputPort0TpyeValue,
	outputPort0BitWidthValue,
	outputPort0RangeValue,

	outputPort1TpyeValue,
	outputPort1BitWidthValue,
	outputPort1RangeValue,

	outputPort2TpyeValue,
	outputPort2BitWidthValue,
	outputPort2RangeValue,

	outputPort3TpyeValue,
	outputPort3BitWidthValue,
	outputPort3RangeValue,

	
	layerAHSizeValue,
	layerAVSizeValue,
	layerAHPosValue,
	layerAVPosValue,
	layerASplitStartValue,
	layerACropStartValue,
	layerAFineValue,

	
	layerBHSizeValue,
	layerBVSizeValue,
	layerBHPosValue,
	layerBVPosValue,
	layerBSplitStartValue,
	layerBCropStartValue,
	layerBFineValue,

	
	layerCHSizeValue,
	layerCVSizeValue,
	layerCHPosValue,
	layerCVPosValue,
	layerCSplitStartValue,
	layerCCropStartValue,
	layerCFineValue,
	
	
	layerDHSizeValue,
	layerDVSizeValue,
	layerDHPosValue,
	layerDVPosValue,
	layerDSplitStartValue,
	layerDCropStartValue,
	layerDFineValue,

	layerAResetValue,
	layerBResetValue,
	layerCResetValue,
	layerDResetValue,

	_4pLTSourceValue,
	_4pLTXValue, 	
	_4pLTYValue, 	
	_4pLTWValue, 	
	_4pLTHValue, 	
	_4pLTSendValue,

	_4pLBSourceValue,
	_4pLBXValue, 	
	_4pLBYValue, 	
	_4pLBWValue, 	
	_4pLBHValue, 	
	_4pLBSendValue,

	_4pRTSourceValue,
	_4pRTXValue, 	
	_4pRTYValue, 	
	_4pRTWValue, 	
	_4pRTHValue, 	
	_4pRTSendValue,

	_4pRBSourceValue,
	_4pRBXValue, 	
	_4pRBYValue, 	
	_4pRBWValue, 	
	_4pRBHValue, 	
	_4pRBSendValue,

	outPutDvi1Value,
	outPutDvi2Value,
	outPutDvi3Value,
	outPutDvi4Value,
	
	pipSubWinPosXValue,
	pipSubWinPosYValue,
	pipSubWinSizeXValue,
	pipSubWinSizeYValue,

	pipMainWinPosXValue,
	pipMainWinPosYValue,
	pipMainWinSizeXValue,
	pipMainWinSizeYValue,

    maxSizeValue
};

typedef enum _STEP_STATE
{
	STEP_STATE_1,
	STEP_STATE_10,
	STEP_STATE_100,
} StepStateType;


int getValue(int index);
int setValue(int index, int value);
const char * valueToString(int index, int value);
const char * dispInputFormatByCh(int ch,int format);
int nextValue(int index, int value);
int prevValue(int index, int value);
int getUpLimit(int index);
void clearStep(void);

void setHotBackupDisable(int source);
void resetHotBackupEnable(void);
void refleshHotBackupEnable(void);

void checkEDIDDetinationList(void);

typedef enum
{ 
	INPUT_AUDIO_1,
	INPUT_AUDIO_2,
	INPUT_AUDIO_3,
	INPUT_AUDIO_MAX_SIZE,
}INPUT_AUDIO_TYPE;

void refleshAudioInputEnable(char index);
void refleshAudioInputEnable_total(void);
INPUT_AUDIO_TYPE getAudioInputConfig(char source);

void drawLEDConnectIcon(int column, int row,char index, char fb);
void displayLEDConnectIcon(int focus);


#endif // MENUDATA_H
