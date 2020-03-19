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


void x39::goingfactory::entity::Player::render(ResourceManager& resource_manager)
{
	auto bitmap = resource_manager.get_bitmap(m_textures[m_texture_index]);
	float angle = m_velocity.angle() + /* 90° */ 1.5708;
	m_prev_rad = m_velocity.x == 0 && m_velocity.y == 0 ? m_prev_rad : angle;
	al_draw_rotated_bitmap(bitmap, 8, 8, m_pos.x, m_pos.y, m_prev_rad, NULL);
	m_texture_index++;
	if (m_textures.size() == m_texture_index)
	{
		m_texture_index = 0;
	}
}

void x39::goingfactory::entity::Player::render_init(ResourceManager& resource_manager)
{
	m_textures.push_back(resource_manager.load_bitmap("Textures/Player.png"));
}

void x39::goingfactory::entity::Player::simulate(EntityManager& entity_manager)
{
	const float coef = 0.4;
	m_pos.x += m_velocity.x;
	m_velocity.x *= 0.9;
	m_pos.y += m_velocity.y;
	m_velocity.y *= 0.9;
	if (m_up_pressed)
	{
		m_velocity.y += -coef * 5;
	}
	if (m_left_pressed)
	{
		m_velocity.x += -coef * 5;
	}
	if (m_down_pressed)
	{
		m_velocity.y += coef * 5;
	}
	if (m_right_pressed)
	{
		m_velocity.x += coef * 5;
	}
	if (m_pos.x < 0) { m_pos.x = 0; }
	else if (m_pos.x > DISPLAY_WIDTH) { m_pos.x = DISPLAY_WIDTH; }
	if (m_pos.y < 0) { m_pos.y = 0; }
	else if (m_pos.y > DISPLAY_HEIGHT) { m_pos.y = DISPLAY_HEIGHT; }
}

void x39::goingfactory::entity::Player::key_down(EntityManager& entity_manager, io::EKey key, io::EModifier modifier)
{
	switch (key)
	{
	case io::EKey::W:
		m_up_pressed = true;
		break;
	case io::EKey::A:
		m_left_pressed = true;
		break;
	case io::EKey::S:
		m_down_pressed = true;
		break;
	case io::EKey::D:
		m_right_pressed = true;
		break;
	case io::EKey::SPACE:
	{
		auto movable = std::make_shared<Movable>();
		movable->pos(m_pos);
		movable->velocity(m_velocity);
		movable->m_slowdown = 0;
		entity_manager.push_back(movable);
	}
		break;
	}
}

void x39::goingfactory::entity::Player::key_up(EntityManager& entity_manager, io::EKey key, io::EModifier modifier)
{
	switch (key)
	{
	case io::EKey::W:
		m_up_pressed = false;
		break;
	case io::EKey::A:
		m_left_pressed = false;
		break;
	case io::EKey::S:
		m_down_pressed = false;
		break;
	case io::EKey::D:
		m_right_pressed = false;
		break;
	}
}
