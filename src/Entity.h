#pragma once
#include <functional>
#include <unordered_map>
#include <string>

#include "vec2.h"
#include "Component.h"

namespace x39::goingfactory
{
	class EntityManager;
	class ResourceManager;
	namespace entity
	{
		class Entity : public Component
		{
		private:
			friend class EntityManager;
			size_t m_local_id;
		protected:
			position m_pos;
		public:
			static std::unordered_map<std::string, std::function<std::shared_ptr<x39::goingfactory::entity::Entity>()>>& registry();
			Entity() : m_local_id(~0), m_pos() {}
			size_t local_id() { return m_local_id; }
			position pos() const { return m_pos; }
			void pos(position pos) { m_pos = pos; }

		};
		template <typename T> struct EntityRegister
		{
			EntityRegister(const std::string& key, std::function<std::shared_ptr<Entity>()> func) { Entity::registry()[key] = func; }
		};
	}
}