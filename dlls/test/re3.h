#pragma once
#undef LoadImage
#include "stdafx.h"

EXTERN_C void Disp_mess(int x, int y, BYTE* str, u32 Attr);
EXTERN_C void Fade_adjust(int a1, char a2, char a3, int a4, int a5, int a6, __int16 a7);
EXTERN_C int Fade_status(int);

// sound stuff
EXTERN_C void Fade_set(int, int, short, short);

EXTERN_C void __cdecl Flg_on(DWORD* flg, DWORD index);
EXTERN_C void __cdecl Flg_off(DWORD* flg, DWORD index);
EXTERN_C DWORD __cdecl Flg_ck(DWORD* flg, DWORD index);

EXTERN_C void Bg_set_mode(int, int);

EXTERN_C void Init_system();
EXTERN_C void ClearOTagR(DWORD* ot, int size);
EXTERN_C void Moji_init();
EXTERN_C void Moji_trans();
EXTERN_C void Pad_read();
EXTERN_C void Reboot();
EXTERN_C void Bgm_update();
EXTERN_C void sub_429540();
/*EXTERN_C*/ void Display();
EXTERN_C void sub_428E00();
EXTERN_C void DisplayBlack();
EXTERN_C void QuitGame();
EXTERN_C void* sub_50A440(u32);
EXTERN_C void Disp_ascii16(unsigned __int16 x, unsigned __int16 y, char* str, ...);
EXTERN_C int Sound_kill(int a1);
EXTERN_C void sub_42C060(int a1);
EXTERN_C void Disp_pause();

EXTERN_C DWORD dword_A67CE0, dwQuit_game, dword_A3A090;
EXTERN_C void* pBuffer;
EXTERN_C BYTE VSync_rate, byte_A67CDC, byte_A5DBC0, byte_A5F270, byte_A5F271;

EXTERN_C BYTE byte_A5DB88, byte_A67CD5, byte_A67CD9;

EXTERN_C void InverseBG();
EXTERN_C void SetGeomOffset(int, int);
EXTERN_C BYTE* PsxToRam(DWORD);
EXTERN_C void sub_4277B0(int*);


DWORD __cdecl Load_file(char* filename, void* buffer);
DWORD __cdecl Load_file_ex(char* filename, void* buffer);
DWORD __cdecl Load_file_seg(char* filename, void* buffer, int pos, int size);
DWORD __cdecl Load_file_seg_ex(char* filename, void* buffer, int pos, int size);

EXTERN_C void Set_light_data();
EXTERN_C void sub_433A10();

int VSync(int mode);