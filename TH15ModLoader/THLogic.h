#pragma once

namespace tml
{
	class THLogic final
	{
	private:
		THLogic() = default;
		~THLogic() = default;

	public:
		static THLogic& GetInstance()
		{
			static THLogic s_instance;
			return s_instance;
		}

		bool IsReady();
	};
}
