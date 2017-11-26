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
    Keypress(sf::Keyboard::Key k, float r, float a)
    {
        pre = cur = ign = false;
        elp = sf::Time::Zero;
        key = k;
        rng = r;
        amp = a;
    }
    float               str;

    bool                pre; // previous state
    bool                cur; // current state
    bool                ign;
    sf::Time            elp; //how long it was held
    float               rng; // Even if held, longer, this will be the maximum duration reported.
    float               amp; // Amplitude
    sf::Keyboard::Key   key;
    vec::VECTOR2        nml;
};
class MouseKeyboard
{
public:
    typedef void(*KeyPressed)(Keypress & kp, void* ud);
    typedef void(*KeyHeld)(Keypress & kp, void* ud);
    typedef void(*KeyReleased)(Keypress & kp, void* ud);
    typedef void(*KeyExpired)(Keypress & kp, void* ud);
    /*void initialize();*/
    void registerKeypress(sf::Keyboard::Key k, float range, float ampl);
    void updateRightLeftKeypress(Keypress & glank, sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released, KeyExpired expired);
    void updateJumpKeypress(Keypress & glank, sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released, KeyExpired expired);
    //void updateKeypress(Keypress & glank, sf::Time elapsed, float range, float ampl, bool & move);
    void doKeyboard(sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released, KeyExpired expired);
    void doMouse(sf::Time elapsed);

    Keypress & getKeypress(sf::Keyboard::Key k);
//private:
    std::map<sf::Keyboard::Key, Keypress> keyStates;
};




}

#endif
