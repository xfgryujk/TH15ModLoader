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


	// UnitResultEvent

	UnitResultEvent::UnitResultEvent(Unit& unit) : 
		UnitEvent(unit) { }

	// 准备执行单位逻辑

	int __fastcall MyOnUpdateUnit(Unit* pUnit)
	{
		UnitResultEvent event_(*pUnit);
		if (g_eventBus.Post(THLogicEvent::OnUpdateUnit, event_))
			return ((int(__fastcall*)(Unit* pUnit))g_onOnUpdateUnitHook.m_newEntry)(pUnit);
		return event_.result;
	}

	// EclContextEvent

	EclContextEvent::EclContextEvent(EclContext& eclContext) : 
		UnitResultEvent(*eclContext.pUnit), m_eclContext(eclContext) { }

	// 准备执行ECL逻辑

	int __fastcall MyOnUpdateEclContext(EclContext* pEclContext)
	{
		float deltaTime;
		__asm movss deltaTime, xmm1
		EclContextEvent event_(*pEclContext);
		if (g_eventBus.Post(THLogicEvent::OnUpdateEclContext, event_))
		{
			__asm movss xmm1, deltaTime
			return ((int(__fastcall*)(EclContext* pEclContext))g_onOnUpdateEclContextHook.m_newEntry)(pEclContext);
		}
		return event_.result;
	}


	// 本模块初始化

	InlineHook g_onCallStruct2Hook((void*)0x40155F, MyOnCallStruct2Wrapper, 6);
	InlineHook g_onOnUpdateUnitHook((void*)0x428830, MyOnUpdateUnit);
	InlineHook g_onOnUpdateEclContextHook((void*)0x48CA80, MyOnUpdateEclContext, 6);

	bool THLogic::IsReady()
	{
		bool res = true;
		res = res && g_onCallStruct2Hook.IsEnabled();
		res = res && g_onOnUpdateUnitHook.IsEnabled();
		res = res && g_onOnUpdateEclContextHook.IsEnabled();
		return res;
	}
}
