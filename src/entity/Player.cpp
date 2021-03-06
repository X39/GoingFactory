#include "Player.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "EntityManager.h"
#include "Marker.h"
#include "Laser.h"
#include "../ResourceManager.h"
#include "../World.h"
#include "../render-actors/healthbar.h"
#include "../render-actors/draw_bitmap.h"
#include "../simulate-actors/move.h"
#include "../simulate-actors/collision.h"
#include "../simulate-actors/velocitycap.h"
#include "../simulate-actors/slowdown.h"
#include "../simulate-actors/no_health_remove.h"


using namespace x39::goingfactory;
using namespace x39::goingfactory::entity;
Player::Player() : Entity(), m_textures(), m_texture_index(0),
SimulateComponent({
    new actors::simulate::collision(),
    new actors::simulate::move(),
    new actors::simulate::no_health_remove(),
    new actors::simulate::slowdown(10),
    new actors::simulate::velocitycap(500),
    }),
    RenderComponent({
    new actors::render::healthbar(),
    new actors::render::draw_bitmap({ "Textures/Player.png" })
        })
{
    width(16);
    height(16);
}
void Player::interact(GameInstance& game, io::EPlayerInteraction playerInteraction)
{
    float move_coef = 100;
    auto vel = velocity();
    if ((playerInteraction & io::EPlayerInteraction::mod_a) != io::EPlayerInteraction::empty)
    { move_coef *= 2; }
    if ((playerInteraction & io::EPlayerInteraction::mod_b) != io::EPlayerInteraction::empty)
    //{ move_coef *= 0.125; }
    { move_coef *= 0.5; }
    can_collide((playerInteraction & io::EPlayerInteraction::mod_c) == io::EPlayerInteraction::empty);
    if ((playerInteraction & io::EPlayerInteraction::move_left) != io::EPlayerInteraction::empty)
    { vel.x += -move_coef; }
    if ((playerInteraction & io::EPlayerInteraction::move_right) != io::EPlayerInteraction::empty)
    { vel.x += move_coef; }
    if ((playerInteraction & io::EPlayerInteraction::move_up) != io::EPlayerInteraction::empty)
    { vel.y += -move_coef; }
    if ((playerInteraction & io::EPlayerInteraction::move_down) != io::EPlayerInteraction::empty)
    { vel.y += move_coef; }
    if ((playerInteraction & io::EPlayerInteraction::trigger_a) != io::EPlayerInteraction::empty)
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
    if ((playerInteraction & io::EPlayerInteraction::trigger_b) != io::EPlayerInteraction::empty)
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