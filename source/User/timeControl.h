#ifndef TIME_CONTROL_H_
#define TIME_CONTROL_H_

#include "define.h"

//TimeControl:
//ÿ��TimeControl��TimeControlID, TimeControlValid,TimeControlEnable��TimeControlBlock[128]���
//eeprom��Ԥ����300��TimeControl�Ĵ洢�ռ�
//TimeControl_0                    ֻ������λ�����õ�TimeControl��,����λ��ֻ������һ��TimeControl����
//TimeControl_0x10~0x2f     ����ext_7�������Ĳ����б�,ֻԤ��32��
//TimeControl_0x30~0x4f     ����ext_8�������Ĳ����б�,ֻԤ��32��
//TimeControl_0x50~0x6f     ����ext_9�������Ĳ����б�,ֻԤ��32��


#define TIMECOTROL_USB1PLSYLISTMINBLOCK	    0x10            //��Ӧext_7
#define TIMECOTROL_USB2PLSYLISTMINBLOCK	    0x30            //��Ӧext_8
#define TIMECOTROL_USB3PLSYLISTMINBLOCK  	0x50            //��Ӧext_9

#define TIMECOTROL_USBPLSYLIST_MAXPERPORT	0x20            //ÿ��USB�����б�Ԥ���Ŀ���

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



