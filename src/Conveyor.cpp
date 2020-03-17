#include "Conveyor.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include "ResourceManager.h"
#include "EntityManager.h"



x39::goingfactory::EntityRegister<x39::goingfactory::Conveyor> entityRegister("Conveyor",
	[]() -> std::shared_ptr<x39::goingfactory::Entity> { return std::make_shared<x39::goingfactory::Conveyor>(); });


void x39::goingfactory::Conveyor::render(ResourceManager& resource_manager)
{
	al_draw_bitmap(resource_manager.get_bitmap(m_textures[m_texture_index]), m_pos.x, m_pos.y, 0);
	m_texture_index++;
	if (m_textures.size() == m_texture_index)
	{
		m_texture_index = 0;
	}
}

void x39::goingfactory::Conveyor::initialize(ResourceManager& resource_manager)
{
	m_textures.push_back(resource_manager.load_bitmap("Textures/1.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/2.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/3.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/4.png"));
	m_textures.push_back(resource_manager.load_bitmap("Textures/5.png"));
}

void x39::goingfactory::Conveyor::simulate(EntityManager& entity_manager)
{
	m_pos.x++;
}
