#include "World.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Entity.h"
#include "EntityManager.h"

void x39::goingfactory::World::render(GameInstance& game)
{
	if (!m_player)
	{
		return;
	}
	vec2 center = { m_viewport_w / 2 + m_viewport_x, m_viewport_h / 2 + m_viewport_y };
	auto player_pos_centered = m_player->pos() - center;
	auto color = al_map_rgb(0, 127, 0);
	al_draw_line(
		m_viewport_x,
		m_viewport_y,
		m_viewport_x + m_viewport_w,
		m_viewport_y,
		color, 1);
	al_draw_line(
		m_viewport_x,
		m_viewport_y,
		m_viewport_x,
		m_viewport_y + m_viewport_h,
		color, 1);
	al_draw_line(
		m_viewport_x + m_viewport_w,
		m_viewport_y + m_viewport_h,
		m_viewport_x + m_viewport_w,
		m_viewport_y,
		color, 1);
	al_draw_line(
		m_viewport_x + m_viewport_w,
		m_viewport_y + m_viewport_h,
		m_viewport_x,
		m_viewport_y + m_viewport_h,
		color, 1);

	color = al_map_rgb(127, 0, 0);
	al_draw_line(
		0 - player_pos_centered.x,
		0 - player_pos_centered.y,
		m_level_w - player_pos_centered.x,
		0 - player_pos_centered.y,
		color, 1);
	al_draw_line(
		0 - player_pos_centered.x,
		0 - player_pos_centered.y,
		0 - player_pos_centered.x,
		m_level_h - player_pos_centered.y,
		color, 1);
	al_draw_line(
		m_level_w - player_pos_centered.x,
		m_level_h - player_pos_centered.y,
		m_level_w - player_pos_centered.x,
		0 - player_pos_centered.y,
		color, 1);
	al_draw_line(
		m_level_w - player_pos_centered.x,
		m_level_h - player_pos_centered.y,
		0 - player_pos_centered.x,
		m_level_h - player_pos_centered.y,
		color, 1);


	for (auto it : game.entity_manager)
	{
		if (!it) { continue; }
		auto entity_pos = it->pos() - player_pos_centered;
		if (entity_pos.x > m_viewport_x && entity_pos.x < m_viewport_w + m_viewport_x &&
			entity_pos.y > m_viewport_y && entity_pos.y < m_viewport_h + m_viewport_y &&
			it->is_type(x39::goingfactory::EComponent::Render))
		{
			auto renderComponent = it->get_component<x39::goingfactory::RenderComponent>();
			renderComponent->render(game, player_pos_centered);
		}
	}
}
