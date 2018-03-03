///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#ifndef HUD_H_
#define HUD_H_

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Time.hpp>
#include "GravityCommon/Vector2.h"
#include <list>
namespace bali
{

class Hud
{
public:
    void initialize(sf::Font* font);
    void update(
        sf::Time        elapsed,
        vec::VECTOR2    pos,
        vec::VECTOR2    vel,
        float           angle,
        float           zoom
    );
    void draw(sf::RenderTarget & target, sf::RenderStates states) const;
    void cleanup();

    sf::Font*           pFont;
    sf::Text            TextInfo1;
    sf::RectangleShape  backDrop;
    sf::RenderTexture   HudRenderTexture;
    std::list<float>  fpsHistory;
    std::list<float>  velHistory;
};

}

#endif

