#include "EntityManager.h"
#include "Entity.h"

#include <future>
#include <algorithm>

size_t x39::goingfactory::EntityManager::push_back(std::shared_ptr<Entity> ptr)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto res = m_atomic_vector_ptr.load();
	auto free_spot = std::find_if(res->begin(), res->end(), [](const std::shared_ptr<Entity>& ptr) -> bool { return !ptr; });
	if (free_spot == res->end())
	{
		if (res->size() == res->capacity())
		{
			auto duplicate = new std::vector<std::shared_ptr<Entity>>(res->capacity() * 2);
			for (auto it : *res)
			{
				duplicate->push_back(it);
			}
			m_atomic_vector_ptr.exchange(duplicate);
			((void)0); // NOP to give potential threads time to finish their tiny ops
			((void)0); // NOP to give potential threads time to finish their tiny ops
			((void)0); // NOP to give potential threads time to finish their tiny ops
			((void)0); // NOP to give potential threads time to finish their tiny ops
			((void)0); // NOP to give potential threads time to finish their tiny ops
			res->clear();
			delete res;
			res = duplicate;
		}
		size_t index = res->size();
		res->push_back(ptr);
		ptr->m_local_id = index;
		onEntityAdded.raise(ptr);
		return index;
	}
	else
	{
		size_t index = free_spot - res->begin();
		res->emplace(free_spot, ptr);
		ptr->m_local_id = index;
		onEntityAdded.raise(ptr);
		return index;
	}
}
