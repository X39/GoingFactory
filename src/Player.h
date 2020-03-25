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
		virtual vec2 collidable_root() const { return position(); }
		virtual const std::vector<x39::goingfactory::CollidableComponent::line> collidable_lines() const override {
			auto pos = position();
			const int size = 16;
			vec2 top_left = pos + vec2{ -(size / 2), -(size / 2) };
			vec2 top_right = top_left + vec2{ size, 0 };
			vec2 bot_left = top_left + vec2{ 0, size };
			vec2 bot_right = top_left + vec2{ size, size };
			vec2 translated_top_left = top_left + m_velocity;
			vec2 translated_top_right = top_right + m_velocity;
			vec2 translated_bot_left = bot_left + m_velocity;
			vec2 translated_bot_right = bot_right + m_velocity;
			return {
				{ top_left, top_right },
				{ top_right, bot_right },
				{ bot_right, bot_left },
				{ bot_left, top_left },

				{ top_left, translated_top_left },
				{ top_right, translated_top_right },
				{ bot_right, translated_bot_right },
				{ bot_left, translated_bot_left },

				{ translated_top_left, translated_top_right },
				{ translated_top_right, translated_bot_right },
				{ translated_bot_right, translated_bot_left },
				{ translated_bot_left, translated_top_left }
			};
		}
	};
}