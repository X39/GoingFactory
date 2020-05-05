#include "collision.h"
#include "../entity/EntityManager.h"
#include "../World.h"
#include "../entity/Entity.h"

using namespace x39::goingfactory;
using namespace x39::goingfactory::entity;
void actors::simulate::collision::simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto collidableComponent = component->get_component<CollidableComponent>();
    auto positionComponent = component->get_component<PositionComponent>();
    if (!collidableComponent || !positionComponent) { throw std::bad_cast(); }
    auto vel = positionComponent->velocity();
    auto pos = positionComponent->position();

    if (collidableComponent->can_collide() && vel.length_squared() != 0)
    {
        int32_t orig_chunk_coord_x = EntityChunk::to_chunk_coordinate_x(pos);
        int32_t orig_chunk_coord_y = EntityChunk::to_chunk_coordinate_y(pos);

        for (auto i = -1; i <= 1; i++)
        {
            for (auto j = -1; j <= 1; j++)
            {
                int32_t chunk_coord_x = orig_chunk_coord_x + i;
                int32_t chunk_coord_y = orig_chunk_coord_y + j;

                auto collisions = game_instance.world.get_chunk_world_collision(chunk_coord_x * EntityChunk::chunk_size, chunk_coord_y * EntityChunk::chunk_size);
                for (auto& collision : collisions)
                {
                    if (collidableComponent->intersects_with(collision))
                    {
                        positionComponent->velocity({});
                        if (m_on_collision)
                        {
                            m_on_collision(collidableComponent, nullptr, game_instance, sim_coef);
                        }
                        return;
                    }
                }

                for (
                    auto local_entity_it = game_instance.entity_manager.begin(chunk_coord_x, chunk_coord_y);
                    local_entity_it != game_instance.entity_manager.end(chunk_coord_x, chunk_coord_y);
                    local_entity_it++)
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
}
