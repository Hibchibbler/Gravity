#include "Level.h"
#include "TMXLoader/TMXReader.h"
#include "Builders.h"

#include <string>
#include <memory>

namespace bali
{

void Level::initialize(
    std::string   font,
    std::string   tmx
)
{
    ////
    //
    // Phase 1: Load Level Assets
    //
    ////

    //
    // Load Fonts
    //
    this->font.loadFromFile(font);
    
    //
    // Load TMX
    //
    map = std::make_shared<bali::TMX::Map>();
    TMX::TMXReader::load("level_test1.tmx", map);

    //
    // Load parallax background images
    //
    parallaxBackground.loadBackgrounds(map->imagelayers);


    //
    // Load Pyxel Edit texture atlas, and texture.
    //
    bali::tilemap::TileMapLoader::load("megaman_v0.xml", playerAniTileMap);
    textureAtlas1Image.loadFromFile("megaman_v0.png");
    textureAtlas1Image.createMaskFromColor(sf::Color::White, 0);
    textureAtlas1Tex.loadFromImage(textureAtlas1Image);


    ////
    //
    // Phase 2: Create level using Assets
    //
    ////

    //
    // Create Collision Polygon geometry from TMX
    //
    buildPolygonLayers(polygons,
        map->getObjectGroup("CollisionPolygons"));


    buildPolygonLayers(glowPolygons,
        map->getObjectGroup("GlowPolygons"));

    //
    // Create player collision polygon geometry from TMX
    //
    buildPlayerObjectLayers(playerPolygons,
        map->objectgroups);

    //
    // Create Background tileset from TMX
    //
    TMX::Tileset::Ptr tilesetBkgnd = map->getTileset("tilesetBack");
    backgroundImg.loadFromFile(tilesetBkgnd->images.back()->source);
    backgroundTex.loadFromImage(backgroundImg);

    //
    // Create internal representation of background geometry from TMX
    //
    tileLayers.push_back(Tile::Vec());
    buildTileLayer(tileLayers.back(), tilesetBkgnd, map->getLayer("BackgroundLayer"));

    //
    // Create renderable background geometry from internal representation
    //
    quadLayers.push_back(bali::QuadArray());
    buildQuadLayer(quadLayers.back(), tileLayers[0], map->tilewidth, map->tileheight);

    //
    // Create Foreground tileset from TMX
    //
    TMX::Tileset::Ptr tilesetA = map->getTileset("tilesetFore");
    tilesetAImg.loadFromFile(tilesetA->images.back()->source);
    tilesetAImg.createMaskFromColor(sf::Color::Black);//transparent is black...
    tilesetATex.loadFromImage(tilesetAImg);

    //
    // Create internal representation of foreground geometry from TMX
    //
    tileLayers.push_back(Tile::Vec());
    buildTileLayer(tileLayers.back(), tilesetA, map->getLayer("ForegroundLayer"));

    //
    // Create renderable foreground geometry from internal representation
    //
    quadLayers.push_back(bali::QuadArray());
    buildQuadLayer(quadLayers.back(), tileLayers[1], map->tilewidth, map->tileheight);

    //
    // Create Quadtree for Collision Polygons.
    //
    int maxDepth = 15;
    qt::AABB aabb;
    aabb.min.x = -64;
    aabb.min.y = -64;
    aabb.max.x = aabb.max.y = (map->width + 2) * 32;//in pixels
    qt::QuadTree::ShPtr searchLayer = std::make_shared<qt::QuadTree>();
    searchLayer->initialize(aabb, maxDepth);
    buildSearchLayer(searchLayer, polygons);
    searchLayers.push_back(searchLayer);

    //
    // Finally
    //
    TargetCameraAngle = 0.f;

    // No longer needed.
    map.reset();
}

void Level::update(
    vec::VECTOR2    size,
    vec::VECTOR2    pos,
    float           angle
    )
{
    this->size = size;
    this->pos = pos;


    //
    // Construct all collision polygons that are visible to the player
    //
    qt::AABB searchRegion = getSearchRegion(pos, size, 1.0f);
    std::vector<qt::XY> sr;
    sr = searchLayers.at(0)->search(searchRegion);
    polygonsVisible.clear();
    for (auto p = sr.begin(); p != sr.end(); p++)
    {
        float x, y;
        x = p->x;
        y = p->y;
        polygonsVisible.push_back(polygons[p->ti]);
    }

    //
    // Update background parallax
    //
    parallaxBackground.updateBackgrounds(pos);

    TargetCameraAngle += (angle - TargetCameraAngle) / 10.f;

}

void Level::draw(sf::RenderTarget & target, sf::RenderStates states) 
{
    // Draw background parallax
    {
        target.draw(parallaxBackground.bgimages[0].sprite, states);
        target.draw(parallaxBackground.bgimages[1].sprite, states);
        target.draw(parallaxBackground.bgimages[2].sprite, states);
        target.draw(parallaxBackground.bgimages[3].sprite, states);
        target.draw(parallaxBackground.bgimages[4].sprite, states);
    }

    // Draw foregound tiles
    {
        states.texture = &tilesetATex;
        target.draw(quadLayers[1], states);
    }

    // Draw player polygons
    for (auto poly = playerPolygons.begin(); poly != playerPolygons.end(); ++poly)
    {
        states.texture = &textureAtlas1Tex;
        //poly->setOutlineThickness(1);
        target.draw(*poly, states);
    }
}

void Level::cleanup()
{

}



void BackgroundParallax::loadBackgrounds(TMX::Imagelayer::Vec & layers)
{
    //
    // Load parallax background images
    //
    bgimages[0].img.loadFromFile(layers[0]->image->source);
    bgimages[0].img.createMaskFromColor(sf::Color::White);
    bgimages[0].tex.loadFromImage(bgimages[0].img);
    bgimages[0].sprite.setTexture(bgimages[0].tex);
    bgimages[0].sprite.scale(8.0f, 8.0f);

    bgimages[1].img.loadFromFile(layers[1]->image->source);
    bgimages[1].img.createMaskFromColor(sf::Color::White);
    bgimages[1].tex.loadFromImage(bgimages[1].img);
    bgimages[1].sprite.setTexture(bgimages[1].tex);
    bgimages[1].sprite.scale(8.0f, 8.0f);

    bgimages[2].img.loadFromFile(layers[2]->image->source);
    bgimages[2].img.createMaskFromColor(sf::Color::White);
    bgimages[2].tex.loadFromImage(bgimages[2].img);
    bgimages[2].sprite.setTexture(bgimages[2].tex);
    bgimages[2].sprite.scale(8.0f, 8.0f);

    bgimages[3].img.loadFromFile(layers[3]->image->source);
    bgimages[3].img.createMaskFromColor(sf::Color::White);
    bgimages[3].tex.loadFromImage(bgimages[3].img);
    bgimages[3].sprite.setTexture(bgimages[3].tex);
    bgimages[3].sprite.scale(8.0f, 8.0f);

    bgimages[4].img.loadFromFile(layers[4]->image->source);
    bgimages[4].img.createMaskFromColor(sf::Color::White);
    bgimages[4].tex.loadFromImage(bgimages[4].img);
    bgimages[4].sprite.setTexture(bgimages[4].tex);
    bgimages[4].sprite.scale(8.0f, 8.0f);
}

void BackgroundParallax::updateBackgrounds(vec::VECTOR2 pos)
{
    bgimages[0].sprite.setPosition(pos / 1.5f);
    bgimages[0].sprite.move(-100, -100);
    bgimages[1].sprite.setPosition(pos / 2.5f);
    bgimages[1].sprite.move(-50, -50);
    bgimages[2].sprite.setPosition(pos / 3.0f);
    bgimages[2].sprite.move(-25, -25);
    bgimages[3].sprite.setPosition(pos / 3.5f);
    bgimages[3].sprite.move(-15, -15);
    bgimages[4].sprite.setPosition(pos / 4.0f);
    bgimages[4].sprite.move(-10, -10);
}

}