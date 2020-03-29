#pragma once
#include <type_traits>
#include <vector>
#include <memory>
#include <algorithm>
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
        vec2 m_velocity;
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
        PositionComponent() : Component(), m_pos(0, 0), m_chunk(nullptr), m_entity_manager(nullptr), m_velocity(0, 0) { push_component(type()); }
        static EComponent type() { return EComponent::Position; }
        vec2 position() const { return m_pos; }
        void position(vec2);
        vec2 velocity() const { return m_velocity; }
        void velocity(vec2 pos) { m_velocity = pos; }
        const chunk* chunk() const { return m_chunk; }
    };
    class RenderComponent : virtual public Component
    {
    public:
        class RenderActor
        {
        public:
            virtual void render(RenderComponent* component, GameInstance& game_instance, vec2 translate) = 0;
            virtual void render_init(GameInstance& game_instance) {}
        };
    private:
        std::vector<RenderActor*> m_actors;
        float m_width;
        float m_height;
    protected:
        void width(float value) { m_width = value; }
        void height(float value) { m_height = value; }
    public:
        float width() const { return m_width; }
        float height() const { return m_height; }
        RenderComponent() :
            Component(),
            m_actors(),
            m_width(0),
            m_height(0)
        {
            push_component(EComponent::Render);
        }
        RenderComponent(std::initializer_list<RenderActor*> initializer) :
            Component(),
            m_actors(initializer),
            m_width(0),
            m_height(0)
        {
            push_component(EComponent::Render);
        }
        ~RenderComponent()
        {
            for (auto actor : m_actors)
            {
                delete actor;
            }
        }
        void render_init(GameInstance& game_instance)
        {
            for (auto actor : m_actors)
            {
                actor->render_init(game_instance);
            }
        }
        void render(GameInstance& game_instance, vec2 translate)
        {
            for (auto actor : m_actors)
            {
                actor->render(this, game_instance, translate);
            }
        }
        void push_back(RenderActor* actor)
        {
            m_actors.push_back(actor);
        }
        void erase(RenderActor* actor)
        {
            // Lookup using reverse iterator as most likely we
            // want to remove temporary stuff, not permanent
            auto it = std::find(m_actors.rbegin(), m_actors.rend(), actor);
            if (it != m_actors.rend())
            {
                std::advance(it, 1);
                m_actors.erase(it.base());
            }
        }
        static EComponent type() { return EComponent::Render; }
    };
    class SimulateComponent : virtual public Component
    {
    public:
        class SimulateActor
        {
        public:
            virtual void simulate(SimulateComponent* component, GameInstance& game_instance, float sim_coef) = 0;
        };
    private:
        std::vector<SimulateActor*> m_actors;
        float m_sim_coef;
    protected:
        float last_sim_coef() const { return m_sim_coef; }
    public:
        SimulateComponent() :
            Component(),
            m_actors(),
            m_sim_coef(0)
        {
            push_component(EComponent::Simulate);
        }
        SimulateComponent(std::initializer_list<SimulateActor*> initializer) :
            Component(),
            m_actors(initializer),
            m_sim_coef(0)
        {
            push_component(EComponent::Simulate);
        }
        ~SimulateComponent()
        {
            for (auto actor : m_actors)
            {
                delete actor;
            }
        }
        void simulate(GameInstance& game_instance, float sim_coef)
        {
            m_sim_coef = sim_coef;
            for (auto actor : m_actors)
            {
                actor->simulate(this, game_instance, sim_coef);
            }
        }
        void push_back(SimulateActor* actor)
        {
            m_actors.push_back(actor);
        }
        void erase(SimulateActor* actor)
        {
            // Lookup using reverse iterator as most likely we
            // want to remove temporary stuff, not permanent
            auto it = std::find(m_actors.rbegin(), m_actors.rend(), actor);
            if (it != m_actors.rend())
            {
                std::advance(it, 1);
                m_actors.erase(it.base());
            }
        }
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
        std::vector<CollidableComponent*> m_no_collide;
        bool m_can_collide;
        bool separate_axis_theorem(
            const std::vector<vec2>& points_a,
            const std::vector<vec2>& points_b) const
        {
            const std::vector<vec2>* points_1 = &points_a;
            const std::vector<vec2>* points_2 = &points_b;
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
                    auto axis = vec.perpendicular_counter_clockwise();

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
                }
            }
            return true;
        }

        // not working
        bool line_collisions(
            const std::vector<vec2>& points_a,
            const std::vector<vec2>& points_b)
        {
            for (size_t a = 0; a < points_a.size(); a++)
            {
                vec2 self_line_p1 = points_a[a];
                vec2 self_line_p2 = points_a[a + 1 >= points_a.size() ? 0 : a + 1];
                for (size_t b = 0; b < points_b.size(); b++)
                {
                    vec2 othr_line_p1 = points_b[b];
                    vec2 othr_line_p2 = points_b[b + 1 >= points_b.size() ? 0 : b + 1];
                    // Check if both lines have the same slope
                    if ((self_line_p2.y - self_line_p1.y) / (self_line_p2.x - self_line_p1.x) == (othr_line_p2.y - othr_line_p1.y) / (othr_line_p2.x - othr_line_p1.x))
                    { // lines are parallel
                        // Check self_line p1 is outside of other_line
                        if (self_line_p1.x <= othr_line_p1.x || self_line_p1.y <= othr_line_p1.y) { continue; }
                        if (self_line_p1.x >= othr_line_p2.x || self_line_p1.y >= othr_line_p2.y) { continue; }
                        // Check self_line p2 is outside of other_line
                        if (self_line_p2.x <= othr_line_p1.x || self_line_p2.y <= othr_line_p1.y) { continue; }
                        if (self_line_p2.x >= othr_line_p2.x || self_line_p2.y >= othr_line_p2.y) { continue; }
                        // Ensure that self_line and othr_line have different base
                        auto dir_a = (self_line_p2 - self_line_p1); dir_a.normalize();
                        auto dir_b = (othr_line_p2 - othr_line_p1); dir_b.normalize();
                        if (dir_a == dir_b) { continue; }
                        // Lines are parallel & one point is inside the other & they do not have the same angle.
                        return true;
                    }
                    else
                    { // Lines are not parallel, find intersection
                        auto cross = (self_line_p2 - self_line_p1).crossproduct((othr_line_p2 - othr_line_p1));
                        // Check cross is outside of self_line
                        if (cross <= self_line_p1.x) { continue; }
                        if (cross >= self_line_p2.x) { continue; }
                        return true;
                    }
                }
            }
            return false;
        }
    public:
        CollidableComponent() : m_can_collide(true) { push_component(type()); }
        static EComponent type() { return EComponent::Collidable; }
        virtual const std::vector<vec2> polygon_points() const = 0;
        bool can_collide() { return m_can_collide; }
        void can_collide(bool flag) { m_can_collide = flag; }

        void no_collide(CollidableComponent* ptr) { m_no_collide.push_back(ptr); }

        bool intersects_with(CollidableComponent& other)
        {
            if (!m_can_collide || !other.m_can_collide)
            {
                return false;
            }
            auto it = std::find(m_no_collide.begin(), m_no_collide.end(), &other);
            if (it != m_no_collide.end())
            {
                return false;
            }
            it = std::find(other.m_no_collide.begin(), other.m_no_collide.end(), this);
            if (it != other.m_no_collide.end())
            {
                return false;
            }
            return separate_axis_theorem(polygon_points(), other.polygon_points());
        }
    };
}