#ifndef GAMESERVER_H_
#define GAMESERVER_H_

#include "GravityCommon/Game.h"
#include "GameServerContext.h"
namespace bali
{

class GameServer : public Game
{
public:        
    GameServer(Context::Ptr c);
    uint32_t initialize();
    uint32_t doProcessing();
    uint32_t cleanup();
protected:
private:
};

}

#endif
