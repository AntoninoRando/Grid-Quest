#ifndef SERVER_QUEST_H_
#define SERVER_QUEST_H_

#include "cursor.h"
#include "grid.h"

/**
 * @brief A quest the user is playing.
 */
class QuestGame
{
    Cursor user_;
    Grid   grid_;
    int    quest_;
    int    hp_          = 10;
    int    hpAddAmount_ = 5;
    bool   hpAdd_       = true;

public:
    QuestGame();

    int         quest()       const { return quest_;                      }
    int         hp()          const { return hp_;                         }
    int         remaining()   const { return grid_.contRemaining();       }
    int         finalResult() const { return grid_.getCell(0, 9).value(); }
    int         nextHp()      const;
    std::string endStatus()   const;

    void        processAction(const std::string action);
    std::string gridString()  const;
    std::string questGrid()   const;
};

#endif