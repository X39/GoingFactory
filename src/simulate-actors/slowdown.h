#pragma once
#include "../entity/Component.h"


namespace x39::goingfactory::actors::simulate
{
	class slowdown : public entity::SimulateComponent::SimulateActor
	{
	private:
		float m_coef;
	public:
		slowdown(float coef) : m_coef(coef) {}
		// Inherited via SimulateActor
		virtual void simulate(entity::SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}