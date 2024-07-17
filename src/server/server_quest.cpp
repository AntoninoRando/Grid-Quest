#include "server_quest.h"
#include <cstdlib> // for rand and srand

QuestGame::QuestGame()
{
    user_ = Cursor();
    user_.setType(CursorType());

    int fillAmount = 18 + (rand() % 8); // i.e., from 18 to 25
    // int fillAmount = 2;
    grid_.fill(fillAmount);
    srand(time(nullptr));
    quest_ = rand() % 100 + 1;
    // quest = grid.getCell(0, 9).value() + grid.getCell(1, 9).value();
    // Redis::get() << "quest-start 1 "
    //              << "quest-grid " << grid.toString() << " "
    //              << "quest-goal " << quest;
    // Redis::get().push();
}

bool QuestGame::isEnd()
{
    return grid_.contRemaining() == 1 || hp_ <= 0;
}

std::string QuestGame::gridString() const
{
    return grid_.toString(user_.xS(), user_.yS(), user_.xE(), user_.yE());
}

void QuestGame::processAction(const std::string action)
{
    if (action == "move-up")
    {
        user_.updateCursor(0, -1);
        user_.modOnGrid(grid_);
    }
    else if (action == "move-down")
    {
        user_.updateCursor(0, 1);
        user_.modOnGrid(grid_);
    }
    else if (action == "move-left")
    {
        user_.updateCursor(-1, 0);
        user_.modOnGrid(grid_);
    }
    else if (action == "move-right")
    {
        user_.updateCursor(1, 0);
        user_.modOnGrid(grid_);
    }
    else if (action == "rotate-left")
    {
        user_.rotateLeft();
        user_.modOnGrid(grid_);
    }
    else if (action == "rotate-right")
    {
        user_.rotateRight();
        user_.modOnGrid(grid_);
    }
    else if (action == "quest-quit")
    {
        // Redis::get() << "input 27 "              // Input key
        //              << "action quest-quit "     // Input action
        //              << "quest-hp " << hp << " " // HP when quest ended
        //              << "quest-end quit";        // Quest end reason
        // Redis::get().push();
        // updateDB();
        // context_->transitionTo(new Menu);
        // return;
    }
    else
    {
        auto hpDiff = grid_.applyAction(action, user_.xS(), user_.yS(), user_.xE(), user_.yE());
        if (hpDiff.has_value())
        {
            int diff = hpDiff.value();
            int add = 0;
            hpAdd_ = !hpAdd_;
            if (hpAdd_)
                add = hpAddAmount_;
            hp_ += add - diff;
            // Redis::get() << "hp " << hp << " "
            //              << "gain " << add << " "
            //              << "loose " << diff;
            // Redis::get().push();
        }

        if (isEnd())
        {
            if (hp_ <= 0)
            {
                // Redis::get() << "quest-hp " << hp << " "
                //              << "quest-end no-hp";
                // Redis::get().push();
                // updateDB();
                // context_->transitionTo(new Defeat);
            }
            // If the optional is empty, the grid_ is lost. Since quest + 1 is
            // returned, and quest +1 != quest, it is in fact lost.
            else if (grid_.getCell(0, 9).value_or(quest_ + 1) == quest_)
            {
                // Redis::get() << "quest-result " << grid_.getCell(0, 9).value() << " "
                //              << "quest-hp " << hp << " "
                //              << "quest-end victory";
                // Redis::get().push();
                // updateDB();
                // context_->transitionTo(new Victory);
            }
            else
            {
                // Redis::get() << "quest-result " << grid_.getCell(0, 9).value() << " "
                //              << "quest-hp " << hp << " "
                //              << "quest-end no-match";
                // Redis::get().push();
                // updateDB();
                // context_->transitionTo(new Defeat);
            }
        }
        return;
    }
}