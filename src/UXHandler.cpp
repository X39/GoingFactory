#include "UXHandler.h"
#include "ResourceManager.h"

#include "allegro5/allegro_primitives.h"

#include <sstream>

x39::goingfactory::ux::UXHandler::UXHandler(ResourceManager& resource_manager) :
	m_uxelements(),
	m_focused(nullptr),
	m_tx_panel_base(0)
{
	m_tx_panel_base = resource_manager.load_bitmap("textures/ui/back.png");
}

void x39::goingfactory::ux::UXHandler::draw_panel(GameInstance& game, size_t px, size_t py, size_t pw, size_t ph)
{
	size_t i;
	// al_draw_line(px, py, px + pw, py, al_map_rgb(255, 255, 0), 1);
	// al_draw_line(px, py, px, py + pw, al_map_rgb(255, 255, 0), 1);
	// al_draw_line(px + pw, py + pw, px + pw, py, al_map_rgb(255, 255, 0), 1);
	// al_draw_line(px + pw, py + pw, px, py + pw, al_map_rgb(255, 255, 0), 1);
	auto texture = game.resource_manager.get_bitmap(m_tx_panel_base);

	al_hold_bitmap_drawing(true);
	al_draw_tinted_scaled_rotated_bitmap_region(
		texture,
		16, 16, 16, 16,
		al_map_rgb(255, 255, 255),
		0, 0,
		px + 16, py + 16, (pw - 16) / 16.0, (ph - 16) / 16,
		0, 0);

	// for (size_t x = px + 16; x < px + pw - 16; x++)
	// {
	// 	for (size_t y = py + 16; y < py + ph - 16; y++)
	// 	{
	// 		al_draw_bitmap_region(texture,
	// 			16,
	// 			16,
	// 			16,
	// 			16,
	// 			x,
	// 			y,
	// 			0);
	// 	}
	// }

	// Top Left
	al_draw_bitmap_region(texture,
		0,
		0,
		16,
		16,
		px,
		py,
		0);
	// Top
	for (i = px + 16; i < px + pw - 32; i += 16)
	{
		al_draw_bitmap_region(texture,
			16,
			0,
			16,
			16,
			i,
			py,
			0);
	}
	if (i < px + pw - 16)
	{
		al_draw_bitmap_region(texture,
			16,
			0,
			(px + pw - 16) - i,
			16,
			i,
			py,
			0);
	}
	// Top Right
	al_draw_bitmap_region(texture,
		32,
		0,
		16,
		16,
		px + pw - 16,
		py,
		0);
	// Right
	for (i = py + 16; i < py + ph - 32; i += 16)
	{
		al_draw_bitmap_region(texture,
			32,
			16,
			16,
			16,
			px + pw - 16,
			i,
			0);
	}
	if (i < py + ph - 16)
	{
		al_draw_bitmap_region(texture,
			32,
			16,
			16,
			(py + ph - 16) - i,
			px + pw - 16,
			i,
			0);
	}
	// Bot Right
	al_draw_bitmap_region(texture,
		32,
		32,
		16,
		16,
		px + pw - 16,
		py + ph - 16,
		0);
	// Bot
	for (i = px + 16; i < px + pw - 32; i += 16)
	{
		al_draw_bitmap_region(texture,
			16,
			32,
			16,
			16,
			i,
			py + ph - 16,
			0);
	}
	if (i < px + pw - 16)
	{
		al_draw_bitmap_region(texture,
			16,
			32,
			(px + pw - 16) - i,
			16,
			i,
			py + ph - 16,
			0);
	}
	// Bot Left
	al_draw_bitmap_region(texture,
		0,
		32,
		16,
		16,
		px,
		py + ph - 16,
		0);
	// Left
	for (i = py + 16; i < py + ph - 32; i += 16)
	{
		al_draw_bitmap_region(texture,
			0,
			16,
			16,
			16,
			px,
			i,
			0);
	}
	if (i < py + ph - 16)
	{
		al_draw_bitmap_region(texture,
			0,
			16,
			16,
			(py + ph - 16) - i,
			px,
			i,
			0);
	}
	al_hold_bitmap_drawing(false);
}