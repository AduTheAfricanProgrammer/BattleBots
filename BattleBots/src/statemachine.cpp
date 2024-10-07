#include "statemachine.h"

#include <string>

#include "example_state.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void StateMachine::update(double delta) {
   
    std::string next_state = states[curr_state]->update(delta);
      
    // If transition, update new state instead
    // note: this implies that if a state is going to transition
    // it should return before it performs any updates
    // so that only one state is actually run per frame
    while (next_state.compare(curr_state)) {
        transition(next_state);
        next_state = states[curr_state]->update(delta);
    }
}

void StateMachine::transition(std::string next_state) {
    states[curr_state]->exit();
    states[next_state]->enter();
    curr_state = next_state;
}

StateMachine::StateMachine(std::unordered_map<std::string, std::shared_ptr<State>>& p_states, std::string p_curr_state) : states(p_states) {
    // states = {
    //     {"ExampleState", std::make_shared<ExampleState>(ExampleState())},
    // };
    curr_state = p_curr_state;

}

StateMachine::~StateMachine() {
}