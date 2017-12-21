///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <stdint.h>

#include "ConfigLoader.h"
#include "MouseKeyboard.h"
#include "GameWindow.h"

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
    GameWindow                  gameWindow;
    MouseKeyboard               mouseKeyboard;
    PhysicsConfig               physicsConfig;
    KeyboardConfig              keyboardConfig;
    //std::vector<Player>         players;
    Player                      player;
    sf::Time                    currentElapsed;
};


}

#endif

