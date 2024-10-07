#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>


namespace godot {

class Waypoint : public Node3D {
    GDCLASS(Waypoint, Node3D)
   private:
   
   protected:
    static void _bind_methods();

   public:
	
    Waypoint();
    ~Waypoint();
	
};

}
#endif