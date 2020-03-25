#include "Movable.h"

#include "ResourceManager.h"
#include "EntityManager.h"
#include "World.h"


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Movable> entityRegister("Movable",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Movable>(); });


void x39::goingfactory::entity::Movable::simulate(GameInstance& game, float sim_coef)
{
	const float coef = 0.4f;
	auto original_pos = position();
	auto pos = original_pos;



	pos.x += m_velocity.x * sim_coef;
	pos.y += m_velocity.y * sim_coef;
	if (m_velocity_modifier < 1 && m_velocity_modifier >= 0)
	{
		m_velocity.x -= m_velocity.x * (m_velocity_modifier * sim_coef);
		m_velocity.y -= m_velocity.y * (m_velocity_modifier * sim_coef);
	}

	if (Component::is_type(EComponent::Collidable))
	{
		auto collidableComponent = Component::get_component<CollidableComponent>();
		if (collidableComponent->can_collide())
		{
			if (!game.world.get_tile(pos.x, pos.y).is_passable)
			{
				velocity({});
				return;
			}
			else
			{
				for (auto local_entity_it = game.entity_manager.begin(position()); local_entity_it != game.entity_manager.end(position()); local_entity_it++)
				{
					if (*local_entity_it == this) { continue; }
					if ((*local_entity_it)->is_type(EComponent::Collidable) && (*local_entity_it)->is_type(EComponent::Position))
					{
						auto otherCollidableComponent = (*local_entity_it)->get_component<CollidableComponent>();
						auto otherPositionComponent = (*local_entity_it)->get_component<PositionComponent>();
						x39::goingfactory::CollidableComponent::line delta;
						if (collidableComponent->intersects_with(*otherCollidableComponent, &delta))
						{
							m_velocity -= delta.p2 - delta.p1;
							pos = original_pos;
							pos.x += m_velocity.x * sim_coef;
							pos.y += m_velocity.y * sim_coef;
						}
					}
				}
			}
		}
	}


	position(pos);
}