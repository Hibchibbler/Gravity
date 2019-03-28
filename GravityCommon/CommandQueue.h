#ifndef CommandQueue_h_
#define CommandQueue_h_

#include <SFML\System.hpp>
#include "Command.h"
#include <queue>
#include <stdint.h>


namespace bali
{

class RigidBody;
class CommandQueue
{
public:
    static double getTimestamp();
    static void postModifyAcceleration(RigidBody & rb, sf::Vector2f a, uint32_t set);
    static void postModifyVelocity(RigidBody & rb, sf::Vector2f v, uint32_t set);
    static void postModifyPosition(RigidBody & rb, sf::Vector2f p, uint32_t set);
    static void postModifyAngle(RigidBody & rb, float ta, uint32_t set);
    static void postMove(RigidBody & rb, float str, sf::Vector2f dir, bool grounded);
    static void postCharge(RigidBody & rb, float str, sf::Vector2f dir, bool grounded);
    static void postJump(RigidBody & rb, float str, sf::Vector2f dir);
    static void postModifyGravity(RigidBody & rb, float str, sf::Vector2f dir);
    bool nextCommand(RigidBody & ph, Command & c);

private:
    struct CompareCommandPriority {
        bool operator()(Command const & p1, Command const & p2) {
            // return "true" if "p1" is ordered before "p2"
            return p1.priority < p2.priority;
        }
    };
    std::priority_queue<Command, std::vector<Command>, CompareCommandPriority>  cmds;
};

}

#endif
