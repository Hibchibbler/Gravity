#ifndef MOUSEKEYBOARD_H_
#define MOUSEKEYBOARD_H_

#include "GravityCommon/Context.h"
#include <SFML/System/Time.hpp>

namespace bali {
class KeyPress
{
public:
    sf::Keyboard::Key key;
    bool active;
    sf::Time duration;
};
class KeyPressWatcher
{
public:
    KeyPressWatcher();
    void update(std::vector<KeyPress> & keyPressReport);
    sf::Clock keyClock;
    std::map<sf::Keyboard::Key, KeyPress> keyPressMap;
};
class MouseKeyboard
{
public:
    static void doKeyboard(Context::Ptr ctx);
    static void doMouse(Context::Ptr ctx);
};
}

#endif
