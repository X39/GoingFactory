#include "draw_bitmap.h"
#include "../ResourceManager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

void x39::goingfactory::actors::render::draw_bitmap::render_init(GameInstance& game_instance)
{
	m_textures.reserve(m_textures_tmp.size());
	for (auto s : m_textures_tmp)
	{
		m_textures.push_back(game_instance.resource_manager.load_bitmap(s));
	}
	m_textures_tmp.clear();
}

void x39::goingfactory::actors::render::draw_bitmap::render(RenderComponent* component, GameInstance& game_instance, vec2 translate)
{
	auto positionComponent = component->get_component<PositionComponent>();
	if (!positionComponent) { throw std::bad_cast(); }

	auto velocity = positionComponent->velocity();
	auto position = positionComponent->position();
	auto bitmap = m_textures[m_texture_index].bitmap();
	float angle = velocity.angle_radians() + /* 90° */ 1.5708;
	m_prev_rad = velocity.x == 0 && velocity.y == 0 ? m_prev_rad : angle;
	auto pos = position - translate;
	al_draw_rotated_bitmap(bitmap, 8, 8, pos.x, pos.y, m_prev_rad, NULL);
	m_texture_index++;
	if (m_textures.size() == m_texture_index)
	{
		m_texture_index = 0;
	}
}
