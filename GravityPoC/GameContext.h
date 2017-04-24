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
            angle = 0.0;
            onSolid = false;
        }

        sf::Vector2f rotate(sf::Vector2f v, float angle)
        {
            angle = angle * (3.14156f / 180.0f);
            v.x = v.x * cos(angle) - v.y * sin(angle);
            v.y = v.x * sin(angle) + v.y * cos(angle);
            return v;
        }
        sf::Vector2f normalize(sf::Vector2f v)
        {
            double len = sqrt(v.x*v.x + v.y*v.y);
            if (len > 0)
            {
                v.x /= len;
                v.y /= len;
            }
            return v;
        }
        void update(sf::Time elapsed)
        {
            updateTime += elapsed;

            if (updateTime.asMilliseconds() >= 30)
            {
                updateTime = sf::Time::Zero;
                position.x += velocity.x;
                position.y += velocity.y;

                float velx = 0.0;
                float vely = 0.0;
                
                velocity.x += acceleration.x;
                velocity.y += acceleration.y;
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
                    sf::Vector2f g(1, 1);
                    float a = (angle+90)*(3.14156f / 180.0f);
                    //std::cout << " G<" <<g.x* cos(a) << ", " << g.y * sin(a) << " >G" << std::endl;
                    accx += g.x * cos(a);
                    accy += g.y * sin(a);
                    //std::cout << accx << ", " << accy << std::endl;
                }

                acceleration.x = accx;
                acceleration.y = accy;
            }
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

        std::vector<sf::Vector2f> posHist;
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Vector2f acceleration;
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
