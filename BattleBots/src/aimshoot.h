#ifndef AIM_SHOOT_H
#define AIM_SHOOT_H

#include <string>
#include <unordered_map>
#include "state.h"
#include "player.h"
#include "barbarian.h"


// The aim state. Used when the projectile enemy aims its attack

namespace godot
{

    class AimShoot : public State
    {
    private:
        Barbarian* agent;
        Player* player;
		int frame;
        int tick;
    public:
        void enter() override;
        void exit() override;
        std::string update(double delta) override;
        void init(Barbarian* agent, Player* player);
    };

}
#endif