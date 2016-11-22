// 和渲染有关的事件

#pragma once
#include "THLogicEvent.h"


namespace tml
{
	enum THRenderEvent
	{
		OnCallStruct2Ren = 200				// 准备调用渲染链中Struct2的mainFunction，事件类型为CallStruct2Event，可取消
	};
}
