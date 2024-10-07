#include "aimshoot.h"


using namespace godot;

void AimShoot::enter() {
	agent->play_sound("res://Sounds/listener_aggroed.wav");
	UtilityFunctions::print("enter aim");
	frame = 0;
	tick = 0;
}

void AimShoot::exit() {
	UtilityFunctions::print("exit aim");
	MeshInstance3D* laser = agent->get_node<MeshInstance3D>("Mesh/Laser");
	laser->set_visible(false);
	MeshInstance3D* dark_laser = agent->get_node<MeshInstance3D>("Mesh/DarkLaser");
	dark_laser->set_visible(false);
}


std::string AimShoot::update(double delta) {
	if (frame > agent->aim_shoot_duration) {
		return "Patrol";
	}
	float dmg = 0.0;
	if (frame > agent->aim_duration) { // Use raycast to check if player is shot
		MeshInstance3D* dark_laser = agent->get_node<MeshInstance3D>("Mesh/DarkLaser");
		agent->play_sound("res://Sounds/laser.wav");
		dark_laser->set_visible(false);
		MeshInstance3D* laser = agent->get_node<MeshInstance3D>("Mesh/Laser");
		laser->set_visible(true);
		laser->set_scale(Vector3(1, 1000, 1));
		laser->set_position(Vector3(0, 0, -500));
		RayCast3D* ray = agent->get_node<RayCast3D>("Mesh/ShootRay");
		if (ray->is_colliding()) {
			Object* collider = ray->get_collider();
			// Set laser to correct length based on collision point
			float dist = ray->get_collision_point().distance_to(agent->get_position()) + 0.1;
			laser->set_scale(Vector3(1, dist, 1));
			laser->set_position(Vector3(0, 0, -dist / 2.0));
			if (Object::cast_to<Player>(collider)) { // If player is being hit, apply damage
				tick++;
			}
		}
	}
	if (tick == agent->ticks_freq) { // After x ticks, call the take_damage function
		player->call("take_damage", agent->damage_per_tick);
		tick = 0;
	}
	if (frame <= agent->aim_rotate_duration) { // Look at player
		if (player->get_position().distance_to(agent->get_position()) < agent->too_close_thresh) {
			// Transition to run state when player gets too close
			return "Run";
		}
		agent->play_sound("res://Sounds/charge.wav");
		Vector3 player_pos = player->get_position();
    	Node3D* mesh = agent->get_node<Node3D>("Mesh");
		player_pos.y = mesh->get_global_position().y;
		mesh->look_at(player_pos);
	} else if (frame <= agent->aim_duration) {
		MeshInstance3D* laser = agent->get_node<MeshInstance3D>("Mesh/DarkLaser");
		laser->set_visible(true);
		laser->set_scale(Vector3(1, 1000, 1));
		laser->set_position(Vector3(0, 0, -500));
		RayCast3D* ray = agent->get_node<RayCast3D>("Mesh/ShootRay");
		if (ray->is_colliding()) {
			Object* collider = ray->get_collider();
			// Set laser to correct length based on collision point
			float dist = ray->get_collision_point().distance_to(agent->get_position()) + 0.1;
			laser->set_scale(Vector3(1, dist, 1));
			laser->set_position(Vector3(0, 0, -dist / 2.0));
		}
	}
    frame++;
	return "AimShoot";
}

void AimShoot::init(Barbarian* agent, Player* player) {
    this->agent = agent;
	this->player = player;
}
