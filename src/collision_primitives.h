#pragma once
#include "vec2.h"

namespace x39::goingfactory::primitives
{
	class collision
	{
	protected:
		vec2 m_pos;
	public:
		enum type
		{
			// new types have to be implemented in EACH other type!
			rectangle
		};
		collision() : m_pos() {}
		virtual bool collides_with(const collision& other) const = 0;
		virtual type get_type() const = 0;
		void position(vec2 pos) { m_pos = pos; }
		vec2 position() { return m_pos; }
	};
	class collisionrect : public collision
	{
	private:
		int m_x_offset;
		int m_y_offset;
		int m_w;
		int m_h;
	public:
		collisionrect() : m_x_offset(0), m_y_offset(0), m_w(0), m_h(0) {}
		vec2 position_off() const { return { m_pos.x - m_x_offset, m_pos.y - m_y_offset }; }
		void set_boundaries(int x_off, int y_off, int w, int h) {
			m_x_offset = x_off;
			m_y_offset = y_off;
			m_w = w;
			m_h = h;
		}
		int x_offset() const { return m_x_offset; }
		int y_offset() const { return m_y_offset; }
		int w() const { return m_w; }
		int h() const { return m_h; }
		virtual type get_type() const override { return rectangle; }
		virtual bool collides_with(const collision& other) const override
		{
			vec2 self_pos = position_off();
			switch (other.get_type())
			{
			case rectangle:
			{
				auto other_rect = static_cast<const collisionrect&>(other);
				auto other_pos = other_rect.position_off();
				// Right < OtherLeft
				if (self_pos.x + m_w < other_pos.x) { return false; }
				// Left > OtherRight
				if (self_pos.x > other_pos.x + other_rect.m_w) { return false; }
				// Bot < OtherTop
				if (self_pos.y + m_h < other_pos.y) { return false; }
				// Top > OtherBo
				if (self_pos.y > other_pos.y + other_rect.m_h) { return false; }
				return true;
			} break;
			default:
				return false;
			}
		}
	};
}