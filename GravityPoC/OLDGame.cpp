#include <memory>
#include <iostream>

#include "Game.h"
#include "Stage.h"


namespace bali {

Game::Game() {
    _curStageIndex = 0;
    _isdone = false;
}

Game::~Game()
{

}

uint32_t Game::initialize()
{

    return 0;
}

uint32_t Game::doProcessing()
{
    //Initialize gameStage if it has not already been initialized.
    if (!getCurrentStage()->isInit())
    {
        std::cout << "Game::doEventProcessing(): Stage (" << getCurrentStageIndex() << ") initialized" << std::endl;
        getCurrentStage()->initialize();
    }

    if (!getCurrentStage()->isDone())
    {
        // Do the do
        getCurrentStage()->doUpdate();
        getCurrentStage()->doDraw();
    }
    else
    {
        //Stage Finished
        std::cout << "Game::doGameProcessing(): Stage (" << getCurrentStageIndex() << ") finished" << std::endl;
        getCurrentStage()->cleanup();
        if (nextStage() == false)
        {
            done();
        }
    }
    return 0;
}

uint32_t Game::cleanup()
{
    gameStages.clear();
    _curStageIndex = 0;
    return 0;
}

void Game::add(std::shared_ptr<Stage> stage) {
    gameStages.push_back(stage);
}

bool Game::nextStage()
{
    if (_curStageIndex < gameStages.size()) {
        _curStageIndex++;
        return true;
    }
    return false;
}

std::shared_ptr<Stage> Game::getCurrentStage()
{
    return gameStages[_curStageIndex];
}

}