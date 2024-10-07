#ifndef PATROL_H
#define PATROL_H

#include <string>
#include <unordered_map>
#include "state.h"
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/engine.hpp>

#include "waypoint.h"


// The Patrol state


namespace godot
{

    class Patrol : public State
    {
    private:
        Node3D* waypoints;
        int num_waypoints;
        int current_waypoint_index = 0;
        Vector3 current_waypoint;
        Vector3 waypoint_position;
        CharacterBody3D* agent;
        NavigationAgent3D* nav_agent;
        float patrol_speed;
        float lerp(float a, float b, float c);
    public:
        void enter() override;
        void exit() override;
        std::string update(double delta) override;
        void init(CharacterBody3D* agent, NavigationAgent3D* nav_agent, Node3D* waypoints, float patrol_speed);
    };

}
#endif