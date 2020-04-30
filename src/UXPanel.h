#pragma once
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "UXElement.h"
#include <vector>

namespace x39::goingfactory::ux
{
	class UXPanel : public UXElement
	{
	private:
		std::vector<UXElement*> m_uxelements;
		std::unordered_map<io::EKey, bool> m_key_pressed;
		io::EModifier m_modifier;
	public:
		UXPanel(GameInstance& game, int x, int y, int w, int h) :
			UXElement(game, x, y, w, h),
			m_key_pressed(),
			m_modifier(io::EModifier::EMPTY),
			m_uxelements()
		{ }
		virtual void render(GameInstance&) override;
		void push_back(UXElement* element) { m_uxelements.push_back(element); }

		virtual bool mouse_button_down(GameInstance& game, int px, int py, EMouseButton button) override
		{
			GameInstance::Log("DOWN");
			return true;
		}
		virtual void mouse_enter(GameInstance& game) override
		{
			GameInstance::Log("ENTER");
		}
		virtual void mouse_leave(GameInstance& game) override
		{
			GameInstance::Log("LEAVE");
		}

		bool is_key_down(io::EKey key) { return m_key_pressed.contains(key) ? m_key_pressed[key] : false; }
		bool is_modifier_active(io::EModifier modifier) { return ((int)m_modifier & (int)modifier) == (int)modifier; }
	};
}