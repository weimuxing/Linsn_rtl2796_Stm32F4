#ifndef UPDATE_FIRMWARE_H_
#define UPDATE_FIRMWARE_H_

#include "define.h"

enum{
	GUID_IDX_MCU,
	GUID_IDX_VIDEO,
	GUID_IDX_FPGA,
	GUID_IDX_FONT,
	
	GUID_MAX_SIZE,
	UNDEFINE_GUID = 0xff,
};

int GetTypeGUID(u8 * guid);

void StartUpdateProgram(void);
void UpdateProgramData(u8 *buffer, int size);
void EndUpdateProgram(void);
int GetUpdateState(void);

int GetUpdateError(void);





#endif

