#ifndef EXAMPLE_STATE_H
#define EXAMPLE_STATE_H

#include "state.h"

namespace godot {

class ExampleState : public State {

   public:
    int x = 0;
	void enter() override;
	void exit() override;
    std::string update(double delta) override;
};

}
#endif
