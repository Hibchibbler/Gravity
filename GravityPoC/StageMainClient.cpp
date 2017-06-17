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
        TMX::TMXReader::load("level6.tmx", ctx->mctx);

        // Still playing with these...no clue yet
        TMX::Objectgroup::Ptr pObjG = ctx->mctx.getObjectGroup(ctx->mctx.maps.back(), "PlayerShape");
        

        ctx->player.position.x = pObjG->objects.back()->x;
        ctx->player.position.y = pObjG->objects.back()->y;

        ctx->player.velocity = vec::VECTOR2(0.0, 0.0);
        ctx->player.acceleration = vec::VECTOR2(0.0, 0.0);

        ctx->size.x = ctx->size.y = 1000;
        ctx->mainView.setCenter(ctx->player.position.x, ctx->player.position.y);
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
        int maxDepth = 10;
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

    bool myfunction(physics::Intersection i, physics::Intersection j) { return (i.angle  > j.angle); }

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
        
        qt::AABB searchRegion = getSearchRegion(ctx->mainView, 1.0);

        TMX::Tileset::Ptr tilesetA = getTileset("tilesetA", ctx->mctx.maps.back()->tilesets);
        TMX::Tileset::Ptr tilesetBkgnd = getTileset("background_01", ctx->mctx.maps.back()->tilesets);


        sf::Uint32 tw = tilesetA->tilewidth;
        sf::Uint32 th = tilesetA->tileheight;

        //Does the player hit anything?
        // convert All obstacle tiles into shapes.
        std::vector<SAT::Shape> shapes;
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

        // Convert Polygons
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

        vec::VECTOR2 pos(floor(ctx->player.position.x), floor(ctx->player.position.y));
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
        physics::update(ctx->player, elapsed);

        std::vector<SAT::Segment> segments;
        physics::ResolveCollisions(segments, shapes, playershapes, ctx->player, ctx->tileLayers[1], ctx, false);

        // We will shoot a ray for each degree of rotation around position.
        segments.clear();
        for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
        {
            std::vector<SAT::Segment> s = shape->getSegments();
            segments.insert(segments.end(), s.begin(), s.end());
        }
        {
            std::vector<physics::Intersection> intersections;
            for (float a = 0; a < 370; a = a + 5.0)
            {
                //if (a == 0)
                //    continue;
                //if (a == 180.0)
                //    continue;

                // A Ray will intersect with an obstacle or the maximum distance (that we cast)
                // hence, we always will have an intersection..but 
                // whether it's relevent or not is in the eye of the beholder.

                ////////////////////////////////////////////////////////////////////////////////////
                //RayCast(intersect, ctx->player.position, segments, a);                
                float radians = a * (3.14159f / 180.0f);
                vec::VECTOR2 rayDir(ctx->player.position.mag() * cos(radians),
                                    ctx->player.position.mag() * sin(radians));
                rayDir = rayDir.norm() /5.0;// This is important
                vec::VECTOR2 rayStart = ctx->player.position;
                ///PHYSPRINT("[Ang](" << angle << ")");

                

                bool found = false;
                float closest = 999999.9;
                physics::Intersection intersectMin;
                for (auto seg = segments.begin(); seg != segments.end(); ++seg)
                {
                   
                    physics::Intersection intersectTemp;
                    intersectTemp.castedPoint = rayStart + rayDir * 900;
                    intersectTemp.distance = intersectTemp.castedPoint;
                    getIntersection(rayStart, rayDir, (*seg), intersectTemp);
                    if (intersectTemp.distance.mag() <= closest)
                    {
                        closest = intersectTemp.distance.mag();
                        intersectMin = intersectTemp;
                        found = true;
                    }
                }
                
                intersections.push_back(intersectMin);

                ////////////////////////////////////////////////////////////////////////////////////
                
            }
            std::sort(intersections.begin(), intersections.end(), myfunction);
            ctx->lineSegments.clear();
            ctx->lineSegments = sf::VertexArray(sf::PrimitiveType::TrianglesFan);//Lines);
            vec::VECTOR2 positionFixed = ctx->player.position;
            //positionFixed.x += 23;
            //positionFixed.y -= 10;
            ctx->lineSegments.append(positionFixed.sf());
            for (auto i = intersections.begin(); i != intersections.end(); ++i)
            {
                //vec::VECTOR2 newLine = i->actualPoint - ctx->player.position;
                //ctx->lineSegments.append(positionFixed.sf());
                ctx->lineSegments.append(i->castedPoint.sf());

            }
        }
        //if (ctx->lineSegments.getVertexCount() > 200)
        //    ctx->lineSegments.clear();
        //
        //int v = 0;
        //for (auto seg = segments.begin(); seg != segments.end(); ++seg)
        //{
        //    ctx->lineSegments.setPrimitiveType(sf::PrimitiveType::Quads);
        //    vec::VECTOR2 midpoint((seg->end.x + seg->start.x)/2.0, (seg->end.y + seg->start.y) / 2.0);
        //    
        //    sf::Vector2f p1(midpoint.x, midpoint.y);
        //    sf::Vector2f p2(midpoint.x + 10, midpoint.y);
        //    sf::Vector2f p3(midpoint.x + 10, midpoint.y+10);
        //    sf::Vector2f p4(midpoint.x , midpoint.y + 10);

        //    ctx->lineSegments.append(p1);
        //    ctx->lineSegments.append(p2);
        //    ctx->lineSegments.append(p3);
        //    ctx->lineSegments.append(p4);
        //    v++;
        //}


        ctx->mainView.setCenter(zxc.x, zxc.y);// ctx->player.position.x, ctx->player.position.y);
        getContext()->mainView.setRotation(getContext()->player.angle);
        return 0;
    }

    uint32_t StageMainClient::doDraw()
    {
        GameContext* ctx = getContext();
        // Draw background
        ctx->pRenderTexture0->clear();
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

        // Hit Edges
        ctx->pRenderTexture0->draw(ctx->lineSegments);

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

        
        ctx->pRenderTexture0->display();

        sf::RenderStates totalRenderStates;
        totalRenderStates.shader = &ctx->shader;
        //totalRenderStates.texture = (sf::Texture*)ctx->pRenderTexture0;
        ctx->shader.setUniform("view", sf::Vector2f(ctx->renderTextures[0].getSize().x, ctx->renderTextures[0].getSize().y));
        ctx->shader.setUniform("texture", sf::Shader::CurrentTexture);
        vec::VECTOR2 pposTemp = ctx->player.position;
        pposTemp.x = pposTemp.x + 20;
        pposTemp.y = pposTemp.y - 13;
        ctx->shader.setUniform("position", sf::Glsl::Vec2(ctx->window.mapCoordsToPixel(pposTemp.sf())));
        ctx->shader.setUniform("resolution", sf::Glsl::Vec2(2048, 2048));


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