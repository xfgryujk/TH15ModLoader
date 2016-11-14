// 东方本体程序用的函数

#pragma once
#include "APIHeader.h"

namespace THAPI
{
	extern TML_API void*(__cdecl* const malloc)(size_t dwBytes);
	extern TML_API void(__cdecl* const free)(void* lpMem);
}
