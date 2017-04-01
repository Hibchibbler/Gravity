#ifndef GameStage_h_
#define GameStage_h_

#include <SFML/Graphics.hpp>
#include <memory>


namespace bali
{
    class GameContext;
    class Game;

    class Stage
    {
    public:
        typedef std::shared_ptr<Stage> Ptr;
    public:
        Stage(GameContext* ctx);

        virtual ~Stage();
        virtual uint32_t initialize() = 0;
        virtual uint32_t doRemoteEvent() = 0;
        virtual uint32_t doWindowEvent(sf::Event & event) = 0;
        virtual uint32_t doLocalInputs() = 0;
        virtual uint32_t doUpdate() = 0;
        virtual uint32_t doDraw() = 0;
        virtual uint32_t cleanup() = 0;
    public:
        GameContext* getContext() { return _ctx; }
        bool isInitialized() { return _isInit; }
        bool isDone() { return _isDone; }
    protected:
        void initialized() { _isInit = true; }
        void done() { _isDone = true; }
        bool _isDone;
        bool _isInit;

        // "weak" reference to context
        GameContext* _ctx;
    };

};


#endif