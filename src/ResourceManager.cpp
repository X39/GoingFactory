#include "ResourceManager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

x39::goingfactory::ResourceManager::~ResourceManager()
{
	for (const auto it : m_bitmaps)
	{
		al_destroy_bitmap(it);
	}
}

size_t x39::goingfactory::ResourceManager::load_bitmap(std::string path)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto existing = m_bitmaps_path.find(path.data());
	if (existing != m_bitmaps_path.end())
	{
		return existing->second;
	}
	size_t index = m_bitmaps.size();

	// Prepare path for allegro
	ALLEGRO_BITMAP* bitmap;
	bitmap = al_load_bitmap(path.c_str());
	if (!bitmap)
	{
		throw std::runtime_error("Failed to load resource.");
	}
	m_bitmaps.push_back(bitmap);
	m_bitmaps_path[path] = index;
	return index;
}
