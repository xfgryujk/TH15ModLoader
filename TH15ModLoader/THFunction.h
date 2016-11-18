// 东方本体程序用的函数
// 应保证使用这里的函数不会触发相应的事件

#pragma once
#include "APIHeader.h"
#include "THType.h"

namespace THAPI
{
	extern TML_API void*(__cdecl* const malloc)(size_t dwBytes);
	extern TML_API void(__cdecl* const free)(void* lpMem);

	// 用完后要free，用DeleteStruct2会自动free，插入之前记得flag|=2，还有设置param
	extern TML_API Struct2* (__stdcall* const NewStruct2)(int(__fastcall* mainFunction)(void* param));
	extern TML_API int WINAPI InsertStruct2ToStruct1(Struct2* pStruct2, DWORD order);
	extern TML_API int WINAPI InsertStruct2ToStruct1Ren(Struct2* pStruct2, DWORD order);
	// 自动从Struct1或Struct1_Ren断开Struct2并free
	extern TML_API void WINAPI DeleteStruct2(Struct1* pStruct1, Struct2* pStruct2);

	extern TML_API Unit* WINAPI CreateUnit(Stage* pStage, const char* eclFuncName, InitUnitArg* pArg, DWORD unused = 0);
	extern TML_API Unit* WINAPI DeleteUnit(Unit* pUnit, BOOL bFree = TRUE);
}
