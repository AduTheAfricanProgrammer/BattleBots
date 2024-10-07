#include "jumppad.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "player.h"

using namespace godot;

void JumpPad::_ready() {  // Simple area that calls Player::on_jump_pad_entered when it detects a player enter
    set_process(true);
    Area3D* myArea = get_node<Area3D>("Area");
    myArea->connect("body_entered", Callable(this, "on_area_entered"));
}
void JumpPad::_bind_methods() {
    ClassDB::bind_method(D_METHOD("on_area_entered", "body"), &JumpPad::on_body_entered);

    ClassDB::bind_method(D_METHOD("set_jump_height", "height"), &JumpPad::set_jump_height);
    ClassDB::bind_method(D_METHOD("get_jump_height"), &JumpPad::get_jump_height);
    ClassDB::add_property("JumpPad", PropertyInfo(Variant::FLOAT, "pad_jump_height"), "set_jump_height", "get_jump_height");
}

JumpPad::JumpPad() {
}

JumpPad::~JumpPad() {
}

void JumpPad::_process(double delta) {
}

void JumpPad::set_jump_height(const float p_height) {
    jump_height = p_height;
}

float JumpPad::get_jump_height() const {
    return jump_height;
}

void JumpPad::on_body_entered(const Node3D* body) {
    if (body->has_meta("is_player")) {
        ((Player*)body)->call("on_jump_pad_entered", jump_height);
    }
}
