#include "example_state.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void ExampleState::enter() {
	UtilityFunctions::print("enter example state");
}

void ExampleState::exit() {
	UtilityFunctions::print("exit example state");
}

std::string ExampleState::update(double delta) {
	if (x == 100) {
		return "NonexistentState";
	}
	UtilityFunctions::print("update example state");
	UtilityFunctions::print(x);
	x++;
	return "ExampleState";
}