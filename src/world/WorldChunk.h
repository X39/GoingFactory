#pragma once

#include <cinttypes>

namespace x39::goingfactory::world
{
	// World Chunk Coordinate type
	typedef int32_t chunkcrd;
	// Chunk Local Coordinate type
	typedef int32_t chunklocal;
	// Tile Global Coordinate type
	typedef int32_t tilecrd;


	class WorldManager;
	class WorldChunk
	{
	public:
		// Max Value for X and Y
		static const int SIZE = 16;
	private:
		bool m_is_original;
		chunkcrd m_x;
		chunkcrd m_y;
		WorldChunk(chunkcrd x, chunkcrd y) : m_is_original(true), m_x(x), m_y(y) {}
		friend WorldManager;
	public:
		chunkcrd x() { return m_x; }
		chunkcrd y() { return m_y; }

		// Returns true if since the last save (or creation) nothing was altered.
		bool is_original() { return m_is_original; }
	};
}