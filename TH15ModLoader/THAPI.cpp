#include "stdafx.h"
#include "THAPI.h"


namespace tml
{
namespace THAPI
{
	TML_API void*(__cdecl* const malloc)(size_t dwBytes) = (void*(__cdecl*)(size_t))0x490432;
	TML_API void(__cdecl* const free)(void* lpMem) = (void(__cdecl*)(void*))0x4904C4;
}
}
