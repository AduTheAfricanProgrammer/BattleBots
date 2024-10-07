#include "flightcollectible.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void FlightCollectible::_ready() {
    collectable = true;
    set_meta("is_flight_collectible", true);
    set_meta("ignore_visibility", true);
    Dictionary rpc_config_dict;
    rpc_config_dict["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    rpc_config_dict["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    rpc_config_dict["call_local"] = true;
    rpc_config_dict["channel"] = 0;
    rpc_config("collect", rpc_config_dict);
    timer = 0;
    set_process(true);
}
void FlightCollectible::_bind_methods() {
    ClassDB::bind_method(D_METHOD("on_area_entered", "area"), &FlightCollectible::on_area_entered);
    ClassDB::bind_method(D_METHOD("on_collected"), &FlightCollectible::on_collected);
    ClassDB::bind_method(D_METHOD("can_collect"), &FlightCollectible::can_collect);
    ClassDB::bind_method(D_METHOD("collect"), &FlightCollectible::collect);
}

FlightCollectible::FlightCollectible() {
}

FlightCollectible::~FlightCollectible() {
}

void FlightCollectible::_process(double delta) {
    if (!collectable) {  // After collection, wait 1 second before deleting
        timer += (float)delta;
        if (timer > 1.0) {
            queue_free();
        }
    }
    MeshInstance3D* mesh = get_node<MeshInstance3D>("MeshInstance3D");
    mesh->rotate_y((float)delta);  // Slowly rotate mesh for visual effect
}

void FlightCollectible::on_area_entered(const Area3D* area) {
}

void FlightCollectible::on_collected() {
    rpc("collect");
}

bool FlightCollectible::can_collect() {
    return collectable;
}


void FlightCollectible::collect(){ 
    CPUParticles3D* particles = (CPUParticles3D*)get_child(0);
    collectable = false;
    particles->set_emitting(true);
    ((MeshInstance3D*)get_child(2))->set_visible(false);
}