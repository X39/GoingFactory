#pragma once
#include "ThreadPool.h"

namespace x39::goingfactory
{
	class EntityManager;
	class ResourceManager;
	class World;
	namespace ux
	{
		class UXHandler;
	}
	class GameInstance
	{
	private:
		ThreadPool* m_thread_pool;
	public:
		GameInstance(EntityManager& entity_manager, ResourceManager& resource_manager, World& world, ux::UXHandler& uxhandler) :
			entity_manager(entity_manager),
			resource_manager(resource_manager),
			world(world),
			uxhandler(uxhandler),
			m_thread_pool(new ThreadPool(12))
		{
		}
		GameInstance(const GameInstance& copy) = delete;
		~GameInstance()
		{
			delete m_thread_pool;
		}
		ux::UXHandler& uxhandler;
		EntityManager& entity_manager;
		ResourceManager& resource_manager;
		World& world;
		ThreadPool& thread_pool() { return *m_thread_pool; }
		static void Log(std::string);
	};
}