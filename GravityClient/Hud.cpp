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
    fpsHistory.push_front((1.0f / elapsed.asSeconds()));
    if (fpsHistory.size() > 300)
    {
        fpsHistory.pop_back();
    }
    float fps = 0.f;
    for (auto k = fpsHistory.begin(); k != fpsHistory.end(); k++)
    {
        fps += *k;
    }
    fps /= fpsHistory.size();


    velHistory.push_front(vec::mag(vel));
    if (velHistory.size() > 100)
    {
        velHistory.pop_back();
    }
    float velp = 0.f;
    for (auto k = velHistory.begin(); k != velHistory.end(); k++)
    {
        velp += *k;
    }
    velp /= velHistory.size();

    std::stringstream ss;
    ss << "FPS: " << (uint32_t)fps << std::endl;
    ss << "POS: " << (uint32_t)pos.x << ", " << (uint32_t)pos.y << std::endl;
    ss << "VEL: " << (int32_t)velp << std::endl;
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