#include "World.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Entity.h"
#include "EntityManager.h"
#include "FastNoise.h"

int z = 0;
int size = 32;
x39::goingfactory::FastNoise generator(1203012041254125152);
// Get-/SetFrequency ~= Zoom

x39::goingfactory::World::World() : m_viewport_x(0), m_viewport_y(0), m_viewport_w(0), m_viewport_h(0), m_player()
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
    auto top_left_viewport = playerPositionComponent->position() - center;


    // Draw Level
    {
        al_hold_bitmap_drawing(true);
        for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % size - size; x < top_left_viewport.x + m_viewport_w; x += size)
        {
            for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % size - size; y < top_left_viewport.y + m_viewport_h; y += size)
            {
                vec2 pos = { x, y };
                float f = 0;
                for (int i = 0; i < size; i++)
                {
                    f += generator.GetPerlin(pos.x + i, pos.y + i, z);
                    f += generator.GetPerlin(pos.x + size - i, pos.y + i, z);
                }
                auto color_rgb = f / (size * 2);
                auto color = al_map_rgb((unsigned char)(color_rgb * 255), (unsigned char)(color_rgb * 255), (unsigned char)(color_rgb * 255));
                // al_draw_pixel(pos.x - player_pos_centered.x + m_viewport_x, pos.y - player_pos_centered.y + m_viewport_y, color);
                al_draw_filled_rectangle(
                  pos.x - top_left_viewport.x + m_viewport_x,
                  pos.y - top_left_viewport.y + m_viewport_y,
                  pos.x + size - top_left_viewport.x + m_viewport_x,
                  pos.y + size - top_left_viewport.y + m_viewport_x,
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
        for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % chunk::chunk_size - chunk::chunk_size; x < top_left_viewport.x + m_viewport_w; x += chunk::chunk_size)
        {
            for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % chunk::chunk_size - chunk::chunk_size; y < top_left_viewport.y + m_viewport_h; y += chunk::chunk_size)
            {
                vec2 pos = { x, y };
                auto chunk = game.entity_manager.chunk_at(pos);
                if (chunk)
                {
                    pos -= top_left_viewport;
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
            auto position = positionComponent->position() - top_left_viewport;
            if (position.x > m_viewport_x&& position.x < m_viewport_w + m_viewport_x &&
                position.y > m_viewport_y&& position.y < m_viewport_h + m_viewport_y)
            {
                auto renderComponent = (*it)->get_component<x39::goingfactory::RenderComponent>();
                renderComponent->render(game, top_left_viewport);
            }
        }
        else
        {
            auto renderComponent = (*it)->get_component<x39::goingfactory::RenderComponent>();
            renderComponent->render(game, top_left_viewport);
        }
    }
}

void x39::goingfactory::World::keydown(io::EKey key)
{
    switch (key)
    {
    case io::EKey::PAD_PLUS:
        generator.SetFrequency(generator.GetFrequency() - 0.001);
        break;
    case io::EKey::PAD_MINUS:
        generator.SetFrequency(generator.GetFrequency() + 0.001);
        break;
    case io::EKey::PAD_1:
        generator.SetInterp(FastNoise::Interp::Hermite);
        break;
    case io::EKey::PAD_2:
        generator.SetInterp(FastNoise::Interp::Linear);
        break;
    case io::EKey::PAD_3:
        generator.SetInterp(FastNoise::Interp::Quintic);
        break;
    case io::EKey::PAD_ASTERISK:
        z++;
        break;
    case io::EKey::PAD_SLASH:
        z--;
        break;
    case io::EKey::PAD_DELETE:
        size++;
        break;
    case io::EKey::PAD_0:
        if (size - 1 <= 0)
        {
            break;
        }
        size--;
        break;
    case io::EKey::PAD_7:
    {
        int left, right;
        generator.GetCellularDistance2Indices(left, right);
        left += 0.1;
        if (left >= right || left > 4)
        {
            break;
        }
        generator.SetCellularDistance2Indices(left, right);
    }
    break;
    case io::EKey::PAD_4:
    {
        int left, right;
        generator.GetCellularDistance2Indices(left, right);
        left -= 0.1;
        if (left < 0)
        {
            break;
        }
        generator.SetCellularDistance2Indices(left, right);
    }
    break;
    case io::EKey::PAD_8:
    {
        int left, right;
        generator.GetCellularDistance2Indices(left, right);
        right += 0.1;
        if (left >= right)
        {
            break;
        }
        generator.SetCellularDistance2Indices(left, right);
    }
    break;
    case io::EKey::PAD_5:
    {
        int left, right;
        generator.GetCellularDistance2Indices(left, right);
        right -= 0.1;
        if (left >= right)
        {
            break;
        }
        generator.SetCellularDistance2Indices(left, right);
    }
    break;
    case io::EKey::PAD_9:
        generator.SetFractalGain(generator.GetFractalGain() + 0.001);
        break;
    case io::EKey::PAD_6:
        generator.SetFractalGain(generator.GetFractalGain() - 0.001);
        break;
    }
}
