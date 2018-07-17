///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////

#ifndef ClientContext_h_
#define ClientContext_h_

#include "GravityCommon/Context.h"
#include "XMLLoaders/TMX.h"
#include "QuadTree/quadtree.h"
#include "SATAlgo/SATAlgo.h"
#include "GravityCommon/ConfigLoader.h"
#include "GravityCommon/Player.h"
#include "GravityCommon/MouseKeyboard.h"
#include "GravityCommon/Level.h"
#include "XMLLoaders/TextureAtlasLoader.h"


namespace bali
{

class ClientContext : public Context
{
public:
    typedef ClientContext*      Ptr;

    sf::RenderTexture           renderTexture;

    sf::Uint32                  centerx;
    sf::Uint32                  centery;
    sf::Vector2f                size;
    sf::Vector2f                worldMousePos;
    sf::Vector2i                screenMousePos;
    sf::Vector2f                cameraPos;

    sf::Font                    systemfont;

    sf::Clock                   mainClock;
    sf::Clock                   inputClock;

    bool                        LostFocus;
    bool                        RightMousButtonPressed;
    bool                        LeftMousButtonPressed;

    //Level                       level;
};


}//end namespace bali
#endif
