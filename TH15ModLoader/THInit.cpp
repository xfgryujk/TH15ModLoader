#include "stdafx.h"
#include "THInit.h"
#include "Hook.h"
#include "THAPI.h"


namespace tml
{
	namespace
	{
		void* const ON_INIT_HOOK_ADDR = (void*)0x4719B0;
		void* const ON_UNINIT_HOOK_ADDR = (void*)0x471CB8;
		void* const READ_RES_HOOK_ADDR = (void*)0x402E0C;
		void* const INSERT_STRUCT2_TO_STRUCT1_HOOK_ADDR = (void*)0x401390;
		void* const INSERT_STRUCT2_TO_STRUCT1REN_HOOK_ADDR = (void*)0x401440;
		void* const DELETE_STRUCT2_HOOK_ADDR = (void*)0x4018A0;

		void* g_newOnInitEntry = NULL;
		void* g_newOnUninitEntry = NULL;
		void* g_newReadResEntry = NULL;
		int(__stdcall* g_newInsertStruct2ToStruct1Entry)(THAPI::Struct2* pStruct2, DWORD order) = NULL;
		int(__stdcall* g_newInsertStruct2ToStruct1RenEntry)(THAPI::Struct2* pStruct2, DWORD order) = NULL;
		void* g_newDeleteStruct2Entry = NULL;
	}

	// 程序初始化
	
	void __stdcall MyOnInit()
	{
		g_thEventBus.Post(THInitEvent::OnInit);
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

	// 程序结束

	void __stdcall MyOnUninit()
	{
		g_thEventBus.Post(THInitEvent::OnUninit);
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

	// ReadResourceEvent

	ReadResourceEvent::ReadResourceEvent(LPCSTR resName, void*& resBuffer, DWORD*& pnBytesRead) :
		m_resName(resName), m_resBuffer(resBuffer), m_pnBytesRead(pnBytesRead) { }

	void ReadResourceEvent::SetResBuffer(void* buffer, size_t size)
	{
		if (m_resBuffer != NULL)
			THAPI::free(m_resBuffer);
		if (buffer == NULL)
		{
			m_resBuffer = NULL;
			if (m_pnBytesRead != NULL)
				*m_pnBytesRead = 0;
		}
		else
		{
			m_resBuffer = THAPI::malloc(size);
			memcpy(m_resBuffer, buffer, size);
			if (m_pnBytesRead != NULL)
				*m_pnBytesRead = size;
		}
	}

	// 读取资源

	void* __stdcall MyReadRes(const char* resName, DWORD* pnBytesRead)
	{
		char path[MAX_PATH] = "data\\";
		strcat_s(path, resName);
		void* buffer = NULL;

		HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file != INVALID_HANDLE_VALUE)
		{
			DWORD size = GetFileSize(file, NULL);
			buffer = THAPI::malloc(size);
			ReadFile(file, buffer, size, &size, NULL);
			CloseHandle(file);
			if (pnBytesRead != NULL)
				*pnBytesRead = size;
		}

		ReadResourceEvent event_(resName, buffer, pnBytesRead);
		g_thEventBus.Post(THInitEvent::OnReadResource, event_);

		return buffer;
	}

	__declspec(naked) void MyReadResWrapper()
	{
		__asm
		{
			push [ebp-8h]
			push ebx
			call MyReadRes
			test eax, eax
			jnz NotNull
			// 返回NULL，交给原函数
			mov eax, g_newReadResEntry
			jmp eax

			NotNull: // 不是NULL
			mov edi, eax
			mov eax, 402E79h
			jmp eax
		}
	}

	// Struct2Event

	Struct2Event::Struct2Event(THAPI::Struct2* pStruct2, DWORD order) : 
		m_pStruct2(pStruct2), m_order(order) { }

	// Struct2插入Struct1

	int __stdcall MyInsertStruct2ToStruct1(THAPI::Struct2* pStruct2, DWORD order)
	{
		int res = 1; // 返回非0代表插入失败
		Struct2Event event_(pStruct2, order);
		if (g_thEventBus.Post(THInitEvent::OnInsertStruct2ToStruct1, event_))
			res = g_newInsertStruct2ToStruct1Entry(pStruct2, order);
		return res;
	}

	// Struct2插入Struct1_Ren

	int __stdcall MyInsertStruct2ToStruct1Ren(THAPI::Struct2* pStruct2, DWORD order)
	{
		int res = 1; // 返回非0代表插入失败
		Struct2Event event_(pStruct2, order);
		if (g_thEventBus.Post(THInitEvent::OnInsertStruct2ToStruct1Ren, event_))
			res = g_newInsertStruct2ToStruct1RenEntry(pStruct2, order);
		return res;
	}

	// Struct2准备断开和析构

	void __stdcall MyDeleteStruct2(THAPI::Struct2* pStruct1, THAPI::Struct2* pStruct2)
	{
		Struct2Event event_(pStruct2, pStruct2->order);
		if (g_thEventBus.Post(THInitEvent::OnDeleteStruct2, event_))
		{
			__asm
			{
				push pStruct2
				mov ecx, pStruct1
				call g_newDeleteStruct2Entry
			}
		}
	}

	__declspec(naked) void MyDeleteStruct2Wrapper()
	{
		__asm
		{
			pop eax		// 返回地址出栈
			push ecx	// pStruct1入栈
			push eax	// 返回地址入栈
			jmp MyDeleteStruct2
		}
	}


	// 本模块初始化

namespace THInit
{
	bool Init()
	{
		bool res = true;
		res = res && HookInlineHook(ON_INIT_HOOK_ADDR, MyOnInitWrapper, &g_newOnInitEntry, 11);
		res = res && HookInlineHook(ON_UNINIT_HOOK_ADDR, MyOnUninitWrapper, &g_newOnUninitEntry);
		res = res && HookInlineHook(READ_RES_HOOK_ADDR, MyReadResWrapper, &g_newReadResEntry, 6);
		res = res && HookInlineHook(INSERT_STRUCT2_TO_STRUCT1_HOOK_ADDR, MyInsertStruct2ToStruct1, (void**)&g_newInsertStruct2ToStruct1Entry, 9);
		res = res && HookInlineHook(INSERT_STRUCT2_TO_STRUCT1REN_HOOK_ADDR, MyInsertStruct2ToStruct1Ren, (void**)&g_newInsertStruct2ToStruct1RenEntry, 9);
		res = res && HookInlineHook(DELETE_STRUCT2_HOOK_ADDR, MyDeleteStruct2Wrapper, &g_newDeleteStruct2Entry, 6);
		return res;
	}

	bool Uninit()
	{
		bool res = true;
		res = res && UnhookInlineHook(ON_INIT_HOOK_ADDR, &g_newOnInitEntry, 11);
		res = res && UnhookInlineHook(ON_UNINIT_HOOK_ADDR, &g_newOnUninitEntry);
		res = res && UnhookInlineHook(READ_RES_HOOK_ADDR, &g_newReadResEntry, 6);
		res = res && UnhookInlineHook(INSERT_STRUCT2_TO_STRUCT1_HOOK_ADDR, (void**)&g_newInsertStruct2ToStruct1Entry, 9);
		res = res && UnhookInlineHook(INSERT_STRUCT2_TO_STRUCT1REN_HOOK_ADDR, (void**)&g_newInsertStruct2ToStruct1RenEntry, 9);
		res = res && UnhookInlineHook(DELETE_STRUCT2_HOOK_ADDR, &g_newDeleteStruct2Entry, 6);
		return res;
	}
}
}
