#include "Laser.h"
#include "simulate-actors/move.h"
#include "simulate-actors/collision.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "EntityManager.h"

static class RenderActorLaserCustom : public x39::goingfactory::RenderComponent::RenderActor
{
public:
	// Inherited via RenderActor
	virtual void render(x39::goingfactory::RenderComponent* component, x39::goingfactory::GameInstance& game_instance, x39::goingfactory::vec2 translate)
	{
		auto positionComponent = component->get_component<x39::goingfactory::PositionComponent>();
		if (!positionComponent) { throw std::bad_cast(); }

		auto pos = positionComponent->position() - translate;
		auto vel = positionComponent->velocity();
		vel.normalize();
		vel *= 10;
		al_draw_line(pos.x, pos.y, pos.x + vel.x, pos.y + vel.y, al_map_rgb(255, 0, 0), 1);
	}
};
static class SimulateActorLaserCustom : public x39::goingfactory::SimulateComponent::SimulateActor
{
public:
	// Inherited via SimulateActor
	virtual void simulate(x39::goingfactory::SimulateComponent* component, x39::goingfactory::GameInstance& game_instance, float sim_coef) override
	{
		auto laser = static_cast<x39::goingfactory::entity::Laser*>(component);
		if (--laser->ttl == 0)
		{
			game_instance.entity_manager.pool_destroy(laser);
		}
	}
};
x39::goingfactory::entity::Laser::Laser() :
	Entity(),
	ttl(200),
	RenderComponent({ new RenderActorLaserCustom() }),
	SimulateComponent({
	new actors::simulate::collision([](CollidableComponent* self, CollidableComponent* other, GameInstance& game_instance, float sim_coef) ->
	void {
			game_instance.entity_manager.pool_destroy(dynamic_cast<Entity*>(self));
			if (!other) { return; }
			if (other->is_type(EComponent::Health))
			{
				auto healthComponent = other->get_component<HealthComponent>();
				healthComponent->damage(0.25);
			}
			else
			{
				game_instance.entity_manager.pool_destroy(dynamic_cast<Entity*>(other));
			}
		}),
		new actors::simulate::move(), new SimulateActorLaserCustom() })
{
}