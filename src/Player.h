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
		std::vector<primitives::collision*> m_collidables;
	protected:
		virtual void position_changed()
		{
			PositionComponent::position_changed();
			for (auto collidable : m_collidables)
			{
				collidable->position(position());
			}
		}
	public:
		Player() : Movable(), m_textures(), m_texture_index(0), m_collidables()
		{
			const int size = 16; // Our texture is 16x16 as of now (20200323)
			auto rect = new primitives::collisionrect();
			rect->set_boundaries(-(size / 2), -(size / 2), size, size);
			m_collidables.push_back(rect);
		}
		~Player()
		{
			for (auto it : m_collidables)
			{
				delete it;
			}
		}

		virtual void render_init(GameInstance&) override;
		virtual void render(GameInstance&, vec2) override;
		virtual void interact(GameInstance&, io::EPlayerInteraction) override;

		virtual std::string type_name() const override { return "Player"; }
		virtual bool is_type(EComponent component) const override
		{
			if (Movable::is_type(component)) { return true; }
			switch (component)
			{
			case x39::goingfactory::EComponent::Render:
			case x39::goingfactory::EComponent::PlayerInteractible:
			case x39::goingfactory::EComponent::Position:
			case x39::goingfactory::EComponent::Collidable:
			case x39::goingfactory::EComponent::Health:
				return true;
			default:
				return false;
			}
		}

		// Inherited via CollidableComponent
		virtual const std::vector<primitives::collision*>& collidables() const override { return m_collidables; }
	};
}