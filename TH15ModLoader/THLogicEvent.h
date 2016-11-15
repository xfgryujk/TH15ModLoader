// 和逻辑有关的事件

#pragma once
#include "THInitEvent.h"


namespace tml
{
	enum THLogicEvent
	{
		OnCallStruct2 = 100					// 准备调用逻辑链中Struct2的mainFunction，事件类型为CallStruct2Event，可取消
	};

	class TML_API CallStruct2Event final : public Struct2Event
	{
	public:
		// 如果事件取消了则返回这个值
		// 0清除这个Struct2，1遍历下一个Struct2，2再次调用这个Struct2，3458未知，6重新遍历所有Struct2，7结束遍历Struct2
		int result = 1;

		CallStruct2Event(THAPI::Struct2& struct2);
	};
}
