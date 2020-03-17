#include "Entity.h"

std::unordered_map<std::string, std::function<std::shared_ptr<x39::goingfactory::Entity>()>>& x39::goingfactory::Entity::registry()
{
	static std::unordered_map<std::string, std::function<std::shared_ptr<x39::goingfactory::Entity>()>> instance;
	return instance;
}
