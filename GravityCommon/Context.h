///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <stdint.h>
#include "XMLLoaders/TMX.h"
#include "ConfigLoader.h"
//#include "MouseKeyboard.h"
#include "GameWindow.h"
#include "QuadTree/QuadTree.h"
#include "Camera.h"

#include "Entity.h"
#include "Texture.h"
#include "Player.h"
#include "SpatialBuckets.h"
#include "Pathfinding.h"
#include "AIDirector.h"
#include "Networking\ConnMan.h"

namespace bali 
{

//
// Context is an abstract base class
//  and only contains information 
//  common to both Client and Server
//
class Context
{
public:
    typedef Context* Ptr;

    enum Locality {
        LOCAL = 0,
        REMOTE1,
        REMOTE2,
        REMOTE3,
        REMOTE4
    };

    // Networking
    ConnMan         cm;
    bool            isServer;

    Camera          camera;
    GameWindow      gameWindow;

    // Configuration files
    KeyboardConfig  keyboardConfig;
    PhysicsConfig   physicsConfig;
    GeneralConfig   generalConfig;
    NetworkConfig   netConfig;

    // External Resources
    TMX::Map::Ptr   map;
    sf::Font        systemfont;

    // Principle state
    Vec<Proto>      protos;        // stores the prototypes, which have geometry, and sprite sequences.
    Vec<Entity>     entities;      // instances of things in the world ( monsters, items)
    Vec<Player>     remoteplayers; // stores players that currently exist.
    Player          localplayer;   // 

    Vec<Waypoint>   waypoints;     // stores static prototypes.
    Vec<Shape>      allcollisionshapes;

    // Per Frame Spatial partitions
    SpatialBuckets  entitybuckets;
    SpatialBuckets  cpolybuckets;
    Vec<ContactInfo> allcontacts;  // TODO this shouldn't be here

    // Rendering data
    Vec<Tile>       backgroundtiles;
    Vec<Tile>       foregroundtiles;
    Vec<Vertex>     backgroundvertices; // these are updated each frame
    Vec<Vertex>     foregroundvertices;

    Texture         backgroundtilesettexture;
    Texture         foregroundtilesettexture;
    Texture         player0spritesheet;// TODO this isn't right

    sf::RenderTexture canvas;

    // AI Facility
    AIDirector      AIDirector;

    // Crap
    bool            lostfocus;
    sf::Time        frametime;
    sf::Time        frameacc;
    float           mainZoomFactor;
    uint32_t        frames_since_jump;
    sf::Clock       clock;
    bool            paused;
    sf::Time        pausedacc;
    sf::Time        pausedftime;

    sf::Vector2f    zaxpos;//for entity 0, hopefully local player.  hacky.

};




}

#endif

