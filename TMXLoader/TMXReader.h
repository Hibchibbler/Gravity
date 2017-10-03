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
        Map::Ptr map;
    public:


        static std::string ToString(std::deque<std::string> & states)
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

        static bool load(const char *fn,  Map::Ptr m) {
            MapContext ctx;
            ctx.map = m;
            return XMLReader::LoadXMLFile(fn, (void*)&ctx, TMXReader::start_element_handler, TMXReader::end_element_handler, TMXReader::character_data_handler);
        }

        static void character_data_handler(void *data, const char *content, int length)
        {

        }

        static void end_element_handler(void *data, const char *element)
        {
            MapContext *ctx = (MapContext*)data;
            ctx->stateDeck.pop_back();
        }

        static void start_element_handler(void *data, const char *element, const char **attribute)
        {
            MapContext *ctx = (MapContext*)data;
            std::string elementName(element);
            ctx->stateDeck.push_back(elementName);

            // Get a string representation of the deque
            std::string state = MapContext::ToString(ctx->stateDeck);
            // cout << state << endl;


            if (state == "map")
            {//case MapContext::LoadState::MAP: {

                Map::Ptr ptr = ctx->map;
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
                
            }
            else if (state == "map properties")
            {
            
            }
            else if (state == "map properties property")
            {//case MapContext::LoadState::MAP_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->properties.push_back(ptr);
                
            }
            else if (state == "map tileset")
            {//case MapContext::LoadState::MAP_TILESET: {
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
                ctx->map->tilesets.push_back(ptr);
                
            }
            else if (state == "map tileset properties")
            {//case MapContext::LoadState::MAP_TILESET_PROPERTIES: {
             
            }
            else if (state == "map tileset properties property")
            {//case MapContext::LoadState::MAP_TILESET_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->tilesets.back()->properties.push_back(ptr);
            }
            else if (state == "map tileset tileoffset")
            {//case MapContext::LoadState::MAP_TILESET_TILEOFFSET: {
                Tileoffset::Ptr ptr = make_shared<Tileoffset>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("x", ptr->x);
                    ASSIGNIFMATCHESINT("y", ptr->y);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->tilesets.back()->tileoffset = ptr;
             
            }
            else if (state == "map tileset image")
            {//case MapContext::LoadState::MAP_TILESET_IMAGE: {
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
                ctx->map->tilesets.back()->images.push_back(ptr);
            }
            else if (state == "map tileset image data")
            {//case MapContext::LoadState::MAP_TILESET_IMAGE_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->tilesets.back()->images.back()->data = ptr;
            }
            else if (state == "map tileset image data tile")
            {//case MapContext::LoadState::MAP_TILESET_IMAGE_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->tilesets.back()->images.back()->data->tiles.push_back(val);
            }
            else if (state == "map tileset terraintypes")
            {//case MapContext::LoadState::MAP_TILESET_TERRAINTYPES: {

            }
            else if (state == "map tileset terraintypes terrain")
            {//case MapContext::LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN: {

            }
            else if (state == "map tileset terraintypes terrain properties")
            {//case MapContext::LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES: {

            }
            else if (state == "map tileset terraintypes terrain properties property")
            {//case MapContext::LoadState::MAP_TILESET_TERRAINTYPES_TERRAIN_PROPERTIES_PROPERTY: {

            }
            else if (state == "map tileset tile")
            {//case MapContext::LoadState::MAP_TILESET_TILEB: {
                TileB::Ptr ptr = make_shared<TileB>();
                ctx->map->tilesets.back()->tiles.push_back(ptr);

            }
            else if (state == "map tileset tile properties")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_PROPERTIES: {

            }
            else if (state == "map tileset tile properties property")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->tilesets.back()->tiles.back()->properties.push_back(ptr);
            }
            else if (state == "map tileset tile image")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_IMAGE: {

            }
            else if (state == "map tileset tile image data")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_IMAGE_DATA: {

            }
            else if (state == "map tileset tile image data tile")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_IMAGE_DATA_TILEA: {

            }
            else if (state == "map tileset tile objectgroup")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP: {

            }
            else if (state == "map tileset tile objectgroup properties")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES: {

            }
            else if (state == "map tileset tile objectgroup properties property")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_PROPERTIES_PROPERTY: {

            }
            else if (state == "map tileset tile objectgroup object")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT: {

            }
            else if (state == "map tileset tile objectgroup object properties")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES: {

            }
            else if (state == "map tileset tile objectgroup object properties property")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY: {

            }
            else if (state == "map tileset tile objectgroup object ellipse")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_ELLIPSE: {

            }
            else if (state == "map tileset tile objectgroup object polygon")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYGON: {

            }
            else if (state == "map tileset tile objectgroup object polyline")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_POLYLINE: {

            }
            else if (state == "map tileset tile objectgroup object image")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE: {

            }
            else if (state == "map tileset tile objectgroup object image data")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA: {

            }
            else if (state == "map tileset tile objectgroup object image data tile")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA: {

            }
            else if (state == "map tileset tile animation")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_ANIMATION: {

            }
            else if (state == "map tileset tile animation frame")
            {//case MapContext::LoadState::MAP_TILESET_TILEB_ANIMATION_FRAME: {

            }
            else if (state == "map layer")
            {//case MapContext::LoadState::MAP_LAYER: {
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
                ctx->map->layers.push_back(ptr);

            }
            else if (state == "map layer properties")
            {//case MapContext::LoadState::MAP_LAYER_PROPERTIES: {

            }
            else if (state == "map layer properties property")
            {//case MapContext::LoadState::MAP_LAYER_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->layers.back()->properties.push_back(ptr);

            }
            else if (state == "map layer data")
            {//case MapContext::LoadState::MAP_LAYER_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->layers.back()->data = ptr;

            }
            else if (state == "map layer data tile")
            {//case MapContext::LoadState::MAP_LAYER_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->layers.back()->data->tiles.push_back(val);

            }
            else if (state == "map objectgroup")
            {//case MapContext::LoadState::MAP_OBJECTGROUP: {
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
                ctx->map->objectgroups.push_back(ptr);

            }
            else if (state == "map objectgroup properties")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_PROPERTIES: {

            }
            else if (state == "map objectgroup properties property")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_PROPERTIES_PROPERTY:{
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->objectgroups.back()->properties.push_back(ptr);

            }
            else if (state == "map objectgroup object")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT:{
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
                ctx->map->objectgroups.back()->objects.push_back(ptr);

            }
            else if (state == "map objectgroup object properties")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_PROPERTIES: {

            }
            else if (state == "map objectgroup object properties property")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->objectgroups.back()->objects.back()->properties.push_back(ptr);

            }
            else if (state == "map objectgroup object ellipse")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_ELLIPSE: {
                Ellipse::Ptr ptr = make_shared<Ellipse>();
                ctx->map->objectgroups.back()->objects.back()->ellipse = ptr;

            }
            else if (state == "map objectgroup object polygon")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_POLYGON: {
                Polygon::Ptr ptr = make_shared<Polygon>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("points", ptr->points);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->objectgroups.back()->objects.back()->polygon = ptr;

            }
            else if (state == "map objectgroup object polyline")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_POLYLINE: {
                Polyline::Ptr ptr = make_shared<Polyline>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("points", ptr->points);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->objectgroups.back()->objects.back()->polyline = ptr;

            }
            else if (state == "map objectgroup object image")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE: {
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
                ctx->map->objectgroups.back()->objects.back()->image = ptr;

            }
            else if (state == "map objectgroup object image data")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->objectgroups.back()->objects.back()->image->data = ptr;

            }
            else if (state == "map objectgroup object image data tile")
            {//case MapContext::LoadState::MAP_OBJECTGROUP_OBJECT_IMAGE_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->objectgroups.back()->objects.back()->image->data->tiles.push_back(val);

            }
            else if (state == "map imagelayer")
            {//case MapContext::LoadState::MAP_IMAGELAYER: {
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
                ctx->map->imagelayers.push_back(ptr);

            }
            else if (state == "map imagelayer properties")
            {//case MapContext::LoadState::MAP_IMAGELAYER_PROPERTIES: {

            }
            else if (state == "map imagelayer properties property")
            {//case MapContext::LoadState::MAP_IMAGELAYER_PROPERTIES_PROPERTY: {
                Property::Ptr ptr = make_shared<Property>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("name", ptr->name);
                    ASSIGNIFMATCHESSTR("type", ptr->type);
                    ASSIGNIFMATCHESSTR("value", ptr->value);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->imagelayers.back()->properties.push_back(ptr);

            }
            else if (state == "map imagelayer image")
            {//case MapContext::LoadState::MAP_IMAGELAYER_IMAGE: {
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
                ctx->map->imagelayers.back()->image = ptr;

            }
            else if (state == "map imagelayer image data")
            {//case MapContext::LoadState::MAP_IMAGELAYER_IMAGE_DATA: {
                Data::Ptr ptr = make_shared<Data>();
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESSTR("encoding", ptr->encoding);
                    ASSIGNIFMATCHESSTR("compression", ptr->compression);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->imagelayers.back()->image->data = ptr;

            }
            else if (state == "map imagelayer image data tile")
            {//case MapContext::LoadState::MAP_IMAGELAYER_IMAGE_DATA_TILEA: {
                TileA::Val val;
                for (size_t i = 0; attribute[i]; i += 2)
                {
                    ASSIGNIFMATCHESINT("gid", val.gid);
                    cout << "Unexpected attribute " << attribute[i] << "=" << attribute[i + 1] << " in " << std::string(element) << endl;
                }
                ctx->map->imagelayers.back()->image->data->tiles.push_back(val);
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
