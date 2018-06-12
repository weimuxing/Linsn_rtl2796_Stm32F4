#ifndef _IDT_TIMING_COMMANDER_H
#define _IDT_TIMING_COMMANDER_H

#include "define.h"


typedef struct
{
	int DSMint;
	int DSMfrac;
}IDTTimming_Q_Typedef;

typedef struct
{
	IDTTimming_Q_Typedef IDTTimming_Q0;
	IDTTimming_Q_Typedef IDTTimming_Q1;
	IDTTimming_Q_Typedef IDTTimming_Q2;
	IDTTimming_Q_Typedef IDTTimming_Q3;
}IDTTimming_Typedef;

extern IDTTimming_Typedef IDTTimming;

void Calculate_IDTTimming(IDTTimming_Typedef *IDTTimming,u64 Fout0,u64 Fout1,u64 Fout2,u64 Fout3);
void Calculate_IDTTimmingIndependent(IDTTimming_Typedef *IDTTimming,u64 Fout,u8 port);
#endif
