///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameContext_h_
#define GameContext_h_

#include <SFML/Graphics.hpp>
#include <stdint.h>
#include "TMXLoader\TMXReader.h"
#include "QuadTree\quadtree.h"
#include "Utility.h"
#include "Vector\Vector.h"
#include "SATAlgo\SATAlgo.h"

namespace bali 
{
    class Player
    {
    public:
        Player()
        {
            gravityApplied = true;
            angle = 0.0;
            onSolid = false;
        }

        void update(sf::Time elapsed)
        {
            

            //if (updateTime.asMilliseconds() >= 50)
            //{
            float pps = elapsed.asSeconds() * 200;
            position.x += velocity.x * pps;
            position.y += velocity.y * pps;

            float velx = 0.0;
            float vely = 0.0;
            velHist.push_back(velocity);
            if (velHist.size() > 50)
                velHist.erase(velHist.begin());
            velocity.x += acceleration.x * pps;
            velocity.y += acceleration.y * pps;
            if (jumpApplied)
            {
                vely -= 15;
                jumpApplied = false;
            }
            velocity.x = velocity.x + velx - (velocity.x * 0.10);
            velocity.y = velocity.y + vely - (velocity.y * 0.10);

            float accx = 0.0;
            float accy = 0.0;
            if (gravityApplied)// #1
            {
                vec::Vector2 g(0, 1);
                float a = (angle+90)*(3.14156f / 180.0f);
                g.x = g.magnitude() * cos(a);
                g.y = g.magnitude() * sin(a);
                //g = g.rotate(angle+45);
                ////std::cout << " G<" <<g.x << ", " << g.y << " >G" << std::endl;
                accx += g.x ;
                accy += g.y ;
                //std::cout << accx << ", " << accy << std::endl;
            }

            acceleration.x = accx;
            acceleration.y = accy;
            //    updateTime = sf::milliseconds(0);
            //}
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

        void applyJump()
        {
            jumpApplied = true;
        }

        std::vector<vec::Vector2> velHist;
        vec::Vector2 position;
        vec::Vector2 velocity;
        vec::Vector2 acceleration;
        float angle;

        SAT::MTV currentMTV;
        QuadLayer playerQuads;

        bool jumpApplied;
        bool gravityApplied;
        bool onSolid;
        
    private:
        
        sf::Time updateTime;
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
