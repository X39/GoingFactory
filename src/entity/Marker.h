#pragma once
#include "Entity.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Marker : public Entity,
		public PositionComponent,
		public RenderComponent,
		public CollidableComponent
	{
	public:
		static const int size = 16;
		Marker();
		virtual std::string type_name() const override { return "Marker"; }

		// Inherited via CollidableComponent
		virtual const std::vector<vec2> polygon_points() const override {
			auto pos = position();
			vec2 top_left = pos + vec2{ -(size / 2), -(size / 2) };
			vec2 top_right = top_left + vec2{ size, 0 };
			vec2 bot_left = top_left + vec2{ 0, size };
			vec2 bot_right = top_left + vec2{ size, size };
			return { top_left, top_right, bot_right, bot_left };
		}
	};
}