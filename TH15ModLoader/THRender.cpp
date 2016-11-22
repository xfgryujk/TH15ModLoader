#include "stdafx.h"
#include "THRenderEvent.h"
#include "THRenderHook.h"
#include "THRender.h"
using namespace THAPI;


namespace tml
{
	// 准备调用渲染链中Struct2的mainFunction

	int __stdcall MyOnCallStruct2Ren(Struct2* pStruct2)
	{
		CallStruct2Event event_(*pStruct2);
		if (g_eventBus.Post(THRenderEvent::OnCallStruct2Ren, event_))
			event_.result = -1;
		else if (event_.result == -1)
			event_.result = 1;
		return event_.result;
	}

	__declspec(naked) void MyOnCallStruct2RenWrapper()
	{
		__asm
		{
			push edi
			call MyOnCallStruct2Ren
			cmp eax, -1
			jne Cancel
			// 没取消，交给原函数
			jmp g_onCallStruct2RenHook.m_newEntry

			// 取消了，返回eax，跳过原函数
			Cancel:
			mov ecx, 401692h
			jmp ecx
		}
	}


	// 本模块初始化

	InlineHook g_onCallStruct2RenHook((void*)0x40168A, MyOnCallStruct2RenWrapper, 6);

	bool THRender::IsReady()
	{
		bool res = true;
		res = res && g_onCallStruct2RenHook.IsEnabled();
		return res;
	}
}
