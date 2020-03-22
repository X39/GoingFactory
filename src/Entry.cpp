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

#include "ThreadPool.h"

#include "ResourceManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Asteroid.h"
#include "Player.h"
#include "EKey.h"
#include "EModifier.h"
#include "GameInstance.h"
#include "World.h"
#include "KeyboardTarget.h"

int DISPLAY_WIDTH;
int DISPLAY_HEIGHT;

const float RENDER_FPS = 60;
const float SIMULATION_FPS = 60;
int initialize_allegro(ALLEGRO_DISPLAY*& display, ALLEGRO_EVENT_QUEUE*& event_queue, ALLEGRO_TIMER*& render_timer, ALLEGRO_TIMER*& simulation_timer, ALLEGRO_FONT*& font)
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

	simulation_timer = al_create_timer(1.0 / SIMULATION_FPS);
	if (!simulation_timer)
	{
		fprintf(stderr, "failed to create simulation_timer!\n");
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
	al_register_event_source(event_queue, al_get_timer_event_source(simulation_timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_keyboard_event_source());


	// al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

	al_start_timer(render_timer);
	al_start_timer(simulation_timer);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	return 0;
}
int main()
{
	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* event_queue;
	ALLEGRO_TIMER* render_timer;
	ALLEGRO_TIMER* simulation_timer;
	ALLEGRO_FONT* font;
	int init_res = initialize_allegro(display, event_queue, render_timer, simulation_timer, font);
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
	keyboard_target.map(x39::goingfactory::io::EPlayerInteraction::trigger_a, x39::goingfactory::io::EKey::SPACE);
	x39::goingfactory::GameInstance game_instance(entity_manager, resources_manager, world);
	entity_manager.onEntityAdded.subscribe([&game_instance](
		x39::goingfactory::EntityManager& entity_manager, x39::goingfactory::EntityManager::EntityAddedEventArgs args) -> void {
			if (args.entity->is_type(x39::goingfactory::EComponent::Render))
			{
				auto renderComponent = args.entity->get_component<x39::goingfactory::RenderComponent>();
				renderComponent->render_init(game_instance);
			}
		});
	auto player = new x39::goingfactory::entity::Player();
	world.set_player(player);
	const int viewport_offset = 32;
	world.set_viewport(viewport_offset, viewport_offset, DISPLAY_WIDTH - viewport_offset * 2, DISPLAY_HEIGHT - viewport_offset * 2);
	const size_t level_size = 5000;
	world.set_level(level_size, level_size);
	player->position({(level_size / 2) + (rand() % 1000 - 500), (level_size / 2) + (rand() % 1000 - 500) });
	entity_manager.pool_create(player);

	for (int i = 0; i < 500; i++)
	{
		auto asteroid = new x39::goingfactory::entity::Asteroid();
		asteroid->position({ rand() % level_size, rand() % level_size });
		asteroid->velocity({ (rand() % 20) / 20.0f, (rand() % 20) / 20.0f });
		entity_manager.pool_create(asteroid);
	}

	entity_manager.act_pools();

	auto old_frame_time = al_get_time();
	auto old_sim_time = al_get_time();
	int last_sim_fps = 0;
	bool redraw = false;
	bool simulate = false;
	bool mouseDown = false;
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
			else if (ev.timer.source == simulation_timer)
			{
				simulate = true;
			}
		}
		if (simulate)
		{
			std::vector<std::future<void>> results;
			auto start = entity_manager.begin(x39::goingfactory::EComponent::Simulate);
			auto end = entity_manager.end(x39::goingfactory::EComponent::Simulate);
			const size_t handle_amount = 100;
			size_t range = end - start;
			size_t tasks = range / handle_amount + (range / handle_amount == 0 ? 0 : 1);
			for (; tasks > 0; tasks--)
			{
				auto range_start = (tasks - 1) * handle_amount;
				auto range_end = tasks * handle_amount;
				results.emplace_back(
					game_instance.thread_pool().enqueue([&start, range_start, range_end, &game_instance, range] {
						for (auto i = range_start; i != range_end && i != range; i++)
						{
							auto it = start + i;
							auto simulateComponent = (*it)->get_component<x39::goingfactory::SimulateComponent>();
							simulateComponent->simulate(game_instance);
						}
						}
					)
				);
			}
			keyboard_target.entity_interact(game_instance);
			for (auto&& result : results) { result.get(); }

			auto new_time = al_get_time();
			last_sim_fps = (int)(1 / (new_time - old_sim_time));
			old_sim_time = new_time;
			simulate = false;
		}
		if (redraw)// && al_is_event_queue_empty(event_queue))
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

			sstream << "Up (W) | Left (A) | Down (S) | Right (D) | Shoot (Space) | Speed x2 (LShift) | Speed x0.5 (LCTRL)";
			al_draw_text(font, al_map_rgb(0, 127, 0), 1, 1 + 10 * 1, 0, sstream.str().c_str());
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

			old_frame_time = new_time;

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
			redraw = false;
		}
		entity_manager.act_pools();
	}

	al_destroy_font(font);
	al_destroy_timer(render_timer);
	al_destroy_timer(simulation_timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
