#include "UXPlaceable.h"
#include "GameInstance.h"
#include "ResourceManager.h"
#include "UXHandler.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>


x39::goingfactory::ux::UXPlaceable::UXPlaceable(GameInstance& game, int x, int y, int w, int h) :
	UXElement(game, x, y, w, h),
	m_tx_placeable_display(0),
	m_tx_placeable_render_preview(0),
	m_placeable_width(0),
	m_placeable_height(0),
	m_queue_left(0),
	m_queue_done(0),
	m_queue_max(1),
	m_build_start(),
	m_build_time()
{
	m_tx_placeable_render_preview = game.resource_manager.load_bitmap("textures/tesla_generator/preview.png");
	m_tx_placeable_display = game.resource_manager.load_bitmap("textures/tesla_generator/icon.png");
	m_tx_placeable_display_highlight = game.resource_manager.load_bitmap("textures/tesla_generator/icon_highlight.png");
	m_placeable_height = m_placeable_width = 32;

	m_build_time = std::chrono::seconds(10);
}

void x39::goingfactory::ux::UXPlaceable::render(GameInstance& game)
{
	if (mouse_inside())
	{
		al_draw_tinted_scaled_rotated_bitmap(
			game.resource_manager.get_bitmap(m_tx_placeable_display_highlight),
			al_map_rgb(255, 255, 255),
			0, 0,
			x, y,
			w / m_placeable_width, h / m_placeable_height,
			0, 0
		);
	}
	else
	{
		al_draw_tinted_scaled_rotated_bitmap(
			game.resource_manager.get_bitmap(m_tx_placeable_display),
			al_map_rgb(255, 255, 255),
			0, 0,
			x, y,
			w / m_placeable_width, h / m_placeable_height,
			0, 0
		);
	}
	if (m_queue_left > 0 || m_queue_done > 0)
	{
		al_draw_textf(game.resource_manager.font(), al_map_rgb(255, 255, 0), x + 1, y + 1, 0, "%llu/%llu", (unsigned long long) m_queue_left, (unsigned long long) m_queue_done);
	}
	if (m_queue_left > 0)
	{
		auto deltaT = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_build_start);
		auto deltaTLeft = m_build_time - deltaT;
		auto ms_left = deltaTLeft.count();
		auto ms_build_time = m_build_time.count();
		auto deltaProgress = ((ms_build_time - ms_left) / (double)ms_build_time);
		auto progress = ms_left < 0 ? w : deltaProgress * w;
		al_draw_line(x, y + h - 1, x + progress, y + h - 1, al_map_rgb(255, 255, 0), 1);
	}
	else if (m_queue_done > 0)
	{
		al_draw_line(x, y + h - 1, x + w, y + h - 1, al_map_rgb(0, 255, 0), 1);
	}
}

void x39::goingfactory::ux::UXPlaceable::tick(GameInstance& game)
{
	if (m_queue_left > 0)
	{
		auto deltaT = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_build_start);
		auto deltaTLeft = m_build_time - deltaT;
		auto ms_left = deltaTLeft.count();
		if (ms_left < 0)
		{
			m_queue_left--;
			m_queue_done++;
			if (m_queue_left > 0)
			{
				m_build_start = std::chrono::steady_clock::now();
			}
		}
	}
}
