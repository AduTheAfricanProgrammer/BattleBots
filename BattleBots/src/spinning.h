#ifndef SPINNING_H
#define SPINNING_H

#include <string>
#include <unordered_map>
#include "state.h"
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "waypoint.h"


// The Spinning state. Used for the blind enemy's attack


namespace godot
{

    class Spinning : public State
    {
    private:
        CharacterBody3D* agent;

        // Spin control variables
        float total_spun;
        bool is_accelerating;
        float timer;
        float spin_timer;
        bool is_decelerating; 
        float mult;
        int spin_damage;
        bool has_played_last; // Used for sound effects.
        float damage_tick_speed; // How often the enemy damages the player while spinning.
    public:
        void enter() override;
        void exit() override;
        std::string update(double delta) override;
        void init(CharacterBody3D* agent, int spin_damage, float damage_tick_speed);
    };

}
#endif