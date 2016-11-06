#pragma once

namespace Hook
{
	void HookInlineHook(void* originalFunction, void* hookFunction, void** newFunction, SIZE_T hookLength = 5);
	void UnhookInlineHook(void* originalFunction, void** newFunction, SIZE_T hookLength = 5);
}
