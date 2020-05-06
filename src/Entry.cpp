#include "Entry.h"

#include "ThreadPool.h"

#include "ResourceManager.h"
#include "entity/EntityManager.h"
#include "entity/Entity.h"
#include "entity/ScriptedEntity.h"
#include "entity/Asteroid.h"
#include "entity/Player.h"
#include "entity/Marker.h"
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "World.h"
#include "KeyboardTarget.h"
#include "UXPanel.h"
#include "UXHandler.h"
#include "UXPlaceable.h"
#include "world/FastNoiseWorldGenerator.h"


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
#include <chrono>

#include <crtdbg.h>
#include <limits>

using namespace x39::goingfactory;


// #define X39_GF_DISABLE_MT


int DISPLAY_WIDTH;
int DISPLAY_HEIGHT;
size_t render_fps = 0;
size_t simulation_fps = 0;
std::deque<size_t> render_fps_deque;
std::deque<size_t> simulation_fps_deque;

#if _DEBUG
const float RENDER_FPS = 30;
#else
const float RENDER_FPS = 60;
#endif
const int SIMULATION_TARGET_FPS = RENDER_FPS;
int initialize_allegro(ALLEGRO_DISPLAY*& display, ALLEGRO_EVENT_QUEUE*& event_queue, ALLEGRO_TIMER*& render_timer)
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
    DISPLAY_WIDTH = 780 + 192;
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

static int LoadYaooslClasses(yaooslhandle runtime, GameInstance& gameInstance)
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
                GameInstance* gameInstance = static_cast<GameInstance*>(data);
                auto entity = new entity::ScriptedEntity();
                // entity->onDestroy.subscribe([instance](entity::Entity& sender, EventArgs& e) -> void {
                //     
                // });
                gameInstance->entity_manager.pool_create(entity);
                instance->additional.ptr = entity;
                return true;
            };
            classhandle->callback_destroy = [](void* data, struct yaoosl_instance* instance) -> void {
                GameInstance* gameInstance = static_cast<GameInstance*>(data);
                auto entity = static_cast<entity::ScriptedEntity*>(instance->additional.ptr);
                gameInstance->entity_manager.pool_destroy(entity);
            };


            /* GoingFactory.Entity.PosX */ { 
                auto posx_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto d = yaoosl_instance_create(vm->classes[DOUBLE]);
                        d->additional.d = entity->position().x;
                        yaoosl_instance_inc_ref(d);
                        yaoosl_context_push_value(context, d);
                    });
                auto posx_getter_method = yaoosl_method_create("get", runtime->classes[DOUBLE], posx_getter_code);
                auto posx_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle d;
                        if (!(d = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (d->type != vm->classes[DOUBLE])
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no Double.");
                            return;
                        }
                        entity->position({ d->additional.d, entity->position().y });
                        yaoosl_instance_dec_ref(d);
                    });
                auto posx_setter_method = yaoosl_method_create("set", nullptr, posx_setter_code);
                auto posx_property = yaoosl_property_create2("PosX", runtime->classes[DOUBLE], posx_getter_method, posx_setter_method);
                yaoosl_class_push_property(classhandle, posx_property);
            }
            /* GoingFactory.Entity.PosY */ {
                auto posy_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto d = yaoosl_instance_create(vm->classes[DOUBLE]);
                        d->additional.d = entity->position().y;
                        yaoosl_instance_inc_ref(d);
                        yaoosl_context_push_value(context, d);
                    });
                auto posy_getter_method = yaoosl_method_create("get", runtime->classes[DOUBLE], posy_getter_code);
                auto posy_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle d;
                        if (!(d = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (d->type != vm->classes[DOUBLE])
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no Double.");
                            return;
                        }
                        entity->position({ entity->position().x, d->additional.d });
                        yaoosl_instance_dec_ref(d);
                    });
                auto posy_setter_method = yaoosl_method_create("set", nullptr, posy_setter_code);
                auto posy_property = yaoosl_property_create2("PosY", runtime->classes[DOUBLE], posy_getter_method, posy_setter_method);
                yaoosl_class_push_property(classhandle, posy_property);
            }

            /* GoingFactory.Entity.VelX */ {
                auto velx_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto d = yaoosl_instance_create(vm->classes[DOUBLE]);
                        d->additional.d = entity->velocity().x;
                        yaoosl_instance_inc_ref(d);
                        yaoosl_context_push_value(context, d);
                    });
                auto velx_getter_method = yaoosl_method_create("get", runtime->classes[DOUBLE], velx_getter_code);
                auto velx_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle d;
                        if (!(d = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (d->type != vm->classes[DOUBLE])
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no Double.");
                            return;
                        }
                        entity->velocity({ d->additional.d, entity->velocity().y });
                        yaoosl_instance_dec_ref(d);
                    });
                auto velx_setter_method = yaoosl_method_create("set", nullptr, velx_setter_code);
                auto velx_property = yaoosl_property_create2("VelX", runtime->classes[DOUBLE], velx_getter_method, velx_setter_method);
                yaoosl_class_push_property(classhandle, velx_property);
            }
            /* GoingFactory.Entity.VelY */ {
                auto vely_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto d = yaoosl_instance_create(vm->classes[DOUBLE]);
                        d->additional.d = entity->velocity().y;
                        yaoosl_instance_inc_ref(d);
                        yaoosl_context_push_value(context, d);
                    });
                auto vely_getter_method = yaoosl_method_create("get", runtime->classes[DOUBLE], vely_getter_code);
                auto vely_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle d;
                        if (!(d = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (d->type != vm->classes[DOUBLE])
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no Double.");
                            return;
                        }
                        entity->velocity({ entity->velocity().x, d->additional.d });
                        yaoosl_instance_dec_ref(d);
                    });
                auto vely_setter_method = yaoosl_method_create("set", nullptr, vely_setter_code);
                auto vely_property = yaoosl_property_create2("VelY", runtime->classes[DOUBLE], vely_getter_method, vely_setter_method);
                yaoosl_class_push_property(classhandle, vely_property);
            }

            /* GoingFactory.Entity.Texture */ {
                auto texture_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto texture = yaoosl_instance_create(yaoosl_declare_class(vm, "GoingFactory.Texture"));
                        texture->additional.uint64 = entity->texture().index();
                        texture->fields[0]->additional.d = entity->texture().width();
                        texture->fields[1]->additional.d = entity->texture().height();
                        yaoosl_instance_inc_ref(texture);
                        yaoosl_context_push_value(context, texture);
                    });
                auto texture_getter_method = yaoosl_method_create("get", yaoosl_declare_class(runtime, "GoingFactory.Texture"), texture_getter_code);
                auto texture_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        GameInstance* gameInstance = static_cast<GameInstance*>(self->type->callback_data);
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle texture;
                        if (!(texture = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (texture->type != yaoosl_declare_class(vm, "GoingFactory.Texture"))
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no GoingFactory.Texture.");
                            return;
                        }
                        entity->texture(gameInstance->resource_manager.from_index(texture->additional.uint64));
                        yaoosl_instance_dec_ref(texture);
                    });
                auto texture_setter_method = yaoosl_method_create("set", nullptr, texture_setter_code);
                auto texture_property = yaoosl_property_create2("Texture", yaoosl_declare_class(runtime, "GoingFactory.Texture"), texture_getter_method, texture_setter_method);
                yaoosl_class_push_property(classhandle, texture_property);
            }

            /* GoingFactory.Entity.TextureCenterX */ {
                auto texturecenterx_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto d = yaoosl_instance_create(vm->classes[DOUBLE]);
                        d->additional.d = entity->texture_center().x;
                        yaoosl_instance_inc_ref(d);
                        yaoosl_context_push_value(context, d);
                    });
                auto texturecenterx_getter_method = yaoosl_method_create("get", runtime->classes[DOUBLE], texturecenterx_getter_code);
                auto texturecenterx_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle d;
                        if (!(d = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (d->type != vm->classes[DOUBLE])
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no Double.");
                            return;
                        }
                        entity->texture_center({ d->additional.d, entity->texture_center().y });
                        yaoosl_instance_dec_ref(d);
                    });
                auto texturecenterx_setter_method = yaoosl_method_create("set", nullptr, texturecenterx_setter_code);
                auto texturecenterx_property = yaoosl_property_create2("TextureCenterX", runtime->classes[DOUBLE], texturecenterx_getter_method, texturecenterx_setter_method);
                yaoosl_class_push_property(classhandle, texturecenterx_property);
            }
            /* GoingFactory.Entity.TextureCenterY */ {
                auto texturecenterx_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto d = yaoosl_instance_create(vm->classes[DOUBLE]);
                        d->additional.d = entity->texture_center().y;
                        yaoosl_instance_inc_ref(d);
                        yaoosl_context_push_value(context, d);
                    });
                auto texturecenterx_getter_method = yaoosl_method_create("get", runtime->classes[DOUBLE], texturecenterx_getter_code);
                auto texturecenterx_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle d;
                        if (!(d = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (d->type != vm->classes[DOUBLE])
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no Double.");
                            return;
                        }
                        entity->texture_center({ entity->texture_center().x, d->additional.d });
                        yaoosl_instance_dec_ref(d);
                    });
                auto texturecenterx_setter_method = yaoosl_method_create("set", nullptr, texturecenterx_setter_code);
                auto texturecenterx_property = yaoosl_property_create2("TextureCenterY", runtime->classes[DOUBLE], texturecenterx_getter_method, texturecenterx_setter_method);
                yaoosl_class_push_property(classhandle, texturecenterx_property);
            }

            /* GoingFactory.Entity.CanCollide */ {
                auto cancollide_getter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        auto flag = yaoosl_instance_create(vm->classes[BOOLEAN]);
                        flag->additional.flag = entity->can_collide();
                        yaoosl_instance_inc_ref(flag);
                        yaoosl_context_push_value(context, flag);
                    });
                auto cancollide_getter_method = yaoosl_method_create("get", runtime->classes[DOUBLE], cancollide_getter_code);
                auto cancollide_setter_code = yaoosl_code_create2(
                    [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                    {
                        auto entity = static_cast<entity::ScriptedEntity*>(self->additional.ptr);
                        yaooslinstancehandle flag;
                        if (!(flag = yaoosl_context_pop_value(context)))
                        {
                            yaoosl_util_throw_generic(vm, context, "No value found.");
                            return;
                        }
                        if (flag->type != vm->classes[BOOLEAN])
                        {
                            yaoosl_util_throw_generic(vm, context, "Value is no Boolean.");
                            return;
                        }
                        entity->can_collide(flag->additional.flag);
                        yaoosl_instance_dec_ref(flag);
                    });
                auto cancollide_setter_method = yaoosl_method_create("set", nullptr, cancollide_setter_code);
                auto cancollide_property = yaoosl_property_create2("CanCollide", runtime->classes[BOOLEAN], cancollide_getter_method, cancollide_setter_method);
                yaoosl_class_push_property(classhandle, cancollide_property);
            }
        }
#pragma endregion
#pragma region GoingFactory.Texture
        {
        auto classhandle = yaoosl_declare_class(runtime, "GoingFactory.Texture");
        classhandle->callback_data = &gameInstance;


        auto constructor_methodgroup = yaoosl_method_group_create("Texture");
        classhandle->constructor_method_group = constructor_methodgroup;

        /* GoingFactory.Texture.Texture(String path) */ {
            auto constructor_string_code = yaoosl_code_create2(
                [](struct yaoosl* vm, struct yaoosl_context* context, struct yaoosl_method* method, struct yaoosl_instance* self) -> void
                {
                    GameInstance* gameInstance = static_cast<GameInstance*>(self->type->callback_data);
                    yaooslinstancehandle str;
                    if (!(str = yaoosl_context_pop_value(context)))
                    {
                        yaoosl_util_throw_generic(vm, context, "No value found.");
                        return;
                    }
                    if (str->type != vm->classes[STRING])
                    {
                        yaoosl_util_throw_generic(vm, context, "Value is no String.");
                        return;
                    }
                    if (!str->additional.str || str->additional.str[0] == '\0')
                    {
                        yaoosl_util_throw_generic(vm, context, "String is empty.");
                        return;
                    }
                    auto val = gameInstance->resource_manager.load_bitmap(str->additional.str);
                    self->additional.uint64 = val.index();
                    self->fields[0]->additional.d = val.width();
                    self->fields[1]->additional.d = val.height();
                    yaoosl_instance_dec_ref(str);

                    yaoosl_instance_inc_ref(self);
                    yaoosl_context_push_value(context, self);
                });
            auto constructor_string_path_arg = yaoosl_arg_create("path", runtime->classes[STRING]);
            auto constructor_string_method = yaoosl_method_create("Texture", classhandle, constructor_string_code);
            yaoosl_method_push_arg(constructor_string_method, constructor_string_path_arg);
            yaoosl_method_group_push_back(constructor_methodgroup, constructor_string_method);
        }
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
    {
        world::FastNoiseWorldGenerator fnwg;
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        for (int x = 0; x < 10000; x++)
        {
            for (int y = 0; y < 10000; y++)
            {
                float gen = fnwg.generate_chunk(x, y);
                if (gen > max)
                {
                    max = gen;
                }
                if (gen < min)
                {
                    min = gen;
                }
                //std::cout << "{ X: " << x << ", Y: " << y << " } => " << gen << std::endl;
            }
        }
        std::cout << "MIN: " << min << "; MAX: " << max << ";" << std::endl;
    }
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* event_queue;
    ALLEGRO_TIMER* render_timer;
    int init_res = initialize_allegro(display, event_queue, render_timer);
    if (init_res)
    {
        return init_res;
    }

    ResourceManager resources_manager;
    entity::EntityManager entity_manager;
    io::KeyboardTarget keyboard_target;
    World world;
    ux::UXHandler uxhandler(resources_manager);
    al_set_target_backbuffer(display);
    keyboard_target.map(io::EPlayerInteraction::move_up, io::EKey::W);
    keyboard_target.map(io::EPlayerInteraction::move_left, io::EKey::A);
    keyboard_target.map(io::EPlayerInteraction::move_down, io::EKey::S);
    keyboard_target.map(io::EPlayerInteraction::move_right, io::EKey::D);
    keyboard_target.map(io::EPlayerInteraction::mod_a, io::EKey::LSHIFT);
    keyboard_target.map(io::EPlayerInteraction::mod_b, io::EKey::LCTRL);
    keyboard_target.map(io::EPlayerInteraction::mod_c, io::EKey::RSHIFT);
    keyboard_target.map(io::EPlayerInteraction::mod_d, io::EKey::RCTRL);
    keyboard_target.map(io::EPlayerInteraction::trigger_a, io::EKey::SPACE);
    keyboard_target.map(io::EPlayerInteraction::trigger_b, io::EKey::ALT);
    GameInstance game_instance(entity_manager, resources_manager, world, uxhandler);
    entity_manager.onEntityAdded.subscribe([&game_instance](
        entity::EntityManager& entity_manager, entity::EntityManager::EntityAddedEventArgs args) -> void {
            if (args.entity->is_type(entity::EComponent::Render))
            {
                auto renderComponent = args.entity->get_component<entity::RenderComponent>();
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

    auto player = new entity::Player();
    world.set_player(player);
    world.set_viewport(1, 1, DISPLAY_WIDTH - 192, DISPLAY_HEIGHT);
    const int level_size = 50000;
    entity_manager.pool_create(player);
    entity_manager.act_pools();

    auto sim_time = std::chrono::steady_clock::now();
    auto render_time = std::chrono::steady_clock::now();
    bool redraw = false;
    bool simulate = true;
    bool halt_simulation = true;


    auto menu = ux::UXPanel(game_instance, DISPLAY_WIDTH - 192, 0, 192, DISPLAY_HEIGHT);
    uxhandler.push_back(&menu);


    auto build_test = new ux::UXPlaceable(game_instance, menu.x + 4, menu.y + 4, 32, 32);
    menu.push_back(build_test);
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
            uxhandler.mouse_button(game_instance, ev.mouse.x, ev.mouse.y, (ux::EMouseButton)ev.mouse.button, true);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            uxhandler.mouse_move(game_instance, ev.mouse.x, ev.mouse.y, ev.mouse.dx, ev.mouse.dy);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            uxhandler.mouse_button(game_instance, ev.mouse.x, ev.mouse.y, (ux::EMouseButton)ev.mouse.button, false);
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            auto modifier = static_cast<io::EModifier>(ev.keyboard.modifiers);
            auto key = static_cast<io::EKey>(ev.keyboard.keycode);
            if (key == io::EKey::PAUSE)
            {
                halt_simulation = !halt_simulation;
            }
            for (auto it : entity_manager)
            {
                if (!it) { continue; }
                if (it->is_type(entity::EComponent::Keyboard))
                {
                    auto keyboardComponent = it->get_component<entity::KeyboardComponent>();
                    keyboardComponent->key_down(game_instance, key, modifier);
                }
            }
            keyboard_target.key_down(key, modifier);
            world.keydown(key);
            uxhandler.key_down(game_instance, key, modifier);
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            auto modifier = static_cast<io::EModifier>(ev.keyboard.modifiers);
            auto key = static_cast<io::EKey>(ev.keyboard.keycode);
            for (auto it : entity_manager)
            {
                if (!it) { continue; }
                if (it->is_type(entity::EComponent::Keyboard))
                {
                    auto keyboardComponent = it->get_component<entity::KeyboardComponent>();
                    keyboardComponent->key_up(game_instance, key, modifier);
                }
            }
            keyboard_target.key_up(key, modifier);
            uxhandler.key_up(game_instance, key, modifier);
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
            auto sim_time_new = std::chrono::steady_clock::now();
            auto sim_time_delta = (sim_time_new - sim_time);
            float sim_coef = 1.0 / (std::chrono::seconds(1) / sim_time_delta);
            if (std::isinf(sim_coef))
            {
                sim_coef = 1;
            }
            auto temp = std::chrono::seconds(1) / sim_time_delta;
            if (temp <= SIMULATION_TARGET_FPS || redraw)
            {
                { // Call yaoosl_game GameLoop
                    yaooslcontexthandle context = yaoosl_context_create();
                    yaoosl_call_method(runtime, context, yaoosl_game->type->method_groups[0]->methods[0], yaoosl_game);
                    yaoosl_execute(runtime, context);
                    yaoosl_context_destroy(context);
                }
                entity_manager.act_pools();

                simulation_fps = temp;
                simulation_fps_deque.push_back(simulation_fps);
                if (simulation_fps_deque.size() > FPS_DEQUE_MAX)
                {
                    simulation_fps_deque.pop_front();
                }
                simulate = false;
                sim_time = sim_time_new;
#ifndef X39_GF_DISABLE_MT
                game_instance.thread_pool().enqueue([&] {
                    std::vector<std::future<void>> results;
#endif // X39_GF_DISABLE_MT
                    auto start = entity_manager.begin(entity::EComponent::Simulate);
                    auto end = entity_manager.end(entity::EComponent::Simulate);
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
                                game_instance.thread_pool().enqueue([sim_coef, &start, range_start, range_end, &game_instance, range] {
#endif // X39_GF_DISABLE_MT
                                    for (auto i = range_start; i != range_end && i != range; i++)
                                    {
                                        auto it = start + i;
                                        auto simulateComponent = (*it)->get_component<entity::SimulateComponent>();
                                        simulateComponent->simulate(game_instance, sim_coef);
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
                    uxhandler.tick(game_instance);
                    simulate = true;
#ifndef X39_GF_DISABLE_MT
                    });
#endif // X39_GF_DISABLE_MT
            }
        }
        if (redraw && al_is_event_queue_empty(event_queue))
        {
            auto render_time_new = std::chrono::steady_clock::now();
            render_fps = (int)(std::chrono::seconds(1) / (render_time_new - render_time));
            std::stringstream sstream;
            render_fps_deque.push_back(render_fps);
            if (render_fps_deque.size() > FPS_DEQUE_MAX)
            {
                render_fps_deque.pop_front();
            }

            world.render(game_instance);



            // sstream << "Controls:";
            // al_draw_text(resources_manager.font(), al_map_rgb(0, 127, 0), 1, 1 + 10 * 0, 0, sstream.str().c_str());
            // sstream.str("");
            // 
            // sstream << "Up (W) | Left (A) | Down (S) | Right (D) | Trigger (Space|Alt) | Fast (LShift) | Slow (LCTRL)";
            // al_draw_text(resources_manager.font(), al_map_rgb(0, 127, 0), 1, 1 + 10 * 1, 0, sstream.str().c_str());
            // sstream.str("");
            // 
            // sstream << "Note that all numpad buttons (but enter) also have a meaning.";
            // al_draw_text(resources_manager.font(), al_map_rgb(0, 127, 0), 1, 1 + 10 * 2, 0, sstream.str().c_str());
            // sstream.str("");
            // 
            // size_t count = 0;
            // for (auto it : entity_manager)
            // {
            //     if (it) { count++; }
            // }
            // sstream << "Entity Manager Info: alive: " << count << "; size: " << entity_manager.size() << "; capacity: " << entity_manager.capacity();
            // al_draw_text(resources_manager.font(), al_map_rgb(255, 255, 0), 1, DISPLAY_HEIGHT - 1 - 10 * 1, 0, sstream.str().c_str());
            // sstream.str("");
            // 
            // 
            // sstream << "Player Position: { " << player->position().x << ", " << player->position().y << " }";
            // al_draw_text(resources_manager.font(), al_map_rgb(255, 255, 0), DISPLAY_WIDTH / 2 + 1, DISPLAY_HEIGHT - 1 - 10 * 3, 0, sstream.str().c_str());
            // sstream.str("");
            // 
            // sstream << "Player Chunk: { " << player->chunk()->index_x() << ", " << player->chunk()->index_y() << "}";
            // al_draw_text(resources_manager.font(), al_map_rgb(255, 255, 0), DISPLAY_WIDTH / 2 + 1, DISPLAY_HEIGHT - 1 - 10 * 2, 0, sstream.str().c_str());
            // sstream.str("");

            // int v_off = 5;
            // auto color_in_view = al_map_rgb(0, 127, 0);
            // auto color_out_of_view = al_map_rgb(127, 0, 0);
            // for (auto it : entity_manager)
            // {
            //     if (it->is_type(EComponent::Position))
            //     {
            //         auto positionComponent = it->get_component<PositionComponent>();
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

            render_time = render_time_new;

            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            redraw = false;
        }
    }

    al_destroy_timer(render_timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    yaoosl_instance_dec_ref(yaoosl_game);
    yaoosl_destroy_virtualmachine(runtime);
    _CrtMemDumpAllObjectsSince(NULL);
    return 0;
}
