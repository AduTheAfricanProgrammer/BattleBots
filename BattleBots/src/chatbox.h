#ifndef CHATBOX_H
#define CHATBOX_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/texture_button.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <vector>

namespace godot {

class Chatbox : public Control {
    GDCLASS(Chatbox, Control)
   private:
    void on_message_sent(String message);
    void send_message(String message, int player_id);
    void send_world_message(String message);
    void shift_messages();
    void on_chatbutton_pressed();
    void on_taunt_made();
    std::vector<String> taunts;
    std::vector<Label*> messages;
    LineEdit* line_edit;
    Ref<PackedScene> text;
    bool is_present = false;
    bool grab_focus_next_frame = false; // Used to grab the focus without typing the input
    String player_name;
    Node* communicator;
    
   protected:
    static void _bind_methods();

   public:

    Chatbox();
    ~Chatbox();
    void _ready() override;
    void _process(double delta) override;
};

}  // namespace godot
#endif

