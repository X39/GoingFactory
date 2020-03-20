#include "Movable.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <array>

#include "ResourceManager.h"
#include "EntityManager.h"
#include "World.h"

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Movable> entityRegister("Movable",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Movable>(); });


void x39::goingfactory::entity::Movable::render(GameInstance& game, vec2 translate)
{
	float angle = m_velocity.to_radians() + /* 90° */ 1.5708;
	m_prev_rad = m_velocity.x == 0 && m_velocity.y == 0 ? m_prev_rad : angle;
	auto color = al_map_rgb(255, 255, 255);
	std::array<std::array<vec2, 2>, 8> arr = {
		std::array<vec2, 2> { vec2 { -3, 0 }, vec2 { -1, -2 } },
		std::array<vec2, 2> { vec2 { -1, -2 }, vec2 { 1, -3 } },
		std::array<vec2, 2> { vec2 { 1, -3 }, vec2 { 3, -2 } },
		std::array<vec2, 2> { vec2 { 3, -2 }, vec2 { 4, 1 } },
		std::array<vec2, 2> { vec2 { 4, 1 }, vec2 { 2, 2 } },
		std::array<vec2, 2> { vec2 { 2, 2 }, vec2 { -1, 2 } },
		std::array<vec2, 2> { vec2 { -1, 2 }, vec2 { -2, 2 } },
		std::array<vec2, 2> { vec2 { -2, 2 }, vec2 { -3, 0 } }
	};
	auto pos = m_pos - translate;
	for (auto it : arr)
	{
		const float scale = 2.0f;
		al_draw_line(pos.x + it[0].x * scale, pos.y + it[0].y * scale, pos.x + it[1].x * scale, pos.y + it[1].y * scale, color, 1);
	}
}

void x39::goingfactory::entity::Movable::simulate(GameInstance& game)
{
	const float coef = 0.4;
	m_pos.x += m_velocity.x;
	m_pos.y += m_velocity.y;
	if (m_velocity_tick_modifier < 1 && m_velocity_tick_modifier >= 0)
	{
		m_velocity.x *= m_velocity_tick_modifier;
		m_velocity.y *= m_velocity_tick_modifier;
	}
	if (m_pos.x < 0) { m_pos.x = 0; }
	else if (m_pos.x > game.world.level_width()) { m_pos.x = game.world.level_width(); }
	if (m_pos.y < 0) { m_pos.y = 0; }
	else if (m_pos.y > game.world.level_height()) { m_pos.y = game.world.level_height(); }
}