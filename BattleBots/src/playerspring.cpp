#include "playerspring.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "player.h"

using namespace godot;

void PlayerSpring::_ready() {
    cam = memnew(PlayerCam);
    add_child(cam);
}
void PlayerSpring::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &PlayerSpring::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &PlayerSpring::get_radius);
    ClassDB::add_property("PlayerSpring", PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
    ClassDB::bind_method(D_METHOD("set_sens", "sens"), &PlayerSpring::set_sens);
    ClassDB::bind_method(D_METHOD("get_sens"), &PlayerSpring::get_sens);
    ClassDB::add_property("PlayerSpring", PropertyInfo(Variant::FLOAT, "sens"), "set_sens", "get_sens");
}

void PlayerSpring::_input(const Ref<InputEvent> &event) {
    Input *input = Input::get_singleton();
    if (input->get_mouse_mode() != Input::MouseMode::MOUSE_MODE_CAPTURED) { // Don't move camera when cursor unlocked
        return;
    }
    Ref<InputEventMouseMotion> mouse_motion = event;
    if (mouse_motion.is_valid()) {
        // Update pitch and yaw according to relative mouse movement
        mouse_rel_y = mouse_motion->get_relative().y;
        mouse_rel_x = mouse_motion->get_relative().x;
        pitch += mouse_rel_y * sens;
        pitch = Math::clamp((double)pitch, -Math_PI / 6.0f, Math_PI / 2.5f); // Don't let player look too far up or down
        yaw -= mouse_rel_x * sens;
    }
}
void PlayerSpring::_process(double delta) {
    if (!Engine::get_singleton()->is_editor_hint()) {
        Node3D *parent = (Node3D *)get_parent();
        Vector3 position = get_position();
        // Calculate camera x, y, z from pitch, yaw, and radius
        position.z = radius * cos(yaw) * cos(pitch);
        position.x = radius * sin(yaw) * cos(pitch);
        position.y = radius * sin(pitch);
        set_position(position);
    }
}

void PlayerSpring::set_radius(const float p_radius) {
    radius = p_radius;
}

float PlayerSpring::get_radius() const {
    return radius;
}

void PlayerSpring::set_sens(const float p_sens) {
    sens = p_sens;
}

float PlayerSpring::get_sens() const {
    return sens;
}

PlayerSpring::PlayerSpring() {
}

PlayerSpring::~PlayerSpring() {
}
