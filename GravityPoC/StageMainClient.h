#ifndef StageMainClient_h_
#define StageMainClient_h_

#include <vector>
#include "Stage.h"

namespace bali
{
    class StageMainClient : public Stage
    {
    public:
        StageMainClient(GameContext* ctx);
        ~StageMainClient();

        uint32_t initialize();
        uint32_t doRemoteEvent();// (CommEvent & event);
        uint32_t doWindowEvent(sf::Event & event);
        uint32_t doLocalInputs();
        uint32_t doUpdate();
        uint32_t doDraw();
        uint32_t cleanup();
    private:

        //uint32_t addStraightQuad(std::shared_ptr<sf::VertexArray> v, sf::FloatRect c, sf::IntRect t);

        sf::View mainView;
        sf::Clock localInputClock;
        uint32_t mouseInView = true;
        std::vector<sf::VertexArray> layers;
    };
}//end namespace bali
#endif // StageMainClient_h_