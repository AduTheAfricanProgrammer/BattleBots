#ifndef SEEK_H
#define SEEK_H

#include <string>
#include <unordered_map>
#include "state.h"
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "waypoint.h"


// The Seek state


namespace godot
{

    class Seek : public State
    {
    private:

        CharacterBody3D* agent;

       
        float lerp(float a, float b, float c);
    public:
        void enter() override;
        void exit() override;
        std::string update(double delta) override;
        void init(CharacterBody3D* agent);
    };

}
#endif