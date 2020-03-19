#include "Movable.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "ResourceManager.h"
#include "EntityManager.h"
#include "Entry.h"

x39::goingfactory::entity::EntityRegister<x39::goingfactory::entity::Movable> entityRegister("Movable",
	[]() -> std::shared_ptr<x39::goingfactory::entity::Entity> { return std::make_shared<x39::goingfactory::entity::Movable>(); });


void x39::goingfactory::entity::Movable::render(ResourceManager& resource_manager)
{
	float angle = m_velocity.angle() + /* 90° */ 1.5708;
	m_prev_rad = m_velocity.x == 0 && m_velocity.y == 0 ? m_prev_rad : angle;
	auto color = al_map_rgb(255, 255, 255);
	al_draw_line(m_pos.x + 0, m_pos.y + 0, m_pos.x + 3, m_pos.y + 3, color, 1);
	al_draw_line(m_pos.x + 3, m_pos.y + 3, m_pos.x + 5, m_pos.y + 4, color, 1);
	al_draw_line(m_pos.x + 5, m_pos.y + 4, m_pos.x + 2, m_pos.y + 5, color, 1);
	al_draw_line(m_pos.x + 2, m_pos.y + 5, m_pos.x + 0, m_pos.y + 2, color, 1);
	al_draw_line(m_pos.x + 0, m_pos.y + 2, m_pos.x + 0, m_pos.y + 0, color, 1);
}

void x39::goingfactory::entity::Movable::simulate(EntityManager& entity_manager)
{
	const float coef = 0.4;
	m_pos.x += m_velocity.x;
	m_pos.y += m_velocity.y;
	if (m_slowdown > 0)
	{
		m_velocity.x *= m_slowdown;
		m_velocity.y *= m_slowdown;
	}
	if (m_pos.x < 0) { m_pos.x = 0; }
	else if (m_pos.x > DISPLAY_WIDTH) { m_pos.x = DISPLAY_WIDTH; }
	if (m_pos.y < 0) { m_pos.y = 0; }
	else if (m_pos.y > DISPLAY_HEIGHT) { m_pos.y = DISPLAY_HEIGHT; }
}