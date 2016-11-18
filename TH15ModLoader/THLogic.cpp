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

	// InsEvent

	InsEvent::InsEvent(EclContext& eclContext, Ins& ins) :
		EclContextEvent(eclContext), m_ins(ins) { }

	// 准备执行一条ECL指令

	int __stdcall MyOnExecuteEclIns(EclContext* pEclContext, Ins* pIns)
	{
		InsEvent event_(*pEclContext, *pIns);
		if (g_eventBus.Post(THLogicEvent::OnExecuteEclIns, event_))
			event_.result = 0;
		else if (event_.result == 0) // 取消但不清除EclContext
			event_.result = 1;
		else // 取消，清除EclContext
			event_.result = 2;
		return event_.result;
	}

	__declspec(naked) void MyOnExecuteEclInsWrapper()
	{
		__asm
		{
			push esi
			push edi
			call MyOnExecuteEclIns
			test eax, eax
			jnz Cancel
			// 没取消，交给原函数
			jmp g_onOnExecuteEclInsHook.m_newEntry

			// 取消了
			Cancel:
			cmp eax, 1
			jne Clear
			// 不清除EclContext，跳到下一条ins
			mov eax, 48E43Ah
			jmp eax

			// 清除EclContext
			Clear:
			mov eax, 48CB8Ch
			jmp eax
		}
	}


	// 本模块初始化

	InlineHook g_onCallStruct2Hook((void*)0x40155F, MyOnCallStruct2Wrapper, 6);
	InlineHook g_onOnUpdateUnitHook((void*)0x428830, MyOnUpdateUnit);
	InlineHook g_onOnUpdateEclContextHook((void*)0x48CA80, MyOnUpdateEclContext, 6);
	InlineHook g_onOnExecuteEclInsHook((void*)0x48CAFF, MyOnExecuteEclInsWrapper, 7);

	bool THLogic::IsReady()
	{
		bool res = true;
		res = res && g_onCallStruct2Hook.IsEnabled();
		res = res && g_onOnUpdateUnitHook.IsEnabled();
		res = res && g_onOnUpdateEclContextHook.IsEnabled();
		res = res && g_onOnExecuteEclInsHook.IsEnabled();
		return res;
	}
}
