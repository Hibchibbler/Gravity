///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "Utility.h"
#include "QuadTree\quadtree.h"
#include "GameContext.h"

namespace bali
{
    // Convert an index to x,y
    //  assuming an array contains a 2d spatial representation,
    //  this will convert the index into that array to the x and y 
    //  position of the element.
    sf::Vector2i GID2XY(int gid, int total_columns)
    {
        sf::Vector2i ret;
        ret.y = gid / total_columns;
        ret.x = gid % total_columns;
        return ret;
    }

    sf::Uint32 addQuad(sf::VertexArray & v, sf::FloatRect c, sf::IntRect t)
    {
        // Create Quad, clockwise winding. Add tex too.
        //             
        //    1        
        //    *---* 2  
        //    |   |    
        //  4 *---*    
        //        3    
        //             
        v.append(sf::Vertex(sf::Vector2f(c.left, c.top), sf::Vector2f(t.left, t.top)));
        v.append(sf::Vertex(sf::Vector2f(c.left + c.width, c.top), sf::Vector2f(t.left + t.width, t.top)));
        v.append(sf::Vertex(sf::Vector2f(c.left + c.width, c.top + c.height), sf::Vector2f(t.left + t.width, t.top + t.height)));
        v.append(sf::Vertex(sf::Vector2f(c.left, c.top + c.height), sf::Vector2f(t.left, t.top + t.height)));
        return 0;
    }

    sf::Uint32 addShape(SAT::Shape & s, uint32_t ti, uint32_t gid, sf::FloatRect c)
    {
        // Create Quad, clockwise winding. Add tex too.
        //             
        //    1        
        //    *---* 2  
        //    |   |    
        //  4 *---*    
        //        3    
        //             
        s.ti = ti;

        
        if (gid == 131-1)
        {// |/

            s.addVertex(c.left, c.top);
            s.addVertex(c.left + c.width, c.top);
            //s.addVertex(c.left + c.width, c.top + c.height);
            s.addVertex(c.left, c.top + c.height);
        }else
        if (gid == 132-1)
        {// /|
            
            s.addVertex(c.left, c.top);
            s.addVertex(c.left + c.width, c.top);
            s.addVertex(c.left + c.width, c.top + c.height);
            //s.addVertex(c.left, c.top + c.height);
        }
        else
        if (gid == 147-1)
        {//
                     
            s.addVertex(c.left, c.top);
            //s.addVertex(c.left + c.width, c.top);
            s.addVertex(c.left + c.width, c.top + c.height);
            s.addVertex(c.left, c.top + c.height);
        }else
        if (gid == 148-1)
        {// /|

            //s.addVertex(c.left, c.top);
            s.addVertex(c.left + c.width, c.top);
            s.addVertex(c.left + c.width, c.top + c.height);
            s.addVertex(c.left, c.top + c.height);
        }
        else
        if (gid == 133 - 1)
        {// /|

            s.addVertex(c.left, c.top);
            s.addVertex(c.left + c.width, c.top);
            //s.addVertex(c.left + c.width, c.top + c.height);
            s.addVertex(c.left, c.top + c.height/2);
        }
        else
        if (gid == 134 - 1)
        {// /|
            s.addVertex(c.left, c.top);
            s.addVertex(c.left + c.width, c.top);
            s.addVertex(c.left + c.width, c.top + c.height/2);
            //s.addVertex(c.left, c.top + c.height);
        }
        else
        if (gid == 149 - 1)
        {// /|

            s.addVertex(c.left, c.top + c.height/2);
            //s.addVertex(c.left + c.width, c.top);
            s.addVertex(c.left + c.width, c.top + c.height);
            s.addVertex(c.left, c.top + c.height);
        }
        else
        if (gid == 150 - 1)
        {// /|

            s.addVertex(c.left, c.top + c.height);
         //s.addVertex(c.left, c.top);
            s.addVertex(c.left + c.width, c.top + c.height/2);
            s.addVertex(c.left + c.width, c.top + c.height);
            
        }
        else
        {
            s.addVertex(c.left, c.top);
            s.addVertex(c.left + c.width, c.top);
            s.addVertex(c.left + c.width, c.top + c.height);
            s.addVertex(c.left, c.top + c.height);
        }
        return 0;
    }

    sf::Uint32 addRotShape(SAT::Shape & s, sf::FloatRect p, float angle)
    {
        float px1, py1;
        float px2, py2;
        float px3, py3;
        float px4, py4;

        angle = angle*(3.14156f / 180.0f);

        px1 = ((0 - p.width / 2.0f) * cos(angle) - (0 - p.height / 2.0f) * sin(angle)) + p.left;// +p.width/2.0f;
        py1 = ((0 - p.width / 2.0f) * sin(angle) + (0 - p.height / 2.0f) * cos(angle)) + p.top;//  +p.height/2.0f;

        px2 = ((p.width - p.width / 2.0f) * cos(angle) - (0 - p.height / 2.0f) * sin(angle)) + p.left;//+p.width/2.0f;
        py2 = ((p.width - p.width / 2.0f) * sin(angle) + (0 - p.height / 2.0f) * cos(angle)) + p.top;// +p.height/2.0f;

        px3 = ((p.width - p.width / 2.0f) * cos(angle) - (p.height - p.height / 2.0f) * sin(angle)) + p.left;//+p.width/2.0f;
        py3 = ((p.width - p.width / 2.0f) * sin(angle) + (p.height - p.height / 2.0f) * cos(angle)) + p.top;// +p.height/2.0f;

        px4 = ((0 - p.width / 2.0f) * cos(angle) - (p.height - p.height / 2.0f) * sin(angle)) + p.left;//+p.width/2.0f;
        py4 = ((0 - p.width / 2.0f) * sin(angle) + (p.height - p.height / 2.0f) * cos(angle)) + p.top;// +p.height/2.0f;

        s.addVertex(px1, py1);
        s.addVertex(px2, py2);
        s.addVertex(px3, py3);
        s.addVertex(px4, py4);

        return 0;
    }

    sf::Uint32 addRotQuad(sf::VertexArray & v, sf::FloatRect p, sf::IntRect t, float angle)
    {

        float px1, py1;
        float px2, py2;
        float px3, py3;
        float px4, py4;

        angle = angle*(3.14156f / 180.0f);

        px1 = ((0 - p.width / 2.0f) * cos(angle) - (0 - p.height / 2.0f) * sin(angle)) + p.left;// +p.width/2.0f;
        py1 = ((0 - p.width / 2.0f) * sin(angle) + (0 - p.height / 2.0f) * cos(angle)) + p.top;//  +p.height/2.0f;

        px2 = ((p.width - p.width / 2.0f) * cos(angle) - (0 - p.height / 2.0f) * sin(angle)) + p.left;//+p.width/2.0f;
        py2 = ((p.width - p.width / 2.0f) * sin(angle) + (0 - p.height / 2.0f) * cos(angle)) + p.top;// +p.height/2.0f;

        px3 = ((p.width - p.width / 2.0f) * cos(angle) - (p.height - p.height / 2.0f) * sin(angle)) + p.left;//+p.width/2.0f;
        py3 = ((p.width - p.width / 2.0f) * sin(angle) + (p.height - p.height / 2.0f) * cos(angle)) + p.top;// +p.height/2.0f;

        px4 = ((0 - p.width / 2.0f) * cos(angle) - (p.height - p.height / 2.0f) * sin(angle)) + p.left;//+p.width/2.0f;
        py4 = ((0 - p.width / 2.0f) * sin(angle) + (p.height - p.height / 2.0f) * cos(angle)) + p.top;// +p.height/2.0f;


        v.append(sf::Vertex(sf::Vector2f(px1, py1),
            sf::Vector2f(t.left, t.top)
        ));

        v.append(sf::Vertex(sf::Vector2f(px2, py2),
            sf::Vector2f(t.left + t.width, t.top)
        ));

        v.append(sf::Vertex(sf::Vector2f(px3, py3),
            sf::Vector2f(t.left + t.width, t.top + t.height)
        ));

        v.append(sf::Vertex(sf::Vector2f(px4, py4),
            sf::Vector2f(t.left, t.top + t.height)
        ));

        return 0;
    }

    qt::AABB getSearchRegion(const sf::View & view, float zoom)
    {
        qt::AABB searchRegion;
        sf::Vector2f c = view.getCenter();
        c.x = floor(c.x);
        c.y = floor(c.y);

        sf::Vector2f s = view.getSize();
        s.x *= zoom;
        s.y *= zoom;

        searchRegion.min.x = (float)((int)(c.x - (s.x / 2) + 64));
        searchRegion.min.y = (float)((int)(c.y - (s.y / 2) + 64));
        searchRegion.max.x = (float)((int)(c.x + (s.x / 2) - 64));
        searchRegion.max.y = (float)((int)(c.y + (s.y / 2) - 64));
        return searchRegion;
    }

    uint32_t buildTileLayer(TileLayer & tileLayer, const TMX::Tileset::Ptr tileset, const TMX::Layer::Ptr layer)
    {
        int tw = tileset->tilewidth;
        int th = tileset->tileheight;

        int totalTilesetCols = tileset->columns;
        int totalLayerCols = layer->width;
        int totalLayerRows = layer->height;
        
        for (int h = 0; h < layer->width; h++)
        {
            for (int w = 0; w < layer->height; w++)
            {
                uint32_t tileIndex = (totalLayerCols * h) + w;
                uint32_t gid = layer->data->tiles[tileIndex].gid - 1;
                // When gid == 0, the map data did not associated a texture with this map location.
                // We translate to array indexing by subtracting 1 from the gid.


                // Mark if neighboring cells are tiled, or not.
                uint32_t nti, eti, sti, wti;
                Tile pt;
                if (h > 0 )
                {
                    nti = (totalLayerCols * (h - 1)) + (w);
                    nti = layer->data->tiles[nti].gid;
                    if (nti == 0)
                    {
                        pt.n = false;
                    }
                    else
                    {
                        pt.n = true;
                    }
                }
                else
                {
                    //Since there is nothing to the north, mark it as a wall.
                    pt.n = true;
                }
                
                if (h <  totalLayerRows-1)
                {
                    sti = (totalLayerCols * (h + 1)) + (w);
                    sti = layer->data->tiles[sti].gid;
                    if (sti == 0)
                    {
                        pt.s = false;
                    }
                    else
                    {
                        pt.s = true;
                    }
                }
                else
                {
                    pt.s = true;
                }
                
                if (w > 0)
                {
                    wti = (totalLayerCols * (h)) + (w - 1);
                    wti = layer->data->tiles[wti].gid;
                    if (wti == 0)
                    {
                        pt.w = false;
                    }
                    else
                    {
                        pt.w = true;
                    }
                }
                else
                {
                    pt.w = true;
                }

                if (w <  totalLayerCols - 1)
                {
                    eti = (totalLayerCols * (h)) + (w + 1);
                    eti = layer->data->tiles[eti].gid;
                    if (eti == 0)
                    {
                        pt.e = false;
                    }
                    else
                    {
                        pt.e = true;
                    }
                }
                else
                {
                    pt.e = true;
                }


                if (gid != -1)
                {
                    // Convert GID to x,y
                    sf::Vector2i texPos = GID2XY(gid - tileset->firstgid+1, totalTilesetCols);

                    
                    
                    pt.gid = gid;
                    pt.ti = tileIndex;
                    //pt.tw = tw;
                    //pt.th = th;
                    pt.x = (float)((w*tw));// +(tw / 2);
                    pt.y = (float)((h*th));// +(th / 2);
                    pt.tx = texPos.x*tw;
                    pt.ty = texPos.y*th;

                    //addQuad(valayer, sf::FloatRect((float)lay.x, (float)lay.y, (float)tw, (float)th), sf::IntRect(pos.x, pos.y, tw, th));
                    tileLayer.push_back(pt);
                }
            }
        }
        return 0;
    }
    uint32_t buildTileLayers(TileLayers & tileLayers, const TMX::Tileset::Ptr tileset, const TMX::Layer::Vec layers)
    {
        for (auto l = layers.begin(); l != layers.end(); l++)
        {
            tileLayers.push_back(TileLayer());
            buildTileLayer(tileLayers.back(), tileset, *l);
        }
        return 0;
    }

    uint32_t buildQuadLayer(QuadLayer & quadLayer, TileLayer & tileLayer, uint32_t tileWidth, uint32_t tileHeight)
    {
        for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
        {
            addQuad(quadLayer, sf::FloatRect(tdi->x, tdi->y, (float)tileWidth, (float)tileHeight), sf::IntRect(tdi->tx, tdi->ty, tileWidth, tileHeight));
        }
        return 0;
    }
    //uint32_t buildQuadLayers(QuadLayers & quadLayers, TileLayers & tileLayers, uint32_t tileWidth, uint32_t tileHeight)
    //{
    //    for (auto tl = tileLayers.begin(); tl != tileLayers.end(); tl++)
    //    {
    //        quadLayers.push_back(QuadLayer(0));
    //        buildQuadLayer(quadLayers.back(), *tl, tileWidth, tileHeight);
    //    }
    //    return 0;
    //}
    template<typename Out>
    void split(const std::string &s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    uint32_t buildPlayerObjectLayers(std::vector<CONVEXSHAPE> & polygons, TMX::Objectgroup::Vec & objectGroups)//std::string strPoints, int x, int y)
    {
        for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
        {
            if ((*objG)->name != "PlayerShape")
                continue;
            for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
            {

                if ((*obj)->polygon != nullptr)
                {
                    std::vector<std::string> pairs = split((*obj)->polygon->points, ' ');

                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(pairs.size());
                    //

                    int i = 0;
                    for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
                    {
                        std::vector<std::string> comp = split(*pair, ',');
                        float x1, y1;
                        x1 = atol(comp[0].c_str());// +(*obj)->x;
                        y1 = atol(comp[1].c_str());// +(*obj)->y;
                        polygons.back().setPoint(i, sf::Vector2f(x1, y1));
                        ++i;
                    }
                }
                else if ((*obj)->polyline != nullptr)
                {//NOTE: discard last point, engine assume last point is same as first.
                 // TMX format is explicit about first and last point. even though they will always be the same.
                    std::vector<std::string> pairs = split((*obj)->polyline->points, ' ');

                    size_t max = pairs.size() - 1;/* - 1;
                                                  if (pairs.size() == 4)
                                                  max = pairs.size();*/

                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(max);
                    //
                    //for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
                    for (int i = 0; i < max; ++i)
                    {
                        std::vector<std::string> comp = split(pairs[i], ',');
                        float x1, y1;
                        x1 = atol(comp[0].c_str());// +(*obj)->x;
                        y1 = atol(comp[1].c_str());// +(*obj)->y;
                        polygons.back().setPoint(i, sf::Vector2f(x1/0.75f, y1/0.75f));
                    }
                }
                else if ((*obj)->ellipse != nullptr)
                {


                }
                else
                {//a rectangle
                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(4);
                    polygons.back().setPoint(0, sf::Vector2f((*obj)->x, (*obj)->y));
                    polygons.back().setPoint(1, sf::Vector2f((*obj)->x + (*obj)->width, (*obj)->y));
                    polygons.back().setPoint(2, sf::Vector2f((*obj)->x + (*obj)->width, (*obj)->y + (*obj)->height));
                    polygons.back().setPoint(3, sf::Vector2f((*obj)->x, (*obj)->y + (*obj)->height));
                    //polygons.back().setPoint(3, (*obj)->x);
                }

            }
        }
        //polygons.back().setPoint(0,)
        //




        return 0;
    }
    uint32_t buildSharedEdgesLayers(std::vector<vec::VECTOR2> & sharedEdges, TMX::Objectgroup::Vec & objectGroups)
    {
        for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
        {
            if ((*objG)->name != "Shared Edges")
                continue;

            std::vector<CONVEXSHAPE> polygons;
            for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
            {
                if ((*obj)->polyline != nullptr)
                {//NOTE: discard last point, engine assume last point is same as first.
                 // TMX format is explicit about first and last point. even though they will always be the same.
                    std::vector<std::string> pairs = split((*obj)->polyline->points, ' ');

                    size_t max = pairs.size();/* - 1;
                                                  if (pairs.size() == 4)
                                                  max = pairs.size();*/

                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(max);
                    
                    //
                    //for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
                    for (int i = 0; i < max; ++i)
                    {
                        std::vector<std::string> comp = split(pairs[i], ',');
                        float x1, y1;
                        x1 = atol(comp[0].c_str()) + (*obj)->x;
                        y1 = atol(comp[1].c_str()) + (*obj)->y;
                        polygons.back().setPoint(i, sf::Vector2f(x1, y1));
                        
                    }

                    //// Assume one line segment, so 2 points.
                    size_t pc = polygons.back().getPointCount();

                    if (pc >= 2)
                    {
                        sf::Vector2f p1 = polygons.back().getPoint(0);
                        sf::Vector2f p2 = polygons.back().getPoint((0 + 1) % pc);

                        sf::Vector2f _edge = p2 - p1;
                        vec::VECTOR2 edge(_edge.x, _edge.y);
                        edge += vec::VECTOR2((*obj)->x, (*obj)->y);
                        sharedEdges.push_back(edge);
                    }
                }
            }
        }
        return 0;
    }

    uint32_t buildPolygonLayers(std::vector<CONVEXSHAPE> & polygons, TMX::Objectgroup::Vec & objectGroups)
    {
        for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
        {
            if ((*objG)->name != "Object Layer 1")
                continue;

            for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
            {
                if ((*obj)->polyline != nullptr)
                {//NOTE: discard last point, engine assume last point is same as first.
                 // TMX format is explicit about first and last point. even though they will always be the same.
                    std::vector<std::string> pairs = split((*obj)->polyline->points, ' ');

                    size_t max = pairs.size() - 1;/* - 1;
                                                  if (pairs.size() == 4)
                                                  max = pairs.size();*/

                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(max);
                    polygons.back().offsetX = (*obj)->x;
                    polygons.back().offsetY = (*obj)->y;
                    //
                    //for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
                    for (int i = 0; i < max; ++i)
                    {
                        std::vector<std::string> comp = split(pairs[i], ',');
                        float x1, y1;
                        x1 = atol(comp[0].c_str()) + (*obj)->x;
                        y1 = atol(comp[1].c_str()) + (*obj)->y;
                        polygons.back().setPoint(i, sf::Vector2f(x1, y1));
                    }
                }
            }
        }
        return 0;
    }

    uint32_t buildObjectLayers(std::vector<CONVEXSHAPE> & polygons, TMX::Objectgroup::Vec & objectGroups)//std::string strPoints, int x, int y)
    {//TODO: need multuple polygons here!
        // <polygon points="162,162 456,234 1176,666 1248,846 1254,1116 -396,1362 -408,1284 -462,810 -72,210"/>        

        for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
        {
            if ((*objG)->name != "Object Layer 1")
                continue;
            for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
            {

                if ((*obj)->polygon != nullptr)
                {
                    std::vector<std::string> pairs = split((*obj)->polygon->points, ' ');

                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(pairs.size());
                    //

                    int i = 0;
                    for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
                    {
                        std::vector<std::string> comp = split(*pair, ',');
                        float x1, y1;
                        x1 = atol(comp[0].c_str()) + (*obj)->x;
                        y1 = atol(comp[1].c_str()) + (*obj)->y;
                        polygons.back().setPoint(i, sf::Vector2f(x1, y1));
                        ++i;
                    }
                }
                else if ((*obj)->polyline != nullptr)
                {//NOTE: discard last point, engine assume last point is same as first.
                    // TMX format is explicit about first and last point. even though they will always be the same.
                    std::vector<std::string> pairs = split((*obj)->polyline->points, ' ');

                    size_t max = pairs.size()-1;/* - 1;
                    if (pairs.size() == 4)
                        max = pairs.size();*/

                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(max);
                    //
                    //for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
                    for (int i = 0;i < max;++i)
                    {
                        std::vector<std::string> comp = split(pairs[i], ',');
                        float x1, y1;
                        x1 = atol(comp[0].c_str()) + (*obj)->x;
                        y1 = atol(comp[1].c_str()) + (*obj)->y;
                        polygons.back().setPoint(i, sf::Vector2f(x1, y1));
                    }
                }
                else if ((*obj)->ellipse != nullptr)
                {
           

                }
                else
                {//a rectangle
                    polygons.push_back(CONVEXSHAPE());
                    polygons.back().setPointCount(4);
                    polygons.back().setPoint(0, sf::Vector2f( (*obj)->x                 , (*obj)->y                  ) );
                    polygons.back().setPoint(1, sf::Vector2f( (*obj)->x + (*obj)->width , (*obj)->y                  ) );
                    polygons.back().setPoint(2, sf::Vector2f( (*obj)->x + (*obj)->width , (*obj)->y + (*obj)->height ) );
                    polygons.back().setPoint(3, sf::Vector2f( (*obj)->x                 , (*obj)->y + (*obj)->height ) );
                    //polygons.back().setPoint(3, (*obj)->x);
                }

            }
        }
        //polygons.back().setPoint(0,)
        //


        
        
        return 0;
    }

    //uint32_t buildSearchLayer(SearchLayer & searchLayer, TileLayer & tileLayer)
    //{
    //    for (int tdi = 0; tdi < tileLayer.size(); tdi++)
    //    //for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
    //    {
    //        qt::XY pt;
    //        pt.ti = tdi;
    //        pt.x = tileLayer[tdi].x;
    //        pt.y = tileLayer[tdi].y;
    //        searchLayer->insert(pt);
    //    }
    //    return 0;
    //}

    uint32_t buildSearchLayer(SearchLayer & searchLayer, std::vector<CONVEXSHAPE> polygons)
    {
        for (int tdi = 0; tdi < polygons.size(); tdi++)
        {
            qt::XY pt;
            pt.ti = tdi;

            sf::FloatRect gb = polygons[tdi].getGlobalBounds();
            pt.x = gb.left + gb.width / 2.0;
            pt.y = gb.top + gb.height / 2.0;
            searchLayer->insert(pt);
        }
        return 0;
    }

    //uint32_t buildSearchLayers(SearchLayers & searchLayers, TileLayers & tileLayers)
    //{
    //    for (auto tl = tileLayers.begin(); tl != tileLayers.end(); tl++)
    //    {
    //        SearchLayer searchLayer = std::make_shared<qt::QuadTree>();
    //        int maxDepth = 10;
    //        qt::AABB aabb;
    //        
    //        aabb.min.x = aabb.min.y = 0;
    //        aabb.max.x = aabb.max.y = 64*32;//in pixels
    //        searchLayer->initialize(aabb, maxDepth);
    //        searchLayers.push_back(searchLayer);

    //        buildSearchLayer(searchLayers.back(), *tl);
    //    }
    //    return 0;
    //}

    TMX::Tileset::Ptr getTileset(std::string name, TMX::Tileset::Vec & tilesets)
    {
        for (auto i = tilesets.begin(); i != tilesets.end(); i++)
        {
            if ((*i)->name == name)
            {
                return *i;
            }
        }
        return nullptr;
    }
    //
    // This creates an array of layers. where each layer is 
    // composed of quads, where the size of each quad is 
    // equal to the size of the tiles in the last tileset.
    // Assumed that tile sizes are uniform in a single tileset.

}
