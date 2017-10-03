#include "Stage.h"
#include "GameContext.h"
namespace bali
{

    Stage::Stage(GameContext* ctx)
    {
        mCtx = ctx;//reference to owner being set.
        mIsDone = false;
        mIsInit = false;
    }
    Stage::~Stage()
    {

    }
    GameContext* Stage::getContext()
    { 
        return mCtx; 
    }

}//end namespace bali