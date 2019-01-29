#ifndef Command_H_
#define Command_H_

#include <SFML\System.hpp>
#include <vector>

namespace bali
{


class Command
{
public:
    Command() {}
    ~Command() {}
    enum Code
    {
        POSITION,
        VELOCITY,
        ACCELERATION,
        ANGLE,
        MOVE,
        JUMP,
        CHARGE,
        GRAVITY,
    };

    enum Priority
    {
        LOW,
        MEDIUM,
        HIGH,
        ULTRA
    };

    class Position {
    public:
        sf::Vector2f pos;
    };

    class Velocity {
    public:
        sf::Vector2f vel;
    };

    class Acceleration {
    public:
        sf::Vector2f accel;
    };

    class Angle {
    public:
        float angle;
    };

    class Move {
    public:
        float        str;
        bool         gnd;
        sf::Vector2f dir;
    };

    class Jump {
    public:
        float str;
        sf::Vector2f dir;
    };

    class Charge
    {
    public:
        float        str;
        bool         gnd;
        sf::Vector2f dir;
    };

    class Gravity
    {
    public:
        float        str;
        sf::Vector2f dir;
    };


    uint64_t timestamp;
    uint32_t code;
    uint32_t priority;
    uint32_t set;//or add

    union
    {
        Position     pos;
        Velocity     vel;
        Acceleration accel;
        Angle        ang;
        Move         mov;
        Jump         jmp;
        Gravity      grv;
        Charge       chg;
    };
};

}

#endif
