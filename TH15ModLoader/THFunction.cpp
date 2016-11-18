#include "stdafx.h"
#include "THFunction.h"
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

	DEF_TH_FUNCTION(NewStruct2, 0x4017E0);
	TML_API int WINAPI InsertStruct2ToStruct1(Struct2* pStruct2, DWORD order)
	{
		return ((int(__stdcall*)(Struct2* pStruct2, DWORD order))g_onInsertStruct2ToStruct1Hook.m_newEntry)(pStruct2, order);
	}
	TML_API int WINAPI InsertStruct2ToStruct1Ren(Struct2* pStruct2, DWORD order)
	{
		return ((int(__stdcall*)(Struct2* pStruct2, DWORD order))g_onInsertStruct2ToStruct1RenHook.m_newEntry)(pStruct2, order);
	}
	TML_API __declspec(naked) void WINAPI DeleteStruct2(Struct1* pStruct1, Struct2* pStruct2)
	{
		StdcallJmpToThiscall(g_onDeleteStruct2Hook.m_newEntry)
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
