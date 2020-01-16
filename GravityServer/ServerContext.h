///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2020
///////////////////////////////////////////////////////////////////////////////

#ifndef ServerContext_h_
#define ServerContext_h_

#include "GravityCommon/Context.h"


namespace bali
{

    class ServerContext : public Context
    {
    public:
        typedef ServerContext*      Ptr;

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

