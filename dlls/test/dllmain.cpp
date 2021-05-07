// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"

void Modsdk_init();
void Modsdk_close();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Modsdk_init();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Modsdk_close();
		break;
	}
	return TRUE;
}

