#ifndef PLAYERCAM_H
#define PLAYERCAM_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/ray_cast3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
namespace godot {

class PlayerCam : public Camera3D {
    GDCLASS(PlayerCam, Camera3D)
   private:
    RayCast3D* ray;
    Node3D* currently_colliding = nullptr;

   protected:
    static void _bind_methods();

   public:
    PlayerCam();
    ~PlayerCam();
    void _ready() override;
    void _physics_process(double delta) override;
    void _process(double delta) override;
};

}  // namespace godot
#endif
