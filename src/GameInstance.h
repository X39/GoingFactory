#pragma once

namespace x39::goingfactory
{
	class EntityManager;
	class ResourceManager;
	class World;
	class GameInstance
	{
	public:
		GameInstance(EntityManager& entity_manager, ResourceManager& resource_manager, World& world) :
			entity_manager(entity_manager),
			resource_manager(resource_manager),
			world(world) { }
		EntityManager& entity_manager;
		ResourceManager& resource_manager;
		World& world;
	};
}