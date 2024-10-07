#ifndef PAUSED_H
#define PAUSED_H

#include <string>
#include <unordered_map>
#include "state.h"
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/engine.hpp>



// The Paused state


namespace godot
{

    class Paused : public State
    {
    private:
        CharacterBody3D* agent;
        float timer = 0;
        float pause_time;
    public:
        void enter() override;
        void exit() override;
        std::string update(double delta) override;
        void init(CharacterBody3D* agent, float pause_time);
    };

}
#endif