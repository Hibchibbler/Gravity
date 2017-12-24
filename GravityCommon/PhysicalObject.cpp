#include "PhysicalObject.h"
#include "Vector2.h"
#include <iostream>
#include <iomanip>

#define MOST_SMALL 0.001f

namespace bali
{

double getTimestamp()
{
    FILETIME t;
    GetSystemTimePreciseAsFileTime(&t);
    //uint64_t timestamp = ((t.dwHighDateTime << 32) | t.dwLowDateTime) / 10000000.0f;
    uint64_t timestamp = ((uint64_t)t.dwHighDateTime << 32) | (uint64_t)t.dwLowDateTime;
    timestamp = (uint64_t)((double)timestamp / 10000.0);
    std::cout << timestamp << std::endl;
    return timestamp;
}
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
    if (abs(v.x) < MOST_SMALL) { v.x = 0.f; }
    if (abs(v.y) < MOST_SMALL) { v.y = 0.f; }

    newCmd.av.delta = v;
    newCmd.timestamp = getTimestamp();
    cmdAddQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}

void PhysicalObject::addSetVelocity(vec::VECTOR2 v)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::SETVELOCITY;
    if (abs(v.x) < MOST_SMALL) { v.x = 0.f; }
    if (abs(v.y) < MOST_SMALL) { v.y = 0.f; }
    newCmd.sv.vel = v;
    newCmd.timestamp = getTimestamp();
    cmdSetQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}

void PhysicalObject::addAddPosition(vec::VECTOR2 v)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::ADDPOSITION;
    if (abs(v.x) < MOST_SMALL) { v.x = 0.f; }
    if (abs(v.y) < MOST_SMALL) { v.y = 0.f; }
    newCmd.ap.delta = v;
    newCmd.timestamp = getTimestamp();
    cmdAddQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}

void PhysicalObject::addSetPosition(vec::VECTOR2 v)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::SETPOSITION;
    if (abs(v.x) < MOST_SMALL) { v.x = 0.f; }
    if (abs(v.y) < MOST_SMALL) { v.y = 0.f; }
    newCmd.sp.pos = v;
    newCmd.timestamp = getTimestamp();
    cmdSetQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}

void PhysicalObject::addSetTargetAngle(float ta, uint32_t g)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::SETTARGETANGLE;
    newCmd.sta.targetangle = ta;
    newCmd.sta.granularity = g;
    newCmd.timestamp = getTimestamp();
    cmdSetQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}

void PhysicalObject::addAddImpulse(vec::VECTOR2 v, float duration_ms)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::SETTARGETANGLE;
    newCmd.ai.force = v;
    newCmd.ai.duration = duration_ms;
    newCmd.timestamp = getTimestamp();
    cmdAddQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}

void PhysicalObject::addMove(float str, vec::VECTOR2 dir, bool grounded)
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::MOVE;
    newCmd.mv.str = str;
    newCmd.mv.dir = dir;
    newCmd.mv.gnd = grounded;
    newCmd.timestamp = getTimestamp();
    cmdAddQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}
void PhysicalObject::addJump(float str, float dur, vec::VECTOR2 dir )
{
    Command newCmd;
    newCmd.code = (uint32_t)Command::Code::JUMP;
    newCmd.jmp.str = str;
    newCmd.jmp.dur = dur;
    newCmd.jmp.cur = 0;
    newCmd.jmp.dir = dir;
    newCmd.timestamp = getTimestamp();
    cmdAddQueue.push(newCmd);
    cmdHistory.push_back(newCmd);
}

bool PhysicalObject::getAddCommand(Command & c)
{
    bool status = false;
    if (!cmdAddQueue.empty())
    {
        c = cmdAddQueue.front();
        cmdAddQueue.pop();
        status = true;
    }
    return status;
}

bool PhysicalObject::getSetCommand(Command & c)
{
    bool status = false;
    if (!cmdSetQueue.empty())
    {
        c = cmdSetQueue.front();
        cmdSetQueue.pop();
        status = true;
    }
    return status;
}

}
