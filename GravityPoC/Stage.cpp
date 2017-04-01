#include "Stage.h"

namespace bali
{

    Stage::Stage(GameContext* ctx)
        
    {
        _ctx = ctx;//reference to owner being set.
        _isDone = false;
        _isInit= false;
    }
    Stage::~Stage()
    {

    }

}//end namespace bali