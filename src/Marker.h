#pragma once
#include "Movable.h"
#include <cstdint>

namespace x39::goingfactory::entity
{
	class Marker : public Entity,
		public PositionComponent,
		public RenderComponent,
		public CollidableComponent
	{
	private:
		const int size = 16;
		std::vector<primitives::collision*> m_collidables;
	protected:
		virtual void position_changed()
		{
			PositionComponent::position_changed();
			for (auto collidable : m_collidables)
			{
				collidable->position(position());
			}
		}
	public:
		Marker() : m_collidables()
		{
			auto rect = new primitives::collisionrect();
			rect->set_boundaries(-(size / 2), -(size / 2), size, size);
			m_collidables.push_back(rect);
		}
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
			case x39::goingfactory::EComponent::Collidable:
				return true;
			default:
				return false;
			}
		}

		// Inherited via CollidableComponent
		virtual const std::vector<primitives::collision*>& collidables() const override { return m_collidables; }
	};
}