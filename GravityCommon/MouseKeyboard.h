#ifndef MOUSEKEYBOARD_H_
#define MOUSEKEYBOARD_H_


#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <map>
#include "Vector2.h"
#include "Player.h"
#include "ConfigLoader.h"

namespace bali {

class MouseKeyboard;
class Keypress
{
    friend class MousKeyboard;
public:
    Keypress() {}
    Keypress(sf::Keyboard::Key k, float duration)
    {
        pre = cur = exp = false;
        elp = sf::Time::Zero;
        nml = vec::Zero();
        key = k;
        dur = duration;
    }

    bool                pre; // previous state
    bool                cur; // current state
    bool                exp;
    sf::Time            elp; //how long it was held
    float               dur; // Even if held, longer, this will be the maximum duration reported.
    sf::Keyboard::Key   key;
    vec::VECTOR2        nml;
};
class MouseKeyboard
{
public:
    typedef void(*KeyPressedEvent)(Keypress & kp, void* ud);
    typedef void(*KeyHeldEvent)(Keypress & kp, void* ud);
    typedef void(*KeyReleasedEvent)(Keypress & kp, void* ud);
    void registerKeypress(sf::Keyboard::Key k, float duration);
    void updateKeypress(Keypress & glank, sf::Time elapsed, void* ud, KeyPressedEvent pressed, KeyHeldEvent held, KeyReleasedEvent released);
    void doKeyboard(sf::Time elapsed, void* ud, KeyPressedEvent pressed, KeyHeldEvent held, KeyReleasedEvent released);
    void doMouse(sf::Time elapsed);
    void MouseKeyboard::updateElapsed(Keypress & kp, sf::Time elapsed);
    static sf::Keyboard::Key getKeyFromConfigCode(uint32_t cc)
    {
        switch (cc)
        {
        case 0x20:
            return sf::Keyboard::Space;
        case 0x44:
            return sf::Keyboard::D;
        case 0x41:
            return sf::Keyboard::A;
        }
    }

    Keypress & getKeypress(sf::Keyboard::Key k);
    Keypress lastKeypress;
    sf::Time totalTime;
//private:
    std::map<sf::Keyboard::Key, Keypress> keyStates;
};




}

#endif
