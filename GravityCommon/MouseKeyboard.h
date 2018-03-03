#ifndef MOUSEKEYBOARD_H_
#define MOUSEKEYBOARD_H_


#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <map>
#include "Vector2.h"
#include "Player.h"
#include "ConfigLoader.h"

#define KEY_BASE    0x10
#define MOUSE_BASE  0x00


namespace bali {
class Keypress;
typedef void(*KeyPressedEvent)(Keypress & kp, void* ud);
typedef void(*KeyDblPressedEvent)(Keypress & kp, void* ud);
typedef void(*KeyHeldEvent)(Keypress & kp, void* ud);
typedef void(*KeyReleasedEvent)(Keypress & kp, void* ud);

class MouseKeyboard;
class Keypress
{
    friend class MousKeyboard;
public:

    Keypress() {}
    Keypress(uint32_t cc,
             sf::Keyboard::Key k, 
             float duration,
             KeyPressedEvent pressed,
             KeyDblPressedEvent dblpressed,
             KeyHeldEvent held,
             KeyReleasedEvent released
            )
    {
        this->pressed = pressed;
        this->dblpressed = dblpressed;
        this->held = held;
        this->released = released;
        pre = cur = exp = false;
        elp = sf::Time::Zero;
        nml = vec::Zero();
        mouse = false;
        key = k;
        dur = duration;
        this->cc = cc;
    }

    Keypress(uint32_t cc, 
             sf::Mouse::Button b,
             float duration,
             KeyPressedEvent pressed,
             KeyHeldEvent held,
             KeyReleasedEvent released
            )
    {
        this->pressed = pressed;
        this->dblpressed = nullptr;
        this->held = held;
        this->released = released;
        pre = cur = exp = false;
        elp = sf::Time::Zero;
        nml = vec::Zero();
        mouse = true;
        btn = b;
        dur = duration;
        this->cc = cc;
        tor = sf::Time::Zero;
    }

    bool                pre; // previous state
    bool                cur; // current state
    bool                exp;
    sf::Time            elp; //how long it was held
    sf::Time            tor;
    float               dur; // Even if held, longer, this will be the maximum duration reported.
    vec::VECTOR2        nml;

    uint32_t            cc;
    bool                mouse;
    sf::Keyboard::Key   key;
    sf::Mouse::Button   btn;

    KeyPressedEvent     pressed;
    KeyDblPressedEvent  dblpressed;
    KeyHeldEvent        held;
    KeyReleasedEvent    released;
};
class MouseKeyboard
{
public:

    void setUserData(void* ud);
    void registerKeypress(uint32_t configCode,
                          float duration,
                          KeyPressedEvent pressed,
                          KeyDblPressedEvent dbpressed,
                          KeyHeldEvent held,
                          KeyReleasedEvent released
                         );

    void updateKeypress(Keypress & glank);

    void Initialize();
    void Update(sf::Time elapsed);
    void Cleanup();
    void* ud;

    void MouseKeyboard::updateElapsed(Keypress & kp, sf::Time elapsed);
    static sf::Keyboard::Key getKeyFromConfigCode(uint32_t cc)
    {
        switch (cc)
        {
        case KEY_BASE + 0:
            return sf::Keyboard::Q;
        case KEY_BASE + 1:
            return sf::Keyboard::W;
        case KEY_BASE + 2:
            return sf::Keyboard::E;
        case KEY_BASE + 3:
            return sf::Keyboard::A;
        case KEY_BASE + 4:
            return sf::Keyboard::S;
        case KEY_BASE + 5:
            return sf::Keyboard::D;
        case KEY_BASE + 6:
            return sf::Keyboard::Z;
        case KEY_BASE + 7:
            return sf::Keyboard::X;
        case KEY_BASE + 8:
            return sf::Keyboard::C;
        case KEY_BASE + 0x20:
            return sf::Keyboard::Space;
        }
    }
    static uint32_t getConfigCodeFromKey(sf::Keyboard::Key k)
    {
        switch (k)
        {
        case sf::Keyboard::Space:
            return 0x20;
        case sf::Keyboard::D:
            return 0x44;
        case sf::Keyboard::A:
            return 0x41;
        case sf::Keyboard::E:
            return 0x45;
        case sf::Keyboard::Q:
            return 0x51;
        }
    }
    static sf::Mouse::Button getButtonFromConfigCode(uint32_t cc)
    {
        switch (cc)
        {
        case MOUSE_BASE + 0:
            return sf::Mouse::Button::Left;
        case MOUSE_BASE + 1:
            return sf::Mouse::Button::Right;
        case MOUSE_BASE + 2:
            return sf::Mouse::Button::Middle;
        case MOUSE_BASE + 3:
            return sf::Mouse::Button::XButton1;
        case MOUSE_BASE + 4:
            return sf::Mouse::Button::XButton2;
        }
    }
    static uint32_t getConfigCodeFromButton(sf::Mouse::Button b)
    {
        switch (b)
        {
        case sf::Mouse::Button::Left:
            return 0x0;
        case sf::Mouse::Button::Middle:
            return 0x1;
        case sf::Mouse::Button::Right:
            return 0x2;
        }
    }
    static bool isConfigCodeForMouse(uint32_t cc)
    {
        if (cc < KEY_BASE) {
            return true;
        }
        return false;
    }

    Keypress & getKeypress(sf::Keyboard::Key k);
    Keypress lastKeypress;
    sf::Time totalTime;
    sf::Time clock;
//private:
    std::map<uint32_t, Keypress> keyStates;
};




}

#endif
