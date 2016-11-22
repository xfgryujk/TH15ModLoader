#pragma once
#include "TMLModule.h"


namespace tml
{
	class THInit final : public Singleton<THInit>
	{
		DECL_SINGLETON(THInit);
	public:
		bool IsReady();
	};
}
