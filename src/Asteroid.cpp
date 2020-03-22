#include "Asteroid.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <array>
#include "EntityManager.h"
#include "World.h"


x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Asteroid> entityRegister("Asteroid",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Asteroid>(); });


void x39::goingfactory::entity::Asteroid::render(GameInstance& game, vec2 translate)
{
	const float scale = 2.0f;

	float angle = m_velocity.to_radians() + /* 90° */ 1.5708;
	m_prev_rad = m_velocity.x == 0 && m_velocity.y == 0 ? m_prev_rad : angle;
	auto color = al_map_rgb(255, 0, 0);
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
	auto pos = position() - translate;
	for (auto it : arr)
	{
		al_draw_line(pos.x + it[0].x * scale, pos.y + it[0].y * scale, pos.x + it[1].x * scale, pos.y + it[1].y * scale, color, 1);
	}

	// HP Bar
	auto hp = health();
	auto left = pos.x + (-4 * scale);
	auto right = pos.x + (5 * scale);
	auto right_reduction = (1 - hp) * (right - left);
	al_draw_line(left - 1, pos.y + 9, right + 1, pos.y + 9, al_map_rgb(0x32, 0x32, 0x32), 3);
	al_draw_line(left, pos.y + 9, right, pos.y + 9, al_map_rgb(127, 0, 0), 1);
	al_draw_line(left, pos.y + 9, right - right_reduction, pos.y + 9, al_map_rgb(0xff, 0x1e, 0x56), 1);
	if (health() == 0)
	{
		game.entity_manager.pool_destroy(this);
	}
}

void x39::goingfactory::entity::Asteroid::simulate(GameInstance& game)
{
	Movable::simulate(game);
	vec2 topLeft = position() - 8;
	vec2 botRight = position() + 8;
	auto position = game.world.player()->get_component<PositionComponent>()->position();
	if (topLeft < position && position < botRight)
	{
		auto healthComponent = game.world.player()->get_component<HealthComponent>();
		healthComponent->damage(0.1);
		game.entity_manager.pool_destroy(this);
	}
}
