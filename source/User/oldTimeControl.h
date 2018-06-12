#ifndef _OLD_TIME_CONTROL_H_
#define _OLD_TIME_CONTROL_H_

#include "define.h"

enum {
	STATE_INIT,
	STATE_LOAD_DATA,
	STATE_COMPARE_TIME,
};

void task_oldTimeControl(void);
void SetTimeControlState(int state);










#endif //_OLD_TIME_CONTROL_H_



























