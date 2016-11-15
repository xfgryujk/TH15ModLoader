// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <THInitEvent.h>
using namespace tml;


int g_onInitListenerID;
int g_onUninitListenerID;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_onInitListenerID = g_eventBus.AddListener(THInitEvent::OnInit, 
			[](EventBase*)
			{
				MessageBox(NULL, "OnInit!", "ModSample", MB_OK);
			}
		);
		g_onUninitListenerID = g_eventBus.AddListener(THInitEvent::OnUninit, 
			[](EventBase*)
			{
				MessageBox(NULL, "OnUninit!", "ModSample", MB_OK);
			}
		);
		break;

	case DLL_PROCESS_DETACH:
		g_eventBus.DeleteListener(THInitEvent::OnInit, g_onInitListenerID);
		g_eventBus.DeleteListener(THInitEvent::OnUninit, g_onUninitListenerID);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

