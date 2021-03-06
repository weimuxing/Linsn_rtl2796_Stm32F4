#include "SysConfig.h"

#include "langStr.h"
#include "userPref.h"
//#include "keyScan.h"






#ifdef SUPPORT_LANG_CHINESE

#if 1
const char * const _langStr_Cn[] =
{
	{"INIT NETWORK" 						},
	{"Art Net Test" 						},
	{"FINISH NETWORK!" 						},
	{"设备初始化"  							},
	{" "									},
	{" >VSP 330" 							},
	{"  Lanuage Select:"					},
	{"  English   中文"						},
	{"保存设置"								},
	{"保存设置"								},
	{"保存到"								},
	{"调保存"								},
	{"保存到U盘"							},
	{"从U盘调保存"							},
	{"Load all from USB"					},
	{"保存失败"								},
	{"调保存失败"							},
	{"完成!"								},
	{"SUN"									},
	{"MON"									},
	{"TUE"									},
	{"WED"									},
	{"THU"									},
	{"FRI"									},
	{"SAT"									},
	{"  TIME      CALENDAR"					},
	{"Great Viwe"							},
	{"STRAIGHT MODE"						},
	{"H 1/2 MODE"							},
	{"V 1/2 MODE"							},
	{"  SETUP H 1/4 MODE"					},
	{"  SETUP V 1/4 MODE"					},
	{"  SETUP TP MODE:"						},
	{"  SCALE     ADVANCE"					},
	{"白色"									},
	{"黄色"									},
	{"天空蓝"								},
	{"绿色"									},
	{"粉色"									},
	{"红色"									},
	{"蓝色"									},
	{"黑色"									},
	{"输出"									},
	{"输出分辨率"							},
	{"输出调整"								},
	{"Scale"     							},      
	{"Scale Mode"							},
	{"Auto Fit"								},
	{"PRESS MENU TO EXIT"					},
	{" "									},
	{"COM BAUDRATE"							},
	{"关" 									},
	{"开" 									},
	{"TP MODE" 								},
	{"TP COLOR" 							},
	{"TP ROLL" 								},
	{"TP FORE" 								},
	{"TP BACK" 								},
	{"LOOP OUT" 							},
	{"DE ON/OFF" 							},
	{"DE H START" 							},
	{"DE V START" 							},
	{"DE WIDTH" 							},
	{"DE HEIGHT"							},
	{"  RANGE     HDMI/DVI"					}, 
	{"X SETUP" 								},
	{"Y SETUP" 								},
	{"HDMI/DVI" 							},
	{"HDMI/SDI"								},
	{"SDI/DVI"								},
	{"SDI/HDMI"								},
	{"DVI/HDMI"								},
	{"DVI/SDI"								},
	{"_update.pkg"							},
	{"update_.pkg"							},
	{"升级固件..."							},
	{"USB 升级固件..."						},
	{"  请等待..." 							},
	{"升级成功!" 							},
	{"升级失败!" 							},
	{"请等待设备重启!"						},
	{"请重试!" 								},
	{"SOURCE SELECT:"						}, 
	{"  SET FREEZE MODE" 					},
	{"  SET LIVE MODE" 						},
	{"亮度" 								},
	{"对比度" 								},
	{"色彩效果" 							},	// colorEffectStr
	{"锐度" 								},
	{"色饱和度"								}, 
	{"红色色温" 							},
	{"绿色色温" 							},
	{"蓝色色温" 							},
	{"色温" 								},
	{"2800 K" 								},
	{"3200 K" 								},
	{"3500 K" 								},
	{"4000 K" 								},
	{"4500 K" 								},
	{"5000 K" 								},
	{"5600 K" 								},
	{"6000 K" 								},
	{"6500 K" 								},
	{"8000 K" 								},
	{"Full RGB" 							},
	{"VOLUME" 								},
	{"  PICTURE MODE:" 						},
	{"UPGRADE MAIN BOARD"					}, 
	{"PLEASE PRESS SEL" 					},
	{"  SCREEN POS X:" 						},
	{"  SCREEN POS Y:" 						},
	{"  SCREEN WIDTH:"						},
	{"  SCREEN HEIGHT:" 					},
	{"自动Scale"							},
	{"PICTURE"								},
	{"4:3"									},
	{"16:9" 								},
	{"Fit to Screen" 						},
	{"  NO FILE" 							},
	{"  TP ON/OFF:" 						},
	{"  SWAP:" 								},
	{"  TOP BLACK:"							},
	{"IMAGE"								},
	{"VIDEO" 								},
	{"DATA RANGE" 							},
	{"DE ADJUST" 							},
	{"复位"  								},               
	{"默认"									},
	{"视频输入"								},
	{"输入"     							},             
	{"主输出"   							},                
	{"预监输出" 							},                  
	{"主输出"  								},                 
	{"预监输出" 							},                  
	{"预监输出" 							},                  
	{"主输出" 								},                  
	{"输出" 								},                
	{"FORMAT" 								},                
	{"系统设置" 							},                
	{"系统信息" 							},           
	{"高级"    								},        
	{"DATE&TIME" 							},            
	{"网络设置" 							},              
	{"网络"  								},              
	{"锁定按键" 							},      
	{"Lock Password"						},
	{"LICENSE SETUP" 						},      
	{"CAN USE TIME"  						},      
	{"NO LIMIT" 							},       
	{"CHANGE PASSWORD" 						},       
	{"请输入新密码"  						},   
	{"请再输入一次密码"						},   
	{"Two Passwords" 						},   
	{"Do Not Match" 						},  
	{"成功"    								},
	{"输入密码"								},
	{" 输入密码:"							},
	{"密码错误"								},
	{"重试密码"								},
	{"LICENSE STATE" 						},       
	{"CHANGE TIME" 							},       
	{"CHANGE DATE"  						},      
	{"输入信息"  							},        
	{"自动"									},
	{"宽度"   								},           
	{"高度"    								},          
	{"宽度/高度"  							},            
	{"水平位置"  							},            
	{"垂直位置"  							},            
	{"位置设置"								},
	{"偏移量位置"  							},                    
	{"Port 1/2 X偏移量"  					},            
	{"Port 1/2 Y偏移量" 					},             
	{"Port 3/4 X偏移量" 					},             
	{"Port 3/4 Y偏移量" 					},             
	{"VGA ADJUST"      						},       
	{"DVI/EXT ADJUST"  						}, 
	{"CLOCK"       							},       
	{"PHASE"       							},       
	{"自动调整"    							},        
	{"SCREEN PARAMETERS" 					},    
	{"MODE"         						},    
	{"FULL SIZE"    						},         
	{"SCREEN SIZE"  						},          
	{"主输出"   							},        
	{"DE"     								},       
	{"DVI"    								},        
	{"HDMI"    								},        
	{"HD-SDI"								},
	{"DVI MODE" 							},           
	{"DE H POS"  							},            
	{"DE V POS"   							},           
	{"DE H SIZE"  							},            
	{"DE V SIZE"  							},            
	{"Gamma"      							},     
	{"Transition" 							},           
	{"Fade Type"							},
	{"淡入淡出时间"       					},     
	{"Alpha"     							},      
	{"Dissolve"  							},          
	{"Cut" 									},
	{"Wipe Square In" 						},         
	{"Wipe Square Out"  					},    
	{"Wipe T_L In"  						},
	{"Wipe T_L"  							},         
	{"Wipe T_R In" 							},      
	{"Wipe T_R"  							},        
	{"Wipe B_L In"							},     
	{"Wipe B_L"   							},     
	{"Wipe B_R In" 							},   
	{"Wipe B_R"  							},     
	{"Wipe Left In"							},           
	{"Wipe Left"    						},        
	{"Wipe Right In" 						},         
	{"Wipe Right" 							},           
	{"Wipe Top In"  						},      
	{"Wipe Top" 							},           
	{"Wipe Bottom In" 						},        
	{"Wipe Bottom"							},            
	{"   Black Out" 						},           
	{"  Live Image" 						},          
	{"Freeze Image" 						},          
	{"  Pure Image" 						},          
	{"Test Pattern" 						},          
	{"Mode"  								},         
	{"Auto Switch" 							},           
	{"HOR STEP"  							},         
	{"VER STEP" 							},           
	{"COLOR" 								},           
	{"画中画"								},           
	{"信号源"								},
	{"TOP WINDOW" 							},          
	{"交换窗口"								},
	{"布局" 								},           
	{"选择" 								},          
	{"TEXT OVERLAY"							},            
	{"PRESET"      							},      
	{"BLEND MODE" 							},           
	{"BLEND LEVEL" 							},           
	{"ABOVE/BELOW" 							},           
	{"KEY IN/OUT" 							},           
	{"USER MODE" 							},            		
	{"画中画左上"							},
	{"画中画右上"							},//pipRTStr,
	{"画中画左下"							},//pipLBStr,
	{"画中画右下"							},//pipRBStr,	
	{"画中画居中"							},//pipCntStr
	{"左右双画面"							},
	{"上下双画面"							},	
	{"Image A"  							},          
	{"Image B"  							},          
	{"语言/LANGUAGE" 						},          
	{"ENG" 									},           
	{"中文"									},            
	{"显示模式" 							},          
	{"PURE COLOR" 							},           
	{"AUDIO" 								},           
	{"MUTE" 								},           
	{"AUDIO IN" 							},           
	{"INTERNAL" 							},           
	{"EXTERNAL" 							},          
	{"工厂复位"  							},         
	{"复位IP地址" 							},        
	{"否" 									},          
	{"是"  									},        
	{"Yes <Knob>, No <Menu>"				},
	{"技术支持"								},
	{"SALES :" 								},
	{"4008-592-114"							},
	{"CUSTOMER SERVICE:"					},
	{"4008-592-315"							},
	{"TEL:   +86(0592)5771197"				},
	{"EMAIL:   rgblinkcs@gmail.com"			},
	{"WEBSITE: www.rgblink.com"				},
	{"IP"									},
	{"请按下Menu键5秒钟"					},
	{"按键解锁!"   							},
	{"按键解锁!" 							},
	{"ADC ADJUST"							},
	{"ADC AUTO ADJUST"						},
	{"ADC RESET ALL"						},
	{"INVALID OPERATION"					},
	{"IN PIP MODE"							},
	{"MODE"									},
	{"LOCK"									},
	{"FREE RUN"								},
	{"FRAME LOCK 1"							},
	{"FRAME LOCK 2"							},
	{"DATE"									},
	{"TIME"									},
	{"WORK TIME"							},
	{"TOTAL TIME"							},
	{"BOOT TIMES"							},
	{"TIMING CONTROL"						},
	{"hours"								},
	{"minutes"								},
	{"DELAY CALL"							},
	{"BELOW"								},
	{"ABOVE"								},
	{"OUT"									},
	{"IN"									},
	{""										},
	{"SYNC"									},
	{"Black Out"							},
	{"RGB444"								},
	{"YUV444"								},
	{"YUV422"								},
	{"色空间"								},
	{"Deinterlace"							},
	{"IP  地址"								},
	{"远程IP地址"							}, 
	{"本机IP地址" 							},
	{"子网掩码"								},
	{"Gateway"								},
	{"DHCP"									},
	{"VLOCK"								},
	{"预监拼接",							},
	{"四口拼接",							},
	{"多进多出",							},
	{"独立输出",							},	//independentModeStr
	{"两进三出",							},
	{"两进四出",							},
	{"三进三出",							},
	{"三进四出",							},
	{"四进四出",							},
	{"H TOTAL"								},
	{"V TOTAL"								},
	{"H POS"    							},          
	{"V POS"   								},           
	{"H SIZE" 								},             
	{"V SIZE"    							},          
	{"标准分辨率"							},
	{"OTHER"								},
	{"自定义分辨率"							},
	{"按对应的数字键保存      "				},
	{"按'MENU'键退出                "		},
	{"按对应的数字键调保存"					},
	{"按'MENU'键退出"						},
	{"保存 "								},
	{" 已使用,确定要覆盖吗?"				},
	{"  Push the red button to       "		},
	{"  请输入数字                "			},
	{"  set input source             "		},
	{"  Please select value          "		},
	{"  Push the rotary to set       "		},
	{"Push button flashs to save     "		},
	{"after power on,now it flashs   "		},
	{"User mode 1 is default recall  "		},
	{"按键已锁定!"							},
	{"BEYOND THE SUPPORTED RANGE!    "		},
	{"When you activate Image Enhance"		},
	{"function,transitions will not"		},
	{"work.Would you like to active?"		},
	{"Image Enhance Enable!"				},
	{"PLEASE SET LANGUAGE/语言:"			},
	{"ENGLISH"								},
	{"PLEASE SET OUTPUT FORMAT: "			},
	{"PLEASE SET SCREEN WIDTH: "			},
	{"PLEASE SET SCREEN HEIGHT: "			},
	{"PLEASE SET SCREEN X POS: "			},
	{"PLEASE SET SCREEN Y POS: "			},
	{"PLEASE SET INPUT SOURCE: "			},
	{"PLEASE SAVE THE SETTINGS "			},
	{"PLEASE SAVE TO USER MODE 1 "			},
	{"ADJUST"								},
	{"CURRENT TIME"							},
	{"LAST TIME"							},
	{"画质调节"								},
	{"放大"      							},
	{"向上放大"								},
	{"向下放大"								},
	{"上下同时放大"							},
	{"向左放大"								},
	{"向右放大"    							},      
	{"左右同时放大"							},
	{"从中间放大"							},
	{"VGA SIGNAL TYPE" 						},      
	{"VGA2 SIGNAL TYPE"						},
	{"VGA"									},
	{"YPbPr"								},
	{"Fade Time"							},
	{"OLED屏幕亮度"							},
	{"H POLARITY"    						},             
	{"V POLARITY"    						},             
	{"POSITIVE"     						},              
	{"NEGATIVE"     						},              
	{"Image Enhance" 						},             
	{"BIT DEPTH"							},
	{"8 BIT"								},
	{"10 BIT"								},
	{"12 BIT"								},
	{"USB UPGRADE" 							},                 
	{"COPY CODE TO DEVICE" 					},  
	{"REBOOT AFTER COPY"					},
	{"ANTI-ALIAS"							},
	{"WAITING"								},
	{"    DO NOT UPGRADE UNLESS      "		},
	{"    CONNECT TO COMPUTER!!!     "		},
	{"热备份"  							},               
	{"热备份 1"								},
	{"热备份 2"								},
	{"热备份 3"								},
	{"热备份 4"								},
	{"热备份 5"								},
	{"INVALID OPERATION IN PIP MODE"		},
	{"记录为空!"							},
	{"HDMI ADJUST"   						},
	{" CHANGE WILL NOT TAKE EFFECT   "		},
	{"        UNTIL RESTART          "		},
	{"软件版本"								},
	{"硬件版本"								},
	{"序列号"   							},
	//{"ENTER SN NUMBER"           			},	//WriteSnStr,
	{"请输入序列号"           				},	//WriteSnStr,
	{"VERSION"   							},                 
	{"Firmware版本"							},
	{"Video版本"   							},        
	{"COMMBOARD VERSION" 					},         
	{"主板版本"   							},        
	{"FPGA版本"   							},        
	{"ExtBoard1 Hard Version"				},
	{"ExtBoard2 Hard Version"				},
	{"ExtBoard1 Soft Version"				},
	{"ExtBoard2 Soft Version"				},
	{"WIFI&LED VERSION"						},
	{"EDID管理"    							},    
	{"EDID源"    							},    
	{"目的端口"  							},   
	{"USB CONNECTING..."					},
	{"PORT A"  								},       
	{"PORT B"    							},     
	{"PHOTO TIME"							},
	{"USB MODULE IS NOT INSTALLED"			},
	{"CONTACT THE SALES TO PURCHASE"		},
	{"     Copy EDID Success       "		},
	{"     Copy EDID Fail          "		},
	{"  Edit EDID"							},
	{"请勿关闭设备电源"						},
	{"G3-NET将自动重启"						},
	{"风扇控制"        						},    
	{"AUTO SPEED"  							},            
	{"CURRENT SPEED" 						},          
	{"CURRENT TEMPERATURE" 					},   
	{"温度"									},
	{"DEVICE STATUS"  						},         
	{"VIDEO STATUS"  						},           
	{"MB STATUS"  							},              
	{"EXT STATUS"  							},            
	{"UNCONNECTED" 							},             
	{"NORMAL"   							},             
	{"INVALID OPERATION"					},
	{"IN SPLIT MODE"						},
	{"用户自定义按键" 						},   
	{"TP"									},
	{"BLACK"								},
	{"BLACK"								},
	{"冻结"									},
	{"Logo Na"								},
	{"TP"									},
	{"亮度"									},
	{"EXTENDED MODULE IS NOT INSTALLED"		},
	{"CVBS MODULE IS NOT INSTALLED"			},
	{"EXT CVBS CONTROL"  					},       
	{"CVBS PORT"   							},      
	{"PORT A"     							},    
	{"PORT B"     							},    
	{"输入分辨率"  							},       
	{"无输入"      							},  
	{"EXT VGA ADJUST" 						},        
	{"REAL-TIME SAVE" 						},       
	{"固定拼接" 							},
	{"NORMAL SPLIT"							},
	{"拼接类型"								},
	{"水平一分二"							},
	{"水平一分三"							},
	{"垂直一分二" 							},
	{"田字拼接", 							},
	{"水平一分四"							},
	{"垂直一分四" 							},
	{"SPLIT INDEX"							},
	{"HORIZONTAL SPLIT"						},
	{"VERICAL SPLIT"						},
	{"Deivce is in external control" 		},
	{"Device is under remote control"		},
	{"Front panel buttons are locked"		},
	{"LOADING USB "							},
	{"FILES  "								},
	{"Double-tap To Switch" 				},
	{"预监输出2.0屏" 						},
	{"预监DVI端口"							},
	{"SWITCH TYPE" 							},
	{"SMIPLE VIDEO" 						},
	{"COMPLEX VIDEO"						},
	{"英文"									},
	{"INVALID OPERATION IN PIP MODE"		},
	{"INVALID OPERATION IN SPLIT MODE"		},
	{"SPLID IS INVALID IN PIP MODE"			},
	{"LOADING" 								},
	{"完成" 								},
	{"EXT CONFIG" 							},
	{"DVI/EXT AUDIO"  						}, 
	{"AUDIO TYPE"							},
	{"NO MODULE"  							},
	{"AUDIO MODULE" 						},
	{"LOADING" 								},
	{"完成"									},
	{"缩小"									},
	{"TAKE"									},
	{"      NO SIGNAL NEED TAKE      "		},
	{"ONLY DVI_5/MODULE_9"					},
	{"SUPPORT SPLICING"						},
	{"ONLY DVI_5/MODULE_HDMI"				},
	{"       IN TRANSITION            "		},
	{"       TRANSTION FINISHED       "		},
	{"MODULE INPUT IMAGE EXHANCE"			},
	{"NO THIS MODULE!"						},
	{"SKIP!"								},
	{"DVI1 输出"							},
	{"输出" 								},
	{"DVI_环出" 							},
	{"HDMI 输入"							},
	{"HDMI/EXT AUDIO"						},
	{"ONLY MB_V1.3 SUPPORT"					},
	{"SCREEN FOLLOW SCALE"					},
	{"加载播放列表"							},
	{"INPUT AUDIO CONFIG"					},
	{"INPUT AUDIO 1"						},
	{"INPUT AUDIO 2"						},
	{"INPUT AUDIO 3"						},
	{"NO EXT"								},
	{"THROUGH"								},
	{"HDMI/EXT ADJUST"						},
	{"PRESS KEY" 							},  
	{"KNOB"									},
	{"KEY TEST"   							},
	{"高级"   								},
	{"Port Tutorial" 						},  
	{"设置宽度"  							}, 
	{"自动"   								},
	{"手动"   								},
	{"Auto Set Width(pixels):"  			}, 
	{"Auto Set Height(pixels):" 			},  
	{"箱体大小:"   							},
	{"Panel Pixel Count:"   				},
	{"箱体大小 宽*高:"  					}, 
	{"带载面积 宽*高:"  					}, 
	{"Total Pixel Res:" 					},  
	{"LED显示屏配置"  						}, 
	{"规则显示屏拼接"  						}, 
	{"不规则显示屏拼接"						},   
	{"Port Set"   							},
	{"Send Card Set"   						},
	{"Port"   								},
	{"H Panels"   							},
	{"V Panels" 							},  
	{"LEDs/Row"  							}, 
	{"LEDs/Column"  						}, 
	{"H Position"							},
	{"V Position"							},
	{"Port 1 x 位置"						},
	{"Port 1 Y 位置"						},
	{"Port 2 X 位置"						},
	{"Port 2 Y 位置"						},
	{"Port 3 X 位置"						},
	{"Port 3 Y 位置"						},
	{"Port 4 X 位置"						},
	{"Port 4 Y 位置"						},
	{"Panel"								},
	{"Panel No."							},
	{"保存到端口"							},
	{" 保存到端口成功   "					},
	{" 保存到端口失败     "					},
	{"Save To Panel"						},
	{"Reset Port Set"						},
	{"连接类型"								},
	{"LED MANAGEMENT"  						},   
	{"快速连接"								},
	{"显示屏连接"							},
	{"ADVANCED"								},
	{"SENDING CARD TYPE"					},
	{"CASCADE"								},
	{"RECEIVING CARD SEPARATE"				},
	{"卡莱特"								},
	{"灵星雨"								},
	{"诺瓦" 								},
	{"SENDING CARD SET"						},
	{"亮度"									},
	{"SENDING CARD POSITION"				},
	{"GAMMA"								},
	{"显示屏连接"							},
	{"设置" 								},
	{"X"									},
	{"Y"									},
	{"宽度"									},
	{"高度"									},
	{"SENDING CARD MODEL"					},
	{"PARAMETER SETTING"					},
	{"HORIZONTAL CARD" 						},
	{"VERTICAL CARD" 						},
	{"宽度"									},
	{"高度"									},
	{"CONNECTION MODE"						},
	{"RECEIVING CARD POSITION" 				},   
	{"SENDING CARD NO. 1/2"					},
	{"POSITION SETTING"						},
	{"RECEIVING CARD NO."					},
	{"控制系统版本号" 						},   
	{"发送卡版本号"							},
	{"接收卡版本号"							},
	{"保存亮度值"							},
	{"PORT D"								},
	{"PORT U"								},
	{"CHOOSE CABLE"							},
	{"SEND TO RECEIVER"						},
	{"SAVE TO SENDER CARD"					},
	{"Offset(A/B)X"							},
	{"Offset(A/B)Y"							},
	{"SENDING CARD NO."						},
	{"发送卡类型"							},
	{"SENDING CARD SET"  					},        
	{"SENDING CARD MODEL"					},
	{"SENDING CARD NO."						},
	{"Video Resolution"      				},    
	{"SENDING CARD Offset X" 				},  
	{"SENDING CARD Offset Y"				},
	{"SAVE TO SENDER CARD" 					},     
	{"RECEIVING CARD SET" 					},   
	{"HORIZONTAL CARD"						},
	{"VERTICAL CARD" 						},
	{"WIDTH" 								},
	{"HEIGHT" 								},
	{"CONNECTION MODE"  					},        
	{"D to U Offset X"  					},        
	{"D to U Offset Y" 						},
	{"SEND TO PRORT"     					},      
	{"SAVE TO RECEIVER"    					},     
	{"端口 A" 								},
	{"端口 B"								},
	{"端口 D" 								},
	{"端口 U" 								},
	{"端口 1"								},
	{"端口 2"								},
	{"端口 3"								},
	{"端口 4"								},
	{"No.1" 								},
	{"No.2" 								},
	{"无" 									},
	{"READ SEND CARD PARAM",  				},   
	{"PLEASE WAIT FOR 1 MINUTE",			}, 
	{"TAKE INVALID IN PIP/SPLIT MODE"		},
	{"HDMI AUDIO"							},
	{"发送卡设置"							},
	{"接收卡设置"							},
	{"色温调节"								},
	{"测试网格"								},
	{"背景"									},
	{"类型"									},
	{"网格颜色"								},
	{"模板颜色"								},
	{"宽度"    								},
	{"高度"									},
	{"测试模板"								},
	{"自动移动"								},
	{"Hold Still"							},
	{"Manual"								},
	{"隐藏"									},
	{"运动"									},
	{"Cross"								},
	{"Row"									},
	{"Column"								},
	{"Rectangle"							},
	{"Video Panel Firmware"					},
	{"发送"									},
	{"保存"									},
	{"From USB"								},
	{"From internal memory"					},
	{"Art-Net开始地址"						},
	{"Art-Net单元地址"						},
	{"Logo"									},
	{"Customized Logo"						},
	{"Custom"								},
	{"Celsius"								},
	{"Fahrenheit"							},
	{"检查发送卡 ..."						},
	{"未检测到发送卡 !"						},
	{"发送卡类型: 灵星雨"					},
	{"发送卡类型: 卡莱特"					},
	{"发送卡类型: 诺瓦"						},

	{"主屏宽"								},
	{"主屏高"								},
	{"子屏宽"								},
	{"子屏高"								},
		
	{"总屏宽"								},
	{"总屏高"								},
	{"第一块屏宽"							},
	{"第一块屏高"							},
	{"第二块屏宽"							},
	{"第二块屏高"							},
	{"第三块屏宽"							},
	{"第三块屏高"							},
	{"第四块屏宽"							},
	{"第四块屏高"							},
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
	{"自定义分辨率"							},//outputCustomStr,
	{"HDMI2.0  端口"						},
	{"HDMI1.4  端口"						},
	{"DP       端口"						},
	{"DVI      端口"						},
	{"EDID HDMI2.0 Pc"						},
	{"EDID HDMI2.0 Video"					},//hdmi20VideoEdidStr,
	{"EDID HDMI2.0 NoHfvsdbNo420"			},//hdmi20NoHfvsdbNo420, 	
	{"EDID HDMI2.0 NoHfvsdbWith420"			},//hdmi20NoHfvsdbWith420, 	
	{"EDID HDMI2.0 WithHfvsdbNo420"			},//hdmi20WithHfvsdbNo420, 	
	{"EDID HDMI2.0 WithHfvsdbNo420Hdr"		},//hdmi20WithHfvsdbNo420Hdr,
	{"EDID HDMI2.0 WithHfvsdbWith420"		},//hdmi20WithHfvsdbWith420,
	{"EDID HDMI1.4"							},
	{"EDID HDMI1.3"							},
	{"EDID DVI"								},
	{"EDID DL DVI"							},
	{"EDID DP"								},
	{"DP 3840*2160@60"						},//dp4k2k60EdidStr,
	{"DP 3840*2160@30"						},//dp4k2k30EdidStr,
	{"自定义EDID分辨率"						},
	{"输出亮度"								},
	{"主输出输入源"							},
	{"预监  输入源"							},
	{"主窗口输入源"							},
	{"子窗口输入源"							},
	{"子窗口 X 位置"						},//pipSubPosX,
	{"子窗口 Y 位置"						},//pipSubPosY,
	{"子窗口 X 宽度"						},//pipSubSizeX,
	{"子窗口 Y 高度"						},//pipSubSizeY,

	{"输出端口选择"							},//outputPortSelSrt,
	{"输出1映射到"							},//outputPort0Srt,
	{"输出2映射到"							},//outputPort1Srt,
	{"输出3映射到"							},//outputPort2Srt,
	{"输出4映射到"							},//outputPort3Srt,

	{"输出类型"								},//outputTpyeStr,
	{"输出位宽"								},//bitWidthSrt,
	{"输出值域"								},//rangeStr,

	{"图层参数"								},//layerParamSrt,
	{"图层 A 宽度"							},//layerAHSizeSrt,
	{"图层 A 高度"							},//layerAVSizeSrt,
	{"图层 A X 位置"						},//layerAHPosSrt,
	{"图层 A Y 位置"						},//layerAVPosSrt,
	{"图层 A 拼接 开始"						},//layerASplitStartSrt,
	{"图层 A 裁剪 开始"						},//layerACropStartSrt,

	{"图层 B 宽度"							},//layerBHSizeSrt,
	{"图层 B 高度"							},//layerBVSizeSrt,
	{"图层 B X 位置"						},//layerBHPosSrt,
	{"图层 B Y 位置"						},//layerBVPosSrt,
	{"图层 B 拼接 开始"						},//layerBSplitStartSrt,
	{"图层 B 裁剪 开始"						},//layerBCropStartSrt,

	{"图层 C 宽度"							},//layerCHSizeSrt,
	{"图层 C 高度"							},//layerCVSizeSrt,
	{"图层 C X 位置"						},//layerCHPosSrt,
	{"图层 C Y 位置"						},//layerCVPosSrt,
	{"图层 C 拼接 开始"						},//layerCSplitStartSrt,
	{"图层 C 裁剪 开始"						},//layerCCropStartSrt,

	{"图层 D 宽度"							},//layerDHSizeSrt,
	{"图层 D 高度"							},//layerDVSizeSrt,
	{"图层 D X 位置"						},//layerDHPosSrt,
	{"图层 D Y 位置"						},//layerDVPosSrt,
	{"图层 D 拼接 开始"						},//layerDSplitStartSrt,
	{"图层 D 裁剪 开始"						},//layerDCropStartSrt,

	{"微调"									},//FineSrt,

	{"输出口DVI1 -> A图层"					},//outPutDvi1srt,
	{"输出口DVI2 -> B图层"					},//outPutDvi2srt,
	{"输出口DVI3 -> C图层"					},//outPutDvi3srt,
	{"输出口DVI4 -> D图层"					},//outPutDvi4srt,


};


#else


#endif

#endif

