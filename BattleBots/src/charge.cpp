#include "charge.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Charge::enter() {
	UtilityFunctions::print("enter charge");
}

void Charge::exit() {
	UtilityFunctions::print("exit charge");
}


std::string Charge::update(double delta) {
    
    String s = agent->call("get_next_after_charge");
    if(s != "Charge"){
        std::string str = s.utf8().get_data();
        return str;
    }
    agent->call("do_charge", delta);
    std::string str = s.utf8().get_data();
    return str;
}

void Charge::init(CharacterBody3D* agent) {
    this->agent = agent;
}
