#ifndef SERVER_QUEST_H_
#define SERVER_QUEST_H_

#include "cursor.h"
#include "grid.h"
#include "utils.h"

/**
 * @brief A quest the user is playing.
 */
class QuestGame
{
    Cursor user_;
    Grid grid_;
    int quest_;
    int hp_ = 10;
    int hpAddAmount_ = 5;
    bool hpAdd_ = true;
    bool isEnd();

public:
    QuestGame();

    int quest() const { return quest_; }
    int hp() const { return hp_; }
    int remaining() const { return grid_.contRemaining(); }
    int nextHp() const;

    void processAction(const std::string action);
    std::string gridString() const;
};

#endif