#include "EntityManager.h"
#include "Entity.h"

#include <future>
#include <algorithm>

size_t x39::goingfactory::EntityManager::push_back(std::shared_ptr<x39::goingfactory::entity::Entity> ptr)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto res = m_atomic_vector_ptr.load();
	auto free_spot = std::find_if(res->begin(), res->end(), [](const std::shared_ptr<entity::Entity>& ptr) -> bool { return !ptr; });
	if (free_spot == res->end())
	{
		if (res->size() == res->capacity())
		{
			auto duplicate = new std::vector<std::shared_ptr<entity::Entity>>();
			duplicate->reserve(res->capacity() == 0 ? 1 : res->capacity() * 2);
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
		EntityAddedEventArgs args(ptr);
		onEntityAdded.raise(*this, args);
		return index;
	}
	else
	{
		size_t index = free_spot - res->begin();
		*free_spot = ptr;
		ptr->m_local_id = index;
		EntityAddedEventArgs args(ptr);
		onEntityAdded.raise(*this, args);
		return index;
	}
}

void x39::goingfactory::EntityManager::destroy(entity::Entity* ptr)
{
	for (auto it : *this)
	{
		if (it.get() == ptr)
		{
			destroy(it);
			break;
		}
	}
}

void x39::goingfactory::EntityManager::destroy(std::shared_ptr<entity::Entity> ptr_in)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto res = m_atomic_vector_ptr.load();
	auto find_res = std::find_if(res->begin(), res->end(), [&ptr_in](const std::shared_ptr<entity::Entity>& ptr) -> bool { return ptr_in == ptr; });
	if (find_res != res->end())
	{
		find_res->reset();
	}
}
