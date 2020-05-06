#pragma once
#include "WorldGenerator.h"
#include "..\FastNoise.h"
#include <vector>
namespace x39::goingfactory::world
{
	struct WorldTile
	{
		enum tilemode
		{
			single,
			surrounding,
			sheet
		};
		bool is_passable;
		texture texture;
		unsigned short sheet_x;
		unsigned short sheet_y;
		unsigned short sheet_scale;

		unsigned short layer_draw;
	};
}