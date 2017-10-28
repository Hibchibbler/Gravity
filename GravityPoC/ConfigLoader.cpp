#include "ConfigLoader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
using namespace bali;
using namespace std;

PhysicsConfig bali::loadPhysicsConfig(std::string filename)
{
    PhysicsConfig c;
    // Set Defaults
    c.HORIZONTAL_MOVE_ANGLE_OFFSET = 75;
    c.MOVE_STRENGTH = 50;
    c.GRAVITY_CONSTANT = 900;
    c.FIXED_DELTA = 1.0f / 2000.0f;
    c.DRAG_CONSTANT = 0.90;
    c.RESTITUTION = 0.10f;
    c.FAST_THRESHOLD = 40.0f;
    c.KINETIC_FRICTION = 1.65f;
    c.DYNAMIC_FRICTION = 0.05f;
    c.FAST_JUMP_THRESHOLD = 400.0;
    c.JUMP_STRENGTH = 700.0;
    c.MAX_VELOCITY = 700.0;

    std::ifstream configIn(filename);
    if (configIn.is_open())
    {
        //std::istringstream is_file(configIn.get);

        typedef std::map<std::string, std::string> ConfigInfo;
        ConfigInfo configValues;
        std::string line;
        while (std::getline(configIn, line))
        {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '='))
            {
                std::string value;
                if (key[0] == '#')
                    continue;

                if (std::getline(is_line, value))
                {
                    configValues[key] = value;
                    //cout << "config[\"" << key << "\"] = " << value << endl;
                }
            }
        }

        // Set from config file
        c.HORIZONTAL_MOVE_ANGLE_OFFSET  = std::stoi(configValues["HORIZONTAL_MOVE_ANGLE_OFFSET"]);
        c.MOVE_STRENGTH                 = std::stoi(configValues["MOVE_STRENGTH"]);
        c.GRAVITY_CONSTANT              = std::stoi(configValues["GRAVITY_CONSTANT"]);
        c.FIXED_DELTA                   = std::stof(configValues["FIXED_DELTA"]);
        c.DRAG_CONSTANT                 = std::stof(configValues["DRAG_CONSTANT"]);
        c.RESTITUTION                   = std::stof(configValues["RESTITUTION"]);
        c.FAST_THRESHOLD                = std::stof(configValues["FAST_THRESHOLD"]);
        c.KINETIC_FRICTION              = std::stof(configValues["KINETIC_FRICTION"]);
        c.DYNAMIC_FRICTION              = std::stof(configValues["DYNAMIC_FRICTION"]);
        c.MAX_VELOCITY                  = std::stof(configValues["MAX_VELOCITY"]);
    }
    else
    {
        cout << "Unable to load Physics Configuration file!" << endl;
    }
    return c;
}