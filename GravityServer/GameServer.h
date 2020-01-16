///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2020
///////////////////////////////////////////////////////////////////////////////

#ifndef GameClient2_h_
#define GameClient2_h_

#include "GravityCommon/Game.h"
#include "ServerContext.h"

namespace bali
{
    class GameServer : public Game
    {
    public:
        GameServer(std::unique_ptr<Context> c);
        uint32_t initialize();
        uint32_t doProcessing();
        uint32_t cleanup();
    };
}

#endif

