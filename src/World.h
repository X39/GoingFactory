#pragma once
#include <memory>

namespace x39::goingfactory
{
	namespace entity
	{
		class Entity;
	}
	namespace world
	{
		class World
		{
		private:
			std::shared_ptr<entity::Entity> m_player_character;
		public:
			void render();
		};
	}
}