#include "PhysicalObject.h"
#include "Vector2.h"

namespace bali
{
vec::VECTOR2 PhysicalObject::impulse(vec::VECTOR2 force)
{
    //f = ma Newtons
    //a = f/m meters per meters squared or whatever
    return (force / (float)mass);
}

void PhysicalObject::addAddVelocity(vec::VECTOR2 v)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::ADDVELOCITY;
    newCmd.av.delta = v;
    cmdQueue.push(newCmd);
}

void PhysicalObject::addSetVelocity(vec::VECTOR2 v)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::SETVELOCITY;
    newCmd.sv.vel = v;
    cmdQueue.push(newCmd);
}

void PhysicalObject::addAddPosition(vec::VECTOR2 v)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::ADDPOSITION;
    newCmd.ap.delta = v;
    cmdQueue.push(newCmd);
}

void PhysicalObject::addSetPosition(vec::VECTOR2 v)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::SETPOSITION;
    newCmd.sp.pos = v;
    cmdQueue.push(newCmd);
}
bool PhysicalObject::getCommand(Command & c)
{
    bool status = false;
    if (!cmdQueue.empty())
    {
        c = cmdQueue.front();
        cmdQueue.pop();
        status = true;
    }
    return status;
}

}
