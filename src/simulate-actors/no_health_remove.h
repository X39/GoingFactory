#pragma once
#include "../Component.h"


namespace x39::goingfactory::actors::simulate
{
	class no_health_remove : public SimulateComponent::SimulateActor
	{
	public:

		// Inherited via SimulateActor
		virtual void simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}