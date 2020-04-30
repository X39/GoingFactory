#include "UXPanel.h"
#include "UXElement.h"
#include "ResourceManager.h"
#include "GameInstance.h"
#include "UXHandler.h"

void x39::goingfactory::ux::UXPanel::render(GameInstance& game)
{
	game.uxhandler.draw_panel(game, x, y, w, h);
}

