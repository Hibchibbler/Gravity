#include "Animation.h"
#include "GravityCommon/Physics.h"

namespace bali
{
namespace ani
{
Animation::Animation(uint32_t maxFrameIndex, uint32_t frameDelay)
{
    initialize(maxFrameIndex, frameDelay);
}

void Animation::initialize(uint32_t maxFrameIndex, uint32_t frameDelay)
{
    this->i = 0;
    this->i_max = maxFrameIndex;
    this->fdelay = frameDelay;
    this->running = false;
}

void Animation::start()
{
    clock.restart();
    this->i = 0;
    this->running = true;
}

void Animation::update()
{
    if (this->running)
    {
        if (clock.getElapsedTime().asMilliseconds() > fdelay)
        {
            i = (i + 1) % i_max;
            clock.restart();
        }
    }
}

void Animation::stop()
{
    this->i = 0;
    this->running = false;
}


Frame Animation::getCurrentFrame(float angle, vec::VECTOR2 velocity)
{
    Frame f = frames[this->i];
    return f;
}

void AnimationManager::addAllFrames(const bali::tilemap::TileMap & tm, const std::vector<struct Layout> & frameLayouts)
{
    for (auto n = frameLayouts.begin(); n != frameLayouts.end(); n++)
    {
        animations[n->state] = ani::Animation(n->len, n->delay);
        for (auto d = 0; d < n->len; d++)
        {
            ani::Frame frame;
            if (n->flipY)
            {
                frame = ani::Frame(
                    tm.layers.back().tiles[d + n->start].x * tm.tilewidth + tm.tilewidth,
                    tm.layers.back().tiles[d + n->start].y * tm.tileheight,
                    tm.tilewidth * -1.0f,
                    tm.tileheight);
            }
            else
            {
                frame = ani::Frame(
                    tm.layers.back().tiles[d + n->start].x * tm.tilewidth,
                    tm.layers.back().tiles[d + n->start].y * tm.tileheight,
                    tm.tilewidth,
                    tm.tileheight);
            }

            animations[n->state].frames.push_back(frame);
        }
    }
}


}
}

