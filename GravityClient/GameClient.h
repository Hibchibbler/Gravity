///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameClient_h_
#define GameClient_h_

#include "GravityCommon/Game.h"
#include "GameClientContext.h"

namespace bali 
{
    class GameClient : public Game
    {
    public:
        GameClient(Context::Ptr c);
        uint32_t initialize();
        uint32_t doProcessing();
        uint32_t cleanup();

        GameClientContext gcc;
    };
}

#endif
