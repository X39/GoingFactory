#pragma once
#include <functional>
#include <unordered_map>
#include <string>

#include "Position.h"

namespace x39::goingfactory
{
	class EntityManager;
	class ResourceManager;
	class Entity
	{
	private:
		friend class EntityManager;
		size_t m_local_id;
	protected:
		position m_pos;
	public:
		static std::unordered_map<std::string, std::function<std::shared_ptr<x39::goingfactory::Entity>()>>& registry();
		Entity() : m_local_id(~0), m_pos() {}
		size_t local_id() { return m_local_id; }
		virtual void initialize(ResourceManager&) {};
		virtual void render(ResourceManager&) = 0;
		virtual void simulate(EntityManager&) = 0;
		position pos() const { return m_pos; }
		void pos(position pos) { m_pos = pos; }
	};
	template <typename T> struct EntityRegister
	{
		EntityRegister(const std::string& key, std::function<std::shared_ptr<Entity>()> func) { Entity::registry()[key] = func; }
	};
}