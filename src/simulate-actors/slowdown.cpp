#include "slowdown.h"
#include <typeinfo>
#include <cmath>

void x39::goingfactory::actors::simulate::slowdown::simulate(entity::SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto positionComponent = component->get_component<entity::PositionComponent>();
    if (!positionComponent) { throw std::bad_cast(); }
    auto vel = positionComponent->velocity();
    float slowdown;
    slowdown = m_coef * sim_coef;
    auto newvel = vel - vel * slowdown;
    if (std::abs(vel.length_squared()) > std::abs(newvel.length_squared()))
    {
        vel = newvel;
    }
    else
    {
        vel = { 0, 0 };
    }
    positionComponent->velocity(vel);
}
