///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////

#ifndef GameClientContext_h_
#define GameClientContext_h_

#include "GravityCommon/Context.h"
#include "XMLLoaders/TMX.h"
#include "QuadTree/quadtree.h"
#include "SATAlgo/SATAlgo.h"
#include "GravityCommon/ConfigLoader.h"
#include "GravityCommon/Player.h"
#include "GravityCommon/MouseKeyboard.h"
#include "GravityCommon/Level.h"
#include "XMLLoaders/TextureAtlasLoader.h"
#include "Hud.h"

namespace bali 
{

//struct SingleSprite
//{
//    sf::Image                   img;
//    sf::Texture                 tex;
//    sf::Sprite                  sprite;
//};

class GameClientContext : public Context
{
public:
    //TMX::Map::Ptr               map;

    sf::RenderTexture           renderTexture;
    //sf::RenderTexture*          pRenderTexture0;
    //sf::RenderTexture*          pRenderTexture1;

    sf::Uint32                  centerx;
    sf::Uint32                  centery;
    sf::Vector2f                size;
    sf::Vector2f                worldMousePos;
    sf::Vector2i                screenMousePos;
    sf::Vector2f                cameraPos;
    /*float                       TargetCameraAngle;*/

    sf::Clock                   mainClock;
    sf::Clock                   inputClock;

    bool                        LostFocus;
    bool                        RightMousButtonPressed;
    bool                        LeftMousButtonPressed;

    //Level                       level;
    //Hud                         hud;
};


}//end namespace bali
#endif
