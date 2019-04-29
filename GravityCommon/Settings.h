#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>

namespace bali
{
struct Settings
{
    Settings()
    {
        SHOW_OBSTRUCTION_POLYGON = true;
        SHOW_ENTITY_POLYGON = false;
        SHOW_WAYPOINTS = true;
        AUTO_GRAVITY_PLAYERS = true;
        AUTO_GRAVITY_ENTITIES = true;
        DISABLE_MOUSE_GRAVITY = false;
        SHOW_ENTITY_CENTROID = false;
        ENABLE_HEAD_BUMP_GRAVITY = true;
    }
    bool SHOW_OBSTRUCTION_POLYGON;
    bool SHOW_ENTITY_POLYGON;
    bool SHOW_WAYPOINTS;
    bool SHOW_ENTITY_CENTROID;
    bool AUTO_GRAVITY_PLAYERS;
    bool AUTO_GRAVITY_ENTITIES;
    bool DISABLE_MOUSE_GRAVITY;
    bool ENABLE_HEAD_BUMP_GRAVITY;
};

}

#endif
