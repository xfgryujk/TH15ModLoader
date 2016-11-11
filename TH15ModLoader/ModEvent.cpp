#include "stdafx.h"
#include "ModEvent.h"


namespace tml
{
	TML_API EventBus g_thEventBus;


	int EventBus::AddListener(int eventID, std::function<void(EventBase*)>&& listener)
	{
		int& listenerID = m_nextListenerID[eventID];
		m_listeners[eventID][listenerID] = listener;
		return listenerID++;
	}

	void EventBus::DeleteListener(int eventID, int listenerID)
	{
		m_listeners[eventID].erase(listenerID);
	}

	bool EventBus::Post(int eventID, EventBase& event_)
	{
		for (const auto& i : m_listeners[eventID])
			i.second(&event_);
		return !event_.IsCanceled();
	}
}
