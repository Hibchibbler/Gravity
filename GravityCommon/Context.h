///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <stdint.h>
#include "XMLLoaders/TMX.h"
#include "ConfigLoader.h"
#include "MouseKeyboard.h"
#include "GameWindow.h"
#include "QuadTree/QuadTree.h"
#include "EntityManager.h"

#include "Entity.h"
#include "Texture.h"

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

    enum TargetTexture {
        FRONT = 0,
        BACK = 0,
    };

    GameWindow      gameWindow;
    MouseKeyboard   mouseKeyboard;
    PhysicsConfig   physicsConfig;
    KeyboardConfig  keyboardConfig;
    EntityManager   entitymanager;
    TMX::Map::Ptr   map;
    sf::Font        systemfont;

    Vec<Entity>     entities;        // holds the entity prototypes. many rigid body may refer to one entity.
    Vec<RigidBody>  bodies;          // holds actualized copies if entities

    Vec<Shape>      collisionshapes; // buildShapes
    Vec<Tile>       backgroundtiles; // buildsf::LineStrip
    Vec<Tile>       foregroundtiles; // sf::LineStrip

    Vec<Vertex>     backgroundvertices; // these are updated each frame
    Vec<Vertex>     foregroundvertices; 

    Texture         backgroundtilesettexture;
    Texture         foregroundtilesettexture;

    sf::RenderTexture canvas;

    bool            lostfocus;
    sf::Time        frametime;
    float           mainZoomFactor;
    uint32_t        frames_since_jump;
    sf::Clock       clock;

};


}

#endif

