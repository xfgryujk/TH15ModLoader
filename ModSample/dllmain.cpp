﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <THInit.h>


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
		g_onInitListenerID = THModEvent::g_thEventBus.AddListener(THInit::THInitEvent::OnInit, 
			[](THModEvent::EventBase*)
			{
				MessageBox(NULL, "OnInit!", "ModSample", MB_OK);
			}
		);
		g_onUninitListenerID = THModEvent::g_thEventBus.AddListener(THInit::THInitEvent::OnUninit, 
			[](THModEvent::EventBase*)
			{
				MessageBox(NULL, "OnUninit!", "ModSample", MB_OK);
			}
		);
		break;

	case DLL_PROCESS_DETACH:
		THModEvent::g_thEventBus.DeleteListener(THInit::THInitEvent::OnInit, g_onInitListenerID);
		THModEvent::g_thEventBus.DeleteListener(THInit::THInitEvent::OnUninit, g_onUninitListenerID);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
