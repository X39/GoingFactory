#include "Laser.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "EntityManager.h"

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Laser> entityRegister("Laser",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Laser>(); });
void x39::goingfactory::entity::Laser::render(GameInstance& game, vec2 translate)
{
	auto pos = m_pos - translate;
	al_draw_line(pos.x, pos.y, pos.x + m_velocity.x, pos.y + m_velocity.y, al_map_rgb(255, 0, 0), 1);
}

void x39::goingfactory::entity::Laser::simulate(GameInstance& game)
{
	Movable::simulate(game);
	vec2 topLeft = m_pos - 8;
	vec2 botRight = m_pos + 8;
	for (auto it : game.entity_manager)
	{
		if (!it || !it->is_type(EComponent::Health) || it.get() == m_owner) { continue; }
		auto position = it->pos();
		if (topLeft < position && position < botRight)
		{
			auto healthComponent = it->get_component<HealthComponent>();
			healthComponent->damage(0.25);
			m_ttl = 0;
			break;
		}
	}
	if (m_ttl-- == 0)
	{
		game.entity_manager.destroy(this);
	}
}
