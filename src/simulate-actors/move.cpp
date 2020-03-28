#include "move.h"

void x39::goingfactory::actors::simulate::move::simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef)
{
    auto positionComponent = component->get_component<PositionComponent>();
    if (!positionComponent) { throw std::bad_cast(); }

    auto vel = positionComponent->velocity();
    auto pos = positionComponent->position();
    pos += vel * sim_coef;
    positionComponent->position(pos);
}
