///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>

#include "Builders.h"
#include "Proto.h"
#include <assert.h>
namespace bali
{

void 
loadTexture(
    Texture         &t,
    TMX::Map::Ptr   map,
    std::string     tilesetname,
    sf::Color       c
)
{
    TMX::Tileset::Ptr tileset = map->getTileset(tilesetname);
    t.img.loadFromFile("assets\\" + tileset->images.back()->source);
    t.img.createMaskFromColor(c);//transparent is black...
    t.tex.loadFromImage(t.img);
}


sf::Color GetRandomColor(int k = 0)
{
    int g = rand() % 6;
    if (k != 0)
        g = k;
    switch (g)
    {
    case 0:
        return sf::Color::Red;
    case 1:
        return sf::Color::Blue;
    case 2:
        return sf::Color::White;
    case 3:
        return sf::Color::Magenta;
    case 4:
        return sf::Color::Cyan;
    case 5:
        return sf::Color::Yellow;
    }
}
Proto
getCopyOfProto(
    Vec<Proto> & protos,
    uint32_t id
)
{
    for (auto y = protos.begin(); y != protos.end(); y++)
    {
        if (y->id == id)
        {
            return *y;
        }
    }
    throw;
}

uint32_t 
loadPrototype(Proto & proto,
              TMX::Objectgroup::Ptr & objectGroup
)
{

    //
    // First load up the properties
    //
    for (auto prop = objectGroup->properties.begin(); prop != objectGroup->properties.end(); prop++)
    {
        if ((*prop)->type == "int")
        {
            if ((*prop)->name == "mass")
            {
                int mass = std::atoi((*prop)->value.c_str());
                proto.body.mass = mass;
            }
            else if ((*prop)->name == "protoid")
            {
                // The unique id for this prototype
                int pid = std::atoi((*prop)->value.c_str());
                proto.id = pid;
            }
        }
        else if ((*prop)->type == "float")
        {
            /*char* end;
            int mass = std::strtof((*prop)->value.c_str(), &end);
            entities.back().body.mass = mass;*/
        }
        else if ((*prop)->type == "bool")
        {

        }
        else if ((*prop)->type == "color")
        {

        }
        else if ((*prop)->type == "file")
        {

        }
        else// string is default
        {
            if ((*prop)->name == "prototype")
            {
                if ((*prop)->value == "player")
                {
                    proto.type = Proto::Type::PLAYER;
                }
                else if ((*prop)->value == "monster")
                {
                    proto.type = Proto::Type::MONSTER;
                }
                else if ((*prop)->value == "consumable")
                {
                    proto.type = Proto::Type::CONSUMABLE;
                }
            }
        }
    }

    //
    // Then, load up the objects (aka geometry)
    //
    for (auto obj = objectGroup->objects.begin(); obj != objectGroup->objects.end(); ++obj)
    {
        if ((*obj)->polygon != nullptr)
        {
            proto.shapes.push_back(Shape());
            buildPolygon(proto.shapes.back(), *obj);
        }
        else if ((*obj)->polyline != nullptr)
        {//NOTE: discard last point, engine assume last point is same as first.
         // TMX format is explicit about first and last point. even though they will always be the same.
            proto.shapes.push_back(Shape());
            buildPolyline(proto.shapes.back(), *obj);
        }
        else
        {//a rectangle
            proto.shapes.push_back(Shape());
            buildRectangle(proto.shapes.back(), *obj);
        }
    }

    return 0;
}
uint32_t loadPolygons(Vec<Shape> & shapes, TMX::Objectgroup::Ptr & objectGroup)
{
    for (auto obj = objectGroup->objects.begin(); obj != objectGroup->objects.end(); ++obj)
    {
        if ((*obj)->polygon != nullptr)
        {
            shapes.push_back(Shape());
            buildPolygon(shapes.back(), *obj);
        }
        else if ((*obj)->polyline != nullptr)
        {//NOTE: discard last point, engine assume last point is same as first.
         // TMX format is explicit about first and last point. even though they will always be the same.
            shapes.push_back(Shape());
            buildPolyline(shapes.back(), *obj);
        }
        else
        {//a rectangle
            shapes.push_back(Shape());
            buildRectangle(shapes.back(), *obj);
        }
    }

    return 0;
}
uint32_t loadTileLayer(Vec<Tile> & tiles, TMX::Tileset::Ptr & tileset, TMX::Layer::Ptr & layer)
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

            // When gid == 0, the map data did not associate a texture with this map location.
            // We translate to array indexing by subtracting 1 from the gid.

            Tile tile;
            if (layer->data->tiles[tileIndex].gid != 0)
            {
                // Convert GID to x,y
                uint32_t gid = (layer->data->tiles[tileIndex].gid);
                tile.flip = (gid & 0xE0000000) >> 29;
                gid = gid & 0x1FFFFFFF;
                sf::Vector2i texPos = GID2XY(gid - tileset->firstgid, totalTilesetCols);
                tile.gid = gid;
                tile.ti = tileIndex;
                tile.x = (float)((w*tw));
                tile.y = (float)((h*th));
                tile.tx = texPos.x*tw;
                tile.ty = texPos.y*th;
                tiles.push_back(tile);
            }
        }
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

bool buildPolygon(Shape & s, TMX::Object::Ptr obj)
{
    bool status = false;
    if (obj->polygon != nullptr)
    {
        std::vector<std::string> pairs = split(obj->polygon->points, ' ');
        s.setPointCount(pairs.size());
        s.setPosition(sf::Vector2f(obj->x, obj->y));

        if (obj->rotation == 0)
        {
            int i = 0;
            for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
            {
                std::vector<std::string> comp = split(*pair, ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                s.setPoint(i, sf::Vector2f(x1, y1));
                ++i;
            }
            status = true;
        }
        else
        {
            int i = 0;
            for (auto p = pairs.begin(); p != pairs.end(); p++)
            {
                std::vector<std::string> comp = split(*p, ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                for (int u = 0; u < abs(obj->rotation); u += 90)
                {
                    float temp = x1;
                    x1 = -y1;
                    y1 = temp;
                }
                s.setPoint(i, sf::Vector2f(x1, y1));
                i++;
            }
        }
    }
    return status;
}
//
// buildPolyline stores the object offset in the position of the Shape.
// That is to say, the vertices that make up the shape are not translated.
//
bool buildPolyline(Shape & s, TMX::Object::Ptr obj)
{
    bool status = false;
    if (obj->polyline != nullptr)
    {
        std::vector<std::string> pairs = split(obj->polyline->points, ' ');

        size_t max = pairs.size() - 1;
        if (max == 1)
            max = 2;

        s.setPointCount(max);
        s.setPosition(sf::Vector2f(obj->x, obj->y));

        if (obj->rotation == 0)
        {
            int i = 0;
            for (auto p = pairs.begin(); p != pairs.end(); p++)
            {
                if (i == max)
                    break;
                std::vector<std::string> comp = split((*p), ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                s.setPoint(i, sf::Vector2f(x1, y1));
                i++;
            }
        }
        else
        {
            int i = 0;
            for (auto p = pairs.begin(); p != pairs.end(); p++)
            {
                if (i == max)
                    break;
                std::vector<std::string> comp = split(*p, ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                for (int u = 0; u < obj->rotation; u += 90)
                {
                    float temp = x1;
                    x1 = -(y1);
                    y1 = (temp);
                }
                s.setPoint(i, sf::Vector2f(x1, y1));
                i++;
            }
        }

        status = true;
    }
    return status;
}

bool buildRectangle(Shape & s, TMX::Object::Ptr obj)
{
    bool status = false;

    s.setPointCount(4);
    s.setPoint(0, sf::Vector2f(obj->x, obj->y));
    s.setPoint(1, sf::Vector2f(obj->x + obj->width, obj->y));
    s.setPoint(2, sf::Vector2f(obj->x + obj->width, obj->y + obj->height));
    s.setPoint(3, sf::Vector2f(obj->x, obj->y + obj->height));

    return status;
}

uint32_t addQuad(Vec<Vertex> & v, sf::FloatRect c, sf::IntRect t, unsigned char flip)
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

    sf::Vertex a(sf::Vector2f(c.left, c.top),                       topleft);
    v.push_back(a);

    sf::Vertex b(sf::Vector2f(c.left + c.width, c.top),             topright);
    v.push_back(b);

    sf::Vertex d(sf::Vector2f(c.left + c.width, c.top + c.height),  bottomright);
    v.push_back(d);

    sf::Vertex e(sf::Vector2f(c.left, c.top + c.height),            bottomleft);
    v.push_back(e);

    return 0;
}

uint32_t createVertexLayer(Vec<Vertex> & vertices, Vec<Tile> & tileLayer, uint32_t tileWidth, uint32_t tileHeight)
{
    for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
    {
        addQuad(vertices,
            sf::FloatRect(tdi->x, tdi->y, (float)tileWidth, (float)tileHeight),
            sf::IntRect(tdi->tx, tdi->ty, tileWidth, tileHeight),
            tdi->flip);
    }
    return 0;
}


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

qt::AABB getSearchRegion(sf::Vector2f center, sf::Vector2f size, float zoom)
{
    qt::AABB searchRegion;
    sf::Vector2f c = center;
    c.x = floor(c.x);
    c.y = floor(c.y);

    sf::Vector2f s = size;
    s.x *= zoom;
    s.y *= zoom;

    searchRegion.min.x = (float)((int)(c.x - (s.x / 2) + 64));
    searchRegion.min.y = (float)((int)(c.y - (s.y / 2) + 64));
    searchRegion.max.x = (float)((int)(c.x + (s.x / 2) - 64));
    searchRegion.max.y = (float)((int)(c.y + (s.y / 2) - 64));
    return searchRegion;
}

/*
sf::Vector2f rotatePoint(sf::Vector2f v, sf::Vector2f origin, float angle)
{
    //std::cout << angle << " ";
    angle = angle*(PI / 180.0f);
    
    v -= origin;
    v.x = v.x * cos(angle) - v.y * sin(angle);
    v.y = v.x * sin(angle) + v.y * cos(angle);
    v += origin;
    return v;
}

uint32_t addQuad(bali::QuadArray & v, sf::FloatRect c, sf::IntRect t, unsigned char flip)
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

uint32_t addRotShape(Shape & s, sf::FloatRect p, float angle)
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

    s.setPointCount(4);
    s.setPoint(0, sf::Vector2f(px1, py1));
    s.setPoint(0, sf::Vector2f(px2, py2));
    s.setPoint(0, sf::Vector2f(px3, py3));
    s.setPoint(0, sf::Vector2f(px4, py4));

    return 0;
}

uint32_t addRotQuad(sf::VertexArray & v, sf::FloatRect p, sf::IntRect t, float angle)
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

qt::AABB getSearchRegion(sf::Vector2f center, sf::Vector2f size, float zoom)
{
    qt::AABB searchRegion;
    sf::Vector2f c = center;
    c.x = floor(c.x);
    c.y = floor(c.y);

    sf::Vector2f s = size;
    s.x *= zoom;
    s.y *= zoom;

    searchRegion.min.x = (float)((int)(c.x - (s.x / 2) + 64));
    searchRegion.min.y = (float)((int)(c.y - (s.y / 2) + 64));
    searchRegion.max.x = (float)((int)(c.x + (s.x / 2) - 64));
    searchRegion.max.y = (float)((int)(c.y + (s.y / 2) - 64));
    return searchRegion;
}

uint32_t buildTileLayer(Tile::Vec & tileLayer, const TMX::Tileset::Ptr tileset, const TMX::Layer::Ptr layer)
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
                // Convert GID to x,y
                uint32_t gid = (layer->data->tiles[tileIndex].gid);
                pt.flip = (gid & 0xE0000000) >> 29;
                gid = gid & 0x1FFFFFFF;
                sf::Vector2i texPos = GID2XY(gid - tileset->firstgid, totalTilesetCols);
                pt.gid = gid;
                pt.ti = tileIndex;
                pt.x = (float)((w*tw));
                pt.y = (float)((h*th));
                pt.tx = texPos.x*tw;
                pt.ty = texPos.y*th;
                tileLayer.push_back(pt);
            }
        }
    }
    return 0;
}
uint32_t buildTileLayers(Tile::VecVec & tileLayers, const TMX::Tileset::Ptr tileset, const TMX::Layer::Vec layers)
{
    for (auto l = layers.begin(); l != layers.end(); l++)
    {
        tileLayers.push_back(Tile::Vec());
        buildTileLayer(tileLayers.back(), tileset, *l);
    }
    return 0;
}

uint32_t buildQuadLayer(bali::QuadArray & quadLayer, Tile::Vec & tileLayer, uint32_t tileWidth, uint32_t tileHeight)
{
    for (auto tdi = tileLayer.begin(); tdi != tileLayer.end(); tdi++)
    {
        addQuad(quadLayer,
                sf::FloatRect(tdi->x, tdi->y,
                (float)tileWidth, (float)tileHeight),
                sf::IntRect(tdi->tx, tdi->ty,
                tileWidth, tileHeight),
                tdi->flip);
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

bool buildPolygon(TMX::Object::Ptr obj, Shape & s, bool applyOffset = false)
{
    bool status = false;
    if (obj->polygon != nullptr)
    {
        std::vector<std::string> pairs = split(obj->polygon->points, ' ');
        s.setPointCount(pairs.size());
        s.setPosition(sf::Vector2f(obj->x, obj->y));

        if (obj->rotation == 0)
        {
            int i = 0;
            for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
            {
                std::vector<std::string> comp = split(*pair, ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                s.setPoint(i, sf::Vector2f(x1, y1));
                ++i;
            }
            status = true;
        }
        else
        {
            int i = 0;
            for (auto p = pairs.begin(); p != pairs.end(); p++)
            {
                std::vector<std::string> comp = split(*p, ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                for (int u = 0; u < abs(obj->rotation); u += 90)
                {
                    float temp = x1;
                    x1 = -y1;
                    y1 = temp;
                }
                s.setPoint(i, sf::Vector2f(x1, y1));
                i++;
            }
        }
    }
    return status;
}
//
// buildPolyline stores the object offset in the position of the Shape.
// That is to say, the vertices that make up the shape are not translated.
//
bool buildPolyline(TMX::Object::Ptr obj, Shape & s, bool applyOffset = false)
{
    bool status = false;
    if (obj->polyline != nullptr)
    {
        std::vector<std::string> pairs = split(obj->polyline->points, ' ');

        size_t max = pairs.size() - 1;
        if (max == 1)
            max = 2;

        s.setPointCount(max);
        s.setPosition(sf::Vector2f(obj->x, obj->y));

        if (obj->rotation == 0)
        {
            int i = 0;
            for (auto p = pairs.begin(); p != pairs.end(); p++)
            {
                if (i == max)
                    break;
                std::vector<std::string> comp = split((*p), ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                s.setPoint(i, sf::Vector2f(x1, y1));
                i++;
            }
        }
        else
        {
            int i = 0;
            for (auto p = pairs.begin(); p != pairs.end(); p++)
            {
                if (i == max)
                    break;
                std::vector<std::string> comp = split(*p, ',');
                float x1, y1;
                x1 = atol(comp[0].c_str());
                y1 = atol(comp[1].c_str());
                for (int u = 0; u < obj->rotation; u += 90)
                {
                    float temp = x1;
                    x1 = -(y1);
                    y1 = (temp);
                }
                s.setPoint(i, sf::Vector2f(x1, y1));
                i++;
            }
        }

        status = true;
    }
    return status;
}

bool buildRectangle(TMX::Object::Ptr obj, Shape & s)
{
    bool status = false;

    s.setPointCount(4);
    s.setPoint(0, sf::Vector2f(obj->x, obj->y));
    s.setPoint(1, sf::Vector2f(obj->x + obj->width, obj->y));
    s.setPoint(2, sf::Vector2f(obj->x + obj->width, obj->y + obj->height));
    s.setPoint(3, sf::Vector2f(obj->x, obj->y + obj->height));

    return status;
}

uint32_t buildPlayerObjectLayers(Shapes & polygons, TMX::Objectgroup::Ptr & objectGroup)
{
    //for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
    {

        for (auto obj = objectGroup->objects.begin(); obj != objectGroup->objects.end(); ++obj)
        {
            if ((*obj)->polygon != nullptr)
            {
                polygons.AddShape(Shape());
                buildPolygon(*obj, polygons.Last(), false);
            }
            else if ((*obj)->polyline != nullptr)
            {//NOTE: discard last point, engine assume last point is same as first.
                // TMX format is explicit about first and last point. even though they will always be the same.
                polygons.AddShape(Shape());
                buildPolyline(*obj, polygons.Last(), false);
            }
            else if ((*obj)->ellipse != nullptr)
            {
            }
            else
            {//a rectangle
                polygons.AddShape(Shape());
                buildRectangle(*obj, polygons.Last());
            }
        }
    }

    return 0;
}

//bool isSharedEdge(SAT::ContactInfo & ci, std::vector<SAT::Segment> & sharedEdges)
//{
//    for (auto se = sharedEdges.begin(); se != sharedEdges.end(); se++)
//    {
//        if (ci.edge.isEqual(*se))
//        {
//            return true;
//        }
//    }

//    return false;
//}

//uint32_t buildSharedEdgesLayers(SAT::Segment::Vec & sharedEdges, TMX::Objectgroup::Vec & objectGroups)
//{
//    for (auto objG = objectGroups.begin(); objG != objectGroups.end(); ++objG)
//    {
//        if ((*objG)->name != "SharedEdges")
//            continue;

//        std::vector<CONVEXSHAPE> polygons;
//        for (auto obj = (*objG)->objects.begin(); obj != (*objG)->objects.end(); ++obj)
//        {
//            if ((*obj)->polyline != nullptr)
//            {//NOTE: discard last point, engine assume last point is same as first.
//             // TMX format is explicit about first and last point. even though they will always be the same.
//                polygons.push_back(CONVEXSHAPE());
//                buildPolyline(*obj, polygons.back(), true);


//                //// Assume one line segment, so 2 points.
//                size_t pc = polygons.back().getPointCount();

//                if (pc >= 2)
//                {
//                    sf::Vector2f p1 = polygons.back().getPoint(0);
//                    sf::Vector2f p2 = polygons.back().getPoint((0 + 1) % pc);

//                    SAT::Segment seg;
//                    seg.start = sf::Vector2f(p1.x, p1.y);
//                    seg.end = sf::Vector2f(p2.x, p2.y);
//                    sharedEdges.push_back(seg);
//                }
//            }
//        }
//    }
//    return 0;
//}

uint32_t buildShapes(Shapes& polygons, TMX::Objectgroup::Ptr & objectGroup)
{

    for (auto obj = objectGroup->objects.begin(); obj != objectGroup->objects.end(); ++obj)
    {
        if ((*obj)->polyline != nullptr)
        {//NOTE: discard last point, engine assume last point is same as first.
            // TMX format is explicit about first and last point. even though they will always be the same.
            polygons.AddShape(Shape());
            buildPolyline(*obj, polygons.Last(), true);
        }
        else if ((*obj)->polygon != nullptr)
        {
            polygons.AddShape(Shape());
            buildPolygon(*obj, polygons.Last(), false);
        }
    }

    return 0;
}

uint32_t buildSearchLayer(qt::QuadTree::ShPtr & searchLayer, Vec<Shapes> & polygons)
{
    uint32_t tileid = 0;
    for (auto i = polygons.begin(); i != polygons.end; i++)
    {
        qt::XY pt;
        pt.ti = tileid;

        sf::FloatRect gb = polygons[tileid].getGlobalBounds();
        pt.x = gb.left + gb.width / 2.0f;
        pt.y = gb.top + gb.height / 2.0f;
        searchLayer->insert(pt);

        tileid++;
    }
    return 0;
}

uint32_t buildPlayerTransformedPolygon(RigidBody & phy, Shapes & pp, Shapes & pcp)
{
    pp.Last().setOrigin(8, 16);
    pp.Last().setPosition(phy.pos);
    pp.Last().setRotation(phy.angle);
    pp.Last().setScale(1.0f, 1.0f);

    pcp.AddShape(Shape());
    pcp.Last().setPointCount(8);
    for (int w = 0; w < 8; w++) {
        pcp.Last().setPoint(w,
            pcp.Last().getTransform().transformPoint(
                pcp.Last().getPoint(w)
            )
        );
    }
    return 0;
}

//
// This creates an array of layers. where each layer is 
// composed of quads, where the size of each quad is 
// equal to the size of the tiles in the last tileset.
// Assumed that tile sizes are uniform in a single tileset.

*/












}
