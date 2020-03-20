#pragma once
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "Event.h"

namespace x39::goingfactory
{
	namespace entity
	{
		class Entity;
	}
	class EntityManager
	{
	public:
		class iterator
		{
		public:
			typedef iterator self_type;
			typedef std::shared_ptr<entity::Entity> value_type;
			typedef std::shared_ptr<entity::Entity>* pointer;
			typedef std::forward_iterator_tag iterator_category;
			typedef int difference_type;
			self_type operator++() { self_type i = *this; m_index++; return i; }
			self_type operator++(int junk) { m_index++; return *this; }
			self_type operator--() { self_type i = *this; m_index--; return i; }
			self_type operator--(int junk) { m_index--; return *this; }
			value_type operator*() { return m_ref[m_index]; }
			pointer operator->() { return &m_ref[m_index]; }
			bool operator==(const self_type& rhs) { return &m_ref == &rhs.m_ref && m_index == rhs.m_index; }
			bool operator!=(const self_type& rhs) { return &m_ref != &rhs.m_ref || m_index != rhs.m_index; }
			iterator(EntityManager& ref) : m_ref(ref), m_index(0) { }
			iterator(EntityManager& ref, size_t index) : m_ref(ref), m_index(index) { }
			iterator(const iterator& iterator) : m_ref(iterator.m_ref), m_index(iterator.m_index) { }
		private:
			EntityManager& m_ref;
			size_t m_index;
		};
	private:
		std::atomic<std::vector<std::shared_ptr<entity::Entity>>*> m_atomic_vector_ptr;
		std::mutex m_mutex;
	public:
		struct EntityAddedEventArgs : public EventArgs { std::shared_ptr<entity::Entity> entity; EntityAddedEventArgs(std::shared_ptr<entity::Entity> entity) : entity(entity) {} };
		Event<EntityManager, EntityAddedEventArgs> onEntityAdded;


		EntityManager() : m_atomic_vector_ptr(new std::vector<std::shared_ptr<entity::Entity>>) { }
		EntityManager(const EntityManager&) = delete;
		~EntityManager()
		{
			auto res = m_atomic_vector_ptr.load();
			m_atomic_vector_ptr.exchange(nullptr);
			res->clear();
			delete res;
		}
		std::shared_ptr<entity::Entity> operator[](size_t index) const
		{
			auto res = m_atomic_vector_ptr.load();
			return res->at(index);
		}
		size_t push_back(std::shared_ptr<entity::Entity> ptr);
		void destroy(entity::Entity* ptr);
		void destroy(std::shared_ptr<entity::Entity> ptr);
		iterator begin() { return { *this }; }
		iterator end() { auto res = m_atomic_vector_ptr.load(); return { *this, res->size() }; }
		size_t size() const { auto res = m_atomic_vector_ptr.load(); return res->size(); }
	};
}