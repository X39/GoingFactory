#pragma once
#include <type_traits>
#include <memory>

#include "Event.h"
#include "EKey.h"
#include "EModifier.h"

namespace x39::goingfactory
{
	class EntityManager;
	class ResourceManager;
	enum class EComponent
	{
		Render,
		Simulate,
		Health,
		Keyboard
	};
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
	class RenderComponent : public Component
	{
	public:
		virtual void render_init(ResourceManager&) {}
		virtual void render(ResourceManager&) = 0;
		static EComponent type() { return EComponent::Render; }
	};
	class SimulateComponent : public Component
	{
	public:
		virtual void simulate(EntityManager&) = 0;
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

		HealthComponent() : m_health(0), m_allow_damage(true) {}
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
		virtual void key_down(EntityManager&, io::EKey, io::EModifier) {};
		virtual void key_up(EntityManager&, io::EKey, io::EModifier) {};
		static EComponent type() { return EComponent::Keyboard; }
	};
}