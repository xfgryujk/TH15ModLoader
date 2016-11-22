#pragma once
#include "TMLModule.h"


namespace tml
{
	class THLogic final : public Singleton<THLogic>
	{
		DECL_SINGLETON(THLogic);
	public:
		bool IsReady();
	};
}
