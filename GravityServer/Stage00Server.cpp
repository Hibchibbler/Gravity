#include "Stage00Server.h"
#include "GameServerContext.h"
#include <SFML/Window.hpp>

namespace bali
{
Stage00Server::Stage00Server()
{
}

Stage00Server::~Stage00Server()
{
}

uint32_t Stage00Server::initialize(Context::Ptr context)
{
    GameServerContext::Ptr ctx = (GameServerContext::Ptr)context;
    initialized();
    return 0;
}

uint32_t Stage00Server::doRemoteEvent(Context::Ptr ctx)
{
    return 0;
}

uint32_t Stage00Server::doWindowEvent(Context::Ptr context, sf::Event & event)
{
    return 0;
}

uint32_t Stage00Server::doLocalInputs(Context::Ptr context)
{
    return 0;
}


uint32_t Stage00Server::doUpdate(Context::Ptr context)
{
    return 0;
}

uint32_t Stage00Server::doDraw(Context::Ptr context)
{
    return 0;
}

uint32_t Stage00Server::cleanup(Context::Ptr context)
{
    return 0;
}

}
