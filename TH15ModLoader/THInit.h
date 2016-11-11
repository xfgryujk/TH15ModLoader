// 和初始化有关的事件

#pragma once
#include "ModEvent.h"


namespace tml
{
	enum THInitEvent
	{
		OnInit = 0,				// 程序刚开始，Struct2插入Struct1前
		OnUninit				// 程序准备结束，Struct1析构之前
	};


namespace THInit
{
	bool Init();
	bool Uninit();
}
}
