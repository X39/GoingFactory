#include "slowdown.h"
#include <typeinfo>

void x39::goingfactory::actors::simulate::slowdown::simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto positionComponent = component->get_component<PositionComponent>();
    if (!positionComponent) { throw std::bad_cast(); }
    auto vel = positionComponent->velocity();
    float slowdown = m_coef * sim_coef;
    vel -= vel * slowdown;
    positionComponent->velocity(vel);
}
