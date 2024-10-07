#include "run.h"


using namespace godot;

void Run::enter() {
	agent->play_sound("res://Sounds/curious.wav");
	UtilityFunctions::print("enter run");
}

void Run::exit() {
	UtilityFunctions::print("exit run");
}


std::string Run::update(double delta) {
	if (player->get_position().distance_to(agent->get_position()) > agent->too_close_thresh) {
		return "Patrol";
	}
	// Move away from player
	nav_agent->set_target_position(agent->get_position() + (agent->get_position() - player->get_position()).normalized());
    Vector3 current_location = agent->get_global_position();
    Vector3 next_location = nav_agent->get_next_path_position();
    Vector3 target_velocity = (next_location - current_location).normalized() * agent->get_patrol_speed();
    Vector3 new_velocity = agent->get_velocity().move_toward(target_velocity, 0.1);
    agent->set_velocity(new_velocity);
	Vector3 player_pos = player->get_position();
	Node3D* mesh = agent->get_node<Node3D>("Mesh");
	player_pos.y = mesh->get_global_position().y;
	mesh->look_at(player_pos);
	return "Run";
}

void Run::init(Barbarian* agent, Player* player, NavigationAgent3D* nav_agent) {
    this->agent = agent;
	this->player = player;
	this->nav_agent = nav_agent;
}
