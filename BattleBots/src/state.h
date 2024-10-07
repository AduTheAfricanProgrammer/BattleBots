#ifndef STATE_H
#define STATE_H

#include <string>

namespace godot {

class State {

   public:
	virtual void enter() = 0;
	virtual void exit() = 0;
    virtual std::string update(double delta) = 0;
};

}
#endif
