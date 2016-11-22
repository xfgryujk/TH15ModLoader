#pragma once
#include "TMLModule.h"


namespace tml
{
	class THRender final : public Singleton<THRender>
	{
		DECL_SINGLETON(THRender);
	public:
		bool IsReady();
	};
}
