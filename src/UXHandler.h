#pragma once
#include "GameInstance.h"
#include "EKey.h"
#include "EModifier.h"
#include "UXElement.h"
#include "EMouseButton.h"
#include "Event.h"
#include "vec2.h"
#include "texture.h"

#include <sstream>
#include <unordered_map>
#include <algorithm>

namespace x39::goingfactory
{
	class ResourceManager;
	namespace ux
	{
		struct MouseMoveEventArgs
		{
			GameInstance& game;
			int px;
			int py;
			int dx;
			int dy;
		};
		struct MouseButtonEventArgs
		{
			bool cancel;
			GameInstance& game;
			int px;
			int py;
			EMouseButton button;
			bool pressed;
		};
		class UXHandler
		{
		private:
			std::vector<ux::UXElement*> m_uxelements;
			std::unordered_map<EMouseButton, bool> m_mouse_button_state;
			std::unordered_map<io::EKey, bool> m_keyboard_state;
			UXElement* m_focused;
			texture m_tx_panel_base;
			bool raise_mouseButton(GameInstance& game, int px, int py, EMouseButton button, bool pressed)
			{
				MouseButtonEventArgs args = { false, game, px, py, button, pressed };
				onMouseButton.raise(*this, args);
				return args.cancel;
			}
			void raise_mouseMove(GameInstance& game, int px, int py, int dx, int dy)
			{
				MouseMoveEventArgs args = { game, px, py, dx, dy };
				onMouseMove.raise(*this, args);
			}
			int m_mouse_x;
			int m_mouse_y;
		public:
			util::Event<UXHandler, MouseButtonEventArgs> onMouseButton;
			util::Event<UXHandler, MouseMoveEventArgs> onMouseMove;
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
				if (!raise_mouseButton(game, px, py, button, pressed))
				{
					for (auto uxelement : m_uxelements)
					{
						if (uxelement->handle_mouse_button(game, px, py, button, pressed))
						{
							break;
						}
					}
				}
			}
			void mouse_move(GameInstance& game, int px, int py, int dx, int dy)
			{
				m_mouse_x = px;
				m_mouse_y = py;
				raise_mouseMove(game, px, py, dx, dy);
				for (auto uxelement : m_uxelements)
				{
					uxelement->handle_mouse_move(game, px, py, dx, dy);
				}
			}
			vec2 mouse_position() { return { m_mouse_x, m_mouse_y }; }

			bool mouse_button_down(EMouseButton button) { return m_mouse_button_state.contains(button) ? m_mouse_button_state[button] : false; }
			bool key_down(io::EKey key) { return m_keyboard_state.contains(key) ? m_keyboard_state[key] : false; }


			void set_focused(GameInstance& game, UXElement* el)
			{
				if (m_focused) { m_focused->lost_focus(game); }
				m_focused = el;
				if (m_focused) { m_focused->gained_focus(game); }
			}
			void pop_back() { m_uxelements.pop_back(); }
			void push_back(ux::UXElement* el) { m_uxelements.push_back(el); }
			std::vector<ux::UXElement*>::iterator uxelements_begin() { return m_uxelements.begin(); }
			std::vector<ux::UXElement*>::iterator uxelements_end() { return m_uxelements.end(); }
			void erase(std::vector<ux::UXElement*>::iterator begin, std::vector<ux::UXElement*>::iterator end, ux::UXElement* value)
			{ m_uxelements.erase(std::remove(begin, end, value), end); }
			void erase(ux::UXElement* value)
			{ erase(m_uxelements.begin(), m_uxelements.end(), value); }

			void draw_panel(GameInstance& game, size_t px, size_t py, size_t pw, size_t ph);
		};
	}
}