#pragma once
#include "GameInstance.h"
#include "UXElement.h"
#include "Entity.h"
#include "texture.h"

#include <functional>

namespace x39::goingfactory::ux
{
	class UXEntityPlacer : public UXElement
	{
	private:
		std::function<entity::Entity * (vec2 position)> m_entity_factory;
		texture m_tx_preview;
		UXEntityPlacer(GameInstance& game, std::function<entity::Entity * (vec2 position)> factory, texture preview_texture_id);
	public:
		static UXEntityPlacer* create(GameInstance& game, std::function<entity::Entity * (vec2 position)> factory, texture preview_texture_id)
		{ return new UXEntityPlacer(game, factory, preview_texture_id); }
		virtual void render(GameInstance&) override;
		virtual bool mouse_button_down(GameInstance& game, int px, int py, EMouseButton button) override;
	};
}