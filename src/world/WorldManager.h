#pragma once

#include <cinttypes>
#include <filesystem>
#include <mutex>

namespace x39::goingfactory::world
{
	class WorldChunk;
	class WorldManager
	{
	private:
		std::mutex m_chunk_access;
	public:
		WorldManager(std::filesystem::path file_path);


		// Loads all changes from disk.
		void Load();
		// Saves all pending changes to disk.
		void Save();
	};
}