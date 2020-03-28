#include "Marker.h"
#include "render-actors/HealthBar.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

class RenderActorCustom : public x39::goingfactory::RenderComponent::RenderActor
{
public:
	// Inherited via RenderActor
	virtual void render(x39::goingfactory::RenderComponent* component, x39::goingfactory::GameInstance& game_instance, x39::goingfactory::vec2 translate)
	{
		auto positionComponent = component->get_component<x39::goingfactory::PositionComponent>();
		if (!positionComponent) { throw std::bad_cast(); }
		auto pos = positionComponent->position() - translate - x39::goingfactory::entity::Marker::size / 2;
		al_draw_line(
			pos.x, pos.y,
			pos.x + x39::goingfactory::entity::Marker::size, pos.y + x39::goingfactory::entity::Marker::size,
			al_map_rgb(255, 0, 0), 1);
		al_draw_line(
			pos.x + x39::goingfactory::entity::Marker::size, pos.y,
			pos.x, pos.y + x39::goingfactory::entity::Marker::size,
			al_map_rgb(255, 0, 0), 1);
	}
};

x39::goingfactory::entity::Marker::Marker() : RenderComponent({ new RenderActorCustom() })
{
}
