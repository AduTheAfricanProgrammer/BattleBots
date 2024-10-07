#include "patrol.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Patrol::enter() {
	UtilityFunctions::print("enter patrol");
}

void Patrol::exit() {
    agent->set_velocity(Vector3(0, 0, 0));
	UtilityFunctions::print("exit patrol");
}

float Patrol::lerp(float a, float b, float c){
    return (1.0 - c) * a + c * b;
}

std::string Patrol::update(double delta) {
    String s = agent->call("get_next_after_patrol");
    if(s != "Patrol"){
        std::string str = s.utf8().get_data();
        return str;
    }
    waypoints->set_global_position(waypoint_position);
    Vector3 old_velocity = agent->get_velocity();
    // If we are close enough to the current waypoint, move to the next one.
    if(abs((agent->get_global_position().x - current_waypoint.x)) < 0.1 && abs((agent->get_global_position().z - current_waypoint.z)) < 0.1){
        current_waypoint_index++;
        if(current_waypoint_index >= num_waypoints){
            
            current_waypoint_index = 0;
        }
        current_waypoint = ((Node3D*) (waypoints->get_child(current_waypoint_index)))->get_global_position();
    }
    // Make the agent move towards the current waypoint.
    nav_agent->set_target_position(current_waypoint);
    Vector3 current_location = agent->get_global_position();
    Vector3 next_location = nav_agent->get_next_path_position();
    Vector3 target_velocity = (next_location - current_location).normalized() * patrol_speed;
    Vector3 new_velocity = agent->get_velocity().move_toward(target_velocity, 0.1);
    agent->set_velocity(new_velocity);
    
    Vector3 direction = ( agent->get_global_position() - nav_agent->get_next_path_position()).normalized();
    float target_angle = atan2(direction.x, direction.z);
    Node3D* mesh = agent->get_node<Node3D>("Mesh");
    float current_angle = mesh->get_global_rotation().y;
    mesh->set_global_rotation(Vector3(0, lerp(current_angle, target_angle, 0.05), 0));
    // Convert s into a std::string
    std::string str = s.utf8().get_data();
    return str;
}

void Patrol::init(CharacterBody3D* agent, NavigationAgent3D* nav_agent, Node3D* waypoints, float patrol_speed) {
    this->agent = agent;
    this->nav_agent = nav_agent;
    this->patrol_speed = patrol_speed;
    this->waypoints = waypoints;
    Waypoint *start_waypoint = memnew(Waypoint);
    waypoints->add_child(start_waypoint);
    start_waypoint->set_global_position(agent->get_global_position());
    num_waypoints = waypoints->get_child_count();
    current_waypoint = ((Node3D*) (waypoints->get_child(current_waypoint_index)))->get_global_position();
    waypoint_position = waypoints->get_global_position();
}
