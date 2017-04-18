///////////////////////////////////////////////////////////////////////////////
// Daniel Jay Ferguson
// You do you
//////////////////////////////////////////////////////////////////////////////

#ifndef tmx_structures_h_
#define tmx_structures_h_
#include <string>
#include <vector>
#include <memory>

namespace bali {
    namespace TMX {

        class Property
        {
        public:
            typedef std::shared_ptr<Property> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Property(){}
        public:
            std::string         name;
            std::string         type;
            std::string         value;
        };

        class TileA
        {
        public:
            typedef std::shared_ptr<TileA> Ptr;
            typedef TileA Val;
            typedef std::vector<Val> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            TileA(){}
        public:
            int32_t             gid;
        };

        class Point
        {
        public:
            typedef std::shared_ptr<Point> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Point()
            {
                x               = 0;
                y               = 0;
            }
        public:
            int32_t             x;
            int32_t             y;
        };

        class Ellipse
        {
        public:
            typedef std::shared_ptr<Ellipse> Ptr;
        public:
            Ellipse() {}
        public:
            int32_t             unused;
        };

        class Polygon
        {
        public:
            typedef std::shared_ptr<Polygon> Ptr;
        public:
            Polygon() {}
        public:
            //Point::Vec points;
            std::string         points;
        };

        class Polyline
        {
        public:
            typedef std::shared_ptr<Polyline> Ptr;
        public:
            Polyline(){}
        public:
            //Point::Vec points;
            std::string         points;
        };

        class Data
        {
        public:
            typedef std::shared_ptr<Data> Ptr;
        public:
            Data()
            {
            }
        public:
            std::string         encoding;
            std::string         compression;
        public:
            // 0 to many
            TileA::Vec          tiles;
        };

        class Image
        {
        public:
            typedef std::shared_ptr<Image> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Image()
            {
                width           = 0;
                height          = 0;
            }
        public:
            std::string         format;
            std::string         source;
            std::string         trans;
            int32_t             width;
            int32_t             height;
        public:
            // 0 or 1
            Data::Ptr           data;
        };

        class Object
        {
        public:
            typedef std::shared_ptr<Object> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Object()
            {
                id              = 0;
                x               = 0;
                y               = 0;
                width           = 0;
                height          = 0;
                rotation        = 0;
                gid             = 0;
                visible         = true;
            }
        public:
            int32_t             id;
            std::string         name;
            std::string         type;

            int32_t             x;
            int32_t             y;
            int32_t             width;
            int32_t             height;
            int32_t             rotation;
            int32_t             gid;
            bool                visible;
        public:
            // 0 or more
            Property::Vec       properties;

            // 0 or 1
            Ellipse::Ptr        ellipse;
            Polygon::Ptr        polygon;
            Polyline::Ptr       polyline;
            Image::Ptr          image;
        };

        class Objectgroup
        {
        public:
            typedef std::shared_ptr<Objectgroup> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Objectgroup()
            {
                x               = 0;
                y               = 0;
                width           = 0;
                height          = 0;
                offsetx         = 0;
                offsety         = 0;
                opacity         = 1.0;
                visible         = true;
            }
        public:
            std::string         name;
            std::string         color;
            int32_t             x; // Deprec8ed - always 0
            int32_t             y; // Deprec8ed - always 0
            int32_t             width; // Deprecated - same as map width
            int32_t             height; // Deprecated - same as map height;
            float               opacity;
            bool                visible;
            int32_t             offsetx;
            int32_t             offsety;
            std::string         draworder;
        public:
            Property::Vec       properties;
            Object::Vec         objects;
        };

        class Frame
        {
        public:
            typedef std::shared_ptr<Frame> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Frame()
            {
                tileid          = 0;
                duration        = 0;
            }
        public:
            int32_t             tileid;
            int32_t             duration;
        };

        class Animation
        {
        public:
            typedef std::shared_ptr<Animation> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Animation() {}
        public:
        };

        class TileB
        {
        public:
            typedef std::shared_ptr<TileB> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            TileB() {}
            int32_t             id;
            std::string         terrain;
            float               probability;
        public:
            // 0 to many
            Property::Vec       properties;
            // 0 or 1
            Image::Ptr          image;
            Objectgroup::Ptr    objectgroup;
            Animation::VecPtr   animation;
        };

        // Terrain Types
        class Terrain
        {
        public:
            typedef std::shared_ptr<Terrain> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Terrain()
            {
                tile            = 0;
            }
        public:
            std::string         name;
            int32_t             tile;
        public:
            // 0 to many
            Property::Vec       properties;
        };


        class Tileoffset
        {
        public:
            typedef std::shared_ptr<Tileoffset> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Tileoffset()
            {
                x               = 0;
                y               = 0;
            }
        public:
            int32_t             x;
            int32_t             y;
        };

        class Tileset
        {
        public:
            typedef std::shared_ptr<Tileset> Ptr;
            typedef std::vector<Ptr>         Vec;
            typedef std::shared_ptr<Vec>     VecPtr;
        public:
            Tileset()
            {
                firstgid        = 0;
                tilewidth       = 0;
                tileheight      = 0;
                spacing         = 0;
                margin          = 0;
                tilecount       = 0;
                columns         = 0;
            }
        public:
            int32_t             firstgid;
            std::string         source;
            std::string         name;
            int32_t             tilewidth;
            int32_t             tileheight;
            int32_t             spacing;
            int32_t             margin;
            int32_t             tilecount;
            int32_t             columns;
        public:
            // 0 to many
            Property::Vec       properties;
            Image::Vec          images;
            Tileoffset::Ptr     tileoffset;

            // Terrain Types
            Terrain::Vec        terraintypes;
            TileB::Vec          tiles;
        };

        class Layer
        {
        public:
            typedef std::shared_ptr<Layer> Ptr;
            typedef std::vector<Ptr>       Vec;
            typedef std::shared_ptr<Vec>   VecPtr;
        public:
            Layer()
            {
                x               = 0;
                y               = 0;
                offsetx         = 0;
                offsety         = 0;
                width           = 0;
                height          = 0;
                opacity         = 1.0;
                visible         = true;
            }

        public:
            std::string         name;
            int32_t             x; // Deprec8ed - always 0
            int32_t             y; // Deprec8ed - always 0
            int32_t             width; // Deprecated - same as map width
            int32_t             height; // Deprecated - same as map height
            float               opacity;
            bool                visible;
            int32_t             offsetx;
            int32_t             offsety;
        public:
            Property::Vec       properties;
            Data::Ptr           data;
        };

        class Imagelayer
        {
        public:
            typedef std::shared_ptr<Imagelayer> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Imagelayer()
            {
                offsetx         = 0;
                offsety         = 0;
                opacity         = 1.0;
                visible         = true;
            }
        public:
            std::string         name;
            int32_t             offsetx;
            int32_t             offsety;
            float               opacity;
            bool                visible;
        public:
            // 0 to many
            Property::Vec       properties;
            // 0 or 1
            Image::Ptr          image;
        };

        class Map
        {
        public:
            typedef std::shared_ptr<Map> Ptr;
            typedef std::vector<Ptr>     Vec;
            typedef std::shared_ptr<Vec> VecPtr;
        public:
            Map()
            {
                width           = 0;
                height          = 0;
                tilewidth       = 0;
                tileheight      = 0;
                hexsidelength   = 0;
                nextobjectid    = 0;
            }
            Layer::Ptr getLayer() { return layers.back(); }
            Layer::Ptr getLayer(int i) { return layers[i]; }
        public:
            std::string         version;
            std::string         orientation;
            std::string         renderorder;
            int32_t             width;
            int32_t             height;
            int32_t             tilewidth;
            int32_t             tileheight;
            int32_t             hexsidelength;
            std::string         staggeraxis;
            std::string         staggerindex;
            std::string         backgroundcolor;
            int32_t             nextobjectid;
        public:
            Property::Vec       properties;
            Tileset::Vec        tilesets;
            Layer::Vec          layers;
            Objectgroup::Vec    objectgroups;
            Imagelayer::Vec     imagelayers;
        };
    }
}

#endif