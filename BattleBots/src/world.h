#ifndef WORLD_H
#define WORLD_H

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
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/multiplayer_synchronizer.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>
#include <unordered_map>


namespace godot {

class World : public Node3D {
    GDCLASS(World, Node3D)
   private:
    void set_in_chatbox(bool is_in_chatbox);
    void on_player_connected(int player_id);
    void on_peer_disconnected(int id);
    void start_other_player(int other_id);
    void get_flight_collected(String playerName);
    void on_player_death();
    void show_win_screen();
    void on_singleplayer_win();
    void set_knockback(int player_id, Vector3 direction);
    void get_health_collected(String playerName);
    Node* communicator;
    Ref<PackedScene> player_scene;
    Node3D* player1;
    Node3D* player2;
    bool player1CanHurt;
    bool player2CanHurt;
   protected:
    static void _bind_methods();

   public:

    World();
    ~World();
    void _ready() override;
    void _process(double delta) override;
};

}  // namespace godot
#endif

