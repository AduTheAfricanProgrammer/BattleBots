#include "world.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;


void World::_ready(){

    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    get_multiplayer()->connect("peer_connected", Callable(this, "on_player_connected"));
    get_multiplayer()->connect("peer_disconnected", Callable(this, "on_peer_disconnected"));
    communicator = get_node<Node>("Communicator");
    player_scene = ResourceLoader::get_singleton()->load("res://player.tscn");
    if(communicator->call("get_in_singleplayer")){
      
        Node3D* player = (Node3D*) player_scene->instantiate();
        add_child(player);
        player->set_global_position(Vector3(0, 5, 0));
        Ref<PackedScene> listener_scene = ResourceLoader::get_singleton()->load("res://listener.tscn");
        Node3D* listener = (Node3D*) listener_scene->instantiate();
        add_child(listener);
        listener->set_global_position(Vector3(-25, 3, 20));
        return;
    }

    Dictionary rpc_config_dict;
    rpc_config_dict["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    rpc_config_dict["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    rpc_config_dict["call_local"] = false;
    rpc_config_dict["channel"] = 0;
    rpc_config("start_other_player", rpc_config_dict);

    rpc_config("show_win_screen", rpc_config_dict);


    bool started = communicator->call("get_already_started");
    if(started){
        int a = communicator->call("get_player_id", get_multiplayer()->get_unique_id());
        UtilityFunctions::print("HERE");
        UtilityFunctions::print(a);
        player1 = (Node3D*) player_scene->instantiate();
        player2 = (Node3D*) player_scene->instantiate();
        add_child(player1);
        add_child(player2);
        player1->set_global_position(Vector3(-5, 5, 0));
        player2->set_global_position(Vector3(5, 5, 0));
        player1->call("set_player_id", 1, true);
        player2->call("set_player_id", 2, true);
        emit_signal("both_players_joined");
        rpc("start_other_player", get_multiplayer()->get_unique_id());
        return;
    }




    int num_players = communicator->call("get_num_players");
    if(num_players == 1){
        player1 = (Node3D*) player_scene->instantiate();
        add_child(player1);
        player1->set_global_position(Vector3(-5, 5, 0));
        player1->call("set_player_id", 1, false);
        return;
    }


  
    // Add the players 
    emit_signal("both_players_joined");
    player1 = (Node3D*) player_scene->instantiate();
    player2 = (Node3D*) player_scene->instantiate();
    add_child(player1);
    add_child(player2);
    player1->set_global_position(Vector3(-5, 5, 0));
    player2->set_global_position(Vector3(5, 5, 0));
    player1->call("set_player_id", 1, true);
    player2->call("set_player_id", 2, true);
    player1->connect("on_player_death", Callable(this, "on_player_death"));
    player2->connect("on_player_death", Callable(this, "on_player_death"));
}

void World::_process(double delta) {
}

void World::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_in_chatbox", "is_in_chatbox"), &World::set_in_chatbox);
    ClassDB::bind_method(D_METHOD("on_player_connected", "player_id"), &World::on_player_connected);
    ClassDB::bind_method(D_METHOD("on_peer_disconnected", "id"), &World::on_peer_disconnected);
    ClassDB::bind_method(D_METHOD("start_other_player", "other_id"), &World::start_other_player);
    ClassDB::bind_method(D_METHOD("get_flight_collected", "playerName"), &World::get_flight_collected);
    ClassDB::bind_method(D_METHOD("get_health_collected", "playerName"), &World::get_health_collected);
    ClassDB::bind_method(D_METHOD("on_player_death"), &World::on_player_death);
    ClassDB::bind_method(D_METHOD("show_win_screen"), &World::show_win_screen);
    ClassDB::bind_method(D_METHOD("on_singleplayer_win"), &World::on_singleplayer_win);
    ClassDB::bind_method(D_METHOD("set_knockback", "player_id", "direction"), &World::set_knockback);
    ADD_SIGNAL(MethodInfo("in_chatbox", PropertyInfo(Variant::BOOL, "is_in_chatbox")));
    ADD_SIGNAL(MethodInfo("both_players_joined"));
}


World::World() {
}

World::~World() {
}


void World::set_in_chatbox(bool is_in_chatbox){
    emit_signal("in_chatbox", is_in_chatbox);
    if(is_in_chatbox){
        get_node<Control>("GUI")->get_node<TextureRect>("UnreadIcon")->set_visible(false);
    }
}

void World::on_player_connected(int player_id){
    communicator->call("after_game_start");
}

void World::on_peer_disconnected(int id){
    Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
    get_tree()->change_scene_to_file("res://end_screen.tscn");
}

void World::start_other_player(int other_id){
    UtilityFunctions::print("Starting other player");
    get_node<Control>("Chatbox")->call("send_world_message", "Both players have joined. Let the games begin!");
    player2 = (Node3D*) player_scene->instantiate();
    add_child(player2);
    player2->set_global_position(Vector3(5, 5, 0));
    player1->call("set_player_id", 1, true);
    player2->call("set_player_id", 2, true);
    return;
}

void World::set_knockback(int player_id, Vector3 direction){
    if(player_id == 1){
        player2->rpc("set_knockback", direction, 5);
    }
    else{
        player1->rpc("set_knockback", direction, 5);
    }
}

void World::get_flight_collected(String playerName){
    get_node<Control>("Chatbox")->call("send_world_message", playerName + " collected a flight collectible!");
}

void World::get_health_collected(String playerName){
    get_node<Control>("Chatbox")->call("send_world_message", playerName + " collected a health collectible!");
}

void World::on_player_death(){
    rpc("show_win_screen"); // for the other player
    Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
    communicator->call("set_game_end", 2);
    get_tree()->change_scene_to_file("res://end_screen.tscn");
}

void World::show_win_screen(){
    Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
    communicator->call("set_game_end", 1);
    get_tree()->change_scene_to_file("res://end_screen.tscn");
}

void World::on_singleplayer_win(){
    Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
    communicator->call("set_game_end", 3);
    get_tree()->change_scene_to_file("res://end_screen.tscn");
}