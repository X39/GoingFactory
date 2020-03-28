#include "Asteroid.h"
#include "render-actors/healthbar.h"
#include "simulate-actors/move.h"
#include "simulate-actors/no_health_remove.h"
#include "simulate-actors/collision.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <array>
#include "EntityManager.h"
#include "World.h"

class RenderActorCustom : public x39::goingfactory::RenderComponent::RenderActor
{
public:
	// Inherited via RenderActor
	virtual void render(x39::goingfactory::RenderComponent* component, x39::goingfactory::GameInstance& game_instance, x39::goingfactory::vec2 translate)
	{
		const float scale = 2.0f;
		auto positionComponent = component->get_component<x39::goingfactory::PositionComponent>();
		if (!positionComponent) { throw std::bad_cast(); }

		auto velocity = positionComponent->velocity();

		float angle = velocity.angle_radians() + /* 90° */ 1.5708;
		auto color = al_map_rgb(255, 0, 0);
		std::array<std::array<x39::goingfactory::vec2, 2>, 8> arr = {
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { -3, 0 },  x39::goingfactory::vec2 { -1, -2 } },
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { -1, -2 }, x39::goingfactory::vec2 { 1, -3 } },
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { 1, -3 },  x39::goingfactory::vec2 { 3, -2 } },
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { 3, -2 },  x39::goingfactory::vec2 { 4, 1 } },
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { 4, 1 },   x39::goingfactory::vec2 { 2, 2 } },
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { 2, 2 },   x39::goingfactory::vec2 { -1, 2 } },
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { -1, 2 },  x39::goingfactory::vec2 { -2, 2 } },
			std::array<x39::goingfactory::vec2, 2> { x39::goingfactory::vec2 { -2, 2 },  x39::goingfactory::vec2 { -3, 0 } }
		};
		auto pos = positionComponent->position() - translate;
		for (auto it : arr)
		{
			al_draw_line(pos.x + it[0].x * scale, pos.y + it[0].y * scale, pos.x + it[1].x * scale, pos.y + it[1].y * scale, color, 1);
		}
	}
};

//void x39::goingfactory::entity::Asteroid::render(GameInstance& game, vec2 translate)
//{
//	const float scale = 2.0f;
//
//
//	// HP Bar
//	auto hp = health();
//	auto left = pos.x + (-4 * scale);
//	auto right = pos.x + (5 * scale);
//	auto right_reduction = (1 - hp) * (right - left);
//	al_draw_line(left - 1, pos.y + 9, right + 1, pos.y + 9, al_map_rgb(0x32, 0x32, 0x32), 3);
//	al_draw_line(left, pos.y + 9, right, pos.y + 9, al_map_rgb(127, 0, 0), 1);
//	al_draw_line(left, pos.y + 9, right - right_reduction, pos.y + 9, al_map_rgb(0xff, 0x1e, 0x56), 1);
//	if (health() == 0)
//	{
//		game.entity_manager.pool_destroy(this);
//	}
//}

//void x39::goingfactory::entity::Asteroid::simulate(GameInstance& game, float sim_coef)
//{
//	Movable::simulate(game, sim_coef);
//	vec2 topLeft = position() - 8;
//	vec2 botRight = position() + 8;
//	auto position = game.world.player()->get_component<PositionComponent>()->position();
//	if (topLeft < position && position < botRight)
//	{
//		auto healthComponent = game.world.player()->get_component<HealthComponent>();
//		healthComponent->damage(0.1);
//		game.entity_manager.pool_destroy(this);
//	}
//}

x39::goingfactory::entity::Asteroid::Asteroid() :
	Entity(),
	SimulateComponent({
	new actors::simulate::collision([](CollidableComponent* self, CollidableComponent* other, GameInstance& game_instance, float sim_coef) ->
	void {
			game_instance.entity_manager.pool_destroy(dynamic_cast<Entity*>(self));
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
		new actors::simulate::move(), new actors::simulate::no_health_remove() }),
	RenderComponent({ new actors::render::healthbar() })
{
	width(32);
	height(32);
}