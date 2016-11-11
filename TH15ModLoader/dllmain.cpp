// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "DllHijack.h"
#include "THInit.h"
#include "ModManager.h"
using namespace tml;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!DllHijack::Init())
			return FALSE;

		if (!THInit::Init())
			return FALSE;

		g_modManager.LoadDir("mods");
		break;

	case DLL_PROCESS_DETACH:
		g_modManager.UnloadAll();

		if (!THInit::Uninit())
			return FALSE;

		if (!DllHijack::Uninit())
			return FALSE;
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

