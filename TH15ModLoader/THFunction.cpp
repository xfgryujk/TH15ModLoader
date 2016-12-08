#include "stdafx.h"
#include <THFunction.h>
#include "THInitHook.h"
using namespace tml;


namespace THAPI
{
#define DEF_TH_FUNCTION(name, entry) TML_API decltype(name) name = (decltype(name))entry

#define StdcallJmpToThiscall(f) \
	__asm pop eax \
	__asm pop ecx \
	__asm push eax \
	__asm jmp f


	DEF_TH_FUNCTION(malloc, 0x490432);
	DEF_TH_FUNCTION(free, 0x4904C4);

	DEF_TH_FUNCTION(NewModule, 0x4017E0);
	TML_API int WINAPI InsertModuleToLogicLink(Module* pModule, DWORD order)
	{
		return ((int(__stdcall*)(Module* pModule, DWORD order))g_onInsertModuleToLogicLinkHook.m_newEntry)(pModule, order);
	}
	TML_API int WINAPI InsertModuleToRenderLink(Module* pModule, DWORD order)
	{
		return ((int(__stdcall*)(Module* pModule, DWORD order))g_onInsertModuleToRenderLinkHook.m_newEntry)(pModule, order);
	}
	TML_API __declspec(naked) void WINAPI DeleteModule(ModulesLinks* pModulesLinks, Module* pModule)
	{
		StdcallJmpToThiscall(g_onDeleteModuleHook.m_newEntry)
	}

	TML_API __declspec(naked) Unit* WINAPI CreateUnit(Stage* pStage, const char* eclFuncName, InitUnitArg* pArg, DWORD unused)
	{
		StdcallJmpToThiscall(g_onInitUnitHook.m_newEntry)
	}
	TML_API __declspec(naked) Unit* WINAPI DeleteUnit(Unit* pUnit, BOOL bFree)
	{
		StdcallJmpToThiscall(g_onUninitUnitHook.m_newEntry)
	}
}
