#include "stdafx.h"
#include "THInitEvent.h"
#include "THInitHook.h"
#include "THInit.h"
#include "THFunction.h"
using namespace THAPI;


namespace tml
{
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


	// ModuleEvent

	ModuleEvent::ModuleEvent(Module& module, DWORD order) :
		m_module(module), m_order(order) { }

	// Module插入逻辑链

	int __stdcall MyInsertModuleToLogicLink(Module* pModule, DWORD order)
	{
		_RPTF2(_CRT_WARN, "插入Module到LogicLink (0x%X, %u)\n", pModule, order);

		int result = 1; // 返回非0代表插入失败
		ModuleEvent event_(*pModule, order);
		if (g_eventBus.Post(THInitEvent::OnInsertModuleToLogicLink, event_))
			result = InsertModuleToLogicLink(pModule, order);
		return result;
	}

	// Module插入渲染链

	int __stdcall MyInsertModuleToRenderLink(Module* pModule, DWORD order)
	{
		_RPTF2(_CRT_WARN, "插入Module到RenderLink (0x%X, %u)\n", pModule, order);

		int result = 1; // 返回非0代表插入失败
		ModuleEvent event_(*pModule, order);
		if (g_eventBus.Post(THInitEvent::OnInsertModuleToRenderLink, event_))
			result = InsertModuleToRenderLink(pModule, order);
		return result;
	}

	// Module准备断开和析构

	void __stdcall MyDeleteModule(ModulesLinks* pModulesLinks, Module* pModule)
	{
		_RPTF2(_CRT_WARN, "删除Module (0x%X, %u)\n", pModule, pModule->order);

		ModuleEvent event_(*pModule, pModule->order);
		if (g_eventBus.Post(THInitEvent::OnDeleteModule, event_))
			DeleteModule(pModulesLinks, pModule);
	}

	ThiscallToStdcallWrapper(MyDeleteModule)


	// StageEvent

	StageEvent::StageEvent(Stage& stage) :
		m_stage(stage) { }

	// 关卡初始化后

	Stage* __fastcall MyInitStage(const char* eclName)
	{
		Stage* pStage = ((Stage*(__fastcall*)(const char* eclName))g_onInitStageHook.m_newEntry)(eclName);
		StageEvent event_(*pStage);
		g_eventBus.Post(THInitEvent::OnInitStage, event_);
		return pStage;
	}

	// 准备析构关卡

	void* __fastcall MyUninitStage(Stage* pStage)
	{
		StageEvent event_(*pStage);
		g_eventBus.Post(THInitEvent::OnUninitStage, event_);
		return ((void*(__fastcall*)(Stage* pStage))g_onUninitStageHook.m_newEntry)(pStage);
	}


	// InitUnitEvent

	InitUnitEvent::InitUnitEvent(LPCSTR eclFuncName, InitUnitArg& arg) :
		m_eclFuncName(eclFuncName), m_arg(arg) { }

	// 准备初始化单位

	Unit* __stdcall MyInitUnit(Stage* pStage, const char* eclFuncName, InitUnitArg* pArg, DWORD unused)
	{
		Unit* result = NULL;
		InitUnitEvent event_(eclFuncName, *pArg);
		if (g_eventBus.Post(THInitEvent::OnInitUnit, event_))
			result = CreateUnit(pStage, eclFuncName, pArg, unused);
		return result;
	}

	ThiscallToStdcallWrapper(MyInitUnit)

	// UnitEvent

	UnitEvent::UnitEvent(Unit& unit) :
		m_unit(unit) { }

	// 准备析构单位

	Unit* __stdcall MyUninitUnit(Unit* pUnit, BOOL bFree)
	{
		UnitEvent event_(*pUnit);
		if (g_eventBus.Post(THInitEvent::OnUninitUnit, event_))
			DeleteUnit(pUnit, bFree);
		return pUnit;
	}

	ThiscallToStdcallWrapper(MyUninitUnit)


	// 本模块初始化

	InlineHook g_onInitHook((void*)0x4719B0, MyOnInitWrapper, 11);
	InlineHook g_onUninitHook((void*)0x471CB8, MyOnUninitWrapper);
	InlineHook g_onReadResourceHook((void*)0x402E0C, MyReadResWrapper, 6);
	InlineHook g_onInsertModuleToLogicLinkHook((void*)0x401390, MyInsertModuleToLogicLink, 9);
	InlineHook g_onInsertModuleToRenderLinkHook((void*)0x401440, MyInsertModuleToRenderLink, 9);
	InlineHook g_onDeleteModuleHook((void*)0x4018A0, MyDeleteModuleWrapper, 6);
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
		res = res && g_onInsertModuleToLogicLinkHook.IsEnabled();
		res = res && g_onInsertModuleToRenderLinkHook.IsEnabled();
		res = res && g_onDeleteModuleHook.IsEnabled();
		res = res && g_onInitStageHook.IsEnabled();
		res = res && g_onUninitStageHook.IsEnabled();
		res = res && g_onInitUnitHook.IsEnabled();
		res = res && g_onUninitUnitHook.IsEnabled();
		return res;
	}
}
