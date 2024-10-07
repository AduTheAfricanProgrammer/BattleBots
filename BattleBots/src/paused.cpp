#include "paused.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Paused::enter() {
	UtilityFunctions::print("enter paused");
}

void Paused::exit() {
	UtilityFunctions::print("exit paused");
}


std::string Paused::update(double delta) {
    String s = agent->call("get_pause_interrupt"); // the Pause state can be interrupted
    if(s != "Paused"){
        timer = 0;
        std::string str = s.utf8().get_data();
        return str;
    }
    timer += delta;
    
    if(timer > pause_time){
        timer = 0;
        agent->call("set_color", Color(0, 1, 0));
        agent->call("play_sound", "res://Sounds/listener_lost_target.wav");
        return "Patrol";
    }
    Vector3 target_velocity = Vector3(0, 0, 0);
    Vector3 new_velocity = agent->get_velocity().move_toward(target_velocity, 0.1);
    agent->set_velocity(new_velocity);
    return "Paused";
}

void Paused::init(CharacterBody3D* agent, float pause_time) {
    this->agent = agent;
    this->pause_time = pause_time;
    
}
