#ifndef RUN_H
#define RUN_H

#include <string>
#include <unordered_map>
#include "state.h"
#include "player.h"
#include "barbarian.h"


// The aim state. Used when the projectile enemy aims its attack


namespace godot
{

    class Run : public State
    {
    private:
        Barbarian* agent;
        Player* player;
        NavigationAgent3D* nav_agent;
    public:
        void enter() override;
        void exit() override;
        std::string update(double delta) override;
        void init(Barbarian* agent, Player* player, NavigationAgent3D* nav_agent);
    };

}
#endif