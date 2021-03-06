﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "DllHijack.h"
#include "THInit.h"
#include "THLogic.h"
#include "THRender.h"
#include "ModManager.h"
using namespace tml;


namespace
{
#define InitModule(module) \
	if (!module::GetInstance().IsReady()) \
	{ \
		MessageBox(NULL, #module "初始化失败！", "TML", MB_ICONERROR); \
		return false; \
	}

	bool InitModules()
	{
		InitModule(DllHijack)
		InitModule(THInit)
		InitModule(THLogic)
		InitModule(THRender)
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

