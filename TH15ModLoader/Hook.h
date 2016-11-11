#pragma once

namespace Hook
{
	bool HookInlineHook(void* originalFunction, void* hookFunction, void** newFunction, SIZE_T hookLength = 5);
	bool UnhookInlineHook(void* originalFunction, void** newFunction, SIZE_T hookLength = 5);
}
