#pragma once
#include <functional>
#include <unordered_map>
#include <string>

#include "EntityChunk.h"
#include "../vec2.h"
#include "Component.h"
#include "../Event.h"

namespace x39::goingfactory::entity
{
	class Entity : virtual public Component
	{
	private:
		friend class EntityManager;
		size_t m_local_id;
		void raise_onDestroy() { x39::goingfactory::util::EventArgs args; onDestroy.raise(*this, args); }
	public:
		x39::goingfactory::util::Event<Entity> onDestroy;
		Entity() : m_local_id(~0) { }
		size_t local_id() { return m_local_id; }
		virtual std::string type_name() const = 0;
	};
}