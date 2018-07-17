///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef StageMainClient_h_
#define StageMainClient_h_

#include <vector>
#include "GravityCommon/Stage.h"

namespace bali
{
class StageMain : public Stage
{
public:
    StageMain(Context* context);
    ~StageMain();

    uint32_t initialize();
    uint32_t doWindowEvent(sf::Event & event);
    uint32_t doUpdate();
    uint32_t doDraw();
    uint32_t cleanup();
private:
    sf::View mainView;
    sf::Clock localInputClock;
    uint32_t mouseInView = true;

private:
    //void MouseButtonPressHandler(void* ud);
    //void MouseButtonReleaseHandler(void* ud);

    //sf::View updateView(Context::Ptr context);
};
}//end namespace bali
#endif // StageMainClient_h_
