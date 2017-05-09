///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageMainClient.h"
#include "GameContext.h"
#include "Utility.h"
#include "MouseKeyboard.h"
#include <math.h>
#include "Vector\Vector.h"
#include "SATAlgo\SATAlgo.h"

#define PIXELS_PER_SEC  4
#define DEG_TO_RAD(x)   \
        x * (3.14156f / 180.0f);

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

        ctx->player.position.x = 250;
        ctx->player.position.y = 100;
        ctx->player.velocity = vec::Vector2(0.0, 0.0);
        ctx->player.acceleration = vec::Vector2(0.0, 0.0);

        ctx->size.x = ctx->size.y = 1000;
        ctx->mainView.setCenter(ctx->player.position.x, ctx->player.position.y);
        ctx->mainView.setSize(sf::Vector2f(ctx->size.x, ctx->size.y));
        //ctx->mainView.setViewport(sf::FloatRect(0.25,0.25,0.5,0.5));

        // Load TMX
        TMX::TMXReader::load("level0.tmx", ctx->mctx);

        ctx->tilesetAImg.loadFromFile(ctx->mctx.maps.back()->tilesets.back()->images.back()->source);
        ctx->tilesetATex.loadFromImage(ctx->tilesetAImg);

        // Let's use this tileset.
        TMX::Tileset::Ptr tileset = getTileset("tilesetA", ctx->mctx.maps.back()->tilesets);
        if (tileset == nullptr)
        {
            cout << "ERROR: didn't find tilesetA" << endl;
            return 0;
        }

        // Store TMX map layers into our TileLayers data structure
        buildTileLayers(ctx->tileLayers,
            ctx->mctx.maps.back()->tilesets.back(),
            ctx->mctx.maps.back()->layers);

        buildObjectLayers(ctx->polygons,
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

        ctx->player.update(elapsed);

        getContext()->mainView.setRotation(getContext()->player.angle);
        ctx->mainView.setCenter(ctx->player.position.x, ctx->player.position.y);

        // Search for foreground that is visible
        qt::AABB searchRegion = getSearchRegion(ctx->mainView, 0.80);

        sf::Uint32 tw = ctx->mctx.getMap()->tilesets.back()->tilewidth;
        sf::Uint32 th = ctx->mctx.getMap()->tilesets.back()->tileheight;

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
            for (int i = 0; i < ctx->polygons[j].getPointCount(); i++)
            {
                sf::Vector2f v = ctx->polygons[j].getPoint(i);
                shapes.back().addVertex(v.x, v.y);
            }
        }
        ////// Convert Tiles for SAT
        //for (auto xy = sr.begin(); xy != sr.end(); ++xy)
        //{
        //    //vec::Vector2 v1(ctx->tileLayers[1][xy->ti].x, ctx->tileLayers[1][xy->ti].y);
        //    //vec::Vector2 v2(ctx->tileLayers[1][xy->ti].x+tw, ctx->tileLayers[1][xy->ti].y);
        //    //vec::Vector2 v3(ctx->tileLayers[1][xy->ti].x+tw, ctx->tileLayers[1][xy->ti].y+th);
        //    //vec::Vector2 v4(ctx->tileLayers[1][xy->ti].x, ctx->tileLayers[1][xy->ti].y+th);
        //    shapes.push_back(SAT::Shape());
        //    addRotShape(shapes.back(), sf::FloatRect(ctx->tileLayers[1][xy->ti].x, ctx->tileLayers[1][xy->ti].y, tw, th), 360-ctx->player.angle);
        //}

        SAT::Shape playerShape;
        {
            vec::Vector2 v(ctx->player.position.x, ctx->player.position.y);

            // Create renderable player
            ctx->player.playerQuads.clear();
            addQuad(ctx->player.playerQuads,
                sf::FloatRect(v.x, v.y, tw, th),
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
        }

        // compare player shape to all other shapes for collision        
        for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
        {
            SAT::MTV mtv1;
            std::vector<vec::Vector2> axes;
            bool collider1 = playerShape.collision(*shape, mtv1, axes);

            if (collider1)
            {
                SAT::MTV mtv = mtv1;
                vec::Vector2 collision_normal = mtv.smallest;

                vec::Vector2 original_velocity = ctx->player.velocity;
                double new_magnitude = original_velocity.dot(collision_normal);
                vec::Vector2 new_velocity = original_velocity * new_magnitude * -1;

                float a = 360 - ctx->player.angle;
                a = DEG_TO_RAD(a);
 
                vec::Vector2 rotated_velocity;
                rotated_velocity.x = new_velocity.magnitude() * cos(a);
                rotated_velocity.y = new_velocity.magnitude() * sin(a);
                rotated_velocity = rotated_velocity.normalize();

                double dp = ctx->player.velocity.dot(rotated_velocity);
                if (dp > 0.0)
                {
                    rotated_velocity.y = rotated_velocity.y * -1;
                    rotated_velocity.x = rotated_velocity.x * -1;
                }

                std::cout << "###";
                std::cout << "<CN " << collision_normal.x << ", " << collision_normal.y << ">, ";
                std::cout << "<OV " << ctx->player.velocity.x << ", " << ctx->player.velocity.y << ">, ";
                std::cout << "<NV " << new_velocity.x << ", " << new_velocity.y << "> ";
                std::cout << "<A " << ctx->player.angle << ">, ";
                std::cout << "<RV " << rotated_velocity.x << ", " << rotated_velocity.y << ">, ";
                std::cout << "<DP " << dp << ">, ";
                std::cout << "### " << std::endl;

                if (collision_normal.x != 0) {
                    ctx->player.position.x += rotated_velocity.x * mtv.overlap * 5;
                    ctx->player.velocity.x = rotated_velocity.x * ctx->player.velocity.magnitude();
                }

                if (collision_normal.y != 0) {
                    ctx->player.position.y += rotated_velocity.y * mtv.overlap * 5;
                    ctx->player.velocity.y = rotated_velocity.y* ctx->player.velocity.magnitude();
                }
            }
        }
        return 0;
    }

    uint32_t StageMainClient::doDraw()
    {
        GameContext* ctx = getContext();

        sf::RenderStates states;

        ctx->window.setView(ctx->mainView);
        ctx->window.clear();

        states.texture = &ctx->tilesetATex;
        // states.transform = ctx->levelRotTrans;

        // Draw background
        auto i = ctx->quadLayers.front();
        ctx->window.draw(i, states);

        // Draw visible foregound
        ctx->window.draw(ctx->visibleQuads, states);

        ctx->window.draw(ctx->player.playerQuads, states);

        for (auto poly = ctx->polygons.begin(); poly != ctx->polygons.end(); ++poly)
        {
            poly->setOutlineColor(sf::Color::Red);
            poly->setFillColor(sf::Color::Transparent);
            poly->setOutlineThickness(1);
            ctx->window.draw(*poly, states);
        }

        // Finalize it
        ctx->window.display();
        return 0;
    }

    uint32_t StageMainClient::cleanup()
    {
        return 0;
    }


}