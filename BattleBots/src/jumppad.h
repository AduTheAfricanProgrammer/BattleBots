#ifndef JUMPPAD_H
#define JUMPPAD_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
namespace godot {

class JumpPad : public StaticBody3D {
    GDCLASS(JumpPad, StaticBody3D)
   private:
    float jump_height = 15;

   protected:
    static void _bind_methods();

   public:
    JumpPad();
    ~JumpPad();
    void _ready() override;
    void _process(double delta) override;
    // void _physics_process(double delta) override;
    void on_body_entered(const Node3D* body);

    void set_jump_height(const float p_height);
    float get_jump_height() const;
};

}  // namespace godot
#endif
