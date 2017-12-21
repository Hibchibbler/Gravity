#ifndef STAGE00SERVER_H_
#define STAGE00SERVER_H_

#include <vector>
#include "GravityCommon/Stage.h"

namespace bali
{
class Stage00Server : public Stage
{
public:
    Stage00Server();
    ~Stage00Server();

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
protected:
private:
};

}

#endif
