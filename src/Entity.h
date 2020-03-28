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
		class Entity : virtual public Component
		{
		private:
			friend class EntityManager;
			size_t m_local_id;
			void raise_onDestroy() { EventArgs args; onDestroy.raise(*this, args); }
		public:
			Event<Entity> onDestroy;
			Entity() : m_local_id(~0) { }
			size_t local_id() { return m_local_id; }
			virtual std::string type_name() const = 0;
		};
	}
}