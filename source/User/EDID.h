#ifndef _EDID_H_
#define _EDID_H_

#include "define.h"

enum {
	EDID_PORT_1,
	EDID_PORT_2,
	EDID_PORT_3,
	//EDID_PORT_4,
	EDID_PORT_MAX_SIZE = 3,
};

void WriteEDID_EEPROM(int EDID_PORT_X, int address, u8 *buffer, int size);
void ReadEDID_EEPROM(int EDID_PORT_X, int address, u8 *buffer, int size);
void SetHPD_Pin(int EDID_PORT_X, int highOrLow); // highOrLow: 0 -- Low, 1 -- Hight
void EdidHpInit(void); // highOrLow: 0 -- Low, 1 -- Hight

#endif

