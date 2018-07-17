///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
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
        Game(std::unique_ptr<Context> c);
        virtual uint32_t  initialize();
        virtual uint32_t  doProcessing();
        virtual uint32_t  cleanup();
    public:
        Context* getContext();
        bool        isDone();
    protected:
        std::unique_ptr<Context> context;
    private:
        bool        mIsDone;
        bool        mIsInit;
        uint32_t    mCurStageIndex;
        uint32_t    getCurrentStageIndex();
        Stage*      getCurrentStage();
        bool        nextStage();
        void        done();
    protected:
        std::vector<std::unique_ptr<Stage>> gameStages;

    };
}

#endif
