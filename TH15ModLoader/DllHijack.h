#pragma once
#include "TMLModule.h"


namespace tml
{
	class DllHijack final : public Singleton<DllHijack>
	{
		DECL_SINGLETON(DllHijack);
	private:
		HMODULE m_d3d9Module = NULL;

		DllHijack();
		~DllHijack();

	public:
		bool IsReady();
	};
}
