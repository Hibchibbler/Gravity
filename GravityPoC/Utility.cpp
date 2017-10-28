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

    sf::Uint32 addQuad(sf::VertexArray & v, sf::FloatRect c, sf::IntRect t, unsigned char flip)
    {
        const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x4;
        const unsigned FLIPPED_VERTICALLY_FLAG = 0x2;
        const unsigned FLIPPED_DIAGONALLY_FLAG = 0x1;

        // Create Quad, clockwise winding. Add tex too.
        //             
        //    1        
        //    *---* 2  
        //    |   |    
        //  4 *---*    
        //        3    
        //             
        sf::Vector2f topleft = sf::Vector2f(t.left, t.top);
        sf::Vector2f topright = sf::Vector2f(t.left + t.width, t.top);
        sf::Vector2f bottomright = sf::Vector2f(t.left + t.width, t.top + t.height);
        sf::Vector2f bottomleft = sf::Vector2f(t.left, t.top + t.height);

        bool dflip = ((flip & FLIPPED_DIAGONALLY_FLAG) > 0 ? true : false);
        bool hflip = ((flip & FLIPPED_HORIZONTALLY_FLAG) > 0 ? true : false);
        bool vflip = ((flip & FLIPPED_VERTICALLY_FLAG) > 0 ? true : false);

        if (dflip)
        {
            sf::Vector2f temp = bottomleft;
            bottomleft = topright;
            topright = temp;
        }

        if (vflip)
        {
            sf::Vector2f temp = topleft;
            topleft = bottomleft;
            bottomleft = temp;

            temp = topright;
            topright = bottomright;
            bottomright = temp;
        }

        if (hflip)
        {
            sf::Vector2f temp = topleft;
            topleft = topright;
            topright = temp;

            temp = bottomleft;
            bottomleft = bottomright;
            bottomright = temp;
        }






        v.append(sf::Vertex(sf::Vector2f(c.left, c.top), topleft));
        v.append(sf::Vertex(sf::Vector2f(c.left + c.width, c.top), topright));
        v.append(sf::Vertex(sf::Vector2f(c.left + c.width, c.top + c.height), bottomright));
        v.append(sf::Vertex(sf::Vector2f(c.left, c.top + c.height), bottomleft));

        return 0;
    }

    //sf::Uint32 addShape(SAT::Shape & s, uint32_t ti, uint32_t gid, sf::FloatRect c)
    //{
    //    // Create Quad, clockwise winding. Add tex too.
    //    //             
    //    //    1        
    //    //    *---* 2  
    //    //    |   |    
    //    //  4 *---*    
    //    //        3    
    //    //             
    //        
    //    s.ti = ti;
    //        
    //    
    //    if (gid == 131-1)
    //    {// |/
    //            
    //        s.addVertex(c.left, c.top);
    //        s.addVertex(c.left + c.width, c.top);
    //        //s.addVertex(c.left + c.width, c.top + c.height);
    //        s.addVertex(c.left, c.top + c.height);
    //    }else
    //    if (gid == 132-1)
    //    {// /|
    //    
    //        s.addVertex(c.left, c.top);
    //        s.addVertex(c.left + c.width, c.top);
    //        s.addVertex(c.left + c.width, c.top + c.height);
    //        //s.addVertex(c.left, c.top + c.height);
    //    }
    //    else
    //    if (gid == 147-1)
    //    {//
    //     
    //        s.addVertex(c.left, c.top);
    //        //s.addVertex(c.left + c.width, c.top);
    //        s.addVertex(c.left + c.width, c.top + c.height);
    //        s.addVertex(c.left, c.top + c.height);
    //    }else
    //    if (gid == 148-1)
    //    {// /|
    //    
    //        //s.addVertex(c.left, c.top);
    //        s.addVertex(c.left + c.width, c.top);
    //        s.addVertex(c.left + c.width, c.top + c.height);
    //        s.addVertex(c.left, c.top + c.height);
    //    }
    //    else
    //    if (gid == 133 - 1)
    //    {// /|
    //        s.addVertex(c.left, c.top);
    //        s.addVertex(c.left + c.width, c.top);
    //        //s.addVertex(c.left + c.width, c.top + c.height);
    //        s.addVertex(c.left, c.top + c.height/2);
    //    }
    //    else
    //    if (gid == 134 - 1)
    //    {// /|
    //        s.addVertex(c.left, c.top);
    //        s.addVertex(c.left + c.width, c.top);
    //        s.addVertex(c.left + c.width, c.top + c.height/2);
    //        //s.addVertex(c.left, c.top + c.height);
    //    }
    //    else
    //    if (gid == 149 - 1)
    //    {// /|
    //        s.addVertex(c.left, c.top + c.height/2);
    //        //s.addVertex(c.left + c.width, c.top);
    //        s.addVertex(c.left + c.width, c.top + c.height);
    //        s.addVertex(c.left, c.top + c.height);
    //    }
    //    else
    //    if (gid == 150 - 1)
    //    {// /|
    //        s.addVertex(c.left, c.top + c.height);
    //     //s.addVertex(c.left, c.top);
    //        s.addVertex(c.left + c.width, c.top + c.height/2);
    //        s.addVertex(c.left + c.width, c.top + c.height);
    //        
    //    }
    //    else
    //    {
    //        s.addVertex(c.left, c.top);
    //        s.addVertex(c.left + c.width, c.top);
    //        s.addVertex(c.left + c.width, c.top + c.height);
    //        s.addVertex(c.left, c.top + c.height);
    //    }
    //    return 0;
    //}

    sf::Uint32 addRotShape(SAT::Shape & s, sf::FloatRect p, float angle)
    {
        float px1, py1;
        float px2, py2;
        float px3, py3;
        float px4, py4;

        angle = angle*(PI / 180.0f);

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

        angle = angle*(PI / 180.0f);

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
                // Mask out the top 3 bits...
                
                // When gid == 0, the map data did not associated a texture with this map location.
                // We translate to array indexing by subtracting 1 from the gid.

                Tile pt;
                if (layer->data->tiles[tileIndex].gid != 0)
                {
                    uint32_t gid = (layer->data->tiles[tileIndex].gid); //-1;
                    //unsigned char* g0 = (unsigned char*)(&gid);
                    //unsigned char* g1 = (unsigned char*)(&gid)+1;
                    //unsigned char* g2 = (unsigned char*)(&gid)+2;
                    //unsigned char* g3 = (unsigned char*)(&gid)+3;
                    //gid = *g0 | *g1<<8 | *g2<<16 | *g3<<24;

                    // Convert GID to x,y
                    pt.flip = (gid & 0xE0000000) >> 29;
                    gid = gid & 0x1FFFFFFF;
                    sf::Vector2i texPos = GID2XY(gid - tileset->firstgid, totalTilesetCols);
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
        //const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x4;
        //const unsigned FLIPPED_VERTICALLY_FLAG = 0x2;
        //const unsigned FLIPPED_DIAGONALLY_FLAG = 0x1;

        for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
        {
            addQuad(quadLayer,
                    sf::FloatRect(tdi->x, tdi->y,
                    (float)tileWidth, (float)tileHeight),
                    sf::IntRect(tdi->tx, tdi->ty,
                    tileWidth, tileHeight),
                    tdi->flip);
                    /*((tdi->flip & FLIPPED_DIAGONALLY_FLAG) > 0 ? true : false),
                    ((tdi->flip & FLIPPED_HORIZONTALLY_FLAG) > 0 ? true : false),
                    ((tdi->flip & FLIPPED_VERTICALLY_FLAG) > 0 ? true : false));*/
        }
        return 0;
    }

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

    bool buildPolygon(TMX::Object::Ptr obj, CONVEXSHAPE & s, bool applyOffset = false)
    {
        bool status = false;
        if (obj->polygon != nullptr)
        {
            std::vector<std::string> pairs = split(obj->polygon->points, ' ');

            s.setPointCount(pairs.size());

            int i = 0;
            for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
            {
                std::vector<std::string> comp = split(*pair, ',');
                float x1, y1;
                x1 = atol(comp[0].c_str()) + (applyOffset ? obj->x : 0);
                y1 = atol(comp[1].c_str()) + (applyOffset ? obj->y : 0);
                s.setPoint(i, sf::Vector2f(x1, y1));
                ++i;
            }
            status = true;
        }
        return status;
    }

    bool buildPolyline(TMX::Object::Ptr obj, CONVEXSHAPE & s, bool applyOffset = false)
    {
        bool status = false;
        if (obj->polyline != nullptr)
        {
            std::vector<std::string> pairs = split(obj->polyline->points, ' ');

            size_t max = pairs.size() -1;
            if (max == 1)
                max = 2;

            s.setPointCount(max);
            s.offsetX = obj->x;
            s.offsetY = obj->y;

            if (obj->rotation == 0)
            {
                int i = 0;
                for (auto p = pairs.begin();p != pairs.end();p++)//int i = 0; i < max; ++i)
                {
                    if (i == max)
                        break;
                    std::vector<std::string> comp = split((*p), ',');
                    float x1, y1;
                    x1 = atol(comp[0].c_str()) + (applyOffset ? obj->x : 0);
                    y1 = atol(comp[1].c_str()) + (applyOffset ? obj->y : 0);
                    s.setPoint(i, sf::Vector2f(x1, y1));
                    i++;
                }
            }
            else
            {
                int i = 0;
                for (auto p = pairs.rbegin(); p != pairs.rend(); p++)
                {
                    if (i == max)
                        break;
                    std::vector<std::string> comp = split(*p, ',');
                    float x1, y1;
                    x1 = atol(comp[0].c_str());// +(applyOffset ? obj->x : 0);
                    y1 = atol(comp[1].c_str());// +(applyOffset ? obj->y : 0);

                    //////////////////////////////////////////////////////////
                    //////////////////////////////////////////////////////////
                    
                    for (int u = 0; u < obj->rotation; u += 90)
                    {
                    float temp = x1;
                    x1 = y1 * -1.0;
                    y1 = temp * 1.0;
                    }
                    x1 += (applyOffset ? obj->x : 0);
                    y1 += (applyOffset ? obj->y : 0);
                    s.setPoint(i, sf::Vector2f(x1, y1));
                    
                    //////////////////////////////////////////////////////////
                    //float px1, py1;

                    //float angle = obj->rotation*(PI / 180.0f);

                    //px1 = ((x1 - 0 / 2.0f) * cos(angle) - (x1 - 0 / 2.0f) * sin(angle));
                    //py1 = ((y1 - 0 / 2.0f) * sin(angle) + (y1 -0 / 2.0f) * cos(angle));

                    //px1 += (applyOffset ? obj->x : 0);
                    //py1 += (applyOffset ? obj->y : 0);
                    //s.setPoint(i, sf::Vector2f(px1, py1));
                    
                    i++;

                }
            }

            status = true;
        }
        return status;
    }

    bool buildRectangle(TMX::Object::Ptr obj, CONVEXSHAPE & s)
    {
        bool status = false;

        s.setPointCount(4);
        s.setPoint(0, sf::Vector2f(obj->x, obj->y));
        s.setPoint(1, sf::Vector2f(obj->x + obj->width, obj->y));
        s.setPoint(2, sf::Vector2f(obj->x + obj->width, obj->y + obj->height));
        s.setPoint(3, sf::Vector2f(obj->x, obj->y + obj->height));

        return status;
    }

    uint32_t buildPlayerObjectLayers(std::vector<CONVEXSHAPE> & polygons, TMX::Objectgroup::Vec & objectGroups)//std::string strPoints, int x, int y)
    {
        for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
        {
            if ((*objG)->name != "PlayerShapes")
                continue;
            for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
            {

                if ((*obj)->polygon != nullptr)
                {
                    polygons.push_back(CONVEXSHAPE());
                    buildPolygon(*obj, polygons.back(), false);
                }
                else if ((*obj)->polyline != nullptr)
                {//NOTE: discard last point, engine assume last point is same as first.
                 // TMX format is explicit about first and last point. even though they will always be the same.
                    //std::vector<std::string> pairs = split((*obj)->polyline->points, ' ');
                    polygons.push_back(CONVEXSHAPE());
                    buildPolyline(*obj, polygons.back(), false);
                }
                else if ((*obj)->ellipse != nullptr)
                {


                }
                else
                {//a rectangle
                    polygons.push_back(CONVEXSHAPE());
                    buildRectangle(*obj, polygons.back());
                }

            }
        }

        return 0;
    }

    bool isSharedEdge(SAT::ContactInfo & ci, std::vector<SAT::Segment> & sharedEdges)
    {
        for (auto se = sharedEdges.begin(); se != sharedEdges.end(); se++)
        {
            if (ci.edge.isEqual(*se))
            {
                return true;
            }
        }

        return false;
    }

    uint32_t buildSharedEdgesLayers(std::vector<SAT::Segment> & sharedEdges, TMX::Objectgroup::Vec & objectGroups)
    {
        for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
        {
            if ((*objG)->name != "SharedEdges")
                continue;

            std::vector<CONVEXSHAPE> polygons;
            for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
            {
                if ((*obj)->polyline != nullptr)
                {//NOTE: discard last point, engine assume last point is same as first.
                 // TMX format is explicit about first and last point. even though they will always be the same.
                    polygons.push_back(CONVEXSHAPE());
                    buildPolyline(*obj, polygons.back(), true);


                    //// Assume one line segment, so 2 points.
                    size_t pc = polygons.back().getPointCount();

                    if (pc >= 2)
                    {
                        sf::Vector2f p1 = polygons.back().getPoint(0);
                        sf::Vector2f p2 = polygons.back().getPoint((0 + 1) % pc);

                        //sf::Vector2f _edge = p2 - p1;
                        //vec::VECTOR2 edge(_edge.x, _edge.y);
                        //edge += vec::VECTOR2((*obj)->x, (*obj)->y);
                        SAT::Segment seg;
                        seg.start = vec::VECTOR2(p1.x, p1.y);
                        seg.end = vec::VECTOR2(p2.x, p2.y);
                        sharedEdges.push_back(seg);
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
            if ((*objG)->name != "CollisionPolygons")
                continue;

            for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
            {
                if ((*obj)->polyline != nullptr)
                {//NOTE: discard last point, engine assume last point is same as first.
                 // TMX format is explicit about first and last point. even though they will always be the same.
                    polygons.push_back(CONVEXSHAPE());
                    buildPolyline(*obj, polygons.back(), true);

                }
            }
        }
        return 0;
    }

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


    //
    // This creates an array of layers. where each layer is 
    // composed of quads, where the size of each quad is 
    // equal to the size of the tiles in the last tileset.
    // Assumed that tile sizes are uniform in a single tileset.

}
