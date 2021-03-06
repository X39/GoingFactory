#pragma once

#include <cmath>
#include <array>
#include <vector>
#include <cctype>
#include "../vec2.h"

namespace x39::goingfactory::entity
{
	class EntityManager;
	class Entity;
	class EntityChunk
	{
	public:
		static const int chunk_size = 160;
	private:
		friend class EntityManager;
		int32_t m_index_x;
		int32_t m_index_y;
		std::vector<x39::goingfactory::entity::Entity*> m_entities;
		EntityChunk(int64_t chunk_coordinate) : m_index_x((int32_t)(chunk_coordinate >> 32)), m_index_y((int32_t)chunk_coordinate) {}
		EntityChunk(int x, int y) : m_index_x(x), m_index_y(y) {}
	public:
		int32_t index_x() const { return m_index_x; }
		int32_t index_y() const { return m_index_y; }
		static inline int64_t to_chunk_coordinate(const vec2& pos)
		{
			const uint32_t max = UINT32_MAX;
			const uint32_t max_halfed = max / 2;

			float x_floored = std::floorf(pos.x / chunk_size);
			uint32_t x_casted = max_halfed + (int32_t)x_floored;

			float y_floored = std::floorf(pos.y / chunk_size);
			uint32_t y_casted = max_halfed + (int32_t)y_floored;

			int64_t result = (((int64_t)x_casted) << 32) | y_casted;
			return result;
		}
		static inline int32_t to_chunk_coordinate_x(const vec2& pos)
		{
			const uint32_t max = UINT32_MAX;
			const uint32_t max_halfed = max / 2;

			float x_floored = std::floorf(pos.x / chunk_size);
			return max_halfed + (int32_t)x_floored;
		}
		static inline int32_t to_chunk_coordinate_y(const vec2& pos)
		{
			const uint32_t max = UINT32_MAX;
			const uint32_t max_halfed = max / 2;

			float y_floored = std::floorf(pos.y / chunk_size);
			return max_halfed + (int32_t)y_floored;
		}
		static inline int64_t to_chunk_coordinate(float x, float y)
		{
			const uint32_t max = UINT32_MAX;
			const uint32_t max_halfed = max / 2;

			float x_floored = std::floorf(x / chunk_size);
			uint32_t x_casted = max_halfed + (int32_t)x_floored;

			float y_floored = std::floorf(y / chunk_size);
			uint32_t y_casted = max_halfed + (int32_t)y_floored;

			int64_t result = (((int64_t)x_casted) << 32) | y_casted;
			return result;
		}
		static inline int64_t to_chunk_coordinate(int32_t x, int32_t y)
		{
			const uint32_t max = UINT32_MAX;
			const uint32_t max_halfed = max / 2;

			int32_t x_floored = x / chunk_size;
			uint32_t x_casted = max_halfed + (int32_t)x_floored;

			int32_t y_floored = y / chunk_size;
			uint32_t y_casted = max_halfed + (int32_t)y_floored;

			int64_t result = (((int64_t)x_casted) << 32) | y_casted;
			return result;
		}
		static inline int64_t concat_chunk_coordinate(int32_t x, int32_t y)
		{
			int64_t result = (((int64_t)x) << 32) | ((uint32_t)y);
			return result;
		}
		static inline int64_t as_chunk_coordinate(int32_t x, int32_t y)
		{
			const uint32_t max = UINT32_MAX;
			const uint32_t max_halfed = max / 2;
			uint32_t x_adjusted = max_halfed + x;
			uint32_t y_adjusted = max_halfed + y;
			int64_t result = (((int64_t)x_adjusted) << 32) | y_adjusted;
			return result;
		}
		int64_t coordinate() { return concat_chunk_coordinate(m_index_x, m_index_y); }
	};
}