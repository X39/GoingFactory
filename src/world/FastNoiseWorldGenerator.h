#pragma once
#include "WorldGenerator.h"
#include "..\FastNoise.h"
#include <vector>
namespace x39::goingfactory::world
{
	class FastNoiseWorldGenerator : public WorldGenerator
	{
	private:
		FastNoise m_fastnoise;
	public:
		FastNoiseWorldGenerator(Rule default_rule) : WorldGenerator(default_rule) {}
		virtual std::vector<WorldTile> generate_chunk(chunkcrd chunk_x, chunkcrd chunk_y) override;
	};
}