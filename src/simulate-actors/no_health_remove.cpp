#include "no_health_remove.h"
#include "../entity/EntityManager.h"
#include "../entity/Entity.h"

void x39::goingfactory::actors::simulate::no_health_remove::simulate(entity::SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto healthComponent = component->get_component<entity::HealthComponent>();
    if (!healthComponent) { throw std::bad_cast(); }

    if (healthComponent->health() == 0)
    {
        game_instance.entity_manager.pool_destroy(dynamic_cast<entity::Entity*>(component));
    }
}
