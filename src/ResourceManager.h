#pragma once
#include "texture.h"


#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

struct ALLEGRO_BITMAP;
struct ALLEGRO_FONT;
namespace x39::goingfactory
{
	class ResourceManager
	{
	private:
		std::vector<texture> m_bitmaps;
		std::unordered_map<std::string, texture> m_bitmaps_path;
		std::mutex m_mutex;
		ALLEGRO_FONT* m_font;
	public:
		ResourceManager();
		ResourceManager(const ResourceManager&) = delete;
		~ResourceManager();

		ALLEGRO_FONT* font() { return m_font; }

		texture from_index(size_t index) { return m_bitmaps[index]; }
		texture load_bitmap(std::string path);
	};
}