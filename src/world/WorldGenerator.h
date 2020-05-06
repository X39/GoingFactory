#pragma once
#include "../texture.h"
#include "WorldChunk.h"
#include "WorldTile.h"

#include <vector>
#include <functional>
#include <optional>
namespace x39::goingfactory::world
{
	class WorldManager;
	class WorldGenerator
	{
	public:
		struct Rule
		{
			// The layer this rule will collide with others.
			// If 0 is passed, collision rules are ignored.
			unsigned short colliding_layer;

			// The code called to generate
			std::function<std::optional<WorldTile>(float coef)> check;
		};
	protected:
		friend WorldManager;
		std::vector<Rule> m_rules;
		Rule m_default_rule;
	public:
		WorldGenerator(Rule default_rule) : m_default_rule(default_rule) {}
		void push_back(Rule r) { m_rules.push_back(r); }

		virtual std::vector<WorldTile> generate_chunk(chunkcrd chunk_x, chunkcrd chunk_y) = 0;
	};
}