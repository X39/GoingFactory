#pragma once
#include "GameInstance.h"

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

		size_t m_level_w;
		size_t m_level_h;
	public:
		World() : m_viewport_x(0), m_viewport_y(0), m_viewport_w(0), m_viewport_h(0), m_level_w(0), m_level_h(0), m_player() { }
		size_t level_width() { return m_level_w; }
		size_t level_height() { return m_level_h; }
		entity::Entity* player() { return m_player; }
		void set_viewport(size_t x, size_t y, size_t w, size_t h) { m_viewport_x = x; m_viewport_y = y; m_viewport_w = w; m_viewport_h = h; }
		void set_level(size_t w, size_t h) { m_level_w = w; m_level_h = h; }
		void set_player(entity::Entity* player) { m_player = player; }
		void render(GameInstance&);
	};
}