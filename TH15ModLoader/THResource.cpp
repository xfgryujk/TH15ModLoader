#include "stdafx.h"
#include "THResource.h"
#include "Hook.h"
#include <stdlib.h>


namespace THResource
{
	static void* const READ_RES_HOOK_ADDR = (void*)0x402E0C;
	void* g_newReadResEntry = NULL;


	void* __stdcall MyReadRes(const char* resName, DWORD* pnBytesRead)
	{
		char path[MAX_PATH] = "data\\";
		strcat_s(path, resName);

		HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			return NULL;
		DWORD size = GetFileSize(file, NULL);
		void* buffer = malloc(size);
		ReadFile(file, buffer, size, &size, NULL);
		if (pnBytesRead != NULL)
			*pnBytesRead = size;
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

	bool Init()
	{
		Hook::HookInlineHook(READ_RES_HOOK_ADDR, MyReadResWrapper, &g_newReadResEntry, 6);
		return g_newReadResEntry != NULL;
	}

	bool Uninit()
	{
		if (g_newReadResEntry != NULL)
			Hook::UnhookInlineHook(READ_RES_HOOK_ADDR, &g_newReadResEntry, 6);
		return true;
	}
}
