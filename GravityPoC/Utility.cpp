#include "Utility.h"
#include "quadtree.h"
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

                if (gid != -1)
                {
                    // Convert GID to x,y
                    sf::Vector2i texPos = GID2XY(gid, totalTilesetCols);

                    Tile pt;
                    pt.ti = tileIndex;
                    pt.tw = tw;
                    pt.th = th;
                    pt.x = (float)((w*tw) + (tw / 2));
                    pt.y = (float)((h*th) + (th / 2));
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

    uint32_t buildQuadLayer(QuadLayer & quadLayer, TileLayer & tileLayer)
    {
        for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
        {
            addQuad(quadLayer, sf::FloatRect(tdi->x, tdi->y, (float)tdi->tw, (float)tdi->th), sf::IntRect(tdi->tx, tdi->ty, tdi->tw, tdi->th));
        }
        return 0;
    }
    uint32_t buildQuadLayers(QuadLayers & quadLayers, TileLayers & tileLayers)
    {
        for (auto tl = tileLayers.begin(); tl != tileLayers.end(); tl++)
        {
            quadLayers.push_back(QuadLayer(0));
            buildQuadLayer(quadLayers.back(), *tl);
        }
        return 0;
    }

    uint32_t buildSearchLayer(SearchLayer & searchLayer, TileLayer & tileLayer)
    {
        for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
        {
            qt::XY pt;
            pt.ti = tdi->ti;
            pt.x = tdi->x;
            pt.y = tdi->y;
            pt.tx = tdi->tx;
            pt.ty = tdi->ty;
            searchLayer->insert(pt);
        }
        return 0;
    }

    uint32_t buildSearchLayers(SearchLayers & searchLayers, TileLayers & tileLayers)
    {
        for (auto tl = tileLayers.begin(); tl != tileLayers.end(); tl++)
        {
            SearchLayer searchLayer = std::make_shared<qt::QuadTree>();
            int maxDepth = 4;
            qt::AABB aabb;
            
            aabb.min.x = aabb.min.y = 0;
            aabb.max.x = aabb.max.y = 64*32;
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
