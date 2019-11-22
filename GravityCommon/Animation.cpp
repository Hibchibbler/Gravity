#include "Animation.h"
#include <iostream>

namespace bali
{

/////////////////////////////////////////////
// Animation definitions
//
//void Animation::initialize(size_t seqid_, uint32_t runlen_, bool repeat_)
void Animation::initialize(std::string frametagname_, uint32_t runlen_, bool repeat_)
{
    //seqid = seqid_;
    frametagname = frametagname_;
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
        if (elapsed > sf::seconds(0.05f))
        {
            if ((celid + 1) % sequence.cels.size() == 0)
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
    elapsed = sf::Time::Zero;
}

void Animation::stop()
{
    state = Animation::State::STOPPED;
    celid = 0;
}

void Animation::pause()
{
    state = Animation::State::PAUSED;
}

uint32_t Animation::getCurrentCell()
{
    return celid;
}

//uint32_t Animation::getCurrentSequence()
//{
//    return seqid;
//}

/////////////////////////////////////////////
// Wardrobe definitions
//
bool Wardrobe::getAnimation(std::string name, bali::Animation *& animation)
{
    bool res = false;
    auto pair = animations.find(name);
    if (pair != animations.end())
    {
        animation = &pair->second;
        res = true;
    }
    return res;
}

bool Wardrobe::getCell(std::string name, bali::Animation & animation, ASE::Cel & cell)
{
    bool res = false;
    auto pair = animations.find(name);
    if (pair != animations.end())
    {
        auto animation = pair->second;
        // auto sequence = sequences[animation.frametagname];
        //cell = animation.getCurrentCell(); // sequence.cels[animation.celid];
        res = true;
    }
    return res;
}

bool Wardrobe::getSubRect(ASE::Cel & cell, sf::IntRect & subrect, bool hflip)
{
    sf::Vector2f topleft = sf::Vector2f(cell.frame.position.x, cell.frame.position.y);
    sf::Vector2f topright = sf::Vector2f(cell.frame.position.x + cell.frame.size.w, cell.frame.position.y);
    sf::Vector2f bottomright = sf::Vector2f(cell.frame.position.x + cell.frame.size.w, cell.frame.position.y + cell.frame.size.h);
    sf::Vector2f bottomleft = sf::Vector2f(cell.frame.position.x, cell.frame.position.y + cell.frame.size.h);

    subrect.left = static_cast<int>(cell.frame.position.x);
    subrect.top = static_cast<int>(cell.frame.position.y);
    subrect.width = static_cast<int>(cell.frame.size.w);
    subrect.height = static_cast<int>(cell.frame.size.h);

    if (!hflip)
    {

    }
    else
    {
        subrect.left = static_cast<int>(cell.frame.position.x + cell.frame.size.w);
        subrect.width = static_cast<int>(-cell.frame.size.w);
    }
    return true;
}

//bool
//Wardrobe::loadWardrobe(
//    std::map<bali::Behavior::State, std::string> & anibehmap,
//    std::vector<Sequence> & sequences
//)
//{
//    uint32_t index = 0;
//    for (auto seq : sequences)
//    {
//        bali::Animation newani;
//        newani.initialize(index, seq.cels.size(), true);
//        animations[seq.name] = newani;
//        animations[seq.name].start();
//        index++;
//    }
//    return true;
//}

}
