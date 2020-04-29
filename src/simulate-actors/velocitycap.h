#pragma once
#include "../Component.h"


namespace x39::goingfactory::actors::simulate
{
	class velocitycap : public SimulateComponent::SimulateActor
	{
	private:
		float m_coef;
		float m_coef_squared;
	public:
		velocitycap(float coef) : m_coef(coef), m_coef_squared(coef * coef) {}
		// Inherited via SimulateActor
		virtual void simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}