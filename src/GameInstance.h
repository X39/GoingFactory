#pragma once
#include "ThreadPool.h"

namespace x39::goingfactory
{
	class EntityManager;
	class ResourceManager;
	class World;
	class GameInstance
	{
	private:
		ThreadPool* m_thread_pool;
	public:
		GameInstance(EntityManager& entity_manager, ResourceManager& resource_manager, World& world) :
			entity_manager(entity_manager),
			resource_manager(resource_manager),
			world(world),
			m_thread_pool(new ThreadPool(12))
		{
		}
		GameInstance(const GameInstance& copy) = delete;
		~GameInstance()
		{
			delete m_thread_pool;
		}
		EntityManager& entity_manager;
		ResourceManager& resource_manager;
		World& world;
		ThreadPool& thread_pool() { return *m_thread_pool; }
	};
}