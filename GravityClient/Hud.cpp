#include "Hud.h"
#include <sstream>
#include <iostream>

namespace bali
{

void Hud::initialize(sf::Font* font)
{
    pFont = font;
}

void Hud::update(
    vec::VECTOR2 up,
    vec::VECTOR2 left,
    vec::VECTOR2 center,
    vec::VECTOR2 size,
    sf::Time elapsed, 
    vec::VECTOR2 pos,
    vec::VECTOR2 vel,
    float angle, 
    float zoom
)
{
    //
    // Update HUD text
    //
    std::stringstream ss;
    ss << "FPS: " << (uint32_t)(1.0f / elapsed.asSeconds()) << std::endl;
    ss << "POS: " << (uint32_t)pos.x << ", " << (uint32_t)pos.y << std::endl;
    ss << "VEL: " << (int32_t)vel.x << ", " << (int32_t)vel.y << std::endl;
    //std::cout << ss.str() << std::endl;
    TextInfo1 = sf::Text();
    TextInfo1.setFont(*pFont);
    TextInfo1.setString(ss.str());
    TextInfo1.setCharacterSize(20);
    TextInfo1.setColor(sf::Color::Red);
    TextInfo1.setPosition(15, 15);

    backDrop.setSize(vec::VECTOR2(250.f, 100.f));
    backDrop.setFillColor(sf::Color(0, 0, 0, 128));
    backDrop.setPosition(5, 5);
}

void Hud::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    target.draw(backDrop, states);
    target.draw(TextInfo1, states);
}

void Hud::cleanup()
{

}

}