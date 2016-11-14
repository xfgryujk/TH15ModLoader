#include "stdafx.h"
#include "THInit.h"
#include "Hook.h"
#include "THFunction.h"


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
		void* const INIT_STAGE_HOOK_ADDR = (void*)0x426A00;
		void* const UNINIT_STAGE_HOOK_ADDR = (void*)0x426820;
		void* const INIT_UNIT_HOOK_ADDR = (void*)0x426050;
		void* const UNINIT_UNIT_HOOK_ADDR = (void*)0x425330;

		void* g_newOnInitEntry = NULL;
		void* g_newOnUninitEntry = NULL;
		void* g_newReadResEntry = NULL;
		int(__stdcall* g_newInsertStruct2ToStruct1Entry)(::THAPI::Struct2* pStruct2, DWORD order) = NULL;
		int(__stdcall* g_newInsertStruct2ToStruct1RenEntry)(::THAPI::Struct2* pStruct2, DWORD order) = NULL;
		void* g_newDeleteStruct2Entry = NULL;
		::THAPI::Stage*(__fastcall* g_newInitStageEntry)(const char* eclName) = NULL;
		void*(__fastcall* g_newUninitStageEntry)(::THAPI::Stage* pStage) = NULL;
		void* g_newInitUnitEntry = NULL;
		void* g_newUninitUnitEntry = NULL;
	}


	// 程序初始化
	
	void __stdcall MyOnInit()
	{
		g_eventBus.Post(THInitEvent::OnInit);
	}

	__declspec(naked) void MyOnInitWrapper()
	{
		__asm
		{
			call MyOnInit
			jmp g_newOnInitEntry
		}
	}

	// 程序结束

	void __stdcall MyOnUninit()
	{
		g_eventBus.Post(THInitEvent::OnUninit);
	}

	__declspec(naked) void MyOnUninitWrapper()
	{
		__asm
		{
			call MyOnUninit
			jmp g_newOnUninitEntry
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
		g_eventBus.Post(THInitEvent::OnReadResource, event_);

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
			jmp g_newReadResEntry

			NotNull: // 不是NULL
			mov edi, eax
			mov eax, 402E79h
			jmp eax
		}
	}


	// Struct2Event

	Struct2Event::Struct2Event(::THAPI::Struct2* pStruct2, DWORD order) :
		m_pStruct2(pStruct2), m_order(order) { }

	// Struct2插入Struct1

	int __stdcall MyInsertStruct2ToStruct1(::THAPI::Struct2* pStruct2, DWORD order)
	{
		int res = 1; // 返回非0代表插入失败
		Struct2Event event_(pStruct2, order);
		if (g_eventBus.Post(THInitEvent::OnInsertStruct2ToStruct1, event_))
			res = g_newInsertStruct2ToStruct1Entry(pStruct2, order);
		return res;
	}

	// Struct2插入Struct1_Ren

	int __stdcall MyInsertStruct2ToStruct1Ren(::THAPI::Struct2* pStruct2, DWORD order)
	{
		int res = 1; // 返回非0代表插入失败
		Struct2Event event_(pStruct2, order);
		if (g_eventBus.Post(THInitEvent::OnInsertStruct2ToStruct1Ren, event_))
			res = g_newInsertStruct2ToStruct1RenEntry(pStruct2, order);
		return res;
	}

	// Struct2准备断开和析构

	void __stdcall MyDeleteStruct2(::THAPI::Struct2* pStruct1, ::THAPI::Struct2* pStruct2)
	{
		Struct2Event event_(pStruct2, pStruct2->order);
		if (g_eventBus.Post(THInitEvent::OnDeleteStruct2, event_))
		{
			__asm
			{
				push pStruct2
				mov ecx, pStruct1
				call g_newDeleteStruct2Entry
			}
		}
	}

#define ThiscallToStdcallWrapper(f) \
	__declspec(naked) void f##Wrapper() \
	{ \
		__asm pop eax\
		__asm push ecx \
		__asm push eax \
		__asm jmp f \
	}

	ThiscallToStdcallWrapper(MyDeleteStruct2)


	// StageEvent

	StageEvent::StageEvent(::THAPI::Stage* pStage) :
		m_pStage(pStage) { }

	// 关卡初始化后

	::THAPI::Stage* __fastcall MyInitStage(const char* eclName)
	{
		::THAPI::Stage* pStage = g_newInitStageEntry(eclName);
		StageEvent event_(pStage);
		g_eventBus.Post(THInitEvent::OnInitStage, event_);
		return pStage;
	}

	// 准备析构关卡

	void* __fastcall MyUninitStage(::THAPI::Stage* pStage)
	{
		StageEvent event_(pStage);
		g_eventBus.Post(THInitEvent::OnUninitStage, event_);
		return g_newUninitStageEntry(pStage);
	}


	// InitUnitEvent

	InitUnitEvent::InitUnitEvent(LPCSTR eclFuncName, void* pArg) :
		m_eclFuncName(eclFuncName), m_pArg(pArg) { }

	// 准备初始化单位

	::THAPI::Unit* __stdcall MyInitUnit(::THAPI::Stage* pStage, const char* eclFuncName, void* pArg, DWORD unknown)
	{
		::THAPI::Unit* res = NULL;
		InitUnitEvent event_(eclFuncName, pArg);
		if (g_eventBus.Post(THInitEvent::OnInitUnit, event_))
		{
			__asm
			{
				push unknown
				push pArg
				push eclFuncName
				mov ecx, pStage
				call g_newInitUnitEntry
				mov res, eax
			}
		}
		return res;
	}

	ThiscallToStdcallWrapper(MyInitUnit)

	// UnitEvent

	UnitEvent::UnitEvent(::THAPI::Unit* pUnit) :
		m_pUnit(pUnit) { }

	// 准备析构单位

	::THAPI::Unit* __stdcall MyUninitUnit(::THAPI::Unit* pUnit, BOOL bFree)
	{
		UnitEvent event_(pUnit);
		if (g_eventBus.Post(THInitEvent::OnUninitUnit, event_))
		{
			__asm
			{
				push bFree
				mov ecx, pUnit
				call g_newUninitUnitEntry
			}
		}
		return pUnit;
	}

	ThiscallToStdcallWrapper(MyUninitUnit)


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
			res = res && HookInlineHook(INIT_STAGE_HOOK_ADDR, MyInitStage, (void**)&g_newInitStageEntry, 7);
			res = res && HookInlineHook(UNINIT_STAGE_HOOK_ADDR, MyUninitStage, (void**)&g_newUninitStageEntry);
			res = res && HookInlineHook(INIT_UNIT_HOOK_ADDR, MyInitUnitWrapper, (void**)&g_newInitUnitEntry);
			res = res && HookInlineHook(UNINIT_UNIT_HOOK_ADDR, MyUninitUnitWrapper, (void**)&g_newUninitUnitEntry, 6);
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
			res = res && UnhookInlineHook(INIT_STAGE_HOOK_ADDR, (void**)&g_newInitStageEntry, 7);
			res = res && UnhookInlineHook(UNINIT_STAGE_HOOK_ADDR, (void**)&g_newUninitStageEntry);
			res = res && UnhookInlineHook(INIT_UNIT_HOOK_ADDR, (void**)&g_newInitUnitEntry);
			res = res && UnhookInlineHook(UNINIT_UNIT_HOOK_ADDR, (void**)&g_newUninitUnitEntry, 6);
			return res;
		}
	}
}
