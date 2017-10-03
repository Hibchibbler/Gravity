#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "GameClient.h"
#include "StageMainClient.h"

namespace bali
{
    GameClient::GameClient()
    {
        mIsDone = false;
        mCurStageIndex = 0;
    }

    uint32_t GameClient::initialize()
    {
        ctx.owner = this;
        ctx.screenWidth = 900;
        ctx.screenHeight = 900;
        ctx.window.create(sf::VideoMode(ctx.screenWidth, ctx.screenHeight), "Gravity PoC");

        Stage::Ptr newStage = std::make_shared<StageMainClient>(&ctx);
        addStage(newStage);

        initialized();
        return 0;
    }

    uint32_t GameClient::doProcessing()
    {
        // Is the current stage done?
        if (getCurrentStage()->isDone())
        {// Yes
            std::cout << "GameClient - doProcessing(); Stage #" << getCurrentStageIndex() << " is done." << std::endl;
            nextStage();
        }
        else
        {// No
            // Initialize gameStage if it has not already been initialized.
            if (!getCurrentStage()->isInitialized())
            {
                getCurrentStage()->initialize();
                std::cout << "GameClient - doProcessing(); Stage #" << getCurrentStageIndex() << " is initialized" << std::endl;
            }

            // Process window events, if there are any.
            sf::Event wevent;
            while (ctx.window.pollEvent(wevent))
            {
                getCurrentStage()->doWindowEvent(wevent);
                if (wevent.type == sf::Event::Resized)
                {
                    ctx.screenWidth = wevent.size.width;
                    ctx.screenHeight = wevent.size.height;
                    std::cout << "GameClient - Window Resized; " << ctx.screenWidth << ", " << ctx.screenHeight << std::endl;
                }
                else if (wevent.type == sf::Event::Closed)
                {
                    ctx.window.close();
                    std::cout << "GameClient - Window Closed " << std::endl;
                    done();
                    return 0;
                }
            }

            // Process local keyboard/mouse inputs
            getCurrentStage()->doLocalInputs();
            // Update and Draw game frame
            getCurrentStage()->doUpdate();
            getCurrentStage()->doDraw();
        }

        return 0;
    }

    uint32_t GameClient::cleanup()
    {
        mCurStageIndex = 0;
        gameStages.clear();
        return 0;
    }
}
