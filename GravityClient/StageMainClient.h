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
    class StageMainClient : public Stage
    {
    public:
        StageMainClient();
        ~StageMainClient();

        uint32_t initialize(Context::Ptr c);
        uint32_t doWindowEvent(Context::Ptr c, sf::Event & event);
        uint32_t doUpdate(Context::Ptr c);
        uint32_t doDraw(Context::Ptr c);
        uint32_t cleanup(Context::Ptr c);
    private:
        sf::View mainView;
        sf::Clock localInputClock;
        uint32_t mouseInView = true;
        std::vector<sf::VertexArray> layers;
    private:
        void MouseButtonPressHandler(void* ud);
        void MouseButtonReleaseHandler(void* ud);

        //sf::View updateView(Context::Ptr context);
    };
}//end namespace bali
#endif // StageMainClient_h_
