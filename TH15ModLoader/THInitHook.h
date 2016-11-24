#pragma once
#include "Hook.h"

namespace tml
{
	extern InlineHook g_onInitHook;
	extern InlineHook g_onUninitHook;
	extern InlineHook g_onReadResourceHook;
	extern InlineHook g_onInsertModuleToLogicLinkHook;
	extern InlineHook g_onInsertModuleToRenderLinkHook;
	extern InlineHook g_onDeleteModuleHook;
	extern InlineHook g_onInitStageHook;
	extern InlineHook g_onUninitStageHook;
	extern InlineHook g_onInitUnitHook;
	extern InlineHook g_onUninitUnitHook;
}
