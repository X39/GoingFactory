#include "Player.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "ResourceManager.h"
#include "EntityManager.h"
#include "Entry.h"
#include "Movable.h"

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Player> entityRegister("Player",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Player>(); });


void x39::goingfactory::entity::Player::render(GameInstance& game)
{
	auto bitmap = game.resource_manager.get_bitmap(m_textures[m_texture_index]);
	float angle = m_velocity.to_radians() + /* 90° */ 1.5708;
	m_prev_rad = m_velocity.x == 0 && m_velocity.y == 0 ? m_prev_rad : angle;
	al_draw_rotated_bitmap(bitmap, 8, 8, m_pos.x, m_pos.y, m_prev_rad, NULL);
	m_texture_index++;
	if (m_textures.size() == m_texture_index)
	{
		m_texture_index = 0;
	}
}

void x39::goingfactory::entity::Player::render_init(GameInstance& game)
{
	m_textures.push_back(game.resource_manager.load_bitmap("Textures/Player.png"));
}

void x39::goingfactory::entity::Player::simulate(GameInstance& game)
{
	const float coef = 0.4;
	m_pos.x += m_velocity.x;
	m_velocity.x *= 0.9;
	m_pos.y += m_velocity.y;
	m_velocity.y *= 0.9;
	if (m_pos.x < 0) { m_pos.x = 0; }
	else if (m_pos.x > DISPLAY_WIDTH) { m_pos.x = DISPLAY_WIDTH; }
	if (m_pos.y < 0) { m_pos.y = 0; }
	else if (m_pos.y > DISPLAY_HEIGHT) { m_pos.y = DISPLAY_HEIGHT; }
}
void x39::goingfactory::entity::Player::interact(GameInstance& game, io::EPlayerInteraction playerInteraction)
{
	const float move_coef = 0.4 * 5;
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_left) != x39::goingfactory::io::EPlayerInteraction::empty)
	{ m_velocity.x += -move_coef; }
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_right) != x39::goingfactory::io::EPlayerInteraction::empty)
	{ m_velocity.x += move_coef; }
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_up) != x39::goingfactory::io::EPlayerInteraction::empty)
	{ m_velocity.y += -move_coef; }
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_down) != x39::goingfactory::io::EPlayerInteraction::empty)
	{ m_velocity.y += move_coef; }
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::trigger_a) != x39::goingfactory::io::EPlayerInteraction::empty)
	{
		auto movable = std::make_shared<Movable>();
		movable->pos(m_pos);
		auto new_velocity = vec2::from_radians(m_prev_rad - /* 90° */ 1.5708) * 10;
		movable->velocity(new_velocity);
		movable->velocity_tick_modifier(1);
		game.entity_manager.push_back(movable);
	}
}