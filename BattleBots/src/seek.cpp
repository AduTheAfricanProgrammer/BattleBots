#include "seek.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Seek::enter() {
	UtilityFunctions::print("enter seek");
}

void Seek::exit() {
	UtilityFunctions::print("exit seek");
}

float Seek::lerp(float a, float b, float c){
    return (1.0 - c) * a + c * b;
}

std::string Seek::update(double delta) {
    String s = agent->call("get_next_after_seek");
    if(s != "Seek"){
        std::string str = s.utf8().get_data();
        return str;
    }
    agent->call("do_seek", delta);
   
    // Convert s into a std::string
    std::string str = s.utf8().get_data();
    return str;
}

void Seek::init(CharacterBody3D* agent) {
    this->agent = agent;
}
