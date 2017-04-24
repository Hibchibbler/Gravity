#include "StageMainClient.h"
#include "GameContext.h"
#include "Utility.h"
#include <math.h>
#include "SATAlgo\SATAlgo.h"

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

        ctx->player.position.x = 250;
        ctx->player.position.y = 100;
        ctx->player.velocity = sf::Vector2f(0.0, 0.0);
        ctx->player.acceleration = sf::Vector2f(0.0, 0.0);

        ctx->size.x = ctx->size.y = 1000;
        ctx->mainView.setCenter(ctx->player.position.x, ctx->player.position.y);
        ctx->mainView.setSize(sf::Vector2f(ctx->size.x, ctx->size.y));
        //ctx->mainView.setViewport(sf::FloatRect(0.25,0.25,0.5,0.5));

        // Load TMX
        TMX::TMXReader::load("level1.tmx", ctx->mctx);

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
            getContext()->mainView.zoom(zoom);
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

        //if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        //{
        //    //this->done();
        //    sf::Vector2i mpos = sf::Mouse::getPosition(getContext()->window);
        //    sf::Vector2f worldPos = getContext()->window.mapPixelToCoords(mpos);
        //}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            getContext()->player.applyJump();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
                getContext()->player.velocity.x += 0.3f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
                getContext()->player.velocity.x -= 0.3f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            getContext()->player.velocity.y -= 0.3f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            getContext()->player.velocity.y += 0.3f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            getContext()->player.angle = getContext()->player.angle - 0.05;
            if (getContext()->player.angle < 0)
                getContext()->player.angle = 360;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            getContext()->player.angle = getContext()->player.angle + 0.05;
            if (getContext()->player.angle > 360)
                getContext()->player.angle = 0;
        }

        return 0;
    }

    sf::Vector2f normalize(sf::Vector2f v)
    {
        double len = sqrt(v.x*v.x + v.y*v.y);
        if (len > 0)
        {
            v.x /= len;
            v.y /= len;
        }
        return v;
    }

    float magnitude(sf::Vector2f v)
    {
        float m;
        m = sqrt(pow(v.x, 2) + pow(v.y, 2));
        return m;
    }

    sf::Vector2f rotate(sf::Vector2f v, float angle)
    {
        angle = angle *(3.14156f / 180.0f);
        v.x = v.x * cos(angle) - v.y * sin(angle);
        v.y = v.x * sin(angle) + v.y * cos(angle);
        return v;
    }

    sf::Vector2f translate(sf::Vector2f v, sf::Vector2f t)
    {
        v.x += t.x;
        v.y += t.y;
        return v;
    }
     
    uint32_t StageMainClient::doUpdate()
    {
        GameContext* ctx = getContext();
        sf::Time elapsed = ctx->mainClock.restart();

        ctx->player.update(elapsed);

        getContext()->mainView.setRotation(getContext()->player.angle);
        ctx->mainView.setCenter(ctx->player.position);

            // Search for foreground that is visible
        qt::AABB searchRegion = getSearchRegion(ctx->mainView);

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
        //// Convert Tiles for SAT
        //for (auto xy = sr.begin(); xy != sr.end(); ++xy)
        //{
        //    float x, y;
        //    x = ctx->tileLayers[1][xy->ti].x;
        //    y = ctx->tileLayers[1][xy->ti].y;

        //    shapes.push_back(SAT::Shape());
        //    shapes.back().addVertex(x     , y     );
        //    shapes.back().addVertex(x + tw, y     );
        //    shapes.back().addVertex(x + tw, y + th);
        //    shapes.back().addVertex(x     , y + th);
        //}

        SAT::Shape playerShape;
        {
            SAT::Vector2 v(ctx->player.position.x, ctx->player.position.y);

            // Create renderable player
            ctx->player.playerQuads.clear();
            addRotQuad(ctx->player.playerQuads,
                    sf::FloatRect(v.x , v.y , tw, th),
                    sf::IntRect(2 * 32, 7 * 32, 32, 32) , ctx->player.angle);

            // convert renderable player into a collision shape.
            v.x = ctx->player.playerQuads[0].position.x;
            v.y = ctx->player.playerQuads[0].position.y;
            playerShape.addVertex(v.x     , v.y     );

            v.x = ctx->player.playerQuads[1].position.x;
            v.y = ctx->player.playerQuads[1].position.y;
            //playerShape.addVertex(v.x + tw, v.y     );
            playerShape.addVertex(v.x , v.y);

            v.x = ctx->player.playerQuads[2].position.x;
            v.y = ctx->player.playerQuads[2].position.y;
            //playerShape.addVertex(v.x + tw, v.y + th);
            playerShape.addVertex(v.x , v.y );

            v.x = ctx->player.playerQuads[3].position.x;
            v.y = ctx->player.playerQuads[3].position.y;
            //playerShape.addVertex(v.x     , v.y + th);
            playerShape.addVertex(v.x, v.y );
        }

        // compare player shape to all other shapes for collision        
        for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
        {
            
            SAT::Shape prev = *shape;
            //for (int tries = 1; tries < 2; ++tries)
            //{
            for (auto v = prev.vertices.begin(); v != prev.vertices.end(); ++v)
            {
                sf::Vector2f w(v->x, v->y);
                sf::Vector2f t = ctx->player.velocity;
                t.x = (t.x / (3)) * -1;
                t.y = (t.y / (3)) * -1;

                w = translate(w, t);
                v->x = w.x;
                v->y = w.y;
            }
            //}
            SAT::MTV mtv;
            SAT::MTV mtv1;
            //SAT::MTV mtv2;
            bool collider1 = playerShape.collision(*shape, mtv1);
            //bool collider2 = playerShape.collision(prev, mtv2);
            //if (collider1 || collider2)
            if (collider1)
            {
                mtv = mtv1;// (collider1 == true ? mtv1 : mtv2);

                SAT::Vector2 v = mtv.smallest;
               
                //std::cout << "###<" << mtv.smallest.x << ", " << mtv.smallest.y << " | " << v.x << ", " << v.y << " | "<< mtv.overlap << ">### " << std::endl;
                //std::cout << "     <" << ctx->player.velocity.x << ", " << ctx->player.velocity.y << ">";

                double dp = (ctx->player.velocity.x * v.x + ctx->player.velocity.y * v.y);
                if (dp > 0.0)
                {
                    v.x = -1 * v.x;
                    v.y = -1 * v.y;
                }

                //std::cout << "###<" << mtv.smallest.x << ", " << mtv.smallest.y << " | " << mtv.overlap << ">### " << std::endl;
                std::cout << "###<" << v.x << ", " << v.y << " | " << mtv.overlap << ">### " << std::endl;

                if (v.x != 0) {
                    ctx->player.position.x += v.x * mtv.overlap * 1.00;
                    ctx->player.velocity.x = 0;// v.x * ctx->player.velocity.x *0.9;// mtv.overlap * 9.5;
                }
                if (v.y != 0) {
                    ctx->player.position.y += v.y * mtv.overlap * 1.00;
                    ctx->player.velocity.y = 0;// v.y * ctx->player.velocity.y *0.9;// mtv.overlap * 9.5;
                }
                //std::cout << "     <" << ctx->player.velocity.x << ", " << ctx->player.velocity.y << ">" << std::endl;

                break;
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

        ctx->window.draw(ctx->player.playerQuads,  states);

        for(auto poly = ctx->polygons.begin(); poly != ctx->polygons.end(); ++poly)
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