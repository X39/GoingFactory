#pragma once

#include <cmath>
#include <array>
#include <vector>
#include <cctype>
#include "vec2.h"

namespace x39::goingfactory
{
	namespace entity
	{
		class Entity;
	}
	class EntityManager;
	class chunk
	{
	public:
		static const size_t chunk_size = 100;
	private:
		friend class EntityManager;
		int32_t m_index_x;
		int32_t m_index_y;
		std::vector<x39::goingfactory::entity::Entity*> m_entities;
		chunk(int64_t chunk_coordinate) : m_index_x((int32_t)(chunk_coordinate >> 32)), m_index_y((int32_t)chunk_coordinate) {}
		chunk(int x, int y) : m_index_x(x), m_index_y(y) {}
	public:
		int32_t index_x() const { return m_index_x; }
		int32_t index_y() const { return m_index_y; }
		static inline int64_t to_chunk_coordinate(const vec2& pos)
		{
			return (((int64_t)std::floorf(pos.x / chunk_size)) << 32) | (int64_t)std::floorf(pos.y / chunk_size);
		}
		static inline int64_t to_chunk_coordinate(int32_t x, int32_t y)
		{
			return (((int64_t)x) << 32) | (int64_t)y;
		}
		int64_t coordinate() { return to_chunk_coordinate(m_index_x, m_index_y); }
	};
}