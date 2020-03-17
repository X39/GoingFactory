#pragma once
#include "Entity.h"
#include <cstdint>

namespace x39::goingfactory
{
	class Conveyor : public Entity
	{
		static EntityRegister<Conveyor> entityRegister;
		std::vector<size_t> m_textures;
		size_t m_texture_index;
	public:
		Conveyor() : Entity(), m_textures(), m_texture_index(0) { }

		virtual void render(ResourceManager&) override;
		virtual void initialize(ResourceManager&) override;
		virtual void simulate(EntityManager&) override;
	};
}