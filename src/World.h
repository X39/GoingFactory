#pragma once
#include "GameInstance.h"
#include "EKey.h"
#include "vec2.h"

namespace x39::goingfactory
{
	namespace entity
	{
		class Entity;
	}
	class World
	{
	private:
		entity::Entity* m_player;
		size_t m_viewport_x;
		size_t m_viewport_y;
		size_t m_viewport_w;
		size_t m_viewport_h;
	public:
		World();
		entity::Entity* player() { return m_player; }
		void set_viewport(size_t x, size_t y, size_t w, size_t h) { m_viewport_x = x; m_viewport_y = y; m_viewport_w = w; m_viewport_h = h; }
		void set_player(entity::Entity* player) { m_player = player; }
		void render(GameInstance&);

		void keydown(io::EKey key);
		bool is_in_view(vec2 pos, int offset = 0);
	};
}