#include "Marker.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
void x39::goingfactory::entity::Marker::render(GameInstance& game, vec2 translate)
{
	const size_t size = 16;
	auto pos = position() - translate - size / 2;
	al_draw_line(pos.x, pos.y, pos.x + size, pos.y + size, al_map_rgb(255, 0, 0), 1);
	al_draw_line(pos.x + size, pos.y, pos.x, pos.y + size, al_map_rgb(255, 0, 0), 1);
}