#include "Animation.h"

namespace bali
{

/////////////////////////////////////////////
// Animation definitions
//
void Animation::initialize(size_t seqid_, uint32_t runlen_, bool repeat_)
{
    seqid = seqid_;
    celid = 0;
    elapsed = sf::Time::Zero;
    runlen = runlen_;
    repeat = repeat_;
    state = Animation::State::STOPPED;
}

void Animation::update(sf::Time ftime)
{
    if (state == Animation::State::STARTED)
    {
        elapsed += ftime;
        if (elapsed.asSeconds() > 0.1f)
        {
            if ((celid + 1) % runlen == 0)
            {
                if (!repeat)
                    return;
            }
            celid = (celid + 1) % runlen;
            elapsed = sf::Time::Zero;
        }
    }
}

void Animation::start()
{
    state = Animation::State::STARTED;
}

void Animation::stop()
{
    state = Animation::State::STOPPED;
}

void Animation::pause()
{
    state = Animation::State::PAUSED;
}

uint32_t Animation::getCurrentCell()
{
    return celid;
}

uint32_t Animation::getCurrentSequence()
{
    return seqid;
}

/////////////////////////////////////////////
// Wardrobe definitions
//
bool Wardrobe::getAnimation(std::string name, bali::Animation & animation)
{
    bool res = false;
    auto pair = animations.find(name);
    if (pair != animations.end())
    {
        animation = pair->second;
        res = true;
    }
    return res;
}

bool Wardrobe::getCell(std::vector<Sequence> & sequences, std::string name, ASE::Cel & cell)
{
    bool res = false;
    auto pair = animations.find(name);
    if (pair != animations.end())
    {
        auto animation = pair->second;
        auto sequence = sequences[animation.seqid];
        cell = sequence.cels[animation.celid];
        res = true;
    }
    return res;
}

bool Wardrobe::getSubRect(ASE::Cel & cell, sf::IntRect & subrect)
{
    subrect.left = cell.frame.position.x;
    subrect.top = cell.frame.position.y;
    subrect.width = cell.frame.size.w;
    subrect.height = cell.frame.size.h;
    return true;
}

}
