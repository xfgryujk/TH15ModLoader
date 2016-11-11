// MOD事件机制实现

#pragma once
#include "APIHeader.h"
#include <map>
#include <functional>


namespace tml
{
	class EventBase;
	class EventBus;
	template class TML_API std::map<int, int>;
	template class TML_API std::map<int, std::map<int, std::function<void(EventBase*)> > >;

	extern TML_API EventBus g_thEventBus;


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

	class TML_API EventBus final
	{
	private:
		// eventID -> listenerID
		std::map<int, int> m_nextListenerID;
		// eventID -> listenerID -> listener
		std::map<int, std::map<int, std::function<void(EventBase*)> > > m_listeners;

	public:
		// 返回listener ID
		int AddListener(int eventID, std::function<void(EventBase*)>&& listener);
		void DeleteListener(int eventID, int listenerID);
		// 如果事件被取消返回false
		bool Post(int eventID, EventBase& event_ = EventBase());
	};
}
