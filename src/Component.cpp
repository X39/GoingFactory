#include "Component.h"

#include "EntityManager.h"

void x39::goingfactory::PositionComponent::position(vec2 newpos)
{
	if (!m_chunk || !m_entity_manager)
	{
		m_pos = newpos;
	}
	else
	{
		auto coord = chunk::to_chunk_coordinate(newpos);
		if (coord != m_chunk->coordinate())
		{
			m_entity_manager->pool_chunk_update(this);
		}
		m_pos = newpos;
	}
}
