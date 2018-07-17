///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameClient2_h_
#define GameClient2_h_

#include "GravityCommon/Game.h"
#include "ClientContext.h"

namespace bali
{
class GameClient : public Game
{
public:
    GameClient(std::unique_ptr<Context> c);
    uint32_t initialize();
    uint32_t doProcessing();
    uint32_t cleanup();
};
}

#endif
