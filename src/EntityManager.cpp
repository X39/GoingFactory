#include "EntityManager.h"
#include "Entity.h"
#include "Component.h"

#include <algorithm>
#include <functional>

void on_all_components(std::function<void(x39::goingfactory::EComponent)> func)
{
	for (int i = 0; i < (int)x39::goingfactory::EComponent::__MAX_COMPONENT; i++)
	{
		x39::goingfactory::EComponent component = static_cast<x39::goingfactory::EComponent>(i);
		func(component);
	}
}

void x39::goingfactory::EntityManager::do_act_pools()
{
	if (!m_chunk_update_pool.empty())
	{
		for (auto positionComponent : m_chunk_update_pool)
		{
			auto entity = dynamic_cast<entity::Entity*>(positionComponent);
			auto chunk_coord = chunk::to_chunk_coordinate(positionComponent->position());
			if (chunk_coord != positionComponent->m_chunk->coordinate())
			{
				// Remove from old chunk
				auto it = std::find(
					positionComponent->m_chunk->m_entities.begin(),
					positionComponent->m_chunk->m_entities.end(),
					entity);
				positionComponent->m_chunk->m_entities.erase(it);

				// Add to new chunk
				if (!m_chunks.contains(chunk_coord))
				{
					m_chunks[chunk_coord] = new chunk(chunk_coord);
				}
				auto chunk = m_chunks[chunk_coord];
				positionComponent->m_chunk = chunk;
				chunk->m_entities.push_back(entity);
			}
		}
		m_chunk_update_pool.clear();
	}
	if (!m_create_pool.empty())
	{
		for (auto entity : m_create_pool)
		{
			// Add to each specialized list
			on_all_components(
				[&](x39::goingfactory::EComponent component) -> void {
					if (!entity->is_type(component)) { return; }
					m_specialized_entities[component].push_back(entity);
				}
			);

			// Add to chunk if applicable
			if (entity->is_type(EComponent::Position))
			{
				auto positionComponent = entity->get_component<PositionComponent>();
				auto chunk_coord = chunk::to_chunk_coordinate(positionComponent->position());
				if (!m_chunks.contains(chunk_coord))
				{
					m_chunks[chunk_coord] = new chunk(chunk_coord);
				}
				auto chunk = m_chunks[chunk_coord];
				positionComponent->m_chunk = chunk;
				positionComponent->m_entity_manager = this;
				chunk->m_entities.push_back(entity);
			}
			// Add to all list
			m_all_entities.push_back(entity);

			// Eventing
			EntityAddedEventArgs args(entity);
			onEntityAdded.raise(*this, args);
		}
		m_create_pool.clear();
	}
	if (!m_destroy_pool.empty())
	{
		for (auto entity : m_destroy_pool)
		{
			auto it = std::find(m_all_entities.begin(), m_all_entities.end(), entity);
			if (it == m_all_entities.end())
			{
				continue;
			}
			// Eventing
			EntityRemovedEventArgs args(entity);
			onEntityRemoved.raise(*this, args);

			// Remove from each specialized list
			on_all_components(
				[&](x39::goingfactory::EComponent component) -> void {
					if (!entity->is_type(component)) { return; }
					auto specialized_it = std::find(m_specialized_entities[component].begin(), m_specialized_entities[component].end(), entity);
					m_specialized_entities[component].erase(specialized_it);
				}
			);

			// Remove from chunk if applicable
			if (entity->is_type(EComponent::Position))
			{
				auto positionComponent = entity->get_component<PositionComponent>();
				auto chunk = positionComponent->m_chunk;
				auto chunk_coord = chunk::to_chunk_coordinate(positionComponent->position());
				auto chunk_it = std::find(chunk->m_entities.begin(), chunk->m_entities.end(), entity);
				chunk->m_entities.erase(chunk_it);
			}
			// Remove from all list
			m_all_entities.erase(it);

			// Finally delete
			delete entity;
		}
		m_destroy_pool.clear();
	}
}

x39::goingfactory::EntityManager::~EntityManager()
{
	for (auto& it : m_all_entities)
	{
		EntityRemovedEventArgs args(it);
		onEntityRemoved.raise(*this, args);
		delete it;
	}
	for (auto& it : m_specialized_entities)
	{
		it.second.clear();
	}
	for (auto& it : m_chunks)
	{
		delete it.second;
	}
	m_chunks.clear();
}
