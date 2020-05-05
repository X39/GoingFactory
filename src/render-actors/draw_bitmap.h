#pragma once
#include "../entity/Component.h"
#include "../texture.h"

#include <string>
#include <initializer_list>


namespace x39::goingfactory::actors::render
{
	class draw_bitmap : public entity::RenderComponent::RenderActor
	{
	private:
		std::vector<std::string> m_textures_tmp;
		std::vector<texture> m_textures;
		size_t m_texture_index;
		float m_prev_rad;
	public:
		draw_bitmap(std::initializer_list<std::string> initializer) : m_textures_tmp(initializer), m_texture_index(0), m_prev_rad(0) {}
		// Inherited via RenderActor
		virtual void render_init(GameInstance& game_instance) override;
		virtual void render(entity::RenderComponent* component, GameInstance& game_instance, vec2 translate) override;
	};
}