#pragma once
#include <cmath>

namespace x39::goingfactory
{
	struct vec2
	{
		float x;
		float y;
		vec2() : x(0), y(0) {}
		vec2(float x, float y) : x(x), y(y) {}

		vec2& operator += (const vec2& rvalue)
		{
			x += rvalue.x;
			y += rvalue.y;
			return *this;
		}
		vec2& operator -= (const vec2& rvalue)
		{
			x -= rvalue.x;
			y -= rvalue.y;
			return *this;
		}
		float angle() const { return std::atan2(y, x); }
	};
	typedef vec2 position;
}