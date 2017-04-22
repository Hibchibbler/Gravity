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
        ctx->player.position.y = 300;
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
            //getContext()->mainView.move(2, 0);//2*co0s(getContext()->angle), 0);
            //if (getContext()->player.currentMTV.smallest.x == 0)
                getContext()->player.velocity.x += 20.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            //getContext()->mainView.move(-2, 0);// -2 * cos(getContext()->angle), 0);
            //if (getContext()->player.currentMTV.smallest.x == 0)
                getContext()->player.velocity.x -= 20.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            //getContext()->mainView.move(0, -2);
            getContext()->player.velocity.y -= 20.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            getContext()->player.velocity.y += 20.0f;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            //getContext()->levelRotTrans.translate(sf::Vector2f(1024, 1024));
            //getContext()->levelRotTrans.rotate(2);
            getContext()->player.angle -= 2;
            if (getContext()->player.angle < 0)
                getContext()->player.angle = 360;
            //getContext()->levelRotTrans.translate(sf::Vector2f(-1024, -1024));

            getContext()->mainView.setRotation(getContext()->mainView.getRotation() - 2);
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            //getContext()->levelRotTrans.translate(sf::Vector2f(1024, 1024));
            //getContext()->levelRotTrans.rotate(-2);
            getContext()->player.angle += 2;
            if (getContext()->player.angle > 360)
                getContext()->player.angle = 0;
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

    sf::Vector2f rotate(sf::Vector2f v, float angle)
    {
        angle = angle *(3.14156f / 180.0f);
        v.x = v.x * cos(angle) - v.y * sin(angle);
        v.y = v.y * sin(angle) + v.y * cos(angle);
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
        float dt = elapsed.asSeconds();// / PIXELS_PER_SEC;

        //ctx->player.acceleration.y += 1.5;// *dt;// Gravity 9.8 m/s^2

        ctx->player.update(elapsed);

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

        for (int j = 0; j < ctx->polygons.size(); ++j)
        {
            shapes.push_back(SAT::Shape());
            for (int i = 0; i < ctx->polygons[j].getPointCount(); i++)
            {
                sf::Vector2f v = ctx->polygons[j].getPoint(i);
                shapes.back().addVertex(v.x, v.y);
            }
        }
        for (auto xy = sr.begin(); xy != sr.end(); ++xy)
        {
            float x, y;
            x = ctx->tileLayers[1][xy->ti].x;
            y = ctx->tileLayers[1][xy->ti].y;

            shapes.push_back(SAT::Shape());
            shapes.back().addVertex(x     , y     );
            shapes.back().addVertex(x + tw, y     );
            shapes.back().addVertex(x + tw, y + th);
            shapes.back().addVertex(x     , y + th);
        }
        //SAT::Shape playerShape;
        //{
        //    sf::Vector2f pos;
        //    pos.x = ctx->player.position.x;
        //    pos.y = ctx->player.position.y;

        //    // Create renderable player
        //    ctx->player.playerQuads.clear();
        //    addQuad(ctx->player.playerQuads,
        //        sf::FloatRect(pos.x, pos.y, tw, th),
        //        sf::IntRect(63 * 32, 63 * 32, 32, 32));

        //    // convert renderable player into a collision shape.
        //    sf::Vector2f v0 = pos;
        //    v0.x += 0;
        //    v0.y += 0;
        //    v0 = rotate(v0, ctx->player.angle);
        //    playerShape.addVertex(v0.x, v0.y);

        //    sf::Vector2f v1 = pos;
        //    v1.x += tw;
        //    v1.y += 0;
        //    v1 = rotate(v1, ctx->player.angle);
        //    playerShape.addVertex(v1.x, v1.y);

        //    sf::Vector2f v2 = pos;
        //    v2.x += tw;
        //    v2.y += th;
        //    v2 = rotate(v2, ctx->player.angle);
        //    playerShape.addVertex(v2.x, v2.y);

        //    sf::Vector2f v3 = pos;
        //    v3.x += 0;
        //    v3.y += th;
        //    v3 = rotate(v3, ctx->player.angle);
        //    playerShape.addVertex(v3.x, v3.y);
        //}
        SAT::Shape playerShape;
        {
            float x, y;
            x = ctx->player.position.x;
            y = ctx->player.position.y;

            // Create renderable player
            ctx->player.playerQuads.clear();
            addRotQuad(ctx->player.playerQuads,
                    sf::FloatRect(x+16, y+16, tw, th),
                    sf::IntRect(63 * 32, 63 * 32, 32, 32), ctx->player.angle);

            // convert renderable player into a collision shape.
            playerShape.addVertex(x     , y     );
            playerShape.addVertex(x + tw, y     );
            playerShape.addVertex(x + tw, y + th);
            playerShape.addVertex(x     , y + th);
        }
        /*<0.757673, 0.652634>, 0.419678     <66, 50>     <37.3819, 25.3494>
<0.757673, 0.652634>, 0.399048     <37.3819, 25.3494>     <10.1706, 1.91046>
<0.757673, 0.652634>, 0.0808105     <10.1706, 1.91046>     <4.66009, -2.83611>
<0.757673, 0.652634>, 0.0179443     <4.66009, -2.83611>     <3.43645, -3.89011>
<0.757673, 0.652634>, 0.000732422     <3.43645, -3.89011>     <3.38651, -3.93313>
<0.757673, 0.652634>, 0     <3.38651, -3.93313>     <3.38651, -3.93313>
<0.757673, 0.652634>, 0     <3.38651, -3.93313>     <3.38651, -3.93313>
<0.757673, 0.652634>, 0     <3.38651, -3.93313>     <3.38651, -3.93313>
<0.757673, 0.652634>, 0     <3.38651, -3.93313>     <3.38651, -3.93313>
<0.757673, 0.652634>, 0     <3.38651, -3.93313>     <3.38651, -3.93313>*/
            
        // compare player shape to all other shapes for collision        
        for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
        {
            SAT::MTV mtv;
            SAT::Shape prev = *shape;
            for (int tries = 1; tries < 6; ++tries)
            {
                for (auto v = prev.vertices.begin(); v != prev.vertices.end(); ++v)
                {
                    sf::Vector2f w(v->x, v->y);
                    sf::Vector2f t = ctx->player.velocity;
                    t.x = (t.x / (tries * 5)) * -1;
                    t.y = (t.y / (tries *5)) * -1;

                    w = translate(w, t);
                    v->x = w.x;
                    v->y = w.y;
                }
            }
            if (1)
            {
                if (playerShape.collision(*shape, mtv) ||
                    playerShape.collision(prev, mtv))
                {
                    ctx->player.currentMTV = mtv;

                    std::cout << "<" << mtv.smallest.x << ", " << mtv.smallest.y << ", " << mtv.overlap << "> ";
                    std::cout << "     <" << ctx->player.velocity.x << ", " << ctx->player.velocity.y << ">";
                    if (mtv.smallest.x == 0 && mtv.smallest.y == 1)
                        int a = 42;
                    SAT::Vector2 v = mtv.smallest.normalize();
                    
                    //////////////////////////////////////////////////////////////////
                    ctx->player.position.x += -v.x * mtv.overlap * 1.5;
                    ctx->player.velocity.x = -v.x * mtv.overlap * 50;

                    ctx->player.position.y += -v.y * mtv.overlap * 1.5;// *dt; // #1
                    ctx->player.velocity.y = -v.y *mtv.overlap * 50;// *20; // #2
                    //////////////////////////////////////////////////////////////////

                    
                    //////if (mtv.smallest.x != 0)
                    //////{
                    //////    //ctx->player.position.x -= mtv.smallest.x*2; // 
                    //////    ctx->player.position.x += -v.x * mtv.overlap * 1.5;// *dt; // #1
                    //////    ctx->player.velocity.x = -v.x * mtv.overlap * 50;// *20; // #2
                    //////    //ctx->player.velocity.x += -mtv.smallest.x; // #3
                    //////    //ctx->player.velocity.x *= -1;
                    //////    //ctx->player.velocity.x *= -mtv.smallest.x;

                    //////}
                    //////if (mtv.smallest.y != 0)
                    //////{
                    //////    //ctx->player.position.y -= mtv.smallest.y*2; // 
                    //////    ctx->player.position.y += -v.y * mtv.overlap * 1.5;// *dt; // #1
                    //////    ctx->player.velocity.y = -v.y * mtv.overlap * 50;// *20; // #2
                    //////    //ctx->player.velocity.y += -mtv.smallest.y;  // #3
                    //////    //ctx->player.velocity.y *= -1;
                    //////    //ctx->player.velocity.y *= -mtv.smallest.y;
                    //////}

                    std::cout << "     <" << ctx->player.velocity.x << ", " << ctx->player.velocity.y << ">" << std::endl;

                    break;
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