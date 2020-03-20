#pragma once

namespace x39::goingfactory
{
	class EntityManager;
	class ResourceManager;
	class GameInstance
	{
	public:
		GameInstance(EntityManager& entity_manager, ResourceManager& resource_manager) : entity_manager(entity_manager), resource_manager(resource_manager) {}
		EntityManager& entity_manager;
		ResourceManager& resource_manager;
	};
}