#include "Movable.h"

#include "ResourceManager.h"
#include "EntityManager.h"
#include "World.h"

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Movable> entityRegister("Movable",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Movable>(); });


void x39::goingfactory::entity::Movable::simulate(GameInstance& game)
{
	const float coef = 0.4f;
	auto pos = position();
	pos.x += m_velocity.x;
	pos.y += m_velocity.y;
	if (m_velocity_tick_modifier < 1 && m_velocity_tick_modifier >= 0)
	{
		m_velocity.x *= m_velocity_tick_modifier;
		m_velocity.y *= m_velocity_tick_modifier;
	}
	if (pos.x < 0) { pos.x = 0; }
	else if (pos.x > game.world.level_width()) { pos.x = game.world.level_width(); }
	if (pos.y < 0) { pos.y = 0; }
	else if (pos.y > game.world.level_height()) { pos.y = game.world.level_height(); }


	position(pos);
}