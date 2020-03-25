#pragma once
#include "Movable.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Laser : public Movable,
		public RenderComponent
	{
		static EntityRegister<Laser> entityRegister;
		size_t m_ttl;
		Entity* m_owner;
	public:
		Laser() : Movable(), m_ttl(200), m_owner(nullptr) { m_velocity_tick_modifier = 1; }
		virtual void render(GameInstance&, vec2) override;
		virtual void simulate(GameInstance& game, float sim_coef) override;
		void set_owner(Entity* owner) { m_owner = owner; }
		virtual std::string type_name() const override { return "Laser"; }
	};
}