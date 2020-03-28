#pragma once
#include "../Component.h"


namespace x39::goingfactory::actors::simulate
{
	class slowdown : public SimulateComponent::SimulateActor
	{
	private:
		float m_coef;
	public:
		slowdown(float coef) : m_coef(coef) {}
		// Inherited via SimulateActor
		virtual void simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}