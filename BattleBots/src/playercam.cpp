
#include "playercam.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void PlayerCam::_ready() {
    ray = memnew(RayCast3D);
    ray->set_enabled(true);
    ray->set_scale(Vector3(1, 100, 1));
    ray->set_collide_with_bodies(true);
    ray->set_collide_with_areas(true);
    add_child(ray);
    ray->set_rotation(Vector3(Math_PI / 2.0, 0, 0));
}

void PlayerCam::_bind_methods() {
}

PlayerCam::PlayerCam() {
}

PlayerCam::~PlayerCam() {
}

void PlayerCam::_process(double delta) {
    Node3D* parent = (Node3D*)get_parent()->get_parent();
    if (parent->has_method("get_direction")) {
        Vector3 position = parent->get_global_position();
        if (!Engine::get_singleton()->is_editor_hint()) {
            // Look at t  player at all times.
            look_at(position, Vector3(0, 1, 0));
        }
    }

   
}

void PlayerCam::_physics_process(double delta) {
}
