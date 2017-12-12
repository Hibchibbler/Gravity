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
    Keypress(sf::Keyboard::Key k, float r, float a, bool repeat)
    {
        str = 0.f;
        pre = cur = exp = false;
        elp = sf::Time::Zero;
        key = k;
        dur = r;
        amp = a;
        rpt = repeat;
    }
    float               str; // This is the pressure of the key press
    bool                pre; // previous state
    bool                cur; // current state
    bool                exp;
    sf::Time            elp; //how long it was held
    float               dur; // Even if held, longer, this will be the maximum duration reported.
    float               amp; // Amplitude; This is the max power of a key press
    sf::Keyboard::Key   key;
    vec::VECTOR2        nml;
    bool                rpt;
};
class MouseKeyboard
{
public:
    typedef void(*KeyPressed)(Keypress & kp, void* ud);
    typedef void(*KeyHeld)(Keypress & kp, void* ud);
    typedef void(*KeyReleased)(Keypress & kp, void* ud);

    void registerKeypress(sf::Keyboard::Key k, float range, float ampl, bool repeat);
    void updateKeypress(Keypress & glank, sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released);
    void doKeyboard(sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released);
    void doMouse(sf::Time elapsed);

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
