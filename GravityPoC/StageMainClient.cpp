///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageMainClient.h"
#include "GameContext.h"
#include "Utility.h"
#include "MouseKeyboard.h"
#include <math.h>
#include "Vector\Vector2.h"
#include "SATAlgo\SATAlgo.h"
#include "Physics.h"
#define PIXELS_PER_SEC  4


namespace bali
{
    StageMainClient::StageMainClient(GameContext* ctx)
        : Stage(ctx)
    {
    }

    StageMainClient::~StageMainClient()
    {
    }

    uint32_t StageMainClient::initialize()
    {
        GameContext* ctx = getContext();
        ///////////////////////////////////////////////////////////////////////
        // Load TMX
        TMX::TMXReader::load("level5.tmx", ctx->mctx);

        // Still playing with these...no clue yet
        TMX::Objectgroup::Ptr pObjG = ctx->mctx.getObjectGroup(ctx->mctx.maps.back(), "PlayerShape");
        

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
        TMX::Tileset::Ptr tilesetA = getTileset("tilesetA", ctx->mctx.maps.back()->tilesets);
        ctx->tilesetAImg.loadFromFile(tilesetA->images.back()->source);
        ctx->tilesetAImg.createMaskFromColor(sf::Color::Black);//transparent is black...
        ctx->tilesetATex.loadFromImage(ctx->tilesetAImg);


        TMX::Tileset::Ptr tilesetBkgnd = getTileset("background_01", ctx->mctx.maps.back()->tilesets);
        ctx->backgroundImg.loadFromFile(tilesetBkgnd->images.back()->source);
        ctx->backgroundTex.loadFromImage(ctx->backgroundImg);
        ///////////////////////////////////////////////////////////////////////
        // Store TMX map layers into our TileLayers data structure
        ctx->tileLayers.push_back(TileLayer());
        buildTileLayer(ctx->tileLayers.back(), tilesetBkgnd, ctx->mctx.maps.back()->layers[0]);

        ctx->tileLayers.push_back(TileLayer());
        buildTileLayer(ctx->tileLayers.back(), tilesetA, ctx->mctx.maps.back()->layers[1]);
        ///////////////////////////////////////////////////////////////////////
        buildPolygonLayers(ctx->polygons,
            ctx->mctx.maps.back()->objectgroups);
        
        buildSharedEdgesLayers(ctx->sharedEdges,
            ctx->mctx.maps.back()->objectgroups);
        
        buildPlayerObjectLayers(ctx->playerpolygons,
            ctx->mctx.maps.back()->objectgroups);

        ///////////////////////////////////////////////////////////////////////
        int maxDepth = 15;
        qt::AABB aabb;
        aabb.min.x = -64;
        aabb.min.y = -64;
        aabb.max.x = aabb.max.y = (ctx->mctx.maps.back()->width+2) * 32;//in pixels
        
        SearchLayer searchLayer = std::make_shared<qt::QuadTree>();
        searchLayer->initialize(aabb, maxDepth);

        //buildSearchLayer(searchLayer, );
        buildSearchLayer(searchLayer, ctx->polygons);
        ctx->searchLayers.push_back(searchLayer);
        //buildSearchLayer(ctx->searchLayers.back(), ctx->tileLayers[0]);

        ///////////////////////////////////////////////////////////////////////
        ctx->quadLayers.push_back(bali::QuadLayer());
        buildQuadLayer(ctx->quadLayers.back(), ctx->tileLayers[0], 32, 32);
        ctx->quadLayers.push_back(bali::QuadLayer());
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

    uint32_t StageMainClient::doRemoteEvent()
    {
        return 0;
    }

    uint32_t StageMainClient::doWindowEvent(sf::Event & event)
    {
        static float zoom = 1.0;
        switch (event.type) {
        case sf::Event::LostFocus:
            break;
        case sf::Event::GainedFocus:
            break;
        case sf::Event::MouseWheelScrolled: {
            getContext()->mainView = getContext()->window.getView();
            zoom = (event.mouseWheelScroll.delta > 0 ? 1.2 : 0.8);
            sf::Vector2f v = getContext()->mainView.getSize();
            v.x *= 1.01 * zoom;
            v.y *= 1.01 * zoom;
            getContext()->mainView.setSize(v);
            //getContext()->mainView.zoom(zoom);
            std::cout << "Wheel Scrolled: " << event.mouseWheelScroll.delta << std::endl;
            break;
        }case sf::Event::Resized: {
            break;
        }case sf::Event::MouseMoved: {
            break;
        }case sf::Event::KeyPressed:
            break;
        case sf::Event::Closed:
            break;
        }
        return 0;
    }

    uint32_t StageMainClient::doLocalInputs()
    {
        MouseKeyboard::doMouse(*getContext());
        MouseKeyboard::doKeyboard(*getContext());

        return 0;
    }

   
    uint32_t StageMainClient::doUpdate()
    {
        GameContext* ctx = getContext();
        sf::Time elapsed = ctx->mainClock.restart();
        vec::VECTOR2 zxc;
        for (int d = 0; d < ctx->player.posHist.size(); d++)
        {
            zxc += ctx->player.posHist[d];
        }
        zxc /= ctx->player.posHist.size();
        // Search for foreground that is visible        
        qt::AABB searchRegion = getSearchRegion(ctx->mainView, 1.0f);
        TMX::Tileset::Ptr tilesetA = getTileset("tilesetA", ctx->mctx.maps.back()->tilesets);
        TMX::Tileset::Ptr tilesetBkgnd = getTileset("background_01", ctx->mctx.maps.back()->tilesets);
        sf::Uint32 tw = tilesetA->tilewidth;
        sf::Uint32 th = tilesetA->tileheight;


        //Does the player hit anything?
        // convert All obstacle tiles into shapes.
        
        std::vector<qt::XY> sr;
        sr = ctx->searchLayers.at(0)->search(searchRegion);
        
        ctx->polygonsVisible.clear();
        for (auto p = sr.begin(); p != sr.end(); p++)
        {
            float x, y;
            x = p->x;
            y = p->y;

            ctx->polygonsVisible.push_back(CONVEXSHAPE());
            ctx->polygonsVisible.back() = ctx->polygons[p->ti];
        }

        // Every shape that is tested for collision by the player
        // shape is to be placed into this vector
        std::vector<SAT::Shape> shapes;

        // Convert generic convex polygons  to shapes
        for (int j = 0; j < ctx->polygonsVisible.size(); ++j)
        {
            shapes.push_back(SAT::Shape());
            shapes.back().offsetX = ctx->polygonsVisible[j].offsetX;
            shapes.back().offsetY = ctx->polygonsVisible[j].offsetY;

            for (int i = 0; i < ctx->polygonsVisible[j].getPointCount(); i++)
            {
                sf::Vector2f v = ctx->polygonsVisible[j].getPoint(i);
                shapes.back().addVertex(v.x, v.y);
            }
        }


        // A player "shape" may be composed of multiple polygons.
        // therefore, we need to convert each polygon to a shape.
        std::vector<SAT::Shape> playershapes;

        vec::VECTOR2 pos((ctx->player.pos.x), (ctx->player.pos.y));
        // Convert Player polygons to shapes
        for (int j = 0; j < ctx->playerpolygons.size(); ++j)
        {
            playershapes.push_back(SAT::Shape());
            for (int i = 0; i < ctx->playerpolygons[j].getPointCount(); i++)
            {
                sf::Vector2f v = ctx->playerpolygons[j].getPoint(i);
                playershapes.back().addVertex(v.x, v.y);
            }
            float hafWid =  ctx->playerpolygons[j].getLocalBounds().width / 2.0;
            float hafHite =  ctx->playerpolygons[j].getLocalBounds().height / 2.0;

            ctx->playerpolygons[j].setPosition(pos.x - hafWid, pos.y + hafHite);
            playershapes.back().translate(pos.x - hafWid, pos.y + hafHite);
        }


        // Resolve collisions
        // Integrate motion
        // Create Line of Sight Triangle Fan
        physics::ResolveCollisions(shapes, playershapes, ctx->player);
        physics::update(ctx->player, elapsed);
        physics::createLoSTriFan(shapes, ctx);

        ctx->mainView.setCenter(zxc.x, zxc.y);
        getContext()->mainView.setRotation(getContext()->player.angle);
        return 0;
    }

    uint32_t StageMainClient::doDraw()
    {
        GameContext* ctx = getContext();
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

        //// Hit Edges
        //ctx->pRenderTexture0->draw(ctx->lineSegments);

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

        // line segments - debug
        //for (auto poly = ctx->lineSegments.begin(); poly != ctx->playerpolygons.end(); ++poly)
        //{
        //    sf::RenderStates polyPlayerRenderStates;
        //    poly->setOutlineColor(sf::Color::Yellow);
        //    poly->setFillColor(sf::Color::Green);
        //    poly->setOutlineThickness(2);
        //    ctx->pRenderTexture0->draw(*poly, polyPlayerRenderStates);
        //}
        sf::RenderStates lineSegRenderState;
        
        ctx->pRenderTexture1->draw(ctx->lineSegments, lineSegRenderState);
        ctx->pRenderTexture1->display();

        ctx->pRenderTexture0->display();

        sf::RenderStates totalRenderStates;
        totalRenderStates.shader = &ctx->shader;
        //totalRenderStates.texture = (sf::Texture*)ctx->pRenderTexture0;
        ctx->shader.setUniform("view", ctx->mainView.getSize());
        ctx->shader.setUniform("texture", sf::Shader::CurrentTexture);
        vec::VECTOR2 pposTemp = ctx->player.pos;
        pposTemp.x = pposTemp.x + 20;
        pposTemp.y = pposTemp.y - 13;
        ctx->shader.setUniform("position", sf::Glsl::Vec2(ctx->window.mapCoordsToPixel(pposTemp.sf())));
        ctx->shader.setUniform("resolution", sf::Glsl::Vec2(2048, 2048));
        ctx->shader.setUniform("losMask", ctx->pRenderTexture1->getTexture());


        sf::Sprite sprite(ctx->pRenderTexture0->getTexture());
        ctx->window.draw(sprite, totalRenderStates);

        //ctx->pRenderTexture0->draw(ctx->lineSegments);
        //sprite = sf::Sprite(ctx->pRenderTexture0->getTexture());
        //totalRenderStates.shader = nullptr;
        //ctx->window.draw(sprite, totalRenderStates);
        //
        // Swap
        //sf::RenderTexture* temp = ctx->pRenderTexture0;
        //ctx->pRenderTexture0 = ctx->pRenderTexture1;
        //ctx->pRenderTexture1 = temp;

        // Finalize it
        ctx->window.display();
        return 0;
    }

    uint32_t StageMainClient::cleanup()
    {
        return 0;
    }


}