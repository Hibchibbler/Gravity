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

    // Load Configuration files for tweakable settings.
    ctx->physicsConfig = loadPhysicsConfig("physics.config.txt");
    ctx->keyboardConfig = loadKeyboardConfig("keyboard.config.txt");

    // Load TMX
    ctx->map = make_shared<bali::TMX::Map>();
    TMX::TMXReader::load("level_test1.tmx", ctx->map);

    // Create Player shape from TMX
    TMX::Objectgroup::Ptr pObjG = ctx->map->getObjectGroup("PlayerShapes");
        
    // Set defaults player position from TMX player shape.
    ctx->player.pos.x = pObjG->objects.back()->x;
    ctx->player.pos.y = pObjG->objects.back()->y;

    // Center the camera on the player.
    ctx->size.x = ctx->size.y = 1000;
    //ctx->mainView.setCenter(ctx->player.pos.x, ctx->player.pos.y);
    //ctx->mainView.setSize(sf::Vector2f(ctx->size.x, ctx->size.y));

    // Create Foreground tileset from TMX
    TMX::Tileset::Ptr tilesetA = ctx->map->getTileset("tilesetFore");
    ctx->tilesetAImg.loadFromFile(tilesetA->images.back()->source);
    ctx->tilesetAImg.createMaskFromColor(sf::Color::Black);//transparent is black...
    ctx->tilesetATex.loadFromImage(ctx->tilesetAImg);

    // Create Background tileset from TMX
    TMX::Tileset::Ptr tilesetBkgnd = ctx->map->getTileset("tilesetBack");
    ctx->backgroundImg.loadFromFile(tilesetBkgnd->images.back()->source);
    ctx->backgroundTex.loadFromImage(ctx->backgroundImg);

    // Create internal representation of background geometry from TMX
    ctx->tileLayers.push_back(Tile::Vec());
    buildTileLayer(ctx->tileLayers.back(), tilesetBkgnd, ctx->map->getLayer("BackgroundLayer"));

    // Create renderable background geometry from internal representation
    ctx->quadLayers.push_back(bali::QuadArray());
    buildQuadLayer(ctx->quadLayers.back(), ctx->tileLayers[0], 32, 32);

    // Create internal representation of foreground geometry from TMX
    ctx->tileLayers.push_back(Tile::Vec());
    buildTileLayer(ctx->tileLayers.back(), tilesetA, ctx->map->getLayer("ForegroundLayer"));

    // Create renderable foreground geometry from internal representation
    ctx->quadLayers.push_back(bali::QuadArray());
    buildQuadLayer(ctx->quadLayers.back(), ctx->tileLayers[1], 32, 32);
    
    // Create Collision Polygon geometry from TMX
    buildPolygonLayers(ctx->polygons,
        ctx->map->objectgroups);

    // Create player collision polygon geometry from TMX
    buildPlayerObjectLayers(ctx->playerpolygons,
        ctx->map->objectgroups);


    // Create Quadtree for Collision Polygons.
    int maxDepth = 15;
    qt::AABB aabb;
    aabb.min.x = -64;
    aabb.min.y = -64;
    aabb.max.x = aabb.max.y = (ctx->map->width+2) * 32;//in pixels
    qt::QuadTree::ShPtr searchLayer = std::make_shared<qt::QuadTree>();
    searchLayer->initialize(aabb, maxDepth);
    buildSearchLayer(searchLayer, ctx->polygons);
    ctx->searchLayers.push_back(searchLayer);

    // Initialize render texture, and shaders
    ctx->pRenderTexture0 = &ctx->renderTextures[0];
    ctx->pRenderTexture0->create(2048,2048);
    //
    ctx->pRenderTexture1 = &ctx->renderTextures[1];
    ctx->pRenderTexture1->create(2048, 2048);
    ctx->shader.loadFromFile("gravity_vert.vert", "gravity_frag.frag");

    // Register interesting Keyboard key presses
    ctx->mouseKeyboard.registerKeypress(MouseKeyboard::getKeyFromConfigCode(ctx->keyboardConfig.JUMP_KEY), 
                                        ctx->keyboardConfig.JUMP_TIME,
                                        ctx->physicsConfig.JUMP_STRENGTH, false);
    ctx->mouseKeyboard.registerKeypress(MouseKeyboard::getKeyFromConfigCode(ctx->keyboardConfig.RIGHT_KEY), 
                                        ctx->keyboardConfig.RIGHT_TIME,
                                        ctx->physicsConfig.MOVE_STRENGTH, false);
    ctx->mouseKeyboard.registerKeypress(MouseKeyboard::getKeyFromConfigCode(ctx->keyboardConfig.LEFT_KEY), 
                                        ctx->keyboardConfig.LEFT_TIME,
                                        ctx->physicsConfig.MOVE_STRENGTH, false);

    // Load Texture Packer texture atlas
    bali::tilemap::TileMapLoader::load("MegaMan_v0.xml", ctx->playerAniTileMap);
    ctx->textureAtlas1Image.loadFromFile("MegaMan_v0.png");
    ctx->textureAtlas1Image.createMaskFromColor(sf::Color::White, 0);
    ctx->textureAtlas1Tex.loadFromImage(ctx->textureAtlas1Image);

    

    //
    // Prepare the animations for each of the different player states.
    //
    {
        struct gwetch {
            Player::State state;
            uint32_t start;
            uint32_t len;
            uint32_t delay;
            uint32_t flipY;
        };
        std::vector<struct gwetch> names = { { Player::State::IDLE,        0,  3, 300, 0},
                                             { Player::State::RIGHTWARDS,  4, 10,  75, 0},
                                             { Player::State::LEFTWARDS,   4, 10,  75, 1},
                                             { Player::State::JUMPING,    14,  6, 500, 0},
                                             { Player::State::FALLING,    18,  1,  75, 0} };

        for (auto n = names.begin(); n != names.end(); n++)
        {
            ctx->player.aniMan.animations[(uint32_t)n->state] = ani::Animation(n->len, n->delay);
            for (auto d = 0; d < n->len; d++)
            {
                ani::Frame frame;
                if (n->flipY)
                {
                    frame = ani::Frame(
                        ctx->playerAniTileMap.layers.back().tiles[d + n->start].x * ctx->playerAniTileMap.tilewidth + ctx->playerAniTileMap.tilewidth,
                        ctx->playerAniTileMap.layers.back().tiles[d + n->start].y * ctx->playerAniTileMap.tileheight,
                        ctx->playerAniTileMap.tilewidth * -1.0f,
                        ctx->playerAniTileMap.tileheight);
                }
                else
                {
                    frame = ani::Frame(
                        ctx->playerAniTileMap.layers.back().tiles[d + n->start].x * ctx->playerAniTileMap.tilewidth,
                        ctx->playerAniTileMap.layers.back().tiles[d + n->start].y * ctx->playerAniTileMap.tileheight,
                        ctx->playerAniTileMap.tilewidth,
                        ctx->playerAniTileMap.tileheight);
                }
                
                ctx->player.aniMan.animations[(uint32_t)n->state].frames.push_back(frame);
            }
        }
    }

    // Last thing
    initialized();
    return 0;
}

uint32_t StageMainClient::doRemoteEvent(Context::Ptr ctx)
{
    return 0;
}

uint32_t StageMainClient::doWindowEvent(Context::Ptr context, sf::Event & event)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    static float zoom = 1.0;
    switch (event.type) {
    case sf::Event::LostFocus:
        //ctx->mouseKeyboard.kbClock.restart();
        break;
    case sf::Event::GainedFocus:
        break;
    case sf::Event::MouseWheelScrolled: {
        ctx->mainView = ctx->gameWindow.window.getView();
        zoom = (event.mouseWheelScroll.delta > 0 ? 1.2 : 0.8);
        sf::Vector2f v = ctx->mainView.getSize();
        v.x *= 1.01 * zoom;
        v.y *= 1.01 * zoom;
        ctx->mainView.setSize(v);
        std::cout << "Wheel Scrolled: " << event.mouseWheelScroll.delta << std::endl;
        break;
    }case sf::Event::Resized: {
        ctx->gameWindow.screenWidth = event.size.width;
        ctx->gameWindow.screenHeight = event.size.height;
        ctx->mainView.setSize(vec::VECTOR2(event.size.width, event.size.height));
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

uint32_t StageMainClient::doLocalInputs(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    ctx->currentElapsed = ctx->mainClock.restart();
    ctx->mouseKeyboard.doMouse(ctx->currentElapsed);

    ctx->mouseKeyboard.doKeyboard(ctx->currentElapsed,  context,
                                                        StageMainClient::KeyPressedHandler,
                                                        StageMainClient::KeyHeldHandler,
                                                        StageMainClient::KeyReleasedHandler);
    return 0;
}

uint32_t StageMainClient::doUpdate(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    vec::VECTOR2 camPos;

    for (auto a = ctx->player.aniMan.animations.begin(); a != ctx->player.aniMan.animations.end(); a++)
    {
        a->second.update();
    }

    //
    // Smooth out camera using average of past player positions
    //
    for (int d = 0; d < ctx->player.posHist.size(); d++)
    {
        camPos += ctx->player.posHist[d];
    }
    camPos /= (float)ctx->player.posHist.size();

    //
    // Construct the visible collision polygons
    //
    qt::AABB searchRegion = getSearchRegion(ctx->mainView, 1.20f);
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

    float hafWid =  ctx->playerpolygons[0].getLocalBounds().width / 3.5;
    float hafHite = ctx->playerpolygons[0].getLocalBounds().height / 2.0;

    // TODO: SAT needs to take into account setRotation;
    ctx->playerpolygons[0].setPosition(pos.x, pos.y);//(pos.x - hafWid, pos.y + hafHite);//
    ctx->playerpolygons[0].setOrigin(hafWid,-hafHite);
    ctx->playerpolygons[0].setRotation(ctx->player.angle);
    ctx->playerpolygons[0].setScale(1.00, 1.00);
    
    ctx->playerpolygons[0].setTexture(&ctx->textureAtlas1Tex, false);//TODO: only needs to be done once.

    ani::Frame f = ctx->player.aniMan.animations[(uint32_t)ctx->player.state].getCurrentFrame(ctx->player.angle, ctx->player.vel);
    ctx->playerpolygons[0].setTextureRect(f.getIntRect(false, false));

    //
    // Resolve collisions
    // Integrate motion
    // Create Line of Sight Triangle Fan
    //
    //physics::createLoSTriFan(ctx->polygonsVisible, ctx->player.pos, ctx->lineSegments);
    
    physics::ResolveCollisions(ctx->polygonsVisible, ctx->playerpolygons.back(), ctx->player, ctx->physicsConfig, ctx->sharedEdges);
    
    physics::update(ctx->player, 
                    ctx->currentElapsed,
                    ctx->physicsConfig, 
                    ctx->mouseKeyboard.keyStates[sf::Keyboard::D].str,
                    ctx->mouseKeyboard.keyStates[sf::Keyboard::A].str,
                    ctx->mouseKeyboard.keyStates[sf::Keyboard::Space].str);

    ctx->mainView.setCenter(camPos);
    ctx->mainView.setRotation(ctx->player.angle);
    return 0;
}

uint32_t StageMainClient::doDraw(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    // Draw background
    ctx->pRenderTexture0->clear();
    ctx->pRenderTexture1->clear(sf::Color::Black);
    
    ctx->gameWindow.window.setView(ctx->mainView);
    ctx->gameWindow.window.clear();

    sf::RenderStates bkgndRenderStates;
    bkgndRenderStates.texture = &ctx->backgroundTex;
    auto back = ctx->quadLayers[0];
    ctx->pRenderTexture0->draw(back, bkgndRenderStates);

    // Draw visible foregound
    sf::RenderStates foreRenderstates;
    foreRenderstates.texture = &ctx->tilesetATex;
    auto fore = ctx->quadLayers[1];
    ctx->pRenderTexture0->draw(fore, foreRenderstates);

    //// Collision Polygons - debug
    //for (auto poly = ctx->polygonsVisible.begin(); poly != ctx->polygonsVisible.end(); ++poly)
    //{
    //    sf::RenderStates polyVisiRenderStates;
    //    poly->setOutlineColor(sf::Color::Red);
    //    poly->setFillColor(sf::Color::Transparent);
    //    poly->setOutlineThickness(1);
    //    ctx->pRenderTexture0->draw(*poly, polyVisiRenderStates);
    //}

    for (auto poly = ctx->playerpolygons.begin(); poly != ctx->playerpolygons.end(); ++poly)
    {
        sf::RenderStates polyPlayerRenderStates;
        polyPlayerRenderStates.texture = &ctx->textureAtlas1Tex;
        //poly->setOutlineColor(sf::Color::Yellow);
        //poly->setFillColor(sf::Color::Transparent);
        poly->setOutlineThickness(1);
        ctx->pRenderTexture0->draw(*poly, polyPlayerRenderStates);
    }

    {
        sf::RenderStates polyPlayerRenderStates;
        polyPlayerRenderStates.texture = &ctx->textureAtlas1Tex;
        CONVEXSHAPE cs;
        float px = ctx->player.pos.x;
        float py = ctx->player.pos.y;

        cs.setPointCount(4);
        cs.setPoint(0, vec::VECTOR2(px+0,py+0));
        cs.setPoint(1, vec::VECTOR2(px+3,py+0));
        cs.setPoint(2, vec::VECTOR2(px+3, py+3));
        cs.setPoint(3, vec::VECTOR2(px+0, py+3));
        
        cs.setOutlineColor(sf::Color::Green);
        //poly->setFillColor(sf::Color::Transparent);
        cs.setOutlineThickness(1);
        ctx->pRenderTexture0->draw(cs, polyPlayerRenderStates);
    }

    ctx->pRenderTexture0->display();

    sf::RenderStates lineSegRenderState;
    ctx->pRenderTexture1->draw(ctx->lineSegments, lineSegRenderState);
    ctx->pRenderTexture1->display();

    sf::RenderStates totalRenderStates;
    totalRenderStates.shader = &ctx->shader;
    ctx->shader.setUniform("view", ctx->mainView.getSize());
    ctx->shader.setUniform("texture", sf::Shader::CurrentTexture);
    vec::VECTOR2 pposTemp = ctx->player.pos;
    pposTemp.x = pposTemp.x - 8;
    pposTemp.y = pposTemp.y + 8;
    ctx->shader.setUniform("position", sf::Glsl::Vec2(ctx->gameWindow.window.mapCoordsToPixel(pposTemp)));
    ctx->shader.setUniform("resolution", sf::Glsl::Vec2(2048, 2048));
    ctx->shader.setUniform("losMask", ctx->pRenderTexture1->getTexture());


    sf::Sprite sprite(ctx->pRenderTexture0->getTexture());
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

    //std::cout << (ctx->player.posCorrection != vec::VECTOR2(0, 0) ? "1 " : "0 ");

    if (kp.key == sf::Keyboard::Space)
    {
        if (vec::dot(ctx->player.vel, ctx->player.jumpNormal) < -0.9f)
        {
            kp.nml = ctx->player.jumpNormal;
        }
        ctx->player.doJumping();
    }
    else if (kp.key == sf::Keyboard::D)
    {
        ctx->player.doRightward();
    }
    else if (kp.key == sf::Keyboard::A)
    {
        ctx->player.doLeftward();
    }

    ctx->player.aniMan.animations[(uint32_t)ctx->player.state].start();
}


void StageMainClient::KeyHeldHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;

    if (kp.key == sf::Keyboard::Space)
    {
        kp.str = cos((kp.elp.asMilliseconds() * ((0.5f * PI) / kp.dur))) + 0.0f;
        ctx->player.jumpNormal = vec::Zero();
        ctx->player.addJump(kp.str, kp.dur, kp.nml);
        ctx->player.doJumping();
    }
    else if (kp.key == sf::Keyboard::D)
    {
        kp.str = 1;// kp.amp;
        kp.nml = vec::normal(ctx->player.latNormal) * -1.0f;
        ctx->player.latNormal = vec::Zero();
        ctx->player.addMove(kp.str, kp.nml, true, (ctx->player.latNormal == vec::Zero() ? false : true));
        ctx->player.doRightward();
    }
    else if (kp.key == sf::Keyboard::A)
    {
        kp.str = 1;// kp.amp;
        kp.nml = vec::normal(ctx->player.latNormal) * 1.0f;
        ctx->player.latNormal = vec::Zero();
        ctx->player.addMove(kp.str, kp.nml, false, (ctx->player.latNormal == vec::Zero() ? false : true));
        ctx->player.doLeftward();
    }
}

void StageMainClient::KeyReleasedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;

    if (kp.key == sf::Keyboard::Space)
    {
        ctx->player.aniMan.animations[(uint32_t)Player::State::JUMPING].stop();
        ctx->player.doIdle(Player::State::JUMPING);
        kp.nml = vec::Zero();
    }
    else if (kp.key == sf::Keyboard::D)
    {
        ctx->player.aniMan.animations[(uint32_t)Player::State::RIGHTWARDS].stop();
        ctx->player.doIdle(Player::State::RIGHTWARDS);
        kp.nml = vec::Zero();
    }
    else if (kp.key == sf::Keyboard::A)
    {
        ctx->player.aniMan.animations[(uint32_t)Player::State::LEFTWARDS].stop();
        ctx->player.doIdle(Player::State::LEFTWARDS);
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

}//end namespace bali
