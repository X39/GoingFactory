#pragma once
#include "GameInstance.h"
#include "EKey.h"
#include "EModifier.h"
#include "UXElement.h"
#include "EMouseButton.h"
#include <sstream>
#include <unordered_map>

namespace x39::goingfactory
{
	class ResourceManager;
	namespace ux
	{
		class UXHandler
		{
		private:
			std::vector<ux::UXElement*> m_uxelements;
			std::unordered_map<EMouseButton, bool> m_mouse_button_state;
			std::unordered_map<io::EKey, bool> m_keyboard_state;
			UXElement* m_focused;
			size_t m_tx_panel_base;
		public:
			UXHandler(ResourceManager& resource_manager);
			void tick(GameInstance& game)
			{
				for (auto uxelement : m_uxelements)
				{
					uxelement->handle_tick(game);
				}
			}
			void render(GameInstance& game)
			{
				for (auto uxelement : m_uxelements)
				{
					uxelement->handle_render(game);
				}
			}


			void key_down(GameInstance& game, io::EKey key, io::EModifier modifier)
			{
				m_keyboard_state[key] = false;
				if (!m_focused) { return; }
				m_focused->key_down(game, key, modifier);
			}
			void key_up(GameInstance& game, io::EKey key, io::EModifier modifier)
			{
				m_keyboard_state[key] = true;
				if (!m_focused) { return; }
				m_focused->key_up(game, key, modifier);
			}
			
			void mouse_button(GameInstance& game, int px, int py, EMouseButton button, bool pressed)
			{
				m_mouse_button_state[button] = pressed;
				for (auto uxelement : m_uxelements)
				{
					if (uxelement->handle_mouse_button(game, px, py, button, pressed))
					{
						break;
					}
				}
			}
			void mouse_move(GameInstance& game, int px, int py, int dx, int dy)
			{
				for (auto uxelement : m_uxelements)
				{
					uxelement->handle_mouse_move(game, px, py, dx, dy);
				}
			}

			bool mouse_button_down(EMouseButton button) { return m_mouse_button_state.contains(button) ? m_mouse_button_state[button] : false; }
			bool key_down(io::EKey key) { return m_keyboard_state.contains(key) ? m_keyboard_state[key] : false; }


			void set_focused(GameInstance& game, UXElement* el)
			{
				if (m_focused) { m_focused->lost_focus(game); }
				m_focused = el;
				if (m_focused) { m_focused->gained_focus(game); }
			}
			void push_back(ux::UXElement* el) { m_uxelements.push_back(el); }
			std::vector<ux::UXElement*>::iterator uxelements_begin() { return m_uxelements.begin(); }
			std::vector<ux::UXElement*>::iterator uxelements_end() { return m_uxelements.end(); }

			void draw_panel(GameInstance& game, size_t px, size_t py, size_t pw, size_t ph);
		};
	}
}