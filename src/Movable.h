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
		float m_prev_rad = 0;
	public:
		float m_slowdown = 0.9;
		Movable() : Entity(), m_velocity(0, 0) { }
		position velocity() const { return m_velocity; }
		void velocity(position pos) { m_velocity = pos; }

		virtual void render(ResourceManager&) override;
		virtual void simulate(EntityManager&) override;

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