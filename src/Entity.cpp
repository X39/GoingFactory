#include "Entity.h"

std::unordered_map<std::string, std::function<std::shared_ptr<x39::goingfactory::entity::Entity>()>>& x39::goingfactory::entity::Entity::registry()
{
	static std::unordered_map<std::string, std::function<std::shared_ptr<x39::goingfactory::entity::Entity>()>> instance;
	return instance;
}
