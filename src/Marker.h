#pragma once
#include "Movable.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Marker : public Entity,
		public PositionComponent,
		public RenderComponent,
		public CollidableComponent
	{
	private:
		const int size = 16;
	protected:
	public:
		Marker() {}
		virtual void render(GameInstance&, vec2) override;
		virtual std::string type_name() const override { return "Marker"; }

		// Inherited via CollidableComponent
		virtual const std::vector<x39::goingfactory::CollidableComponent::line>& collidable_lines() const override {
			auto pos = position();
			return {
				{
					vec2{-(size / 2), -(size / 2) } -pos,
					vec2{-(size / 2), -(size / 2)} +vec2{ size, 0 } -pos },
				{
					vec2{-(size / 2), -(size / 2) } -pos,
					vec2{-(size / 2), -(size / 2)} +vec2{ 0, size } -pos
				},
				{
					vec2{-(size / 2), -(size / 2) } +vec2{ size, size } -pos,
					vec2{-(size / 2), -(size / 2)} +vec2{ size, 0 } -pos
				},
				{
					vec2{-(size / 2), -(size / 2) } +vec2{ size, size } -pos,
					vec2{-(size / 2), -(size / 2)} +vec2{ 0, size } -pos
				},
			};
		}
	};
}