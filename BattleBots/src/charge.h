#ifndef CHARGE_H
#define CHARGE_H

#include <string>
#include <unordered_map>
#include "state.h"
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "waypoint.h"


// The Charge state


namespace godot
{

    class Charge : public State
    {
    private:

        CharacterBody3D* agent;

       
    public:
        void enter() override;
        void exit() override;
        std::string update(double delta) override;
        void init(CharacterBody3D* agent);
    };

}
#endif