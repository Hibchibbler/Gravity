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

        // Still playing with these...no clue yet

        ctx->player.position.x = 175;
        ctx->player.position.y = 90;
        ctx->player.velocity = vec::VECTOR2(0.0, 0.0);
        ctx->player.acceleration = vec::VECTOR2(0.0, 0.0);

        ctx->size.x = ctx->size.y = 1000;
        ctx->mainView.setCenter(ctx->player.position.x, ctx->player.position.y);
        ctx->mainView.setSize(sf::Vector2f(ctx->size.x, ctx->size.y));
        //ctx->mainView.setViewport(sf::FloatRect(0.25,0.25,0.5,0.5));

        // Load TMX
        TMX::TMXReader::load("level0.tmx", ctx->mctx);

        // Let's use this tileset.wadasdsa
        TMX::Tileset::Ptr tilesetA = getTileset("tilesetA", ctx->mctx.maps.back()->tilesets);
        ctx->tilesetAImg.loadFromFile(tilesetA->images.back()->source);
        ctx->tilesetAImg.createMaskFromColor(sf::Color::Black);//transparent is black...
        ctx->tilesetATex.loadFromImage(ctx->tilesetAImg);


        TMX::Tileset::Ptr tilesetBkgnd = getTileset("background_01", ctx->mctx.maps.back()->tilesets);
        ctx->backgroundImg.loadFromFile(tilesetBkgnd->images.back()->source);
        //ctx->backgroundImg.createMaskFromColor(sf::Color::White);//transparent is black...
        ctx->backgroundTex.loadFromImage(ctx->backgroundImg);

        // Store TMX map layers into our TileLayers data structure
        //buildTileLayers(ctx->tileLayers,
        //    tilesetA,
        //    ctx->mctx.maps.back()->layers);
        ctx->tileLayers.push_back(TileLayer());
        buildTileLayer(ctx->tileLayers.back(), tilesetBkgnd, ctx->mctx.maps.back()->layers[0]);

        ctx->tileLayers.push_back(TileLayer());
        buildTileLayer(ctx->tileLayers.back(), tilesetA, ctx->mctx.maps.back()->layers[1]);


        buildPolygonLayers(ctx->polygons,
            ctx->mctx.maps.back()->objectgroups);

        buildSharedEdgesLayers(ctx->sharedEdges,
            ctx->mctx.maps.back()->objectgroups);

        buildPlayerObjectLayers(ctx->playerpolygons,
            ctx->mctx.maps.back()->objectgroups);
        //// Then use the TileLayers to construct the other representations
        //// A representation of the layer that can drawn to the screen
        //buildQuadLayers(ctx-W>quadLayers,
        //                ctx->tileLayers);

        // A representation of the layer that can be searched
        buildSearchLayers(ctx->searchLayers,
            ctx->tileLayers);

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



        // Search for foreground that is visible
        
        qt::AABB searchRegion = getSearchRegion(ctx->mainView, 1.00);

        TMX::Tileset::Ptr tilesetA = getTileset("tilesetA", ctx->mctx.maps.back()->tilesets);
        TMX::Tileset::Ptr tilesetBkgnd = getTileset("background_01", ctx->mctx.maps.back()->tilesets);


        sf::Uint32 tw = tilesetA->tilewidth;
        sf::Uint32 th = tilesetA->tileheight;

        // Generate renderable for background layer (0)
        std::vector<qt::XY> sr;

        sr = ctx->searchLayers.at(0)->search(searchRegion);
        ctx->quadLayers.clear();
        ctx->quadLayers.push_back(QuadLayer());
        for (auto p = sr.begin(); p != sr.end(); p++)
        {
            float x, y;
            x = p->x;
            y = p->y;

            addQuad(ctx->quadLayers.front(), sf::FloatRect(x, y, tw, th), sf::IntRect(ctx->tileLayers[0][p->ti].tx, ctx->tileLayers[0][p->ti].ty, tw, th));
        }

        // Generate renderables for obstacle layer (1)
        sr = ctx->searchLayers.at(1)->search(searchRegion);
        ctx->visibleQuads.clear();
        for (auto p = sr.begin(); p != sr.end(); p++)
        {
            float x, y;
            x = p->x;
            y = p->y;

            addQuad(ctx->visibleQuads, sf::FloatRect(x, y, tw, th), sf::IntRect(ctx->tileLayers[1][p->ti].tx, ctx->tileLayers[1][p->ti].ty, tw, th));
        }

        //Does the player hit anything?
        // convert All obstacle tiles into shapes.
        std::vector<SAT::Shape> shapes;

        // Convert Polygons
        for (int j = 0; j < ctx->polygons.size(); ++j)
        {
            shapes.push_back(SAT::Shape());
            shapes.back().offsetX = ctx->polygons[j].offsetX;
            shapes.back().offsetY = ctx->polygons[j].offsetY;

            for (int i = 0; i < ctx->polygons[j].getPointCount(); i++)
            {
                sf::Vector2f v = ctx->polygons[j].getPoint(i);
                shapes.back().addVertex(v.x, v.y);
                
            }
        }

        //// Convert Tiles for SAT
        //for (auto xy = sr.begin(); xy != sr.end(); ++xy)
        //{
        //    
        //    //vec::Vector2 v1(ctx->tileLayers[1][xy->ti].x, ctx->tileLayers[1][xy->ti].y);
        //    //vec::Vector2 v2(ctx->tileLayers[1][xy->ti].x+tw, ctx->tileLayers[1][xy->ti].y);
        //    //vec::Vector2 v3(ctx->tileLayers[1][xy->ti].x+tw, ctx->tileLayers[1][xy->ti].y+th);
        //    //vec::Vector2 v4(ctx->tileLayers[1][xy->ti].x, ctx->tileLayers[1][xy->ti].y+th);
        //    shapes.push_back(SAT::Shape());
        //    addShape(shapes.back(), xy->ti, ctx->tileLayers[1][xy->ti].gid, sf::FloatRect(ctx->tileLayers[1][xy->ti].x, ctx->tileLayers[1][xy->ti].y, tw, th));
        //}

        vec::VECTOR2 pos((ctx->player.position.x), (ctx->player.position.y));
        SAT::Shape playerShape;
        std::vector<SAT::Shape> playershapes;//not used
        
        // Convert Player Polygon
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

            ctx->playerpolygons[j].setPosition(pos.x + hafWid, pos.y + hafHite);
            playershapes.back().translate(pos.x + hafWid, pos.y + hafHite);
        }

         // Create renderable player
        {
            vec::VECTOR2 v;
            ctx->player.playerQuads.clear();
            addQuad(ctx->player.playerQuads,
                sf::FloatRect(pos.x, pos.y, tw*0.5, th*0.5),
                sf::IntRect(2 * 32, 7 * 32, 32, 32));// , ctx->player.angle);

            // convert renderable player into a collision shape.
            v.x = ctx->player.playerQuads[0].position.x;
            v.y = ctx->player.playerQuads[0].position.y;
            playerShape.addVertex(v.x, v.y);

            v.x = ctx->player.playerQuads[1].position.x;
            v.y = ctx->player.playerQuads[1].position.y;
            playerShape.addVertex(v.x, v.y);

            v.x = ctx->player.playerQuads[2].position.x;
            v.y = ctx->player.playerQuads[2].position.y;
            playerShape.addVertex(v.x, v.y);

            v.x = ctx->player.playerQuads[3].position.x;
            v.y = ctx->player.playerQuads[3].position.y;
            playerShape.addVertex(v.x, v.y);

            playerShape.offsetX = pos.x;
            playerShape.offsetY = pos.y;

            //playershapes.push_back(playerShape);
        }

        
        bool isCollided = physics::ResolveCollisions(shapes, playershapes, ctx->player, ctx->tileLayers[1], ctx);
        ctx->player.update(elapsed);
        

        vec::VECTOR2 zxc;
        for (int d = 0; d < ctx->player.posHist.size(); d++)
        {
            zxc += ctx->player.posHist[d];
        }
        zxc /= ctx->player.posHist.size();

        ctx->mainView.setCenter(zxc.x, zxc.y);// ctx->player.position.x, ctx->player.position.y);
        getContext()->mainView.setRotation(getContext()->player.angle);
        return 0;
    }

    uint32_t StageMainClient::doDraw()
    {
        GameContext* ctx = getContext();

        sf::RenderStates states;

        ctx->window.setView(ctx->mainView);
        ctx->window.clear();

        states.texture = &ctx->backgroundTex;
        // Draw background
        auto i = ctx->quadLayers.front();
        ctx->window.draw(i, states);


        states.texture = &ctx->tilesetATex;
        // Draw visible foregound
        ctx->window.draw(ctx->visibleQuads, states);

        //for (auto poly = ctx->polygons.begin(); poly != ctx->polygons.end(); ++poly)
        //{
        //    poly->setOutlineColor(sf::Color::Red);
        //    poly->setFillColor(sf::Color::Transparent);
        //    poly->setOutlineThickness(1);
        //    ctx->window.draw(*poly, states);
        //}

        for (auto poly = ctx->playerpolygons.begin(); poly != ctx->playerpolygons.end(); ++poly)
        {
            poly->setOutlineColor(sf::Color::Yellow);
            poly->setFillColor(sf::Color::Green);
            poly->setOutlineThickness(1);
            ctx->window.draw(*poly, states);
        }


        //ctx->window.draw(ctx->player.playerQuads, states);

        // Finalize it
        ctx->window.display();
        return 0;
    }

    uint32_t StageMainClient::cleanup()
    {
        return 0;
    }


}