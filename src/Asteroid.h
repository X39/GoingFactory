#pragma once
#include "Movable.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Asteroid : public Movable,
		public RenderComponent,
		public HealthComponent
	{
		static EntityRegister<Asteroid> entityRegister;
	public:
		Asteroid() : Movable() { m_velocity_tick_modifier = 1; }
		virtual void render(GameInstance&, vec2) override;
		virtual void simulate(GameInstance&) override;

		virtual std::string type_name() const override { return "Asteroid"; }
		virtual bool is_type(EComponent component) const override
		{
			if (Movable::is_type(component))
			{
				return true;
			}
			switch (component)
			{
			case x39::goingfactory::EComponent::Render:
			case x39::goingfactory::EComponent::Health:
				return true;
			default:
				return false;
			}
		}
	};
}