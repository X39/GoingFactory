#pragma once
#include "Entity.h"
#include "../texture.h"
#include <cstdint>

extern "C" struct yaoosl_instance;
namespace x39::goingfactory::entity
{
	class ScriptedEntity : public Entity,
		public PositionComponent,
		public RenderComponent,
		public SimulateComponent,
		public CollidableComponent
	{
	private:
		texture m_texture;
		vec2 m_texture_center;
	public:
		static const int size = 16;
		ScriptedEntity();

		void texture(texture val) { m_texture = val; }
		x39::goingfactory::texture texture() { return m_texture; }

		void texture_center(vec2 val) { m_texture_center = val; }
		vec2 texture_center() { return m_texture_center; }

		virtual std::string type_name() const override { return "ScriptedEntity"; }

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