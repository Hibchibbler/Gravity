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
    c.JUMP_VELOCITY_MAX = 700.0f;
    c.FREEFALL_MOVE_STRENGTH = 25.0f;
    c.JUMP_COUNT = 2;
    c.MOVE_VELOCITY_MAX = 800.0f;
    c.CHARGE_VELOCITY_MAX = 600.0f;

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
        c.JUMP_COUNT = std::stol(configValues["JUMP_COUNT"]);
        c.MOVE_VELOCITY_MAX = std::stof(configValues["MOVE_VELOCITY_MAX"]);
        c.CHARGE_VELOCITY_MAX = std::stof(configValues["CHARGE_VELOCITY_MAX"]);
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
    c.ROTATE_RIGHT_KEY = 'E';
    c.ROTATE_RIGHT_TIME = 0;
    c.ROTATE_LEFT_KEY = 'Q';
    c.ROTATE_LEFT_TIME = 0;

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
        c.ROTATE_RIGHT_KEY= std::strtoul(configValues["ROTATE_RIGHT_KEY"].c_str(), NULL, 0);
        c.ROTATE_RIGHT_TIME = std::stoi(configValues["ROTATE_RIGHT_TIME"]);
        c.ROTATE_LEFT_KEY = std::strtoul(configValues["ROTATE_LEFT_KEY"].c_str(), NULL, 0);
        c.ROTATE_LEFT_TIME = std::stoi(configValues["ROTATE_LEFT_TIME"]);
        c.ATTACK_KEY = std::strtoul(configValues["ATTACK_KEY"].c_str(), NULL, 0);
        c.ATTACK_TIME = std::stoi(configValues["ATTACK_TIME"]);
        c.HARPOON_KEY= std::strtoul(configValues["HARPOON_KEY"].c_str(), NULL, 0);
        c.HARPOON_TIME = std::stoi(configValues["HARPOON_TIME"]);

        c.DOWN_KEY = std::strtoul(configValues["DOWN_KEY"].c_str(), NULL, 0);
        c.DOWN_TIME = std::stoi(configValues["DOWN_TIME"]);
        c.UP_KEY = std::strtoul(configValues["UP_KEY"].c_str(), NULL, 0);
        c.UP_TIME = std::stoi(configValues["UP_TIME"]);
        c.RESET_KEY = std::strtoul(configValues["RESET_KEY"].c_str(), NULL, 0);
        c.RESET_TIME = std::stoi(configValues["RESET_TIME"]);

    }
    else
    {
        cout << "Unable to load Keyboard Configuration file!" << endl;
    }
    return c;
}


GeneralConfig bali::loadGeneralConfig(std::string filename)
{
    GeneralConfig c;
    // Set Defaults
    c.SHOW_OBSTRUCTION_POLYGON = 1;
    c.SHOW_ENTITY_POLYGON = 0;
    c.SHOW_WAYPOINTS = 0;
    c.AUTO_GRAVITY_PLAYERS = 1;
    c.AUTO_GRAVITY_ENTITIES = 1;
    c.DISABLE_MOUSE_GRAVITY = 0;
    c.SHOW_ENTITY_CENTROID = 0;
    c.ENABLE_HEAD_BUMP_GRAVITY = 1;
    c.HUNT_THRESHOLD = 64.f;
    c.SEEK_THRESHOLD = 512.f;
    c.ARRIVED_THRESHOLD = 64.f;
    c.SHOW_ENTITY_PATHS = 1;

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
        c.SHOW_OBSTRUCTION_POLYGON = std::stol(configValues["SHOW_OBSTRUCTION_POLYGON"]);
        c.SHOW_ENTITY_POLYGON = std::stol(configValues["SHOW_ENTITY_POLYGON"]);
        c.SHOW_ENTITY_PATHS = std::stol(configValues["SHOW_ENTITY_PATHS"]);
        c.SHOW_WAYPOINTS = std::stol(configValues["SHOW_WAYPOINTS"]);
        c.AUTO_GRAVITY_PLAYERS = std::stol(configValues["AUTO_GRAVITY_PLAYERS"]);
        c.AUTO_GRAVITY_ENTITIES = std::stol(configValues["AUTO_GRAVITY_ENTITIES"]);
        c.DISABLE_MOUSE_GRAVITY = std::stol(configValues["DISABLE_MOUSE_GRAVITY"]);
        c.SHOW_ENTITY_CENTROID = std::stol(configValues["SHOW_ENTITY_CENTROID"]);
        c.ENABLE_HEAD_BUMP_GRAVITY = std::stol(configValues["ENABLE_HEAD_BUMP_GRAVITY"]);
        c.HUNT_THRESHOLD = std::stof(configValues["HUNT_THRESHOLD"]);
        c.ARRIVED_THRESHOLD = std::stof(configValues["ARRIVED_THRESHOLD"]);
        c.SEEK_THRESHOLD = std::stof(configValues["SEEK_THRESHOLD"]);
    }
    else
    {
        cout << "Unable to load General Configuration file!" << endl;
    }
    return c;
}



