#pragma once
#include "../entity/Component.h"

#include <functional>

namespace x39::goingfactory::actors::simulate
{
	class collision : public entity::SimulateComponent::SimulateActor
	{
	private:
		std::function<void(entity::CollidableComponent*, entity::CollidableComponent*, GameInstance&, float)> m_on_collision;
	public:
		collision() : m_on_collision() {}
		collision(std::function<void(entity::CollidableComponent*, entity::CollidableComponent*, GameInstance&, float)> on_collision) : m_on_collision(on_collision) {}
		// Inherited via SimulateActor
		virtual void simulate(entity::SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}