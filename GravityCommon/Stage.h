///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameStage_h_
#define GameStage_h_

#include <SFML/Window.hpp>
#include <memory>
#include "Context.h"


namespace bali
{
//class Game;

class Stage
{

public:
    typedef Stage* Ptr;
    Stage(Context::Ptr context);

    virtual ~Stage();
    virtual uint32_t initialize() = 0;
    virtual uint32_t doWindowEvent(sf::Event & event) = 0;
    virtual uint32_t doUpdate() = 0;
    virtual uint32_t doDraw() = 0;
    virtual uint32_t cleanup() = 0;
public:
    bool isDone() { return mIsDone; }
protected:
    void done() { mIsDone = true; }
    bool mIsDone;
    Context* context;
};

};


#endif
