#pragma once
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"

#include <vector>

namespace x39::goingfactory::ui
{
	class UXElement;
	class UXPanel
	{
	private:
		std::vector<UXElement*> m_uxelements;
		std::unordered_map<io::EKey, bool> m_key_pressed;
		io::EModifier m_modifier;
	public:
		UXPanel();
		UXPanel(const UXPanel& panel) = delete;
		~UXPanel();
		void key_down(GameInstance&, io::EKey, io::EModifier);
		void key_up(GameInstance&, io::EKey, io::EModifier);
		void tick(GameInstance&);
		void render(GameInstance&);
		void push_back(UXElement* element) { m_uxelements.push_back(element); }

		bool is_key_down(io::EKey key) { return m_key_pressed.contains(key) ? m_key_pressed[key] : false; }
		bool is_modifier_active(io::EModifier modifier) { return ((int)m_modifier & (int)modifier) == (int)modifier; }
	};
}