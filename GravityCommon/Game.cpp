#include "Game.h"
//#include "StageMainClient.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "Context.h"


namespace bali
{
    Game::Game(Context::Ptr c)
    {
        mIsDone = false;
        mCurStageIndex = 0;
        context = c;
    }

    Context::Ptr Game::getContext()
    {
        return context;
    }

    uint32_t Game::initialize()
    {
        Context* ctx = (Context*)context;

        initialized();
        return 0;
    }

    uint32_t Game::doProcessing()
    {
        Context* ctx = context;
        // Is the current stage done?
        if (getCurrentStage()->isDone())
        {// Yes
            std::cout << "GameClient - doProcessing(); Stage #" << getCurrentStageIndex() << " is done." << std::endl;
            getCurrentStage()->cleanup(ctx);
            nextStage();
        }
        else
        {// No
         // Initialize gameStage if it has not already been initialized.
            if (!getCurrentStage()->isInitialized())
            {
                getCurrentStage()->initialize(ctx);
                std::cout << "GameClient - doProcessing(); Stage #" << getCurrentStageIndex() << " is initialized" << std::endl;
            }

            // Process window events, if there are any.
            sf::Event wevent;
            while (ctx->gameWindow.window.pollEvent(wevent))
            {
                getCurrentStage()->doWindowEvent(ctx, wevent);
                if (wevent.type == sf::Event::Resized)
                {
                    ctx->gameWindow.screenWidth = wevent.size.width;
                    ctx->gameWindow.screenHeight = wevent.size.height;
                    std::cout << "GameClient - Window Resized; " << ctx->gameWindow.screenWidth << ", " << ctx->gameWindow.screenHeight << std::endl;
                }
                else if (wevent.type == sf::Event::Closed)
                {
                    ctx->gameWindow.window.close();
                    std::cout << "GameClient - Window Closed " << std::endl;
                    done();
                    return 0;
                }
            }

            // Update and Draw game frame
            getCurrentStage()->doUpdate(ctx);
            getCurrentStage()->doDraw(ctx);
        }

        return 0;
    }

    uint32_t Game::cleanup()
    {
        Context* ctx = context;
        mCurStageIndex = 0;
        gameStages.clear();
        return 0;
    }


    uint32_t Game::getCurrentStageIndex() 
    {
        return mCurStageIndex; 
    }

    Stage::ShPtr Game::getCurrentStage() 
    { 
        return gameStages[mCurStageIndex]; 
    }
    bool Game::nextStage() 
    {
        mCurStageIndex++;
        if (mCurStageIndex < gameStages.size()) {
            return true;
        }

        // Game is done because there are no more stages
        done();
        return false;
    }
    void Game::initialized() 
    { 
        mIsDone = false; mIsInit = false; 
    }
    void Game::done() 
    { 
        mIsDone = true; std::cout << "GameClient - GameClient is done." << std::endl; 
    }

    bool Game::isInitialized() 
    { 
        return mIsInit; 
    }
    bool Game::isDone() 
    {
        return mIsDone; 
    }
    void Game::addStage(Stage::ShPtr stage) 
    {
        gameStages.push_back(stage); 
    }
}
