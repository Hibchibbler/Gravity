///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageMainClient.h"
#include <math.h>
#include "TMXloader/TMXReader.h"
#include "SATAlgo/SATAlgo.h"
#include "GravityCommon/Physics.h"
#include "GravityCommon/ConfigLoader.h"
#include "GravityCommon/Builders.h"
#include "GravityCommon/MouseKeyboard.h"
#include "GameClientContext.h"
#include "Input.h"
#include "Collision.h"
#include  <assert.h>

#define PIXELS_PER_SEC  4


namespace bali
{
StageMainClient::StageMainClient()
{
}

StageMainClient::~StageMainClient()
{
}

uint32_t StageMainClient::initialize(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    Player & p = ctx->player;
    Physical & phys = p.physical;

    //
    // Load Configuration files for tweakable settings.
    //
    ctx->physicsConfig = loadPhysicsConfig("physics.config.txt");
    ctx->keyboardConfig = loadKeyboardConfig("keyboard.config.txt");

    //
    // Load Assets, and build Level
    //
    ctx->level.initialize(
        "neuropol x rg.ttf",
        "level_test1.tmx"
        );

    //
    // Initialize HUD 
    //
    ctx->hud.initialize(&ctx->level.font);

    //
    // Starting Position of Player is specified in the level data
    //
    phys.pos = ctx->level.playerPolygons[0].getPosition();

    //
    // Initialize render texture, and shaders
    //
    ctx->hud.HudRenderTexture.create(1024, 1024);
    ctx->level.LevelRenderTexture.create(4096, 4096);

    ctx->level.shader.loadFromFile("gravity_vert.vert", "gravity_frag.frag");

    //
    // Register interesting Keyboard key presses
    //
    ctx->mouseKeyboard.setUserData(context);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.JUMP_KEY, 
                                        ctx->keyboardConfig.JUMP_TIME, 
                                        input::KeyPressedHandler,
                                        nullptr,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.RIGHT_KEY, 
                                        ctx->keyboardConfig.RIGHT_TIME,
                                        input::KeyPressedHandler,
                                        input::KeyDblPressedHandler,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.LEFT_KEY, 
                                        ctx->keyboardConfig.LEFT_TIME,
                                        input::KeyPressedHandler,
                                        input::KeyDblPressedHandler,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.ROTATE_RIGHT_KEY,
                                        ctx->keyboardConfig.ROTATE_RIGHT_TIME,
                                        input::KeyPressedHandler,
                                        nullptr,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.ROTATE_LEFT_KEY,
                                        ctx->keyboardConfig.ROTATE_LEFT_TIME,
                                        input::KeyPressedHandler,
                                        nullptr,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.ATTACK_BUTTON,
                                        ctx->keyboardConfig.ATTACK_TIME,
                                        input::KeyPressedHandler,
                                        nullptr,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.HARPOON_BUTTON,
                                        ctx->keyboardConfig.HARPOON_TIME,
                                        input::KeyPressedHandler,
                                        nullptr,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);


    //
    // Prepare the animations for each of the different player states.
    //
    std::vector<struct ani::AnimationManager::Layout> names =
    {
      { (uint32_t)Player::State::IDLERIGHT,     0,   3, 200, 0, 1 },
      { (uint32_t)Player::State::IDLELEFT,      0,   3, 200, 1, 1 },
      { (uint32_t)Player::State::RIGHTWARDS,    4,  10,  69, 0, 1 },
      { (uint32_t)Player::State::LEFTWARDS,     4,  10,  69, 1, 1 },
      { (uint32_t)Player::State::JUMPINGRIGHT,  14,  3, 200, 0, 0 },
      { (uint32_t)Player::State::JUMPINGLEFT,   14,  3, 200, 1, 0 },
      { (uint32_t)Player::State::FALLINGRIGHT,  18,  1, 100, 0, 1 },
      { (uint32_t)Player::State::FALLINGLEFT,   18,  1, 100, 1, 1 },
      { (uint32_t)Player::State::CHARGINGRIGHT, 22,  2, 50, 0, 0 },
      { (uint32_t)Player::State::CHARGINGLEFT,  22,  2, 50, 1, 0 }
    };
    ctx->player.aniMan.addFrames(ctx->level.playerAniTileMap, names);

    //
    // Last things
    //
    ctx->LostFocus = false;
    ctx->RightMousButtonPressed = false;
    ctx->LeftMousButtonPressed = false;
    ctx->mainZoomFactor = 1.0f;
    ctx->mainClock.restart();
    sf::FloatRect visibleArea(0, 0, 900, 900);
    ctx->gameWindow.window.setView(sf::View(visibleArea));
    ctx->level.TargetCameraAngle = 0.f;
    ctx->player.physical.angle = 0.0f;
    ctx->cameraPos = ctx->player.physical.pos;
    
    
    initialized();
    return 0;
}

uint32_t StageMainClient::doWindowEvent(Context::Ptr context, sf::Event & event)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    static float zoom = 1.0;
    switch (event.type) {
    case sf::Event::LostFocus:
        ctx->LostFocus = true;
        break;
    case sf::Event::GainedFocus:
        ctx->LostFocus = false;
        break;
    case sf::Event::MouseWheelScrolled: {
        ctx->mainZoomFactor += event.mouseWheelScroll.delta / 5.0f;
        std::cout << "ZF " << ctx->mainZoomFactor << " ";
        break;
    }case sf::Event::Resized: {
        ctx->gameWindow.screenWidth = event.size.width;
        ctx->gameWindow.screenHeight = event.size.height;

        // update the view to the new size of the window
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        ctx->gameWindow.window.setView(sf::View(visibleArea));
        std::cout << "[" << visibleArea.left << ", " <<visibleArea.top << ", " << visibleArea.width << ", " << visibleArea.height << "] ";
        break;
    }case sf::Event::MouseMoved: {
        break;
    }case sf::Event::KeyPressed:
        break;
    case sf::Event::KeyReleased:
        break;
    case sf::Event::Closed:
        break;
    }
    return 0;
}


#include <sstream>
uint32_t StageMainClient::doUpdate(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    Player & player = ctx->player;
    ctx->currentElapsed = ctx->mainClock.restart();

    //
    // Smooth out camera using average of past player positions
    //
    for (int d = 0; d < ctx->player.posHist.size(); d++) { ctx->cameraPos += ctx->player.posHist[d]; }
    if (ctx->player.posHist.size() > 0)
    {
        ctx->cameraPos /= (float)ctx->player.posHist.size();
    }

    //
    // Update Level
    //
    sf::View wView = ctx->gameWindow.window.getView();
    ctx->level.update(vec::VECTOR2(4096, 4096),//ctx->gameWindow.window.getView().getSize(),
                      ctx->currentElapsed,
                      ctx->player.physical.pos,//ctx->worldMousePos,
                      ctx->player.physical.vel,
                      ctx->player.physical.angle,
                      ctx->mainZoomFactor
                     );

    //
    // Update HUD text
    //
    ctx->hud.update(physics::upVector(ctx->player.physical.angle),
        physics::leftVector(ctx->player.physical.angle),
        ctx->gameWindow.window.getView().getCenter(),
        ctx->gameWindow.window.getView().getSize(),//vec::VECTOR2(ctx->gameWindow.screenWidth, ctx->gameWindow.screenWidth),
        ctx->currentElapsed,
        ctx->cameraPos,//ctx->player.physical.pos,
        ctx->player.physical.vel,
        ctx->player.physical.angle,
        ctx->mainZoomFactor
    );

    //
    // Get User Input, and apply
    //
    ctx->mouseKeyboard.update(ctx->currentElapsed);


    ctx->player.updatePolygon(&ctx->level);


    //
    // Collision Detection and Resolution
    //
    std::stringstream ss1;
    physics::ResolveCollisions(ss1, ctx->level.polygonsVisible, ctx->level.playerCollisionPolygons[0], ctx->player, ctx->physicsConfig, ctx->level.sharedEdges, onCollisionHandler, onNonCollisionHandler);
    //std::cout << ss.str();


    //TODO
    // Update player state, and animation
    //
    uint32_t oldState = player.updateState();
    player.aniMan.updateFrames(oldState, (uint32_t)player.state);

    //
    // Motion Integration
    //
    physics::updatePlayerMotion(ctx->player,
                                ctx->currentElapsed,
                                ctx->physicsConfig);





    //
    // TODO: ?maybe? Slightly resposition the render polygon, based 
    // on the changes that may have occured during
    // collision resolution.
    //

    return 0;
}

uint32_t StageMainClient::doDraw(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    sf::RenderStates levelRS;
    ctx->gameWindow.window.clear(sf::Color::Black);

    //
    // Draw Level Texture
    //
    sf::View defView = sf::View(vec::VECTOR2(0, 0), vec::VECTOR2(ctx->gameWindow.screenWidth, ctx->gameWindow.screenHeight));//ctx->gameWindow.window.getView();
    defView.setCenter(ctx->cameraPos);//defView.setCenter(vec::VECTOR2(ctx->player.physical.pos.x, -(ctx->player.physical.pos.y) + 4096));
    defView.setRotation(ctx->player.physical.angle);
    defView.zoom(ctx->mainZoomFactor);

    ctx->level.LevelRenderTexture.clear(sf::Color::Green);
    ctx->level.draw(ctx->level.LevelRenderTexture, levelRS);
    ctx->level.LevelRenderTexture.display();
    ctx->gameWindow.window.setView(defView);
    ctx->gameWindow.window.draw(sf::Sprite(ctx->level.LevelRenderTexture.getTexture()), levelRS);


    //
    // Draw HUD Texture 
    //
    sf::View hudView = sf::View(vec::VECTOR2(0, 0), vec::VECTOR2(ctx->gameWindow.screenWidth, ctx->gameWindow.screenHeight));
    hudView.setCenter(ctx->cameraPos);

    ctx->hud.HudRenderTexture.clear(sf::Color::Transparent);
    ctx->hud.draw(ctx->hud.HudRenderTexture, levelRS);
    ctx->hud.HudRenderTexture.display();
    ctx->gameWindow.window.setView(hudView);
    sf::Sprite sprite(ctx->hud.HudRenderTexture.getTexture());
    sprite.setPosition(ctx->cameraPos - (vec::VECTOR2(ctx->gameWindow.screenWidth, ctx->gameWindow.screenHeight) / 2.0f));
    ctx->gameWindow.window.draw(sprite, levelRS);


    //
    // And Finally
    //
    ctx->gameWindow.window.display();

    return 0;
}

uint32_t StageMainClient::cleanup(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context; 
    return 0;
}


void StageMainClient::MouseButtonPressHandler(void* ud)
{

}

void StageMainClient::MouseButtonReleaseHandler(void* ud)
{

}

}//end namespace bali
