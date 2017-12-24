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

    //
    // Load Configuration files for tweakable settings.
    //
    ctx->physicsConfig = loadPhysicsConfig("physics.config.txt");
    ctx->keyboardConfig = loadKeyboardConfig("keyboard.config.txt");

    //
    // Load Fonts
    //
    ctx->font.loadFromFile("neuropol x rg.ttf");
    ctx->fpsText.setFont(ctx->font);

    //
    // Load TMX
    //
    ctx->map = make_shared<bali::TMX::Map>();
    TMX::TMXReader::load("level_test1.tmx", ctx->map);

    //
    // Create Player shape from TMX
    //
    TMX::Objectgroup::Ptr pObjG = ctx->map->getObjectGroup("PlayerShapes");
    ctx->player.pos.x = pObjG->objects.back()->x;
    ctx->player.pos.y = pObjG->objects.back()->y;

    //
    // Set size of main view.
    //
    ctx->size.x = 1000;
    ctx->size.y = 1000;

    //
    // Create Foreground tileset from TMX
    //
    TMX::Tileset::Ptr tilesetA = ctx->map->getTileset("tilesetFore");
    ctx->tilesetAImg.loadFromFile(tilesetA->images.back()->source);
    ctx->tilesetAImg.createMaskFromColor(sf::Color::Black);//transparent is black...
    ctx->tilesetATex.loadFromImage(ctx->tilesetAImg);

    //
    // Create Background tileset from TMX
    //
    TMX::Tileset::Ptr tilesetBkgnd = ctx->map->getTileset("tilesetBack");
    ctx->backgroundImg.loadFromFile(tilesetBkgnd->images.back()->source);
    ctx->backgroundTex.loadFromImage(ctx->backgroundImg);

    //
    // Create internal representation of background geometry from TMX
    //
    ctx->tileLayers.push_back(Tile::Vec());
    buildTileLayer(ctx->tileLayers.back(), tilesetBkgnd, ctx->map->getLayer("BackgroundLayer"));

    //
    // Create renderable background geometry from internal representation
    //
    ctx->quadLayers.push_back(bali::QuadArray());
    buildQuadLayer(ctx->quadLayers.back(), ctx->tileLayers[0], ctx->map->tilewidth, ctx->map->tileheight);

    //
    // Create internal representation of foreground geometry from TMX
    //
    ctx->tileLayers.push_back(Tile::Vec());
    buildTileLayer(ctx->tileLayers.back(), tilesetA, ctx->map->getLayer("ForegroundLayer"));

    //
    // Create renderable foreground geometry from internal representation
    //
    ctx->quadLayers.push_back(bali::QuadArray());
    buildQuadLayer(ctx->quadLayers.back(), ctx->tileLayers[1], ctx->map->tilewidth, ctx->map->tileheight);

    //
    // Create Collision Polygon geometry from TMX
    //
    buildPolygonLayers(ctx->polygons,
        ctx->map->getObjectGroup("CollisionPolygons"));


    buildPolygonLayers(ctx->glowPolygons,
        ctx->map->getObjectGroup("GlowPolygons"));

    //
    // Create player collision polygon geometry from TMX
    //
    buildPlayerObjectLayers(ctx->playerpolygons,
        ctx->map->objectgroups);

    //
    // Create Quadtree for Collision Polygons.
    //
    int maxDepth = 15;
    qt::AABB aabb;
    aabb.min.x = -64;
    aabb.min.y = -64;
    aabb.max.x = aabb.max.y = (ctx->map->width+2) * 32;//in pixels
    qt::QuadTree::ShPtr searchLayer = std::make_shared<qt::QuadTree>();
    searchLayer->initialize(aabb, maxDepth);
    buildSearchLayer(searchLayer, ctx->polygons);
    ctx->searchLayers.push_back(searchLayer);


    //
    // Load parallax background images
    //
    ctx->parallaxBackgrounds[0].img.loadFromFile(ctx->map->imagelayers[0]->image->source);
    ctx->parallaxBackgrounds[0].img.createMaskFromColor(sf::Color::White);
    ctx->parallaxBackgrounds[0].tex.loadFromImage(ctx->parallaxBackgrounds[0].img);
    ctx->parallaxBackgrounds[0].sprite.setTexture(ctx->parallaxBackgrounds[0].tex);
    ctx->parallaxBackgrounds[0].sprite.scale(8.0f, 8.0f);

    ctx->parallaxBackgrounds[1].img.loadFromFile(ctx->map->imagelayers[1]->image->source);
    ctx->parallaxBackgrounds[1].img.createMaskFromColor(sf::Color::White);
    ctx->parallaxBackgrounds[1].tex.loadFromImage(ctx->parallaxBackgrounds[1].img);
    ctx->parallaxBackgrounds[1].sprite.setTexture(ctx->parallaxBackgrounds[1].tex);
    ctx->parallaxBackgrounds[1].sprite.scale(8.0f, 8.0f);

    ctx->parallaxBackgrounds[2].img.loadFromFile(ctx->map->imagelayers[2]->image->source);
    ctx->parallaxBackgrounds[2].img.createMaskFromColor(sf::Color::White);
    ctx->parallaxBackgrounds[2].tex.loadFromImage(ctx->parallaxBackgrounds[2].img);
    ctx->parallaxBackgrounds[2].sprite.setTexture(ctx->parallaxBackgrounds[2].tex);
    ctx->parallaxBackgrounds[2].sprite.scale(8.0f, 8.0f);

    ctx->parallaxBackgrounds[3].img.loadFromFile(ctx->map->imagelayers[3]->image->source);
    ctx->parallaxBackgrounds[3].img.createMaskFromColor(sf::Color::White);
    ctx->parallaxBackgrounds[3].tex.loadFromImage(ctx->parallaxBackgrounds[3].img);
    ctx->parallaxBackgrounds[3].sprite.setTexture(ctx->parallaxBackgrounds[3].tex);
    ctx->parallaxBackgrounds[3].sprite.scale(8.0f, 8.0f);

    ctx->parallaxBackgrounds[4].img.loadFromFile(ctx->map->imagelayers[4]->image->source);
    ctx->parallaxBackgrounds[4].img.createMaskFromColor(sf::Color::White);
    ctx->parallaxBackgrounds[4].tex.loadFromImage(ctx->parallaxBackgrounds[4].img);
    ctx->parallaxBackgrounds[4].sprite.setTexture(ctx->parallaxBackgrounds[4].tex);
    ctx->parallaxBackgrounds[4].sprite.scale(8.0f, 8.0f);

    //
    // Initialize render texture, and shaders
    //
    ctx->pRenderTexture0 = &ctx->renderTextures[0];
    ctx->pRenderTexture0->create(4096, 4096);
    //
    ctx->pRenderTexture1 = &ctx->renderTextures[1];
    ctx->pRenderTexture1->create(4096, 4096);
    ctx->shader.loadFromFile("gravity_vert.vert", "gravity_frag.frag");

    //
    // Register interesting Keyboard key presses
    //
    ctx->mouseKeyboard.registerKeypress(MouseKeyboard::getKeyFromConfigCode(ctx->keyboardConfig.JUMP_KEY), 
                                        ctx->keyboardConfig.JUMP_TIME);
    ctx->mouseKeyboard.registerKeypress(MouseKeyboard::getKeyFromConfigCode(ctx->keyboardConfig.RIGHT_KEY), 
                                        ctx->keyboardConfig.RIGHT_TIME);
    ctx->mouseKeyboard.registerKeypress(MouseKeyboard::getKeyFromConfigCode(ctx->keyboardConfig.LEFT_KEY), 
                                        ctx->keyboardConfig.LEFT_TIME);

    //
    // Load Pyxel Edit texture atlas, and texture.
    //
    //bali::tilemap::TileMapLoader::load("Bot_v0.xml", ctx->playerAniTileMap);
    //ctx->textureAtlas1Image.loadFromFile("Bot_v0.png");
    //ctx->textureAtlas1Image.createMaskFromColor(sf::Color::White, 0);
    //ctx->textureAtlas1Tex.loadFromImage(ctx->textureAtlas1Image);
    bali::tilemap::TileMapLoader::load("megaman_v0.xml", ctx->playerAniTileMap);
    ctx->textureAtlas1Image.loadFromFile("megaman_v0.png");
    ctx->textureAtlas1Image.createMaskFromColor(sf::Color::White, 0);
    ctx->textureAtlas1Tex.loadFromImage(ctx->textureAtlas1Image);

    //
    // Prepare the animations for each of the different player states.
    //
    //std::vector<struct ani::AnimationManager::Layout> names = 
    //          { { (uint32_t)Player::State::IDLE,        12, 7, 100, 0},
    //            { (uint32_t)Player::State::RIGHTWARDS,  60, 6,  75, 0},
    //            { (uint32_t)Player::State::LEFTWARDS,   60, 6,  75, 1},
    //            { (uint32_t)Player::State::JUMPING,     24, 4,  45, 0},
    //            { (uint32_t)Player::State::FALLING,      5, 1, 100, 0} };
    std::vector<struct ani::AnimationManager::Layout> names =
    {
      { (uint32_t)Player::State::IDLERIGHT,   0,  3, 200, 0 },
      { (uint32_t)Player::State::IDLELEFT,    0,  3, 200, 1 },
      { (uint32_t)Player::State::RIGHTWARDS,  4, 10,  75, 0 },
      { (uint32_t)Player::State::LEFTWARDS,   4, 10,  75, 1 },
      { (uint32_t)Player::State::JUMPINGRIGHT,    14,  3, 200, 0 },
      { (uint32_t)Player::State::JUMPINGLEFT,    14,  3, 200, 1 },
      { (uint32_t)Player::State::FALLINGRIGHT,    18,  1, 100, 0 },
      { (uint32_t)Player::State::FALLINGLEFT,    18,  1, 100, 1 }
    };
    ctx->player.aniMan.addAllFrames(ctx->playerAniTileMap, names);

    //
    // Last things
    //
    //ctx->mainZoomFactor = 1.0f;
    //ctx->fixedView = ctx->gameWindow.window.getView();
    ctx->mainZoomFactor = 1.0f;
    ctx->mainClock.restart();
    sf::FloatRect visibleArea(0, 0, 900, 900);
    ctx->gameWindow.window.setSize(sf::Vector2u(900, 900));
    ctx->gameWindow.window.setView(sf::View(visibleArea));

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
    // Update all animation sequences..
    //
    for (auto a = ctx->player.aniMan.animations.begin(); a != ctx->player.aniMan.animations.end(); a++)
    {
        a->second.update();
    }

    //
    // Construct all collision polygons that are visible to the player
    //
    qt::AABB searchRegion = getSearchRegion(ctx->gameWindow.window.getView(), 0.80f);
    std::vector<qt::XY> sr;
    sr = ctx->searchLayers.at(0)->search(searchRegion);
    ctx->polygonsVisible.clear();
    for (auto p = sr.begin(); p != sr.end(); p++)
    {
        float x, y;
        x = p->x;
        y = p->y;
        ctx->polygonsVisible.push_back(ctx->polygons[p->ti]);
    }

    //
    // Construct the Player Polygons
    //
    vec::VECTOR2 pos((ctx->player.pos.x), (ctx->player.pos.y));
    // TODO: SAT needs to take into account setRotation;
    ctx->playerpolygons[0].setOrigin(16, 32);
    ctx->playerpolygons[0].setPosition(pos);
    ctx->playerpolygons[0].setRotation(ctx->player.angle);
    ctx->playerpolygons[0].setScale(2.0f, 2.0f);

    CONVEXSHAPE rotatedPlayerShape;
    rotatedPlayerShape.setPointCount(8);
    for (int w = 0; w < 8;w++) {
        rotatedPlayerShape.setPoint(w,
            ctx->playerpolygons[0].getTransform().transformPoint(
                ctx->playerpolygons[0].getPoint(w)
            )
        );
    }
    
    ctx->playerpolygons[0].setTexture(&ctx->textureAtlas1Tex, true);//TODO: only needs to be done once.
    ani::Frame f = ctx->player.aniMan.animations[(uint32_t)ctx->player.state].getCurrentFrame(ctx->player.angle, ctx->player.vel);
    ctx->playerpolygons[0].setTextureRect(f.getIntRect(false, false));

    //
    // Collision Detection and Resolution
    //
    physics::ResolveCollisions(ctx->polygonsVisible, rotatedPlayerShape, ctx->player, ctx->physicsConfig, ctx->sharedEdges);

    //
    // Jump State Reset
    //
    if (ctx->player.isCollided)
    {
        //float d = vec::dot(physics::downVector(ctx->player.angle), ctx->player.surfaceNormal );
        //if (d < 0.01f)
        float d = vec::dot(physics::downVector(ctx->player.angle), vec::norm(ctx->player.vel));
        if (d > 0.6f)
        {
            if (ctx->player.doubleJumpCnt != ctx->physicsConfig.JUMP_COUNT)
            {
                std::cout << "CJ ";
            }
            ctx->player.doubleJumpCnt = ctx->physicsConfig.JUMP_COUNT;
        }
    }


    //
    // Based on flags and velocity
    // set player state, and reset
    // animation.
    //
    //std::cout << vec::stringify(player.vel) << "\n";
    float right = vec::dot(physics::rightVector(player.angle), vec::norm(player.vel));
    float left = vec::dot(physics::leftVector(player.angle), vec::norm(player.vel));
    float up = vec::dot(physics::upVector(player.angle), vec::norm(player.vel));
    float down = vec::dot(physics::downVector(player.angle), vec::norm(player.vel));
    float mag = vec::mag(player.vel);

    if (down  > 0.1f &&
        right < 0.8f &&
        left  < 0.8f &&
        mag   > 35.0f)
    {
        if (player.state != Player::State::FALLINGRIGHT && player.state != Player::State::FALLINGLEFT)
        {
            player.aniMan.animations[(uint32_t)player.state].stop();
            if (right > left)
                player.state = Player::State::FALLINGRIGHT;
            else
                player.state = Player::State::FALLINGLEFT;
            player.aniMan.animations[(uint32_t)player.state].start();
        }
    }
    //else if (vec::dot(physics::upVector(player.angle), vec::norm(player.vel)) > 0.2f &&
    //    vec::mag(player.vel) > 40.0f)
    else if (player.isJumping)
    {
        if (player.state != Player::State::JUMPINGRIGHT &&
            player.state != Player::State::JUMPINGLEFT)
        {
            player.aniMan.animations[(uint32_t)player.state].stop();
            if (right > left)
                player.state = Player::State::JUMPINGRIGHT;
            else
                player.state = Player::State::JUMPINGLEFT;
            player.aniMan.animations[(uint32_t)player.state].start();
        }
    }
    else if (mag < 20.0f)
    {
        if (player.state != Player::State::IDLERIGHT &&
            player.state != Player::State::IDLELEFT)
        {
            std::cout << "IDLE ";
            player.aniMan.animations[(uint32_t)player.state].stop();
            if (right > left)
            {
                player.state = Player::State::IDLERIGHT;
            }
            else
            {
                player.state = Player::State::IDLELEFT;
            }
            player.aniMan.animations[(uint32_t)player.state].start();
        }
    }
    else if (right > 0.5f)
    {
        if (player.state != Player::State::RIGHTWARDS)
        {
            player.aniMan.animations[(uint32_t)player.state].stop();
            player.state = Player::State::RIGHTWARDS;
            player.aniMan.animations[(uint32_t)player.state].start();
        }
    }
    else if (left > 0.5f)
    {
        if (player.state != Player::State::LEFTWARDS)
        {
            player.aniMan.animations[(uint32_t)player.state].stop();
            player.state = Player::State::LEFTWARDS;
            player.aniMan.animations[(uint32_t)player.state].start();
        }
    }


    //
    // Get User Input, and apply
    //
    //ctx->mouseKeyboard.doMouse(ctx->currentElapsed);
    if (!ctx->LostFocus)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            //this->done();
            ctx->screenMousePos = sf::Mouse::getPosition(ctx->gameWindow.window);
            ctx->worldMousePos = ctx->gameWindow.window.mapPixelToCoords(ctx->screenMousePos);
            //ctx->player.vel = ctx->worldMousePos - ctx->player.pos;
            vec::VECTOR2 dir = 1.0f * (ctx->worldMousePos - ctx->player.pos);
            ctx->player.addMove(1, dir, false);//addJump(1, 300, dir);//
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                ctx->player.isMovingRight = true;
            }
        }
    }
    ctx->mouseKeyboard.doKeyboard(ctx->currentElapsed, context,
        StageMainClient::KeyPressedHandler,
        StageMainClient::KeyHeldHandler,
        StageMainClient::KeyReleasedHandler);
    //
    // Motion Integration
    //
    physics::update(ctx->player, 
                    ctx->currentElapsed,
                    ctx->physicsConfig);
    return 0;
}

uint32_t StageMainClient::doDraw(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    vec::VECTOR2 camPos;

    ctx->pRenderTexture0->clear(sf::Color::Transparent);//(sf::Color(25, 75, 100, 0));
    ctx->pRenderTexture1->clear(sf::Color(100, 75, 25, 0));//

                                                           //
                                                           // Smooth out camera using average of past player positions
                                                           //
    for (int d = 0; d < ctx->player.posHist.size(); d++)
    {
        camPos += ctx->player.posHist[d];
    }
    camPos /= (float)ctx->player.posHist.size();
    sf::View mainView(camPos, vec::VECTOR2(ctx->gameWindow.screenWidth, ctx->gameWindow.screenHeight));
    //mainView.setRotation(ctx->player.angle);
    float angle;
    Player & player = ctx->player;
    mainView.setRotation(player.angle);
    //mainView.setSize(mainView.getSize() * ctx->mainZoomFactor);
    mainView.zoom(ctx->mainZoomFactor);


    {
        //// Draw background tiles
        //sf::RenderStates bkgndRenderStates;
        //bkgndRenderStates.texture = &ctx->backgroundTex;
        //bali::QuadArray back = ctx->quadLayers[0];
        //ctx->pRenderTexture0->draw(back, bkgndRenderStates);
    }

    //ctx->pRenderTexture0->setView(mainView);

    ctx->parallaxBackgrounds[0].sprite.setPosition(ctx->player.pos / 1.5f);
    ctx->parallaxBackgrounds[0].sprite.move(-100, -100);    
    ctx->parallaxBackgrounds[1].sprite.setPosition(ctx->player.pos / 2.5f);
    ctx->parallaxBackgrounds[1].sprite.move(-50, -50);    
    ctx->parallaxBackgrounds[2].sprite.setPosition(ctx->player.pos / 3.0f);
    ctx->parallaxBackgrounds[2].sprite.move(-25, -25);
    ctx->parallaxBackgrounds[3].sprite.setPosition(ctx->player.pos / 3.5f);
    ctx->parallaxBackgrounds[3].sprite.move(-15, -15);
    ctx->parallaxBackgrounds[4].sprite.setPosition(ctx->player.pos / 4.0f);
    ctx->parallaxBackgrounds[4].sprite.move(-10, -10);    
    ctx->pRenderTexture0->draw(ctx->parallaxBackgrounds[0].sprite);
    ctx->pRenderTexture0->draw(ctx->parallaxBackgrounds[1].sprite);
    ctx->pRenderTexture0->draw(ctx->parallaxBackgrounds[2].sprite);
    ctx->pRenderTexture0->draw(ctx->parallaxBackgrounds[3].sprite);
    ctx->pRenderTexture0->draw(ctx->parallaxBackgrounds[4].sprite);

    // Draw foregound tiles
    sf::RenderStates foreRenderstates;
    foreRenderstates.texture = &ctx->tilesetATex;
    auto fore = ctx->quadLayers[1];
    ctx->pRenderTexture0->draw(fore, foreRenderstates);

    // Draw collision polygons - debug
    //for (auto poly = ctx->polygonsVisible.begin(); poly != ctx->polygonsVisible.end(); ++poly)
    //{
    //    sf::RenderStates polyVisiRenderStates;
    //    poly->setOutlineColor(sf::Color::Red);
    //    poly->setFillColor(sf::Color::Transparent);
    //    poly->setOutlineThickness(1);
    //    ctx->pRenderTexture0->draw(*poly, polyVisiRenderStates);
    //}

    // Draw player polygons
    for (auto poly = ctx->playerpolygons.begin(); poly != ctx->playerpolygons.end(); ++poly)
    {
        sf::RenderStates polyPlayerRenderStates;
        polyPlayerRenderStates.texture = &ctx->textureAtlas1Tex;
        //poly->setOutlineColor(sf::Color::Yellow);
        //poly->setFillColor(sf::Color::Transparent);
        poly->setOutlineThickness(1);
        ctx->pRenderTexture0->draw(*poly, polyPlayerRenderStates);
    }

    // Draw a small box at the player position - debug
    // this represents the players' center of mass
    {
    //    sf::RenderStates polyPlayerRenderStates;
    //    polyPlayerRenderStates.texture = &ctx->textureAtlas1Tex;
    //    CONVEXSHAPE cs;
    //    float px = ctx->player.pos.x;
    //    float py = ctx->player.pos.y;

    //    cs.setPointCount(4);
    //    cs.setPoint(0, vec::VECTOR2(px - 1, py - 1));
    //    cs.setPoint(1, vec::VECTOR2(px + 1, py - 1));
    //    cs.setPoint(2, vec::VECTOR2(px + 1, py + 1));
    //    cs.setPoint(3, vec::VECTOR2(px - 1, py + 1));

    //    cs.setOutlineColor(sf::Color::Green);
    //    //poly->setFillColor(sf::Color::Transparent);
    //    cs.setOutlineThickness(1);
    //    ctx->pRenderTexture0->draw(cs, polyPlayerRenderStates);
    }
    
{
        //sf::RenderStates rs;
        std::stringstream ss;
        ss << "FPS: " << (uint32_t)(1.0f / ctx->currentElapsed.asSeconds()) << std::endl;
        ss << "POS: " << (uint32_t)ctx->player.pos.x << ", " << (uint32_t)ctx->player.pos.y << std::endl;
        ss << "VEL: " << (int32_t)ctx->player.vel.x << ", " << (int32_t)ctx->player.vel.y << std::endl;
        ctx->fpsText.setString(ss.str());
        ctx->fpsText.setCharacterSize(30* ctx->mainZoomFactor);
        ctx->fpsText.setColor(sf::Color::Red);
        ctx->fpsText.setRotation(ctx->player.angle);
        ctx->fpsText.setPosition(camPos + physics::upVector(ctx->player.angle)*ctx->mainZoomFactor*(ctx->gameWindow.window.getSize().y / 2.0f) + physics::leftVector(ctx->player.angle)*ctx->mainZoomFactor*(ctx->gameWindow.window.getSize().x/2.0f));//(hudView.getCenter() + vec::VECTOR2(-0.5f * hudView.getSize().x, -0.5f * hudView.getSize().y));
        
       // ctx->pRenderTexture0->setView(hudView);
        ctx->pRenderTexture0->draw(ctx->fpsText);
        ctx->pRenderTexture0->display();
    }
    //sf::RenderStates lineSegRenderState;
    //ctx->pRenderTexture1->draw(ctx->glowPolygons.back(), lineSegRenderState);
    //ctx->pRenderTexture1->display();

    //sf::RenderStates totalRenderStates;
    //totalRenderStates.shader = &ctx->shader;
    //ctx->shader.setUniform("view", ctx->mainView.getSize());
    //ctx->shader.setUniform("texture", sf::Shader::CurrentTexture);
    //ctx->shader.setUniform("position", sf::Glsl::Vec2(ctx->gameWindow.window.mapCoordsToPixel(ctx->player.pos)));
    //ctx->shader.setUniform("resolution", sf::Glsl::Vec2(2048, 2048));
    //ctx->shader.setUniform("losMask", ctx->pRenderTexture1->getTexture());


    ctx->gameWindow.window.clear();



    sf::RenderStates totalRenderStates;
    
    sf::Sprite sprite(ctx->pRenderTexture0->getTexture());
    //sf::Sprite sprite2(ctx->pRenderTexture1->getTexture());
    ctx->gameWindow.window.setView(mainView);
    //totalRenderStates.blendMode = sf::BlendAlpha;
    //ctx->gameWindow.window.setView(ctx->gameWindow.window.getView());
    ctx->gameWindow.window.draw(sprite, totalRenderStates);

    // Finalize it
    ctx->gameWindow.window.display();
    return 0;
}

uint32_t StageMainClient::cleanup(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    return 0;
}

void StageMainClient::KeyPressedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    if (kp.key == sf::Keyboard::Space)
    {
        if (ctx->player.doubleJumpCnt == ctx->physicsConfig.JUMP_COUNT)
        {
            if (vec::dot(physics::upVector(ctx->player.angle), ctx->player.jumpNormal) > -0.001f)
            {
                std::cout << "FJ ";
                kp.nml = ctx->player.jumpNormal;
                ctx->player.doubleJumpCnt--;
                ctx->player.isJumping = true;
            }
        }
        else if (ctx->player.doubleJumpCnt > 0)
        {
            std::cout << "SJ ";
            kp.nml = physics::upVector(ctx->player.angle);
            kp.elp = sf::Time::Zero;// So subsequent jumps have full duration too
            ctx->player.doubleJumpCnt--;
            ctx->player.isJumping = true;
        }
        else
        {
            std::cout << "LJ ";
            ctx->player.jumpNormal = vec::Zero();
        }
    }
    else if (kp.key == sf::Keyboard::D)
    {
        ctx->player.isMovingRight = true;
    }
    else if (kp.key == sf::Keyboard::A)
    {
        ctx->player.isMovingLeft = true;
    }
}


void StageMainClient::KeyHeldHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    if (kp.key == sf::Keyboard::Space)
    {
        if (ctx->player.isJumping == true)
        {
            float str = 1;// cos((kp.elp.asMilliseconds() * ((0.5f * PI) / kp.dur))) + 0.0f;
            ctx->player.addJump(str, kp.dur, kp.nml);
            //ctx->player.isJumping = true;
        }
    }
    else if (kp.key == sf::Keyboard::D)
    {
        float str = 1.f;
        bool grounded = false;
        if (ctx->player.latNormal == vec::Zero())
        {
            kp.nml = physics::rightVector(ctx->player.angle);
        }
        else
        {
            kp.nml = vec::normal(ctx->player.latNormal) * -1.0f;
            grounded = true;
        }
        ctx->player.addMove(str,
                            kp.nml,
                            grounded);

        ctx->player.isMovingRight = true;
        ctx->player.latNormal = vec::Zero();
    }
    else if (kp.key == sf::Keyboard::A)
    {
        float str = 1.f;
        bool grounded = false;
        if (ctx->player.latNormal == vec::Zero())
        {
            kp.nml = physics::leftVector(ctx->player.angle);
        }
        else
        {
            kp.nml = vec::normal(ctx->player.latNormal) * 1.0f;
            grounded = true;
        }
        ctx->player.addMove(str,
                            kp.nml,
                            grounded);

        ctx->player.isMovingLeft = true;
        ctx->player.latNormal = vec::Zero();
    }
}

void StageMainClient::KeyReleasedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;

    if (kp.key == sf::Keyboard::Space)
    {
        ctx->player.isJumping = false;
        kp.nml = vec::Zero();
    }
    else if (kp.key == sf::Keyboard::D)
    {
        ctx->player.isMovingRight = false;
        kp.nml = vec::Zero();
    }
    else if (kp.key == sf::Keyboard::A)
    {
        ctx->player.isMovingLeft = false;
        kp.nml = vec::Zero();
    }
}


#define MY_PI 3.14159265359f
float logisticsFunction(float t, float L, float K, float t0)
{
    /*
    f(t) = L / (1 + e^(-k*(t-t0)))
    t  => current elapsed time
    L  => Max Value
    t0 => x-value midpoint
    K  => steepness
    */
    float exp_denom = -K * (t - t0);
    float denom = 1 + pow(2.71828, (exp_denom));
    return L / denom;
}


void StageMainClient::loadLevelData()
{
}

void StageMainClient::buildLevel()
{

}

}//end namespace bali
