#include "DelayCall.h"
#include "systick.h"
#include "define.h"

#define DELAY_CALL_FUNC_MAX 8
static DelayCallFunc _pFunc[DELAY_CALL_FUNC_MAX] = {NULL};
static u32 _startTime[DELAY_CALL_FUNC_MAX] = {0};
static u32 _delayTime[DELAY_CALL_FUNC_MAX] = {0};


int AddDelayCallFunc(DelayCallFunc pFunc, int delayTime)
{
	int i;
	for(i=0; i<DELAY_CALL_FUNC_MAX; i++)
	{
		if(_pFunc[i] == NULL)
		{
			break;
		}
		if(_pFunc[i] == pFunc)
		{
			break;
		}
	}
	if(i<DELAY_CALL_FUNC_MAX)
	{
		_pFunc[i] = pFunc;
		_startTime[i] = GetSysTick();
		_delayTime[i] = delayTime;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}
int RemoveDelayCallFunc(DelayCallFunc pFunc)
{
	int i;
	for(i=0; i<DELAY_CALL_FUNC_MAX; i++)
	{
		
		if(_pFunc[i] == pFunc)
		{
			break;
		}
	}
	if(i<DELAY_CALL_FUNC_MAX)
	{
		_pFunc[i] = NULL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void delayCall_task(void)
{
	int i;
	for(i=0; i<DELAY_CALL_FUNC_MAX; i++)
	{
		if(_pFunc[i] != NULL)
		{
			if(isSysTickTimeout(_startTime[i],_delayTime[i]))
			{
				_pFunc[i]();
				_pFunc[i] = NULL;
			}
		}
	}
}



