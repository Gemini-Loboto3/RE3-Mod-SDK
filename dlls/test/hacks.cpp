#include "stdafx.h"
#include "global.h"
#include "re3.h"
#include "re3_input.h"
#include "inject.h"

EXTERN_C DWORD dword_A3AE68, dword_A3AE40[];
EXTERN_C BYTE byte_52DEA0[], byte_A51DC9;
EXTERN_C WORD word_52DEC8, word_52DECA;
EXTERN_C void door_scheduler();
EXTERN_C void Door_trans();
EXTERN_C void Snd_mute(int);
EXTERN_C void SsExSeqSetVol4(int vol);
EXTERN_C void Task_sleep(int);

void Door_r1()
{
	while ((G.System_flg & 0x800000) != 0)
		Task_sleep(1);
	if ((*(BYTE*)(dword_A3AE40[0] + 193) & 0x40) == 0)
		VSync_rate = 1;
	while (byte_A51DC9)
	{
		if ((G.Status_flg & 0x800) == 0)
			G.field_7830 = 0;
		door_scheduler();
		if (G.Pad_trg & (PAD_TRIANGLE))
		{
			SsExSeqSetVol4(0);
			byte_A51DC9 = 0;
		}
		Door_trans();
		++*(WORD*)(dword_A3AE68 + 550);
		Task_sleep(1);
	}
}

void Inject_hacks()
{
	INJECT(0x46C940, Door_r1);
}