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

        ctx->player.position.x = 1024;
        ctx->player.position.y = 500;
        ctx->player.velocity = sf::Vector2f(0.0, 0.0);
        ctx->player.acceleration = sf::Vector2f(0.0, 0.0);

        ctx->size.x = ctx->size.y = 800;
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            //getContext()->mainView.move(2, 0);//2*co0s(getContext()->angle), 0);
           // if (getContext()->player.currentMTV.smallest.x == 0)
                getContext()->player.velocity.x += 2.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            //getContext()->mainView.move(-2, 0);// -2 * cos(getContext()->angle), 0);
            //if (getContext()->player.currentMTV.smallest.x == 0)
                getContext()->player.velocity.x -= 2.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            //getContext()->mainView.move(0, -2);
            getContext()->player.velocity.y -= 2.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            getContext()->player.velocity.y += 2.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            //getContext()->levelRotTrans.translate(sf::Vector2f(1024, 1024));
            //getContext()->levelRotTrans.rotate(2);
            getContext()->angle -= 2;
            //getContext()->levelRotTrans.translate(sf::Vector2f(-1024, -1024));

            getContext()->mainView.setRotation(getContext()->mainView.getRotation() - 2);
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            //getContext()->levelRotTrans.translate(sf::Vector2f(1024, 1024));
            //getContext()->levelRotTrans.rotate(-2);
            getContext()->angle += 2;
            //getContext()->levelRotTrans.translate(sf::Vector2f(-1024, -1024));

            getContext()->mainView.setRotation(getContext()->mainView.getRotation() + 2);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        {
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        {
        }
        return 0;
    }

    sf::Vector2f normalize(sf::Vector2f v)
    {
        
        float len = sqrt(v.x*v.x + v.y*v.y);
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

    uint32_t StageMainClient::doUpdate()
    {
        GameContext* ctx = getContext();

        sf::Time elapsed = ctx->mainClock.restart();
        float dt = elapsed.asSeconds();// / PIXELS_PER_SEC;

        //ctx->player.acceleration.y += 1.5;// *dt;// Gravity 9.8 m/s^2

        ctx->player.update(elapsed);

        ctx->mainView.setCenter(ctx->player.position.x, ctx->player.position.y);

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

        for (int j = 0; j < ctx->polygons.size(); ++j)
        {
            shapes.push_back(SAT::Shape());
            for (int i = 0; i < ctx->polygons[j].getPointCount(); i++)
            {
                sf::Vector2f v = ctx->polygons[j].getPoint(i);
                shapes.back().addVertex(v.x, v.y);
            }
        }
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
            float x, y;
            x = ctx->player.position.x;
            y = ctx->player.position.y;

            // Create renderable player
            ctx->player.playerQuads.clear();
            addQuad(ctx->player.playerQuads,
                    sf::FloatRect(x, y, tw, th),
                    sf::IntRect(63 * 32, 63 * 32, 32, 32));

            // convert renderable player into a collision shape.
            playerShape.addVertex(x     , y     );
            playerShape.addVertex(x + tw, y     );
            playerShape.addVertex(x + tw, y + th);
            playerShape.addVertex(x     , y + th);
        }

        // compare player shape to all other shapes for collision        
        for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
        {
            SAT::MTV mtv;
            if (playerShape.collision(*shape, mtv))
            {
                ctx->player.currentMTV = mtv;

                std::cout << "<" << mtv.smallest.x << ", " << mtv.smallest.y << ">, " << mtv.overlap;
                std::cout << "     <" << ctx->player.velocity.x << ", " << ctx->player.velocity.y << ">";

                SAT::Vector2 v = mtv.smallest.normalize();
                if (mtv.smallest.x != 0)
                {
                    ctx->player.position.x += -v.x * mtv.overlap;
                    //ctx->player.velocity.x += -v.x * mtv.overlap;
                    //ctx->player.velocity.x = -mtv.smallest.x;
                    
                }
                if (mtv.smallest.y != 0)
                {
                    ctx->player.position.y += -v.y * mtv.overlap;
                    //ctx->player.velocity.y += -v.y * mtv.overlap;
                    //ctx->player.velocity.y = -mtv.smallest.y;
                }
              
                std::cout << "     <" << ctx->player.velocity.x << ", " << ctx->player.velocity.y << ">" << std::endl;

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
            poly->setOutlineThickness(6);
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