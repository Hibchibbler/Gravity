#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include <map>
#include <SFML\Graphics.hpp>
#include "Sequence.h"
#include "Behavior.h"


namespace bali
{
struct ABMapElement
{
    Behavior::State behaviorstate;
    std::string sequencename;
};

class Animation
{
public:
    enum State
    {
        STOPPED,
        STARTED,
        PAUSED
    };
    Animation()
    {
        runlen = 0;
        repeat = false;
        state = Animation::State::STOPPED;
        celid = 0;
    }

    //void initialize(size_t seqid_, uint32_t runlen_, bool repeat_);
    void initialize(std::string frametagname_, uint32_t runlen_, bool repeat_);
    void update(sf::Time ftime);
    void start();
    void stop();
    void pause();

    uint32_t getCurrentCell();
public:
    std::string frametagname;
    uint32_t celid;     // Cel id, refers to a specific cell in a specific sequence,
                        // use to keep track of which frame is currently on

    Sequence sequence;
    sf::Time elapsed;   // Used to track time. We will update a  
    uint32_t runlen;    // how many cells
    bool     repeat;
    State    state;
};

class Wardrobe
{
public:
    //bool
    //loadWardrobe(
    //    std::map<bali::Behavior::State, std::string> & anibehmap,
    //    std::vector<Sequence> & sequences
    //);

    bool getAnimation(std::string name, bali::Animation *& animation);
    bool getCell(std::string name, bali::Animation & animation, ASE::Cel & cell);
    bool getSubRect(ASE::Cel & cell, sf::IntRect & subrect, bool hflip);

    std::map<std::string, bali::Animation> animations;
};

}

#endif

