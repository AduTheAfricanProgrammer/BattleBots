#include "chatbox.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;


void Chatbox::_ready(){
    if(Engine::get_singleton()->is_editor_hint()){
        return;
    }
    line_edit = get_node<LineEdit>("LineEdit");
    line_edit->connect("text_submitted", Callable(this, "on_message_sent"));
    get_node<Button>("TauntButton")->connect("pressed", Callable(this, "on_taunt_made"));
    text = ResourceLoader::get_singleton()->load("res://text.tscn");
    communicator = get_parent()->get_node<Node>("Communicator");
    player_name = communicator->call("get_player_name", get_multiplayer()->get_unique_id()); 
    get_parent()->get_node<Control>("GUI")->get_node<TextureButton>("ChatButton")->connect("pressed", Callable(this, "on_chatbutton_pressed"));
    // Make a dictionary with the RPC condig needed. 
    Dictionary rpc_config_dict;
    rpc_config_dict["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    rpc_config_dict["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    rpc_config_dict["call_local"] = true;
    rpc_config_dict["channel"] = 0;
    rpc_config("send_message", rpc_config_dict);

    // Add some taunts
    taunts.push_back("You're gonna go down!");
    taunts.push_back("You're going down!");
    taunts.push_back("I shall cut you down, break you apart...");
    taunts.push_back("My hands shall RELISH ending you... HERE AND NOW!");
    taunts.push_back("I will not be defeated!");
    taunts.push_back("Oh, you're approaching me?");
    taunts.push_back("How many breads have you eaten in your life?");
    taunts.push_back("You will never reach the truth!");
    taunts.push_back("You are strong, child. But I am beyond strength. I am... the end.");
    taunts.push_back("Count your days!");
    taunts.push_back("You're already dead.");
    taunts.push_back("Throughout heaven and earth, I alone am the honored one.");
    taunts.push_back("You're not even worth the effort.");
    taunts.push_back("Don't underestimate me!");
    taunts.push_back("Too easy!");
    taunts.push_back("Burn this in your memory!");

    set_visible(false);
}

void Chatbox::_process(double delta) {
    if(Engine::get_singleton()->is_editor_hint()){
        return;
    }
    if(Input::get_singleton()->is_action_just_pressed("input_c")){
        is_present = true;
        get_parent()->call("set_in_chatbox", is_present);
        Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
        set_visible(true);
        line_edit->grab_focus();
    }
    if(Input::get_singleton()->is_action_just_pressed("ui_cancel")){
        is_present = false;
        get_parent()->call("set_in_chatbox", is_present);
        set_visible(false);
        line_edit->release_focus();
    }
    if(grab_focus_next_frame){
        line_edit->grab_focus();
        grab_focus_next_frame = false;
    }
   
}

void Chatbox::_bind_methods() {
    ClassDB::bind_method(D_METHOD("on_message_sent", "message"), &Chatbox::on_message_sent);
    ClassDB::bind_method(D_METHOD("send_message", "message", "player_id"), &Chatbox::send_message);
    ClassDB::bind_method(D_METHOD("on_chatbutton_pressed"), &Chatbox::on_chatbutton_pressed);
    ClassDB::bind_method(D_METHOD("send_world_message", "message"), &Chatbox::send_world_message);
    ClassDB::bind_method(D_METHOD("on_taunt_made"), &Chatbox::on_taunt_made);
}


Chatbox::Chatbox() {
}

Chatbox::~Chatbox() {
}

void Chatbox::send_message(String message, int player_id){
    if(!is_present){
        get_parent()->get_node<Control>("GUI")->get_node<TextureRect>("UnreadIcon")->set_visible(true);
    }
    String my_name = communicator->call("get_player_name", player_id); 
    if(communicator->call("get_in_singleplayer")){
        my_name = "Self";
    }
    Label* label = (Label*) text->instantiate();
    label->set_position(Vector2(120, 590));
    // World message
    if(player_id == 0){
        label->add_theme_color_override("font_color", Color(1, 1, 0));
        label->set_text(message);
        messages.push_back(label);
        shift_messages();
        add_child(label);
        return;
    }
    label->set_text(my_name + ": " + message);
    messages.push_back(label);
    shift_messages();
    add_child(label);
}

void Chatbox::on_message_sent(String message){
    int num_players = communicator->call("get_num_players");
    if(num_players == 1){
        send_message(message, 1);
        return;
    }
    rpc("send_message", message, get_multiplayer()->get_unique_id());
    line_edit->set_text("");
}

// Shifts the messages up by moving their transforms up.
void Chatbox::shift_messages(){
    if(messages.size() > 12){
        messages[0]->queue_free();
        messages.erase(messages.begin());
    }
    for(int i = 0; i < messages.size(); i++){
        Vector2 position = messages[i]->get_position();
        messages[i]->set_position(position + Vector2(0, -35));
    }
    
}

void Chatbox::on_chatbutton_pressed(){
    is_present = !is_present;
    get_parent()->call("set_in_chatbox", is_present);
    set_visible(is_present);
    if(is_present){
        line_edit->grab_focus();
        Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
    }
    else{
        line_edit->release_focus();
    }
}

void Chatbox::send_world_message(String message){
    int num_players = communicator->call("get_num_players");
    if(num_players == 1){
        send_message(message, 0);
        return;
    }
    rpc("send_message", message, 0);
}

void Chatbox::on_taunt_made(){
    int taunt_index = rand() % taunts.size();
    String taunt = taunts[taunt_index];
    rpc("send_message", taunt, get_multiplayer()->get_unique_id());
}