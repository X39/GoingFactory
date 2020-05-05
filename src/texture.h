#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

struct ALLEGRO_BITMAP;
namespace x39::goingfactory
{
	class ResourceManager;
	class texture
	{
	private:
		ALLEGRO_BITMAP* m_bitmap;
		int m_width;
		int m_height;
		size_t m_index;

		friend ResourceManager;
	public:
		texture() : m_bitmap(0), m_width(-1), m_height(-1), m_index(0) {}
		int width() { return m_width; }
		int height() { return m_height; }
		size_t index() { return m_index; }
		ALLEGRO_BITMAP* bitmap() { return m_bitmap; }
		bool is_valid() { return m_bitmap != 0; }
	};
}