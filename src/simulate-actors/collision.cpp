#include "collision.h"
#include "../EntityManager.h"
#include "../World.h"
#include "../Entity.h"

void x39::goingfactory::actors::simulate::collision::simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto collidableComponent = component->get_component<CollidableComponent>();
    auto positionComponent = component->get_component<PositionComponent>();
    if (!collidableComponent || !positionComponent) { throw std::bad_cast(); }
    auto vel = positionComponent->velocity();
    auto pos = positionComponent->position();

    if (collidableComponent->can_collide() && vel.length_squared() != 0)
    {
        // ToDo: Rework world collision to use actual points
        if (!game_instance.world.get_tile(pos.x, pos.y).is_passable)
        {
            positionComponent->velocity({});
            return;
        }
        else
        {
            for (auto local_entity_it = game_instance.entity_manager.begin(pos); local_entity_it != game_instance.entity_manager.end(pos); local_entity_it++)
            {
                if ((*local_entity_it)->is_type(EComponent::Collidable) && (*local_entity_it)->is_type(EComponent::Position))
                {
                    auto otherCollidableComponent = (*local_entity_it)->get_component<CollidableComponent>();

                    // Check if we try to collide against ourself here
                    if (otherCollidableComponent == collidableComponent) { continue; }

                    auto otherPositionComponent = (*local_entity_it)->get_component<PositionComponent>();


                    if (collidableComponent->intersects_with(*otherCollidableComponent))
                    {
                        positionComponent->velocity({});
                        if (m_on_collision)
                        {
                            m_on_collision(collidableComponent, otherCollidableComponent, game_instance, sim_coef);
                        }
                        return;
                    }
                }
            }
        }
    }
}
