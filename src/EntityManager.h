#pragma once
#include <vector>
#include <array>
#include <mutex>
#include <unordered_map>
#include <type_traits>

#include "Event.h"
#include "vec2.h"
#include "chunk.h"
#include "Component.h"

namespace x39::goingfactory
{
	namespace entity
	{
		class Entity;
	}
	class chunk;
	class EntityManager
	{
	private:
		std::vector<entity::Entity*> m_all_entities;
		std::unordered_map<EComponent, std::vector<entity::Entity*>> m_specialized_entities;
		std::unordered_map<int64_t, chunk*> m_chunks;

		std::mutex m_pool_create;
		std::vector<entity::Entity*> m_create_pool;
		std::mutex m_pool_destroy;
		std::vector<entity::Entity*> m_destroy_pool;
		std::mutex m_pool_chunk_update;
		std::vector<PositionComponent*> m_chunk_update_pool;
		void do_act_pools();
	public:
		struct EntityAddedEventArgs : public EventArgs { entity::Entity* entity; EntityAddedEventArgs(entity::Entity* entity) : entity(entity) {} };
		Event<EntityManager, EntityAddedEventArgs> onEntityAdded;
		struct EntityRemovedEventArgs : public EventArgs { entity::Entity* entity; EntityRemovedEventArgs(entity::Entity* entity) : entity(entity) {} };
		Event<EntityManager, EntityRemovedEventArgs> onEntityRemoved;


		EntityManager() :
			m_all_entities(),
			m_specialized_entities(),
			m_pool_create(),
			m_create_pool(),
			m_pool_destroy(),
			m_destroy_pool(),
			m_chunks()
		{ }
		EntityManager(const EntityManager&) = delete;
		~EntityManager();
		void pool_create(entity::Entity* ptr)
		{
			std::lock_guard lock(m_pool_create);
			m_create_pool.push_back(ptr);
		}
		void pool_destroy(entity::Entity* ptr)
		{
			std::lock_guard lock(m_pool_destroy);
			m_destroy_pool.push_back(ptr);
		}
		
		void pool_chunk_update(PositionComponent* ptr)
		{
			std::lock_guard lock(m_pool_chunk_update);
			m_chunk_update_pool.push_back(ptr);
		}


		entity::Entity* operator[](size_t index) const
		{
			return m_all_entities.at(index);
		}


		chunk* chunk_at(vec2 pos)
		{
			auto coordinate = chunk::to_chunk_coordinate(pos);
			if (m_chunks.contains(coordinate))
			{
				return m_chunks[coordinate];
			}
			return nullptr;
		}
		chunk* chunk_at(int32_t x, int32_t y)
		{
			auto coordinate = chunk::to_chunk_coordinate(x, y);
			if (m_chunks.contains(coordinate))
			{
				return m_chunks[coordinate];
			}
			return nullptr;
		}
		/*
			Iterator start for ALL entities.
			Use specialized variant to filter more specific.
		*/
		std::vector<entity::Entity*>::const_iterator begin() const { return m_all_entities.begin(); }
		/*
			Iterator start for entities implementing the provided component.
		*/
		std::vector<entity::Entity*>::const_iterator begin(EComponent component) const
		{
			if (!m_specialized_entities.contains(component))
			{
				return {};
			}
			return m_specialized_entities.at(component).begin();
		}
		/*
			Iterator start for entities at the provided world position.
			World position will be transformed into chunk position and the entities in that given chunk are returned.
		*/
		std::vector<entity::Entity*>::const_iterator begin(vec2 vec) const {
			auto chunk_coord = chunk::to_chunk_coordinate(vec);
			if (!m_chunks.contains(chunk_coord))
			{
				return {};
			}
			return m_chunks.at(chunk_coord)->m_entities.begin();
		}
		/*
			Iterator start for entities inside the provided chunk, determined via the chunk coordinates provided.
		*/
		std::vector<entity::Entity*>::const_iterator begin(int32_t x, int32_t y) const {
			auto chunk_coord = chunk::to_chunk_coordinate(x, y);
			if (!m_chunks.contains(chunk_coord))
			{
				return {};
			}
			return m_chunks.at(chunk_coord)->m_entities.begin();
		}


		/*
			Iterator end for ALL entities.
			Use specialized variant to filter more specific.
		*/
		std::vector<entity::Entity*>::const_iterator end() const { return m_all_entities.end(); }
		/*
			Iterator end for entities implementing the provided component.
		*/
		std::vector<entity::Entity*>::const_iterator end(EComponent component) const 
		{
			if (!m_specialized_entities.contains(component))
			{
				return {};
			}
			return m_specialized_entities.at(component).end();
		}
		/*
			Iterator end for entities at the provided world position.
			World position will be transformed into chunk position and the entities in that given chunk are returned.
		*/
		std::vector<entity::Entity*>::const_iterator end(vec2 vec) const {
			auto chunk_coord = chunk::to_chunk_coordinate(vec);
			if (!m_chunks.contains(chunk_coord))
			{
				return {};
			}
			return m_chunks.at(chunk_coord)->m_entities.end();
		}
		/*
			Iterator end for entities inside the provided chunk, determined via the chunk coordinates provided.
		*/
		std::vector<entity::Entity*>::const_iterator end(int32_t x, int32_t y) const {
			auto chunk_coord = chunk::to_chunk_coordinate(x, y);
			if (!m_chunks.contains(chunk_coord))
			{
				return {};
			}
			return m_chunks.at(chunk_coord)->m_entities.end();
		}

		/*
			Performs the pooled create & delete operations and clears the pools afterwards.
			It is required that during this step, nothing happens!
		*/
		void act_pools() {
			if (m_create_pool.empty() && m_chunk_update_pool.empty() && m_destroy_pool.empty())
			{
				return;
			}
			do_act_pools();
		}

		size_t size() const { return m_all_entities.size(); }
		size_t capacity() const { return m_all_entities.capacity(); }
	};
}