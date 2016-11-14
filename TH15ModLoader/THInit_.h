#pragma once

namespace tml
{
	class THInit final
	{
	private:
		THInit() = default;
		~THInit() = default;

	public:
		static THInit& GetInstance()
		{
			static THInit s_instance;
			return s_instance;
		}

		bool IsReady();
	};
}
