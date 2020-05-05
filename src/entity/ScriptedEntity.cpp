#include "ScriptedEntity.h"
#include "../simulate-actors/move.h"
#include "../simulate-actors/collision.h"
#include "../GameInstance.h"
#include "../ResourceManager.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>


using namespace x39::goingfactory;
using namespace x39::goingfactory::entity;
class draw_bitmap : public RenderComponent::RenderActor
{
private:
	float m_prev_rad;
public:
	draw_bitmap() : m_prev_rad(0) {}
	virtual void render(
		RenderComponent* component,
		GameInstance& game_instance,
		vec2 translate) override
	{
		auto positionComponent = component->get_component<PositionComponent>();
		if (!positionComponent) { throw std::bad_cast(); }

		auto velocity = positionComponent->velocity();
		auto position = positionComponent->position();
		float angle = velocity.angle_radians() + /* 90° */ 1.5708;
		m_prev_rad = velocity.x == 0 && velocity.y == 0 ? m_prev_rad : angle;
		auto pos = position - translate;

		auto scriptedEntity = dynamic_cast<ScriptedEntity*>(component);
		if (scriptedEntity->texture().is_valid())
		{
			auto center = scriptedEntity->texture_center();
			al_draw_rotated_bitmap(scriptedEntity->texture().bitmap(), center.x, center.y, pos.x, pos.y, m_prev_rad, NULL);
		}
	}
};

ScriptedEntity::ScriptedEntity() :
	m_texture({ }),
	m_texture_center(8, 8),
	Entity(),
	RenderComponent({ new draw_bitmap() }),
	SimulateComponent({ new actors::simulate::collision(), new actors::simulate::move() })
{
}
