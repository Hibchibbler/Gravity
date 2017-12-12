#ifndef Animation_H_
#define Animation_H_

#include <SFML/System/Clock.hpp>
#include <stdint.h>
#include <vector>
#include <map>
#include "TMXLoader/TextureAtlasLoader.h"
#include "Vector2.h"
//#include "Player.h"

namespace bali
{
namespace ani
{

struct Frame
{
    Frame() = default;
    Frame(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    vec::INTRECT getIntRect(bool flipX, bool flipY)
    {
        vec::INTRECT r;
        r.left = x;
        r.top = y;
        r.width = w;
        r.height = h;
        //if (flipY)
        //{
        //    uint32_t temp = r.left;
        //    r.left = r.left + r.width;
        //    r.width = -1.0f * r.width;
        //}
    
        //if (flipX)
        //{
        //    uint32_t temp = r.top;
        //    r.top = r.top + r.height;
        //    r.height = -1.0f * r.height;
        //}
        return r;
    }
    uint32_t x, y;
    uint32_t w, h;
};

class Animation
{
public:
    Animation() = default;
    Animation(uint32_t maxFrameIndex, uint32_t frameDelay);
    void initialize(uint32_t maxFrameIndex, uint32_t frameDelay);
    void start();
    void update();
    void stop();

    Frame getCurrentFrame(float angle, vec::VECTOR2 velocity);

    uint32_t i;
    uint32_t i_max;
    uint32_t fdelay;
    uint32_t running;
    sf::Clock clock;
    sf::Time totalTime;
    std::vector<Frame> frames;
};

class AnimationManager
{
public:
    //Frame getCurrentFrame(float angle, vec::VECTOR2 velocity);
    //Player::State currentAnimation;
    std::map<uint32_t, Animation> animations;
    
};

}
}


#endif


