#include "stdafx.h"
#include "THInit.h"
#include "Hook.h"


namespace THInit
{
	static void* const ON_INIT_HOOK_ADDR = (void*)0x4719B0;
	static void* const ON_UNINIT_HOOK_ADDR = (void*)0x471CB8;
	static void* g_newOnInitEntry = NULL;
	static void* g_newOnUninitEntry = NULL;


	void __stdcall MyOnInit()
	{
		THModEvent::g_thEventBus.Post(THInitEvent::OnInit);
	}

	__declspec(naked) void MyOnInitWrapper()
	{
		__asm
		{
			call MyOnInit
			mov eax, g_newOnInitEntry
			jmp eax
		}
	}

	void __stdcall MyOnUninit()
	{
		THModEvent::g_thEventBus.Post(THInitEvent::OnUninit);
	}

	__declspec(naked) void MyOnUninitWrapper()
	{
		__asm
		{
			call MyOnUninit
			mov eax, g_newOnUninitEntry
			jmp eax
		}
	}

	bool Init()
	{
		bool res = true;
		res = res && Hook::HookInlineHook(ON_INIT_HOOK_ADDR, MyOnInitWrapper, &g_newOnInitEntry, 11);
		res = res && Hook::HookInlineHook(ON_UNINIT_HOOK_ADDR, MyOnUninitWrapper, &g_newOnUninitEntry);
		return res;
	}

	bool Uninit()
	{
		bool res = true;
		res = res && Hook::UnhookInlineHook(ON_INIT_HOOK_ADDR, &g_newOnInitEntry, 11);
		res = res && Hook::UnhookInlineHook(ON_UNINIT_HOOK_ADDR, &g_newOnUninitEntry);
		return res;
	}
}
