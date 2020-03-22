#include "World.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Entity.h"
#include "EntityManager.h"
#include "FastNoise.h"

x39::goingfactory::World::World() : m_viewport_x(0), m_viewport_y(0), m_viewport_w(0), m_viewport_h(0), m_level_w(0), m_level_h(0), m_player()
{
    // m_bitmap = al_create_bitmap(256, 0);
    // al_set_target_bitmap(m_bitmap);
    // for (int i = 0; i < 256; i++)
    // {
    // 	al_put_pixel(i, 0, al_map_rgb(i, i, i));
    // }
}

void x39::goingfactory::World::render(GameInstance& game)
{
    if (!m_player || !m_player->is_type(EComponent::Position))
    {
        return;
    }
    auto playerPositionComponent = m_player->get_component<PositionComponent>();
    vec2 center = { m_viewport_w / 2 + m_viewport_x, m_viewport_h / 2 + m_viewport_y };
    auto player_pos_centered = playerPositionComponent->position() - center;


    // Draw Level
    {
        al_hold_bitmap_drawing(true);
        FastNoise generator(1203012041254125152);
        const int size = 16;
        for (int32_t x = ((int32_t)player_pos_centered.x) - ((int32_t)player_pos_centered.x) % size; x < player_pos_centered.x + m_viewport_w; x += size)
        {
            for (int32_t y = ((int32_t)player_pos_centered.y) - ((int32_t)player_pos_centered.y) % size; y < player_pos_centered.y + m_viewport_h; y += size)
            {
                vec2 pos = { x, y };
                float f = 0;
                for (int i = 0; i < size; i++)
                {
                    f += generator.GetPerlin(pos.x + i, pos.y + i);
                    f += generator.GetPerlin(pos.x + size - i, pos.y + i);
                }
                auto color_rgb = f / (size * 2);
                auto color = al_map_rgb((unsigned char)(color_rgb * 255), (unsigned char)(color_rgb * 255), (unsigned char)(color_rgb * 255));
                // al_draw_pixel(pos.x - player_pos_centered.x + m_viewport_x, pos.y - player_pos_centered.y + m_viewport_y, color);
                al_draw_filled_rectangle(
                  pos.x - player_pos_centered.x + m_viewport_x,
                  pos.y - player_pos_centered.y + m_viewport_y,
                  pos.x + size - player_pos_centered.x + m_viewport_x,
                  pos.y + size - player_pos_centered.y + m_viewport_x,
                  color);
                // al_draw_bitmap_region(
                //   m_bitmap,
                //   (unsigned char)(color_rgb * 255), 0, 1, 1,
                //   pos.x - player_pos_centered.x + m_viewport_x,
                //   pos.y - player_pos_centered.y + m_viewport_y,
                //   0);
            }
        }
        al_hold_bitmap_drawing(false);
    }

    // Draw Chunks
    {
        auto color = al_map_rgb(0, 0, 32);
        for (int32_t x = 0; x < (m_level_w / chunk::chunk_size + 1); x++)
        {
            for (int32_t y = 0; y < (m_level_h / chunk::chunk_size + 1); y++)
            {
                auto chunk = game.entity_manager.chunk_at(x, y);
                vec2 pos = { x * chunk::chunk_size, y * chunk::chunk_size };
                pos -= player_pos_centered;
                if (chunk &&
                    pos.x + chunk::chunk_size > m_viewport_x&& pos.x < m_viewport_w + m_viewport_x &&
                    pos.y + chunk::chunk_size > m_viewport_y&& pos.y < m_viewport_h + m_viewport_y)
                {
                    al_draw_line(
                        pos.x,
                        pos.y,
                        pos.x + chunk::chunk_size,
                        pos.y,
                        color, 1);
                    al_draw_line(
                        pos.x,
                        pos.y,
                        pos.x,
                        pos.y + chunk::chunk_size,
                        color, 1);
                    al_draw_line(
                        pos.x + chunk::chunk_size,
                        pos.y,
                        pos.x + chunk::chunk_size,
                        pos.y + chunk::chunk_size,
                        color, 1);
                    al_draw_line(
                        pos.x,
                        pos.y + chunk::chunk_size,
                        pos.x + chunk::chunk_size,
                        pos.y + chunk::chunk_size,
                        color, 1);
                    al_draw_line(
                        pos.x,
                        pos.y,
                        pos.x + chunk::chunk_size,
                        pos.y + chunk::chunk_size,
                        color, 1);
                    al_draw_line(
                        pos.x + chunk::chunk_size,
                        pos.y,
                        pos.x,
                        pos.y + chunk::chunk_size,
                        color, 1);
                }
            }
        }
    }

    // Draw Level Boundaries
    {
        auto color = al_map_rgb(127, 0, 0);
        al_draw_line(
            0 - player_pos_centered.x,
            0 - player_pos_centered.y,
            m_level_w - player_pos_centered.x,
            0 - player_pos_centered.y,
            color, 1);
        al_draw_line(
            0 - player_pos_centered.x,
            0 - player_pos_centered.y,
            0 - player_pos_centered.x,
            m_level_h - player_pos_centered.y,
            color, 1);
        al_draw_line(
            m_level_w - player_pos_centered.x,
            m_level_h - player_pos_centered.y,
            m_level_w - player_pos_centered.x,
            0 - player_pos_centered.y,
            color, 1);
        al_draw_line(
            m_level_w - player_pos_centered.x,
            m_level_h - player_pos_centered.y,
            0 - player_pos_centered.x,
            m_level_h - player_pos_centered.y,
            color, 1);
    }

    // Draw Viewport
    {
        auto color = al_map_rgb(0, 127, 0);
        al_draw_line(
            m_viewport_x,
            m_viewport_y,
            m_viewport_x + m_viewport_w,
            m_viewport_y,
            color, 1);
        al_draw_line(
            m_viewport_x,
            m_viewport_y,
            m_viewport_x,
            m_viewport_y + m_viewport_h,
            color, 1);
        al_draw_line(
            m_viewport_x + m_viewport_w,
            m_viewport_y + m_viewport_h,
            m_viewport_x + m_viewport_w,
            m_viewport_y,
            color, 1);
        al_draw_line(
            m_viewport_x + m_viewport_w,
            m_viewport_y + m_viewport_h,
            m_viewport_x,
            m_viewport_y + m_viewport_h,
            color, 1);
        color = al_map_rgb(0, 0, 0);
    }

    for (auto it = game.entity_manager.begin(EComponent::Render); it != game.entity_manager.end(EComponent::Render); it++)
    {
        if ((*it)->is_type(EComponent::Position))
        {
            auto positionComponent = (*it)->get_component<x39::goingfactory::PositionComponent>();
            auto position = positionComponent->position() - player_pos_centered;
            if (position.x > m_viewport_x&& position.x < m_viewport_w + m_viewport_x &&
                position.y > m_viewport_y&& position.y < m_viewport_h + m_viewport_y)
            {
                auto renderComponent = (*it)->get_component<x39::goingfactory::RenderComponent>();
                renderComponent->render(game, player_pos_centered);
            }
        }
        else
        {
            auto renderComponent = (*it)->get_component<x39::goingfactory::RenderComponent>();
            renderComponent->render(game, player_pos_centered);
        }
    }
}
