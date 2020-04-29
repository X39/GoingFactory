#include "ScriptedEntity.h"
#include "simulate-actors/move.h"
#include "simulate-actors/collision.h"
#include "GameInstance.h"
#include "ResourceManager.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>


	static class draw_bitmap : public x39::goingfactory::RenderComponent::RenderActor
	{
	private:
		float m_prev_rad;
	public:
		draw_bitmap() : m_prev_rad(0) {}
		virtual void render(
			x39::goingfactory::RenderComponent* component,
			x39::goingfactory::GameInstance& game_instance,
			x39::goingfactory::vec2 translate) override
		{
			auto positionComponent = component->get_component<x39::goingfactory::PositionComponent>();
			if (!positionComponent) { throw std::bad_cast(); }

			auto velocity = positionComponent->velocity();
			auto position = positionComponent->position();
			float angle = velocity.angle_radians() + /* 90° */ 1.5708;
			m_prev_rad = velocity.x == 0 && velocity.y == 0 ? m_prev_rad : angle;
			auto pos = position - translate;

			auto scriptedEntity = dynamic_cast<x39::goingfactory::entity::ScriptedEntity*>(component);
			if (scriptedEntity->texture() != 0)
			{
				auto center = scriptedEntity->texture_center();
				auto bitmap = game_instance.resource_manager.get_bitmap(scriptedEntity->texture());
				al_draw_rotated_bitmap(bitmap, center.x, center.y, pos.x, pos.y, m_prev_rad, NULL);
			}
		}
	};

x39::goingfactory::entity::ScriptedEntity::ScriptedEntity() :
	m_texture(0),
	m_texture_center(8, 8),
	Entity(),
	RenderComponent({ new draw_bitmap() }),
	SimulateComponent({ new actors::simulate::collision(), new actors::simulate::move() })
{
}
