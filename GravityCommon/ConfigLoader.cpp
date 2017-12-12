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
    c.MOVE_STRENGTH = 200.0f;
    c.GRAVITY_CONSTANT = 900.0f;
    c.FIXED_DELTA = 1.0f / 2000.0f;
    c.DRAG_CONSTANT = 0.90f;
    c.RESTITUTION = 0.10f;
    c.STATIC_FRICTION_VELOCITY_MAX = 40.0f;
    c.STATIC_FRICTION = 1.65f;
    c.DYNAMIC_FRICTION = 0.05f;
    c.FAST_JUMP_THRESHOLD = 400.0f;
    c.SLOW_THRESHOLD = 2.0f;
    c.JUMP_STRENGTH = 700.0f;
    c.VELOCITY_MAX = 700.0f;
    c.JUMP_VELOCITY_MAX = 100.0f;
    c.FREEFALL_MOVE_STRENGTH = 25.0f;

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
        c.MOVE_STRENGTH = std::stof(configValues["MOVE_STRENGTH"]);
        c.GRAVITY_CONSTANT = std::stof(configValues["GRAVITY_CONSTANT"]);
        c.FIXED_DELTA = std::stof(configValues["FIXED_DELTA"]);
        c.DRAG_CONSTANT = std::stof(configValues["DRAG_CONSTANT"]);
        c.RESTITUTION = std::stof(configValues["RESTITUTION"]);
        c.SLOW_THRESHOLD = std::stof(configValues["SLOW_THRESHOLD"]);
        c.STATIC_FRICTION_VELOCITY_MAX = std::stof(configValues["STATIC_FRICTION_VELOCITY_MAX"]);
        c.STATIC_FRICTION = std::stof(configValues["STATIC_FRICTION"]);
        c.DYNAMIC_FRICTION = std::stof(configValues["DYNAMIC_FRICTION"]);
        c.VELOCITY_MAX = std::stof(configValues["VELOCITY_MAX"]);
        c.JUMP_STRENGTH = std::stof(configValues["JUMP_STRENGTH"]);
        c.JUMP_VELOCITY_MAX = std::stof(configValues["JUMP_VELOCITY_MAX"]);
        c.FREEFALL_MOVE_STRENGTH = std::stof(configValues["FREEFALL_MOVE_STRENGTH"]);
    }
    else
    {
        cout << "Unable to load Physics Configuration file!" << endl;
    }
    return c;
}

KeyboardConfig bali::loadKeyboardConfig(std::string filename)
{
    KeyboardConfig c;
    // Set Defaults
    c.JUMP_KEY = ' ';
    c.JUMP_TIME = 220;
    c.RIGHT_KEY = 'D';
    c.RIGHT_TIME = 0;
    c.LEFT_KEY = 'A';
    c.LEFT_TIME = 0;

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
        
        c.JUMP_KEY = std::strtoul(configValues["JUMP_KEY"].c_str(), NULL, 0);
        c.JUMP_TIME = std::stoi(configValues["JUMP_TIME"]);
        c.RIGHT_KEY = std::strtoul(configValues["RIGHT_KEY"].c_str(), NULL, 0);
        c.RIGHT_TIME = std::stoi(configValues["RIGHT_TIME"]);
        c.LEFT_KEY = std::strtoul(configValues["LEFT_KEY"].c_str(), NULL, 0);
        c.LEFT_TIME = std::stoi(configValues["LEFT_TIME"]);
    }
    else
    {
        cout << "Unable to load Keyboard Configuration file!" << endl;
    }
    return c;
}



