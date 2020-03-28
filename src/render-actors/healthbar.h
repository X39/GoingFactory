#pragma once
#include "../Component.h"


namespace x39::goingfactory::actors::render
{
	class healthbar : public RenderComponent::RenderActor
	{
	public:

		// Inherited via RenderActor
		virtual void render(RenderComponent* component, GameInstance& game_instance, vec2 translate) override;
	};
}