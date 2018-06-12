#include "SysConfig.h"

#include "langStr.h"

const char * const _langStr_En[] =
{
	{"Init Network" 						},	// InitNetWorkStr
	{"Art Net Test" 						},	// artnetTestStr
	{"Finish Network!" 						},	// FinalNetWorkStr
	{"Init Device"  						},	// initDeviceStr
	{" "									},	// spaceStr
	{" >VSP 330" 							},	// Menu0Line1Str
	{"  Lanuage Select:"					},	// langLine1Str
	{"  English   中文"						},	// langLine2Str
	{"Save Setup"							},	// saveSetupStr
	{"Backup"								},	// backupStr	
	{"Save Setup"							},	// saveToStr
	{"Load Setup"							},	// loadFromStr
	{"Save all to USB"						},	// usbAllSaveStr
	{"Load single from USB"					},	// usbLoadSingleStr,
	{"Load all from USB"					},	// usbLoadAllStr,	
	{"Save Fail"					        },	// saveFailStr
	{"Load Fail"					        },	// loadFailStr	
	{"Finished!"							},	// finishedStr	
	{"SUN"									},	// sundayStr
	{"MON"									},	// mondayStr
	{"TUE"									},	// tuesdayStr
	{"WED"									},	// wednesdayStr
	{"THU"									},	// thursdayStr
	{"FRI"									},	// fridayStr
	{"SAT"									},	// saturdayStr
	{"  TIME      CALENDAR"					},	// timeCalendarStr
	{"Great Viwe"							},	// ydsStr
	{"STRAIGHT MODE"						},	// layoutSingleStr
	{"H 1/2 Mode"							},	// layoutLeftRightStr
	{"V 1/2 Mode"							},	// layoutUpDownStr
	{"  Setup H 1/4 MODE"					},	// layout4cloumnStr
	{"  Setup V 1/4 MODE"					},	// layout4rowStr
	{"  Setup TP Mode:"						},	// setTPmodeStr
	{"  Scale     Advance"					},	// scaleAndAdvanceStr
	{"White"								},	// whiteStr
	{"Yellow"								},
	{"Sky Blue"								},
	{"Green"								},
	{"Pink"									},
	{"Red"									},
	{"Blue"									},
	{"Black"								},
	{"Output Detall"						},
	{"Output Format"	    				},	// outputFormatStr
	{"Output Adjust"	    				},
	{"Scale"           						},	// scaleStr
	{"Scale Mode"							},	// scaleModeStr	
	{"Auto Fit"								},	// autoFitStr		
	{"Press Menu To Exit"	    			},
	{" "	   								},	// companyInfoStr
	{"COM Baudrate"	    					},
	{"Off" 									},	//offStr
	{"On" 									},	//onStr	
	{"TP Mode" 								},
	{"TP Color" 							},
	{"TP Roll" 								},
	{"TP Fore" 								},
	{"TP Back" 								},
	{"Loop Out" 							},
	{"DE ON/OFF" 							},
	{"DE H Start" 							},
	{"DE V Start" 							},
	{"DE Width" 							},
	{"DE Height"							},
	{"  Range     HDMI/DVI" 				},
	{"X Setup" 								},
	{"Y Setup" 								},
	{"HDMI/DVI" 							},	// hdmiDviStr
	{"HDMI/SDI"								},	// hdmiSdiStr,
	{"SDI/DVI"								},	// sdiDviStr,
	{"SDI/HDMI"								},	// sdiHdmiStr,
	{"DVI/HDMI"								},	// dviHdmiStr,
	{"DVI/SDI"								},	// dviSdiStr,	
	{"_update.pkg"							},	// updateFile1Str,
	{"update_.pkg"							},	// updateFile2Str,	
	{"Upgrade Firmware..."					},	// updateFirmwareStr
	{"USB Upgrade Firmware..."				},	// usbUpdateFirmwareStr	
	{"  Please Wait..." 					},	// pleaseWaitStr
	{"Upgrade Success!" 					},	// updateFirmwareSuccessStr
	{"Upgrade Fail!" 						},	// updateFirmwareFailStr
	{"Please Wait Restart!"					},	// pleaseRestartStr
	{"Please Try Again!" 					},	// pleaseTryAgainStr
	{"Source Select:" 						},	// inputSourceSelectStr
	{"  Set Freeze Mode" 					},
	{"  Set Live Mode" 						},
	{"Brightness" 							},	// brightnessStr
	{"Contrast" 							},	// contrastStr
	{"Color Effect" 						},	// colorEffectStr
	{"Sharpness" 							},	// sharpnessStr
	{"Saturation" 							},	// saturationStr
	{"Red Level" 							},	// colorTempRedStr
	{"Green Level" 							},	// colorTempGreenStr
	{"Blue Level" 							},	// colorTempBlueStr
	{"Color Temperature" 					},	// colorTempStr,
	{"2800 K" 								},	//Color2800KStr,
	{"3200 K" 								},	//Color3200KStr,
	{"3500 K" 								},	//Color3500KStr,
	{"4000 K" 								},	//Color4000KStr,
	{"4500 K" 								},	//Color4500KStr,
	{"5000 K" 								},	//Color5000KStr,
	{"5600 K" 								},	//Color5600KStr,
	{"6000 K" 								},	//Color6000KStr,
	{"6500 K" 								},	//Color6500KStr,
	{"8000 K" 								},	//Color8000KStr,	
	{"Full RGB" 							},	//fullRGBStr,	
	{"Volume" 								},					
	{"  Picture Mode:" 						},
	{"Upgrade Main Board" 					},	// updateMainBoardStr
	{"Please Press Sel" 					},
	{"  Screen Pos X:" 						},
	{"  Screen Pos Y:" 						},
	{"  Screen Width:"						},
	{"  Screen Height:" 					},
	{"Auto Scale"							},	// windowRatioStr	
	{"Picture"								},	// pictureStr
	{"4:3"									},	// ratio_4_3_str
	{"16:9" 								},	// ratio_6_9_str
	{"Fit to Screen" 						},	// ratio_normal_str
	{"  No File" 							},	// noFileStr
	{"  TP ON/OFF:" 						},
	{"  Swap:" 								},
	{"  Top Black:"							},
	{"Image"								},
	{"Video" 								},
	{"Data Range" 							},
	{"DE Adjust" 							},
	{"Reset"                 				},	// resetStr
	{"Default"								},	// defaultStr
	{"Video Input"							},	// videoInputStr	
	{"Input"                  				},	// inputStr
	{"PGM"                   				},	// programStr,
	{"PRE"                   				},	// previewStr,
	{"Program"                   			},	// programStr1,
	{"Preview"                   			},	// previewStr1,
	{"Pre"                   			    },	// preStr,
	{"Post"                   			    },	// postStr,	
	{"Output Format"                 		},	//outputStr
	{"Format"                 				},
	{"System"                 				},	//systemStr
	{"Device Info"            				},	//systemInfoStr
	{"Advanced"            					},	//advancedStr
	{"Date&Time"             				},
	{"Ethernet"               				},	// ethernetStr
	{"Network"                				},
	{"Front Panel Lock"       				},	// lockPanelStr
	{"Lock Password"						},	// lockPanelPasswordStr 	
	{"Llcense Setup"       					},
	{"Can Use Time"        					},
	{"No Limit"        						},
	{"Change Password"        				},
	{"Input New Password"     				},	// inputNewPasswordStr
	{"Input Password Again"   				},	// inputPasswordAgainStr
	{"Two Passwords"    					},	// twoPasswordsStr
	{"Do Not Match"   						},	// doNotMatchStr
	{"Success"    							},	// successStr
	{"Enter Password"						},	// enterPasswordStr	
	{" Enter Password:"	    				},	// passwordStr
	{"Password Error"	    				},	// passwordErrorStr
	{"Retype Password"	    				},	// retypePasswordStr
	{"License State"        				},
	{"Change Time"        					},
	{"Change Date"        					},
	{"Input Detail"          				},	// inputDetailStr
	{"Auto"									},	// autoStr
	{"H Size"              					},	// hSizeStr
	{"V Size"              					},	// vSizeStr
	{"H/V Size"              				},	// HVSizeStr
	{"H Position"              				},	// hPosStr
	{"V Position"              				},	// vPosStr
	{"Position Settings"				    },	//positionSettStr	
	{"Offset Settings"                      },  // offsetSettingsStr	
	{"Port 1/2 Offset X"              		},	// port12hOffsetStr,
	{"Port 1/2 Offset Y"              		},	// port12vOffsetStr,
	{"Port 3/4 Offset X"              		},	// port34hOffsetStr,
	{"Port 3/4 Offset Y"              		},	// port34vOffsetStr,
	{"VGA Adjust"             				},
	{"DVI/EXT Adjust"   					},
	{"CLOCK"              					},
	{"Phase"              					},
	{"Auto Adjust"            				},
	{"Screen Parameters"     				},
	{"Mode"             					},
	{"Full Size"             				},
	{"Screen Size"            				},
	{"Port"           						},
	{"DE"            						},
	{"DVI"            						},	// dviStr
	{"HDMI"            						},	// hdmiStr
	{"HD-SDI"								},	// sdiStr
	{"DVI Mode"            					},				
	{"DE H Pos"              				},
	{"DE V Pos"              				},
	{"DE H Size"              				},
	{"DE V Size"              				},
	{"Gamma"           						},	// gammaStr
	{"Transition"            				},	// transitionStr
	{"Fade Type"							},	// fadeTypeStr		
	{"Fade Time"            				},	// fadeTimeStr 
	{"Alpha"           						},	// alphaStr
	{"Dissolve"            					},
	{"Cut" 									},
	{"Wipe Square In"          				},
	{"Wipe Square Out"      				},
	{"Wipe T_L In"  						},
	{"Wipe T_L"           					},
	{"Wipe T_R In"       					},
	{"Wipe T_R"          					},
	{"Wipe B_L In"     						},
	{"Wipe B_L"        						},
	{"Wipe B_R In"    						},
	{"Wipe B_R"       						},
	{"Wipe Left In"           				},
	{"Wipe Left"            				},
	{"Wipe Right In"          				},
	{"Wipe Right"            				},
	{"Wipe Top In"        					},
	{"Wipe Top"            					},
	{"Wipe Bottom In"         				},
	{"Wipe Bottom"            				},
	{"   Black Out"            				},	// noneStr
	{"  Live Image"           				},	// liveStr
	{"Freeze Image"           				},	// freezeStr
	{"  Pure Image"           				},	// flatStr
	{"Test Pattern"           				},	// tpSwitchStr	
	{"Test Pattern"           				},	// testPatternStr
	{"Auto Switch"            				},	// autoTpTimeStr
	{"HOR Step"           					},
	{"VER Step"            					},
	{"Color"            					},	// tpStepColorStr
	{"PIP"           						},	// pipStr
	{"Input"								},	// pipSourceStr	
	{"Top Window"           				},	// topWindowStr
	{"Swap"		            				},	// swapWindowStr
	{"Layout"            					},	// layoutStr
	{"Select"           					},	// selectStr									
	{"TEXT OVERLAY"            				},
	{"PRESET"            					},
	{"BLEND MODE"            				},
	{"BLEND LEVEL"            				},
	{"ABOVE/BELOW"            				},
	{"KEY IN/OUT"            				},	//"与／或"},
	{"USER MODE"            				},		
	{"PIP L+T"            					},	// pipLTStr
	{"PIP R+T"            					},	// pipRTStr,
	{"PIP L+B"            					},	// pipLBStr,
	{"PIP R+B"            					},	// pipRBStr,			
	{"PIP Cent"            					},	// pipCntStr
	{"PBP L+R"            					},	// pbpLRStr
	{"PBP T+B"            					},	// pbpTBStr
	{"Image A"            					},	// imageAStr
	{"Image B"            					},	// imageBStr
	//{"LANG"           					},	//languageStr
	{"语言/LANG"           					},	//languageStr
	{"ENG"            						},
	{"中文"            						},
	{"Display Mode"           				},
	{"Pure Color"            				},
	{"Audio"            					},
	{"Mute"            						},
	{"Audio In"            					},
	{"Internal"            					},
	{"External"           					},
	{"Factory Reset"           				},	// factoryResetStr
	{"Reset IP Address"         			},	// saveIpResetStr
	{"No"           						},	// noStr
	{"Yes"          						},	// yesStr
	{"Yes <Knob>, No <Menu>"				},	// yesOrNoStr
	{"Tech Support"							},	// techSupportStr
	{"SALES :" 								},
	{"	4008-592-114"						},		
	{"CUSTOMER SERVICE:"					},
	{"	4008-592-315"						},
	{"TEL:	   +86(0592)5771197"			},
	{"EMAIL:   rgblinkcs@gmail.com"			},
	{"WEBSITE: www.rgblink.com	  "			},
	{"IP"									},
	{"Press Menu buttons for 5s"			},//pressMenuKeyStr
	{"unlock button!"   					},	// toUnlockPanelStr
	{"Button are unlocked" 					},	// unLockPanelStr
	{"ADC Adjust"							},
	{"ADC Auto Adiust"						},
	{"ADC Reset All"						},
	{"Invalld Operation"					},
	{"In PIP Mode"							},
	{"Mode"									},
	{"Lock"									},
	{"Free Run"								},
	{"Frame Lock 1"							},
	{"Frame Lock 2"							},
	{"Date"									},
	{"Time"									},
	{"Work Time"							},
	{"Total Time"							},
	{"Boot Times"							},
	{"Timing Control"						},
	{"hours"								},
	{"minutes"								},
	{"Delay Call"							},
	{"Below"								},
	{"Above"								},
	{"Out"									},
	{"In"									},
	{""										},
	{"Sync"									},
	{"Black Out"							},  // blackScreenStr
	{"RGB444"								},
	{"YUV444"								},
	{"YUV422"								},
	{"Color Space"							},
	{"Deinterlace"							},	// interlaceSwitchStr
	{"IP Address"							},	// setIpAddressStr,	
	{"Remote Ip Address" 	 				},	// setRemoteIpAddressStr,
	{"Local Ip Address" 	 				},	// setLocalIpAddressStr,
	{"Subnet Mask"							},	// setSubnetMaskStr,
	{"Gateway"								},	// setGatewayStr,
	{"DHCP"									},	//dhcpEnableStr,
	{"VLOCK"								}, 	// vLockLineStr
	{"Preview Split",						},	//splitStr
	{"Four Port Split",						},	//FourPortSplitStr
	{"MIMO",								},	//MIMOStr
	{"Independent Mode",					},	//independentModeStr
	{"Two In Three Out",					},	//twoInThreeOutSrt
	{"Two In Four Out",						},	//twoInFourOutSrt
	{"Three In Three Out",					},	//threeInThreeOutSrt
	{"Three In Four Out",					},	//threeInFourOutSrt
	{"Four In Four Oout",					},	//fourInFourOutSrt
	{"H Total"								},
	{"V Total"								},
	{"H Pos"              					},
	{"V Pos"              					},
	{"H Size"              					},
	{"V Size"              					},
	{"Standard"								},//standardFormatStr,
	{"OTther"								},//otherFormatStr
	{"Customized"							},//customFormatStr,
	{"Button Knob can be saved      "		},	// saveToPageTipsStr1,
	{"Button Menu will be quit"				},	// saveToPageTipsStr2,
	{"Button Knob is ready for Load  "		},	// loadFromPageTipsStr1,
	{"Button Menu will be quit"				},	// loadFromPageTipsStr2,
	{"Save "								},	// saveToRecoededWarnStr_beforeNum,
	{" has been used,overwrite?"			},	// will you overwrite,saveToRecoededWarnStr_afterNum,
	{"  Push the red button to       "		},	// guideSetNumberStr1
	{"  enter numbers                "		},	// guideSetNumberStr2
	{"  set input source             "		},	// guideSetNumbersToSourceStr
	{"  Please select value          "		},	// guideChooseItemStr1,
	{"  Push the rotary to set       "		},	// guideChooseItemStr2,
	{"Push button flashs to save     "		},	// guideSaveStr1,
	{"after power on,now it flashs   "		},	// guideSaveStr2,
	{"User mode 1 is default recall  "		},	// guideSaveToOneTipsStr,
	{"Buttons are locked!"					},	// lockPanelTipsStr,
	{"Beyond The Supported Range!    "		},	// beyondCustomFormatStr,
	{"When you activate Image Enhance"		},	// imageEnhanceWarnStr1
	{"function,transitions will not"		},	// imageEnhanceWarnStr2
	{"work.Would you like to active?"		},	// imageEnhanceWarnStr3	
	{"Image Enhance Enable!"				},	// imageEnhanceEnableStr,
	{"PLEASE SET LANGUAGE/语言:"			},	//guideSetLanguageStr,
	{"ENGLISH"								},	//guideEnglishStr
	{"Please Set Output Format: "			},	//guideSetOutputFormatStr
	{"Please Set Screen Width: "			}, 	//guideSetScreenWidthStr
	{"Please Set Screen Height: "			}, 	//guideSetScreenHeightStr
	{"Please Set Screen X Pos: "			},	//guideSetScreenXPosStr
	{"Please Set Screen Y Pos: "			},	//guideSetScreenYPosStr
	{"Please Set Input Source: "			},	//guideSetSourceStr
	{"Please Save The Settings "			},	//guideSetSaveStr,
	{"Please Save To User Mode 1 "			},	//guideSaveToOneStr,
	{"Adjust"								},	//adjustStr
	{"Current Time"							},	//currentTimeStr,
	{"Last Time"							},	//lastTimeStr,
	{"Color Adjustments"					},	//imageQualityStr
	{"Zoom/Crop Image"      				},	// zoomStr
	{"Top"									},	//zoomNewVUpStr,
	{"Bottom"								},	//zoomNewVDownStr,
	{"Top/Bottom"	          				},	//zoomNewVUpAndDownStr,
	{"Left"	          						},	//zoomNewHLeftStr,
	{"Right"          						},	//zoomNewHRightStr,
	{"Left/Right"	        				},	//zoomNewHLeftAndRightStr,
	{"Center"	          					},	//zoomNewCenterStr,
	{"VGA SIGNAL TYPE"       	   			},	//vga1KeyModeStr,
	{"VGA2 SIGNAL TYPE"	      				},	//vga2KeyModeStr,
	{"VGA"	          						},	//vgaStr,
	{"YPbPr"	          					},	//ycbcrStr,
	{"Fade Time"							},	//FadeTimeStr
	{"OLED Brightness"						},	//luminanceStr	
	{"H POLARITY"                 			},	//hPolarityStr
	{"V POLARITY"                 			},	//vPolarityStr
	{"POSITIVE"                   			},	//positiveStr
	{"NEGATIVE"                   			},	//negativeStr
	{"Image Enhance"              			},	//ImageEnhanceStr
	{"BIT DEPTH"							},	//bitDepthStr
	{"8 BIT"					    		},	//bit_8Str
	{"10 BIT"					    		},	//bit_10Str
	{"12 BIT"					    		},	//bit_12Str
	{"USB UPGRADE"                  		},
	{"COPY CODE TO DEVICE"   		 		},
	{"REBOOT AFTER COPY"	  		 		},
	{"ANTI-ALIAS"							},	//ttlInterlaveModeStr
	{"WAITING"								},	//"WAIT AND LOAD"
	{"    DO NOT UPGRADE UNLESS      "		},	//usbUpgradeWarnTipStr1,
	{"    CONNECT TO COMPUTER!!!     "		},	//usbUpgradeWarnTipStr2,
	{"HOT BACKUP"                 			},	//hotBackUpStr
	{"Backup_1"								},	//hotBackUpClass1Str,
	{"Backup_2"								},	//hotBackUpClass2Str,
	{"Backup_3"								},	//hotBackUpClass3Str,
	{"Backup_4"								},	//hotBackUpClass4Str,
	{"Backup_5"								},	//hotBackUpClass5Str,
	{"INVALID OPERATION IN PIP MODE"		},	//invalidFunctionInPIPStr,
	{"NOT SET! "							},	//recordFreeStr
	{"HDMI ADJUST"   						},	//HdmiAdjustStr
	{" CHANGE WILL NOT TAKE EFFECT   "		},	//changeNotTakeEffectStr
	{"        UNTIL RESTART          "		},	//reStartPleaseStr,
	{"Software Version"						},	// softVersionStr,
	{"Hardware Version"						},	// hardVersionStr,
	{"  SN"           						},	// snStr
	{"ENTER SN NUMBER"           			},	//WriteSnStr,
	{"VERSION"                    			},	// versionStr
	{"Firmware Soft  Version"				}, 	// mcuVersionStr
	{"Video    Soft  Version"           	},	// videoVersionStr		
	{"COMMBOARD VERSION"          			},	// commBoardVersionStr	
	{"Mainboard Hard Version"           	},	// mainBoaedVersionStr,
	{"FPGA     Soft  Version"           	},	// fpgaVersionStr,	
	{"ExtBoard1 Hard Version"			    },	// extMainBoardVersionStr,
	{"ExtBoard2 Hard Version"			    },	// ext2MainBoardVersionStr,
	{"ExtBoard1 Soft Version"			    },	// ext1MainBoardSoftVersionStr,
	{"ExtBoard2 Soft Version"			    },	// ext1MainBoardSoftVersionStr,
	{"WIFI&LED VERSION"						},	// wifiControl_VersionStr
	{"EDID Management"        					},	//edidManageStr,
	{"EDID Source"        					},	//sourcePortStr,
	{"DETINATION PORT"     					},	//destinationPortStr,
	{"USB CONNECTING..."					},	// usbconnectStr
	{"PORT A"         						},	//usbPortAStr,
	{"PORT B"         						},	//usbPortBStr,
	{"PHOTO TIME"							},	//usbPhotoTimeStr,
	{"USB MODULE IS NOT INSTALLED"			},	//noUSBBoardStr
	{"CONTACT THE SALES TO PURCHASE"		},	//contactSalesStr,
	{"     Copy EDID Success       "		},	//studyEDIDSuccessStr,
	{"     Copy EDID Fail          "		},	//studyEDIDFailStr,
	{"  Edit EDID"							},	//edidEditingStr
	{"  PLease do not shut down      "		},	//pleaseDoNotShautDownStr
	{"  G3 NET will auto reboot "			},	//pleaseWaitAutoRebootStr
	{"FAN CONTROL"            				},	//fanControlStr,
	{"AUTO SPEED"              				},	//automaticSpeedControlStr,
	{"CURRENT SPEED"           				},	//fanSpeedStr,
	{"CURRENT TEMPERATURE"    				},	//CurTempStr
	{"Temperature"							},	// temperatureStr		
	{"DEVICE STATUS"           				},//deviceStatusStr,
	{"VIDEO STATUS"             			},//videoProcessStatusStr,
	{"MB STATUS"                			},//mbProcessStatusStr,
	{"EXT STATUS"              				},//extProcessStatusStr,
	{"UNCONNECTED"              			},//unnormalStr,
	{"NORMAL"                	  			},//normalStr,
	{"INVALID OPERATION"					},
	{"IN SPLIT MODE"						},		
	{"Customized Button"    				},	//userDefinedButtonStr
	{"TP"									},	// mapKeyStr
	{"BLACK"								},	//blackZeroStr,
	{"BLACK"								},	//blackDisplayModeStr,
	{"Freeze"								},	//freezeDisplayModeStr,
	{"Logo Na"								},	//logoNaModeStr,		
	{"TP"									},	//tpDisplayModeStr,
	{"BRIGHTNESS"							},	//brightnessDisplayModeStr,
	{"EXTENDED MODULE IS NOT INSTALLED"		},	//noExternedModuleStr
	{"CVBS MODULE IS NOT INSTALLED"			},	//noCVBoardStr
	{"EXT CVBS CONTROL"         			},	//extCVBSControlStr
	{"CVBS PORT"         					},	//extCVBSPortStr	
	{"PORT A"         						},	//extCVBSPortAStr,
	{"PORT B"         						},	//extCVBSPortBStr,
	{"Input Format"         				},	//inputFormatStr
	{"No Input"        						},	//noInputStr	
	{"EXT VGA ADJUST"         				},	//extVGAAdjustStr	
	{"REAL-TIME SAVE"        				},	//saveRealTimeStr		
	{"Fixed Split" 							},	//fixedSplitStr,
	{"Normal Split"							},	//normalSplitStr,	
	{"Split Type"							},	//splitTypeStr,
	{"Horizontal_1/2"						},	//Horizontal2Str,
	{"Horizontal_1/3"						},	//Horizontal3Str,
	{"vertical_1/2" 						},	//vertical2Str,
	{"Field Split", 						},	//fieldSplitStr,
	{"Horizontal_1/4"						},	//Horizontal4Str,
	{"vertical_1/4" 						},	//vertical4Str,
	{"Split Index"							},	//splitIndexStr,	
	{"Horizontal Split"						},	//HorizontalNumberStr,
	{"Vertical Split"						},	//VerticalNumberStr,
	{"Deivce is in external control" 		},	//deviceInSerialControlStr,
	{"Device is under remote control"		},	// deviceInRemoteControlStr	
	{"Front panel buttons are locked"		},	//lockFrontPanelStr,
	{"LOADING USB "							},	//readFilesFromUSBStr
	{"FILES  "								},	//readFilesFromUSBStr2,
	{"Double-tap To Switch" 				},	// PreviewDblTapSwitchStr
	{"Preview Output" 						},	// PreviewSwitchStr
	{"Preview DVI Port"						},	// PreviewDVISwitchStr,	
	{"SWITCH TYPE" 							},	//switchTypeStr
	{"SMIPLE VIDEO" 						},	//simpleVideoStr,
	{"COMPLEX VIDEO"						},	//complexVideoStr,	
	{"English"			    				},	//englishLongStr										
	{"INVALID OPERATION IN PIP MODE"		},	//invalidOpInPIPModeStr
	{"INVALID OPERATION IN SPLIT MODE"		},	//invalidOpInSPLITModeStr
	{"SPLID IS INVALID IN PIP MODE"			},	//SplitInvalidInPIPModeStr
	{"LOADING" 								},	//collectingSignalStr,
	{"Finished" 							},	//acquisitionCompleteStr,				
	{"EXT CONFIG" 						   	},	//extInputConfigStr
	{"DVI/EXT AUDIO"   				    	},	//dviAndAudioStr
	{"AUDIO TYPE"							},	//audioTypeStr
	{"NO MODULE"  							},
	{"AUDIO MODULE" 						},
	{"Loading" 								},	//operationReadyTipStr,
	{"Finished"								},	//operationReadyFinishedTipStr,		
	{"Scale"								},	//scalePauseStr,
	{"Take"									},	//takeStr,
	{"      NO SIGNAL NEED TAKE      "		},	//noSignalNeedTakeStr
	{"ONLY DVI_5/MODULE_9"					},	//splitSourceLimitTipStr1,
	{"SUPPORT SPLICING"						},	//splitSourceLimitTipStr2,
	{"ONLY DVI_5/MODULE_HDMI"				},	//splitSourceLimitTipStr3,
	{"       IN TRANSITION            "		},	//inTransitionStr
	{"       TRANSTION FINISHED       "		},	//transitionFinishedStr
	{"MODULE INPUT IMAGE EXHANCE"			},	//inputSDExhanceStr	
	{"NO THIS MODULE!"						},	//noThisModuleStr,
	{"SKIP!"								},	//upgradeSkipStr,
	{"DVI1 OUutput"							},	//dvi1OutputStr
	{"Output" 								},	//outputNormalStr
	{"DVI_LOOP" 							},	//outputLoopStr
	{"HDMI IN"								},	//dvi1InputStr
	{"HDMI/EXT AUDIO"						},	//hdmiAndExtAudioStr
	{"ONLY MB_V1.3 SUPPORT"					},	//mainBoardNotSupportStr,
	{"SCREEN FOLLOW SCALE"					},	//screenFollowStr
	{"LOADING PLAY LIST"					},	//readUSBPlayListStr
	{"Input Audio Config"					},	//audioInputConfigStr,
	{"Input Audio 1"						},	//audioInput1Str,
	{"Input Audio 2"						},	//audioInput2Str,
	{"Input Audio 3"						},	//audioInput3Str,
	{"No Ext"								},	//noExtTipStr
	{"Through"								},	//throughModuleStr
	{"HDMI/EXT Adjust"						},	//hdmiAndExtAdjustStr,	
	{"Press Key"   							},	//pressKeyStr
	{"Knob"									},	//knobKeyStr
	{"Key Test"   		    				},	//keyTestOnStr				
	{"Advance"   		    				},	//advanceStr
	{"Port Tutorial"   		    			},	// portTutorialStr
	{"Set Width"   		    			    },	// sendCardSetWidthStr,
	{"Auto"   		    				    },	// sendCardAutoStr,
	{"Manual"   		    				},	// sendCardManualStr,
	{"Auto Set Width(pixels):"   	        },	// autoWidthParamStr
	{"Auto Set Height(pixels):"   	        },	// autoHeightParamStr	
	{"Wall Size:"   		    			},	// wallSizeStr,
	{"Panel Pixel Count:"   		    	},	// panelPixelStr,
	{"Wall Size W*H:"   		    		},	// wallSizeStr1,
	{"Panel Pixel W*H:"   		        	},	// panelPixelStr1,		
	{"Total Pixel Res:"   		            },	// totalPixelStr,	
	{"LED Display"   		    			},	// sendCardConfigStr
	{"Regular"   		    				},	// sendCardNormalStr,	inerratic Normal
	{"Unregular"   		    				},	// sendCardAbNormalStr,		irregular
	{"Port Set"   		    				},	// sendCardPortSetStr	
	{"Send Card Set"   		    			},	// sendCardCommonSetStr	
	{"Port"   		    					},	// sendCardPortSelStr
	{"H Panels"   		    				},	// sendCardHPanelsStr,	
	{"V Panels"   		    				},	// sendCardVPanelsStr	
	{"LEDs/Row"   		    				},	// sendCardHLedsStr
	{"LEDs/Column"   		    			},	// sendCardVLedsStr	
	{"H Position"							},	// sendCardPortHOffsetStr	
	{"V Position"							},	// sendCardPortVOffsetStr	
	{"Port 1 X Position"					},	//sendCardPortH1OffsetStr,	
	{"Port 1 Y Position"					},	//sendCardPortV1OffsetStr,
	{"Port 2 X Position"					},	//sendCardPortH2OffsetStr,	
	{"Port 2 Y Position"					},	//sendCardPortV2OffsetStr,
	{"Port 3 X Position"					},	//sendCardPortH3OffsetStr,	
	{"Port 3 Y Position"					},	//sendCardPortV3OffsetStr,
	{"Port 4 X Position"					},	//sendCardPortH4OffsetStr,	
	{"Port 4 Y Position"					},	//sendCardPortV4OffsetStr,	
	{"Panel"								},	// sendCardPortPanelStr
	{"Panel No."							},	// sendCardPanelNoStr
	{"Save To Port"							},	// sendCardSendToPortStr
	{" Save To Port Success   "				},	// sendCardSendToPortOkStr,
	{" Save To Port  Fail     "				},	// sendCardSendToPortFailStr,
	{"Save To Panel"						},	// sendCardSendToPanelStr
	{"Reset Port Set"						},	// sendCardResetPortStr
	{"Connection Type"						},	// sendCardPortConectTypeStr	
	{"LED MANAGEMENT"     					},	//LEDScreenManageStr
	{"QUICK CONNECTION"						},	//quickPointScreenStr
	{"DISPLAY CONNECTION"					},	//displayConnectionStr
	{"ADVANCED"								},	//seniorstr
	{"SENDING CARD TYPE"					},	//sendcardtypeStr
	{"CASCADE"		   						},	//cascadeStr
	{"RECEIVING CARD SEPARATE"				},	//receivingCardSeparateSetting,
	{"COLORLIGHT"							},	//sendCardType_Colorlight,
	{"LINSN"								},	//sendCardType_Linsn,
	{"NOVA STAR" 							},	//sendCardType_NovaStar,
	{"SENDING CARD SET"						},	//sendcardadjusts	
	{"Brightness"		   		  			},  // brightness_sendcardstr
	{"SENDING CARD POSITION"				},
	{"Gamma"								},	//gamma_sendcard_str
	{"DISPLAY CONNECTION"					},
	{"Set" 									},
	{"X"									},	//hPosStr1
	{"Y"									},	//vPosStr1
	{"Width"								},	//hSizeStr1
	{"Height"								},	//vSizeStr1
	{"SENDING CARD MODEL"					},
	{"PARAMETER SETTING"					},				                    
	{"HORIZONTAL CARD" 						},	//vlinestr
	{"VERTICAL CARD" 						},	//hlinestr
	{"Width" 								},	//vsize_str
	{"Height"								},	//hsize_str
	{"CONNECTION MODE"	 		   			},	//screenconnectstr
	{"RECEIVING CARD POSITION"    			},	//receivecardstr									
	{"SENDING CARD NO. 1/2"					},	//choosecard12str
	{"POSITION SETTING"						},	//positon_str									
	{"RECEIVING CARD NO."					},	//receivecard_number_str									
	{"CONTROL SYSYTEM VERSION"    			},	//versionstr									
	{"SENDING CARD VERSION"					},	//sendcardinfostr
	{"RECEIVING CARD VERSION"				},	//receivecardinfostr
	{"SAVE BRIGHTNESS"		   				},	//savebrightnessstr
	{"PORT D"			  					},	//networkastr
	{"PORT U"			  					},	//networkbstr
	{"CHOOSE CABLE"			  				},	//networkchoosestr
	{"SEND TO RECEIVER"						},	//sendtoreceivestr
	{"SAVE TO SENDER CARD"					},	//savetosendercardstr
	{"Offset(A/B)X"							},
	{"Offset(A/B)Y"		   					},
	{"SENDING CARD NO."						},
	{"SENDING CARD TYPE"					},
    {"SENDING CARD SET"          			}, //sendingCardSetStr
	{"SENDING CARD MODEL"					},
	{"SENDING CARD NO."						},
	{"Video Resolution"          			},
	{"SENDING CARD Offset X"   				},
	{"SENDING CARD Offset Y"				},
    {"SAVE TO SENDER CARD"      			},	// savetosendercardstr
    {"RECEIVING CARD SET"    				},	//recevingCardSetStr
	{"HORIZONTAL CARD"						},
	{"VERTICAL CARD" 		  				},
	{"Width" 				  				},
	{"Height" 				  				},
	{"CONNECTION MODE"          			},
	{"D to U Offset X"          			},
	{"D to U Offset Y" 		 				},
	{"SEND TO PRORT"           				},
	{"SAVE TO RECEIVER"         			},
	{"PORT A" 								},	//portAStr,
	{"PORT B"								},	//portBStr,
	{"PORT D" 								},	//portDStr,
	{"PORT U" 								},	//portUStr,
	{"Port 1"								},	// port1Str,
	{"Port 2"								},	// port2Str,
	{"Port 3"								},	// port3Str,
	{"Port 4"								},	// port4Str,								
	{"No.1" 								},	//card1Str,
	{"No.2" 								},	//card2Str,
	{"NA" 	    							},  // naStr					                
	{"READ SEND CARD PARAM",     			},	//readLEDInfoStr
	{"PLEASE WAIT FOR 1 MINUTE", 			},	//pleaseWaitForOneMinuteStr,
	{"TAKE INVALID IN PIP/SPLIT MODE"		},	//invalidInPIPOrSplitModeStr
	{"HDMI AUDIO"							},	//hdmiColorSpaceAdjustStr,
	{"SENDING CARD SET"						},
	{"RECEIVING CARD SET"					},
	{"COLOR TEMPERATURE"					},	//ColorTempSetStr
	{"Test Grid"	    					},	//GridTestStr
	{"Background"							},	//gridBackgroudStr
	{"Type"									},	//gridModeStr
	{"Grid Color"	    					},	//gridColorStr
	{"Patterns Color"	    				},	//gridDotColorStr	
	{"Width"    							},	//gridHSpaceStr
	{"Height"	    						},	//gridVSpaceStr
	{"Test Patterns"	    				},	//gridDotStr
	{"Auto Move"	    					},	//gridDotMoveStr,
	{"Hold Still"	    					},	//gridDotStillStr,
	{"Manual"	    					    },	//gridDotManualStr
	{"Hide"	    							},	//gridDotHideStr,	
	{"Moving"	    						},	//gridDotSpeedStr
	{"Cross"	    						},	//lineModeCrossStr,
	{"Row"	    							},	//lineModeHStr,
	{"Column"	    						},	//lineModeVStr,
	{"Rectangle"	    					},	//lineModeBordersStr,		
	{"Video Panel Firmware"					},	// videoPanelFirmwareStr
	{"Send"									}, 	// sendStr
	{"Save"									}, 	// saveStr
	{"From USB"								},	// fromUSBStr
	{"From internal memory"					},	// fromInternalMemStr
	{"Art-Net Start Address"				},	// artNetStartAddrStr,
	{"Art-Net Universe"						},	// artNetUniverseStr,
	{"Logo"									},	//logoStr,
	{"Customized Logo"						},	// customLogoStr
	{"Custom"							    },	// customStr
	{"Celsius"								},	// celsiusStr,
	{"Fahrenheit"							},	// fahrenheitStr,
	{"Check Sending Card ..."				},
	{"No Sending Card !"					},
	{"Sending Card Type is: Linsn"			},
	{"Sending Card Type is: colorlight"		},
	{"Sending Card Type is: Nowa"			},
		
	{"H total"								},
	{"V total"								},
	{"Sub Win Width"						},
	{"Sub Win Height"						},
		
	{"H total"								},	//totalWidth,
	{"V total"								},//totalHeigh,
	{"Width  1 "							},//firstDisWidth,
	{"Height 1 "							},//firstDisHeigh,
	{"Width  2"								},//secondDisWidth,
	{"Heigh  2"								},//secondDisHeigh,
	{"Width  3"								},//thirdDisWidth,
	{"Heigh  3"								},//thirdDisHeigh,
	{"Width  4"								},//fourthDisWidth,
	{"Heigh  4"								},//fourthDisHeigh,
	{"1024*768@60"							},	//output800x600_60Str,
	{"1024*1280@60"							},//output1024x768_60Str,
	{"1152*1152@60"							},//output1280x720_60Str,
	{"1280*720@60"							},//output1280x720_50Str,
	{"1280*1024@60"							},//output1280x768_60Str,
	{"1280*1280@60"							},//output1280x800_60Str,
	{"1400*1050@60"							},//output1280x1024_60Str,
	{"1536*1536@60"							},//output1360x768_60Str,
	{"1560*1600@60"							},//output1366x768_60Str,
	{"1600*1200@60"							},//output1400x1050_60Str,
	{"1920*1080@60"							},//output1440x900_60Str,
	{"1920*1200@60"							},//output1600x1200_60Str,
	{"2048*1152@60"							},//output1680x1050_60Str,
	{"2560*960@60"							},//output1920x1080_60Str,
	{"2560*1024@50"							},//output1920x1080_50Str,
	{"3840*540@60"							},//output1920x1200_60Str,
	{"3840*1080@30"							},//output2048x1152_60Str,
	{"1080*1920@60"							},//output2560x812_60Str,
	{"Customized"							},//outputCustomStr,
	{"HDMI2.0  Port"						},//hdmi20PortStr,
	{"HDMI1.4  Port"						},//hdmi14PortStr,
	{"DP       Port"						},//dpPortStr,
	{"DVI      Port"						},//dviPortStr,
	{"EDID HDMI2.0 Pc"						},//hdmi20EdidStr,
	{"EDID HDMI2.0 Video"					},//hdmi20VideoEdidStr,
	{"EDID HDMI2.0 NoHfvsdbNo420"			},//hdmi14EdidStr,
	{"EDID HDMI2.0 NoHfvsdbWith420"			},//hdmi13EdidStr,
	{"EDID HDMI2.0 WithHfvsdbNo420"			},//dviEdidStr,
	{"EDID HDMI2.0 WithHfvsdbNo420Hdr"		},
	{"EDID HDMI2.0 WithHfvsdbWith420"		},
	{"EDID HDMI1.4"							},
	{"EDID HDMI1.3"							},
	{"EDID DVI"								},
	{"EDID DUAL DVI"						},//dualLinkDviEdidStr,
	{"EDID DP"								},//dpEdidStr,
	{"DP 3840*2160@60"						},//dp4k2k60EdidStr,
	{"DP 3840*2160@30"						},//dp4k2k30EdidStr,
	{"EDID Customer"						},//customerEdidStr,
	{"OUTPUT Brightness"					},//diaplayAlphaStr
	{"PGM Input Source"						},//pgmInputSourceStr,
	{"PRE Input Source"						},//prvInputSourceStr,
	{"Main Input Source"					},//pipMainInputSrt,
	{"Sub  Input Source"					},//pipSubInputSrt,
	{"Sub Win X Pos"						},//pipSubPosX,
	{"Sub Win Y Pos"						},//pipSubPosY,
	{"Sub Win X Size"						},//pipSubSizeX,
	{"Sub Win Y Size"						},//pipSubSizeY,
	{"Output Port Select"					},//outputPortSelSrt,
	{"Output1 Map To"						},//outputPort0Srt,
	{"Output2 Map To"						},//outputPort1Srt,
	{"Output3 Map To"						},//outputPort2Srt,
	{"Output4 Map To"						},//outputPort3Srt,

	{"Output Type"							},//outputTpyeStr,
	{"Output Bit Width"						},//bitWidthSrt,
	{"Output Range"							},//rangeStr,

	{"Layer Param"							},//layerParamSrt,
	{"Layer A Width"						},//layerAHSizeSrt,
	{"Layer A Height"						},//layerAVSizeSrt,
	{"Layer A X Pos"						},//layerAHPosSrt,
	{"Layer A Y Pos"						},//layerAVPosSrt,
	{"Layer A Split Start"					},//layerASplitStartSrt,
	{"Layer A Crop  Start"					},//layerACropStartSrt,

	{"Layer B Width"						},//layerBHSizeSrt,
	{"Layer B Height"						},//layerBVSizeSrt,
	{"Layer B X Pos"						},//layerBHPosSrt,
	{"Layer B Y Pos"						},//layerBVPosSrt,
	{"Layer B Split Start"					},//layerBSplitStartSrt,
	{"Layer B Crop  Start"					},//layerBCropStartSrt,

	{"Layer C WIDTH"						},//layerCHSizeSrt,
	{"Layer C HEIGHT"						},//layerCVSizeSrt,
	{"Layer C X POS"						},//layerCHPosSrt,
	{"Layer C Y POS"						},//layerCVPosSrt,
	{"Layer C Split Start"					},//layerCSplitStartSrt,
	{"Layer C Crop  Start"					},//layerCCropStartSrt,

	{"Layer D WIDTH"						},//layerDHSizeSrt,
	{"Layer D HEIGHT"						},//layerDVSizeSrt,
	{"Layer D X POS"						},//layerDHPosSrt,
	{"Layer D Y POS"						},//layerDVPosSrt,
	{"Layer D Split Start"					},//layerDSplitStartSrt,
	{"Layer D Crop  Start"					},//layerDCropStartSrt,

	{"Fine Adjust"							},//FineSrt,

	{"Output DVI1 -> Layer A"				},//outPutDvi1srt,
	{"Output DVI2 -> Layer B"				},//outPutDvi2srt,
	{"Output DVI3 -> Layer C"				},//outPutDvi3srt,
	{"Output DVI4 -> Layer D"				},//outPutDvi4srt,
};


