#pragma once
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>

namespace x39::goingfactory
{
	template <typename TFunc>
	class Event
	{
	private:
		struct EventStorage
		{
			TFunc func;
			size_t id;
			EventStorage(size_t id, TFunc func) : id(id), func(func) {}
		};


		size_t m_event_id_top;
		std::vector<EventStorage> m_events;
		std::mutex m_mutex;


	public:
		Event() : m_event_id_top(0), m_events(), m_mutex() {}
		Event(const Event&) = delete;
		size_t subscribe(TFunc func)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_events.push_back({ ++m_event_id_top, func });
			return m_event_id_top;
		}
		void unsubscribe(size_t event_id)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = std::find_if(m_events.begin(), m_events.end(), [event_id](const EventStorage& storage) -> bool { return storage.id == event_id; });
			if (m_events.end() != it)
			{
				m_events.erase(it);
			}
		}
		void clear()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_event_id_top = 0;
			m_events.clear();
		}
		template<typename... Ts>
		void raise(Ts... args)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			for (auto& it : m_events)
			{
				it.func(args...);
			}
		}
	};
}