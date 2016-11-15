#include "stdafx.h"
#include "THLogicEvent.h"
#include "THLogicHook.h"
#include "THLogic.h"
using namespace THAPI;


namespace tml
{
	// CallStruct2Event

	CallStruct2Event::CallStruct2Event(Struct2& struct2) :
		Struct2Event(struct2, struct2.order) { }

	// 准备调用逻辑链中Struct2的mainFunction

	int __stdcall MyOnCallStruct2(Struct2* pStruct2)
	{
		CallStruct2Event event_(*pStruct2);
		if (g_eventBus.Post(THLogicEvent::OnCallStruct2, event_))
			event_.result = -1;
		else if (event_.result == -1)
			event_.result = 1;
		return event_.result;
	}

	__declspec(naked) void MyOnCallStruct2Wrapper()
	{
		__asm
		{
			push edi
			call MyOnCallStruct2
			cmp eax, -1
			jne Cancel
			// 没取消，交给原函数
			jmp g_onCallStruct2Hook.m_newEntry

			// 取消了，返回eax，跳过原函数
			Cancel:
			mov ecx, 401567h
			jmp ecx
		}
	}


	// 本模块初始化

	InlineHook g_onCallStruct2Hook((void*)0x40155F, MyOnCallStruct2Wrapper, 6);

	bool THLogic::IsReady()
	{
		bool res = true;
		res = res && g_onCallStruct2Hook.IsEnabled();
		return res;
	}
}
