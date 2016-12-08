// MOD事件机制实现

#pragma once
#include "APIHeader.h"
#include <map>
#include <functional>


namespace tml
{
	class EventBus;
	extern TML_API EventBus g_eventBus;


	class TML_API EventBase
	{
	private:
		bool m_canceled = false;

	public:
		virtual ~EventBase() = default;

		bool IsCanceled() { return m_canceled; }
		// 有些不能取消的事件会忽略
		void Cancel() { m_canceled = true; }
	};

#pragma warning(push)
#pragma warning(disable: 4251)
	class TML_API EventBus final
	{
	private:
		class Listener
		{
		public:
			std::function<void(EventBase*)> m_callback;
			HMODULE m_module = NULL;
		};

		// eventID -> listenerID
		std::map<int, int> m_nextListenerID;
		// eventID -> listenerID -> listener
		std::map<int, std::map<int, Listener> > m_listeners;

	public:
		// 返回listener ID，如果指定module则在MOD卸载时会自动删除，否则需要手动删除
		int AddListener(int eventID, std::function<void(EventBase*)>&& callback, HMODULE module = NULL);
		void DeleteListener(int eventID, int listenerID);
		void DeleteListenersOfModule(HMODULE module);
		// 如果事件被取消返回false
		bool Post(int eventID, EventBase& event_ = EventBase());
	};
#pragma warning(pop)
}
