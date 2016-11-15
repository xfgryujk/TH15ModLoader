// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "DllHijack.h"
#include "THInit.h"
#include "THLogic.h"
#include "ModManager.h"
using namespace tml;


namespace
{
	bool InitModules()
	{
		if (!DllHijack::GetInstance().IsReady())
		{
			MessageBox(NULL, "DllHijack初始化失败！", "TML", MB_ICONERROR);
			return false;
		}
		if (!THInit::GetInstance().IsReady())
		{
			MessageBox(NULL, "THInit初始化失败！", "TML", MB_ICONERROR);
			return false;
		}
		if (!THLogic::GetInstance().IsReady())
		{
			MessageBox(NULL, "THLogic初始化失败！", "TML", MB_ICONERROR);
			return false;
		}
		return true;
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!InitModules())
			return FALSE;

		ModManager::GetInstance().LoadDir("mods");
		break;

	case DLL_PROCESS_DETACH:
		ModManager::GetInstance().UnloadAll();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

