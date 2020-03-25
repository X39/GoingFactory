#pragma once
#include "Movable.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Player : public Movable,
		public RenderComponent,
		public PlayerInteractibleComponent,
		public HealthComponent,
		public CollidableComponent
	{
	private:
		static EntityRegister<Player> entityRegister;
		std::vector<size_t> m_textures;
		size_t m_texture_index;

		float m_prev_rad = 0;
		double m_last_trigger_a = 0;
		double m_last_trigger_b = 0;
	public:
		Player() : Movable(), m_textures(), m_texture_index(0) {}

		virtual void render_init(GameInstance&) override;
		virtual void render(GameInstance&, vec2) override;
		virtual void interact(GameInstance&, io::EPlayerInteraction) override;

		virtual std::string type_name() const override { return "Player"; }

		// Inherited via CollidableComponent
		virtual const std::vector<x39::goingfactory::CollidableComponent::line>& collidable_lines() const override {
			auto pos = position();
			const int size = 16;
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



				{
					vec2{-(size / 2), -(size / 2) } -pos + m_velocity,
					vec2{-(size / 2), -(size / 2)} +vec2{ size, 0 } -pos },
				{
					vec2{-(size / 2), -(size / 2) } -pos + m_velocity,
					vec2{-(size / 2), -(size / 2)} +vec2{ 0, size } -pos + m_velocity
				},
				{
					vec2{-(size / 2), -(size / 2) } +vec2{ size, size } -pos + m_velocity,
					vec2{-(size / 2), -(size / 2)} +vec2{ size, 0 } -pos + m_velocity
				},
				{
					vec2{-(size / 2), -(size / 2) } +vec2{ size, size } -pos + m_velocity,
					vec2{-(size / 2), -(size / 2)} +vec2{ 0, size } -pos + m_velocity
				},
			};
		}
	};
}