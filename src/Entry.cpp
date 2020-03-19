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

#include "ResourceManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Movable.h"
#include "Player.h"
#include "EKey.h"
#include "EModifier.h"

int DISPLAY_WIDTH;
int DISPLAY_HEIGHT;

const float RENDER_FPS = 60;
const float SIMULATION_FPS = 30;
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

	font = al_load_ttf_font("pirulen.ttf", 72, 0);
	if (!font)
	{
		fprintf(stderr, "Could not load 'pirulen.ttf'.\n");
		font = al_create_builtin_font(); //Load fallback
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(render_timer));
	al_register_event_source(event_queue, al_get_timer_event_source(simulation_timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_keyboard_event_source());

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
	entity_manager.onEntityAdded.subscribe([&resources_manager](
		x39::goingfactory::EntityManager& entity_manager, x39::goingfactory::EntityManager::EntityAddedEventArgs args) -> void {
			if (args.entity->is_type(x39::goingfactory::EComponent::Render))
			{
				auto renderComponent = args.entity->get_component<x39::goingfactory::RenderComponent>();
				renderComponent->render_init(resources_manager);
			}
		});
	entity_manager.push_back(std::make_shared<x39::goingfactory::entity::Player>());
	auto movable = std::make_shared<x39::goingfactory::entity::Movable>();
	movable->pos({ DISPLAY_WIDTH / 2.0f, DISPLAY_HEIGHT / 2.0f });
	entity_manager.push_back(movable);

	auto old_time = al_get_time();
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
				if (it->is_type(x39::goingfactory::EComponent::Keyboard))
				{
					auto keyboardComponent = it->get_component<x39::goingfactory::KeyboardComponent>();
					keyboardComponent->key_down(entity_manager, key, modifier);
				}
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			auto modifier = static_cast<x39::goingfactory::io::EModifier>(ev.keyboard.modifiers);
			auto key = static_cast<x39::goingfactory::io::EKey>(ev.keyboard.keycode);
			for (auto it : entity_manager)
			{
				if (it->is_type(x39::goingfactory::EComponent::Keyboard))
				{
					auto keyboardComponent = it->get_component<x39::goingfactory::KeyboardComponent>();
					keyboardComponent->key_up(entity_manager, key, modifier);
				}
			}
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
		if (redraw && al_is_event_queue_empty(event_queue))
		{
			auto new_time = al_get_time();
			auto fpsString = std::to_string((int)(1 / (new_time - old_time)));
			al_draw_text(font, al_map_rgb(255, 0, 0), 0, 0, 0, fpsString.c_str());
			old_time = new_time;

			for (auto it : entity_manager)
			{
				if (it->is_type(x39::goingfactory::EComponent::Render))
				{
					auto renderComponent = it->get_component<x39::goingfactory::RenderComponent>();
					renderComponent->render(resources_manager);
				}
			}

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
			redraw = false;
		}
		if (simulate && al_is_event_queue_empty(event_queue))
		{
			for (auto it : entity_manager)
			{
				if (it->is_type(x39::goingfactory::EComponent::Simulate))
				{
					auto simulateComponent = it->get_component<x39::goingfactory::SimulateComponent>();
					simulateComponent->simulate(entity_manager);
				}
			}
			simulate = false;
		}
	}

	al_destroy_font(font);
	al_destroy_timer(render_timer);
	al_destroy_timer(simulation_timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
