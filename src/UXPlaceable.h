#pragma once
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "UXElement.h"
#include <vector>
#include <chrono>

namespace x39::goingfactory::ux
{
	class UXPlaceable : public UXElement
	{
	private:
		size_t m_tx_placeable_display;
		size_t m_tx_placeable_display_highlight;
		size_t m_tx_placeable_render_preview;
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

		virtual bool mouse_button_down(GameInstance& game, int px, int py, EMouseButton button) override
		{
			if (m_queue_left + m_queue_done < m_queue_max)
			{
				if (m_queue_left == 0)
				{
					m_build_start = std::chrono::steady_clock::now();
				}
				m_queue_left++;
			}
			return true;
		}
		virtual void mouse_enter(GameInstance& game) override
		{
		}
		virtual void mouse_leave(GameInstance& game) override
		{
		}
	};
}