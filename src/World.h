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
	public:
		struct Tile
		{
			bool is_passable;
			bool has_tree;
			size_t tile_texture;
			size_t tile_texture_surrounding;
			size_t texture_index;
		};
		static const size_t tile_size = 16;
	private:
		entity::Entity* m_player;
		size_t m_viewport_x;
		size_t m_viewport_y;
		size_t m_viewport_w;
		size_t m_viewport_h;
		void draw_level(GameInstance&);
	public:
		World();
		~World();
		entity::Entity* player() { return m_player; }
		void set_viewport(size_t x, size_t y, size_t w, size_t h) { m_viewport_x = x; m_viewport_y = y; m_viewport_w = w; m_viewport_h = h; }
		void set_player(entity::Entity* player) { m_player = player; }
		void render(GameInstance&);

		void keydown(io::EKey key);
		bool is_in_view(vec2 pos, int offset = 0);
		Tile get_tile(vec2 vec) { return get_tile((int)vec.x, (int)vec.y); }
		Tile get_tile(int x, int y);
		std::vector<std::array<x39::goingfactory::vec2, 4>> get_chunk_world_collision(int x, int y);
	};
}