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
	{"�豸��ʼ��"  							},
	{" "									},
	{" >VSP 330" 							},
	{"  Lanuage Select:"					},
	{"  English   ����"						},
	{"��������"								},
	{"��������"								},
	{"���浽"								},
	{"������"								},
	{"���浽U��"							},
	{"��U�̵�����"							},
	{"Load all from USB"					},
	{"����ʧ��"								},
	{"������ʧ��"							},
	{"���!"								},
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
	{"��ɫ"									},
	{"��ɫ"									},
	{"�����"								},
	{"��ɫ"									},
	{"��ɫ"									},
	{"��ɫ"									},
	{"��ɫ"									},
	{"��ɫ"									},
	{"���"									},
	{"����ֱ���"							},
	{"�������"								},
	{"Scale"     							},      
	{"Scale Mode"							},
	{"Auto Fit"								},
	{"PRESS MENU TO EXIT"					},
	{" "									},
	{"COM BAUDRATE"							},
	{"��" 									},
	{"��" 									},
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
	{"�����̼�..."							},
	{"USB �����̼�..."						},
	{"  ��ȴ�..." 							},
	{"�����ɹ�!" 							},
	{"����ʧ��!" 							},
	{"��ȴ��豸����!"						},
	{"������!" 								},
	{"SOURCE SELECT:"						}, 
	{"  SET FREEZE MODE" 					},
	{"  SET LIVE MODE" 						},
	{"����" 								},
	{"�Աȶ�" 								},
	{"ɫ��Ч��" 							},	// colorEffectStr
	{"���" 								},
	{"ɫ���Ͷ�"								}, 
	{"��ɫɫ��" 							},
	{"��ɫɫ��" 							},
	{"��ɫɫ��" 							},
	{"ɫ��" 								},
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
	{"�Զ�Scale"							},
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
	{"��λ"  								},               
	{"Ĭ��"									},
	{"��Ƶ����"								},
	{"����"     							},             
	{"�����"   							},                
	{"Ԥ�����" 							},                  
	{"�����"  								},                 
	{"Ԥ�����" 							},                  
	{"Ԥ�����" 							},                  
	{"�����" 								},                  
	{"���" 								},                
	{"FORMAT" 								},                
	{"ϵͳ����" 							},                
	{"ϵͳ��Ϣ" 							},           
	{"�߼�"    								},        
	{"DATE&TIME" 							},            
	{"��������" 							},              
	{"����"  								},              
	{"��������" 							},      
	{"Lock Password"						},
	{"LICENSE SETUP" 						},      
	{"CAN USE TIME"  						},      
	{"NO LIMIT" 							},       
	{"CHANGE PASSWORD" 						},       
	{"������������"  						},   
	{"��������һ������"						},   
	{"Two Passwords" 						},   
	{"Do Not Match" 						},  
	{"�ɹ�"    								},
	{"��������"								},
	{" ��������:"							},
	{"�������"								},
	{"��������"								},
	{"LICENSE STATE" 						},       
	{"CHANGE TIME" 							},       
	{"CHANGE DATE"  						},      
	{"������Ϣ"  							},        
	{"�Զ�"									},
	{"����"   								},           
	{"�߶�"    								},          
	{"����/�߶�"  							},            
	{"ˮƽλ��"  							},            
	{"��ֱλ��"  							},            
	{"λ������"								},
	{"ƫ����λ��"  							},                    
	{"Port 1/2 Xƫ����"  					},            
	{"Port 1/2 Yƫ����" 					},             
	{"Port 3/4 Xƫ����" 					},             
	{"Port 3/4 Yƫ����" 					},             
	{"VGA ADJUST"      						},       
	{"DVI/EXT ADJUST"  						}, 
	{"CLOCK"       							},       
	{"PHASE"       							},       
	{"�Զ�����"    							},        
	{"SCREEN PARAMETERS" 					},    
	{"MODE"         						},    
	{"FULL SIZE"    						},         
	{"SCREEN SIZE"  						},          
	{"�����"   							},        
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
	{"���뵭��ʱ��"       					},     
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
	{"���л�"								},           
	{"�ź�Դ"								},
	{"TOP WINDOW" 							},          
	{"��������"								},
	{"����" 								},           
	{"ѡ��" 								},          
	{"TEXT OVERLAY"							},            
	{"PRESET"      							},      
	{"BLEND MODE" 							},           
	{"BLEND LEVEL" 							},           
	{"ABOVE/BELOW" 							},           
	{"KEY IN/OUT" 							},           
	{"USER MODE" 							},            		
	{"���л�����"							},
	{"���л�����"							},//pipRTStr,
	{"���л�����"							},//pipLBStr,
	{"���л�����"							},//pipRBStr,	
	{"���л�����"							},//pipCntStr
	{"����˫����"							},
	{"����˫����"							},	
	{"Image A"  							},          
	{"Image B"  							},          
	{"����/LANGUAGE" 						},          
	{"ENG" 									},           
	{"����"									},            
	{"��ʾģʽ" 							},          
	{"PURE COLOR" 							},           
	{"AUDIO" 								},           
	{"MUTE" 								},           
	{"AUDIO IN" 							},           
	{"INTERNAL" 							},           
	{"EXTERNAL" 							},          
	{"������λ"  							},         
	{"��λIP��ַ" 							},        
	{"��" 									},          
	{"��"  									},        
	{"Yes <Knob>, No <Menu>"				},
	{"����֧��"								},
	{"SALES :" 								},
	{"4008-592-114"							},
	{"CUSTOMER SERVICE:"					},
	{"4008-592-315"							},
	{"TEL:   +86(0592)5771197"				},
	{"EMAIL:   rgblinkcs@gmail.com"			},
	{"WEBSITE: www.rgblink.com"				},
	{"IP"									},
	{"�밴��Menu��5����"					},
	{"��������!"   							},
	{"��������!" 							},
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
	{"ɫ�ռ�"								},
	{"Deinterlace"							},
	{"IP  ��ַ"								},
	{"Զ��IP��ַ"							}, 
	{"����IP��ַ" 							},
	{"��������"								},
	{"Gateway"								},
	{"DHCP"									},
	{"VLOCK"								},
	{"Ԥ��ƴ��",							},
	{"�Ŀ�ƴ��",							},
	{"������",							},
	{"�������",							},	//independentModeStr
	{"��������",							},
	{"�����ĳ�",							},
	{"��������",							},
	{"�����ĳ�",							},
	{"�Ľ��ĳ�",							},
	{"H TOTAL"								},
	{"V TOTAL"								},
	{"H POS"    							},          
	{"V POS"   								},           
	{"H SIZE" 								},             
	{"V SIZE"    							},          
	{"��׼�ֱ���"							},
	{"OTHER"								},
	{"�Զ���ֱ���"							},
	{"����Ӧ�����ּ�����      "				},
	{"��'MENU'���˳�                "		},
	{"����Ӧ�����ּ�������"					},
	{"��'MENU'���˳�"						},
	{"���� "								},
	{" ��ʹ��,ȷ��Ҫ������?"				},
	{"  Push the red button to       "		},
	{"  ����������                "			},
	{"  set input source             "		},
	{"  Please select value          "		},
	{"  Push the rotary to set       "		},
	{"Push button flashs to save     "		},
	{"after power on,now it flashs   "		},
	{"User mode 1 is default recall  "		},
	{"����������!"							},
	{"BEYOND THE SUPPORTED RANGE!    "		},
	{"When you activate Image Enhance"		},
	{"function,transitions will not"		},
	{"work.Would you like to active?"		},
	{"Image Enhance Enable!"				},
	{"PLEASE SET LANGUAGE/����:"			},
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
	{"���ʵ���"								},
	{"�Ŵ�"      							},
	{"���ϷŴ�"								},
	{"���·Ŵ�"								},
	{"����ͬʱ�Ŵ�"							},
	{"����Ŵ�"								},
	{"���ҷŴ�"    							},      
	{"����ͬʱ�Ŵ�"							},
	{"���м�Ŵ�"							},
	{"VGA SIGNAL TYPE" 						},      
	{"VGA2 SIGNAL TYPE"						},
	{"VGA"									},
	{"YPbPr"								},
	{"Fade Time"							},
	{"OLED��Ļ����"							},
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
	{"�ȱ���"  							},               
	{"�ȱ��� 1"								},
	{"�ȱ��� 2"								},
	{"�ȱ��� 3"								},
	{"�ȱ��� 4"								},
	{"�ȱ��� 5"								},
	{"INVALID OPERATION IN PIP MODE"		},
	{"��¼Ϊ��!"							},
	{"HDMI ADJUST"   						},
	{" CHANGE WILL NOT TAKE EFFECT   "		},
	{"        UNTIL RESTART          "		},
	{"�����汾"								},
	{"Ӳ���汾"								},
	{"���к�"   							},
	//{"ENTER SN NUMBER"           			},	//WriteSnStr,
	{"���������к�"           				},	//WriteSnStr,
	{"VERSION"   							},                 
	{"Firmware�汾"							},
	{"Video�汾"   							},        
	{"COMMBOARD VERSION" 					},         
	{"����汾"   							},        
	{"FPGA�汾"   							},        
	{"ExtBoard1 Hard Version"				},
	{"ExtBoard2 Hard Version"				},
	{"ExtBoard1 Soft Version"				},
	{"ExtBoard2 Soft Version"				},
	{"WIFI&LED VERSION"						},
	{"EDID����"    							},    
	{"EDIDԴ"    							},    
	{"Ŀ�Ķ˿�"  							},   
	{"USB CONNECTING..."					},
	{"PORT A"  								},       
	{"PORT B"    							},     
	{"PHOTO TIME"							},
	{"USB MODULE IS NOT INSTALLED"			},
	{"CONTACT THE SALES TO PURCHASE"		},
	{"     Copy EDID Success       "		},
	{"     Copy EDID Fail          "		},
	{"  Edit EDID"							},
	{"����ر��豸��Դ"						},
	{"G3-NET���Զ�����"						},
	{"���ȿ���"        						},    
	{"AUTO SPEED"  							},            
	{"CURRENT SPEED" 						},          
	{"CURRENT TEMPERATURE" 					},   
	{"�¶�"									},
	{"DEVICE STATUS"  						},         
	{"VIDEO STATUS"  						},           
	{"MB STATUS"  							},              
	{"EXT STATUS"  							},            
	{"UNCONNECTED" 							},             
	{"NORMAL"   							},             
	{"INVALID OPERATION"					},
	{"IN SPLIT MODE"						},
	{"�û��Զ��尴��" 						},   
	{"TP"									},
	{"BLACK"								},
	{"BLACK"								},
	{"����"									},
	{"Logo Na"								},
	{"TP"									},
	{"����"									},
	{"EXTENDED MODULE IS NOT INSTALLED"		},
	{"CVBS MODULE IS NOT INSTALLED"			},
	{"EXT CVBS CONTROL"  					},       
	{"CVBS PORT"   							},      
	{"PORT A"     							},    
	{"PORT B"     							},    
	{"����ֱ���"  							},       
	{"������"      							},  
	{"EXT VGA ADJUST" 						},        
	{"REAL-TIME SAVE" 						},       
	{"�̶�ƴ��" 							},
	{"NORMAL SPLIT"							},
	{"ƴ������"								},
	{"ˮƽһ�ֶ�"							},
	{"ˮƽһ����"							},
	{"��ֱһ�ֶ�" 							},
	{"����ƴ��", 							},
	{"ˮƽһ����"							},
	{"��ֱһ����" 							},
	{"SPLIT INDEX"							},
	{"HORIZONTAL SPLIT"						},
	{"VERICAL SPLIT"						},
	{"Deivce is in external control" 		},
	{"Device is under remote control"		},
	{"Front panel buttons are locked"		},
	{"LOADING USB "							},
	{"FILES  "								},
	{"Double-tap To Switch" 				},
	{"Ԥ�����2.0��" 						},
	{"Ԥ��DVI�˿�"							},
	{"SWITCH TYPE" 							},
	{"SMIPLE VIDEO" 						},
	{"COMPLEX VIDEO"						},
	{"Ӣ��"									},
	{"INVALID OPERATION IN PIP MODE"		},
	{"INVALID OPERATION IN SPLIT MODE"		},
	{"SPLID IS INVALID IN PIP MODE"			},
	{"LOADING" 								},
	{"���" 								},
	{"EXT CONFIG" 							},
	{"DVI/EXT AUDIO"  						}, 
	{"AUDIO TYPE"							},
	{"NO MODULE"  							},
	{"AUDIO MODULE" 						},
	{"LOADING" 								},
	{"���"									},
	{"��С"									},
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
	{"DVI1 ���"							},
	{"���" 								},
	{"DVI_����" 							},
	{"HDMI ����"							},
	{"HDMI/EXT AUDIO"						},
	{"ONLY MB_V1.3 SUPPORT"					},
	{"SCREEN FOLLOW SCALE"					},
	{"���ز����б�"							},
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
	{"�߼�"   								},
	{"Port Tutorial" 						},  
	{"���ÿ���"  							}, 
	{"�Զ�"   								},
	{"�ֶ�"   								},
	{"Auto Set Width(pixels):"  			}, 
	{"Auto Set Height(pixels):" 			},  
	{"�����С:"   							},
	{"Panel Pixel Count:"   				},
	{"�����С ��*��:"  					}, 
	{"������� ��*��:"  					}, 
	{"Total Pixel Res:" 					},  
	{"LED��ʾ������"  						}, 
	{"������ʾ��ƴ��"  						}, 
	{"��������ʾ��ƴ��"						},   
	{"Port Set"   							},
	{"Send Card Set"   						},
	{"Port"   								},
	{"H Panels"   							},
	{"V Panels" 							},  
	{"LEDs/Row"  							}, 
	{"LEDs/Column"  						}, 
	{"H Position"							},
	{"V Position"							},
	{"Port 1 x λ��"						},
	{"Port 1 Y λ��"						},
	{"Port 2 X λ��"						},
	{"Port 2 Y λ��"						},
	{"Port 3 X λ��"						},
	{"Port 3 Y λ��"						},
	{"Port 4 X λ��"						},
	{"Port 4 Y λ��"						},
	{"Panel"								},
	{"Panel No."							},
	{"���浽�˿�"							},
	{" ���浽�˿ڳɹ�   "					},
	{" ���浽�˿�ʧ��     "					},
	{"Save To Panel"						},
	{"Reset Port Set"						},
	{"��������"								},
	{"LED MANAGEMENT"  						},   
	{"��������"								},
	{"��ʾ������"							},
	{"ADVANCED"								},
	{"SENDING CARD TYPE"					},
	{"CASCADE"								},
	{"RECEIVING CARD SEPARATE"				},
	{"������"								},
	{"������"								},
	{"ŵ��" 								},
	{"SENDING CARD SET"						},
	{"����"									},
	{"SENDING CARD POSITION"				},
	{"GAMMA"								},
	{"��ʾ������"							},
	{"����" 								},
	{"X"									},
	{"Y"									},
	{"����"									},
	{"�߶�"									},
	{"SENDING CARD MODEL"					},
	{"PARAMETER SETTING"					},
	{"HORIZONTAL CARD" 						},
	{"VERTICAL CARD" 						},
	{"����"									},
	{"�߶�"									},
	{"CONNECTION MODE"						},
	{"RECEIVING CARD POSITION" 				},   
	{"SENDING CARD NO. 1/2"					},
	{"POSITION SETTING"						},
	{"RECEIVING CARD NO."					},
	{"����ϵͳ�汾��" 						},   
	{"���Ϳ��汾��"							},
	{"���տ��汾��"							},
	{"��������ֵ"							},
	{"PORT D"								},
	{"PORT U"								},
	{"CHOOSE CABLE"							},
	{"SEND TO RECEIVER"						},
	{"SAVE TO SENDER CARD"					},
	{"Offset(A/B)X"							},
	{"Offset(A/B)Y"							},
	{"SENDING CARD NO."						},
	{"���Ϳ�����"							},
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
	{"�˿� A" 								},
	{"�˿� B"								},
	{"�˿� D" 								},
	{"�˿� U" 								},
	{"�˿� 1"								},
	{"�˿� 2"								},
	{"�˿� 3"								},
	{"�˿� 4"								},
	{"No.1" 								},
	{"No.2" 								},
	{"��" 									},
	{"READ SEND CARD PARAM",  				},   
	{"PLEASE WAIT FOR 1 MINUTE",			}, 
	{"TAKE INVALID IN PIP/SPLIT MODE"		},
	{"HDMI AUDIO"							},
	{"���Ϳ�����"							},
	{"���տ�����"							},
	{"ɫ�µ���"								},
	{"��������"								},
	{"����"									},
	{"����"									},
	{"������ɫ"								},
	{"ģ����ɫ"								},
	{"����"    								},
	{"�߶�"									},
	{"����ģ��"								},
	{"�Զ��ƶ�"								},
	{"Hold Still"							},
	{"Manual"								},
	{"����"									},
	{"�˶�"									},
	{"Cross"								},
	{"Row"									},
	{"Column"								},
	{"Rectangle"							},
	{"Video Panel Firmware"					},
	{"����"									},
	{"����"									},
	{"From USB"								},
	{"From internal memory"					},
	{"Art-Net��ʼ��ַ"						},
	{"Art-Net��Ԫ��ַ"						},
	{"Logo"									},
	{"Customized Logo"						},
	{"Custom"								},
	{"Celsius"								},
	{"Fahrenheit"							},
	{"��鷢�Ϳ� ..."						},
	{"δ��⵽���Ϳ� !"						},
	{"���Ϳ�����: ������"					},
	{"���Ϳ�����: ������"					},
	{"���Ϳ�����: ŵ��"						},

	{"������"								},
	{"������"								},
	{"������"								},
	{"������"								},
		
	{"������"								},
	{"������"								},
	{"��һ������"							},
	{"��һ������"							},
	{"�ڶ�������"							},
	{"�ڶ�������"							},
	{"����������"							},
	{"����������"							},
	{"���Ŀ�����"							},
	{"���Ŀ�����"							},
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
	{"�Զ���ֱ���"							},//outputCustomStr,
	{"HDMI2.0  �˿�"						},
	{"HDMI1.4  �˿�"						},
	{"DP       �˿�"						},
	{"DVI      �˿�"						},
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
	{"�Զ���EDID�ֱ���"						},
	{"�������"								},
	{"���������Դ"							},
	{"Ԥ��  ����Դ"							},
	{"����������Դ"							},
	{"�Ӵ�������Դ"							},
	{"�Ӵ��� X λ��"						},//pipSubPosX,
	{"�Ӵ��� Y λ��"						},//pipSubPosY,
	{"�Ӵ��� X ����"						},//pipSubSizeX,
	{"�Ӵ��� Y �߶�"						},//pipSubSizeY,

	{"����˿�ѡ��"							},//outputPortSelSrt,
	{"���1ӳ�䵽"							},//outputPort0Srt,
	{"���2ӳ�䵽"							},//outputPort1Srt,
	{"���3ӳ�䵽"							},//outputPort2Srt,
	{"���4ӳ�䵽"							},//outputPort3Srt,

	{"�������"								},//outputTpyeStr,
	{"���λ��"								},//bitWidthSrt,
	{"���ֵ��"								},//rangeStr,

	{"ͼ�����"								},//layerParamSrt,
	{"ͼ�� A ����"							},//layerAHSizeSrt,
	{"ͼ�� A �߶�"							},//layerAVSizeSrt,
	{"ͼ�� A X λ��"						},//layerAHPosSrt,
	{"ͼ�� A Y λ��"						},//layerAVPosSrt,
	{"ͼ�� A ƴ�� ��ʼ"						},//layerASplitStartSrt,
	{"ͼ�� A �ü� ��ʼ"						},//layerACropStartSrt,

	{"ͼ�� B ����"							},//layerBHSizeSrt,
	{"ͼ�� B �߶�"							},//layerBVSizeSrt,
	{"ͼ�� B X λ��"						},//layerBHPosSrt,
	{"ͼ�� B Y λ��"						},//layerBVPosSrt,
	{"ͼ�� B ƴ�� ��ʼ"						},//layerBSplitStartSrt,
	{"ͼ�� B �ü� ��ʼ"						},//layerBCropStartSrt,

	{"ͼ�� C ����"							},//layerCHSizeSrt,
	{"ͼ�� C �߶�"							},//layerCVSizeSrt,
	{"ͼ�� C X λ��"						},//layerCHPosSrt,
	{"ͼ�� C Y λ��"						},//layerCVPosSrt,
	{"ͼ�� C ƴ�� ��ʼ"						},//layerCSplitStartSrt,
	{"ͼ�� C �ü� ��ʼ"						},//layerCCropStartSrt,

	{"ͼ�� D ����"							},//layerDHSizeSrt,
	{"ͼ�� D �߶�"							},//layerDVSizeSrt,
	{"ͼ�� D X λ��"						},//layerDHPosSrt,
	{"ͼ�� D Y λ��"						},//layerDVPosSrt,
	{"ͼ�� D ƴ�� ��ʼ"						},//layerDSplitStartSrt,
	{"ͼ�� D �ü� ��ʼ"						},//layerDCropStartSrt,

	{"΢��"									},//FineSrt,

	{"�����DVI1 -> Aͼ��"					},//outPutDvi1srt,
	{"�����DVI2 -> Bͼ��"					},//outPutDvi2srt,
	{"�����DVI3 -> Cͼ��"					},//outPutDvi3srt,
	{"�����DVI4 -> Dͼ��"					},//outPutDvi4srt,


};


#else


#endif

#endif
