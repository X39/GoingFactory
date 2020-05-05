#pragma once
#include "../entity/Component.h"


namespace x39::goingfactory::actors::render
{
	class healthbar : public entity::RenderComponent::RenderActor
	{
	public:

		// Inherited via RenderActor
		virtual void render(entity::RenderComponent* component, GameInstance& game_instance, vec2 translate) override;
	};
}