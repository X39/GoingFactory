#pragma once
#include "../Component.h"

#include <functional>

namespace x39::goingfactory::actors::simulate
{
	class collision : public SimulateComponent::SimulateActor
	{
	private:
		std::function<void(CollidableComponent*, CollidableComponent*, GameInstance&, float)> m_on_collision;
	public:
		collision() : m_on_collision() {}
		collision(std::function<void(CollidableComponent*, CollidableComponent*, GameInstance&, float)> on_collision) : m_on_collision(on_collision) {}
		// Inherited via SimulateActor
		virtual void simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}