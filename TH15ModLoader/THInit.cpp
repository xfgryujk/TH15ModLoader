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
		void* g_newOnInitEntry = NULL;
		void* g_newOnUninitEntry = NULL;
		void* g_newReadResEntry = NULL;
	}

	// 初始化
	
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

	// 结束

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

	// 读取资源

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


	// 本模块初始化

namespace THInit
{
	bool Init()
	{
		bool res = true;
		res = res && HookInlineHook(ON_INIT_HOOK_ADDR, MyOnInitWrapper, &g_newOnInitEntry, 11);
		res = res && HookInlineHook(ON_UNINIT_HOOK_ADDR, MyOnUninitWrapper, &g_newOnUninitEntry);
		res = res && HookInlineHook(READ_RES_HOOK_ADDR, MyReadResWrapper, &g_newReadResEntry, 6);
		return res;
	}

	bool Uninit()
	{
		bool res = true;
		res = res && UnhookInlineHook(ON_INIT_HOOK_ADDR, &g_newOnInitEntry, 11);
		res = res && UnhookInlineHook(ON_UNINIT_HOOK_ADDR, &g_newOnUninitEntry);
		res = res && UnhookInlineHook(READ_RES_HOOK_ADDR, &g_newReadResEntry, 6);
		return res;
	}
}
}
