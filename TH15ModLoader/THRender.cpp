#include "stdafx.h"
#include <THRenderEvent.h>
#include "THRenderHook.h"
#include "THRender.h"
using namespace THAPI;


namespace tml
{
	// 准备调用渲染链中Module的mainFunction

	int __stdcall MyOnCallRenderModule(Module* pModule)
	{
		CallModuleEvent event_(*pModule);
		if (g_eventBus.Post(THRenderEvent::OnCallRenderModule, event_))
			event_.result = -1;
		else if (event_.result == -1)
			event_.result = 1;
		return event_.result;
	}

	__declspec(naked) void MyOnCallRenderModuleWrapper()
	{
		__asm
		{
			push edi
			call MyOnCallRenderModule
			cmp eax, -1
			jne Cancel
			// 没取消，交给原函数
			jmp g_onCallRenderModuleHook.m_newEntry

			// 取消了，返回eax，跳过原函数
			Cancel:
			mov ecx, 401692h
			jmp ecx
		}
	}


	// 本模块初始化

	InlineHook g_onCallRenderModuleHook((void*)0x40168A, MyOnCallRenderModuleWrapper, 6);

	bool THRender::IsReady()
	{
		bool res = true;
		res = res && g_onCallRenderModuleHook.IsEnabled();
		return res;
	}
}
