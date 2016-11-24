// 东方本体程序用的函数
// 应保证使用这里的函数不会触发相应的事件

#pragma once
#include "APIHeader.h"
#include "THType.h"

namespace THAPI
{
	extern TML_API void*(__cdecl* const malloc)(size_t dwBytes);
	extern TML_API void(__cdecl* const free)(void* lpMem);

	// 用完后要free，用DeleteModule会自动free，插入之前记得flag|=2，还有设置param
	extern TML_API Module* (__stdcall* const NewModule)(int(__fastcall* mainFunction)(void* param));
	extern TML_API int WINAPI InsertModuleToLogicLink(Module* pModule, DWORD order);
	extern TML_API int WINAPI InsertModuleToRenderLink(Module* pModule, DWORD order);
	// 自动从ModulesLinks断开Module并free
	extern TML_API void WINAPI DeleteModule(ModulesLinks* pModulesLinks, Module* pModule);

	extern TML_API Unit* WINAPI CreateUnit(Stage* pStage, const char* eclFuncName, InitUnitArg* pArg, DWORD unused = 0);
	extern TML_API Unit* WINAPI DeleteUnit(Unit* pUnit, BOOL bFree = TRUE);
}
