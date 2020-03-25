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
		virtual vec2 collidable_root() const { return position(); }
		virtual const std::vector<x39::goingfactory::CollidableComponent::line> collidable_lines() const override {
			auto pos = position();
			vec2 top_left = pos + vec2{ -(size / 2), -(size / 2) };
			vec2 top_right = top_left + vec2{ size, 0 };
			vec2 bot_left = top_left + vec2{ 0, size };
			vec2 bot_right = top_left + vec2{ size, size };
			return { { top_left, top_right }, { top_right, bot_right }, { bot_right, bot_left }, { bot_left, top_left } };
		}
	};
}