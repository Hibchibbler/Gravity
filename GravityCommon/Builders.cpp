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
#include <unordered_map>


#include "Pathfinding.h"
namespace bali
{

std::string
createProtoName(
    std::string name,
    uint32_t & index
)
{
    std::stringstream res;
    res << name << index;
    return res.str();
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

void
loadTexture(
    Texture         &t,
    TMX::Tileset::Ptr & tsb,
    sf::Color       c
)
{
    //TMX::Tileset::Ptr tileset = map->getTileset(tilesetname);
    t.img.loadFromFile("assets\\" + tsb->images.back()->source);
    t.img.createMaskFromColor(c);//transparent is black...
    t.tex.loadFromImage(t.img);
}

// debug
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


bool
assignProto(
    std::vector<Proto> & protos,
    std::string pid,
    Proto & proto
)
{
    for (auto & p : protos)
    {
        if (p.pid == pid)
        {
            proto = p;
            return true;
        }
    }
    return false;
}

bool GetTMXPropertyInt(TMX::Property::Vec & properties, std::string name, int & val)
{
    for (auto p : properties)
    {
        if (p->type == "int")
        {
            if (p->name == name)
            {
                val = std::strtol(p->value.c_str(), NULL, 16);
                return true;
            }
        }
    }
    return false;
}
bool GetTMXPropertyFloat(TMX::Property::Vec & properties, std::string name, float & val)
{
    for (auto p : properties)
    {
        if (p->type == "float")
        {
            if (p->name == name)
            {
                val = std::strtof(p->value.c_str(), NULL);
                return true;
            }
        }
    }
    return false;
}
bool GetTMXPropertyString(TMX::Property::Vec & properties, std::string name, std::string & val)
{
    for (auto p : properties)
    {
        if (p->type == "") // Type is empty when it is a string
        {
            if (p->name == name)
            {
                val = p->value;
                return true;
            }
        }
    }
    return false;
}

bool
buildWaypoints10(
    Vec<Waypoint> & waypoints,
    TMX::Objectgroup::Ptr & objectGroup
)
{
    typedef std::pair<sf::Vector2f, sf::Vector2f> Element;
    std::vector<PolylineDescriptor> allpolylines;
    for (auto obj = objectGroup->objects.begin(); obj != objectGroup->objects.end(); ++obj)
    {
        //std::unordered_map<sf::Vector2f, Waypoint> wpoints;
        bool status = false;
        std::vector<std::string> pairs;
        // if polyline, then waypoint system is a one way rail. if not polyline, 
        // it is a loop. we will reverse the ai paths of one way rails.
        bool ispolyline = false;
        if ((*obj)->polyline != nullptr) {
            pairs = split((*obj)->polyline->points, ' ');
        }
        else if ((*obj)->polygon != nullptr) {
            pairs = split((*obj)->polygon->points, ' ');
            ispolyline = true;
        }
        int id;
        uint32_t flags = 0;
        size_t weight = 0;
        if (!GetTMXPropertyInt((*obj)->properties, "flags", id))
        {
            std::cout << "A Waypoint path does not have an 'flags' property" << std::endl;
        }

        PolylineDescriptor polyline1;
        std::vector<Waypoint> allwaypoints; // After processing, should contain all unique waypoints, and their neighbors.
        std::vector<PathSegment> segments;      // Contains all segments for all polylines.
        assert(pairs.size() > 1);

        polyline1.pathid = (*obj)->id;
        polyline1.flags = flags;
        polyline1.looped = ispolyline;
        for (auto p = 0; p < pairs.size();p++)
        {
            std::vector<std::string> comp1 = split(pairs[p], ',');
            float x1 = strtof(comp1[0].c_str(), NULL) + (*obj)->x;
            float y1 = strtof(comp1[1].c_str(), NULL) + (*obj)->y;
            polyline1.points.push_back(sf::Vector2f(x1, y1));

            // We loop it back around
            if ((*obj)->polygon && p == pairs.size() - 1) {
                polyline1.points.push_back(polyline1.points.front());
            }
        }
        allpolylines.push_back(polyline1);
    }

    std::vector<PathSegment> allsegments;
    for (auto poly : allpolylines)
    {
        AddPolyline(poly, allsegments);
    }

    CreateGraph(allsegments, waypoints);
    return true;
}

uint32_t 
loadPrototype(Proto & proto,
              TMX::Objectgroup::Ptr & objectGroup
)
{
    //
    // First load up the properties
    //
    int mass;
    std::string pt;
    if (GetTMXPropertyInt(objectGroup->properties, "mass", mass))
    {
        proto.body.mass = mass;
    }
    else
    {
        std::cout << "A Prototype ["<< objectGroup->name << "] does not have an 'mass' property" << std::endl;
    }

    if (GetTMXPropertyString(objectGroup->properties, "prototype", pt))
    {
        if (pt == "player")
        {
            proto.type = Proto::ProtoType::PLAYER;
        }
        else if (pt == "monster")
        {
            proto.type = Proto::ProtoType::MONSTER;
        }
        else if (pt == "consumable")
        {
            proto.type = Proto::ProtoType::CONSUMABLE;
        }
        else if (pt == "static")
        {
            proto.type = Proto::ProtoType::STATIC;
        }
        else if (pt == "bullet")
        {
            proto.type = Proto::ProtoType::BULLET;
        }
    }
    else
    {
        std::cout << "A Prototype [" << objectGroup->name << "] does not have an 'prototype' property" << std::endl;
    }


    //
    // Then, load up the objects (aka geometry)
    //
    for (auto obj = objectGroup->objects.begin(); obj != objectGroup->objects.end(); ++obj)
    {
        std::string ssn;

        if ((*obj)->polygon != nullptr)
        {
            proto.shapes.push_back(Shape());
            buildPolygon(proto.shapes.back(), *obj);
        }
        else if ((*obj)->polyline != nullptr)
        {
            proto.shapes.push_back(Shape());
            buildPolygon(proto.shapes.back(), *obj);
        }
        else if ((*obj)->ellipse != nullptr)
        {
        }
        else
        {//a rectangle
            proto.shapes.push_back(Shape());
            buildRectangle(proto.shapes.back(), *obj);
        }

        if (GetTMXPropertyString((*obj)->properties, "spritesheetname", ssn))
        {
            proto.spritesheetname = ssn;
        }
        else
        {
            std::cout << "A Prototype [" << (*obj)->name << "] does not have an 'spritesheetname' property" << std::endl;
        }

        /*
        <object id="2132" x="944" y="248">
            <properties>
                <property name="spritesheetname" value="assets\evilman.json"/>
            </properties>
            <polyline points="0,0 6,-6 18,-6 24,0 24,12 18,18 6,18 0,12 0,0"/>
        </object>
        */
        // id="2132"  <-- this one.
        proto.tiledid = (*obj)->id;
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
        else
        {//a rectangle
            shapes.push_back(Shape());
            buildRectangle(shapes.back(), *obj);
        }

        // Calculate shape meta
        shapes.back().position = sf::Vector2f((float)(*obj)->x, (float)(*obj)->y);
        shapes.back().bounds = GetBounds(shapes.back());
        shapes.back().edges = calulateContactInfo(shapes.back());

    }

    // Calculate internal edges
    class Edge {
    public:
        Edge() {
            freq = 0;
            s = sf::Vector2f(0, 0);
            e = sf::Vector2f(0, 0);
            off = sf::Vector2f(0, 0);
        }
        sf::Vector2f s;
        sf::Vector2f e;
        sf::Vector2f off;
        uint64_t freq;
    };

    uint32_t ecnt = 0;
    size_t scnt = 0;
    Vec<Edge> edgehisto;
    for (auto & shape : shapes)
    {
        scnt += shape.edges.size();
        for (auto & edge : shape.edges)
        {
            // Is this edge in the histo ?
            bool found = false;
            for (auto & eh : edgehisto)
            {
                //if (eh.s == edge.p1 &&
                //    eh.e == edge.p2 &&
                //    eh.off == edge.off)
                if (eh.s+ eh.off == edge.p1+edge.off &&
                    eh.e+ eh.off == edge.p2+edge.off || 
                    eh.e + eh.off == edge.p1 + edge.off &&
                    eh.s + eh.off == edge.p2 + edge.off)
                {
                    eh.freq++;
                    found = true;
                }
            }
            if (!found)
            {
                edgehisto.push_back(Edge());
                edgehisto.back().s = edge.p1;
                edgehisto.back().e = edge.p2;
                edgehisto.back().off = edge.off;
                edgehisto.back().freq = 1;
                ecnt++;
            }
        }
    }

    uint32_t sharedcnt = 0;
    for (auto & eh : edgehisto)
    {
        if (eh.freq > 1)
        {
            for (auto & shape : shapes)
            {
                for (auto & edge : shape.edges)
                {
                    //if (eh.s == edge.p1 &&
                    //    eh.e == edge.p2 &&
                    //    eh.off == edge.off)
                    if (eh.s + eh.off == edge.p1 + edge.off &&
                        eh.e + eh.off == edge.p2 + edge.off ||
                        eh.e + eh.off == edge.p1 + edge.off &&
                        eh.s + eh.off == edge.p2 + edge.off)
                    {
                        edge.isinternal = 1;
                        sharedcnt++;
                    }
                }
            }
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

    if (layer->data->encoding == "csv")
    {

    }
    else // XML
    {
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
    }
    return 0;
}

Vec<ContactInfo>
calulateContactInfo(Shape & shape)
{
    Vec<ContactInfo> edges;
    // loop over the vertices
    size_t pc = shape.points.size();
    
    for (int i = 0; i < pc; i++)
    {
        // get the current vertex
        sf::Vector2f p1 = shape.points[i];

        // get the next vertex
        sf::Vector2f p2 = shape.points[(i + 1 == pc ? 0 : i + 1)];

        // subtract the two to get the edge vector
        sf::Vector2f edgeraw = p2 - p1;//.subtract(p1);

                                    // get either normal vector
        sf::Vector2f normal = vec::norm(vec::normal(edgeraw));

        // the perp method is just (x, y) => (-y, x) or (y, -x)
        ContactInfo ci;
        ci.normal = normal;
        ci.p1 = p1;// +shape.getPosition();
        ci.p2 = p2;// +shape.getPosition();
        ci.off = shape.position;
        ci.isinternal = 0;
        ci.overlap = 0;
        ci.thatindex = -1;
        ci.thisindex = -1;
        edges.push_back(ci);
    }
    return edges;
}

bool buildPolygon(Shape & s, TMX::Object::Ptr obj)
{
    bool status = false;

    assert(obj->polygon != nullptr);
    std::vector<std::string> pairs = split(obj->polygon->points, ' ');

    std::cout << obj->id << std::endl;
    if (obj->rotation == 0)
    {
        int i = 0;
        for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
        {
            std::vector<std::string> comp = split(*pair, ',');
            float x1, y1;
            x1 = strtof(comp[0].c_str(), NULL);
            y1 = strtof(comp[1].c_str(), NULL);
                
            s.points.push_back(sf::Vector2f(x1, y1));
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
            x1 = strtof(comp[0].c_str(), NULL);
            y1 = strtof(comp[1].c_str(), NULL);
            for (int u = 0; u < abs(obj->rotation); u += 90)
            {
                float temp = x1;
                x1 = -y1;
                y1 = temp;
            }
            s.points.push_back(sf::Vector2f(x1, y1));
            i++;
        }
    }
    return status;
}

bool buildRectangle(Shape & s, TMX::Object::Ptr obj)
{
    bool status = false;

    //s.setPointCount(4);
    s.points.push_back(sf::Vector2f((float)obj->x, (float)obj->y));
    s.points.push_back(sf::Vector2f((float)(obj->x + obj->width), (float)obj->y));
    s.points.push_back(sf::Vector2f((float)(obj->x + obj->width), (float)(obj->y + obj->height)));
    s.points.push_back(sf::Vector2f((float)obj->x, (float)(obj->y + obj->height)));

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
    sf::Vector2f topleft = sf::Vector2f((float)t.left, (float)t.top);
    sf::Vector2f topright = sf::Vector2f((float)(t.left + t.width), (float)t.top);
    sf::Vector2f bottomright = sf::Vector2f((float)(t.left + t.width), (float)(t.top + t.height));
    sf::Vector2f bottomleft = sf::Vector2f((float)t.left, (float)(t.top + t.height));

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

    searchRegion.min.x = (float)((int)(c.x - (s.x / 2)));
    searchRegion.min.y = (float)((int)(c.y - (s.y / 2)));
    searchRegion.max.x = (float)((int)(c.x + (s.x / 2)));
    searchRegion.max.y = (float)((int)(c.y + (s.y / 2)));
    return searchRegion;
}

void
CreateEntityBucket(
    Vec<Entity> & entities,
    SpatialBuckets & buckets
)
{
    for (uint32_t e = 0; e < entities.size(); e++)
    {
        sf::Vector2f newpos = GetCentroid(entities[e].proto.shapes[0]);

        buckets.add((uint32_t)newpos.x,
                    (uint32_t)newpos.y,// Change it also in StageMain.cpp
                    e);
    }
}
void
CreateCPolyBucket(
    Vec<Shape> & cpolys,
    SpatialBuckets & buckets
)
{
    uint32_t cnt = 0;
    for (uint32_t tid = 0; tid < cpolys.size(); tid++)
    {
        qt::XY pt;
        pt.ti = tid;
        Shape & shape = cpolys[tid];
        sf::FloatRect gb = shape.bounds;//GetBounds(shape);
        pt.x = gb.left + (gb.width / 2.0f);
        pt.y = gb.top + (gb.height / 2.0f);

        pt.x = pt.x + shape.position.x;
        pt.y = pt.y + shape.position.y;
        buckets.add((uint32_t)pt.x, (uint32_t)pt.y, tid);
        cnt++;
    }
}

void
GetEntityNeighbors(
    Vec<Entity> & entities,
    SpatialBuckets & buckets
)
{
    for (int e = 0; e < entities.size(); e++)
    {
        std::vector<size_t> nays;
        buckets.getneighbors((uint32_t)entities[e].proto.body.pos.x,
                             (uint32_t)entities[e].proto.body.pos.y,
                             nays);
        entities[e].collisionentities.insert(entities[e].collisionentities.end(),
                                             nays.begin(),
                                             nays.end());
    }
}

void
GetCPolyNeighbors(
    Context* context,
    Vec<Entity> & entities,
    SpatialBuckets & buckets
)
{
    for (int e = 0; e < entities.size(); e++)
    {
        entities[e].collisionshapes.clear();

        std::vector<size_t> nays;
        buckets.getneighbors((uint32_t)entities[e].proto.body.pos.x,
                             (uint32_t)entities[e].proto.body.pos.y,
                             nays);
        entities[e].collisionshapes.insert(entities[e].collisionshapes.end(),
                                           nays.begin(),
                                           nays.end());
    }
    //for (int e = 0; e < entities.size(); e++)
    //{
    //    entities[e].visiblesegments.clear();
    //    for (auto ko : entities[e].collisionshapes)
    //    {
    //        size_t pc = context->allcollisionshapes[ko].getPointCount();
    //        for (auto p = 0; p < pc - 1; p++)
    //        {
    //            sf::Vector2f point1 = context->allcollisionshapes[ko].getPoint(p);
    //            sf::Vector2f point2 = context->allcollisionshapes[ko].getPoint(p+1);
    //            Segment seg;
    //            seg.start = point1;
    //            seg.end = point2;
    //            seg.off = context->allcollisionshapes[ko].getPosition();
    //            entities[e].visiblesegments.push_back(seg);
    //        }
    //    }
    //}
}

bool
addEntity(
    bool registerwithaidirector,
    bool ignoreentitycollision,
    bool moving,
    Vec<Entity> & entities,
    Vec<Proto> & protos,
    TMX::Objectgroup::Ptr & ogptr
)
{
    for (auto o : ogptr->objects)
    {
        std::string pid;
        int mass;
        int pathid;
        if (!GetTMXPropertyInt(o->properties, "mass", mass))
        {
            std::cout << "A Entity [" << o->name << "] does not have an 'mass' property" << std::endl;
        }

        if (!GetTMXPropertyString(o->properties, "pid", pid))
        {
            std::cout << "A Entity [" << o->name << "] does not have an 'pid' property" << std::endl;
        }

        if (!GetTMXPropertyInt(o->properties, "pathid", pathid))
        {
            std::cout << "A Entity [" << o->name << "] does not have an 'pathid' property" << std::endl;
        }


        entities.push_back(Entity());
        // Find proto with matching pid
        // make this entity from that proto.
        assignProto(protos, pid, entities.back().proto);

        entities.back().pathid = pathid;
        entities.back().moving = moving;
        entities.back().registerwithaidirector = registerwithaidirector;
        entities.back().ignoreentitycollision = ignoreentitycollision;

        float x = (float)(*o).x;
        float y = (float)(*o).y;
        entities.back().proto.body.pos = sf::Vector2f(x, y);
        //context->entities.back().proto.shapes[0].setPosition(sf::Vector2f(x, y));
    }
    return true;
}


}
