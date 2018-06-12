#ifndef _DELAY_CALL_H_
#define _DELAY_CALL_H_

typedef void (*DelayCallFunc)(void);

int AddDelayCallFunc(DelayCallFunc pFunc, int delayTime);
int RemoveDelayCallFunc(DelayCallFunc pFunc);
void delayCall_task(void);












#endif

