///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2020
///////////////////////////////////////////////////////////////////////////////

#ifndef StageLobby_H_
#define StageLobby_H_

#include <vector>
#include "GravityCommon/Stage.h"

namespace bali
{
    class StageLobby : public Stage
    {
    public:
        StageLobby(Context::Ptr context);
        ~StageLobby();

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
#endif // StageLobby_H_
