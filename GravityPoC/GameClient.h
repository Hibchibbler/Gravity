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
    private:
        std::vector<Stage::Ptr> gameStages;
        void addStage(Stage::Ptr stage);
    public:
        GameContext* getContext() { return &ctx; }
        bool isInitialized() { return _isInit; }
        bool isDone() { return _isDone; }
    private:
        uint32_t getCurrentStageIndex();
        bool nextStage();
        Stage::Ptr getCurrentStage();
        void initialized() { _isDone = false; }
        void done() { _isDone = true; std::cout << "GameClient - GameClient is done." << std::endl; }
        bool _isDone;
        bool _isInit;
        uint32_t _curStageIndex;
        GameContext ctx;

    private:
    };
}

#endif
