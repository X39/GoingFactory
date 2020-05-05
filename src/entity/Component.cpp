#include "Component.h"

#include "EntityManager.h"

using namespace x39::goingfactory;
using namespace x39::goingfactory::entity;
void PositionComponent::position(vec2 newpos)
{
	OnPositionChangingEventArgs args(m_pos, newpos);
	OnPositionChanging.raise(*this, args);
	if (!m_chunk || !m_entity_manager)
	{
		m_pos = newpos;
	}
	else
	{
		auto coord = EntityChunk::to_chunk_coordinate(newpos);
		if (coord != m_chunk->coordinate())
		{
			m_entity_manager->pool_chunk_update(this);
		}
		m_pos = newpos;
	}
	position_changed();
}
