#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/ray_cast3d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/classes/multiplayer_synchronizer.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>
namespace godot {

class Player : public CharacterBody3D {
    GDCLASS(Player, CharacterBody3D)
   private:
    Vector3 velocity;
    float acceleration;
    int ledge_mode;
    float dash_meter;
    float dash_fill_speed;
    float flight_left;
    float gravity;
    float flight_gravity;
    float jump_height;
    float max_speed;
    float air_control;
    float air_drag;
    float dash_time;
    bool inFlight;
    float flight_time;
    float currentDashTime;
    int health;
    int current_health;
    int player_damage;
    Vector3 last_pos;
    Node* communicator;
    RandomNumberGenerator rng = RandomNumberGenerator();
    bool can_play_sound = true;
    bool can_play_music = true;
    Input* input;
    AudioStreamPlayer* soundPlayer;
    AudioStreamPlayer* musicPlayer;
    void play_sound(String path);
    void set_started_dash();
    void set_stopped_dash();
    bool other_player_dashing = false;
    void transmit_sound(String path);
    void set_material(bool turnDash);
    bool just_hit_pad = false;
    bool dash_state = false;
    bool dashable = false;

    float damage_start_timer = 0;
    bool is_emitting_blood = false; // Used to determine when to emit blood. 

    int player_id;
    int other_player_id;
    String player_name;
    bool is_in_singleplayer = true;
    float cooldown_timer = 0;
    bool can_take_damage = true;
    bool is_in_chatbox = false;

    void set_in_chatbox(bool is_in_chatbox);

    void set_knockback(Vector3 direction, int intensity);


   protected:
    static void _bind_methods();

   public:
    // Enum for player state
    enum PlayerState {
        Idle,
        Moving,
        Jumping,
        Dashing,
        Flying,
        Ledge
    };
    Vector3 dir_vectors[3][3] = {
        {Vector3(-1, 0, -1), Vector3(0, 0, -1), Vector3(1, 0, -1)},
        {Vector3(-1, 0, 0), Vector3(0, 0, 0), Vector3(1, 0, 0)},
        {Vector3(-1, 0, 1), Vector3(0, 0, 1), Vector3(1, 0, 1)}};
    PlayerState playerState;
    Player();
    ~Player();
    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;

    void deccel(double delta);
    float get_friction(const float p_speed) const;
    float get_accel_factor() const;

    void set_ledge_mode(const int p_ledge_mode);
    int get_ledge_mode() const;

    void set_acceleration(const float p_acceleration);
    float get_acceleration() const;
    void set_gravity(const float p_gravity);
    float get_gravity() const;

    void set_max_speed(const float p_max_speed);
    float get_max_speed() const;

    void set_air_control(const float p_air_control);
    float get_air_control() const;

    void set_flight_mode();

    void set_flight_time(const float p_flight_time);
    float get_flight_time() const;

    void set_jump_height(const float p_jump_height);
    float get_jump_height() const;

    void set_dash_time(const float p_dash_time);
    float get_dash_time() const;

    void set_air_drag(const float p_air_drag);
    float get_air_drag() const;

    void set_dash_fill_speed(const float p_dash_fill_speed);
    float get_dash_fill_speed() const;

    void set_health(const int p_health);
    int get_health() const;

    void movement(double delta);

    Vector3 get_direction();

    void on_sound_pressed();
    void on_music_pressed();
    void on_jump_pad_entered(float pad_jump_heightx);

    void take_damage(int damage);
    void take_player_damage();


    void set_player_id(int player_id, bool set_sync);  
    void on_player_disconnected(int id);

    void on_body_entered(const Node3D* body);
    void on_player_death();


    void reset();
};

}  // namespace godot
#endif
