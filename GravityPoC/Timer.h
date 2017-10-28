#ifndef BALI_TIMER_H_
#define BALI_TIMER_H_

#include <SFML/System/Clock.hpp>
#include <stdint.h>

namespace bali
{
    namespace Timer
    {
        class Timer
        {
        public:
            typedef void(*TimerFunction)(void*);
            enum Type
            {
                ONESHOT,
                PERIODIC
            };

            void initialize(Type t, uint32_t e, TimerFunction tf, void* u)
            {//If One-SHot, then e is the wait time, in milliseconds, prior to
             // when the function call will occur.
             //If Periodic, then e is the time, in milliseconds, between function calls.
                timerFunction = tf;
                elapseTarget =sf::milliseconds(e);
                type = t;
                user = u;
                clock.restart();
                started = false;
            }

            void start()
            {
                clock.restart();
                started = true;
            }

            void stop()
            {
                started = false;
            }

            void update()
            {
                if (started && clock.getElapsedTime() > elapseTarget)
                {
                    timerFunction(user);
                    if (type == ONESHOT)
                    {
                        stop();
                    }
                    else
                    {
                        start();
                    }
                }
            }

            sf::Time getElapedTime()
            {
                return clock.getElapsedTime();
            }

        private:
            bool started;
            void* user;
            Type type;
            sf::Clock clock;
            sf::Time elapseTarget;
            TimerFunction timerFunction;
        };
    }
}


#endif