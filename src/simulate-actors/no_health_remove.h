#pragma once
#include "../entity/Component.h"


namespace x39::goingfactory::actors::simulate
{
	class no_health_remove : public entity::SimulateComponent::SimulateActor
	{
	public:

		// Inherited via SimulateActor
		virtual void simulate(entity::SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}