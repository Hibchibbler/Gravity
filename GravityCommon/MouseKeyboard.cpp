///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "MouseKeyboard.h"
#include "Vector2.h"
#include "Physics.h"
#include <assert.h>

using namespace bali;

Keypress & MouseKeyboard::getKeypress(sf::Keyboard::Key k)
{
    return keyStates[k];
}
void MouseKeyboard::Initialize()
{

}
void MouseKeyboard::Cleanup()
{

}
void MouseKeyboard::Update(sf::Time elapsed)
{
    this->totalTime += elapsed;
    this->clock += elapsed;

    // We want to update elapsed regardless whether we
    // poll input or not.
    for (auto i = keyStates.begin(); i != keyStates.end(); i++)
    {
        updateElapsed(i->second, elapsed);
    }

    if (this->totalTime.asMilliseconds() > 40.f)
    {
        this->totalTime = sf::Time::Zero;
        for (auto i = keyStates.begin(); i != keyStates.end(); i++)
        {
            updateKeypress(i->second);
        }
    }
}
void MouseKeyboard::setUserData(void* ud)
{
    this->ud = ud;
}


void MouseKeyboard::registerKeypress(
    uint32_t configCode, 
    float duration, 
    KeyPressedEvent pressed, 
    KeyDblPressedEvent dblpressed,
    KeyHeldEvent held, 
    KeyReleasedEvent released
    )
{
    if (MouseKeyboard::isConfigCodeForMouse(configCode))
    {
        sf::Mouse::Button b = getButtonFromConfigCode(configCode);
        keyStates[configCode] =
            Keypress(configCode,
                     b,
                     duration,
                     pressed,
                     held,
                     released);
    }
    else
    {
        sf::Keyboard::Key k = getKeyFromConfigCode(configCode);
        keyStates[configCode] =
            Keypress(configCode,
                     k,
                     duration,
                     pressed,
                     dblpressed,
                     held,
                     released);
    }
    
}


void MouseKeyboard::updateElapsed(
    Keypress & kp, 
    sf::Time elapsed
    )
{
    kp.elp += elapsed;
}

void MouseKeyboard::updateKeypress(Keypress & kp)
{
    assert(kp.pressed!= nullptr);
    assert(kp.held != nullptr);
    assert(kp.released != nullptr);

    kp.pre = kp.cur;
    if (kp.mouse)
    {
        kp.cur = sf::Mouse::isButtonPressed(kp.btn);
    }
    else
    {
        kp.cur = sf::Keyboard::isKeyPressed(kp.key);
    }
    if (kp.pre == true && kp.cur == false)
    {
        // Released
        if (!kp.exp)
        {
            // We do not want to call release, if we already expired
            kp.tor = clock;// Time of Release
            kp.elp = sf::Time::Zero;
            kp.exp = false;
            kp.released(kp, ud);
        }
    }
    else if (kp.pre == false && kp.cur == true)
    {
        // Pressed
        kp.elp = sf::Time::Zero;
        kp.exp = false;

        // If dblpressed available, and it has been less 180 ms since
        // this key was last released, then we have a double press.
        if (kp.dblpressed != nullptr && 
            (clock - kp.tor).asMilliseconds() < 180)
        {
            kp.dblpressed(kp, ud);
        }
        else
        {
            kp.pressed(kp, ud);
        }
        kp.held(kp, ud);
    }
    else if (kp.pre == true && kp.cur == true)
    {
        //
        // Still held
        // We only care if we haven't expired.
        //
        if (!kp.exp)
        {
            //kp.elp += elapsed;
            float ms = kp.elp.asMilliseconds();
            if (kp.dur == 0.f || ms < kp.dur)
            {
                // And holding
                kp.held(kp, ud);
            }
            else
            {
                //
                // Held it too long. Expired.
                //
                kp.elp = sf::Time::Zero;
                kp.exp = true;
                std::cout << "expired ";
                kp.released(kp, ud);

            }
        }
    }
    else
    {
        // Still released
    }
}


