#include "healthbar.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

void x39::goingfactory::actors::render::healthbar::render(RenderComponent* component, GameInstance& game_instance, vec2 translate)
{
	auto positionComponent = component->get_component<x39::goingfactory::PositionComponent>();
	auto healthComponent = component->get_component<x39::goingfactory::HealthComponent>();
	if (!positionComponent || !healthComponent) { throw std::bad_cast(); }

	auto hp = healthComponent->health();
	auto pos = positionComponent->position() - translate;

	float left = pos.x + -2 - component->width() / 2;
	float right = pos.x + 2 + component->width() / 2;
	float right_reduction = (1 - hp) * (right - left);
	float vertical_offset = pos.y - 4 - component->height() / 2;
	al_draw_line(left - 1, vertical_offset, right + 1, vertical_offset, al_map_rgb(0x32, 0x32, 0x32), 3);
	al_draw_line(left, vertical_offset, right, vertical_offset, al_map_rgb(127, 0, 0), 1);
	al_draw_line(left, vertical_offset, right - right_reduction, vertical_offset, al_map_rgb(0xff, 0x1e, 0x56), 1);
}
