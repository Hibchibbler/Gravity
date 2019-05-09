#include "Command.h"
#include "windows.h"
#include "RigidBody.h"

namespace bali
{
bool CommandQueue::nextCommand(RigidBody & rb, Command & c)
{
    bool status = false;
    if (!rb.cmdqueue.cmds.empty())
    {
        c = rb.cmdqueue.cmds.top();
        rb.cmdqueue.cmds.pop();
        status = true;
    }
    return status;
}



double CommandQueue::getTimestamp()
{
    FILETIME t;
    GetSystemTimePreciseAsFileTime(&t);
    //uint64_t timestamp = ((t.dwHighDateTime << 32) | t.dwLowDateTime) / 10000000.0f;
    uint64_t timestamp = ((uint64_t)t.dwHighDateTime << 32) | (uint64_t)t.dwLowDateTime;
    timestamp = (uint64_t)((double)timestamp / 10000.0f);
    //std::cout << timestamp << std::endl;
    return timestamp;
}

void CommandQueue::postModifyAcceleration(RigidBody & rb, sf::Vector2f a, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::ACCELERATION;
    newCmd.priority = static_cast<uint32_t>((set > 0 ? Command::Priority::ULTRA : Command::Priority::HIGH));
    newCmd.set = set;
    newCmd.accel.accel = a;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}

void CommandQueue::postModifyVelocity(RigidBody & rb, sf::Vector2f v, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::VELOCITY;
    newCmd.priority = static_cast<uint32_t>((set > 0 ? Command::Priority::ULTRA : Command::Priority::HIGH));
    newCmd.set = set;
    newCmd.vel.vel = v;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}

void CommandQueue::postModifyPosition(RigidBody & rb, sf::Vector2f p, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::POSITION;
    newCmd.priority = static_cast<uint32_t>((set > 0 ? Command::Priority::ULTRA : Command::Priority::HIGH));
    newCmd.set = set;
    newCmd.pos.pos = p;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}

void CommandQueue::postModifyAngle(RigidBody & rb, float ta, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::ANGLE;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::ULTRA);
    newCmd.set = set;
    newCmd.ang.angle = ta;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}

void CommandQueue::postMove(RigidBody & rb, float str, sf::Vector2f dir, bool grounded)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::MOVE;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::MEDIUM);
    newCmd.mov.str = str;
    newCmd.mov.dir = dir;
    newCmd.mov.gnd = grounded;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}

void CommandQueue::postCharge(RigidBody & rb, float str, sf::Vector2f dir, bool grounded)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::CHARGE;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::MEDIUM);
    newCmd.mov.str = str;
    newCmd.mov.dir = dir;
    newCmd.mov.gnd = grounded;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}

void CommandQueue::postJump(RigidBody & rb, float str, sf::Vector2f dir)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::JUMP;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::MEDIUM);
    newCmd.jmp.str = str;
    newCmd.jmp.dir = dir;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}


void CommandQueue::postModifyGravity(RigidBody & rb, float str, sf::Vector2f dir)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::GRAVITY;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::ULTRA);
    newCmd.grv.str = str;
    newCmd.grv.dir = dir;

    rb.cmdqueue.cmds.push(newCmd);
    //rb.cmdqueue.cmdhist.push(newCmd);
}


}
