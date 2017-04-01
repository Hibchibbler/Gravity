#ifndef Game_h_
#define Game_h_

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "GameContext.h"

namespace bali {

    //Game is a list of Stage's.
    //You can think of Game as a Flip book,
    //and a Stage as a page in the Flip book.
    //Game is meant to be derived. It supplies the basic infrastructure
    //to transition GameStages.
    class Stage;
    class Game
    {
    public:
        Game();
        virtual ~Game();

        virtual uint32_t initialize();
        virtual uint32_t doProcessing();
        virtual uint32_t cleanup();

        bool isDone() { return _isdone; }
        GameContext ctx;
    protected:
        std::shared_ptr<Stage> getCurrentStage();
        void add(std::shared_ptr<Stage> stage);
        bool nextStage();

    private:
        uint32_t getCurrentStageIndex() { return _curStageIndex; }
        uint32_t _curStageIndex;
        std::vector<std::shared_ptr<Stage>> gameStages;
        bool _isdone;
        void done(bool d=true) { _isdone = d; }

    };
};


#endif