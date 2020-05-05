#pragma once
#include "../entity/Component.h"


namespace x39::goingfactory::actors::simulate
{
	class move : public entity::SimulateComponent::SimulateActor
	{
	public:

		// Inherited via SimulateActor
		virtual void simulate(entity::SimulateComponent* component, GameInstance& game_instance, float sim_coef) override;
	};
}