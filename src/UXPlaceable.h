#pragma once
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "UXElement.h"
#include "texture.h"

#include <vector>
#include <chrono>

namespace x39::goingfactory::ux
{
	class UXPlaceable : public UXElement
	{
	private:
		texture m_tx_placeable_display;
		texture m_tx_placeable_display_highlight;
		texture m_tx_placeable_render_preview;
		size_t m_placeable_width;
		size_t m_placeable_height;
		size_t m_queue_done;
		size_t m_queue_left;
		size_t m_queue_max;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_build_start;
		std::chrono::milliseconds m_build_time;
	public:
		UXPlaceable(GameInstance& game, int x, int y, int w, int h);
		virtual void render(GameInstance&) override;
		virtual void tick(GameInstance&) override;

		virtual bool mouse_button_down(GameInstance& game, int px, int py, EMouseButton button) override;
	};
}