#include "World.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Entity.h"
#include "EntityManager.h"

void x39::goingfactory::World::render(GameInstance& game)
{
	if (!m_player || !m_player->is_type(EComponent::Position))
	{
		return;
	}
	auto playerPositionComponent = m_player->get_component<PositionComponent>();
	vec2 center = { m_viewport_w / 2 + m_viewport_x, m_viewport_h / 2 + m_viewport_y };
	auto player_pos_centered = playerPositionComponent->position() - center;
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


	for (auto it = game.entity_manager.begin(EComponent::Render); it != game.entity_manager.end(EComponent::Render); it++)
	{
		if ((*it)->is_type(EComponent::Position))
		{
			auto positionComponent = (*it)->get_component<x39::goingfactory::PositionComponent>();
			auto position = positionComponent->position() - player_pos_centered;
			if (position.x > m_viewport_x&& position.x < m_viewport_w + m_viewport_x &&
				position.y > m_viewport_y&& position.y < m_viewport_h + m_viewport_y)
			{
				auto renderComponent = (*it)->get_component<x39::goingfactory::RenderComponent>();
				renderComponent->render(game, player_pos_centered);
			}
		}
		else
		{
			auto renderComponent = (*it)->get_component<x39::goingfactory::RenderComponent>();
			renderComponent->render(game, player_pos_centered);
		}
	}
}
