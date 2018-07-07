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
    Player & p = context->entitymanager.player;
    RigidBody & phys = p.physical;

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
    ctx->level.playerPolygons[0].setPosition(0, 0);

    //
    // Initialize render texture, and shaders
    //
    ctx->hud.HudRenderTexture.create(1024, 1024);
    ctx->level.LevelRenderTexture.create(4096, 4096);

    ctx->level.shader.loadFromFile("gravity_vert.vert", "gravity_frag.frag");

    //
    // Register interesting Keyboard key presses
    //
    ctx->mouseKeyboard.Initialize();
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
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.ATTACK_KEY,
                                        ctx->keyboardConfig.ATTACK_TIME,
                                        input::KeyPressedHandler,
                                        nullptr,
                                        input::KeyHeldHandler,
                                        input::KeyReleasedHandler);
    ctx->mouseKeyboard.registerKeypress(ctx->keyboardConfig.HARPOON_KEY,
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
    context->entitymanager.player.aniMan.addFrames(ctx->level.playerAniTileMap, names);

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
    context->entitymanager.player.physical.angle = 0.0f;
    ctx->cameraPos = context->entitymanager.player.physical.pos;
    ctx->frames_since_jump = 0;
    
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
    context->frames_since_jump++;
    Player & player = context->entitymanager.player;
    ctx->currentElapsed = ctx->mainClock.restart();

    //
    // Smooth out camera 
    //
    ctx->cameraPos += (context->entitymanager.player.physical.pos - ctx->cameraPos) / 125.f;

    //
    // Update Level
    //
    ctx->level.update(vec::VECTOR2(4096, 4096),
                      context->entitymanager.player.physical.pos,
                      context->entitymanager.player.physical.angle);

    //
    // Update HUD text
    //
    ctx->hud.update(ctx->currentElapsed,
                    ctx->cameraPos,
                    context->entitymanager.player.physical.vel,
                    context->entitymanager.player.physical.angle,
                    ctx->mainZoomFactor
                   );

    //
    // Update render and collision polygons for player
    //
    CONVEXSHAPE & ppoly = ctx->level.playerPolygons[0];
    CONVEXSHAPE & cpoly = ctx->level.playerCollisionPolygon;

    // Update render polygon
    ppoly.setPosition(0, 0);
    ppoly.setOrigin(0, 0);
    ppoly.setRotation(player.physical.angle);

    // Get the collision polygon
    // N.B The collision polygon needs to have it's transformations
    //     applied before being used with SAT algorithm.
    cpoly = GetTransformedShape(ppoly);

    // Update position
    cpoly.setPosition(player.physical.pos);
    ppoly.setPosition(player.physical.pos);

    //
    // Update player state, and animation frame
    //
    uint32_t oldState = player.updateState();
    player.aniMan.updateFrames(oldState, (uint32_t)player.state);
    ctx->level.playerPolygons[0].setTexture(&ctx->level.textureAtlas1Tex, true);//TODO: only needs to be done once.
    ani::Frame f = ctx->entitymanager.player.aniMan.animations[(uint32_t)ctx->entitymanager.player.state].getCurrentFrame();
    ctx->level.playerPolygons[0].setTextureRect(f.getIntRect(false, false));

    //
    //
    // Get User Input, and apply
    //
    ctx->mouseKeyboard.Update(ctx->currentElapsed);


    //
    // Collision Detection and Resolution
    //
    physics::ResolveCollisions(context,
                               ctx->level.polygonsVisible, 
                               cpoly,// Collision Polygon
                               ctx->level.sharedEdges, 
                               onCollisionHandler, 
                               onNonCollisionHandler);

    //
    // Motion Integration
    //
    physics::UpdateMotion(context->entitymanager.player.physical,
                          ctx->currentElapsed,
                          context->entitymanager.player.isCollided,
                          ctx->physicsConfig,
                          context->entitymanager.player.accumulator);

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
    sf::View defView = sf::View(vec::VECTOR2(0, 0), 
                                vec::VECTOR2(ctx->gameWindow.screenWidth, ctx->gameWindow.screenHeight));
    defView.setCenter(ctx->cameraPos);
    defView.setRotation(ctx->level.TargetCameraAngle);
    defView.zoom(ctx->mainZoomFactor);

    ctx->level.LevelRenderTexture.clear(sf::Color::Black);
    ctx->level.draw(ctx->level.LevelRenderTexture, levelRS);
    ctx->level.LevelRenderTexture.display();
    ctx->gameWindow.window.setView(defView);
    sf::Sprite levelSprite(ctx->level.LevelRenderTexture.getTexture());
    //levelSprite.setPosition(-2048, -2048);
    ctx->gameWindow.window.draw( levelSprite, levelRS);


    //
    // Draw HUD Texture 
    //
    sf::View hudView = sf::View(vec::VECTOR2(0, 0), 
                                vec::VECTOR2(ctx->gameWindow.screenWidth, ctx->gameWindow.screenHeight));
    hudView.setCenter(ctx->cameraPos);

    ctx->hud.HudRenderTexture.clear(sf::Color::Transparent);
    ctx->hud.draw(ctx->hud.HudRenderTexture, levelRS);
    ctx->hud.HudRenderTexture.display();
    ctx->gameWindow.window.setView(hudView);
    sf::Sprite sprite(ctx->hud.HudRenderTexture.getTexture());
    sprite.setPosition(ctx->cameraPos - (vec::VECTOR2(ctx->gameWindow.screenWidth, 
                       ctx->gameWindow.screenHeight) / 2.0f));
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

    ctx->mouseKeyboard.Cleanup();
    return 0;
}


void StageMainClient::MouseButtonPressHandler(void* ud)
{

}

void StageMainClient::MouseButtonReleaseHandler(void* ud)
{

}

}//end namespace bali
