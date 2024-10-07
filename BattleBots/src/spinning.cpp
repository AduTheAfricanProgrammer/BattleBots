#include "spinning.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Spinning::enter() {
	UtilityFunctions::print("enter spinning");
}

void Spinning::exit() {
	UtilityFunctions::print("exit spinning");
}


std::string Spinning::update(double delta) {
    Vector3 target_velocity = Vector3(0, agent->get_velocity().y, 0);
    Vector3 new_velocity = agent->get_velocity().move_toward(target_velocity, 0.05);
    agent->set_velocity(new_velocity);
    if(is_accelerating){
        mult += 3*delta;
        if(mult >= 1){
            is_accelerating = false;
        } 
    }
    else if(!is_accelerating && !is_decelerating){
        timer += delta;
        if(timer >= 1){
            timer = 0;
            is_decelerating = true;
        }
    }
    else if(is_decelerating){
        mult -= delta;
        if(mult <= 0){
            is_decelerating = false;
            is_accelerating = true;
            mult = 0;
            agent->call("set_blade_color", Color(0.4, 0, 0));
            // Used just for timing the sound effects. 
            total_spun = Math_PI;
            has_played_last = false;
            return "Paused";
        }
        if(mult <= 0.2 && !has_played_last){
                 agent->call("play_blade", 0.7);
                 has_played_last = true;
        }
    }
    Node3D* mesh = agent->get_node<Node3D>("Mesh");
    mesh->rotate_y(mult * 25 * delta);
    total_spun += mult * 25 * delta;
    if(total_spun >= Math_PI * 2){
        total_spun = 0;
        agent->call("play_blade", 0.5 + mult);
    }
    spin_timer += delta;
    if(spin_timer * damage_tick_speed >= (0.4 / (1 + mult))){
        spin_timer = 0;
        TypedArray<Node3D> bodies = agent->get_node<Area3D>("FeelArea")->get_overlapping_bodies();
        for (int i = 0; i < bodies.size(); i++) {
            Node3D* body = (Area3D*)((Object*)bodies[i]);
            if(body->has_meta("is_player")){
                body->call("take_damage", spin_damage);
            }
        }
    }
    return "Spinning";
}

void Spinning::init(CharacterBody3D* agent, int spin_damage, float damage_tick_speed) {
    this->agent = agent;
    this->spin_damage = spin_damage;
    mult = 0;
    total_spun = Math_PI;
    has_played_last = false;
    this->damage_tick_speed = damage_tick_speed;
    is_accelerating = true;
}
