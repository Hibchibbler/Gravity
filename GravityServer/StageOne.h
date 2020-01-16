///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2020
///////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_ONE_H_
#define STAGE_ONE_H_


#include <vector>
#include "GravityCommon/Stage.h"

namespace bali
{

class StageOne : public Stage
{
public:
    StageOne(Context::Ptr context);
    ~StageOne();

    uint32_t initialize();
    uint32_t doWindowEvent(sf::Event & event);
    uint32_t doUpdate();
    uint32_t doDraw();
    uint32_t cleanup();
private:
    //sf::View mainView;
    //sf::Clock localInputClock;
    //uint32_t mouseInView = true;

};

}//end namespace bali

#endif // STAGE_ONE_H_



