///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef Game_h_
#define Game_h_

#include "Stage.h"
#include <stdint.h>
#include "Context.h"

namespace bali
{
    class Game
    {
    public:
        Game(Context::Ptr c);
        virtual uint32_t  initialize();
        virtual uint32_t  doProcessing();
        virtual uint32_t  cleanup();
    public:
        Context::Ptr getContext();
        bool        isInitialized();
        bool        isDone();
    protected:
        Context::Ptr context;
    private:
        bool        mIsDone;
        bool        mIsInit;
        uint32_t    mCurStageIndex;
        uint32_t    getCurrentStageIndex();
        Stage::ShPtr  getCurrentStage();
        bool        nextStage();
        void        initialized();
        void        done();
    private:
        std::vector<Stage::ShPtr> gameStages;
    public:
        void        addStage(Stage::ShPtr stage);
    };
}

#endif
