#include "Aseprite.h"
#include <iostream>
#include <fstream>

namespace ASE
{
bool LoadSpriteSheet(
    char* jsonbuf,
    SpriteSheet & sheet
)
{

    json_t mem[2048];
    json_t const* json = json_create(jsonbuf, mem, sizeof mem / sizeof *mem);
    if (!json) {
        std::cout << "Error json create." << std::endl;
        return EXIT_FAILURE;
    }

    json_t const* meta = json_getProperty(json, "meta");
    if (Bad(meta, JSON_OBJ, "meta")) { return EXIT_FAILURE; }
    std::cout << json_getName(meta) << std::endl;

    json_t const* app = json_getProperty(meta, "app");
    if (Bad(app, JSON_TEXT, "meta.app")) { return EXIT_FAILURE; }
    sheet.meta.app = json_getValue(app);

    json_t const* version = json_getProperty(meta, "version");
    if (Bad(version, JSON_TEXT, "meta.version")) { return EXIT_FAILURE; }
    sheet.meta.version = json_getValue(version);

    json_t const* image = json_getProperty(meta, "image");
    if (Bad(image, JSON_TEXT, "meta.image")) { return EXIT_FAILURE; }
    sheet.meta.image = json_getValue(image);

    json_t const* format = json_getProperty(meta, "format");
    if (Bad(format, JSON_TEXT, "meta.format")) { return EXIT_FAILURE; }
    sheet.meta.format = json_getValue(format);

    json_t const* size = json_getProperty(meta, "size");
    if (Bad(size, JSON_OBJ, "meta.size")) { return EXIT_FAILURE; }

    json_t const* w = json_getProperty(size, "w");
    if (Bad(w, JSON_INTEGER, "meta.size.w")) { return EXIT_FAILURE; }
    sheet.meta.size.w = static_cast<float>(json_getInteger(w));

    json_t const* h = json_getProperty(size, "h");
    if (Bad(h, JSON_INTEGER, "meta.size.h")) { return EXIT_FAILURE; }
    sheet.meta.size.h = static_cast<float>(json_getInteger(h));

    json_t const* scale = json_getProperty(meta, "scale");
    if (Bad(scale, JSON_TEXT, "meta.scale")) { return EXIT_FAILURE; }
    sheet.meta.scale = json_getValue(scale);


    json_t const* frametags = json_getProperty(meta, "frameTags");
    if (Bad(frametags, JSON_ARRAY, "meta.frameTags")) { return EXIT_FAILURE; }

    for (auto tagobj = json_getChild(frametags); tagobj != 0; tagobj = json_getSibling(tagobj))
    {
        if (Bad(tagobj, JSON_OBJ, "meta.frameTags.tag")) { return EXIT_FAILURE; }
        FrameTag newframetag;
        json_t const* name = json_getProperty(tagobj, "name");
        if (Bad(name, JSON_TEXT, "meta.frameTags.tag.name")) { return EXIT_FAILURE; }
        newframetag.name = json_getValue(name);

        json_t const* from = json_getProperty(tagobj, "from");
        if (Bad(from, JSON_INTEGER, "meta.frameTags.tag.from")) { return EXIT_FAILURE; }
        newframetag.from = static_cast<uint32_t>(json_getInteger(from));

        json_t const* to = json_getProperty(tagobj, "to");
        if (Bad(to, JSON_INTEGER, "meta.frameTags.tag.to")) { return EXIT_FAILURE; }
        newframetag.to = static_cast<uint32_t>(json_getInteger(to));

        json_t const* direction = json_getProperty(tagobj, "direction");
        if (Bad(direction, JSON_TEXT, "meta.frameTags.tag.direction")) { return EXIT_FAILURE; }
        newframetag.direction = json_getValue(direction);

        sheet.meta.frametags.push_back(newframetag);
    }

    json_t const* frames = json_getProperty(json, "frames");
    if (Bad(frames, JSON_OBJ, "frames")) { return EXIT_FAILURE; }
    std::cout << json_getName(frames) << std::endl;

    for (auto cel = json_getChild(frames); cel != 0; cel = json_getSibling(cel)) {
        if (Bad(cel, JSON_OBJ, "frames.cel")) { return EXIT_FAILURE; }
        Cel newcel;
        newcel.name = json_getName(cel);

        json_t const* frame = json_getProperty(cel, "frame");
        if (Bad(frame, JSON_OBJ, "frames.cel.frame")) { return EXIT_FAILURE; }

        json_t const* x = json_getProperty(frame, "x");
        if (Bad(x, JSON_INTEGER, "frames.cel.frame.x")) { return EXIT_FAILURE; }
        newcel.frame.position.x = static_cast<float>(json_getInteger(x));

        json_t const* y = json_getProperty(frame, "y");
        if (Bad(y, JSON_INTEGER, "frames.cel.frame.y")) { return EXIT_FAILURE; }
        newcel.frame.position.y = static_cast<float>(json_getInteger(y));

        json_t const* w = json_getProperty(frame, "w");
        if (Bad(w, JSON_INTEGER, "frames.cel.frame.w")) { return EXIT_FAILURE; }
        newcel.frame.size.w = static_cast<float>(json_getInteger(w));

        json_t const* h = json_getProperty(frame, "h");
        if (Bad(h, JSON_INTEGER, "frames.cel.frame.h")) { return EXIT_FAILURE; }
        newcel.frame.size.h = static_cast<float>(json_getInteger(h));

        json_t const* rotated = json_getProperty(cel, "rotated");
        if (Bad(rotated, JSON_BOOLEAN, "frames.cel.rotated")) { return EXIT_FAILURE; }
        newcel.rotated = json_getBoolean(rotated);

        json_t const* trimmed = json_getProperty(cel, "trimmed");
        if (Bad(trimmed, JSON_BOOLEAN, "frames.cel.trimmed")) { return EXIT_FAILURE; }
        newcel.trimmed = json_getBoolean(trimmed);

        json_t const* spritesourcesize = json_getProperty(cel, "spriteSourceSize");
        if (Bad(spritesourcesize, JSON_OBJ, "frames.cel.spriteSourceSize")) { return EXIT_FAILURE; }

        x = json_getProperty(spritesourcesize, "x");
        if (Bad(x, JSON_INTEGER, "frames.cel.spritesourcesize.x")) { return EXIT_FAILURE; }
        newcel.spritesourcesize.position.x = static_cast<float>(json_getInteger(x));

        y = json_getProperty(spritesourcesize, "y");
        if (Bad(y, JSON_INTEGER, "frames.cel.spritesourcesize.y")) { return EXIT_FAILURE; }
        newcel.spritesourcesize.position.y = static_cast<float>(json_getInteger(y));

        w = json_getProperty(spritesourcesize, "w");
        if (Bad(w, JSON_INTEGER, "frames.cel.spritesourcesize.w")) { return EXIT_FAILURE; }
        newcel.spritesourcesize.size.w = static_cast<float>(json_getInteger(w));

        h = json_getProperty(spritesourcesize, "h");
        if (Bad(h, JSON_INTEGER, "frames.cel.spritesourcesize.h")) { return EXIT_FAILURE; }
        newcel.spritesourcesize.size.h = static_cast<float>(json_getInteger(h));

        json_t const* sourcesize = json_getProperty(cel, "sourceSize");
        if (Bad(sourcesize, JSON_OBJ, "frames.cel.sourceSize")) { return EXIT_FAILURE; }

        w = json_getProperty(frame, "w");
        if (Bad(w, JSON_INTEGER, "frames.cel.sourceSize.w")) { return EXIT_FAILURE; }
        newcel.sourcesize.w = static_cast<float>(json_getInteger(w));

        h = json_getProperty(frame, "h");
        if (Bad(h, JSON_INTEGER, "frames.cel.sourceSize.h")) { return EXIT_FAILURE; }
        newcel.sourcesize.h = static_cast<float>(json_getInteger(h));

        json_t const* duration = json_getProperty(cel, "duration");
        if (Bad(duration, JSON_INTEGER, "frames.cel.duration")) { return EXIT_FAILURE; }
        newcel.duration = static_cast<float>(json_getInteger(duration));

        sheet.frames.push_back(newcel);
    }
    return false; // Good!
}

bool Bad(
    json_t const* o,
    jsonType_t t,
    const char* n
)
{
    if (!o || t != json_getType(o))
    {
        std::cout << n << " not found\n";
        return true;
    }
    return false;
}

bool
LoadJSONString(
    std::string jsonfilename,
    std::string & jsonstr
)
{
    std::ifstream fin(jsonfilename);

    if (fin.is_open())
    {
        std::string line;
        while (std::getline(fin, line))
        {
            jsonstr += line;
        }
        fin.close();
    }
    return true;
}

}