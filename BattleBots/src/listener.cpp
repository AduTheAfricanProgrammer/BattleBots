#include "listener.h"
#include "player.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void Listener::_ready()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    
   
    set_process(true);
    set_meta("ignore_visibility", true);

    // Connect some important signals to the player.
    Player *player = get_parent()->get_node<Player>("Player");
    player->connect("made_sound", Callable(this, "on_sound_heard"));
    player->connect("reset_color", Callable(this, "on_reset_color"));
    player->connect("dash_state", Callable(this, "can_die"));

    sound_player = memnew(AudioStreamPlayer3D);
    sound_player->set_max_polyphony(5);
    add_child(sound_player);

    blade_player = memnew(AudioStreamPlayer3D);
    add_child(blade_player);
    
    player_dash = false;

    previous_color = Color(0, 1, 0);
    
    body_particles = get_node<CPUParticles3D>("BodyParticles");
    set_color(Color(0, 1, 0));
    set_blade_color(Color(0.4, 0, 0));
    // Connect to the sound button.
    Button* sound = get_parent()->get_node<Control>("GUI")->get_node<Button>("SoundButton");
    sound->connect("pressed", Callable(this, "on_sound_pressed"));

    nav_agent = memnew(NavigationAgent3D);
    add_child(nav_agent);
    waypoints = get_node<Node3D>("Waypoints");
    // Create a new waypoint at the start position
    Waypoint *start_waypoint = memnew(Waypoint);
    waypoints->add_child(start_waypoint);
    start_waypoint->set_global_position(get_global_position());
    num_waypoints = waypoints->get_child_count();
    current_waypoint = ((Node3D *)(waypoints->get_child(current_waypoint_index)))->get_global_position();
    waypoint_position = waypoints->get_global_position();

    // Connect the feeling area's signal to the listener's on_feel_entered method
    Area3D* feel_area = get_node<Area3D>("FeelArea");
    feel_area->connect("body_entered", Callable(this, "on_feel_entered"));
    feel_area->connect("body_exited", Callable(this, "on_feel_exited"));


    Area3D* hit_box = get_node<Area3D>("HitBox");
    hit_box->connect("body_entered", Callable(this, "on_body_entered"));
    hit_box->connect("body_exited", Callable(this, "on_body_exited"));
    
    // Create an unordered map of the form std::unordered_map<std::string, std::shared_ptr<State> that just contains the Patrol state
    std::shared_ptr<Patrol> patrol = std::make_shared<Patrol>();
    std::shared_ptr<Seek> seek = std::make_shared<Seek>();
    std::shared_ptr<Charge> charge = std::make_shared<Charge>();
    std::shared_ptr<Paused> paused = std::make_shared<Paused>();
    std::shared_ptr<Spinning> spinning = std::make_shared<Spinning>();
    patrol->init(this, nav_agent, waypoints, patrol_speed);
    seek->init(this);
    charge->init(this);
    paused->init(this, 2);
    spinning->init(this, damage, damage_tick_speed);
    states["Patrol"] = patrol;
    states["Seek"] = seek;
    states["Charge"] = charge;
    states["Paused"] = paused;
    states["Spinning"] = spinning;
    state_machine = memnew(StateMachine(states, "Patrol"));
    set_meta("is_listener", true);
}

void Listener::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_patrol_speed", "patrol_speed"), &Listener::set_patrol_speed);
    ClassDB::bind_method(D_METHOD("get_patrol_speed"), &Listener::get_patrol_speed);
    ClassDB::add_property("Listener", PropertyInfo(Variant::FLOAT, "patrol_speed"), "set_patrol_speed", "get_patrol_speed");

    ClassDB::bind_method(D_METHOD("set_seek_speed", "seek_speed"), &Listener::set_seek_speed);
    ClassDB::bind_method(D_METHOD("get_seek_speed"), &Listener::get_seek_speed);
    ClassDB::add_property("Listener", PropertyInfo(Variant::FLOAT, "seek_speed"), "set_seek_speed", "get_seek_speed");

    ClassDB::bind_method(D_METHOD("set_charge_speed", "charge_speed"), &Listener::set_charge_speed);
    ClassDB::bind_method(D_METHOD("get_charge_speed"), &Listener::get_charge_speed);
    ClassDB::add_property("Listener", PropertyInfo(Variant::FLOAT, "charge_speed"), "set_charge_speed", "get_charge_speed");

    ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &Listener::set_gravity);
    ClassDB::bind_method(D_METHOD("get_gravity"), &Listener::get_gravity);
    ClassDB::add_property("Listener", PropertyInfo(Variant::FLOAT, "gravity"), "set_gravity", "get_gravity");

    ClassDB::bind_method(D_METHOD("get_damage"), &Listener::get_damage);
    ClassDB::bind_method(D_METHOD("set_damage", "damage"), &Listener::set_damage);
    ClassDB::add_property("Listener", PropertyInfo(Variant::INT, "damage"), "set_damage", "get_damage");

    ClassDB::bind_method(D_METHOD("get_sensitivity"), &Listener::get_sensitivity);
    ClassDB::bind_method(D_METHOD("set_sensitivity", "sensitivity"), &Listener::set_sensitivity);
    ClassDB::add_property("Listener", PropertyInfo(Variant::FLOAT, "sensitivity", PROPERTY_HINT_RANGE, "0.1,5,0.01"), "set_sensitivity", "get_sensitivity");

    ClassDB::bind_method(D_METHOD("get_feel_area_radius"), &Listener::get_feel_area_radius);
    ClassDB::bind_method(D_METHOD("set_feel_area_radius", "feel_area_radius"), &Listener::set_feel_area_radius);
    ClassDB::add_property("Listener", PropertyInfo(Variant::FLOAT, "spin_radius", PROPERTY_HINT_RANGE, "0.1,10,0.01"), "set_feel_area_radius", "get_feel_area_radius");

    ClassDB::bind_method(D_METHOD("get_blade_noise"), &Listener::get_blade_noise);
    ClassDB::bind_method(D_METHOD("set_blade_noise", "play_blade_noise"), &Listener::set_blade_noise);
    ClassDB::add_property("Listener", PropertyInfo(Variant::BOOL, "play_blade_noise"), "set_blade_noise", "get_blade_noise");

    ClassDB::bind_method(D_METHOD("get_damage_tick_speed"), &Listener::get_damage_tick_speed);
    ClassDB::bind_method(D_METHOD("set_damage_tick_speed", "damage_tick_speed"), &Listener::set_damage_tick_speed);
    ClassDB::add_property("Listener", PropertyInfo(Variant::FLOAT, "damage_tick_speed", PROPERTY_HINT_RANGE, "0.5,5,0.01"), "set_damage_tick_speed", "get_damage_tick_speed");


    ClassDB::bind_method(D_METHOD("on_sound_heard", "position", "intensity"), &Listener::on_sound_heard);

    ClassDB::bind_method(D_METHOD("do_seek", "delta"), &Listener::do_seek);
    ClassDB::bind_method(D_METHOD("do_charge", "delta"), &Listener::do_charge);

    ClassDB::bind_method(D_METHOD("get_next_after_patrol"), &Listener::get_next_after_patrol);
    ClassDB::bind_method(D_METHOD("get_next_after_seek"), &Listener::get_next_after_seek);
    ClassDB::bind_method(D_METHOD("get_next_after_charge"), &Listener::get_next_after_charge);
    ClassDB::bind_method(D_METHOD("get_pause_interrupt"), &Listener::get_pause_interrupt);

    ClassDB::bind_method(D_METHOD("on_feel_entered", "body"), &Listener::on_feel_entered);
    ClassDB::bind_method(D_METHOD("on_feel_exited", "body"), &Listener::on_feel_exited);

    ClassDB::bind_method(D_METHOD("set_color", "color"), &Listener::set_color);
    ClassDB::bind_method(D_METHOD("set_blade_color", "color"), &Listener::set_blade_color);

    ClassDB::bind_method(D_METHOD("on_reset_color"), &Listener::on_reset_color);

    ClassDB::bind_method(D_METHOD("on_sound_pressed"), &Listener::on_sound_pressed);

    ClassDB::bind_method(D_METHOD("play_sound", "path"), &Listener::play_sound);
    ClassDB::bind_method(D_METHOD("play_blade", "pitch"), &Listener::play_blade);

    ClassDB::bind_method(D_METHOD("on_body_entered", "body"), &Listener::on_body_entered);
    ClassDB::bind_method(D_METHOD("on_body_exited", "body"), &Listener::on_body_exited);

    ClassDB::bind_method(D_METHOD("can_die", "dash"), &Listener::can_die);
    
}

Listener::Listener()
{
}

Listener::~Listener()
{
}

void Listener::set_patrol_speed(const float p_patrol_speed)
{
    patrol_speed = p_patrol_speed;
}

float Listener::get_patrol_speed() const
{
    return patrol_speed;
}

void Listener::set_seek_speed(const float p_seek_speed)
{
    seek_speed = p_seek_speed;
}

float Listener::get_seek_speed() const
{
    return seek_speed;
}

void Listener::set_charge_speed(const float p_charge_speed)
{
    charge_speed = p_charge_speed;
}

float Listener::get_charge_speed() const
{
    return charge_speed;
}

void Listener::_process(double delta)
{
}

void Listener::_physics_process(double delta)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    if (is_first_frame)
    {
        is_first_frame = false; // Due to navigation agent not being properly registered on the first frame.
        return;
    }
    Vector3 velocity = get_velocity();
    if (!is_on_floor()) {
        velocity.y -= gravity * float(delta);
    } else {
        velocity.y = 0;
    }
    set_velocity(velocity);
    state_machine->update(delta);
    move_and_slide();

}

float Listener::lerp(float a, float b, float c)
{
    return (1.0 - c) * a + c * b;
}


void Listener::on_sound_heard(Vector3 position, float intensity)
{
    float distance = (get_global_position() - position).length();
    if (distance > 0)
    {
        float true_intensity = intensity / distance;

        if (sensitivity * true_intensity > 0.8)
        {
            heard_loud = true;
            heard_soft = false;
            seek_position = Vector3(position.x, position.y - 0.2, position.z);
        }
        else if (sensitivity * true_intensity > 0.3 && !heard_loud)
        {
            heard_soft = true;
            can_be_hurt = true;
            seek_position = Vector3(position.x, position.y - 0.2, position.z);
        }
    }
}

String Listener::get_next_after_patrol()
{   
    if (heard_loud){
        set_color(Color(1, 0, 0)); // Turn red
        play_sound("res://Sounds/listener_aggroed.wav");
        
        return "Charge";
    }
    if (heard_soft){
        set_color(Color(1, 1, 0)); // Turn yellow
        can_be_hurt = true;

        play_sound("res://Sounds/curious.wav");
        return "Seek";
    }
    return "Patrol";
}

String Listener::get_next_after_seek()
{
    if(can_feel_player){
        set_color(Color(1, 0, 0)); // Turn red
        can_feel_player = false;
        heard_soft = false;
        heard_loud = false;
        set_blade_color(Color(1, 0, 0)); // Turn blades bright red
        play_blade(1);
        return "Spinning";
    }
    if (heard_loud){
        set_color(Color(1, 0, 0)); // Turn red
        can_feel_player = false;
        play_sound("res://Sounds/listener_aggroed.wav");
        return "Charge";
    }
    if(heard_soft){
        can_be_hurt = true;
        return "Seek";
    }
    can_feel_player = false;
    return "Paused";
    
}

String Listener::get_next_after_charge()
{
    if(heard_loud){
        return "Charge";
    }
    set_blade_color(Color(1, 0, 0)); // Turn bright red
    play_sound("res://Sounds/blade.mp3");
    return "Spinning";
}

String Listener::get_pause_interrupt()
{
    if(heard_loud){
        set_color(Color(1, 0, 0)); // Turn red
        play_sound("res://Sounds/listener_aggroed.wav");
        return "Charge";
    }
    if(heard_soft){
        set_color(Color(1, 1, 0)); // Turn yellow
        can_be_hurt = true;
        play_sound("res://Sounds/curious.wav");
        return "Seek";
    }
    return "Paused";
}


void Listener::do_seek(double delta)
{
    // If we reach the seek position, stop seeking.
    if(abs(seek_position.x - get_global_position().x) < 0.1 && abs(seek_position.z - get_global_position().z) < 0.1 && heard_soft){
        can_be_hurt = true;
        heard_soft = false;
        return;
    }
    
    // Slowl approach the player, with loose turns. 
    nav_agent->set_target_position(seek_position);
    Vector3 current_location = get_global_position();
    Vector3 next_location = nav_agent->get_next_path_position();
    Vector3 target_velocity = (next_location - current_location).normalized() * seek_speed;
    Vector3 new_velocity = get_velocity().move_toward(target_velocity, 0.1);
    set_velocity(new_velocity);
    Vector3 direction = (nav_agent->get_next_path_position() - get_global_position()).normalized();
    float target_angle = atan2(direction.x, direction.z);
    Node3D *mesh = get_node<Node3D>("Mesh");
    float current_angle = mesh->get_global_rotation().y;
    mesh->set_global_rotation(Vector3(0, lerp(current_angle, target_angle, 0.05), 0));
}

float Listener::get_distance(Vector3 a, Vector3 b){
    return sqrt(pow(a.x - b.x, 2) + pow(a.z - b.z, 2));
}

void Listener::do_charge(double delta)
{
    // If the player is close enough, begin to spin.
    if(get_distance(get_global_position(), seek_position) < feel_area_radius && heard_loud){
        heard_loud = false;
        return;
    }

    // Rapidly approach the player, with tight turns. 
    nav_agent->set_target_position(seek_position);
    Vector3 current_location = get_global_position();
    Vector3 next_location = nav_agent->get_next_path_position();
    Vector3 target_velocity = (next_location - current_location).normalized() * charge_speed;
    Vector3 new_velocity = get_velocity().move_toward(target_velocity, 0.5);
    set_velocity(new_velocity);
    Vector3 direction = (nav_agent->get_next_path_position() - get_global_position()).normalized();
    float target_angle = atan2(direction.x, direction.z);
    Node3D *mesh = get_node<Node3D>("Mesh");
    float current_angle = mesh->get_global_rotation().y;
    mesh->set_global_rotation(Vector3(0, lerp(current_angle, target_angle, 0.05), 0));
}

void Listener::on_feel_entered(const Node3D* body){
    // If the player enters the feeling area, set can_feel_player to true.
    if(body->has_meta("is_player")){
        can_feel_player = true;
    }
}

void Listener::on_feel_exited(const Node3D* body){
    if(body->has_meta("is_player")){
        can_feel_player = false;
    }
}

void Listener::set_color(Color color){
    Ref<Mesh> mesh = get_node<Node3D>("Mesh")->get_node<MeshInstance3D>("Body")->get_mesh();
    Ref<StandardMaterial3D> agent_material = mesh->surface_get_material(0); 
    agent_material->set_albedo(color);  
    // Make the agent "shed" the previous color. 
    body_particles->set_emitting(true);
    Ref<StandardMaterial3D> particle_material =  body_particles->get_mesh()->surface_get_material(0);
    particle_material->set_albedo(previous_color);
    previous_color = color;
   
}

void Listener::set_blade_color(Color color){
    Ref<Mesh> mesh = get_node<Node3D>("Mesh")->get_node<MeshInstance3D>("Blade")->get_mesh();
    Ref<StandardMaterial3D> agent_material = mesh->surface_get_material(0); 
    agent_material->set_emission(color);
    agent_material->set_albedo(color);
}

void Listener::on_reset_color(){
    //can_be_hurt = false;
    set_color(Color(0, 1, 0));
    set_blade_color(Color(0.4, 0, 0));
}

void Listener::set_gravity(const float p_gravity)
{
    gravity = p_gravity;
}

float Listener::get_gravity() const
{
    return gravity;
}

void Listener::set_damage(int damage){
    this->damage = damage;
}

int Listener::get_damage() const{
    return damage;
}

void Listener::set_sensitivity(float sensitivity){
    this->sensitivity = sensitivity;
}

float Listener::get_sensitivity() const{
    return sensitivity;
}

void Listener::set_feel_area_radius(float feel_area_radius){
    this->feel_area_radius = feel_area_radius;
}

float Listener::get_feel_area_radius() const{
    return feel_area_radius;
}

void Listener::set_blade_noise(bool play_blade_noise){
    this->play_blade_noise = play_blade_noise;
}

bool Listener::get_blade_noise() const{
    return play_blade_noise;
}

void Listener::set_damage_tick_speed(float damage_tick_speed){
    this->damage_tick_speed = damage_tick_speed;
}

float Listener::get_damage_tick_speed() const{
    return damage_tick_speed;
}


void Listener::play_sound(String path){
    if (can_play_sound) {
        Ref<AudioStream> stream = ResourceLoader::get_singleton()->load(path);
        sound_player->set_stream(stream);
        sound_player->set_pitch_scale(1);
        sound_player->play();
    }
}

void Listener::on_sound_pressed(){
    can_play_sound = !can_play_sound;
}

// Pitch controls the pitch of the blade sound effect.
void Listener::play_blade(float pitch){
   
    if (can_play_sound && play_blade_noise) {
      
        Ref<AudioStream> stream2 = ResourceLoader::get_singleton()->load("res://Sounds/blade.mp3");
        blade_player->set_stream(stream2);
        blade_player->set_pitch_scale(pitch);
        blade_player->play();
        //sound_player->set_pitch_scale(1);
    }
}


void Listener::on_body_entered(const Node3D* body) {
    if (body->has_meta("is_player") && can_be_hurt && player_dash) {
        get_parent()->get_node<Player>("Player")->call("play_sound", "res://Sounds/robot_death.mp3");
        queue_free();
        hide();
        set_physics_process(false);
        set_process_input(false);
        set_process(false);
       
        get_parent()->call("on_singleplayer_win");
    }
}

void Listener::on_body_exited(const Node3D* body) {
    //this = Null;
}

void Listener::can_die(const bool dash) {
    if (dash) {
        player_dash = true;
    } else {
        player_dash = false; 
    }
}