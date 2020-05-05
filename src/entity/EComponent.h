#pragma once
namespace x39::goingfactory::entity
{
	enum class EComponent
	{
		Position,
		Render,
		Simulate,
		Health,
		Keyboard,
		PlayerInteractible,
		Collidable,


		__MAX_COMPONENT
	};
}