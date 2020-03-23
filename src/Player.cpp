#include "Player.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "ResourceManager.h"
#include "World.h"
#include "EntityManager.h"
#include "Marker.h"
#include "Laser.h"

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Player> entityRegister("Player",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Player>(); });


void x39::goingfactory::entity::Player::render(GameInstance& game, vec2 translate)
{
	auto bitmap = game.resource_manager.get_bitmap(m_textures[m_texture_index]);
	float angle = m_velocity.to_radians() + /* 90° */ 1.5708;
	m_prev_rad = m_velocity.x == 0 && m_velocity.y == 0 ? m_prev_rad : angle;
	auto pos = position() - translate;
	al_draw_rotated_bitmap(bitmap, 8, 8, pos.x, pos.y, m_prev_rad, NULL);
	m_texture_index++;
	if (m_textures.size() == m_texture_index)
	{
		m_texture_index = 0;
	}


	// HP Bar
	auto hp = health();
	auto left = pos.x - 8;
	auto right = pos.x + 8;
	auto right_reduction = (1 - hp) * (right - left);
	al_draw_line(left - 1, pos.y + 9, right + 1, pos.y + 9, al_map_rgb(0x32, 0x32, 0x32), 3);
	al_draw_line(left, pos.y + 9, right, pos.y + 9, al_map_rgb(127, 0, 0), 1);
	al_draw_line(left, pos.y + 9, right - right_reduction, pos.y + 9, al_map_rgb(0xff, 0xac, 0x41), 1);
	if (health() == 0)
	{
		exit(0);
	}
}

void x39::goingfactory::entity::Player::render_init(GameInstance& game)
{
	m_textures.push_back(game.resource_manager.load_bitmap("Textures/Player.png"));
}
void x39::goingfactory::entity::Player::interact(GameInstance& game, io::EPlayerInteraction playerInteraction)
{
	float move_coef = 1;
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::mod_a) != x39::goingfactory::io::EPlayerInteraction::empty)
	{ move_coef *= 2; }
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::mod_b) != x39::goingfactory::io::EPlayerInteraction::empty)
	{ move_coef *= 0.5; }
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
		double d = al_get_time();
		if (d - m_last_trigger_a > 0.25)
		{
			m_last_trigger_a = d;
			auto laser = new Laser();
			laser->position(position());
			laser->set_owner(this);
			auto new_velocity = vec2::from_radians(m_prev_rad - /* 90° */ 1.5708) * 10;
			laser->velocity(new_velocity);
			game.entity_manager.pool_create(laser);
		}
	}
	if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::trigger_b) != x39::goingfactory::io::EPlayerInteraction::empty)
	{
		double d = al_get_time();
		if (d - m_last_trigger_b > 1)
		{
			m_last_trigger_b = d;
			auto marker = new Marker();
			marker->position(position());
			game.entity_manager.pool_create(marker);
		}
	}
}