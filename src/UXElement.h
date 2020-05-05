#pragma once
#include "GameInstance.h"
#include "EKey.h"
#include "EModifier.h"
#include "EMouseButton.h"

namespace x39::goingfactory::ux
{
	class UXElement
	{
	private:
		bool m_mouse_inside;
		std::vector<UXElement*> m_uxelements;
		UXElement* m_parent;
	protected:
		UXElement* parent() { return m_parent; }
	public:
		int x;
		int y;
		int w;
		int h;
		UXElement(GameInstance& game) : x(0), y(0), w(0), h(0), m_mouse_inside(false), m_parent(nullptr) {}
		UXElement(GameInstance& game, int x, int y, int w, int h) : x(x), y(y), w(w), h(h), m_mouse_inside(false), m_parent(nullptr) {}

		virtual void render(GameInstance& game) = 0;

		virtual void key_down(GameInstance& game, io::EKey key, io::EModifier modifier) {}
		virtual void key_up(GameInstance& game, io::EKey key, io::EModifier modifier) {}
		virtual void gained_focus(GameInstance& game) {}
		virtual void lost_focus(GameInstance& game) {}
		virtual void mouse_move(GameInstance& game, int px, int py, int dx, int dy) {}
		virtual void mouse_enter(GameInstance& game) {}
		virtual void mouse_leave(GameInstance& game) {}
		// Called whenever a mouse button is pressed
		//
		// Param 0: The GameInstance
		// Param 1: Pixel location X
		// Param 2: Pixel location Y
		// Param 3: The button pressed
		// Return:  true if further handling should be cancelled, false if it may continue. 
		virtual bool mouse_button_down(GameInstance& game, int px, int py, EMouseButton button) { return false; }
		// Called whenever a mouse button is released
		//
		// Param 0: The GameInstance
		// Param 1: Pixel location X
		// Param 2: Pixel location Y
		// Param 3: The button pressed
		// Return:  true if further handling should be cancelled, false if it may continue.
		virtual bool mouse_button_up(GameInstance& game, int px, int py, EMouseButton button) { return false; }
		virtual void tick(GameInstance& game) {}

		bool mouse_inside() { return m_mouse_inside; }
		bool handle_mouse_button(GameInstance& game, int px, int py, EMouseButton button, bool pressed)
		{
			auto mouse_inside = (px >= x && px <= x + w) && (py >= y && py <= y + h);
			if (m_mouse_inside)
			{
				for (auto uxelement : m_uxelements)
				{
					if (uxelement->handle_mouse_button(game, px, py, button, pressed))
					{
						return true;
					}
				}
				if (pressed)
				{
					if (mouse_button_down(game, px, py, button))
					{
						return true;
					}
				}
				else
				{
					if (mouse_button_up(game, px, py, button))
					{
						return true;
					}
				}
			}
			return false;
		}
		void handle_mouse_move(GameInstance& game, int px, int py, int dx, int dy)
		{
			auto ppx = px - dx;
			auto ppy = py - dy;
			auto mouse_inside = (px >= x && px <= x + w) && (py >= y && py <= y + h);
			bool act_children = false;
			if (mouse_inside && !m_mouse_inside)
			{
				mouse_enter(game);
				act_children = true;
			}
			else if (!mouse_inside && m_mouse_inside)
			{
				mouse_leave(game);
				act_children = true;
			}
			m_mouse_inside = mouse_inside;
			if (mouse_inside)
			{
				mouse_move(game, px, py, dx, dy);
				act_children = true;
			}
			if (act_children)
			{
				for (auto uxelement : m_uxelements)
				{
					uxelement->handle_mouse_move(game, px, py, dx, dy);
				}
			}
		}
		void handle_render(GameInstance& game)
		{
			render(game);
			for (auto uxelement : m_uxelements)
			{
				uxelement->handle_render(game);
			}
		}
		void handle_tick(GameInstance& game)
		{
			tick(game);
			for (auto uxelement : m_uxelements)
			{
				uxelement->handle_tick(game);
			}
		}


		void push_back(ux::UXElement* el) { el->m_parent = this; m_uxelements.push_back(el); }
		std::vector<ux::UXElement*>::iterator uxelements_begin() { return m_uxelements.begin(); }
		std::vector<ux::UXElement*>::iterator uxelements_end() { return m_uxelements.end(); }
	};
}