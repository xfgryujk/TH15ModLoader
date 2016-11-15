// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <THInitEvent.h>
using namespace tml;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_eventBus.AddListener(THInitEvent::OnInit, 
			[](EventBase*)
			{
				MessageBox(NULL, "OnInit!", "ModSample", MB_OK);
			}
		, hModule);
		g_eventBus.AddListener(THInitEvent::OnUninit, 
			[](EventBase*)
			{
				MessageBox(NULL, "OnUninit!", "ModSample", MB_OK);
			}
		, hModule);
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

