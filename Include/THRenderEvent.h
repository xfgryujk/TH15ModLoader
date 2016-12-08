// 和渲染有关的事件

#pragma once
#include "THLogicEvent.h"


namespace tml
{
	enum THRenderEvent
	{
		OnCallRenderModule = 200				// 准备调用渲染链中Module的mainFunction，事件类型为CallModuleEvent，可取消
	};
}
