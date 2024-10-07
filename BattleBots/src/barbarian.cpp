#include "barbarian.h"
#include "patrol.h"
#include "aimshoot.h"
#include "run.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void Barbarian::_ready() {
   
    set_process(true);
    nav_agent = memnew(NavigationAgent3D);
    add_child(nav_agent);
    waypoints = get_node<Node3D>("Waypoints");

    // Setup sound
    Button* sound = get_parent()->get_node<Control>("GUI")->get_node<Button>("SoundButton");
    sound->connect("pressed", Callable(this, "on_sound_pressed"));
    sound_player = memnew(AudioStreamPlayer3D);
    sound_player->set_max_polyphony(5);
    add_child(sound_player);
   
    player_dash = false;

    // Create a new waypoint at the start position
    Waypoint *start_waypoint = memnew(Waypoint);
    waypoints->add_child(start_waypoint);
    start_waypoint->set_global_position(get_global_position());
    num_waypoints = waypoints->get_child_count();
    current_waypoint = ((Node3D*) (waypoints->get_child(current_waypoint_index)))->get_global_position();
    waypoint_position = waypoints->get_global_position();

    Area3D* hit_box = get_node<Area3D>("HitBoxBarb");
    hit_box->connect("body_entered", Callable(this, "on_body_entered"));
    hit_box->connect("body_exited", Callable(this, "on_body_exited"));

    if(!Engine::get_singleton()->is_editor_hint()) {
        // Connect relevant signals for vision area
        Area3D* vision_area = get_node<Area3D>("Mesh/Area");
        vision_area->connect("body_entered", Callable(this, "on_vision_entered"));
        vision_area->connect("body_exited", Callable(this, "on_vision_exited"));

        // Setup hashmap of states, initialize states and instance a state machine
        std::shared_ptr<Patrol> patrol = std::make_shared<Patrol>();
        patrol->init(this, nav_agent, waypoints, patrol_speed);
        states["Patrol"] = patrol;

        std::shared_ptr<AimShoot> aim_shoot = std::make_shared<AimShoot>();
        player = get_parent()->get_node<Player>("Player");
        player->connect("dash_state", Callable(this, "can_die"));
        aim_shoot->init(this, player);
        states["AimShoot"] = aim_shoot;

        std::shared_ptr<Run> run = std::make_shared<Run>();
        run->init(this, player, nav_agent);
        states["Run"] = run;

        state_machine = memnew(StateMachine(states, "Patrol"));
    }

}

void Barbarian::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_patrol_speed", "patrol_speed"), &Barbarian::set_patrol_speed);
    ClassDB::bind_method(D_METHOD("get_patrol_speed"), &Barbarian::get_patrol_speed);
    ClassDB::add_property("Barbarian", PropertyInfo(Variant::FLOAT, "patrol_speed"), "set_patrol_speed", "get_patrol_speed");

    ClassDB::bind_method(D_METHOD("get_next_after_patrol"), &Barbarian::get_next_after_patrol);
    
    ClassDB::bind_method(D_METHOD("on_vision_entered", "body"), &Barbarian::on_vision_entered);
    ClassDB::bind_method(D_METHOD("on_vision_exited", "body"), &Barbarian::on_vision_exited);
    ClassDB::bind_method(D_METHOD("on_body_entered", "body"), &Barbarian::on_body_entered);
    ClassDB::bind_method(D_METHOD("on_body_exited", "body"), &Barbarian::on_body_exited);

    ClassDB::bind_method(D_METHOD("can_die", "dash"), &Barbarian::can_die);

    
    ClassDB::bind_method(D_METHOD("play_sound", "path"), &Barbarian::play_sound);
    ClassDB::bind_method(D_METHOD("on_sound_pressed"), &Barbarian::on_sound_pressed);

    ClassDB::bind_method(D_METHOD("set_aim_shoot_duration", "aim_shoot_duration"), &Barbarian::set_aim_shoot_duration);
    ClassDB::bind_method(D_METHOD("get_aim_shoot_duration"), &Barbarian::get_aim_shoot_duration);
    ClassDB::add_property("Barbarian", PropertyInfo(Variant::INT, "aim_shoot_duration"), "set_aim_shoot_duration", "get_aim_shoot_duration");

    ClassDB::bind_method(D_METHOD("set_aim_duration", "aim_duration"), &Barbarian::set_aim_duration);
    ClassDB::bind_method(D_METHOD("get_aim_duration"), &Barbarian::get_aim_duration);
    ClassDB::add_property("Barbarian", PropertyInfo(Variant::INT, "aim_duration"), "set_aim_duration", "get_aim_duration");

    ClassDB::bind_method(D_METHOD("set_aim_rotate_duration", "aim_rotate_duration"), &Barbarian::set_aim_rotate_duration);
    ClassDB::bind_method(D_METHOD("get_aim_rotate_duration"), &Barbarian::get_aim_rotate_duration);
    ClassDB::add_property("Barbarian", PropertyInfo(Variant::INT, "aim_rotate_duration"), "set_aim_rotate_duration", "get_aim_rotate_duration");

    ClassDB::bind_method(D_METHOD("set_too_close_thresh", "too_close_thresh"), &Barbarian::set_too_close_thresh);
    ClassDB::bind_method(D_METHOD("get_too_close_thresh"), &Barbarian::get_too_close_thresh);
    ClassDB::add_property("Barbarian", PropertyInfo(Variant::INT, "too_close_thresh"), "set_too_close_thresh", "get_too_close_thresh");

    ClassDB::bind_method(D_METHOD("set_damage_per_tick", "damage_per_tick"), &Barbarian::set_damage_per_tick);
    ClassDB::bind_method(D_METHOD("get_damage_per_tick"), &Barbarian::get_damage_per_tick);
    ClassDB::add_property("Barbarian", PropertyInfo(Variant::INT, "damage_per_tick"), "set_damage_per_tick", "get_damage_per_tick");

    ClassDB::bind_method(D_METHOD("set_ticks_freq", "ticks_freq"), &Barbarian::set_ticks_freq);
    ClassDB::bind_method(D_METHOD("get_ticks_freq"), &Barbarian::get_ticks_freq);
    ClassDB::add_property("Barbarian", PropertyInfo(Variant::INT, "ticks_freq"), "set_ticks_freq", "get_ticks_freq");
    
}

Barbarian::Barbarian() {
}

Barbarian::~Barbarian() {
}

void Barbarian::set_patrol_speed(const float p_patrol_speed) {
    patrol_speed = p_patrol_speed;
}

float Barbarian::get_patrol_speed() const {
    return patrol_speed;
}

void Barbarian::_process(double delta) {
}

void Barbarian::_physics_process(double delta) {
    if(Engine::get_singleton()->is_editor_hint()){
        return;
    }
    if(is_first_frame){
        is_first_frame = false; // Due to navigation agent not being properly registered on the first frame.
        return; 
    }
    state_machine->update(delta);
    move_and_slide();
}

String Barbarian::get_next_after_patrol() {
    if (might_see_player) { // Use raycast to check if anything is blocking sightline to player
    	Node3D* mesh = get_node<Node3D>("Mesh");
        RayCast3D* ray = memnew(RayCast3D);
        ray->set_target_position(Vector3(mesh->to_local(player->get_position())));
        mesh->add_child(ray);
        ray->force_raycast_update();
        if (ray->is_colliding()) {
            Object* collider = ray->get_collider();
            if (Object::cast_to<Player>(collider)) {
                return "AimShoot";
            }
        }
        ray->queue_free();
        if (player->get_position().distance_to(get_position()) < too_close_thresh) {
			// Transition to run state when player gets too close, but only if player is visible
			return "Run";
		}
    }
    return "Patrol";  
}

void Barbarian::on_vision_entered(const Node3D* body) {
    // If the player enters the vision area
    if(body->has_meta("is_player")){
        might_see_player = true;
    }
}

void Barbarian::on_vision_exited(const Node3D* body) {
    // If the player enters the vision area
    if(body->has_meta("is_player")){
        might_see_player = false;
    }
}

void Barbarian::on_body_entered(const Node3D* body) {
    if (body->has_meta("is_player") && player_dash) {
        body->get_parent()->call_deferred("remove_child", this);
        get_parent()->get_node<Player>("Player")->call("play_sound", "res://Sounds/robot_death.mp3");
        hide();
        set_physics_process(false);
        set_process_input(false);
        set_process(false);
        
        
    }
}

void Barbarian::on_body_exited(const Node3D* body) {
    //this = Null;
}

void Barbarian::can_die(const bool dash) {
    if (dash) {
        player_dash = true;
    } else {
        player_dash = false; 
    }
}
void Barbarian::play_sound(String path){
    if (can_play_sound) {
        Ref<AudioStream> stream = ResourceLoader::get_singleton()->load(path);
        sound_player->set_stream(stream);
        sound_player->set_pitch_scale(1);
        sound_player->play();
    }
}

void Barbarian::on_sound_pressed(){
    can_play_sound = !can_play_sound;
}

void Barbarian::set_aim_shoot_duration(const int p_aim_shoot_duration) {
    aim_shoot_duration = p_aim_shoot_duration;
}

int Barbarian::get_aim_shoot_duration() const {
    return aim_shoot_duration;
}

void Barbarian::set_aim_duration(const int p_aim_duration) {
    aim_duration = p_aim_duration;
}

int Barbarian::get_aim_duration() const {
    return aim_duration;
}

void Barbarian::set_aim_rotate_duration(const int p_aim_rotate_duration) {
    aim_rotate_duration = p_aim_rotate_duration;
}

int Barbarian::get_aim_rotate_duration() const {
    return aim_rotate_duration;
}

void Barbarian::set_too_close_thresh(const int p_too_close_thresh) {
    too_close_thresh = p_too_close_thresh;
}

int Barbarian::get_too_close_thresh() const {
    return too_close_thresh;
}

void Barbarian::set_damage_per_tick(const int p_damage_per_tick) {
    damage_per_tick = p_damage_per_tick;
}

int Barbarian::get_damage_per_tick() const {
    return damage_per_tick;
}

void Barbarian::set_ticks_freq(const int p_ticks_freq) {
    ticks_freq = p_ticks_freq;
}

int Barbarian::get_ticks_freq() const {
    return ticks_freq;
}

