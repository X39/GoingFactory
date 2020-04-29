#include "Entry.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/mouse.h>

#include <cstdio>
#include <string>
#include <sstream>
#include <random>
#include <iostream>
#include <filesystem>
#include <yaoosl.h>
#include <yaoosl_code.h>
#include <yaoosl_class.h>
#include <yaoosl_instance.h>
#include <yaoosl_util.h>
#include <yaoosl_context.h>

#include "ThreadPool.h"

#include "ResourceManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "ScriptedEntity.h"
#include "Asteroid.h"
#include "Player.h"
#include "Marker.h"
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "World.h"
#include "KeyboardTarget.h"


#define X39_GF_DISABLE_MT
#define X39_GF_SKIP_SIM 20


int DISPLAY_WIDTH;
int DISPLAY_HEIGHT;

#if _DEBUG
const float RENDER_FPS = 30;
#else
const float RENDER_FPS = 60;
#endif
int initialize_allegro(ALLEGRO_DISPLAY*& display, ALLEGRO_EVENT_QUEUE*& event_queue, ALLEGRO_TIMER*& render_timer, ALLEGRO_FONT*& font)
{
    if (!al_init())
    {
        fprintf(stderr, "Failed to initialize Allegro.\n");
        return -1;
    }
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_mouse();
    al_install_keyboard();
    DISPLAY_WIDTH = 780;
    DISPLAY_HEIGHT = 780;
    display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!display)
    {
        fprintf(stderr, "Failed to create display.\n");
        return -1;
    }

    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_display(display);
        return -1;
    }

    render_timer = al_create_timer(1.0 / RENDER_FPS);
    if (!render_timer)
    {
        fprintf(stderr, "failed to create render_timer!\n");
        return -1;
    }

    font = al_load_ttf_font("arial.ttf", 72, 0);
    if (!font)
    {
        fprintf(stderr, "Could not load 'arial.ttf'.\n");
        font = al_create_builtin_font(); //Load fallback
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(render_timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());


    // al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

    al_start_timer(render_timer);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    return 0;
}

static int get_bom_skip(const char* ubuff)
{
    if (!ubuff)
        return 0;
    // We are comparing against unsigned
    if (ubuff[0] == 0xEF && ubuff[1] == 0xBB && ubuff[2] == 0xBF)
    {
        //UTF-8
        return 3;
    }
    else if (ubuff[0] == 0xFE && ubuff[1] == 0xFF)
    {
        //UTF-16 (BE)
        return 2;
    }
    else if (ubuff[0] == 0xFE && ubuff[1] == 0xFE)
    {
        //UTF-16 (LE)
        return 2;
    }
    else if (ubuff[0] == 0x00 && ubuff[1] == 0x00 && ubuff[2] == 0xFF && ubuff[3] == 0xFF)
    {
        //UTF-32 (BE)
        return 2;
    }
    else if (ubuff[0] == 0xFF && ubuff[1] == 0xFF && ubuff[2] == 0x00 && ubuff[3] == 0x00)
    {
        //UTF-32 (LE)
        return 2;
    }
    else if (ubuff[0] == 0x2B && ubuff[1] == 0x2F && ubuff[2] == 0x76 &&
        (ubuff[3] == 0x38 || ubuff[3] == 0x39 || ubuff[3] == 0x2B || ubuff[3] == 0x2F))
    {
        //UTF-7
        return 4;
    }
    else if (ubuff[0] == 0xF7 && ubuff[1] == 0x64 && ubuff[2] == 0x4C)
    {
        //UTF-1
        return 3;
    }
    else if (ubuff[0] == 0xDD && ubuff[1] == 0x73 && ubuff[2] == 0x66 && ubuff[3] == 0x73)
    {
        //UTF-EBCDIC
        return 3;
    }
    else if (ubuff[0] == 0x0E && ubuff[1] == 0xFE && ubuff[2] == 0xFF)
    {
        //SCSU
        return 3;
    }
    else if (ubuff[0] == 0xFB && ubuff[1] == 0xEE && ubuff[2] == 0x28)
    {
        //BOCU-1
        if (ubuff[3] == 0xFF)
            return 4;
        return 3;
    }
    else if (ubuff[0] == 0x84 && ubuff[1] == 0x31 && ubuff[2] == 0x95 && ubuff[3] == 0x33)
    {
        //GB 18030
        return 3;
    }
    return 0;
}
static char* readFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    size_t file_size;
    char* out;
    int bom_skip, i;
    if (!file)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (!(out = new char[file_size + 1]))
    {
        fclose(file);
        return NULL;
    }
    if (file_size > 5)
    {
        fread(out, sizeof(char), 5, file);
        out[5] = '\0';
        bom_skip = get_bom_skip(out);
        for (i = bom_skip; i < 5; i++)
        {
            out[i - bom_skip] = out[i];
        }
        fread(out + 5 - bom_skip, sizeof(char), file_size - 5, file);
    }
    else
    {
        fread(out, sizeof(char), file_size, file);
    }
    out[file_size] = '\0';
    fclose(file);

    return out;
}

#pragma region Trim std::string
// https://stackoverflow.com/a/217605/2684203
// trim from start (in place)
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
#pragma endregion

static int LoadYaooslClasses(yaooslhandle runtime, x39::goingfactory::GameInstance& gameInstance)
{
    std::vector<std::string> classesnames;
    std::vector<std::array<std::string, 2>> files;
    std::filesystem::path path("yaoosl/classes");
    path = std::filesystem::absolute(path);
    if (!std::filesystem::exists(path))
    {
        std::cerr << "'" << path.string() << "' is not existing." << std::endl;
        return -1;
    }
    for (auto file : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
    {
        if (file.is_directory())
        {
            std::cerr << "Skipping directory '" << file.path() << "'." << std::endl;
            continue;
        }
        auto contents = readFile(file.path().string().c_str());
        std::string str(contents);
        files.push_back(std::array<std::string, 2> { file.path().string(), str });
        delete[] contents;

        auto classdef = str.find("class ") + std::strlen("class ");
        auto end = str.find('\n', classdef);
        auto classname = str.substr(classdef, end - classdef);
        trim(classname);
        classesnames.push_back(classname);
    }
    {
#pragma region GoingFactory.Entity
        {
            auto classhandle = yaoosl_declare_class(runtime, "GoingFactory.Entity");
            classhandle->callback_data = &gameInstance;
            classhandle->callback_create = [](void* data, struct yaoosl_instance* instance) -> bool {
                x39::goingfactory::GameInstance* gameInstance = static_cast<x39::goingfactory::GameInstance*>(data);
                auto entity = new x39::goingfactory::entity::ScriptedEntity();
                gameInstance->entity_manager.pool_create(entity);
                instance->additional.ptr = entity;
                return true;
            };
            classhandle->callback_destroy = [](void* data, struct yaoosl_instance* instance) -> void {
                x39::goingfactory::GameInstance* gameInstance = static_cast<x39::goingfactory::GameInstance*>(data);
                auto entity = static_cast<x39::goingfactory::entity::ScriptedEntity*>(instance->additional.ptr);
                gameInstance->entity_manager.pool_destroy(entity);
            };

            auto position_getter_code = yaoosl_code_create2(
                [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                {
                    auto entity = static_cast<x39::goingfactory::entity::ScriptedEntity*>(self->additional.ptr);
                    auto vector2 = yaoosl_instance_create(yaoosl_declare_class(vm, "GoingFactory.Vector2"));
                    vector2->fields[0]->additional.d = entity->position().x;
                    vector2->fields[1]->additional.d = entity->position().y;
                    yaoosl_instance_inc_ref(vector2);
                    yaoosl_context_push_value(context, vector2);
                });
            auto position_getter_method = yaoosl_method_create("get", yaoosl_declare_class(runtime, "GoingFactory.Vector2"), position_getter_code);
            auto position_setter_code = yaoosl_code_create2(
                [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                {
                    auto entity = static_cast<x39::goingfactory::entity::ScriptedEntity*>(self->additional.ptr);
                    yaooslinstancehandle vector2;
                    if (!(vector2 = yaoosl_context_pop_value(context)))
                    {
                        yaoosl_util_throw_generic(vm, context, "No value found.");
                        return;
                    }
                    if (vector2->type != yaoosl_declare_class(vm, "GoingFactory.Vector2"))
                    {
                        yaoosl_util_throw_generic(vm, context, "Value is no Vector2.");
                        return;
                    }
                    entity->position({ vector2->fields[0]->additional.d, vector2->fields[1]->additional.d });
                    yaoosl_instance_dec_ref(vector2);
                });
            auto position_setter_method = yaoosl_method_create("set", yaoosl_declare_class(runtime, "GoingFactory.Vector2"), position_setter_code);
            auto propertyhandle = yaoosl_property_create2("Position", yaoosl_declare_class(runtime, "GoingFactory.Vector2"), position_getter_method, position_setter_method);
            yaoosl_class_push_property(classhandle, propertyhandle);
        }
#pragma endregion
    }
    for (auto& classname : classesnames)
    {
        auto classhandle = yaoosl_declare_class(runtime, classname.c_str());
        if (!classhandle)
        {
            std::cerr << "Failed to declare class '" << classname << "'. Aborting." << std::endl;
            return -1;
        }
    }
    for (auto& file : files)
    {
        auto code = yaoosl_code_parse_contents(file[1].c_str(), true, file[0].c_str());
        if (code)
        {
            yaoosl_util_execute_code(runtime, code);
        }
        else
        {
            std::cerr << "Failed to parse '" << file[0] << "'. Aborting." << std::endl;
            return -1;
        }
    }
    return 0;
}

static bool yaoosl_error_handle(struct yaoosl* vm, struct yaoosl_context* context, const char* errmsg)
{
    printf("!!! FATAL ERROR !!!\n%s\n", errmsg);
    return false;
}
int main()
{
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* event_queue;
    ALLEGRO_TIMER* render_timer;
    ALLEGRO_FONT* font;
    int init_res = initialize_allegro(display, event_queue, render_timer, font);
    if (init_res)
    {
        return init_res;
    }

    x39::goingfactory::ResourceManager resources_manager;
    x39::goingfactory::EntityManager entity_manager;
    x39::goingfactory::io::KeyboardTarget keyboard_target;
    x39::goingfactory::World world;
    al_set_target_backbuffer(display);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::move_up, x39::goingfactory::io::EKey::W);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::move_left, x39::goingfactory::io::EKey::A);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::move_down, x39::goingfactory::io::EKey::S);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::move_right, x39::goingfactory::io::EKey::D);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::mod_a, x39::goingfactory::io::EKey::LSHIFT);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::mod_b, x39::goingfactory::io::EKey::LCTRL);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::mod_c, x39::goingfactory::io::EKey::RSHIFT);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::mod_d, x39::goingfactory::io::EKey::RCTRL);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::trigger_a, x39::goingfactory::io::EKey::SPACE);
    keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::trigger_b, x39::goingfactory::io::EKey::ALT);
    x39::goingfactory::GameInstance game_instance(entity_manager, resources_manager, world);
    entity_manager.onEntityAdded.subscribe([&game_instance](
        x39::goingfactory::EntityManager& entity_manager, x39::goingfactory::EntityManager::EntityAddedEventArgs args) -> void {
            if (args.entity->is_type(x39::goingfactory::EComponent::Render))
            {
                auto renderComponent = args.entity->get_component<x39::goingfactory::RenderComponent>();
                renderComponent->render_init(game_instance);
            }
        });


    yaooslhandle runtime = yaoosl_create_virtualmachine();
    runtime->error_handle = yaoosl_error_handle;
    if (!runtime)
    {
        fprintf(stderr, "Failed to create yaoosl runtime.\n");
        return -1;
    }
    if (LoadYaooslClasses(runtime, game_instance))
    {
        return -1;
    }
    yaooslinstancehandle yaoosl_game = yaoosl_instance_create(yaoosl_declare_class(runtime, "GoingFactory.Game"));
    yaoosl_instance_inc_ref(yaoosl_game);
    { // Call yaoosl_game constructor
        yaooslcontexthandle context = yaoosl_context_create();
        yaoosl_call_method(runtime, context, yaoosl_game->type->constructor_method_group->methods[0], yaoosl_game);
        yaoosl_execute(runtime, context);
        yaoosl_context_destroy(context);
    }


    auto player = new x39::goingfactory::entity::Player();
    world.set_player(player);
    const int viewport_offset = 32;
    world.set_viewport(viewport_offset, viewport_offset, DISPLAY_WIDTH - viewport_offset * 2, DISPLAY_HEIGHT - viewport_offset * 2);
    const int level_size = 50000;
    /*
    m_pos
    {x=-299.819855 y=-198.473175 }
    x: -299.819855
    y: -198.473175
    m_velocity
    {x=48.4052124 y=-58.0977020 }
        x: 48.4052124
        y: -58.0977020
    */
    //player->position({ -299.819855, -198.473175 });
    //x39::goingfactory::vec2 player_vel = { 48.4052124, -58.0977020 };
    //player->velocity(player_vel);
    entity_manager.pool_create(player);
    auto marker = new x39::goingfactory::entity::Marker();
    //auto player_vel_len = player_vel.length();
    //player_vel.normalize();
    //player_vel *= player_vel_len * 0.6666;
    //marker->position(player->position() + player_vel);
    marker->position({ 32, 8 });
    entity_manager.pool_create(marker);

    for (int i = 0; i < 0; i++)
    {
        auto asteroid = new x39::goingfactory::entity::Asteroid();
        asteroid->position({ (rand() % level_size - level_size / 2), (rand() % level_size - level_size / 2) });
        asteroid->velocity({ ((rand() % 20) / 20.0f) * 60, ((rand() % 20) / 20.0f) * 60 });
        entity_manager.pool_create(asteroid);
    }

    entity_manager.act_pools();

    auto old_frame_time = al_get_time();
    auto old_sim_time = al_get_time();
    int last_sim_fps = 0;
    bool redraw = false;
    bool simulate = true;
    bool mouseDown = false;
    bool halt_simulation = true;
#ifdef X39_GF_SKIP_SIM
    int _x39_gf_skip_sim = 0;
#endif // X39_GF_SKIP_SIM
    while (true)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
        {
            DISPLAY_HEIGHT = ev.display.height;
            DISPLAY_WIDTH = ev.display.width;
            world.set_viewport(32, 32, DISPLAY_WIDTH - 64, DISPLAY_HEIGHT - 64);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            mouseDown = true;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
        {

        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            mouseDown = false;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            auto modifier = static_cast<x39::goingfactory::io::EModifier>(ev.keyboard.modifiers);
            auto key = static_cast<x39::goingfactory::io::EKey>(ev.keyboard.keycode);
            if (key == x39::goingfactory::io::EKey::PAUSE)
            {
                halt_simulation = !halt_simulation;
            }
            for (auto it : entity_manager)
            {
                if (!it) { continue; }
                if (it->is_type(x39::goingfactory::EComponent::Keyboard))
                {
                    auto keyboardComponent = it->get_component<x39::goingfactory::KeyboardComponent>();
                    keyboardComponent->key_down(game_instance, key, modifier);
                }
            }
            keyboard_target.key_down(key, modifier);
            world.keydown(key);
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            auto modifier = static_cast<x39::goingfactory::io::EModifier>(ev.keyboard.modifiers);
            auto key = static_cast<x39::goingfactory::io::EKey>(ev.keyboard.keycode);
            for (auto it : entity_manager)
            {
                if (!it) { continue; }
                if (it->is_type(x39::goingfactory::EComponent::Keyboard))
                {
                    auto keyboardComponent = it->get_component<x39::goingfactory::KeyboardComponent>();
                    keyboardComponent->key_up(game_instance, key, modifier);
                }
            }
            keyboard_target.key_up(key, modifier);
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            if (ev.timer.source == render_timer)
            {
                redraw = true;
            }
        }
        if (simulate)
        {
            { // Call yaoosl_game GameLoop
                yaooslcontexthandle context = yaoosl_context_create();
                yaoosl_call_method(runtime, context, yaoosl_game->type->method_groups[0]->methods[0], yaoosl_game);
                yaoosl_execute(runtime, context);
                yaoosl_context_destroy(context);
            }
            simulate = false;
            entity_manager.act_pools();
            auto new_time = al_get_time();
            float sim_delta = (float)(new_time - old_sim_time);
            last_sim_fps = sim_delta >= 1 ? 0 : (int)(1 / sim_delta);
            old_sim_time = new_time;
#ifndef X39_GF_DISABLE_MT
                game_instance.thread_pool().enqueue([&] {
                    std::vector<std::future<void>> results;
#endif // X39_GF_DISABLE_MT
                    auto start = entity_manager.begin(x39::goingfactory::EComponent::Simulate);
                    auto end = entity_manager.end(x39::goingfactory::EComponent::Simulate);
                    const size_t handle_amount = 100;
                    size_t range = end - start;
                    size_t tasks = range / handle_amount + (range % handle_amount == 0 ? 0 : 1);
                    if (!halt_simulation)
                    {
                        for (; tasks > 0; tasks--)
                        {
                            auto range_start = (tasks - 1) * handle_amount;
                            auto range_end = tasks * handle_amount;
#ifndef X39_GF_DISABLE_MT
                            results.emplace_back(
                                game_instance.thread_pool().enqueue([sim_delta, &start, range_start, range_end, &game_instance, range] {
#endif // X39_GF_DISABLE_MT
                                    for (auto i = range_start; i != range_end && i != range; i++)
                                    {
                                        auto it = start + i;
                                        auto simulateComponent = (*it)->get_component<x39::goingfactory::SimulateComponent>();
                                        simulateComponent->simulate(game_instance, sim_delta);
                                    }
#ifndef X39_GF_DISABLE_MT
                                    }));
#endif // X39_GF_DISABLE_MT
                        }
                    }
                    keyboard_target.entity_interact(game_instance);
#ifndef X39_GF_DISABLE_MT
                    for (auto&& result : results) { result.get(); }
#endif // X39_GF_DISABLE_MT
                    simulate = true;
#ifndef X39_GF_DISABLE_MT
                    });
#endif // X39_GF_DISABLE_MT

        }
        if (redraw && al_is_event_queue_empty(event_queue))
        {
            world.render(game_instance);
            // world.set_viewport(viewport_offset, viewport_offset, DISPLAY_WIDTH - viewport_offset * 2, DISPLAY_HEIGHT - viewport_offset * 2);
            auto color = al_map_rgba(0, 0, 0, 200);
            al_draw_filled_rectangle(0, 0, DISPLAY_WIDTH, viewport_offset, color);
            al_draw_filled_rectangle(0, 0, viewport_offset, DISPLAY_HEIGHT, color);
            al_draw_filled_rectangle(0, DISPLAY_HEIGHT - viewport_offset, DISPLAY_WIDTH, DISPLAY_HEIGHT, color);
            al_draw_filled_rectangle(DISPLAY_WIDTH - viewport_offset, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, color);

            auto new_time = al_get_time();
            auto fps = (int)(1 / (new_time - old_frame_time));
            std::stringstream sstream;


            sstream << "Controls:";
            al_draw_text(font, al_map_rgb(0, 127, 0), 1, 1 + 10 * 0, 0, sstream.str().c_str());
            sstream.str("");

            sstream << "Up (W) | Left (A) | Down (S) | Right (D) | Trigger (Space|Alt) | Fast (LShift) | Slow (LCTRL)";
            al_draw_text(font, al_map_rgb(0, 127, 0), 1, 1 + 10 * 1, 0, sstream.str().c_str());
            sstream.str("");

            sstream << "Note that all numpad buttons (but enter) also have a meaning.";
            al_draw_text(font, al_map_rgb(0, 127, 0), 1, 1 + 10 * 2, 0, sstream.str().c_str());
            sstream.str("");


            sstream << "Render-FPS: " << fps;
            al_draw_text(font, al_map_rgb(255, 255, 0), 1, DISPLAY_HEIGHT - 1 - 10 * 3, 0, sstream.str().c_str());
            sstream.str("");

            sstream << "Simulation-FPS: " << last_sim_fps;
            al_draw_text(font, al_map_rgb(255, 255, 0), 1, DISPLAY_HEIGHT - 1 - 10 * 2, 0, sstream.str().c_str());
            sstream.str("");

            size_t count = 0;
            for (auto it : entity_manager)
            {
                if (it) { count++; }
            }
            sstream << "Entity Manager Info: alive: " << count << "; size: " << entity_manager.size() << "; capacity: " << entity_manager.capacity();
            al_draw_text(font, al_map_rgb(255, 255, 0), 1, DISPLAY_HEIGHT - 1 - 10 * 1, 0, sstream.str().c_str());
            sstream.str("");


            sstream << "Player Position: { " << player->position().x << ", " << player->position().y << " }";
            al_draw_text(font, al_map_rgb(255, 255, 0), DISPLAY_WIDTH / 2 + 1, DISPLAY_HEIGHT - 1 - 10 * 3, 0, sstream.str().c_str());
            sstream.str("");

            sstream << "Player Chunk: { " << player->chunk()->index_x() << ", " << player->chunk()->index_y() << "}";
            al_draw_text(font, al_map_rgb(255, 255, 0), DISPLAY_WIDTH / 2 + 1, DISPLAY_HEIGHT - 1 - 10 * 2, 0, sstream.str().c_str());
            sstream.str("");

            int v_off = 5;
            // auto color_in_view = al_map_rgb(0, 127, 0);
            // auto color_out_of_view = al_map_rgb(127, 0, 0);
            // for (auto it : entity_manager)
            // {
            //     if (it->is_type(x39::goingfactory::EComponent::Position))
            //     {
            //         auto positionComponent = it->get_component<x39::goingfactory::PositionComponent>();
            //         sstream << it->type_name() << "(" << "): { " << positionComponent->position().x << ", " << positionComponent->position().y << " }";
            //         if (world.is_in_view(positionComponent->position(), -32))
            //         {
            //             al_draw_text(font, color_in_view, 1, DISPLAY_HEIGHT - 1 - 10 * v_off, 0, sstream.str().c_str());
            //         }
            //         else
            //         {
            //             al_draw_text(font, color_out_of_view, 1, DISPLAY_HEIGHT - 1 - 10 * v_off, 0, sstream.str().c_str());
            //         }
            //         sstream.str("");
            //         v_off++;
            //     }
            // }

            old_frame_time = new_time;

            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_timer(render_timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
