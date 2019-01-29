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

};




}

#endif
