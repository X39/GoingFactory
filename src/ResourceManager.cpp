#include "ResourceManager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

x39::goingfactory::ResourceManager::ResourceManager()
{
	m_font = al_load_ttf_font("arial.ttf", 72, 0);
	if (!m_font)
	{
		fprintf(stderr, "Could not load 'arial.ttf'.\n");
		m_font = al_create_builtin_font(); //Load fallback
	}
}

x39::goingfactory::ResourceManager::~ResourceManager()
{
	for (const auto it : m_bitmaps)
	{
		al_destroy_bitmap(it);
	}
	al_destroy_font(m_font);
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
	return index + 1;
}
