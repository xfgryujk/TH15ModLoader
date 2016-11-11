#pragma once
#include "THModEvent.h"


namespace THInit
{
	enum THInitEvent
	{
		OnInit = 0,				// 程序刚开始，Struct2插入Struct1前
		OnUninit				// 程序准备结束，Struct1析构之前
	};


	bool Init();
	bool Uninit();
}
