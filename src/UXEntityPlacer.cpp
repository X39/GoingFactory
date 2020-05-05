#include "UXEntityPlacer.h"
#include "UXHandler.h"
#include "ResourceManager.h"
#include "entity/EntityManager.h"
#include "World.h"

#include <cmath>
#include <allegro5/allegro_primitives.h>
#include <algorithm>

x39::goingfactory::ux::UXEntityPlacer::UXEntityPlacer(GameInstance& game, std::function<entity::Entity * (vec2 position)> factory, texture preview_texture)
	: UXElement(game, 0, 0, 32, 32),
	m_entity_factory(factory),
	m_tx_preview(preview_texture)
{
	vec2 pos = game.uxhandler.mouse_position();
	x = pos.x - (m_tx_preview.width() / 2);
	y = pos.y - (m_tx_preview.height() / 2);
	w = m_tx_preview.width();
	h = m_tx_preview.height();
}

void x39::goingfactory::ux::UXEntityPlacer::render(GameInstance& game)
{
	vec2 screen = game.uxhandler.mouse_position();
	vec2 screen_adjusted = { screen.x + World::tile_size / 2, screen.y + World::tile_size  / 2 };
	vec2 world = game.world.screen_to_world(screen_adjusted);
	auto modx = std::fmod(world.x, (float)World::tile_size);
	auto mody = std::fmod(world.y, (float)World::tile_size);
	vec2 world_adjusted = {
			modx > 0 ? (float)(world.x - modx) : (float)(world.x - ((float)World::tile_size + modx)),
			mody > 0 ? (float)(world.y - mody) : (float)(world.y - ((float)World::tile_size + mody))
	};
	vec2 screen_fixed = game.world.world_to_screen(world_adjusted);
	x = (int)screen_fixed.x - (m_tx_preview.width() / 2);
	y = (int)screen_fixed.y - (m_tx_preview.height() / 2);
	al_draw_bitmap(m_tx_preview.bitmap(), x, y, 0);
}

bool x39::goingfactory::ux::UXEntityPlacer::mouse_button_down(GameInstance& game, int px, int py, EMouseButton button)
{
	if (button == EMouseButton::LeftMouseButton)
	{
		vec2 screen = { px, py };
		vec2 screen_adjusted = { screen.x + World::tile_size / 2, screen.y + World::tile_size / 2 };
		vec2 world = game.world.screen_to_world(screen_adjusted);
		auto modx = std::fmod(world.x, (float)World::tile_size);
		auto mody = std::fmod(world.y, (float)World::tile_size);
		vec2 world_adjusted = {
			modx > 0 ? (float)(world.x - modx) : (float)(world.x - ((float)World::tile_size + modx)),
			mody > 0 ? (float)(world.y - mody) : (float)(world.y - ((float)World::tile_size + mody))
		};
		auto entity = m_entity_factory(world_adjusted);
		game.entity_manager.pool_create(entity);
		game.uxhandler.erase(this);
		delete this;
	}
	return false;
}
