#ifndef LISTENER_H
#define LISTENER_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <unordered_map>
#include "waypoint.h"
#include "state.h"
#include "patrol.h"
#include "seek.h"
#include "charge.h"
#include "paused.h"
#include "spinning.h"
#include "statemachine.h"
namespace godot {

class Listener : public CharacterBody3D {
    GDCLASS(Listener, CharacterBody3D)
   private:
    NavigationAgent3D* nav_agent;
    Node3D* waypoints;
    float patrol_speed; // How fast the enemy moves when patrolling.
    float seek_speed; // How fast the enemy moves when seeking a target. 
    float charge_speed; // How fast the enemy moves when charging at the player.
    bool is_first_frame = true; // Used to only start the navigation agent when the mesh is properly initialized. 
    int hitpoints = 1; //number of hitpoints the enemy currently has.
    int num_waypoints;
    int current_waypoint_index = 0;
    Vector3 current_waypoint;
    Vector3 waypoint_position;
    StateMachine* state_machine;
    std::unordered_map<std::string, std::shared_ptr<State>> states;

    void do_seek(double delta);
    void do_charge(double delta);
    String get_next_after_patrol();
    String get_next_after_seek();
    String get_next_after_charge();
    String get_pause_interrupt();

    float sensitivity; // How sensitive the agent is to sounds.

    float gravity;

    bool heard_soft = false; // When the agent hears a quiet sound.
    bool stop_seeking = false; // Used to tell the agent to go back to patrol mode.  
    bool heard_loud = false; // When the agent hears a loud sound (player nearby).
    bool stop_charging = false; // Used to tell the agent to stop charging.
    bool can_feel_player = false; // When the player is close enough to be felt.
    bool can_be_hurt = false;
    bool player_dash = false;
   
    Vector3 seek_position;
    float lerp(float a, float b, float c);
    float get_distance(Vector3 a, Vector3 b);
    void set_color(Color color); // For the body
    void set_blade_color(Color color); // For the blade

    void on_reset_color();
    bool can_play_sound = true;
    AudioStreamPlayer3D* sound_player; // For the bot noises
    AudioStreamPlayer3D* blade_player; // For the blade noises
    void play_sound(String path);
    void play_blade(float pitch);
    void on_sound_pressed();


    void on_sound_heard(Vector3 position, float intensity); // When the agent hears a sound.
    void on_feel_entered(const Node3D* body); // When the player enters the "feeling range" of the agent. 
    void on_feel_exited(const Node3D* body);  // When the player exits the "feeling range" of the agent. 
    void on_body_entered(const Node3D* body);
    void on_body_exited(const Node3D* body);

    
    int damage; 

    float feel_area_radius; // The radius of the area that the agent can feel the player in to begin spinning.
    float damage_tick_speed; // How often the player takes damage when the agent is spinning. Higher numbers -> faster damage.

    Color previous_color; // The color of the body before it was changed by the player.

    CPUParticles3D* body_particles; // The particles that come out of the body when the agent changes state. 

    bool play_blade_noise; // Needs to be true for the blade sound effect to play. 

    bool play_death_noise;

   protected:
    static void _bind_methods();

   public:
    Listener();
    ~Listener();
    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;
    void set_patrol_speed(const float p_patrol_speed);
    float get_patrol_speed() const;
    void set_seek_speed(const float p_seek_speed);
    float get_seek_speed() const;
    void set_charge_speed(const float p_charge_speed);
    float get_charge_speed() const;
    void set_sensitivity(const float p_sensitivity);
    float get_sensitivity() const;
    void set_gravity(const float p_gravity);
    float get_gravity() const;
    void set_damage(const int p_damage);
    int get_damage() const; 
    void set_feel_area_radius(const float p_feel_area_radius);
    float get_feel_area_radius() const;
    void set_blade_noise(const bool p_play_blade_noise);
    bool get_blade_noise() const;
    void set_damage_tick_speed(const float p_damage_tick_speed);
    float get_damage_tick_speed() const;
    void can_die(const bool p_dash_state);
};

}  // namespace godot
#endif
