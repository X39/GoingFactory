#include "KeyboardTarget.h"
#include "Entity.h"
#include "EntityManager.h"

void x39::goingfactory::io::KeyboardTarget::key_down(EKey key, EModifier mod)
{
	m_modifier = mod;
	m_key_pressed[key] = true;
}

void x39::goingfactory::io::KeyboardTarget::key_up(EKey key, EModifier mod)
{
	m_modifier = mod;
	m_key_pressed[key] = false;
}

void x39::goingfactory::io::KeyboardTarget::entity_interact(GameInstance& game)
{
	x39::goingfactory::io::EPlayerInteraction interaction = x39::goingfactory::io::EPlayerInteraction::empty;
	if (is_active(x39::goingfactory::io::EPlayerInteraction::move_left)) { interaction |= x39::goingfactory::io::EPlayerInteraction::move_left; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::move_right)) { interaction |= x39::goingfactory::io::EPlayerInteraction::move_right; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::move_up)) { interaction |= x39::goingfactory::io::EPlayerInteraction::move_up; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::move_down)) { interaction |= x39::goingfactory::io::EPlayerInteraction::move_down; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::button_a)) { interaction |= x39::goingfactory::io::EPlayerInteraction::button_a; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::button_b)) { interaction |= x39::goingfactory::io::EPlayerInteraction::button_b; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::button_c)) { interaction |= x39::goingfactory::io::EPlayerInteraction::button_c; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::button_d)) { interaction |= x39::goingfactory::io::EPlayerInteraction::button_d; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::mod_a)) { interaction |= x39::goingfactory::io::EPlayerInteraction::mod_a; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::mod_b)) { interaction |= x39::goingfactory::io::EPlayerInteraction::mod_b; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::mod_c)) { interaction |= x39::goingfactory::io::EPlayerInteraction::mod_c; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::mod_d)) { interaction |= x39::goingfactory::io::EPlayerInteraction::mod_d; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::trigger_a)) { interaction |= x39::goingfactory::io::EPlayerInteraction::trigger_a; }
	if (is_active(x39::goingfactory::io::EPlayerInteraction::trigger_b)) { interaction |= x39::goingfactory::io::EPlayerInteraction::trigger_b; }
	if (interaction != x39::goingfactory::io::EPlayerInteraction::empty)
	{
		for (auto it : game.entity_manager)
		{
			if (!it) { continue; }
			if (it->is_type(EComponent::PlayerInteractible))
			{
				auto playerInteractible = it->get_component<PlayerInteractibleComponent>();
				playerInteractible->interact(game, interaction);
			}
		}
	}
}
