#pragma once
#include "Entity.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Asteroid : public Entity,
		public PositionComponent,
		public SimulateComponent,
		public RenderComponent,
		public HealthComponent
	{
	public:
		Asteroid();

		virtual std::string type_name() const override { return "Asteroid"; }
	};
}