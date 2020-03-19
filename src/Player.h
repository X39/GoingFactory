#pragma once
#include "Entity.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Player : public Entity,
		public RenderComponent,
		public KeyboardComponent,
		public SimulateComponent
	{
		static EntityRegister<Player> entityRegister;
		std::vector<size_t> m_textures;
		size_t m_texture_index;
		vec2 m_velocity;
		bool m_down_pressed = false;
		bool m_left_pressed = false;
		bool m_up_pressed = false;
		bool m_right_pressed = false;
	public:
		Player() : Entity(), m_textures(), m_texture_index(0), m_velocity(0, 0) { }
		position velocity() const { return m_velocity; }
		void velocity(position pos) { m_velocity = pos; }

		virtual void render_init(ResourceManager&) override;
		virtual void render(ResourceManager&) override;
		virtual void simulate(EntityManager&) override;
		virtual void key_down(EntityManager&, io::EKey, io::EModifier) override;
		virtual void key_up(EntityManager&, io::EKey, io::EModifier) override;

		virtual bool is_type(EComponent component) const override
		{
			switch (component)
			{
			case x39::goingfactory::EComponent::Render:
			case x39::goingfactory::EComponent::Simulate:
			case x39::goingfactory::EComponent::Keyboard:
				return true;
			default:
				return false;
			}
		}
	};
}