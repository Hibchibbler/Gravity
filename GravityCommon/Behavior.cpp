#include "Behavior.h"
#include <iostream>

namespace bali
{

Behavior::Behavior()
{
    initialize();
}

void Behavior::initialize()
{
    lifestate = LifeCycleState::BIRTH;
    disposition = Disposition::WANDERING;
    specie = Specie::BIPED;
}


}