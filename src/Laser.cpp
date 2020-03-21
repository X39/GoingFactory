#include "Laser.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "EntityManager.h"

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Laser> entityRegister("Laser",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Laser>(); });
void x39::goingfactory::entity::Laser::render(GameInstance& game, vec2 translate)
{
	auto pos = position() - translate;
	al_draw_line(pos.x, pos.y, pos.x + m_velocity.x, pos.y + m_velocity.y, al_map_rgb(255, 0, 0), 1);
}

void x39::goingfactory::entity::Laser::simulate(GameInstance& game)
{
	Movable::simulate(game);
	vec2 topLeft = position() - 8;
	vec2 botRight = position() + 8;
	for (auto it = game.entity_manager.begin(position()); it != game.entity_manager.end(position()); it++)
	{
		if ((*it) == m_owner || !(*it)->is_type(EComponent::Health)) { continue; }
		auto position = (*it)->get_component<PositionComponent>()->position();
		if (topLeft < position && position < botRight)
		{
			auto healthComponent = (*it)->get_component<HealthComponent>();
			healthComponent->damage(0.25);
			m_ttl = 0;
			break;
		}
	}
	if (m_ttl-- == 0)
	{
		game.entity_manager.pool_destroy(this);
	}
}
