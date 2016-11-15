#include "stdafx.h"
#include "THInit.h"
#include "THInit_.h"
#include "Hook.h"
#include "THFunction.h"
using namespace THAPI;


namespace tml
{
	void MyOnInitWrapper();
	void MyOnUninitWrapper();
	void MyReadResWrapper();
	int __stdcall MyInsertStruct2ToStruct1(THAPI::Struct2* pStruct2, DWORD order);
	int __stdcall MyInsertStruct2ToStruct1Ren(THAPI::Struct2* pStruct2, DWORD order);
	void MyDeleteStruct2Wrapper();
	Stage* __fastcall MyInitStage(const char* eclName);
	void* __fastcall MyUninitStage(Stage* pStage);
	void MyInitUnitWrapper();
	void MyUninitUnitWrapper();

	// 本模块初始化

	InlineHook g_onInitHook((void*)0x4719B0, MyOnInitWrapper, 11);
	InlineHook g_onUninitHook((void*)0x471CB8, MyOnUninitWrapper);
	InlineHook g_onReadResourceHook((void*)0x402E0C, MyReadResWrapper, 6);
	InlineHook g_onInsertStruct2ToStruct1Hook((void*)0x401390, MyInsertStruct2ToStruct1, 9);
	InlineHook g_onInsertStruct2ToStruct1RenHook((void*)0x401440, MyInsertStruct2ToStruct1Ren, 9);
	InlineHook g_onDeleteStruct2Hook((void*)0x4018A0, MyDeleteStruct2Wrapper, 6);
	InlineHook g_onInitStageHook((void*)0x426A00, MyInitStage, 7);
	InlineHook g_onUninitStageHook((void*)0x426820, MyUninitStage);
	InlineHook g_onInitUnitHook((void*)0x426050, MyInitUnitWrapper);
	InlineHook g_onUninitUnitHook((void*)0x425330, MyUninitUnitWrapper, 6);

	bool THInit::IsReady()
	{
		bool res = true;
		res = res && g_onInitHook.IsEnabled();
		res = res && g_onUninitHook.IsEnabled();
		res = res && g_onReadResourceHook.IsEnabled();
		res = res && g_onInsertStruct2ToStruct1Hook.IsEnabled();
		res = res && g_onInsertStruct2ToStruct1RenHook.IsEnabled();
		res = res && g_onDeleteStruct2Hook.IsEnabled();
		res = res && g_onInitStageHook.IsEnabled();
		res = res && g_onUninitStageHook.IsEnabled();
		res = res && g_onInitUnitHook.IsEnabled();
		res = res && g_onUninitUnitHook.IsEnabled();
		return res;
	}


	// 程序初始化
	
	void __stdcall MyOnInit()
	{
		_RPTF0(_CRT_WARN, "程序初始化\n");

		g_eventBus.Post(THInitEvent::OnInit);
	}

	__declspec(naked) void MyOnInitWrapper()
	{
		__asm
		{
			call MyOnInit
			jmp g_onInitHook.m_newEntry
		}
	}

	// 程序结束

	void __stdcall MyOnUninit()
	{
		_RPTF0(_CRT_WARN, "程序结束\n");

		g_eventBus.Post(THInitEvent::OnUninit);
	}

	__declspec(naked) void MyOnUninitWrapper()
	{
		__asm
		{
			call MyOnUninit
			jmp g_onUninitHook.m_newEntry
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
			jmp g_onReadResourceHook.m_newEntry

			// 不是NULL，替换资源
			NotNull:
			mov edi, eax
			mov eax, 402E79h
			jmp eax
		}
	}


	// Struct2Event

	Struct2Event::Struct2Event(Struct2* pStruct2, DWORD order) :
		m_pStruct2(pStruct2), m_order(order) { }

	// Struct2插入Struct1

	int __stdcall MyInsertStruct2ToStruct1(Struct2* pStruct2, DWORD order)
	{
		_RPTF2(_CRT_WARN, "插入Struct2到Struct1 (0x%X, %u)\n", pStruct2, order);

		int res = 1; // 返回非0代表插入失败
		Struct2Event event_(pStruct2, order);
		if (g_eventBus.Post(THInitEvent::OnInsertStruct2ToStruct1, event_))
			res = ((int(__stdcall*)(Struct2* pStruct2, DWORD order))g_onInsertStruct2ToStruct1Hook.m_newEntry)(pStruct2, order);
		return res;
	}

	// Struct2插入Struct1_Ren

	int __stdcall MyInsertStruct2ToStruct1Ren(Struct2* pStruct2, DWORD order)
	{
		_RPTF2(_CRT_WARN, "插入Struct2到Struct1Ren (0x%X, %u)\n", pStruct2, order);

		int res = 1; // 返回非0代表插入失败
		Struct2Event event_(pStruct2, order);
		if (g_eventBus.Post(THInitEvent::OnInsertStruct2ToStruct1Ren, event_))
			res = ((int(__stdcall*)(Struct2* pStruct2, DWORD order))g_onInsertStruct2ToStruct1RenHook.m_newEntry)(pStruct2, order);
		return res;
	}

	// Struct2准备断开和析构

	void __stdcall MyDeleteStruct2(Struct2* pStruct1, Struct2* pStruct2)
	{
		_RPTF2(_CRT_WARN, "删除Struct2 (0x%X, %u)\n", pStruct2, pStruct2->order);

		Struct2Event event_(pStruct2, pStruct2->order);
		if (g_eventBus.Post(THInitEvent::OnDeleteStruct2, event_))
		{
			__asm
			{
				push pStruct2
				mov ecx, pStruct1
				call g_onDeleteStruct2Hook.m_newEntry
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

	StageEvent::StageEvent(Stage* pStage) :
		m_pStage(pStage) { }

	// 关卡初始化后

	Stage* __fastcall MyInitStage(const char* eclName)
	{
		Stage* pStage = ((Stage*(__fastcall*)(const char* eclName))g_onInitStageHook.m_newEntry)(eclName);
		StageEvent event_(pStage);
		g_eventBus.Post(THInitEvent::OnInitStage, event_);
		return pStage;
	}

	// 准备析构关卡

	void* __fastcall MyUninitStage(Stage* pStage)
	{
		StageEvent event_(pStage);
		g_eventBus.Post(THInitEvent::OnUninitStage, event_);
		return ((void*(__fastcall*)(Stage* pStage))g_onUninitStageHook.m_newEntry)(pStage);
	}


	// InitUnitEvent

	InitUnitEvent::InitUnitEvent(LPCSTR eclFuncName, void* pArg) :
		m_eclFuncName(eclFuncName), m_pArg(pArg) { }

	// 准备初始化单位

	Unit* __stdcall MyInitUnit(Stage* pStage, const char* eclFuncName, void* pArg, DWORD unknown)
	{
		Unit* res = NULL;
		InitUnitEvent event_(eclFuncName, pArg);
		if (g_eventBus.Post(THInitEvent::OnInitUnit, event_))
		{
			__asm
			{
				push unknown
				push pArg
				push eclFuncName
				mov ecx, pStage
				call g_onInitUnitHook.m_newEntry
				mov res, eax
			}
		}
		return res;
	}

	ThiscallToStdcallWrapper(MyInitUnit)

	// UnitEvent

	UnitEvent::UnitEvent(Unit* pUnit) :
		m_pUnit(pUnit) { }

	// 准备析构单位

	Unit* __stdcall MyUninitUnit(Unit* pUnit, BOOL bFree)
	{
		UnitEvent event_(pUnit);
		if (g_eventBus.Post(THInitEvent::OnUninitUnit, event_))
		{
			__asm
			{
				push bFree
				mov ecx, pUnit
				call g_onUninitUnitHook.m_newEntry
			}
		}
		return pUnit;
	}

	ThiscallToStdcallWrapper(MyUninitUnit)
}
