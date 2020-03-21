#pragma once
#include "Movable.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Player : public Movable,
		public RenderComponent,
		public PlayerInteractibleComponent,
		public HealthComponent
	{
		static EntityRegister<Player> entityRegister;
		std::vector<size_t> m_textures;
		size_t m_texture_index;

		float m_prev_rad = 0;
		double m_last_shot = 0;
	public:
		Player() : Movable(), m_textures(), m_texture_index(0) { }

		virtual void render_init(GameInstance&) override;
		virtual void render(GameInstance&, vec2) override;
		virtual void interact(GameInstance&, io::EPlayerInteraction) override;

		virtual bool is_type(EComponent component) const override
		{
			if (Movable::is_type(component)) { return true; }
			switch (component)
			{
			case x39::goingfactory::EComponent::Render:
			case x39::goingfactory::EComponent::PlayerInteractible:
			case x39::goingfactory::EComponent::Position:
			case x39::goingfactory::EComponent::Health:
				return true;
			default:
				return false;
			}
		}
	};
}