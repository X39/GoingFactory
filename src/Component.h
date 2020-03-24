#pragma once
#include <type_traits>
#include <memory>

#include "Event.h"
#include "EKey.h"
#include "vec2.h"
#include "chunk.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "PlayerInteraction.h"
#include "EComponent.h"
#include "collision_primitives.h"

namespace x39::goingfactory
{
	class Component
	{
	public:
		virtual bool is_type(EComponent) const = 0;
		template<typename T>
		T* get_component() {
			static_assert(std::is_base_of<Component, T>::value, "x39::goingfactory::entity::Entity::get_component<T>() can only convert to x39::goingfactory::Component types");
			return dynamic_cast<T*>(this);
		}
	};
	class PositionComponent : public Component
	{
	private:
		friend class EntityManager;
		vec2 m_pos;
		chunk* m_chunk;
		EntityManager* m_entity_manager;
	protected:
		virtual void position_changed() {};
	public:
		struct OnPositionChangingEventArgs : public EventArgs {
		private:
			vec2 m_old_pos;
			vec2 m_new_pos;
		public:
			vec2 position_old() { return m_old_pos; }
			vec2 position_new() { return m_new_pos; }

			OnPositionChangingEventArgs(vec2 old_pos, vec2 new_pos) :
				m_old_pos(old_pos),
				m_new_pos(new_pos) {}
		};
		Event<PositionComponent, OnPositionChangingEventArgs> OnPositionChanging;
		PositionComponent() : m_pos(0, 0), m_chunk(nullptr), m_entity_manager(nullptr) {}
		vec2 position() { return m_pos; }
		void position(vec2);
		const chunk* chunk() { return m_chunk; }
	};
	class RenderComponent : public Component
	{
	public:
		virtual void render_init(GameInstance&) {}
		virtual void render(GameInstance&, vec2) = 0;
		static EComponent type() { return EComponent::Render; }
	};
	class SimulateComponent : public Component
	{
	public:
		virtual void simulate(GameInstance& game, float sim_coef) = 0;
		static EComponent type() { return EComponent::Simulate; }
	};
	class HealthComponent : public Component
	{
	protected:
		float m_health;
		bool m_allow_damage;
	public:

		struct OnHealthChangedEventArgs : public EventArgs {
		private:
			float m_health;
			float m_change;
		public:
			float health() { return m_health; }
			float change() { return m_change; }
			float result_health;

			OnHealthChangedEventArgs(float health, float change) :
				m_health(health),
				m_change(change),
				result_health(health + change) {}
		};
		Event<HealthComponent, OnHealthChangedEventArgs> onHealthChanged;

		HealthComponent() : m_health(1), m_allow_damage(true) {}
		float health() { return m_health; };
		void health(float value)
		{
			m_health = value;
			OnHealthChangedEventArgs args(m_health, 0);
			onHealthChanged.raise(*this, args);
			m_health = args.result_health;
		};
		void damage(float value) { heal(-value); }
		void heal(float value)
		{
			OnHealthChangedEventArgs args(m_health, value);
			onHealthChanged.raise(*this, args);
			float hp = args.result_health < 0 ? 0 : args.result_health;
			m_health = hp < m_health && m_allow_damage ? hp : m_health;
		}
		bool allow_damage() { return m_allow_damage; }
		void allow_damage(bool flag) { m_allow_damage = flag; }
		static EComponent type() { return EComponent::Health; }
	};
	class KeyboardComponent : public Component
	{
	public:
		virtual void key_down(GameInstance&, io::EKey, io::EModifier) {};
		virtual void key_up(GameInstance&, io::EKey, io::EModifier) {};
		static EComponent type() { return EComponent::Keyboard; }
	};
	class PlayerInteractibleComponent : public Component
	{
	public:
		static EComponent type() { return EComponent::PlayerInteractible; }
		virtual void interact(GameInstance&, io::EPlayerInteraction) = 0;
	};
	class CollidableComponent : public Component
	{
	private:
		bool m_can_collide;
	protected:
		virtual void collision_happened(CollidableComponent& other) {}
	public:
		CollidableComponent() : m_can_collide(true) {}
		static EComponent type() { return EComponent::Collidable; }
		virtual const std::vector<x39::goingfactory::primitives::collision*>& collidables() const = 0;
		bool can_collide() { return m_can_collide; }
		void can_collide(bool flag) { m_can_collide = flag; }
		bool collides_with(CollidableComponent& other)
		{
			if (!m_can_collide || !other.m_can_collide)
			{
				return false;
			}
			auto& self_collidables = collidables();
			auto& other_collidables = other.collidables();
			for (auto self_collidable : self_collidables)
			{
				for (auto other_collidable : other_collidables)
				{
					if (self_collidable->collides_with(*other_collidable))
					{
						collision_happened(other);
						return true;
					}
				}
			}
			return false;
		}
	};
}