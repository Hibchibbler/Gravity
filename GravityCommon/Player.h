#ifndef Player_H_
#define Player_H_

#include "Controller.h"
#include "Entity.h"
#include <stdint.h>

namespace bali
{

class Player
{
public:
    Entity*         entity;
    Controller      controller;
    std::string     name;
    uint64_t        nid;

    enum class LocalState {
        DISCONNECT,
        CONNECT,
        READY,
        START,
        GO
    };

    enum class RemoteState {
        CONNECTED,
        DISCONNECTED
    };

    LocalState cstate;
    RemoteState rstate;

};




}

#endif
