#pragma once
#include "Entity.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Laser : public Entity,
		public RenderComponent,
		public CollidableComponent,
		public SimulateComponent,
		public PositionComponent
	{
	public:
		size_t ttl;
		Laser();
		virtual std::string type_name() const override { return "Laser"; }


		// Inherited via CollidableComponent
		virtual vec2 collidable_root() const { return position(); }
		virtual const std::vector<vec2> polygon_points() const override {
			auto pos = position();
			return { pos, pos + velocity() * last_sim_coef() };
		}
	};
}