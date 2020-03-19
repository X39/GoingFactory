#include "Player.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include "ResourceManager.h"
#include "EntityManager.h"



x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Player> entityRegister("Player",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Player>(); });


void x39::goingfactory::entity::Player::render(ResourceManager& resource_manager)
{
	al_draw_bitmap(resource_manager.get_bitmap(m_textures[m_texture_index]), m_pos.x, m_pos.y, 0);
	m_texture_index++;
	if (m_textures.size() == m_texture_index)
	{
		m_texture_index = 0;
	}
}

void x39::goingfactory::entity::Player::render_init(ResourceManager& resource_manager)
{
	m_textures.push_back(resource_manager.load_bitmap("Textures/1.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/2.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/3.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/4.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/5.png"));
}

void x39::goingfactory::entity::Player::simulate(EntityManager& entity_manager)
{
	const float coef = 0.1;
	m_pos += m_velocity;
	if (m_velocity.x > coef)
	{
		m_velocity.x -= coef;
	}
	else if (m_velocity.x < -coef)
	{
		m_velocity.x += coef;
	}
	else
	{
		m_velocity.x = 0;
	}
	if (m_velocity.y > coef)
	{
		m_velocity.y -= coef;
	}
	else if (m_velocity.y < -coef)
	{
		m_velocity.y += coef;
	}
	else
	{
		m_velocity.y = 0;
	}

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
