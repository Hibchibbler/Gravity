///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <list>
namespace bali
{
    class Observer
    {
    public:
        Observer() = default;
        ~Observer() = default;

        virtual void onNotify(uint32_t e) = 0;

    };
    class Subscription
    {
    public:
        Observer* o;
        uint32_t e;
    };

    class Subject
    {
    protected:
        std::list<Subscription> subscriptions;

    public:
        Subject() = default;
        ~Subject() = default;
        virtual void subscribe(Subscription s)
        {
            subscriptions.push_back(s);
        }
        virtual void unsubscribe(Subscription s) 
        {
            for (auto s = subscriptions.begin(); s != subscriptions.end(); ++s)
            {
                
            }
        }
    };
}