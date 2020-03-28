#include "no_health_remove.h"
#include "../EntityManager.h"
#include "../Entity.h"

void x39::goingfactory::actors::simulate::no_health_remove::simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto healthComponent = component->get_component<HealthComponent>();
    if (!healthComponent) { throw std::bad_cast(); }

    if (healthComponent->health() == 0)
    {
        game_instance.entity_manager.pool_destroy(dynamic_cast<entity::Entity*>(component));
    }
}
