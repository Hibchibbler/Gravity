#ifndef GameContext_h_
#define GameContext_h_

#include <SFML/Graphics.hpp>
#include <stdint.h>
#include "TMXLoader\TMXReader.h"
#include "QuadTree\quadtree.h"
#include "Utility.h"
#include "SATAlgo\SATAlgo.h"

namespace bali 
{
    class Player
    {
    public:
        Player()
        {
            gravityApplied = true;
            mtvApplied = false;
        }
        void update(sf::Time elapsed)
        {
            position.x += velocity.x *elapsed.asSeconds();
            position.y += velocity.y *elapsed.asSeconds();

            //if (abs(velocity.x) < 1)
            velocity.x += acceleration.x*elapsed.asSeconds();
            //if (abs(velocity.y) < 1)
            velocity.y += acceleration.y*elapsed.asSeconds();
            
            //float velx = 0.0;
            //float vely = 0.0;
            //if (mtvApplied)
            //{
            //    SAT::Vector2 v = currentMTV.smallest.normalize();
            //    if (currentMTV.smallest.x != 0)
            //    {
            //        velx += -v.x * currentMTV.overlap;
            //        //velx += -mtv.smallest.x;

            //    }
            //    if (currentMTV.smallest.y != 0)
            //    {
            //        vely += -v.y * currentMTV.overlap;
            //        //ctx->player.velocity.y = -mtv.smallest.y;
            //    }
            //    mtvApplied = false;
            //}
            //velocity.x = velx;
            //velocity.y = vely;
            
            float accx = 0.0;
            float accy = 0.0;
            if (gravityApplied)// #1
            {
                accy += 50.0;
            }
            acceleration.x = accx;
            acceleration.y = accy;
        }

        bool applyGravity()
        {
            gravityApplied = !gravityApplied;
            return gravityApplied;

        }

        bool applyMTV()
        {
            mtvApplied = !mtvApplied;
            return mtvApplied;
        }

        void applyUser()
        {

        }

        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Vector2f acceleration;

        SAT::MTV currentMTV;
        QuadLayer playerQuads;
    private:
        bool gravityApplied;
        bool mtvApplied;
    };
    
    class GameClient;
    class GameContext
    {
    public:
        GameContext()
            /*:layerBack({0,0,800,600},5),
             layerFore({ 0,0,800,600 }, 5)*/
        {
            angle = 0.0;

        }
    public:
        GameClient*         owner;
        uint32_t            screenWidth;
        uint32_t            screenHeight;
        sf::RenderWindow    window;
        TMX::MapContext     mctx;

        sf::Image           tilesetAImg;
        sf::Texture         tilesetATex;

        sf::Image           backgroundImg;
        sf::Texture         backgroundTex;

        sf::View            mainView;
        sf::Uint32          centerx;
        sf::Uint32          centery;
        sf::Vector2f        size;

        bali::SearchLayers        searchLayers;
        bali::QuadLayers          quadLayers;
        bali::QuadLayer           visibleQuads;
        bali::TileLayers          tileLayers;
        std::vector<ConvexShape>          polygons;

        sf::Transform  levelRotTrans;
        float angle;
        Player player;

        sf::Clock mainClock;

    };


}//end namespace bali
#endif
