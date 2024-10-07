#ifndef PLAYERSPRING_H
#define PLAYERSPRING_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/classes/spring_arm3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "playercam.h"

namespace godot {

class PlayerSpring : public SpringArm3D {
    GDCLASS(PlayerSpring, SpringArm3D)
   private:
    float mouse_rel_x = 0.0;
    float mouse_rel_y = 0.0;
    float pitch = 0.0;
    float yaw = Math_PI;
    float radius = 5;
    float sens = 0.001;
    PlayerCam* cam;

   protected:
    static void _bind_methods();

   public:
    PlayerSpring();
    ~PlayerSpring();
    void _input(const Ref<InputEvent>& event) override;
    void _ready() override;
    void _process(double delta) override;
    void set_radius(const float p_radius);
    float get_radius() const;
    void set_sens(const float p_sens);
    float get_sens() const;
};

}  // namespace godot
#endif
