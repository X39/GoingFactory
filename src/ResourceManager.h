#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

struct ALLEGRO_BITMAP;
namespace x39::goingfactory
{
	class ResourceManager
	{
	private:
		std::vector<struct ALLEGRO_BITMAP*> m_bitmaps;
		std::unordered_map<std::string, size_t> m_bitmaps_path;
		std::mutex m_mutex;
	public:
		ResourceManager() {}
		ResourceManager(const ResourceManager&) = delete;
		~ResourceManager();

		size_t load_bitmap(std::string path);
		struct ALLEGRO_BITMAP* get_bitmap(size_t id)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_bitmaps[id];
		}
	};
}