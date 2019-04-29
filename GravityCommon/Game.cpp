#include "Game.h"
//#include "StageMainClient.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "Context.h"


namespace bali
{
    Game::Game(std::unique_ptr<Context> c)
        : context(std::move(c))
    {
        mIsDone = false;
        mCurStageIndex = 0;
    }

    Context* Game::getContext()
    {
        return context.get();
    }

    uint32_t Game::initialize()
    {
        mIsDone = false; 
        mCurStageIndex = 0;
        getCurrentStage()->initialize();
        std::cout << "Game - doProcessing(); Stage #" << getCurrentStageIndex() << " is initialized" << std::endl;
        return 0;
    }

    uint32_t Game::doProcessing()
    {
        // Is the current stage done?
        if (getCurrentStage()->isDone())
        {// Yes
            std::cout << "Game - doProcessing(); Stage #" << getCurrentStageIndex() << " is done." << std::endl;
            getCurrentStage()->cleanup();
            if (nextStage())
            {
                getCurrentStage()->initialize();
                std::cout << "Game - doProcessing(); Stage #" << getCurrentStageIndex() << " is initialized" << std::endl;
            }
        }
        else
        {// No
            // Process window events, if there are any.
            sf::Event wevent;
            while (context->gameWindow.window.pollEvent(wevent))
            {
                getCurrentStage()->doWindowEvent(wevent);
                switch (wevent.type)
                {
                case sf::Event::Resized:
                {
                    context->gameWindow.screenWidth = wevent.size.width;
                    context->gameWindow.screenHeight = wevent.size.height;

                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0, 0, wevent.size.width, wevent.size.height);
                    context->gameWindow.window.setView(sf::View(visibleArea));
                    std::cout << "Game - Window Resized; " << context->gameWindow.screenWidth << ", " << context->gameWindow.screenHeight << std::endl;
                    break;
                }
                case sf::Event::Closed:
                {
                    context->gameWindow.window.close();
                    std::cout << "Game - Window Closed " << std::endl;
                    done();
                    return 0;
                    break;
                }
                case sf::Event::LostFocus:
                    context->lostfocus = true;
                    break;
                case sf::Event::GainedFocus:
                    context->lostfocus = false;
                    break;
                case sf::Event::MouseWheelScrolled: {
                    context->mainZoomFactor += wevent.mouseWheelScroll.delta / 5.0f;
                    std::cout << "ZF " << context->mainZoomFactor << " ";
                    break;
                }
                }
            }

            // Update and Draw game frame
            context->frametime = context->clock.restart();
            getCurrentStage()->doUpdate();
            getCurrentStage()->doDraw();
        }

        return 0;
    }

    uint32_t Game::cleanup()
    {
        mCurStageIndex = 0;
        gameStages.clear();
        return 0;
    }


    uint32_t Game::getCurrentStageIndex() 
    {
        return mCurStageIndex; 
    }

    Stage* Game::getCurrentStage() 
    { 
        return gameStages[mCurStageIndex].get(); 
    }
    bool Game::nextStage() 
    {
        //
        // One time through, then done..
        //
        mCurStageIndex++;
        if (mCurStageIndex < gameStages.size()) {
            return true;
        }

        // Game is done because there are no more stages
        done();
        return false;


        ////
        //// Repeat. go back to beginning, when done with last
        ////
        //mCurStageIndex = (mCurStageIndex + 1) % gameStages.size();
        //return true;
    }

    void Game::done() 
    { 
        mIsDone = true; 
        std::cout << "Game - Game is done." << std::endl; 
    }

    bool Game::isDone() 
    {
        return mIsDone; 
    }
}
