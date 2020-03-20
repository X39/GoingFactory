#pragma once
#include "Entity.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Movable : public Entity,
		public RenderComponent,
		public SimulateComponent
	{
		static EntityRegister<Movable> entityRegister;
		vec2 m_velocity;
		float m_prev_rad;
		float m_velocity_tick_modifier;
	public:
		Movable() : Entity(), m_velocity(0, 0), m_prev_rad(0), m_velocity_tick_modifier(0.9) { }
		position velocity() const { return m_velocity; }
		void velocity(position pos) { m_velocity = pos; }

		void velocity_tick_modifier(float f) { m_velocity_tick_modifier = f; }
		float velocity_tick_modifier() { return m_velocity_tick_modifier; }
		virtual void render(GameInstance&, vec2) override;
		virtual void simulate(GameInstance&) override;

		virtual bool is_type(EComponent component) const override
		{
			switch (component)
			{
			case x39::goingfactory::EComponent::Render:
			case x39::goingfactory::EComponent::Simulate:
				return true;
			default:
				return false;
			}
		}
	};
}