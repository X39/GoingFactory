#include "World.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Entity.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "FastNoise.h"
#include <fstream>

bool render_grayscale = false;
bool render_chunks = false;
bool render_background = true;
bool render_collisions = false;
bool render_position_component = false;
x39::goingfactory::FastNoise generator(12030220512152);
size_t grass1_texture_id = 0;
size_t grass2_1_texture_id = 0;
size_t grass2_2_texture_id = 0;
size_t grass2_3_texture_id = 0;
size_t tree_texture_id = 0;
size_t dirt_texture_id = 0;
size_t stone_texture_id = 0;
// Get-/SetFrequency ~= Zoom

x39::goingfactory::vec2 store_a_player_vel;
x39::goingfactory::vec2 store_a_player_pos;

class RandomFromPerlin
{
private:
    std::vector<float> m_base;
    size_t m_index;
public:
    RandomFromPerlin(std::vector<float> base) : m_base(base) {}
    float next_float()
    {
        float f = m_base[m_index];
        if (m_index == m_base.size())
        {
            m_index = 0;
        }
        return f;
    }
    int next_int() { float f = next_float(); return *reinterpret_cast<int*>(&f); }
};

x39::goingfactory::World::Tile x39::goingfactory::World::get_tile(int x, int y)
{
    x = x - x % tile_size;
    y = y - y % tile_size;
    float f = 0;
    std::vector<float> floats;
    for (int i = 0; i < tile_size; i++)
    {
        floats.push_back(std::fabsf(generator.GetPerlin(x + i, y + i)));
        floats.push_back(std::fabsf(generator.GetPerlin(x + tile_size - i, y + i)));
    }
    RandomFromPerlin random(floats);
    
    x39::goingfactory::World::Tile tile;
    tile.has_tree = false;
    tile.is_passable = true;
    tile.tile_texture = dirt_texture_id;
    if (random.next_float() > 0.5)
    {
        auto t = (int)(al_get_time() * 3);
        switch ((t + random.next_int()) % 3)
        {
        case 0:
            tile.tile_texture = grass2_1_texture_id;
            break;
        case 1:
            tile.tile_texture = grass2_2_texture_id;
            break;
        default:
            tile.tile_texture = grass2_3_texture_id;
            break;
        }
        if (random.next_int() % 5 == 0)
        {
            tile.is_passable = false;
            tile.has_tree = true;
        }
    }
    else if (random.next_float() > 0.3)
    {
        tile.tile_texture = grass1_texture_id;
    }
    else if (random.next_float() < 0.1)
    {
        tile.tile_texture = stone_texture_id;
        tile.is_passable = false;
    }

    return tile;
}
std::vector<std::array<x39::goingfactory::vec2, 4>> x39::goingfactory::World::get_chunk_world_collision(int x, int y)
{
    std::vector<std::array<x39::goingfactory::vec2, 4>> vec;
    for (int i = 0; i <= chunk::chunk_size; i += x39::goingfactory::World::tile_size)
    {
        for (int j = 0; j <= chunk::chunk_size; j += x39::goingfactory::World::tile_size)
        {
            auto tile = get_tile(x + i, y + j);
            if (!tile.is_passable)
            {
                vec.push_back(std::array<x39::goingfactory::vec2, 4>
                {
                    vec2
                    {
                        (float)x + (i + x39::goingfactory::World::tile_size / 2) - x39::goingfactory::World::tile_size / 2,
                        (float)y + (j + x39::goingfactory::World::tile_size / 2) - x39::goingfactory::World::tile_size / 2
                    },
                    vec2
                    {
                        (float)x + (i + x39::goingfactory::World::tile_size / 2) + x39::goingfactory::World::tile_size / 2,
                        (float)y + (j + x39::goingfactory::World::tile_size / 2) - x39::goingfactory::World::tile_size / 2
                    },
                    vec2
                    {
                        (float)x + (i + x39::goingfactory::World::tile_size / 2) + x39::goingfactory::World::tile_size / 2,
                        (float)y + (j + x39::goingfactory::World::tile_size / 2) + x39::goingfactory::World::tile_size / 2
                    },
                    vec2{
                        (float)x + (i + x39::goingfactory::World::tile_size / 2) - x39::goingfactory::World::tile_size / 2,
                        (float)y + (j + x39::goingfactory::World::tile_size / 2) + x39::goingfactory::World::tile_size / 2
                    }
                });
            }
        }
    }
    return vec;
}

void x39::goingfactory::World::draw_level(GameInstance& game)
{
    if (!m_player || !m_player->is_type(EComponent::Position))
    {
        return;
    }
    auto playerPositionComponent = m_player->get_component<PositionComponent>();
    vec2 center = { m_viewport_w / 2 + m_viewport_x, m_viewport_h / 2 + m_viewport_y };
    vec2 top_left_viewport =  playerPositionComponent->position() - center;

    // Draw background
    al_hold_bitmap_drawing(true);
    for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % tile_size - tile_size; x < top_left_viewport.x + m_viewport_w; x += tile_size)
    {
        for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % tile_size - tile_size; y < top_left_viewport.y + m_viewport_h; y += tile_size)
        {
            vec2 pos = { x + (float)m_viewport_x, y + (float)m_viewport_y };
            auto tile = get_tile(pos);
            pos -= top_left_viewport;
            al_draw_bitmap(game.resource_manager.get_bitmap(tile.tile_texture), pos.x, pos.y, 0);
        }
    }
    al_hold_bitmap_drawing(false);

    // Draw specifics
    al_hold_bitmap_drawing(true);
    for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % tile_size - tile_size; x < top_left_viewport.x + m_viewport_w; x += tile_size)
    {
        for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % tile_size - tile_size; y < top_left_viewport.y + m_viewport_h; y += tile_size)
        {
            vec2 pos = { x + (float)m_viewport_x, y + (float)m_viewport_y };
            auto tile = get_tile(pos);
            if (tile.has_tree)
            {
                pos -= top_left_viewport;
                al_draw_bitmap(game.resource_manager.get_bitmap(tree_texture_id), pos.x - 8, pos.y - 16, 0);
            }
        }
    }
    al_hold_bitmap_drawing(false);
}

x39::goingfactory::World::World() : m_viewport_x(0), m_viewport_y(0), m_viewport_w(0), m_viewport_h(0), m_player()
{
    std::ifstream f("world.settings");
    if (f.good())
    {
        float frequency;
        f >>
            frequency >>
            store_a_player_vel.x >> store_a_player_vel.y >>
            store_a_player_pos.x >> store_a_player_pos.y >>
            render_grayscale >>
            render_chunks >>
            render_background >>
            render_collisions >>
            render_position_component;
        generator.SetFrequency(frequency);
    }
    else
    {
        generator.SetFrequency(generator.GetFrequency() - 0.003);
    }
}
x39::goingfactory::World::~World()
{
    std::ofstream f("world.settings");
    if (f.good())
    {
        f <<
            generator.GetFrequency() << std::endl <<
            store_a_player_vel.x << std::endl << store_a_player_vel.y << std::endl <<
            store_a_player_pos.x << std::endl << store_a_player_pos.y << std::endl <<
            render_grayscale << std::endl <<
            render_chunks << std::endl <<
            render_background << std::endl <<
            render_collisions << std::endl <<
            render_position_component << std::endl;
    }
}

void x39::goingfactory::World::render(GameInstance& game)
{
    if (grass1_texture_id == 0) { grass1_texture_id = game.resource_manager.load_bitmap("Textures/Grass.png"); }
    if (grass2_1_texture_id == 0) { grass2_1_texture_id = game.resource_manager.load_bitmap("Textures/Grass2_1.png"); }
    if (grass2_2_texture_id == 0) { grass2_2_texture_id = game.resource_manager.load_bitmap("Textures/Grass2_2.png"); }
    if (grass2_3_texture_id == 0) { grass2_3_texture_id = game.resource_manager.load_bitmap("Textures/Grass2_3.png"); }
    if (dirt_texture_id == 0) { dirt_texture_id = game.resource_manager.load_bitmap("Textures/Dirt.png"); }
    if (stone_texture_id == 0) { stone_texture_id = game.resource_manager.load_bitmap("Textures/Stone.png"); }
    if (tree_texture_id == 0) { tree_texture_id = game.resource_manager.load_bitmap("Textures/Tree.png"); }

    if (!m_player || !m_player->is_type(EComponent::Position))
    {
        return;
    }
    auto playerPositionComponent = m_player->get_component<PositionComponent>();
    vec2 center = { m_viewport_w / 2 + m_viewport_x, m_viewport_h / 2 + m_viewport_y };
    vec2 top_left_viewport =  playerPositionComponent->position() - center;
    auto yellow = al_map_rgb(255, 255, 0);
    auto green = al_map_rgb(0, 255, 0);


    // Draw Level
    {
        if (render_grayscale && render_background)
        {
            for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % tile_size - tile_size; x < top_left_viewport.x + m_viewport_w; x += tile_size)
            {
                for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % tile_size - tile_size; y < top_left_viewport.y + m_viewport_h; y += tile_size)
                {
                    vec2 pos = { x + (float)m_viewport_x, y + (float)m_viewport_y };
                    float f = 0;
                    for (int i = 0; i < tile_size; i++)
                    {
                        f += std::fabsf(generator.GetPerlin(pos.x + i, pos.y + i));
                        f += std::fabsf(generator.GetPerlin(pos.x + tile_size - i, pos.y + i));
                    }
                    f = f / (tile_size * 2);
                    auto color = al_map_rgba((unsigned char)(f * 255), (unsigned char)(f * 255), (unsigned char)(f * 255), 255);
                    pos -= top_left_viewport;
                    // al_draw_pixel(pos.x - player_pos_centered.x + m_viewport_x, pos.y - player_pos_centered.y + m_viewport_y, color);
                    al_draw_filled_rectangle(
                        pos.x,
                        pos.y,
                        pos.x + tile_size,
                        pos.y + tile_size,
                        color);
                    // al_draw_bitmap_region(
                    //   m_bitmap,
                    //   (unsigned char)(color_rgb * 255), 0, 1, 1,
                    //   pos.x - player_pos_centered.x + m_viewport_x,
                    //   pos.y - player_pos_centered.y + m_viewport_y,
                    //   0);
                }
            }
        }
        else if (render_background)
        {
            draw_level(game);
        }
        if (render_collisions)
        {
            for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % chunk::chunk_size - chunk::chunk_size; x < top_left_viewport.x + m_viewport_w; x += chunk::chunk_size)
            {
                for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % chunk::chunk_size - chunk::chunk_size; y < top_left_viewport.y + m_viewport_h; y += chunk::chunk_size)
                {
                    auto points_collection = get_chunk_world_collision(x, y);
                    for (auto& points : points_collection)
                    {
                        for (size_t i = 0; i < points.size(); i++)
                        {
                            vec2 p1 = points[i];
                            vec2 p2 = points[i + 1 >= points.size() ? 0 : i + 1];
                            al_draw_line(
                                p1.x - top_left_viewport.x, p1.y - top_left_viewport.y,
                                p2.x - top_left_viewport.x, p2.y - top_left_viewport.y,
                                yellow,
                                1);
                        }
                    }
                }
            }
        }
    }

    // Draw Chunks
    if (render_chunks)
    {
        auto playerpos = playerPositionComponent->position();
        auto playerchunk = chunk::to_chunk_coordinate(playerpos);
        auto playerchunk_x = chunk::to_chunk_coordinate_x(playerpos);
        auto playerchunk_y = chunk::to_chunk_coordinate_y(playerpos);
        auto color_chunk = al_map_rgb(0, 0, 255);
        auto color_active = color_chunk;
        auto color_player = al_map_rgb(255, 255, 255);
        auto color_collission = al_map_rgb(255, 0, 255);
        for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % chunk::chunk_size - chunk::chunk_size; x < top_left_viewport.x + m_viewport_w; x += chunk::chunk_size)
        {
            for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % chunk::chunk_size - chunk::chunk_size; y < top_left_viewport.y + m_viewport_h; y += chunk::chunk_size)
            {
                vec2 pos = { x, y };
                auto chunk = game.entity_manager.chunk_at(pos);
                if (chunk)
                {
                    if (chunk->coordinate() == playerchunk)
                    {
                        color_active = color_player;
                    }
                    else
                    {
                        color_active = color_chunk;
                        for (int x2 = -1; x2 <= 1; x2++)
                        {
                            for (int y2 = -1; y2 <= 1; y2++)
                            {
                                if (chunk->coordinate() == chunk::concat_chunk_coordinate(playerchunk_x + x2, playerchunk_y + y2))
                                {
                                    color_active = color_collission;
                                    break;
                                }
                            }
                        }
                    }
                    pos -= top_left_viewport;
                    al_draw_line(
                        pos.x,
                        pos.y,
                        pos.x + chunk::chunk_size,
                        pos.y,
                        color_active, 1);
                    al_draw_line(
                        pos.x,
                        pos.y,
                        pos.x,
                        pos.y + chunk::chunk_size,
                        color_active, 1);
                    al_draw_line(
                        pos.x + chunk::chunk_size,
                        pos.y,
                        pos.x + chunk::chunk_size,
                        pos.y + chunk::chunk_size,
                        color_active, 1);
                    al_draw_line(
                        pos.x,
                        pos.y + chunk::chunk_size,
                        pos.x + chunk::chunk_size,
                        pos.y + chunk::chunk_size,
                        color_active, 1);
                    al_draw_line(
                        pos.x,
                        pos.y,
                        pos.x + chunk::chunk_size,
                        pos.y + chunk::chunk_size,
                        color_active, 1);
                    al_draw_line(
                        pos.x + chunk::chunk_size,
                        pos.y,
                        pos.x,
                        pos.y + chunk::chunk_size,
                        color_active, 1);
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
        if (render_collisions && (*it)->is_type(EComponent::Collidable))
        {
            auto collidableComponent = (*it)->get_component<CollidableComponent>();
            auto points = collidableComponent->polygon_points();
            for (size_t i = 0; i < points.size(); i++)
            {
                vec2 p1 = points[i];
                vec2 p2 = points[i + 1 >= points.size() ? 0 : i + 1];
                al_draw_line(
                    p1.x - top_left_viewport.x, p1.y - top_left_viewport.y,
                    p2.x - top_left_viewport.x, p2.y - top_left_viewport.y,
                    yellow,
                    1);
            }
        }
        if (render_position_component)
        {
            auto positionComponent = (*it)->get_component<PositionComponent>();
            if (positionComponent)
            {
                al_draw_line(
                    positionComponent->position().x - top_left_viewport.x, positionComponent->position().y - top_left_viewport.y,
                    positionComponent->position().x - top_left_viewport.x + positionComponent->velocity().x, positionComponent->position().y - top_left_viewport.y + positionComponent->velocity().y,
                    green,
                    1);

                al_draw_line(
                    positionComponent->position().x - top_left_viewport.x - 2, positionComponent->position().y - top_left_viewport.y - 2,
                    positionComponent->position().x - top_left_viewport.x + 2, positionComponent->position().y - top_left_viewport.y + 2,
                    green,
                    1);
                al_draw_line(
                    positionComponent->position().x - top_left_viewport.x + 2, positionComponent->position().y - top_left_viewport.y - 2,
                    positionComponent->position().x - top_left_viewport.x - 2, positionComponent->position().y - top_left_viewport.y + 2,
                    green,
                    1);
            }
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
    case io::EKey::PAD_4:
        render_collisions = !render_collisions;
        break;
    case io::EKey::PAD_5:
        render_position_component = !render_position_component;
        break;
    case io::EKey::PAD_7:
        store_a_player_pos = (m_player)->get_component<PositionComponent>()->position();
        store_a_player_vel = (m_player)->get_component<PositionComponent>()->velocity();
        break;
    case io::EKey::PAD_8:
        (m_player)->get_component<PositionComponent>()->position(store_a_player_pos);
        (m_player)->get_component<PositionComponent>()->velocity(store_a_player_vel);
        break;
    case io::EKey::PAD_ASTERISK:
        render_chunks = !render_chunks;
        break;
    case io::EKey::PAD_ENTER:
        render_background = !render_background;
        break;
    case io::EKey::PAD_SLASH:
        render_grayscale = !render_grayscale;
        break;
    //case io::EKey::PAD_DELETE:
    //    size++;
    //    break;
    // case io::EKey::PAD_0:
    //     if (size - 1 <= 0)
    //     {
    //         break;
    //     }
    //     size--;
    //     break;
    }
}

bool x39::goingfactory::World::is_in_view(vec2 pos, int offset)
{
    if (!m_player || !m_player->is_type(EComponent::Position))
    {
        return false;
    }
    auto playerPositionComponent = m_player->get_component<PositionComponent>();
    vec2 center = { m_viewport_w / 2 + m_viewport_x, m_viewport_h / 2 + m_viewport_y };
    auto top_left_viewport = playerPositionComponent->position() - center;
    pos -= top_left_viewport;
    if (pos.x > m_viewport_x + offset && pos.y > m_viewport_y + offset && pos.x < m_viewport_x + m_viewport_w - offset && pos.y < m_viewport_y + m_viewport_h - offset)
    {
        return true;
    }
    return false;
}
