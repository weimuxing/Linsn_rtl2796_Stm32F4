#include <stdio.h>
#include <math.h>
#include "IDT_Timing_Commander.h"

//u64 fvco = 2800000000ll;
u64 fvco = 2500000000ll;
//#define FDC_PLL 0x40000000


IDTTimming_Typedef IDTTimming;

void FractionalDividerCalculate(IDTTimming_Q_Typedef *IDTTimming_Q1Q2Q3,u64 Fvco, u64 Fout)
{
    int Nint, Nfrac;
    u64 temp;
    
    Nint = Fvco/2/Fout;
    temp = (Fvco/2 -Fout*Nint)*0x1000000; 
    Nfrac = (temp+Fout/2)/Fout;

//    printf("Nint = %d 0x%X, Nfrac = %d 0x%X\r\n", Nint, Nint, Nfrac, Nfrac);

    IDTTimming_Q1Q2Q3->DSMint = Nint;
    IDTTimming_Q1Q2Q3->DSMfrac = Nfrac;
}

void Calculate_IDTTimming(IDTTimming_Typedef *IDTTimming,u64 Fout0,u64 Fout1,u64 Fout2,u64 Fout3)
{
  //  printf("Fvco = %u\r\n", fvco);
    FractionalDividerCalculate(&IDTTimming->IDTTimming_Q0, fvco, Fout0);
    FractionalDividerCalculate(&IDTTimming->IDTTimming_Q1, fvco, Fout1);
    FractionalDividerCalculate(&IDTTimming->IDTTimming_Q2, fvco, Fout2);
    FractionalDividerCalculate(&IDTTimming->IDTTimming_Q3, fvco, Fout3);
}


void Calculate_IDTTimmingIndependent(IDTTimming_Typedef *IDTTimming,u64 Fout,u8 port)
{
  //  printf("Fvco = %u\r\n", fvco);
  	switch(port)
  	{
		case 0:
			FractionalDividerCalculate(&IDTTimming->IDTTimming_Q0, fvco, Fout);
		break;
		case 1:
			FractionalDividerCalculate(&IDTTimming->IDTTimming_Q1, fvco, Fout);
		break;
		case 2:
			FractionalDividerCalculate(&IDTTimming->IDTTimming_Q2, fvco, Fout);
		break;
		case 3:
			FractionalDividerCalculate(&IDTTimming->IDTTimming_Q3, fvco, Fout);
		break;

		default:
			FractionalDividerCalculate(&IDTTimming->IDTTimming_Q2, fvco, Fout);
		break;
	}
    //FractionalDividerCalculate(&IDTTimming->IDTTimming_Q0, fvco, Fout0);
    //FractionalDividerCalculate(&IDTTimming->IDTTimming_Q1, fvco, Fout1);
    //FractionalDividerCalculate(&IDTTimming->IDTTimming_Q2, fvco, Fout2);
    //FractionalDividerCalculate(&IDTTimming->IDTTimming_Q3, fvco, Fout3);
}


