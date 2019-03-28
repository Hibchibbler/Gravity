#ifndef ASEPRITE_H_
#define ASEPRITE_H_

#include <string>
#include <vector>
#include "tinyjson.h"

namespace ASE
{
struct Size
{
    float       w;
    float       h;
};

struct Position
{
    float       x;
    float       y;
};

struct Frame
{
    Position    position;
    Size        size;
};

struct SpriteSourceSize
{
    Position    position;
    Size        size;
};

struct Cel
{
    std::string name;
    Frame       frame;
    bool        rotated;
    bool        trimmed;
    SpriteSourceSize spritesourcesize;
    Size        sourcesize;
    float       duration;
};

struct FrameTag
{
    std::string name;
    uint32_t    from;
    uint32_t    to;
    std::string direction;
};

struct Meta
{
    std::string app;
    std::string version;
    std::string image;
    std::string format;
    Size        size;
    std::string scale;
    std::vector<FrameTag> frametags;
};

struct SpriteSheet
{
    Meta        meta;
    std::vector<Cel> frames;
};

bool Bad(
    json_t const* o,
    jsonType_t t,
    const char* n
);

bool LoadSpriteSheet(
    char* jsonbuf,
    SpriteSheet & sheet
);

bool
LoadJSONString(
    std::string jsonfilename,
    std::string & jsonstr
);

}

#endif

