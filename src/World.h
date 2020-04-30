#pragma once
#include "GameInstance.h"
#include "EKey.h"
#include "vec2.h"
#include <string>
#include <chrono>


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
			size_t index;
		};
		static const size_t tile_size = 16;
	private:
		entity::Entity* m_player;
		size_t m_viewport_x;
		size_t m_viewport_y;
		size_t m_viewport_w;
		size_t m_viewport_h;
		void draw_level(GameInstance&, vec2 top_left_viewport);
		void draw_level_simple(GameInstance&, vec2 top_left_viewport);
		void draw_collision_boxes(GameInstance&, vec2 top_left_viewport);
		void draw_chunks(GameInstance&, vec2 top_left_viewport);
		void draw_entities(GameInstance&, vec2 top_left_viewport);
		void draw_graph(GameInstance&, vec2 top_left_viewport);

		// If false, Skips rendering the level
		bool m_rf_level;
		// If false, Skips rendering surroundings of tiles where it is available
		bool m_rf_level_surroundings;
		// If true, Skips rendering the tiles and uses a
		// grayscale instead.
		bool m_rf_level_simple;
		// If true, renders chunks
		bool m_rf_chunks;
		// If false, Skips rendering Entities with RenderComponent
		bool m_rf_entities;
		// If true, renders the center and the velocity vector
		// of objects inheriting from PositionComponent
		bool m_rf_entities_positioncomponent;
		// If true, renders the collision models
		// of objects inheriting from CollidableComponent
		bool m_rf_entities_collision_boxes;
		// Enables or disables various graph renderings.
		bool m_rf_graph;
		// If true, displays a graph for render FPS.
		bool m_rf_graph_render_fps;
		// If true, displays a graph for simulation FPS.
		bool m_rf_graph_simulation_fps;

		size_t m_selected_option;
		const size_t selected_option_max = 11;
		std::chrono::time_point<std::chrono::system_clock> m_selected_last;

		void next_option() { if (++m_selected_option >= selected_option_max) { m_selected_option = 1; } m_selected_last = std::chrono::system_clock::now(); }
		void switch_option(uint8_t option) {
			switch (option)
			{
			case  1: m_rf_level = !m_rf_level; break;
			case  2: m_rf_level_surroundings = !m_rf_level_surroundings; break;
			case  3: m_rf_level_simple = !m_rf_level_simple; break;
			case  4: m_rf_chunks = !m_rf_chunks; break;
			case  5: m_rf_entities = !m_rf_entities; break;
			case  6: m_rf_entities_positioncomponent = !m_rf_entities_positioncomponent; break;
			case  7: m_rf_entities_collision_boxes = !m_rf_entities_collision_boxes; break;
			case  8: m_rf_graph = !m_rf_graph; break;
			case  9: m_rf_graph_render_fps = !m_rf_graph_render_fps; break;
			case 10: m_rf_graph_simulation_fps = !m_rf_graph_simulation_fps; break;
			}
			m_selected_last = std::chrono::system_clock::now();
		}
		bool value_option(uint8_t option) {
			switch (option)
			{
			case  1: return m_rf_level;
			case  2: return m_rf_level_surroundings;
			case  3: return m_rf_level_simple;
			case  4: return m_rf_chunks;
			case  5: return m_rf_entities;
			case  6: return m_rf_entities_positioncomponent;
			case  7: return m_rf_entities_collision_boxes;
			case  8: return m_rf_graph;
			case  9: return m_rf_graph_render_fps;
			case 10: return m_rf_graph_simulation_fps;
			default: return false;
			}
		}
		std::string name_option(uint8_t option) {
			switch (option)
			{
			case  1: return "level";
			case  2: return "level_surroundings";
			case  3: return "level_simple";
			case  4: return "chunks";
			case  5: return "entities";
			case  6: return "entities_positioncomponent";
			case  7: return "entities_collision_boxes";
			case  8: return "graph";
			case  9: return "graph_render_fps";
			case 10: return "graph_simulation_fps";
			default: return "";
			}
		}
		void previous_option() { if (m_selected_option <= 1) { m_selected_option = selected_option_max; } m_selected_option--; m_selected_last = std::chrono::system_clock::now(); }
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