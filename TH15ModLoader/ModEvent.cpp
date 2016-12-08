#include "stdafx.h"
#include <ModEvent.h>


namespace tml
{
	TML_API EventBus g_eventBus;


	int EventBus::AddListener(int eventID, std::function<void(EventBase*)>&& callback, HMODULE module)
	{
		int& listenerID = m_nextListenerID[eventID];
		Listener& listener = m_listeners[eventID][listenerID];
		listener.m_callback = std::move(callback);
		listener.m_module = module;
		return listenerID++;
	}

	void EventBus::DeleteListener(int eventID, int listenerID)
	{
		m_listeners[eventID].erase(listenerID);
	}

	void EventBus::DeleteListenersOfModule(HMODULE module)
	{
		for (auto& i : m_listeners)
		{
			for (auto it = i.second.begin(); it != i.second.end();)
			{
				if (it->second.m_module == module)
					it = i.second.erase(it);
				else
					++it;
			}
		}
	}

	bool EventBus::Post(int eventID, EventBase& event_)
	{
		for (const auto& i : m_listeners[eventID])
			i.second.m_callback(&event_);
		return !event_.IsCanceled();
	}
}
