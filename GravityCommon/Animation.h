#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include <map>
#include <SFML\Graphics.hpp>
#include "Sequence.h"

namespace bali
{

class Animation
{
public:
    enum State
    {
        STOPPED,
        STARTED,
        PAUSED
    };
    void initialize(size_t seqid_, uint32_t runlen_, bool repeat_);
    void update(sf::Time ftime);
    void start();
    void stop();
    void pause();

    uint32_t getCurrentCell();
    uint32_t getCurrentSequence();
public:
    uint32_t seqid;     // Sequence id, refers to a specific sequence
    uint32_t celid;     // Cel id, refers to a specific cell in a specific sequence
    sf::Time elapsed;   // Used to track time. We will update a  
    uint32_t runlen;    // how many cells
    bool     repeat;
    State    state;
};

class Wardrobe
{
public:
    bool getAnimation(std::string name, bali::Animation & animation);
    bool getCell(std::vector<Sequence> & sequences, std::string name, ASE::Cel & cell);
    bool getSubRect(ASE::Cel & cell, sf::IntRect & subrect);

    std::map<std::string, bali::Animation> animations;
};

}

#endif

