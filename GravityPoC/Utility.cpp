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

    qt::AABB getSearchRegion(const sf::View & view)
    {
        qt::AABB searchRegion;
        sf::Vector2f c = view.getCenter();
        sf::Vector2f s = view.getSize();

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
                if (gid != -1)
                {
                    // Convert GID to x,y
                    sf::Vector2i texPos = GID2XY(gid, totalTilesetCols);

                    Tile pt;
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

    //uint32_t buildQuadLayer(QuadLayer & quadLayer, TileLayer & tileLayer, uint32_t tileWidth, uint32_t tileHeight)
    //{
    //    for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
    //    {
    //        addQuad(quadLayer, sf::FloatRect(tdi->x, tdi->y, (float)tdi->tw, (float)tdi->th), sf::IntRect(tdi->tx, tdi->ty, tdi->tw, tdi->th));
    //    }
    //    return 0;
    //}
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
    uint32_t buildObjectLayers(std::vector<ConvexShape> & polygons, std::string strPoints, int x, int y)
    {//TODO: need multuple polygons here!
        // <polygon points="162,162 456,234 1176,666 1248,846 1254,1116 -396,1362 -408,1284 -462,810 -72,210"/>        
        std::vector<std::string> pairs = split(strPoints, ' ');
       
        polygons.push_back(ConvexShape());
        polygons.back().setPointCount(pairs.size());
        //

        int i = 0;
        for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
        {
            std::vector<std::string> comp = split(*pair, ',');
            float x1, y1;
            x1 = atol(comp[0].c_str())+x;
            y1 = atol(comp[1].c_str())+y;
            polygons.back().setPoint(i, sf::Vector2f(x1,y1));
            ++i;
        }

        //polygons.back().setPoint(0,)
        //


        
        
        return 0;
    }

    uint32_t buildSearchLayer(SearchLayer & searchLayer, TileLayer & tileLayer)
    {
        for (int tdi = 0; tdi < tileLayer.size(); tdi++)
        //for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
        {
            qt::XY pt;
            pt.ti = tdi;
            pt.x = tileLayer[tdi].x;
            pt.y = tileLayer[tdi].y;
            searchLayer->insert(pt);
        }
        return 0;
    }

    uint32_t buildSearchLayers(SearchLayers & searchLayers, TileLayers & tileLayers)
    {
        for (auto tl = tileLayers.begin(); tl != tileLayers.end(); tl++)
        {
            SearchLayer searchLayer = std::make_shared<qt::QuadTree>();
            int maxDepth = 10;
            qt::AABB aabb;
            
            aabb.min.x = aabb.min.y = 0;
            aabb.max.x = aabb.max.y = 64*32;//in pixels
            searchLayer->initialize(aabb, maxDepth);
            searchLayers.push_back(searchLayer);

            buildSearchLayer(searchLayers.back(), *tl);
        }
        return 0;
    }

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
