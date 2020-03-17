#pragma once

namespace x39::goingfactory
{
	struct position
	{
		float x;
		float y;
		position() : x(0), y(0) {}
		position(float x, float y) : x(x), y(y) {}
	};
}