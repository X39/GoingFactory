#include "velocitycap.h"

void x39::goingfactory::actors::simulate::velocitycap::simulate(entity::SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto positionComponent = component->get_component<entity::PositionComponent>();
    if (!positionComponent) { throw std::bad_cast(); }
    auto vel = positionComponent->velocity();
    if (vel.length_squared() > m_coef_squared)
    {
        vel.normalize();
        vel *= m_coef;
        positionComponent->velocity(vel);
    }
}
