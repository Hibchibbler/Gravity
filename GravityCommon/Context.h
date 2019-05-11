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
    Camera          camera;
    GameWindow      gameWindow;
    KeyboardConfig  keyboardConfig;
    PhysicsConfig   physicsConfig;
    GeneralConfig   generalConfig;

    TMX::Map::Ptr   map;
    sf::Font        systemfont;

    Vec<Proto>      protos;        // stores the entity prototypes. entities refer to these prototypes
                                   // for their default information, and geometry
    Vec<Entity>     entities;      // stores entities that currently exist.
    Vec<Entity>     shadowcopy;

    Vec<Player>     players;       // stores players that currently exist. players[Locality::LOCAL] always exists.
    Vec<Waypoint>   waypoints;     // stores static prototypes.
    SpatialBuckets  entitybuckets;
    SpatialBuckets  cpolybuckets;

    Vec<Shape>      allcollisionshapes;
    Vec<Shape>      collisionshapesvisible;

    Vec<Tile>       backgroundtiles;
    Vec<Tile>       foregroundtiles;
    Vec<ContactInfo> allcontacts;

    Vec<Vertex>     backgroundvertices; // these are updated each frame
    Vec<Vertex>     foregroundvertices;

    Texture         backgroundtilesettexture;
    Texture         foregroundtilesettexture;
    Texture         player0spritesheet;

    ASE::SpriteSheet spritesheet;
    std::vector<bali::Sequence> sequences;

    sf::RenderTexture canvas;

    bool            lostfocus;
    sf::Time        frametime;
    sf::Time        frameacc;
    float           mainZoomFactor;
    uint32_t        frames_since_jump;
    sf::Clock       clock;

    AIDirector      AIDirector;

    bool            paused;
    sf::Time        pausedacc;
    sf::Time        pausedftime;

    sf::Vector2f    zaxpos;//for entity 0, hopefully local player.  hacky.

};




}

#endif

