#pragma once
#include <cmath>

namespace x39::goingfactory
{
	struct vec2
	{
		float x;
		float y;
		vec2() : x(0), y(0) {}
		vec2(int x, float y) : x((float)x), y(y) {}
		vec2(int x, int y) : x((float)x), y((float)y) {}
		vec2(float x, int y) : x(x), y((float)y) {}
		vec2(size_t x, float y) : x((float)x), y(y) {}
		vec2(size_t x, size_t y) : x((float)x), y((float)y) {}
		vec2(float x, size_t y) : x(x), y((float)y) {}
		vec2(float x, float y) : x(x), y(y) {}
		vec2(double x, float y) : x((float)x), y(y) {}
		vec2(double x, double y) : x((float)x), y((float)y) {}
		vec2(float x, double y) : x(x), y((float)y) {}


		float angle_radians() const { return std::atan2(y, x); }
		static vec2 from_angle_radians(float rad) { return { std::cos(rad), std::sin(rad) }; }
		float length_squared() const {
			return std::powf(x, 2) + std::powf(y, 2);
		}
		float length() const {
			return std::sqrtf(std::powf(x, 2) + std::powf(y, 2));
		}
		void normalize() {
			auto len = length();
			if (len == 0)
			{
				x = y = 1;
			}
			else
			{
				x /= len;
				y /= len;
			}
		}


		float crossproduct (const vec2& rvalue) const
		{
			return x * rvalue.y - y * rvalue.x;
		}

		vec2 operator + (float rvalue) const
		{
			return { x + rvalue, y + rvalue };
		}
		vec2 operator + (const vec2& rvalue) const
		{
			return { x + rvalue.x, y + rvalue.y };
		}
		vec2& operator += (const vec2& rvalue)
		{
			x += rvalue.x;
			y += rvalue.y;
			return *this;
		}
		vec2 operator - (const vec2& rvalue) const
		{
			return { x - rvalue.x, y - rvalue.y };
		}
		vec2 operator - (float rvalue) const
		{
			return { x - rvalue, y - rvalue };
		}
		vec2& operator -= (const vec2& rvalue)
		{
			x -= rvalue.x;
			y -= rvalue.y;
			return *this;
		}
		vec2 operator * (const vec2& rvalue) const
		{
			return { x * rvalue.x, y * rvalue.y };
		}
		vec2 operator * (float rvalue) const
		{
			return { x * rvalue, y * rvalue };
		}
		vec2& operator *= (const vec2& rvalue)
		{
			x *= rvalue.x;
			y *= rvalue.y;
			return *this;
		}
		vec2& operator *= (float rvalue)
		{
			x *= rvalue;
			y *= rvalue;
			return *this;
		}
		vec2 operator / (const vec2& rvalue) const
		{
			return { x / rvalue.x, y / rvalue.y };
		}
		vec2 operator / (float rvalue) const
		{
			return { x / rvalue, y / rvalue };
		}
		vec2& operator /= (const vec2& rvalue)
		{
			x /= rvalue.x;
			y /= rvalue.y;
			return *this;
		}
		bool operator < (const vec2& rvalue) const
		{
			return x < rvalue.x && y < rvalue.y;
		}
		bool operator <= (const vec2& rvalue) const
		{
			return x <= rvalue.x && y <= rvalue.y;
		}
		bool operator > (const vec2& rvalue) const
		{
			return x > rvalue.x && y > rvalue.y;
		}
		bool operator >= (const vec2& rvalue) const
		{
			return x >= rvalue.x && y >= rvalue.y;
		}
		bool operator == (const vec2& rvalue) const
		{
			return x == rvalue.x && y == rvalue.y;
		}
		bool operator != (const vec2& rvalue) const
		{
			return x != rvalue.x && y != rvalue.y;
		}
	};
}