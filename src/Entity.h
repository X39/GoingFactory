#pragma once
#include <functional>
#include <unordered_map>
#include <string>

#include "chunk.h"
#include "vec2.h"
#include "Component.h"
#include "Event.h"

namespace x39::goingfactory
{
	namespace entity
	{
		class Entity : public Component
		{
		private:
			friend class EntityManager;
			size_t m_local_id;
			void raise_onDestroy() { EventArgs args; onDestroy.raise(*this, args); }
		public:
			Event<Entity> onDestroy;
			static std::unordered_map<std::string, std::function<std::shared_ptr<x39::goingfactory::entity::Entity>()>>& registry();
			Entity() : m_local_id(~0) { }
			size_t local_id() { return m_local_id; }
			virtual bool is_type(EComponent component) const override { return false; }

			virtual std::string type_name() const = 0;
		};
		template <typename T> struct EntityRegister
		{
			EntityRegister(const std::string& key, std::function<std::shared_ptr<Entity>()> func) { Entity::registry()[key] = func; }
		};
	}
}