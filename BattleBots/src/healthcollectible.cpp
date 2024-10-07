#include "healthcollectible.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void HealthCollectible::_ready() {
    collectable = true;
    set_meta("is_health_collectible", true);
    set_meta("ignore_visibility", true);
    Dictionary rpc_config_dict;
    rpc_config_dict["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    rpc_config_dict["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    rpc_config_dict["call_local"] = true;
    rpc_config_dict["channel"] = 0;
    rpc_config("collect", rpc_config_dict);
   // get_parent()->connect("both_players_joined", Callable(this, "on_both_players_joined"));
    timer = 0;
    set_process(true);
}
void HealthCollectible::_bind_methods() {
    ClassDB::bind_method(D_METHOD("on_area_entered", "area"), &HealthCollectible::on_area_entered);
    ClassDB::bind_method(D_METHOD("on_collected"), &HealthCollectible::on_collected);
    ClassDB::bind_method(D_METHOD("can_collect"), &HealthCollectible::can_collect);
    ClassDB::bind_method(D_METHOD("collect"), &HealthCollectible::collect);
    ClassDB::bind_method(D_METHOD("on_both_players_joined"), &HealthCollectible::on_both_players_joined);
}

HealthCollectible::HealthCollectible() {
}

HealthCollectible::~HealthCollectible() {
}

void HealthCollectible::_process(double delta) {
    if (!collectable) {  // After collection, wait 1 second before deleting
        timer += (float)delta;
        if (timer > 1.0) {
            queue_free();
        }
    }
    MeshInstance3D* mesh = get_node<MeshInstance3D>("MeshInstance3D");
    mesh->rotate_y((float)delta);  // Slowly rotate mesh for visual effect
}

void HealthCollectible::on_area_entered(const Area3D* area) {
}

void HealthCollectible::on_collected() {
    rpc("collect");
}

bool HealthCollectible::can_collect() {
    return collectable;
}

void HealthCollectible::on_both_players_joined(){

}

void HealthCollectible::collect(){
    CPUParticles3D* particles = (CPUParticles3D*)get_child(0);
    collectable = false;
    particles->set_emitting(true);
    ((MeshInstance3D*)get_child(2))->set_visible(false);
}