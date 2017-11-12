///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <stdint.h>
#include <SFML/Graphics.hpp>

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
    typedef void* Owner;
    enum TargetTexture {
        FRONT = 0,
        BACK = 0,

    };
    Owner                       owner;
    uint32_t                    screenWidth;
    uint32_t                    screenHeight;
    sf::RenderWindow            window;

};


}

#endif

