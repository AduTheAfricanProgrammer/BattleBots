#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <memory>
#include <unordered_map>

#include "state.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class StateMachine {
   private:
    std::unordered_map<std::string, std::shared_ptr<State>>& states;

   public:
    std::string curr_state;
    StateMachine(std::unordered_map<std::string, std::shared_ptr<State>>& p_states, std::string p_curr_state);
    ~StateMachine();
    void update(double delta);
    void transition(std::string next_state);
};

}  // namespace godot
#endif
