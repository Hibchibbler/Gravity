#ifndef MOUSEKEYBOARD_H_
#define MOUSEKEYBOARD_H_


#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <map>
#include "Vector2.h"
//#include "Player.h"
#include "ConfigLoader.h"

/*
 Use the keyboard.config.txt file to map
 keyboard&mouse to game-actions.
 The first keyboard key is q=0x10, and subsequent keys are monotonically increasing from there.
 The first mouse button is Left=0x0, and subsequent mouse button values are monotonically increasing from there
 until the mouse button maximum value of 0x0F is reached. However, as of now, there are only 5 Mouse buttons used.

 the possible keyboard keys you can map are:
 q=0x10, w=0x11, e=0x12
 a=0x13, s=0x14, d=0x15
 z=0x16, x=0x17, c=0x18

 and the possible mouse buttons you can map are:
 left=0x0,  middle=0x1, right=0x2,
 button1=0x3, button2=0x4

 P.S. - this route was chosen to unify the keyboard and mouse.
*/
#define KEY_BASE    0x10
#define MOUSE_BASE  0x00


/*
We offer this:
Keyboard keys, and mouse buttons are treated homogoneously.
We'll call them buttons.
A button is associated with a keyboard key, or mouse button.

A button can be pressed, held, and released.
a button can be configured to expire after a timeout.
an expired button press acts as if the button were released, when in fact it has not.
A button can be configured to detect a second press within a window of time, and trigger a dbl click.

Our scenario:
buttons associated with lateral movements, do not expire.
 i.e. - walk laterally
buttons associated with lateral movements are double click capable.
 i.e. - dash laterally
button associated with jump, does expire
 i.e. - jump a little, or jump a lot. but, after a while, will automatically release.


*/

namespace bali {
class Keypress;
typedef void(*KeyPressedEvent)(Keypress & kp, void* ud);
typedef void(*KeyDblPressedEvent)(Keypress & kp, void* ud);
typedef void(*KeyHeldEvent)(Keypress & kp, void* ud);
typedef void(*KeyReleasedEvent)(Keypress & kp, void* ud);

class Keypress
{
public:

    Keypress(
    );
    
    Keypress(
        uint32_t cc,
        sf::Keyboard::Key k, 
        float duration,
        KeyPressedEvent pressed,
        KeyDblPressedEvent dblpressed,
        KeyHeldEvent held,
        KeyReleasedEvent released
    );

    Keypress(
        uint32_t cc, 
        sf::Mouse::Button b,
        float duration,
        KeyPressedEvent pressed,
        KeyHeldEvent held,
        KeyReleasedEvent released
    );

    bool                pre; // previous state
    bool                cur; // current state
    bool                exp;
    sf::Time            elp; //how long it was held
    sf::Time            tor;
    float               dur; // Even if held, longer, this will be the maximum duration reported.

    sf::Vector2f        nml; //TODO: this needs to be a generic datum...
    void*               user;

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
    void registerKeypress(
        uint32_t configCode,
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

    void updateElapsed(Keypress & kp, sf::Time elapsed);
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

    static bool isConfigCodeForMouse(uint32_t cc)
    {
        if (cc < KEY_BASE) {
            return true;
        }
        return false;
    }

    Keypress & getKeypress(sf::Keyboard::Key k);
    sf::Time totalTime;
    sf::Time clock;
//private:
    std::map<uint32_t, Keypress> keyStates;
};




}

#endif
