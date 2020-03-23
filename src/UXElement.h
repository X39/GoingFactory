#pragma once
#include "GameInstance.h"
#include "EKey.h"
#include "EModifier.h"

namespace x39::goingfactory::ui
{
	class UXElement
	{
	public:
		virtual void initialize(GameInstance& game) = 0;
		virtual void tick(GameInstance& game) = 0;
		virtual void render(GameInstance& game) = 0;

		virtual void key_down(GameInstance& game, io::EKey key, io::EModifier modifier) {}
		virtual void key_up(GameInstance& game, io::EKey key, io::EModifier modifier) {}
	};
}