#include "KeyboardTarget.h"
#include "entity/Entity.h"
#include "entity/EntityManager.h"

using namespace x39::goingfactory;
using namespace x39::goingfactory::io;
void KeyboardTarget::key_down(EKey key, EModifier mod)
{
	m_modifier = mod;
	m_key_pressed[key] = true;
}

void KeyboardTarget::key_up(EKey key, EModifier mod)
{
	m_modifier = mod;
	m_key_pressed[key] = false;
}

void KeyboardTarget::entity_interact(GameInstance& game)
{
	EPlayerInteraction interaction = EPlayerInteraction::empty;
	if (is_active(EPlayerInteraction::move_left)) { interaction |= EPlayerInteraction::move_left; }
	if (is_active(EPlayerInteraction::move_right)) { interaction |= EPlayerInteraction::move_right; }
	if (is_active(EPlayerInteraction::move_up)) { interaction |= EPlayerInteraction::move_up; }
	if (is_active(EPlayerInteraction::move_down)) { interaction |= EPlayerInteraction::move_down; }
	if (is_active(EPlayerInteraction::button_a)) { interaction |= EPlayerInteraction::button_a; }
	if (is_active(EPlayerInteraction::button_b)) { interaction |= EPlayerInteraction::button_b; }
	if (is_active(EPlayerInteraction::button_c)) { interaction |= EPlayerInteraction::button_c; }
	if (is_active(EPlayerInteraction::button_d)) { interaction |= EPlayerInteraction::button_d; }
	if (is_active(EPlayerInteraction::mod_a)) { interaction |= EPlayerInteraction::mod_a; }
	if (is_active(EPlayerInteraction::mod_b)) { interaction |= EPlayerInteraction::mod_b; }
	if (is_active(EPlayerInteraction::mod_c)) { interaction |= EPlayerInteraction::mod_c; }
	if (is_active(EPlayerInteraction::mod_d)) { interaction |= EPlayerInteraction::mod_d; }
	if (is_active(EPlayerInteraction::trigger_a)) { interaction |= EPlayerInteraction::trigger_a; }
	if (is_active(EPlayerInteraction::trigger_b)) { interaction |= EPlayerInteraction::trigger_b; }
	if (interaction != EPlayerInteraction::empty)
	{
		for (auto it = game.entity_manager.begin(entity::EComponent::PlayerInteractible); it != game.entity_manager.end(entity::EComponent::PlayerInteractible); it++)
		{
			auto playerInteractible = (*it)->get_component<entity::PlayerInteractibleComponent>();
			playerInteractible->interact(game, interaction);
		}
	}
}
