#include "server_quest.h"
#include <cstdlib> // for rand and srand

QuestGame::QuestGame()
{
    user_ = Cursor();
    user_.setType(CursorType());

    srand(time(nullptr));
    int fillAmount = 18 + (rand() % 8); // i.e., from 18 to 25
    grid_.fill(fillAmount);
    quest_ = rand() % 100 + 1;
}

std::string QuestGame::gridString() const
{
    return grid_.toString(user_.xS(), user_.yS(), user_.xE(), user_.yE());
}

std::string QuestGame::questGrid() const
{
    return grid_.toString();
}

int QuestGame::nextHp() const
{
    auto v1 = grid_.getCell(user_.xS(), user_.yS());
    auto v2 = grid_.getCell(user_.xE(), user_.yE());
    int nextHp = hp_;

    if (!hpAdd_)
        nextHp += hpAddAmount_;

    if (v1.has_value() && v2.has_value())
        nextHp -= abs(v1.value() - v2.value());

    return nextHp;
}

std::string QuestGame::endStatus() const
{
    if (hp_ <= 0)
        return "no-hp";

    if (grid_.contRemaining() > 1)
        return "none";

    // If the optional is empty, the grid_ is lost. Since quest + 1 is
    // returned, and quest +1 != quest, it is in fact lost.
    else if (grid_.getCell(0, 9).value_or(quest_ + 1) == quest_)
        return "victory";

    return "no-match";
}

void QuestGame::processAction(const std::string action)
{
    if (action == "Move-up")
    {
        user_.updateCursor(0, -1);
        user_.modOnGrid(grid_);
    }
    else if (action == "Move-down")
    {
        user_.updateCursor(0, 1);
        user_.modOnGrid(grid_);
    }
    else if (action == "Move-left")
    {
        user_.updateCursor(-1, 0);
        user_.modOnGrid(grid_);
    }
    else if (action == "Move-right")
    {
        user_.updateCursor(1, 0);
        user_.modOnGrid(grid_);
    }
    else if (action == "Rotate-left")
    {
        user_.rotateLeft();
        user_.modOnGrid(grid_);
    }
    else if (action == "Rotate-right")
    {
        user_.rotateRight();
        user_.modOnGrid(grid_);
    }
    else
    {
        auto hpDiff = grid_.applyAction(action, user_.xS(), user_.yS(), user_.xE(), user_.yE());
        if (hpDiff.has_value())
        {
            int add = 0;
            hpAdd_ = !hpAdd_;
            if (hpAdd_)
                add = hpAddAmount_;
            hp_ += add - hpDiff.value();
        }
    }
}