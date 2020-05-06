#include "FastNoiseWorldGenerator.h"

#include <algorithm>


using namespace x39::goingfactory::world;

std::vector<WorldTile> FastNoiseWorldGenerator::generate_chunk(chunkcrd chunk_x, chunkcrd chunk_y)
{
	// Range: [-1, 1]
	auto value = m_fastnoise.GetPerlin(chunk_x, chunk_y, 0);
	std::vector<WorldTile> tiles;
	std::vector<unsigned short> collisions;
	for (auto rule : m_rules)
	{
		if (rule.colliding_layer != 0)
		{
			auto res = std::find(collisions.begin(), collisions.end(), rule.colliding_layer);
			if (res == collisions.end())
			{
				continue;
			}
		}
		auto opt = rule.check(value);
		if (opt.has_value())
		{
			auto tile = opt.value();
			
			tiles.push_back(tile);
			if (rule.colliding_layer != 0)
			{
				collisions.push_back(rule.colliding_layer);
			}
		}
	}

	return tiles;
}
