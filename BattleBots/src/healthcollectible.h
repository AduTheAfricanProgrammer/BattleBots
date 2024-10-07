#ifndef HEALTHCOLLECTIBLE_H
#define HEALTHCOLLECTIBLE_H

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
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/multiplayer_synchronizer.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>

namespace godot {

class HealthCollectible : public Area3D {
    GDCLASS(HealthCollectible, Area3D)
   private:
    bool collectable = true;
    void collect();
    void on_both_players_joined();
    float timer;

   protected:
    static void _bind_methods();

   public:
    HealthCollectible();
    ~HealthCollectible();
    void _ready() override;
    void _process(double delta) override;
    // void _physics_process(double delta) override;
    void on_area_entered(const Area3D* area);
    void on_collected();

    bool can_collect();
};

}  // namespace godot
#endif
