#include "StageMainClient.h"
#include "GameContext.h"
#include "Utility.h"

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

        // Then use the TileLayers to construct the other representations
        // A representation of the layer that can drawn to the screen
        buildQuadLayers(ctx->quadLayers,
                        ctx->tileLayers);

        // A representation of the layer that can be searched
        buildSearchLayers(ctx->searchLayers,
                          ctx->tileLayers);

        // Still playing with these...no clue yet
        ctx->mainView.setCenter(1024, 1024);
        ctx->mainView.setSize(sf::Vector2f(800, 800));
        //ctx->mainView.setViewport(sf::FloatRect(0.25,0.25,0.5,0.5));

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
            getContext()->mainView.move(2, 0);
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            getContext()->mainView.move(-2, 0);
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            getContext()->mainView.move(0, -2);
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            getContext()->mainView.move(0, 2);
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            getContext()->levelRotTrans.translate(sf::Vector2f(1024, 1024));
            getContext()->levelRotTrans.rotate(2);
            getContext()->angle -= 2;
            getContext()->levelRotTrans.translate(sf::Vector2f(-1024, -1024));
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            getContext()->levelRotTrans.translate(sf::Vector2f(1024, 1024));
            getContext()->levelRotTrans.rotate(-2);
            getContext()->angle += 2;
            getContext()->levelRotTrans.translate(sf::Vector2f(-1024, -1024));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        {
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        {
        }
        return 0;
    }

    uint32_t StageMainClient::doUpdate()
    {
        GameContext* ctx = getContext();

        sf::VertexArray backgroundQuad;
        sf::Vertex v;

        sf::Vector2f c = ctx->mainView.getCenter();
        sf::Vector2f s = ctx->mainView.getSize();

        // Search for foreground that is visible
        qt::AABB searchRegion;
        searchRegion.min.x = (float)((int)(c.x - (s.x / 2) + 64));
        searchRegion.min.y = (float)((int)(c.y - (s.y / 2) + 64));
        searchRegion.max.x = (float)((int)(c.x + (s.x / 2) - 64));
        searchRegion.max.y = (float)((int)(c.y + (s.y / 2) - 64));

        sf::Uint32 tw = ctx->mctx.getMap()->tilesets.back()->tilewidth;
        sf::Uint32 th = ctx->mctx.getMap()->tilesets.back()->tileheight;

        std::vector<qt::XY> res = ctx->searchLayers.at(1)->search(searchRegion);
        ctx->visibleQuads.clear();
        for (auto p = res.begin(); p != res.end(); p++)
        {
            addQuad(ctx->visibleQuads, sf::FloatRect(p->x, p->y, tw, th), sf::IntRect(p->tx, p->ty, tw, th));
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
        states.transform = ctx->levelRotTrans;

        // Draw background
        auto i = ctx->quadLayers.front();
        ctx->window.draw(i, states);

        // Draw visible foregound
        ctx->window.draw(ctx->visibleQuads, states);

        // Finalize it
        ctx->window.display();
        return 0;
    }

    uint32_t StageMainClient::cleanup()
    {
        return 0;
    }


}