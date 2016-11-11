#pragma once

namespace tml
{
	bool HookInlineHook(void* originalFunction, void* hookFunction, void** newFunction, SIZE_T hookLength = 5);
	bool UnhookInlineHook(void* originalFunction, void** newFunction, SIZE_T hookLength = 5);
}
