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
    state = State::BIRTH;
    disposition = Disposition::AGGRESSIVE;
    specie = Specie::BIPED;
}


}