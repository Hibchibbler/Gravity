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
            angle = 0.0;
        }

        sf::Vector2f rotate(sf::Vector2f v, float angle)
        {
            angle = angle * (3.1415 / 180.0);
            v.x = v.x * cos(angle) - v.y * sin(angle);
            v.y = v.y * sin(angle) + v.y * cos(angle);
            return v;
        }

        void update(sf::Time elapsed)
        {
            position.x += velocity.x * 1 * elapsed.asSeconds();
            position.y += velocity.y * 1 * elapsed.asSeconds();

            float velx = 0.0;
            float vely = 0.0;

            
            //if (abs(velocity.x) < 1)
            velocity.x += acceleration.x *elapsed.asSeconds();
            //if (abs(velocity.y) < 1)
            velocity.y += acceleration.y *elapsed.asSeconds();
            if (jumpApplied)
            {
                vely -= 25;
                jumpApplied = false;
            }
            velocity.x += velx;
            velocity.y += vely;

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
                sf::Vector2f g(0,1);
                g= rotate(g, angle);
                accy += g.y * 100.0;
                accx += g.x * 100.0;
                std::cout << accx << ", " << accy << std::endl;
            }

            acceleration.x = accx;
            acceleration.y = accy;
        }

        bool applyGravity()
        {            
            gravityApplied = true;
            return gravityApplied;
        }

        void releaseGravity()
        {
            gravityApplied = false;
        }

        bool applyMTV()
        {
            mtvApplied = !mtvApplied;
            return mtvApplied;
        }

        void applyJump()
        {
            jumpApplied = true;
        }

        void applyMove()
        {

        }

        void applyGrip()
        {

        }
        void releaseGrip()
        {

        }

        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Vector2f acceleration;
        float angle;

        SAT::MTV currentMTV;
        QuadLayer playerQuads;

        bool gripApplied;
        bool jumpApplied;
        bool moveApplied;

        
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
        
        Player player;

        sf::Clock mainClock;

    };


}//end namespace bali
#endif
