#pragma once
#include "Hook.h"

namespace tml
{
	extern InlineHook g_onInitHook;
	extern InlineHook g_onUninitHook;
	extern InlineHook g_onReadResourceHook;
	extern InlineHook g_onInsertStruct2ToStruct1Hook;
	extern InlineHook g_onInsertStruct2ToStruct1RenHook;
	extern InlineHook g_onDeleteStruct2Hook;
	extern InlineHook g_onInitStageHook;
	extern InlineHook g_onUninitStageHook;
	extern InlineHook g_onInitUnitHook;
	extern InlineHook g_onUninitUnitHook;
}
