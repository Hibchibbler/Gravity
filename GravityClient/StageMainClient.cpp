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

#include "GameClientContext.h"
#include "MouseKeyboard.h"

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
        
        ///////////////////////////////////////////////////////////////////////
        ctx->physicsConfig = loadPhysicsConfig("physics.config.txt");


        ///////////////////////////////////////////////////////////////////////
        // Load TMX
        ctx->map = make_shared<bali::TMX::Map>();
        TMX::TMXReader::load("level_test1.tmx", ctx->map);

        // Still playing with these...no clue yet
        TMX::Objectgroup::Ptr pObjG = ctx->map->getObjectGroup("PlayerShapes");
        

        ctx->player.pos.x = pObjG->objects.back()->x;
        ctx->player.pos.y = pObjG->objects.back()->y;

        ctx->player.vel = vec::VECTOR2(0.0, 0.0);
        ctx->player.acc = vec::VECTOR2(0.0, 0.0);

        ctx->size.x = ctx->size.y = 1000;
        ctx->mainView.setCenter(ctx->player.pos.x, ctx->player.pos.y);
        ctx->mainView.setSize(sf::Vector2f(ctx->size.x, ctx->size.y));
        //ctx->mainView.setViewport(sf::FloatRect(0.25,0.25,0.5,0.5));
        ///////////////////////////////////////////////////////////////////////

        // Let's use this tileset.wadasdsa
        TMX::Tileset::Ptr tilesetA = ctx->map->getTileset("tilesetFore");
        ctx->tilesetAImg.loadFromFile(tilesetA->images.back()->source);
        ctx->tilesetAImg.createMaskFromColor(sf::Color::Black);//transparent is black...
        ctx->tilesetATex.loadFromImage(ctx->tilesetAImg);


        TMX::Tileset::Ptr tilesetBkgnd = ctx->map->getTileset("tilesetBack");
        ctx->backgroundImg.loadFromFile(tilesetBkgnd->images.back()->source);
        ctx->backgroundTex.loadFromImage(ctx->backgroundImg);
        ///////////////////////////////////////////////////////////////////////
        // Store TMX map layers into our TileLayers data structure
        ctx->tileLayers.push_back(Tile::Vec());
        buildTileLayer(ctx->tileLayers.back(), tilesetBkgnd, ctx->map->getLayer("BackgroundLayer"));

        ctx->tileLayers.push_back(Tile::Vec());
        buildTileLayer(ctx->tileLayers.back(), tilesetA, ctx->map->getLayer("ForegroundLayer"));
        ///////////////////////////////////////////////////////////////////////
        buildPolygonLayers(ctx->polygons,
            ctx->map->objectgroups);
        
        //buildSharedEdgesLayers(ctx->sharedEdges,
        //    ctx->map->objectgroups);
        
        buildPlayerObjectLayers(ctx->playerpolygons,
            ctx->map->objectgroups);

        ///////////////////////////////////////////////////////////////////////
        int maxDepth = 15;
        qt::AABB aabb;
        aabb.min.x = -64;
        aabb.min.y = -64;
        aabb.max.x = aabb.max.y = (ctx->map->width+2) * 32;//in pixels

        qt::QuadTree::ShPtr searchLayer = std::make_shared<qt::QuadTree>();
        searchLayer->initialize(aabb, maxDepth);

        //buildSearchLayer(searchLayer, );
        buildSearchLayer(searchLayer, ctx->polygons);
        ctx->searchLayers.push_back(searchLayer);
        //buildSearchLayer(ctx->searchLayers.back(), ctx->tileLayers[0]);

        ///////////////////////////////////////////////////////////////////////
        ctx->quadLayers.push_back(bali::QuadArray());
        buildQuadLayer(ctx->quadLayers.back(), ctx->tileLayers[0], 32, 32);
        ctx->quadLayers.push_back(bali::QuadArray());
        buildQuadLayer(ctx->quadLayers.back(), ctx->tileLayers[1], 32, 32);
        ///////////////////////////////////////////////////////////////////////

        // Shader setup
        ctx->pRenderTexture0 = &ctx->renderTextures[0];
        ctx->pRenderTexture0->create(2048,2048);
        //
        ctx->pRenderTexture1 = &ctx->renderTextures[1];
        ctx->pRenderTexture1->create(2048, 2048);


        ctx->shader.loadFromFile("gravity_vert.vert", "gravity_frag.frag");

        ///////////////////////////////////////////////////////////////////////
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
            break;
        case sf::Event::GainedFocus:
            break;
        case sf::Event::MouseWheelScrolled: {
            ctx->mainView = ctx->window.getView();
            zoom = (event.mouseWheelScroll.delta > 0 ? 1.2 : 0.8);
            sf::Vector2f v = ctx->mainView.getSize();
            v.x *= 1.01 * zoom;
            v.y *= 1.01 * zoom;
            ctx->mainView.setSize(v);
            //getContext()->mainView.zoom(zoom);
            std::cout << "Wheel Scrolled: " << event.mouseWheelScroll.delta << std::endl;
            break;
        }case sf::Event::Resized: {
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
        MouseKeyboard::doMouse(ctx);
        MouseKeyboard::doKeyboard(ctx);

        return 0;
    }

   
    uint32_t StageMainClient::doUpdate(Context::Ptr context)
    {
        GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
        sf::Time elapsed = ctx->mainClock.restart();
        vec::VECTOR2 zxc;
        //Smooth out player position for camera
        for (int d = 0; d < ctx->player.posHist.size(); d++)
        {
            zxc += ctx->player.posHist[d];
        }
        zxc /= (float)ctx->player.posHist.size();

        // Search for foreground that is visible        
        qt::AABB searchRegion = getSearchRegion(ctx->mainView, 0.90f);

        //Does the player hit anything?
        // convert All obstacle tiles returned from quad tree into shapes for rendering.
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

        // Every shape that is tested for collision by the player
        // shape is to be placed into this vector
        // individual player shapes are to be placed into this vector.
        //  Note: A player "shape" may be composed of multiple polygons.
        vec::VECTOR2 pos((ctx->player.pos.x), (ctx->player.pos.y));
        for (int j = 0; j < ctx->playerpolygons.size(); ++j)
        {
            float hafWid = 0;// ctx->playerpolygons[j].getLocalBounds().width / 2.0;//getLocalBounds
            float hafHite = 0;// ctx->playerpolygons[j].getLocalBounds().height / 2.0;

            ctx->playerpolygons[j].setPosition(pos.x - hafWid, pos.y + hafHite);
        }


        // Resolve collisions
        // Integrate motion
        // Create Line of Sight Triangle Fan
        //physics::createLoSTriFan(ctx->polygonsVisible, ctx->player.pos, ctx->lineSegments);
        physics::ResolveCollisions(ctx->polygonsVisible, ctx->playerpolygons.back(), ctx->player, ctx->physicsConfig, ctx->sharedEdges);
        physics::update(ctx->player, elapsed, ctx->physicsConfig);


        //ctx->mainView.setCenter(ctx->player.pos.x, ctx->player.pos.y);
        ctx->mainView.setCenter(zxc.x, zxc.y);
        ctx->mainView.setRotation(ctx->player.angle);
        return 0;
    }

    uint32_t StageMainClient::doDraw(Context::Ptr context)
    {
        GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
        // Draw background
        ctx->pRenderTexture0->clear();
        ctx->pRenderTexture1->clear(sf::Color::Black);
        ctx->window.setView(ctx->mainView);
        ctx->window.clear();

        sf::RenderStates bkgndRenderStates;
        bkgndRenderStates.texture = &ctx->backgroundTex;
        auto back = ctx->quadLayers[0];
        ctx->pRenderTexture0->draw(back, bkgndRenderStates);

        // Draw visible foregound
        sf::RenderStates foreRenderstates;
        foreRenderstates.texture = &ctx->tilesetATex;
        auto fore = ctx->quadLayers[1];
        ctx->pRenderTexture0->draw(fore, foreRenderstates);

        // Collision Polygons - debug
        for (auto poly = ctx->polygonsVisible.begin(); poly != ctx->polygonsVisible.end(); ++poly)
        {
            sf::RenderStates polyVisiRenderStates;
            poly->setOutlineColor(sf::Color::Red);
            poly->setFillColor(sf::Color::Transparent);
            poly->setOutlineThickness(1);
            ctx->pRenderTexture0->draw(*poly, polyVisiRenderStates);
        }

        for (auto poly = ctx->playerpolygons.begin(); poly != ctx->playerpolygons.end(); ++poly)
        {
            sf::RenderStates polyPlayerRenderStates;
            poly->setOutlineColor(sf::Color::Yellow);
            poly->setFillColor(sf::Color::Green);
            poly->setOutlineThickness(2);
            ctx->pRenderTexture0->draw(*poly, polyPlayerRenderStates);
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
        ctx->shader.setUniform("position", sf::Glsl::Vec2(ctx->window.mapCoordsToPixel(pposTemp)));
        ctx->shader.setUniform("resolution", sf::Glsl::Vec2(2048, 2048));
        ctx->shader.setUniform("losMask", ctx->pRenderTexture1->getTexture());


        sf::Sprite sprite(ctx->pRenderTexture0->getTexture());
        ctx->window.draw(sprite, totalRenderStates);

        // Finalize it
        ctx->window.display();
        return 0;
    }

    uint32_t StageMainClient::cleanup(Context::Ptr context)
    {
        GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
        return 0;
    }


}