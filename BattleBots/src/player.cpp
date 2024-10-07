

#include "player.h"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void Player::_ready() {
    velocity = Vector3(0, 0, 0);
    playerState = Idle;
    inFlight = false;
    input = Input::get_singleton();
    flight_left = 0;
    dash_meter = 0;
  
    
    
    set_material(false);
    soundPlayer = memnew(AudioStreamPlayer);
    add_child(soundPlayer);
    can_play_sound = true;

    musicPlayer = memnew(AudioStreamPlayer);
    add_child(musicPlayer);
    can_play_music = true;

    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Control* GUI = get_parent()->get_node<Control>("GUI");
    connect("dashes_changed", Callable(GUI, "OnDashUpdate"));
    connect("flight_changed", Callable(GUI, "OnFlightUpdate"));
    connect("health_changed", Callable(GUI, "OnHealthUpdate"));
    get_parent()->connect("in_chatbox", Callable(this, "set_in_chatbox"));
    emit_signal("dashes_changed", dash_meter);
    emit_signal("flight_changed", flight_left);
   
    Button* sound = get_parent()->get_node<Control>("GUI")->get_node<Button>("SoundButton");
    sound->connect("pressed", Callable(this, "on_sound_pressed"));

    Button* music = get_parent()->get_node<Control>("GUI")->get_node<Button>("MusicButton");
    music->connect("pressed", Callable(this, "on_music_pressed"));
    input->set_mouse_mode(Input::MouseMode::MOUSE_MODE_CAPTURED);
    Ref<AudioStream> stream = ResourceLoader::get_singleton()->load("res://Sounds/music.mp3");
    musicPlayer->set_stream(stream);
    //musicPlayer->play();
    current_health = health/2;
    player_damage = health/4;
    set_meta("is_player", true);

    communicator = get_parent()->get_node<Node>("Communicator");

    get_multiplayer()->connect("peer_disconnected", Callable(this, "on_player_disconnected"));

    Area3D* hit_box = get_node<Area3D>("HitBox");
    hit_box->connect("body_entered", Callable(this, "on_body_entered"));

    Dictionary rpc_config_dict;
    rpc_config_dict["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    rpc_config_dict["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_UNRELIABLE;
    rpc_config_dict["call_local"] = false;
    rpc_config_dict["channel"] = 0;
   
    rpc_config("set_started_dash", rpc_config_dict);
    rpc_config("set_stopped_dash", rpc_config_dict);

    Dictionary rpc_config_dict_2;
    rpc_config_dict_2["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    rpc_config_dict_2["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_UNRELIABLE;
    rpc_config_dict_2["call_local"] = true;
    rpc_config_dict_2["channel"] = 0;
    rpc_config("transmit_sound", rpc_config_dict_2);
    rpc_config("set_knockback", rpc_config_dict_2);

    set_process(true);
}
void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_direction"), &Player::get_direction);

    ClassDB::bind_method(D_METHOD("set_acceleration", "acceleration"), &Player::set_acceleration);
    ClassDB::bind_method(D_METHOD("get_acceleration"), &Player::get_acceleration);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "acceleration"), "set_acceleration", "get_acceleration");

    ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &Player::set_gravity);
    ClassDB::bind_method(D_METHOD("get_gravity"), &Player::get_gravity);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "gravity"), "set_gravity", "get_gravity");

    ClassDB::bind_method(D_METHOD("set_max_speed", "max_speed"), &Player::set_max_speed);
    ClassDB::bind_method(D_METHOD("get_max_speed"), &Player::get_max_speed);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "max_speed"), "set_max_speed", "get_max_speed");

    ClassDB::bind_method(D_METHOD("set_air_control", "air_control"), &Player::set_air_control);
    ClassDB::bind_method(D_METHOD("get_air_control"), &Player::get_air_control);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "air_control"), "set_air_control", "get_air_control");

    ClassDB::bind_method(D_METHOD("set_flight_mode"), &Player::set_flight_mode);

    ClassDB::bind_method(D_METHOD("set_jump_height", "jump_height"), &Player::set_jump_height);
    ClassDB::bind_method(D_METHOD("get_jump_height"), &Player::get_jump_height);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "jump_height"), "set_jump_height", "get_jump_height");

    ClassDB::bind_method(D_METHOD("set_dash_time", "dash_time"), &Player::set_dash_time);
    ClassDB::bind_method(D_METHOD("get_dash_time"), &Player::get_dash_time);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "dash_time"), "set_dash_time", "get_dash_time");

    ClassDB::bind_method(D_METHOD("set_air_drag", "air_drag"), &Player::set_air_drag);
    ClassDB::bind_method(D_METHOD("get_air_drag"), &Player::get_air_drag);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "air_drag"), "set_air_drag", "get_air_drag");

    ClassDB::bind_method(D_METHOD("set_ledge_mode", "ledge_mode"), &Player::set_ledge_mode);
    ClassDB::bind_method(D_METHOD("get_ledge_mode"), &Player::get_ledge_mode);
    ClassDB::add_property("Player", PropertyInfo(Variant::INT, "ledge_mode"), "set_ledge_mode", "get_ledge_mode");

    ClassDB::bind_method(D_METHOD("set_flight_time", "flight_time"), &Player::set_flight_time);
    ClassDB::bind_method(D_METHOD("get_flight_time"), &Player::get_flight_time);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "flight_time"), "set_flight_time", "get_flight_time");

    ClassDB::bind_method(D_METHOD("set_dash_fill_speed", "dash_fill_speed"), &Player::set_dash_fill_speed);
    ClassDB::bind_method(D_METHOD("get_dash_fill_speed"), &Player::get_dash_fill_speed);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "dash_fill_speed"), "set_dash_fill_speed", "get_dash_fill_speed");

    ClassDB::bind_method(D_METHOD("set_health", "p_health"), &Player::set_health);
    ClassDB::bind_method(D_METHOD("get_health"), &Player::get_health);
    ClassDB::add_property("Player", PropertyInfo(Variant::INT, "max_health"), "set_health", "get_health");
    

    ADD_SIGNAL(MethodInfo("health_changed", PropertyInfo(Variant::INT, "current_health")));
    ADD_SIGNAL(MethodInfo("dashes_changed", PropertyInfo(Variant::FLOAT, "dashes_left")));
    ADD_SIGNAL(MethodInfo("flight_changed", PropertyInfo(Variant::FLOAT, "flight_left")));
    ADD_SIGNAL(MethodInfo("made_sound", PropertyInfo(Variant::VECTOR3, "position"), PropertyInfo(Variant::FLOAT, "intensity")));
    ADD_SIGNAL(MethodInfo("reset_color"));
    ADD_SIGNAL(MethodInfo("dash_state", PropertyInfo(Variant::BOOL, "dash_state")));
    ADD_SIGNAL(MethodInfo("on_player_death"));



    ClassDB::bind_method(D_METHOD("on_sound_pressed"), &Player::on_sound_pressed);
    ClassDB::bind_method(D_METHOD("on_music_pressed"), &Player::on_music_pressed);

    ClassDB::bind_method(D_METHOD("on_jump_pad_entered"), &Player::on_jump_pad_entered);

    ClassDB::bind_method(D_METHOD("take_damage"), &Player::take_damage);

    ClassDB::bind_method(D_METHOD("take_player_damage"), &Player::take_player_damage);


    ClassDB::bind_method(D_METHOD("play_sound", "path"), &Player::play_sound);

    ClassDB::bind_method(D_METHOD("set_player_id", "player_id", "set_sync"), &Player::set_player_id);
    ClassDB::bind_method(D_METHOD("on_player_disconnected", "id"), &Player::on_player_disconnected);
    ClassDB::bind_method(D_METHOD("set_in_chatbox", "is_in_chatbox"), &Player::set_in_chatbox);
    ClassDB::bind_method(D_METHOD("set_knockback", "direction", "intensity"), &Player::set_knockback);
    ClassDB::bind_method(D_METHOD("transmit_sound", "path"), &Player::transmit_sound);
    ClassDB::bind_method(D_METHOD("on_body_entered", "body"), &Player::on_body_entered);  

    ClassDB::bind_method(D_METHOD("set_started_dash"), &Player::set_started_dash); 
    ClassDB::bind_method(D_METHOD("set_stopped_dash"), &Player::set_stopped_dash);
    



}

Player::Player() {
}

Player::~Player() {
}

void Player::_process(double delta) {
   
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Regenerate can take damage
    if (!can_take_damage){
       
        cooldown_timer += (float) delta;
        if (cooldown_timer >= 0.2) {
             
            can_take_damage = true;
            cooldown_timer = 0;
        }
    }

    if(!is_in_singleplayer){
        int id = communicator->call("get_player_id", get_multiplayer()->get_unique_id());
        if(id != player_id){
            return;
        }
    }
    // Determine when to stop bleeding
    if(is_emitting_blood){
        damage_start_timer += (float)delta;
        if (damage_start_timer >= 0.5) {
            is_emitting_blood = false;
            get_node<CPUParticles3D>("HurtParticles")->set_emitting(false);
            damage_start_timer = 0;
        }
    }
    

    // Regenerate dash meter
    if (dash_meter < 100) {
        dash_meter += dash_fill_speed * (float) delta;
        if (dash_meter > 100) {
            dash_meter = 100;
        }
        emit_signal("dashes_changed", dash_meter);
    }

    // Lock/unlock cursor when pressing escape
    if (input->is_action_just_pressed("ui_cancel")) {
        UtilityFunctions::print("Escape pressed");
        if (input->get_mouse_mode() == Input::MouseMode::MOUSE_MODE_VISIBLE) {
            input->set_mouse_mode(Input::MouseMode::MOUSE_MODE_CAPTURED);
        } else if (input->get_mouse_mode() == Input::MouseMode::MOUSE_MODE_CAPTURED) {
            input->set_mouse_mode(Input::MouseMode::MOUSE_MODE_VISIBLE);
        }
    }

    // Attempt to grab powerup
    if (input->is_action_just_pressed("input_z") && !is_in_chatbox) {
        Area3D* grabArea = get_node<Area3D>("GrabArea");
        // Get overlapping areas, then iterate through them to see if they're a collectible.
        TypedArray<Area3D> areas = grabArea->get_overlapping_areas();
        bool hasPlayedSound = false;
        
        for (int i = 0; i < areas.size(); i++) {
            Area3D* area = (Area3D*)((Object*)areas[i]);
            if (area->has_meta("is_health_collectible")) {  
                bool collectable = area->call("can_collect");
                if (collectable) {
                    current_health = health;
                    hasPlayedSound = true;
                    emit_signal("health_changed", 100);
                    area->call("on_collected");
                    play_sound("res://Sounds/grabhealth.wav");
                    emit_signal("made_sound", get_global_position(), 5.0);
                    String playerName = communicator->call("get_player_name", get_multiplayer()->get_unique_id());
                    get_parent()->call("get_health_collected", playerName);
                    break;  // Only grab one collectible at a time.
                }
            }
            if (area->has_meta("is_flight_collectible")) {
                bool collectable = area->call("can_collect");
                if (collectable) {
                    flight_left = flight_time;
                    hasPlayedSound = true;
                    emit_signal("flight_changed", 100);
                    area->call("on_collected");
                    play_sound("res://Sounds/grabflight.wav");
                    emit_signal("made_sound", get_global_position(), 5.0);
                    String playerName = communicator->call("get_player_name", get_multiplayer()->get_unique_id());
                    get_parent()->call("get_flight_collected", playerName);

                    break;
                }
                
            }
        }
        if (!hasPlayedSound) {
            play_sound("res://Sounds/nograb.wav");
            emit_signal("made_sound", get_global_position(), 5.0);
        }
    }

    if (input->is_action_just_pressed("input_f") && !is_in_chatbox) {
        set_flight_mode();
    }
    if (inFlight && flight_left < 0) {
        set_flight_mode();
    }

    if (!musicPlayer->is_playing() && can_play_music) {
        musicPlayer->play();
    }
}

void Player::play_sound(String path) {
    rpc("transmit_sound", path);
}

void Player::transmit_sound(String path){
    if (can_play_sound) {
        Ref<AudioStream> stream = ResourceLoader::get_singleton()->load(path);
        soundPlayer->set_stream(stream);
        soundPlayer->play();
    }
}

// Physics process
void Player::_physics_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    if(!is_in_singleplayer){
        int id = communicator->call("get_player_id", get_multiplayer()->get_unique_id());
        if(id != player_id){
            return;
        }
    }
    
    if (get_global_position().y < -10) {  // Respawn when player falls
        reset();
        return;
    }
    if (playerState == Ledge) {                         // Ledge grab
        if (input->is_action_pressed("input_space") && !is_in_chatbox) {  // Climb up
            set_position(last_pos);
            playerState = Idle;
        } else if (input->is_action_pressed("input_shift") && !is_in_chatbox) {  // Drop
            playerState = Idle;
        } else {  // Set player position to slightly under ledge
            Vector3 pos = last_pos;
            pos.y -= 0.5;
            set_position(pos);
        }
        return;
    }
    if (playerState == Dashing) {  // Dash for certain amount of time
        currentDashTime += (float)delta;
        if (currentDashTime >= dash_time) {
            playerState = Idle;
            rpc("set_stopped_dash");
            set_material(false);
            currentDashTime = 0;
        }
        set_velocity(velocity);
        move_and_slide();
        return;
    }
    Vector3 old_rot = get_rotation();
    if(!is_in_chatbox){
        movement(delta);  // Keyboard movement
    }
    else{
        deccel(delta);
    }
    if (playerState == Jumping) {
        if (is_on_floor() && velocity.y < 0) {  // Update player state when jump finishes and player hits ground
            playerState = Idle;
            emit_signal("made_sound", get_global_position(), 5.0);
        }
    }

    // Set velocity.y to be affected by gravity if not on the ground
    if (!is_on_floor()) {
        velocity.y -= gravity * float(delta);
    } else {
        if (just_hit_pad) {
            just_hit_pad = false;  // Don't lock velocity after hitting jump pad
        } else {
            velocity.y = 0;  // Keep velocity.y to 0 as long as player is on ground
        }
    }

    if (is_on_floor() && !(playerState == Jumping || playerState == Flying || playerState == Dashing)) {  // Initiate jump
        if (input->is_action_pressed("input_space") && !is_in_chatbox) {
            velocity.y = jump_height;
            emit_signal("made_sound", get_global_position(), 3.0); // You make more sound when landing.
            playerState = Jumping;
        }

    } else if (inFlight)  // Flying
    {
        flight_left -= (float)delta;
        emit_signal("flight_changed", (flight_left / flight_time) * 100);
        if (input->is_action_pressed("input_space") && !is_in_chatbox) {
            emit_signal("made_sound", get_global_position(), 3.0);
            velocity.y = jump_height;
        }

        if (input->is_action_pressed("input_shift") && !is_in_chatbox) {
            emit_signal("made_sound", get_global_position(), 3.0);
            velocity.y = -jump_height;
        }
    }
    if (!inFlight) {  // Initiate dash
        if (input->is_action_just_pressed("input_shift") && dash_meter == 100 && !is_in_chatbox) {
            rpc("set_started_dash");
            playerState = Dashing;
            emit_signal("dash_state", true);
            Vector3 forward = get_global_transform().basis.xform(Vector3(0, 0, 1));
            velocity = forward * max_speed * 10; 
            dash_meter = 0;
            set_material(true);
            emit_signal("made_sound", get_global_position(), 5.0);
            emit_signal("dashes_changed", dash_meter);
            dashable = true;
            communicator->call("set_player_hurt", 0);
        } else if (input->is_action_just_pressed("input_shift") && !(dash_meter == 100) && !is_in_chatbox) {
            emit_signal("made_sound", get_global_position(), 12.0);
            play_sound("res://Sounds/error.wav");
        }
    }
    if (ledge_mode) {  // Ledge stop/grab is enabled
        Node3D* rays = get_node<Node3D>("Rays");
        Vector3 vel_horiz = velocity;
        vel_horiz.y = 0.0;
        Vector3 vel_transformed = get_global_transform().basis.xform_inv(vel_horiz);
        Vector3 translation;
        if (ledge_mode == 1) {
            translation = 5.0 * vel_transformed * delta;
        } else {
            translation = vel_transformed * delta;
        }
        rays->translate(translation);  // Test move the player by moving it's raycasts in the direction of its velocity
        if (is_on_floor() && !(playerState == Jumping || playerState == Flying || playerState == Dashing)) {
            // Only perform ledge stop/grab if player is walking off ledge
            TypedArray<Node> arr = rays->get_children();
            bool all_off = true;
            for (int i = 0; i < arr.size(); i++) {
                RayCast3D* ray = Object::cast_to<RayCast3D>(arr[i]);
                ray->force_raycast_update();
                if (ray->is_colliding()) {
                    all_off = false;
                }
            }
            if (all_off) {              // Player is moving in the direction of falling off
                if (ledge_mode == 1) {  // Ledge stop
                    velocity.x = 0.0;
                    velocity.z = 0.0;
                    set_rotation(old_rot);
                } else {  // Initiate ledge grab
                    playerState = Ledge;
                    last_pos = get_position();
                }
            }
        }
        rays->translate(-translation);  // Undo raycast translation
    }
    // Actually move player
    set_velocity(velocity);
    move_and_slide();
    
}

// Basic player ground movement
void Player::movement(double delta) {
    // Rotate with q and e
    int direction_rotate = input->get_axis("input_q", "input_e");
    if (direction_rotate > 0) {
        rotate_y(-3 * (float)delta);
    } else if (direction_rotate < 0) {
        rotate_y(3 * (float)delta);
    }
    int direction_forward = input->get_axis("input_s", "input_w");
    int direction_left = input->get_axis("input_d", "input_a");
    // Use precomputed matrix of direction vectors to handle cases of holding
    // multiple keys at same time, normalize so diagonal movement isn't faster
    Vector3 dir = dir_vectors[direction_forward + 1][direction_left + 1].normalized();
    // Get direction in world space
    Vector3 transformed = get_global_transform().basis.xform(dir);
    // Add direction to velocity, scaled by acceleration depending on whether player
    // is in air or on ground
    velocity += transformed * acceleration * get_accel_factor() * float(delta);
    Vector2 vel_no_up = Vector2(velocity.x, velocity.z);
    vel_no_up = vel_no_up.limit_length(max_speed);  // Clamp horizontal speed
    // Don't update y component of velocity in this function, as it's handled separately
    velocity.x = vel_no_up.x;
    velocity.z = vel_no_up.y;
    emit_signal("dash_state", false);
    if (dir.length() == 0 || !is_on_floor()) {  // No input or in air
        deccel(delta);
    }
    else{
        emit_signal("made_sound", get_global_position(), 3.0);
    }
}

void Player::deccel(double delta) {  // Apply either ground friction or air resistance
    if (velocity.length() > 0) {
        Vector3 new_vel = velocity - (velocity.normalized() * get_friction(float(velocity.length())) * float(delta));
        if (velocity.dot(new_vel) < 0) {  // Implies that velocity flipped signs this frame, which means it should be clamped to 0 to prevent overshoot
            velocity = Vector3(0.0, 0.0, 0.0);
        } else if (!inFlight) {  // Don't apply deccel when flying
            velocity.x = new_vel.x;
            velocity.z = new_vel.z;
        } else {
            velocity = new_vel;
        }
    }
    if (velocity.x == 0.0 && velocity.z == 0.0) {
        if (playerState == Moving) {
            playerState = Idle;
        }
    }
}

void Player::on_sound_pressed() {
    can_play_sound = !can_play_sound;
}

void Player::on_music_pressed() {
    can_play_music = !can_play_music;
    if (can_play_music) {
        musicPlayer->set_stream_paused(false);
    } else {
        musicPlayer->set_stream_paused(true);
    }
}

void Player::set_flight_mode() {
    if (!inFlight && flight_left > 0) {
        flight_gravity = gravity;
        gravity = gravity * 0;
        playerState = Flying;
        get_node<CPUParticles3D>("FlightParticles")->set_emitting(true);
        inFlight = true;
    } else if (inFlight) {
        gravity = flight_gravity;
        playerState = Idle;
        get_node<CPUParticles3D>("FlightParticles")->set_emitting(false);
        inFlight = false;
    }

    if (!inFlight && !(flight_left > 0)) {
        emit_signal("made_sound", get_global_position(), 12.0);
        play_sound("res://Sounds/error.wav");
    }
}
void Player::on_jump_pad_entered(float pad_jump_height) {
    play_sound("res://Sounds/boing.mp3");
    velocity.y = pad_jump_height;
    just_hit_pad = true;
}

void Player::reset() {  
    set_global_position(Vector3(0, 5, 0));
    emit_signal("reset_color"); 
}

float Player::get_accel_factor() const {
    if (playerState == Jumping) {
        return air_control;
    } else {
        return 1.0;
    }
}

float Player::get_friction(const float p_speed) const {
    if (is_on_floor()) {
        return acceleration;
    } else {
        return air_drag * p_speed * p_speed;
    }
}

void Player::set_ledge_mode(const int p_ledge_mode) {
    ledge_mode = p_ledge_mode;
}

int Player::get_ledge_mode() const {
    return ledge_mode;
}

void Player::set_acceleration(const float p_acceleration) {
    acceleration = p_acceleration;
}
float Player::get_acceleration() const {
    return acceleration;
}

void Player::set_gravity(const float p_gravity) {
    gravity = p_gravity;
}

float Player::get_gravity() const {
    return gravity;
}

void Player::set_air_control(const float p_air_control) {
    air_control = p_air_control;
}

float Player::get_air_control() const {
    return air_control;
}

void Player::set_jump_height(const float p_jump_height) {
    jump_height = p_jump_height;
}

float Player::get_jump_height() const {
    return jump_height;
}

void Player::set_dash_time(const float p_dash_time) {
    dash_time = p_dash_time;
}

float Player::get_dash_time() const {
    return dash_time;
}

void Player::set_air_drag(const float p_air_drag) {
    air_drag = p_air_drag;
}

float Player::get_air_drag() const {
    return air_drag;
}

void Player::set_max_speed(const float p_max_speed) {
    max_speed = p_max_speed;
}

void Player::set_flight_time(const float p_flight_time) {
    flight_time = p_flight_time;
}

float Player::get_flight_time() const {
    return flight_time;
}

float Player::get_max_speed() const {
    return max_speed;
}

void Player::set_dash_fill_speed(const float p_dash_fill_speed) {
    dash_fill_speed = p_dash_fill_speed;
}
float Player::get_dash_fill_speed() const {
    return dash_fill_speed;
}


void Player::set_health(const int p_health) {
    health = p_health;
}
int Player::get_health() const {
    return health;
}


// Changes the player's material to red if turnDash is true, white if false.
void Player::set_material(bool turnDash){
    // Ref<Mesh> mesh = get_node<MeshInstance3D>("MeshInstance3D")->get_mesh();
    // Ref<StandardMaterial3D> playerMaterial = mesh->surface_get_material(0);
    // if(turnDash){
    //     if (playerMaterial.is_valid()) {
    //         playerMaterial->set_albedo(Color(1, 0, 0)); // Set to red
    //         playerMaterial->set_metallic(1);
    //     }
    // }
    // else{
    //     if (playerMaterial.is_valid()) {
    //         playerMaterial->set_albedo(Color(1, 1, 1)); // Set to white
    //         playerMaterial->set_metallic(0);
    //     }
    // }
}

void Player::take_damage(int damage) {
    current_health -= damage;
    emit_signal("health_changed", ((float) current_health)/health * 100);
    if (current_health <= 0) {
        reset();
    }
}

void Player::take_player_damage() {
    current_health -= player_damage;
    emit_signal("health_changed", ((float) current_health)/health * 100);
    if (current_health <= 0) {
        emit_signal("on_player_death");;
    }
}


Vector3 Player::get_direction() {
    return velocity.normalized();
}


void Player::set_player_id(int player_id, bool set_sync) {
    this->player_id = player_id;
    is_in_singleplayer = false;
    if (this->player_id == 1) {
        other_player_id = 2;
    } else {
        other_player_id = 1;
    }
    if(set_sync){
        Ref<PackedScene> sync_scene = ResourceLoader::get_singleton()->load("res://synchronizer.tscn");
        MultiplayerSynchronizer* sync = (MultiplayerSynchronizer*) sync_scene->instantiate();
        add_child(sync);
        Node *game_manager = get_node<Node>("/root/GameManager");
        int authority_id = game_manager->call("get_multiplayer_id_from_player_id", player_id);
        sync->set_multiplayer_authority(authority_id);
        UtilityFunctions::print(authority_id);
    }
    int id = communicator->call("get_player_id", get_multiplayer()->get_unique_id()); 
    if(id != player_id){
        return;
    }
    emit_signal("dashes_changed", dash_meter);
    emit_signal("flight_changed", flight_left);
    get_node<Node>("PlayerSpring")->get_node<Camera3D>("PlayerCam")->set_current(true);

}

void Player::on_player_disconnected(int id) {
    UtilityFunctions::print("Player disconnected");
    // TODO: Handle the other player disconnecting
}

void Player::set_in_chatbox(bool is_in_chatbox){
    this->is_in_chatbox = is_in_chatbox;
}

void Player::set_knockback(Vector3 direction, int intensity){
    velocity.y = intensity;
    direction.normalize();
    velocity.x = direction.x * intensity;
    velocity.z = direction.z * intensity;
    just_hit_pad = true;
}

void Player::on_body_entered(const Node3D* body) {
    if (body->has_meta("is_player") &&  playerState != Dashing && other_player_dashing && can_take_damage) {
        get_parent()->get_node<Player>("Player")->call("play_sound", "res://Sounds/playerhurt.mp3");
        take_player_damage();
        Vector3 dir = get_global_transform().basis.xform(Vector3(0, 0, 1));
        get_parent()->call("set_knockback", player_id, dir);
        can_take_damage = false;
        cooldown_timer = 0;
        UtilityFunctions::print(player_id);
        
    }
}

void Player::set_started_dash(){
    other_player_dashing = true;
}

void Player::set_stopped_dash(){
     UtilityFunctions::print("set to false");
    other_player_dashing = false;
}