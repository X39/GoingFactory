#include "Player.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "ResourceManager.h"
#include "World.h"
#include "EntityManager.h"
#include "Marker.h"
#include "Laser.h"
#include "render-actors/healthbar.h"
#include "render-actors/draw_bitmap.h"
#include "simulate-actors/move.h"
#include "simulate-actors/collision.h"
#include "simulate-actors/slowdown.h"
#include "simulate-actors/no_health_remove.h"


x39::goingfactory::entity::Player::Player() : Entity(), m_textures(), m_texture_index(0),
SimulateComponent({
    new actors::simulate::collision(),
    new actors::simulate::move(),
    new actors::simulate::no_health_remove(),
    new actors::simulate::slowdown(0.9)
    }),
    RenderComponent({
    new actors::render::healthbar(),
    new actors::render::draw_bitmap({ "Textures/Player.png" })
        })
{
    width(16);
    height(16);
}
void x39::goingfactory::entity::Player::interact(GameInstance& game, io::EPlayerInteraction playerInteraction)
{
    float move_coef = 60;
    auto vel = velocity();
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::mod_a) != x39::goingfactory::io::EPlayerInteraction::empty)
    { move_coef *= 2; }
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::mod_b) != x39::goingfactory::io::EPlayerInteraction::empty)
    //{ move_coef *= 0.125; }
    { move_coef *= 0.05; }
    can_collide((playerInteraction & x39::goingfactory::io::EPlayerInteraction::mod_c) == x39::goingfactory::io::EPlayerInteraction::empty);
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_left) != x39::goingfactory::io::EPlayerInteraction::empty)
    { vel.x += -move_coef; }
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_right) != x39::goingfactory::io::EPlayerInteraction::empty)
    { vel.x += move_coef; }
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_up) != x39::goingfactory::io::EPlayerInteraction::empty)
    { vel.y += -move_coef; }
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::move_down) != x39::goingfactory::io::EPlayerInteraction::empty)
    { vel.y += move_coef; }
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::trigger_a) != x39::goingfactory::io::EPlayerInteraction::empty)
    {
        double d = al_get_time();
        if (d - m_last_trigger_a > 0.25)
        {
            m_last_trigger_a = d;
            auto laser = new Laser();
            laser->position(position());
            auto vel = velocity();
            auto len = vel.length();
            vel.normalize();
            vel *= 100 + len;
            laser->velocity(vel);
            laser->no_collide(this);
            game.entity_manager.pool_create(laser);
        }
    }
    if ((playerInteraction & x39::goingfactory::io::EPlayerInteraction::trigger_b) != x39::goingfactory::io::EPlayerInteraction::empty)
    {
        double d = al_get_time();
        if (d - m_last_trigger_b > 1)
        {
            m_last_trigger_b = d;
            auto marker = new Marker();
            marker->position(position());
            game.entity_manager.pool_create(marker);
        }
    }
    if (vel.length_squared() > 0)
    {
        velocity(vel);
    }
}