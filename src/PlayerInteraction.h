#pragma once
#include <cstdint>

namespace x39::goingfactory::io
{
	enum class EPlayerInteraction : uint16_t
	{
		empty = 0,


		move_left  = 0b0000'0000'0000'0001,
		move_right = 0b0000'0000'0000'0010,
		move_up    = 0b0000'0000'0000'0100,
		move_down  = 0b0000'0000'0000'1000,
				   
		button_a   = 0b0000'0000'0001'0000,
		button_b   = 0b0000'0000'0010'0000,
		button_c   = 0b0000'0000'0100'0000,
		button_d   = 0b0000'0000'1000'0000,
				   
		mod_a      = 0b0000'0001'0000'0000,
		mod_b      = 0b0000'0010'0000'0000,
		mod_c      = 0b0000'0100'0000'0000,
		mod_d      = 0b0000'1000'0000'0000,
				   
		trigger_a  = 0b0001'0000'0000'0000,
		trigger_b  = 0b0010'0000'0000'0000
	};
	inline EPlayerInteraction operator | (EPlayerInteraction left, EPlayerInteraction right)
	{
		return static_cast<EPlayerInteraction>(static_cast<uint16_t>(left) | static_cast<uint16_t>(right));
	}
	inline EPlayerInteraction& operator |= (EPlayerInteraction& left, EPlayerInteraction right)
	{
		left = left | right;
		return left;
	}
	inline EPlayerInteraction operator & (EPlayerInteraction left, EPlayerInteraction right)
	{
		return static_cast<EPlayerInteraction>(static_cast<uint16_t>(left) & static_cast<uint16_t>(right));
	}
	inline EPlayerInteraction& operator &= (EPlayerInteraction& left, EPlayerInteraction right)
	{
		left = left | right;
		return left;
	}
}