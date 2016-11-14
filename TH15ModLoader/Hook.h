#pragma once

namespace tml
{
	class InlineHook final
	{
	private:
		void* const m_originalFunction;
		void* const m_hookFunction;
		const SIZE_T m_hookLength;
		bool m_isEnabled = false;
	public:
		void* const m_newEntry = NULL;

		InlineHook(void* originalFunction, void* hookFunction, SIZE_T hookLength = 5, bool enable = true);
		~InlineHook();

		bool Enable();
		bool Disable();

		bool IsEnabled();
	};
}
