#ifndef BARBARIAN_H
#define BARBARIAN_H

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
#include "statemachine.h"
#include "player.h"
namespace godot {

class Barbarian : public CharacterBody3D {
    GDCLASS(Barbarian, CharacterBody3D)
   private:
    Player *player;
    NavigationAgent3D* nav_agent;
    Node3D* waypoints;
    bool player_dash = false;
    AudioStreamPlayer3D* sound_player;
    float patrol_speed;
    bool is_first_frame = true;
    bool might_see_player = false;
    bool can_play_sound = true;
    int num_waypoints;
    int current_waypoint_index = 0;
    Vector3 current_waypoint;
    Vector3 waypoint_position;
    StateMachine* state_machine;
    std::unordered_map<std::string, std::shared_ptr<State>> states;
    void do_patrol();
    String get_next_after_patrol();
   protected:
    static void _bind_methods();

   public:
    int aim_shoot_duration = 160;
    int aim_duration = 120;
    int aim_rotate_duration = 100;
    int too_close_thresh = 10;
    int damage_per_tick = 5;
    int ticks_freq = 5;

    Barbarian();
    ~Barbarian();
    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;
    void set_patrol_speed(const float p_patrol_speed);
    float get_patrol_speed() const;
    void on_vision_entered(const Node3D* body);
    void on_vision_exited(const Node3D* body);
    void on_body_entered(const Node3D* body);
    void on_body_exited(const Node3D* body);
    void can_die(const bool p_dash_state);

    void play_sound(String path);
    void on_sound_pressed();

    void set_aim_shoot_duration(const int p_aim_shoot_duration);
    int get_aim_shoot_duration() const;
    void set_aim_duration(const int p_aim_duration);
    int get_aim_duration() const;
    void set_aim_rotate_duration(const int p_aim_rotate_duration);
    int get_aim_rotate_duration() const;
    void set_too_close_thresh(const int p_too_close_thresh);
    int get_too_close_thresh() const;
    void set_damage_per_tick(const int p_damage_per_tick);
    int get_damage_per_tick() const;
    void set_ticks_freq(const int p_ticks_freq);
    int get_ticks_freq() const;
};

}  // namespace godot
#endif

