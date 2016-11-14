#pragma once

namespace tml
{
	class DllHijack final
	{
	private:
		HMODULE m_d3d9Module = NULL;

		DllHijack();
		~DllHijack();

	public:
		static DllHijack& GetInstance()
		{
			static DllHijack s_instance;
			return s_instance;
		}

		bool IsReady();
	};
}
