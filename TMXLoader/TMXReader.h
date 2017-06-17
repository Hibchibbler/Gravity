///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef TMXReader_h_
#define TMXReader_h_

#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <memory>

using std::cout;

#include "XMLReader.h"
#include "tmx.h"

using namespace std;

namespace bali {
  namespace TMX { 

    class MapContext
    {
    public:
        typedef std::shared_ptr<MapContext> Ptr;
        Map::Vec maps;
    public:

        TMX::Map::Ptr getMap(int i) {
            return maps[i];
        }

        static TMX::Objectgroup::Ptr getObjectGroup(TMX::Map::Ptr map, std::string name)
        {
            for (auto objG = map->objectgroups.begin(); objG != map->objectgroups.end(); ++objG)
            {
                if ((*objG)->name != name)
                    continue;
                return *objG;
            }
        }

        enum LoadState
        {
            UNINITIALIZED,
            MAP,
            MAP_PROPERTIES,
            MAP_PROPERTIES_PROPERTY,
            MAP_TILESET,
            MAP_TILESET_PROPERTIES,
            MAP_TILESET_PROPERTIES_PROPERTY,
            MAP_TILESET_TILEOFFSET,
            MAP_TILESET_IMAGE,
            MAP_TILESET_IMAGE_DATA,
            MAP_TILESET_IMAGE_DATA_TILEA,
            MAP_TILESET_TERRAINTYPES,
            MAP_TILESET_TERRAINTYPES_TERRAIN,
            MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES,
            MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES_PROPERTY,
            MAP_TILESET_TILEB,
            MAP_TILESET_TILEB_PROPERTIES,
            MAP_TILESET_TILEB_PROPERTIES_PROPERTY,
            MAP_TILESET_TILEB_IMAGE,
            MAP_TILESET_TILEB_IMAGE_DATA,
            MAP_TILESET_TILEB_IMAGE_DATA_TILEA,
            MAP_TILESET_TILEB_OBJECTGROUP,
            MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES,
            MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES_PROPERTY,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_ELLIPSE,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYGON,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYLINE,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA,
            MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA,
            MAP_TILESET_TILEB_ANIMATION,
            MAP_TILESET_TILEB_ANIMATION_FRAME,
            MAP_LAYER,
            MAP_LAYER_PROPERTIES,
            MAP_LAYER_PROPERTIES_PROPERTY,
            MAP_LAYER_DATA,
            MAP_LAYER_DATA_TILEA,
            MAP_OBJECTGROUP,
            MAP_OBJECTGROUP_PROPERTIES,
            MAP_OBJECTGROUP_PROPERTIES_PROPERTY,
            MAP_OBJECTGROUP_OBJECT,
            MAP_OBJECTGROUP_OBJECT_PROPERTIES,
            MAP_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY,
            MAP_OBJECTGROUP_OBJECT_ELLIPSE,
            MAP_OBJECTGROUP_OBJECT_POLYGON,
            MAP_OBJECTGROUP_OBJECT_POLYLINE,
            MAP_OBJECTGROUP_OBJECT_IMAGE,
            MAP_OBJECTGROUP_OBJECT_IMAGE_DATA,
            MAP_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA,
            MAP_IMAGELAYER,
            MAP_IMAGELAYER_PROPERTIES,
            MAP_IMAGELAYER_PROPERTIES_PROPERTY,
            MAP_IMAGELAYER_IMAGE,
            MAP_IMAGELAYER_IMAGE_DATA,
            MAP_IMAGELAYER_IMAGE_DATA_TILEA
        };
        std::string ToString(std::deque<std::string> & states)
        {
            std::string out("");
            for (size_t i = 0; i < states.size(); i++)
            {
                out += states[i];
                if (i < states.size()-1)
                    out += " ";
            }
            //cout << "<" << out << ">" << endl;
            return out;
        }
        LoadState ToState(std::deque<std::string> states)
        {
            LoadState state = LoadState::UNINITIALIZED;
            std::string stringifiedState = ToString(states);
            if (stringifiedState == "map")
                state = LoadState::MAP;
            else if (stringifiedState == "map properties")
                state = LoadState::MAP_PROPERTIES;
            else if (stringifiedState == "map properties property")
                state = LoadState::MAP_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map tileset")
                state = LoadState::MAP_TILESET;
            else if (stringifiedState == "map tileset properties")
                state = LoadState::MAP_TILESET_PROPERTIES;
            else if (stringifiedState == "map tileset properties property")
                state = LoadState::MAP_TILESET_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map tileset tileoffset")
                state = LoadState::MAP_TILESET_TILEOFFSET;
            else if (stringifiedState == "map tileset image")
                state = LoadState::MAP_TILESET_IMAGE;
            else if (stringifiedState == "map tileset image data")
                state = LoadState::MAP_TILESET_IMAGE_DATA;
            else if (stringifiedState == "map tileset image data tile")
                state = LoadState::MAP_TILESET_IMAGE_DATA_TILEA;
            else if (stringifiedState == "map tileset terraintypes")
                state = LoadState::MAP_TILESET_TERRAINTYPES;
            else if (stringifiedState == "map tileset terraintypes terrain")
                state = LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN;
            else if (stringifiedState == "map tileset terraintypes terrain properties")
                state = LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES;
            else if (stringifiedState == "map tileset terraintypes terrain properties property")
                state = LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map tileset tile")
                state = LoadState::MAP_TILESET_TILEB;
            else if (stringifiedState == "map tileset tile properties")
                state = LoadState::MAP_TILESET_TILEB_PROPERTIES;
            else if (stringifiedState == "map tileset tile properties property")
                state = LoadState::MAP_TILESET_TILEB_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map tileset tile image")
                state = LoadState::MAP_TILESET_TILEB_IMAGE;
            else if (stringifiedState == "map tileset tile image data")
                state = LoadState::MAP_TILESET_TILEB_IMAGE_DATA;
            else if (stringifiedState == "map tileset tile image data tile")
                state = LoadState::MAP_TILESET_TILEB_IMAGE_DATA_TILEA;
            else if (stringifiedState == "map tileset tile objectgroup")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP;
            else if (stringifiedState == "map tileset tile objectgroup properties")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES;
            else if (stringifiedState == "map tileset tile objectgroup properties property")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map tileset tile objectgroup object")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT;
            else if (stringifiedState == "map tileset tile objectgroup object properties")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES;
            else if (stringifiedState == "map tileset tile objectgroup object properties property")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map tileset tile objectgroup object ellipse")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_ELLIPSE;
            else if (stringifiedState == "map tileset tile objectgroup object polygon")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYGON;
            else if (stringifiedState == "map tileset tile objectgroup object polyline")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYLINE;
            else if (stringifiedState == "map tileset tile objectgroup object image")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE;
            else if (stringifiedState == "map tileset tile objectgroup object image data")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA;
            else if (stringifiedState == "map tileset tile objectgroup object image data tile")
                state = LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA;
            else if (stringifiedState == "map tileset tile animation")
                state = LoadState::MAP_TILESET_TILEB_ANIMATION;
            else if (stringifiedState == "map tileset tile animation frame")
                state = LoadState::MAP_TILESET_TILEB_ANIMATION_FRAME;
            else if (stringifiedState == "map layer")
                state = LoadState::MAP_LAYER;
            else if (stringifiedState == "map layer properties")
                state = LoadState::MAP_LAYER_PROPERTIES;
            else if (stringifiedState == "map layer properties property")
                state = LoadState::MAP_LAYER_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map layer data")
                state = LoadState::MAP_LAYER_DATA;
            else if (stringifiedState == "map layer data tile")
                state = LoadState::MAP_LAYER_DATA_TILEA;
            else if (stringifiedState == "map objectgroup")
                state = LoadState::MAP_OBJECTGROUP;
            else if (stringifiedState == "map objectgroup properties")
                state = LoadState::MAP_OBJECTGROUP_PROPERTIES;
            else if (stringifiedState == "map objectgroup properties property")
                state = LoadState::MAP_OBJECTGROUP_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map objectgroup object")
                state = LoadState::MAP_OBJECTGROUP_OBJECT;
            else if (stringifiedState == "map objectgroup object properties")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_PROPERTIES;
            else if (stringifiedState == "map objectgroup object properties property")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map objectgroup object ellipse")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_ELLIPSE;
            else if (stringifiedState == "map objectgroup object polygon")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_POLYGON;
            else if (stringifiedState == "map objectgroup object polyline")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_POLYLINE;
            else if (stringifiedState == "map objectgroup object image")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE;
            else if (stringifiedState == "map objectgroup object image data")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE_DATA;
            else if (stringifiedState == "map objectgroup object image data tile")
                state = LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA;
            else if (stringifiedState == "map imagelayer")
                state = LoadState::MAP_IMAGELAYER;
            else if (stringifiedState == "map imagelayer properties")
                state = LoadState::MAP_IMAGELAYER_PROPERTIES;
            else if (stringifiedState == "map imagelayer properties property")
                state = LoadState::MAP_IMAGELAYER_PROPERTIES_PROPERTY;
            else if (stringifiedState == "map imagelayer image")
                state = LoadState::MAP_IMAGELAYER_IMAGE;
            else if (stringifiedState == "map imagelayer image data")
                state = LoadState::MAP_IMAGELAYER_IMAGE_DATA;
            else if (stringifiedState == "map imagelayer image data tile")
                state = LoadState::MAP_IMAGELAYER_IMAGE_DATA_TILEA;
            return state;
        }
    public:
        std::deque<std::string> stateDeck;
    };

    class TMXReader : public XMLReader
    {
    public:

        TMXReader() {
        }

        ~TMXReader() {
        }

        static bool load(char *fn, MapContext & m) {
            return XMLReader::load(fn, &m, TMXReader::start_element_handler, TMXReader::end_element_handler, TMXReader::character_data_handler);
        }

        static void character_data_handler(void *data, const char *content, int length)
        {

        }

        static void end_element_handler(void *data, const char *element)
        {
            MapContext *ctx = (MapContext*)data;
            std::string elementName(element);
            ctx->stateDeck.pop_back();
            MapContext::LoadState currentState = ctx->ToState(ctx->stateDeck);
        }

        static void start_element_handler(void *data, const char *element, const char **attribute)
        {
            MapContext *ctx = (MapContext*)data;
            std::string elementName(element);
            ctx->stateDeck.push_back(elementName);
            MapContext::LoadState currentState = ctx->ToState(ctx->stateDeck);
            switch (currentState)
            {
            case MapContext::LoadState::UNINITIALIZED: {
                break;
            }case MapContext::LoadState::MAP: {
                Map::Ptr ptr = make_shared<Map>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("version", ptr->version);
                    ASSIGNIFMATCHESSTR("orientation", ptr->orientation);
                    ASSIGNIFMATCHESSTR("renderorder", ptr->renderorder);
                    ASSIGNIFMATCHESINT("width", ptr->width);
                    ASSIGNIFMATCHESINT("height", ptr->height);
                    ASSIGNIFMATCHESINT("tilewidth", ptr->tilewidth);
                    ASSIGNIFMATCHESINT("tileheight", ptr->tileheight);
                    ASSIGNIFMATCHESINT("hexsidelength", ptr->hexsidelength);
                    ASSIGNIFMATCHESSTR("staggeraxis", ptr->staggeraxis);
                    ASSIGNIFMATCHESSTR("backgroundcolor", ptr->backgroundcolor);
                    ASSIGNIFMATCHESINT("nextobjectid", ptr->nextobjectid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                } 
                ctx->maps.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->properties.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_TILESET: {
                Tileset::Ptr ptr = make_shared<Tileset>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("firstgid", ptr->firstgid);
                    ASSIGNIFMATCHESSTR("source", ptr->source);
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESINT("tilewidth", ptr->tilewidth);
                    ASSIGNIFMATCHESINT("tileheight", ptr->tileheight);
                    ASSIGNIFMATCHESINT("spacing", ptr->spacing);
                    ASSIGNIFMATCHESINT("margin", ptr->margin);
                    ASSIGNIFMATCHESINT("tilecount", ptr->tilecount);
                    ASSIGNIFMATCHESINT("columns", ptr->columns);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->tilesets.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_TILESET_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_TILESET_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->tilesets.back()->properties.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEOFFSET: {
                Tileoffset::Ptr ptr = make_shared<Tileoffset>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("x", ptr->x);
                    ASSIGNIFMATCHESINT("y", ptr->y);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->tilesets.back()->tileoffset = ptr;
                break;
            }case MapContext::LoadState::MAP_TILESET_IMAGE: {
                Image::Ptr ptr = make_shared<Image>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("format", ptr->format);
                    ASSIGNIFMATCHESSTR("source", ptr->source);
                    ASSIGNIFMATCHESSTR("trans", ptr->trans);
                    ASSIGNIFMATCHESINT("width", ptr->width);
                    ASSIGNIFMATCHESINT("height", ptr->height);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->tilesets.back()->images.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_TILESET_IMAGE_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->tilesets.back()->images.back()->data = ptr;
                break;
            }case MapContext::LoadState::MAP_TILESET_IMAGE_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->tilesets.back()->images.back()->data->tiles.push_back(val);
                break;
            }case MapContext::LoadState::MAP_TILESET_TERRAINTYPES: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES_PROPERTY: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB: {
                TileB::Ptr ptr = make_shared<TileB>();
                ctx->maps.back()->tilesets.back()->tiles.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->tilesets.back()->tiles.back()->properties.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_IMAGE: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_IMAGE_DATA: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_IMAGE_DATA_TILEA: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES_PROPERTY: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_ELLIPSE: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYGON: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYLINE: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_ANIMATION: {
                break;
            }case MapContext::LoadState::MAP_TILESET_TILEB_ANIMATION_FRAME: {
                break;
            }case MapContext::LoadState::MAP_LAYER: {
                Layer::Ptr ptr = make_shared<Layer>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESINT("x", ptr->x);
                    ASSIGNIFMATCHESINT("y", ptr->y);
                    ASSIGNIFMATCHESINT("width", ptr->width);
                    ASSIGNIFMATCHESINT("height", ptr->height);
                    ASSIGNIFMATCHESFLT("opacity", ptr->opacity);
                    ASSIGNIFMATCHESBLN("visible", ptr->visible);
                    ASSIGNIFMATCHESINT("offsetx", ptr->offsetx);
                    ASSIGNIFMATCHESINT("offsety", ptr->offsety);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->layers.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_LAYER_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_LAYER_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->layers.back()->properties.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_LAYER_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->layers.back()->data = ptr;
                break;
            }case MapContext::LoadState::MAP_LAYER_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->layers.back()->data->tiles.push_back(val);
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP: {
                Objectgroup::Ptr ptr = make_shared<Objectgroup>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("color", ptr->color);
                    ASSIGNIFMATCHESFLT("opacity", ptr->opacity);
                    ASSIGNIFMATCHESBLN("visible", ptr->visible);
                    ASSIGNIFMATCHESINT("offsetx", ptr->offsetx);
                    ASSIGNIFMATCHESINT("offsety", ptr->offsety);
                    ASSIGNIFMATCHESSTR("draworder", ptr->draworder);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_PROPERTIES_PROPERTY:{
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->properties.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT:{
                Object::Ptr ptr = make_shared<Object>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("id", ptr->x);
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESINT("x", ptr->x);
                    ASSIGNIFMATCHESINT("y", ptr->y);
                    ASSIGNIFMATCHESINT("width", ptr->width);
                    ASSIGNIFMATCHESINT("height", ptr->height);
                    ASSIGNIFMATCHESINT("rotation", ptr->rotation);
                    ASSIGNIFMATCHESINT("gid", ptr->gid);
                    ASSIGNIFMATCHESBLN("visible", ptr->visible);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->objects.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->objects.back()->properties.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_ELLIPSE: {
                Ellipse::Ptr ptr = make_shared<Ellipse>();
                ctx->maps.back()->objectgroups.back()->objects.back()->ellipse = ptr;
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_POLYGON: {
                Polygon::Ptr ptr = make_shared<Polygon>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("points", ptr->points);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->objects.back()->polygon = ptr;
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_POLYLINE: {
                Polyline::Ptr ptr = make_shared<Polyline>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("points", ptr->points);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->objects.back()->polyline = ptr;
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE: {
                Image::Ptr ptr = make_shared<Image>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("format", ptr->format);
                    ASSIGNIFMATCHESSTR("source", ptr->source);
                    ASSIGNIFMATCHESSTR("trans", ptr->trans);
                    ASSIGNIFMATCHESINT("width", ptr->width);
                    ASSIGNIFMATCHESINT("height", ptr->height);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->objects.back()->image = ptr;
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->objects.back()->image->data = ptr;
                break;
            }case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->objectgroups.back()->objects.back()->image->data->tiles.push_back(val);
                break;
            }case MapContext::LoadState::MAP_IMAGELAYER: {
                Imagelayer::Ptr ptr = make_shared<Imagelayer>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESINT("offsetx", ptr->offsetx);
                    ASSIGNIFMATCHESINT("offsety", ptr->offsety);
                    ASSIGNIFMATCHESFLT("opacity", ptr->opacity);
                    ASSIGNIFMATCHESBLN("visible", ptr->visible);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->imagelayers.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_IMAGELAYER_PROPERTIES: {
                break;
            }case MapContext::LoadState::MAP_IMAGELAYER_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->imagelayers.back()->properties.push_back(ptr);
                break;
            }case MapContext::LoadState::MAP_IMAGELAYER_IMAGE: {
                Image::Ptr ptr = make_shared<Image>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("format", ptr->format);
                    ASSIGNIFMATCHESSTR("source", ptr->source);
                    ASSIGNIFMATCHESSTR("trans", ptr->trans);
                    ASSIGNIFMATCHESINT("width", ptr->width);
                    ASSIGNIFMATCHESINT("height", ptr->height);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->imagelayers.back()->image = ptr;
                break;
            }case MapContext::LoadState::MAP_IMAGELAYER_IMAGE_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->imagelayers.back()->image->data = ptr;
                break;
            }case MapContext::LoadState::MAP_IMAGELAYER_IMAGE_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->maps.back()->imagelayers.back()->image->data->tiles.push_back(val);
                break;
            }
            }

        }

        ///Save
        static bool save(Map *m, const char *fn) {
            //        const char * xmlStart = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
            //        ofstream fout(fn);
            //        if (!fout.is_open())
            //            return false;
            //
            //        fout.writeline(xmlStart, strlen(xmlStart));
            return false;
        }
    private:
    };
  }//namespace tmx
}//namespace bali

#endif
