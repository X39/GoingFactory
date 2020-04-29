#include "World.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "Entity.h"
#include "Entry.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "FastNoise.h"


#include <fstream>
#include <sstream>
#include <unordered_map>

x39::goingfactory::FastNoise generator(12030220512152);
size_t grass1_texture_id = 0;
size_t grass1_surroundings_texture_id = 0;
size_t grass2_1_texture_id = 0;
size_t grass2_2_texture_id = 0;
size_t grass2_3_texture_id = 0;
size_t tree_texture_id = 0;
size_t dirt_texture_id = 0;
size_t stonea_texture_id = 0;
size_t stoneb_texture_id = 0;
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


static enum TextureIndex
{
    Dirt,
    GrassAnimated,
    Grass,
    StoneA,
    StoneB,
    Tree,
    
    __SIZE
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
    
    x39::goingfactory::World::Tile tile = { 0 };
    tile.has_tree = false;
    tile.is_passable = true;
    tile.tile_texture = dirt_texture_id;
    tile.index = TextureIndex::Dirt;
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
        if (random.next_int() % 7 == 0)
        {
            tile.is_passable = false;
            tile.has_tree = true;
        }
        tile.index = TextureIndex::GrassAnimated;
    }
    else if (random.next_float() > 0.3)
    {
        tile.tile_texture = 0;
        tile.tile_texture_surrounding = grass1_surroundings_texture_id;
        tile.index = TextureIndex::Grass;
    }
    else if ((f = random.next_float()) < 0.1)
    {
        if (f < 0.02)
        {
            tile.tile_texture = 0;
            tile.tile_texture_surrounding = stoneb_texture_id;
            tile.is_passable = false;
            tile.index = TextureIndex::StoneB;
        }
        else
        {
            tile.tile_texture = 0;
            tile.tile_texture_surrounding = stonea_texture_id;
            tile.is_passable = false;
            tile.index = TextureIndex::StoneA;
        }
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

void x39::goingfactory::World::draw_level(GameInstance& game, vec2 top_left_viewport)
{
    struct texture_spot
    {
        size_t texture_id;
        float sx, sy, sw, sh;
        float dx, dy;

        texture_spot(size_t texture_id, float sx, float sy, float sw, float sh, float dx, float dy) :
            texture_id(texture_id),
            sx(sx),
            sy(sy),
            sw(sw),
            sh(sh),
            dx(dx),
            dy(dy)
        {
        }
    };
    std::vector<std::vector<texture_spot>> textures;
    textures.resize(TextureIndex::__SIZE);

    // Draw background
    for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % tile_size - tile_size; x < top_left_viewport.x + m_viewport_w; x += tile_size)
    {
        for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % tile_size - tile_size; y < top_left_viewport.y + m_viewport_h; y += tile_size)
        {
            vec2 pos = { x + (float)m_viewport_x, y + (float)m_viewport_y };
            auto tile = get_tile(pos);
            if (tile.has_tree)
            {
                textures[TextureIndex::Tree].emplace_back(tree_texture_id, 0, 0, 32, 32, pos.x - top_left_viewport.x - 8, pos.y - top_left_viewport.y - 16);
            }
            if (tile.tile_texture != 0)
            {
                pos -= top_left_viewport;
                textures[tile.index].emplace_back(tile.tile_texture, 0, 0, 16, 16, pos.x, pos.y);
            }
            if (tile.tile_texture_surrounding != 0)
            {
                if (tile.tile_texture == 0)
                {
                    textures[tile.index].emplace_back(tile.tile_texture_surrounding, 16, 16, 16, 16, pos.x - top_left_viewport.x, pos.y - top_left_viewport.y);
                }
                int i = 0;
                for (int32_t x2 = x - (int32_t)tile_size; x2 <= x + (int32_t)tile_size; x2 += (int32_t)tile_size)
                {
                    for (int32_t y2 = y - (int32_t)tile_size; y2 <= y + (int32_t)tile_size; y2 += (int32_t)tile_size)
                    {
                        i++;
                        vec2 otherpos = { x2 + (float)m_viewport_x, y2 + (float)m_viewport_y };
                        auto othertile = get_tile(otherpos);
                        if (othertile.index != tile.index)
                        {
                            otherpos -= top_left_viewport;
                            switch (i)
                            {
                            case 1: /* LEF TOP */ textures[tile.index].emplace_back(tile.tile_texture_surrounding,  0,  0, 16, 16, otherpos.x, otherpos.y); break;
                            case 2: /* LEF MID */ textures[tile.index].emplace_back(tile.tile_texture_surrounding,  0, 16, 16, 16, otherpos.x, otherpos.y); break;
                            case 3: /* LEF BOT */ textures[tile.index].emplace_back(tile.tile_texture_surrounding,  0, 32, 16, 16, otherpos.x, otherpos.y); break;
                            case 4: /* CEN TOP */ textures[tile.index].emplace_back(tile.tile_texture_surrounding, 16,  0, 16, 16, otherpos.x, otherpos.y); break;
                            case 5: /* CEN MID */ break;
                            case 6: /* CEN BOT */ textures[tile.index].emplace_back(tile.tile_texture_surrounding, 16, 32, 16, 16, otherpos.x, otherpos.y); break;
                            case 7: /* RIG TOP */ textures[tile.index].emplace_back(tile.tile_texture_surrounding, 32,  0, 16, 16, otherpos.x, otherpos.y); break;
                            case 8: /* RIG MID */ textures[tile.index].emplace_back(tile.tile_texture_surrounding, 32, 16, 16, 16, otherpos.x, otherpos.y); break;
                            case 9: /* RIG BOT */ textures[tile.index].emplace_back(tile.tile_texture_surrounding, 32, 32, 16, 16, otherpos.x, otherpos.y); break;
                            }
                        }
                    }
                }
            }
        }
    }

    for (auto& it : textures)
    {
        al_hold_bitmap_drawing(true);
        for (auto& texture : it)
        {
            al_draw_bitmap_region(game.resource_manager.get_bitmap(texture.texture_id), texture.sx, texture.sy, texture.sh, texture.sw, texture.dx, texture.dy, 0);
        }
        al_hold_bitmap_drawing(false);
    }

    //std::vector<vec2> trees;
    // Draw specifics
    // al_hold_bitmap_drawing(true);
    // for (int32_t x = ((int32_t)top_left_viewport.x) - ((int32_t)top_left_viewport.x) % tile_size - tile_size; x < top_left_viewport.x + m_viewport_w; x += tile_size)
    // {
    //     for (int32_t y = ((int32_t)top_left_viewport.y) - ((int32_t)top_left_viewport.y) % tile_size - tile_size; y < top_left_viewport.y + m_viewport_h; y += tile_size)
    //     {
    //         vec2 pos = { x + (float)m_viewport_x, y + (float)m_viewport_y };
    //         auto tile = get_tile(pos);
    //         if (tile.has_tree)
    //         {
    //             vec2 treecoord = { pos.x - top_left_viewport.x - 8, pos.y - top_left_viewport.y };
    //             al_draw_bitmap(game.resource_manager.get_bitmap(tree_texture_id), treecoord.x, treecoord.y, 0);
    //             // trees.emplace_back(pos.x - top_left_viewport.x - 8, pos.y - top_left_viewport.y - 16);
    //         }
    //         // else if (tile.tile_texture_surrounding != 0)
    //         // {
    //         //     int i = 0;
    //         //     for (int32_t x2 = x - (int32_t)tile_size; x2 <= x + (int32_t)tile_size; x2 += (int32_t)tile_size)
    //         //     {
    //         //         for (int32_t y2 = y - (int32_t)tile_size; y2 <= y + (int32_t)tile_size; y2 += (int32_t)tile_size)
    //         //         {
    //         //             i++;
    //         //             if (x2 == x && y2 == y) { continue; }
    //         //             vec2 otherpos = { x2 + (float)m_viewport_x, y2 + (float)m_viewport_y };
    //         //             auto othertile = get_tile(otherpos);
    //         //             if (othertile.tile_texture != tile.tile_texture)
    //         //             {
    //         //                 otherpos -= top_left_viewport;
    //         //                 switch (i)
    //         //                 {
    //         //                 case 1: /* LEF TOP */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding),  0,  0, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 case 2: /* LEF MID */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding),  0, 16, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 case 3: /* LEF BOT */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding),  0, 32, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 case 4: /* CEN TOP */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding), 16,  0, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 case 5: /* CEN MID */ break;
    //         //                 case 6: /* CEN BOT */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding), 16, 32, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 case 7: /* RIG TOP */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding), 32,  0, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 case 8: /* RIG MID */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding), 32, 16, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 case 9: /* RIG BOT */ al_draw_bitmap_region(game.resource_manager.get_bitmap(tile.tile_texture_surrounding), 32, 32, 16, 16, otherpos.x, otherpos.y, 0); break;
    //         //                 }
    //         //             }
    //         //         }
    //         //     }
    //         // }
    //     }
    // }
    // al_hold_bitmap_drawing(false);
    // al_hold_bitmap_drawing(true);
    // for (auto& treecoord : trees)
    // {
    //     al_draw_bitmap(game.resource_manager.get_bitmap(tree_texture_id), treecoord.x, treecoord.y, 0);
    // }
    // al_hold_bitmap_drawing(false);
}
void x39::goingfactory::World::draw_level_simple(GameInstance& game, vec2 top_left_viewport)
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
void x39::goingfactory::World::draw_collision_boxes(GameInstance& game, vec2 top_left_viewport)
{
    auto yellow = al_map_rgb(255, 255, 0);
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
void x39::goingfactory::World::draw_chunks(GameInstance& game, vec2 top_left_viewport)
{
    if (!m_player || !m_player->is_type(EComponent::Position))
    {
        return;
    }
    auto playerPositionComponent = m_player->get_component<PositionComponent>();
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
void x39::goingfactory::World::draw_entities(GameInstance& game, vec2 top_left_viewport)
{
    auto yellow = al_map_rgb(255, 255, 0);
    auto green = al_map_rgb(0, 255, 0);
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
        if (m_rf_entities_collision_boxes && (*it)->is_type(EComponent::Collidable))
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
        if (m_rf_entities_positioncomponent)
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
void x39::goingfactory::World::draw_graph(GameInstance& game, vec2 top_left_viewport)
{
    auto white = al_map_rgb(255, 255, 255);
    auto blue = al_map_rgba(127, 127, 255, 255);
    auto yellow = al_map_rgb(255, 255, 0);

    if (m_rf_graph_render_fps)
    {
        size_t avg = 0;
        for (size_t i = 0; i < render_fps_deque.size(); i++)
        {
            size_t r_fps = render_fps_deque[i];
            avg += r_fps;
            al_draw_line(m_viewport_x + i + 1, m_viewport_y, m_viewport_x + i + 1, m_viewport_y + r_fps, blue, 1);
        }
        // X Axis
        al_draw_line(m_viewport_x, m_viewport_y, m_viewport_x + FPS_DEQUE_MAX, m_viewport_y, white, 1);
        // Y Axis
        al_draw_line(m_viewport_x, m_viewport_y, m_viewport_x, m_viewport_y + 200, white, 1);
        std::stringstream sstream;

        sstream << "FPS: " << render_fps;
        al_draw_text(game.resource_manager.font(), yellow, m_viewport_x + 8, m_viewport_y + 8, 0, sstream.str().c_str());
        sstream.str("");

        sstream << "FPS AVG: " << (avg / render_fps_deque.size());
        al_draw_text(game.resource_manager.font(), yellow, m_viewport_x + 8, m_viewport_y + 8 + 16, 0, sstream.str().c_str());
        sstream.str("");
    }
    if (m_rf_graph_simulation_fps)
    {
        size_t avg = 0;
        for (size_t i = 0; i < simulation_fps_deque.size(); i++)
        {
            size_t s_fps = simulation_fps_deque[i];
            avg += s_fps;
            al_draw_line(
                m_viewport_x + m_viewport_w - FPS_DEQUE_MAX + i + 1,
                m_viewport_y,
                m_viewport_x + m_viewport_w - FPS_DEQUE_MAX + i + 1,
                m_viewport_y + s_fps,
                blue,
                1);
        }
        // X Axis
        al_draw_line(
            m_viewport_x + m_viewport_w - FPS_DEQUE_MAX,
            m_viewport_y,
            m_viewport_x + m_viewport_w,
            m_viewport_y,
            white,
            1);
        // Y Axis
        al_draw_line(
            m_viewport_x,
            m_viewport_y,
            m_viewport_x,
            m_viewport_y + 200,
            white,
            1);
        std::stringstream sstream;

        sstream << "SIM FPS: " << simulation_fps;
        al_draw_text(game.resource_manager.font(), yellow, m_viewport_x + m_viewport_w - FPS_DEQUE_MAX + 8, m_viewport_y + 8, 0, sstream.str().c_str());
        sstream.str("");

        if (simulation_fps_deque.size() == 0)
        {
            sstream << "SIM FPS AVG: " << "DIV0";
        }
        else
        {
            sstream << "SIM FPS AVG: " << (avg / simulation_fps_deque.size());
        }
        al_draw_text(game.resource_manager.font(), yellow, m_viewport_x + m_viewport_w - FPS_DEQUE_MAX + 8, m_viewport_y + 8 + 16, 0, sstream.str().c_str());
        sstream.str("");
    }
}
x39::goingfactory::World::World() :
    m_player(),
    m_viewport_x(0),
    m_viewport_y(0),
    m_viewport_w(0),
    m_viewport_h(0),
    m_rf_level(true),
    m_rf_level_simple(false),
    m_rf_chunks(false),
    m_rf_entities(true),
    m_rf_entities_positioncomponent(false),
    m_rf_entities_collision_boxes(false),
    m_rf_graph(false),
    m_rf_graph_render_fps(true),
    m_rf_graph_simulation_fps(true),
    m_selected_option(0),
    m_selected_last()
{
    std::ifstream f("world.settings");
    if (f.good())
    {
        float frequency;
        f >>
            frequency >>
            store_a_player_vel.x >> store_a_player_vel.y >>
            store_a_player_pos.x >> store_a_player_pos.y >>
            m_rf_level >>
            m_rf_level_simple >>
            m_rf_chunks >>
            m_rf_entities >>
            m_rf_entities_positioncomponent >>
            m_rf_entities_collision_boxes >>
            m_rf_graph >>
            m_rf_graph_render_fps >>
            m_rf_graph_simulation_fps;
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
            m_rf_level << std::endl <<
            m_rf_level_simple << std::endl <<
            m_rf_chunks << std::endl <<
            m_rf_entities << std::endl <<
            m_rf_entities_positioncomponent << std::endl <<
            m_rf_entities_collision_boxes << std::endl <<
            m_rf_graph << std::endl <<
            m_rf_graph_render_fps << std::endl <<
            m_rf_graph_simulation_fps << std::endl;
    }
}

void x39::goingfactory::World::render(GameInstance& game)
{
    // if (grass1_texture_id == 0) { grass1_texture_id = game.resource_manager.load_bitmap("Textures/Grass.png"); }
    if (grass1_surroundings_texture_id == 0) { grass1_surroundings_texture_id = game.resource_manager.load_bitmap("Textures/GrassSurroundings.png"); }
    if (grass2_1_texture_id == 0) { grass2_1_texture_id = game.resource_manager.load_bitmap("Textures/Grass2_1.png"); }
    if (grass2_2_texture_id == 0) { grass2_2_texture_id = game.resource_manager.load_bitmap("Textures/Grass2_2.png"); }
    if (grass2_3_texture_id == 0) { grass2_3_texture_id = game.resource_manager.load_bitmap("Textures/Grass2_3.png"); }
    if (dirt_texture_id == 0) { dirt_texture_id = game.resource_manager.load_bitmap("Textures/Dirt.png"); }
    if (stonea_texture_id == 0) { stonea_texture_id = game.resource_manager.load_bitmap("Textures/StoneA.png"); }
    if (stoneb_texture_id == 0) { stoneb_texture_id = game.resource_manager.load_bitmap("Textures/StoneB.png"); }
    if (tree_texture_id == 0) { tree_texture_id = game.resource_manager.load_bitmap("Textures/Tree.png"); }

    if (!m_player || !m_player->is_type(EComponent::Position))
    {
        return;
    }
    auto playerPositionComponent = m_player->get_component<PositionComponent>();
    vec2 center = { m_viewport_w / 2 + m_viewport_x, m_viewport_h / 2 + m_viewport_y };
    vec2 top_left_viewport =  playerPositionComponent->position() - center;

    if (m_rf_level)
    {
        if (m_rf_level_simple) { draw_level_simple(game, top_left_viewport); }
        else { draw_level(game, top_left_viewport); }
    }
    if (m_rf_entities_collision_boxes) { draw_collision_boxes(game, top_left_viewport); }
    if (m_rf_chunks) { draw_chunks(game, top_left_viewport); }
    if (m_rf_entities) { draw_entities(game, top_left_viewport); }
    if (m_rf_graph) { draw_graph(game, top_left_viewport); }
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

    if (m_selected_option != 0)
    {
        auto now = std::chrono::system_clock::now();
        auto delta_t = now - m_selected_last;
        auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta_t);
        if (miliseconds.count() <= 2000)
        {
            auto white = al_map_rgb(255, 255, 255);
            std::stringstream sstream;
            size_t i = 0;
            al_draw_text(game.resource_manager.font(), white, m_viewport_x, m_viewport_y + m_viewport_h - (i * 16 + 16), 0, "[F8] down; [F7] use; [F6] up;");
            i++;

            for (; i < selected_option_max; i++)
            {
                sstream << (selected_option_max - i == m_selected_option ? "-> " : "   ") << name_option(selected_option_max - i) << "=" << value_option(selected_option_max - i);
                al_draw_text(game.resource_manager.font(), white, m_viewport_x, m_viewport_y + m_viewport_h - (i * 16 + 16), 0, sstream.str().c_str());
                sstream.str("");
            }
        }
        else
        {
            m_selected_option = 0;
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
    case io::EKey::F8:
        next_option();
        break;
    case io::EKey::F7:
        switch_option(m_selected_option);
        break;
    case io::EKey::F6:
        previous_option();
        break;
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
