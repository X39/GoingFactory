#pragma once
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "UXElement.h"
#include <vector>

namespace x39::goingfactory::ux
{
	class UXPanel : public UXElement
	{
	public:
		UXPanel(GameInstance& game, int x, int y, int w, int h) :
			UXElement(game, x, y, w, h)
		{ }
		virtual void render(GameInstance&) override;

		virtual bool mouse_button_down(GameInstance& game, int px, int py, EMouseButton button) override
		{
			GameInstance::Log("DOWN");
			return true;
		}
		virtual void mouse_enter(GameInstance& game) override
		{
			GameInstance::Log("ENTER");
		}
		virtual void mouse_leave(GameInstance& game) override
		{
			GameInstance::Log("LEAVE");
		}
	};
}