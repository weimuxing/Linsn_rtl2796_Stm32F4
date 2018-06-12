#ifndef TIME_CONTROL_H_
#define TIME_CONTROL_H_

#include "define.h"

//TimeControl:
//每组TimeControl由TimeControlID, TimeControlValid,TimeControlEnable和TimeControlBlock[128]组成
//eeprom共预留了300组TimeControl的存储空间
//TimeControl_0                    只存由上位机设置的TimeControl块,且上位机只设置这一块TimeControl数据
//TimeControl_0x10~0x2f     保存ext_7读出来的播放列表,只预留32块
//TimeControl_0x30~0x4f     保存ext_8读出来的播放列表,只预留32块
//TimeControl_0x50~0x6f     保存ext_9读出来的播放列表,只预留32块


#define TIMECOTROL_USB1PLSYLISTMINBLOCK	    0x10            //对应ext_7
#define TIMECOTROL_USB2PLSYLISTMINBLOCK	    0x30            //对应ext_8
#define TIMECOTROL_USB3PLSYLISTMINBLOCK  	0x50            //对应ext_9

#define TIMECOTROL_USBPLSYLIST_MAXPERPORT	0x20            //每个USB播放列表预留的块数

#define timeControl_getStartMonth() (pTimeControl->timeControlBlock[2])
#define timeControl_getEndMonth() (pTimeControl->timeControlBlock[6])
#define timeControl_getStartDay() (pTimeControl->timeControlBlock[3])
#define timeControl_getEndDay() (pTimeControl->timeControlBlock[7])
#define timeControl_getStartWeekday() (pTimeControl->timeControlBlock[8])  // 0 -- monday, 6 -- sunday
#define timeControl_getEndWeekday() (pTimeControl->timeControlBlock[9])
#define timeControl_getStartEveryDay() (pTimeControl->timeControlBlock[10])
#define timeControl_getEndEveryDay() (pTimeControl->timeControlBlock[11])
#define timeControl_getStartHour() (pTimeControl->timeControlBlock[12])
#define timeControl_getEndHour() (pTimeControl->timeControlBlock[14])
#define timeControl_getStartMinute() (pTimeControl->timeControlBlock[13])
#define timeControl_getEndMinute() (pTimeControl->timeControlBlock[15])


void timeControl_init(void);
void timeControl_task(void);





#endif // TIME_CONTROL_H_



