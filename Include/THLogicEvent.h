﻿// 和逻辑有关的事件

#pragma once
#include "THInitEvent.h"


namespace tml
{
	enum THLogicEvent
	{
		OnCallLogicModule = 100,			// 准备调用逻辑链中Module的mainFunction，事件类型为CallModuleEvent，可取消
		OnUpdateUnit,						// 准备执行单位逻辑，事件类型为UnitEvent，可取消
		OnUpdateEclContext,					// 准备执行ECL逻辑，事件类型为EclContextEvent，可取消
		OnExecuteEclIns						// 准备执行一条ECL指令，事件类型为InsEvent，可取消
	};

	class TML_API CallModuleEvent final : public ModuleEvent
	{
	public:
		// 如果事件取消了则返回这个值
		// 0清除这个Module，1遍历下一个Module，2再次调用这个Module，3458未知，6重新遍历所有Module，7结束遍历Module
		int result = 1;

		CallModuleEvent(THAPI::Module& module);
	};

	class TML_API UnitResultEvent : public UnitEvent
	{
	public:
		// 如果事件取消了则返回这个值
		// 非0则清除这个单位，用于单位死亡
		int result = 0;

		UnitResultEvent(THAPI::Unit& unit);
		virtual ~UnitResultEvent() = default;
	};

	class TML_API EclContextEvent : public UnitResultEvent
	{
	public:
		// 如果事件取消了则返回result
		// 非0则清除这个EclContext，如果是第一个EclContext则会清除单位，用于ECL函数执行完时

		THAPI::EclContext& m_eclContext;

		EclContextEvent(THAPI::EclContext& eclContext);
		virtual ~EclContextEvent() = default;
	};

	class TML_API InsEvent final : public EclContextEvent
	{
	public:
		// 如果事件取消了则返回result
		// 非0则清除这个EclContext，如果是第一个EclContext则会清除单位，用于ECL函数执行完时

		THAPI::Ins& m_ins;

		InsEvent(THAPI::EclContext& eclContext, THAPI::Ins& ins);
	};
}
