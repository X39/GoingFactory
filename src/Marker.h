#pragma once
#include "Movable.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Marker : public Entity,
		public PositionComponent,
		public RenderComponent
	{
	public:
		virtual void render(GameInstance&, vec2) override;
		virtual std::string type_name() const override { return "Marker"; }
		virtual bool is_type(EComponent component) const override
		{
			if (Entity::is_type(component))
			{
				return true;
			}
			switch (component)
			{
			case x39::goingfactory::EComponent::Render:
			case x39::goingfactory::EComponent::Position:
				return true;
			default:
				return false;
			}
		}
	};
}