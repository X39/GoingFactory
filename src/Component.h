#pragma once
#include <type_traits>
#include <vector>
#include <memory>
#include <cmath>

#include "Event.h"
#include "EKey.h"
#include "vec2.h"
#include "chunk.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "PlayerInteraction.h"
#include "EComponent.h"

namespace x39::goingfactory
{
    class Component
    {
    private:
        std::vector<EComponent> m_components;
    protected:
        void push_component(EComponent component) { m_components.push_back(component); }
    public:
        Component() : m_components() {}
        virtual bool is_type(EComponent component) const
        {
            for (auto it : m_components)
            {
                if (it == component)
                {
                    return true;
                }
            }
            return false;
        }
        template<typename T>
        T* get_component() {
            static_assert(std::is_base_of<Component, T>::value, "x39::goingfactory::entity::Entity::get_component<T>() can only convert to x39::goingfactory::Component types");
            return dynamic_cast<T*>(this);
        }
    };
    class PositionComponent : virtual public Component
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
        PositionComponent() : Component(), m_pos(0, 0), m_chunk(nullptr), m_entity_manager(nullptr) { push_component(type()); }
        static EComponent type() { return EComponent::Position; }
        vec2 position() const { return m_pos; }
        void position(vec2);
        const chunk* chunk() const { return m_chunk; }
    };
    class RenderComponent : virtual public Component
    {
    public:
        RenderComponent() : Component() { push_component(EComponent::Render); }
        virtual void render_init(GameInstance&) {}
        virtual void render(GameInstance&, vec2) = 0;
        static EComponent type() { return EComponent::Render; }
    };
    class SimulateComponent : virtual public Component
    {
    public:
        SimulateComponent() : Component() { push_component(EComponent::Simulate); }
        virtual void simulate(GameInstance& game, float sim_coef) = 0;
        static EComponent type() { return EComponent::Simulate; }
    };
    class HealthComponent : virtual public Component
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

        HealthComponent() : Component(), m_health(1), m_allow_damage(true) { push_component(EComponent::Health); }
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
    class KeyboardComponent : virtual public Component
    {
    public:
        KeyboardComponent() : Component() { push_component(EComponent::Keyboard); }
        virtual void key_down(GameInstance&, io::EKey, io::EModifier) {};
        virtual void key_up(GameInstance&, io::EKey, io::EModifier) {};
        static EComponent type() { return EComponent::Keyboard; }
    };
    class PlayerInteractibleComponent : virtual public Component
    {
    public:
        PlayerInteractibleComponent() : Component() { push_component(EComponent::PlayerInteractible); }
        static EComponent type() { return EComponent::PlayerInteractible; }
        virtual void interact(GameInstance&, io::EPlayerInteraction) = 0;
    };
    class CollidableComponent : virtual public Component
    {
    private:
        bool m_can_collide;
        bool separating_axis_theorem(
            const std::vector<vec2>& points_a,
            const std::vector<vec2>& points_b,
            float* overlap) const
        {
            const std::vector<vec2>* points_1 = &points_a;
            const std::vector<vec2>* points_2 = &points_b;

            if (overlap)
            {
                *overlap = INFINITY;
            }
            for (int i = 0; i <= 1; i++)
            {
                if (i == 1)
                {
                    points_2 = &points_a;
                    points_1 = &points_b;
                }
                for (size_t i = 0; i < points_1->size(); i++)
                {
                    vec2 p1 = (*points_1)[i];
                    vec2 p2 = (*points_1)[i + 1 >= points_1->size() ? 0 : i + 1];
                    // Find projected axis
                    auto vec = p2 - p1;
                    auto axis = vec.perpendicular_clockwise();

                    // Get minimum & maximum for projected axis with line_1
                    float min_1 = INFINITY, max_1 = -INFINITY;
                    for (size_t a = 0; a < points_1->size(); a++)
                    {
                        vec2 p = (*points_1)[a];
                        float projection = p.dotproduct(vec);
                        min_1 = std::min(projection, min_1);
                        max_1 = std::max(projection, max_1);
                    }

                    // Get minimum & maximum for projected axis with line_1
                    float min_2 = INFINITY, max_2 = -INFINITY;
                    for (size_t b = 0; b < points_2->size(); b++)
                    {
                        vec2 p = (*points_2)[b];
                        float projection = p.dotproduct(vec);
                        min_2 = std::min(projection, min_2);
                        max_2 = std::max(projection, max_2);
                    }


                    // Check if overlap is occuring between max & min of a and b
                    if (!(max_1 >= min_2 && max_2 >= min_1))
                    { // No overlap, collission impossible. Return false.
                        return false;
                    }
                    if (overlap)
                    {
                        *overlap = std::min(std::min(max_1, max_2) - std::max(min_1, min_2), *overlap);
                    }
                }
            }
            return true;
        }
    protected:
        virtual void collision_happened(CollidableComponent& other) {}
    public:
        CollidableComponent() : m_can_collide(true) { push_component(type()); }
        static EComponent type() { return EComponent::Collidable; }
        virtual const std::vector<vec2> polygon_points() const = 0;
        bool can_collide() { return m_can_collide; }
        void can_collide(bool flag) { m_can_collide = flag; }

        bool intersects_with(CollidableComponent& other, float* distance)
        {
            if (!m_can_collide || !other.m_can_collide)
            {
                return false;
            }
            return separating_axis_theorem(polygon_points(), other.polygon_points(), distance);
        }
    };
}