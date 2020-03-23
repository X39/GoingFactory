#include "UXPanel.h"
#include "UXElement.h"

x39::goingfactory::ui::UXPanel::UXPanel() : m_key_pressed(), m_modifier(io::EModifier::EMPTY), m_uxelements()
{
}

x39::goingfactory::ui::UXPanel::~UXPanel()
{
	for (auto it : m_uxelements)
	{
		delete it;
	}
}

void x39::goingfactory::ui::UXPanel::key_down(GameInstance& game, io::EKey key, io::EModifier mod)
{
	m_modifier = mod;
	m_key_pressed[key] = true;
}

void x39::goingfactory::ui::UXPanel::key_up(GameInstance& game, io::EKey key, io::EModifier mod)
{
	m_modifier = mod;
	m_key_pressed[key] = false;
}

void x39::goingfactory::ui::UXPanel::tick(GameInstance& game)
{
	for (auto it : m_uxelements)
	{
		it->tick(game);
	}
}

void x39::goingfactory::ui::UXPanel::render(GameInstance& game)
{
	for (auto it : m_uxelements)
	{
		it->render(game);
	}
}
