#pragma once
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "PlayerInteraction.h"

#include <unordered_map>

namespace x39::goingfactory::io
{
	class KeyboardTarget
	{
	private:
		std::unordered_map<EPlayerInteraction, EKey> m_interaction_key_map;
		std::unordered_map<EKey, bool> m_key_pressed;
		EModifier m_modifier;
	public:
		bool is_active(EPlayerInteraction playerInteraction) {
			return m_interaction_key_map.contains(playerInteraction) && is_key_down(m_interaction_key_map[playerInteraction]);
		}
		void map(EPlayerInteraction playerInteraction, EKey key) { m_interaction_key_map[playerInteraction] = key; }
		void key_down(EKey, EModifier);
		void key_up(EKey, EModifier);

		void entity_interact(GameInstance&);

		bool is_key_down(EKey key) { return m_key_pressed.contains(key) ? m_key_pressed[key] : false; }
		EModifier is_modifier() { return m_modifier; }
	};
}