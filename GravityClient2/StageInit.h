///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef StageInit_H_
#define StageInit_H_

#include <vector>
#include "GravityCommon/Stage.h"

namespace bali
{
class StageInit : public Stage
{
public:
    StageInit(Context::Ptr context);
    ~StageInit();

    uint32_t initialize();
    uint32_t doWindowEvent(sf::Event & event);
    uint32_t doUpdate();
    uint32_t doDraw();
    uint32_t cleanup();
private:
    sf::View mainView;
    sf::Clock localInputClock;
    uint32_t mouseInView = true;

};
}//end namespace bali
#endif // StageMainClient_h_
