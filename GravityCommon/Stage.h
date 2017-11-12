///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameStage_h_
#define GameStage_h_

#include <SFML/Window.hpp>
#include <memory>
#include "Context.h"


namespace bali
{
    //class Game;

    class Stage
    {
    public:
        typedef std::shared_ptr<Stage> ShPtr;
    public:
        Stage();

        virtual ~Stage();
        virtual uint32_t initialize(Context::Ptr c) = 0;
        virtual uint32_t doRemoteEvent(Context::Ptr c) = 0;
        virtual uint32_t doWindowEvent(Context::Ptr c, sf::Event & event) = 0;
        virtual uint32_t doLocalInputs(Context::Ptr c) = 0;
        virtual uint32_t doUpdate(Context::Ptr c) = 0;
        virtual uint32_t doDraw(Context::Ptr c) = 0;
        virtual uint32_t cleanup(Context::Ptr c) = 0;
    public:
        bool isInitialized() { return mIsInit; }
        bool isDone() { return mIsDone; }
    protected:
        void initialized() { mIsInit = true; }
        void done() { mIsDone = true; }
        bool mIsDone;
        bool mIsInit;
    };

};


#endif