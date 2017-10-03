///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameClient_h_
#define GameClient_h_

#include "Stage.h"
#include "GameContext.h"

namespace bali 
{
    class GameClient
    {
    public:
        GameClient();
        uint32_t initialize();
        uint32_t doProcessing();
        uint32_t cleanup();
    public:
        GameContext* getContext() { return &ctx; }
        bool isInitialized() { return mIsInit; }
        bool isDone() { return mIsDone; }
    private:
        bool mIsDone;
        bool mIsInit;
        uint32_t mCurStageIndex;
        GameContext ctx;

        uint32_t getCurrentStageIndex() { return mCurStageIndex; }
        Stage::Ptr getCurrentStage() { return gameStages[mCurStageIndex]; }
        bool nextStage() {
            gameStages[mCurStageIndex]->cleanup();
            mCurStageIndex++;
            if (mCurStageIndex < gameStages.size()) {
                return true;
            }

            // Game is done because there are no more stages
            done();
            return false;
        }
        void initialized() { mIsDone = false; mIsInit = false; }
        void done() { mIsDone = true; std::cout << "GameClient - GameClient is done." << std::endl; }
    private:
        std::vector<Stage::Ptr> gameStages;
        void addStage(Stage::Ptr stage) { gameStages.push_back(stage); }
    private:
    };
}

#endif
