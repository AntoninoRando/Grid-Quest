#include "state.h"
#include "utils.h"
#include "monitors.h"
#include <cstdlib> // for rand and srand
#include <iostream>
#include <optional>
#include <chrono>
#include "monitors/sessionTrack.cpp"
#include "monitors/questAnalyze.cpp"

void State::setup() { clearConsole(); }

void Context::transitionTo(State *state)
{
    state_ = state;
    state_->setContext(this);
    state_->setup();
}
void Context::show() const
{
    auto start = std::chrono::system_clock::now();

    state_->show();
    std::cout.flush();

    auto end = std::chrono::system_clock::now();
    auto showTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Redis::get() << "enter-state " << state_->getName() << " "
                 << "time(ms):state-show " << showTime;
    Redis::get().push();
}

void Context::processInput(char input) { state_->processInput(input); }

void Bye::setup()
{
    clearConsole();
    std::cout << "Bye!";
    Redis::get() << "game-end 1";
    Redis::get().push();
    exit(0); //@TODO: replace this with a more elegant solution
}

void Victory::show() const { std::cout << "You won!\nPress any key to continue..."; }

void Victory::processInput(char input) { context_->transitionTo(new Menu); }

void Defeat::show() const { std::cout << "You lost!\nPress any key to continue..."; }

void Defeat::processInput(char input) { context_->transitionTo(new Menu); }

bool Quest::isEnd() { return grid.contRemaining() == 1 || hp <= 0; }

void updateDB()
{
    SessionTracker *st = new SessionTracker("postgresql://postgres:postgres@localhost/gridquest");
    QuestAnalyze *qa = new QuestAnalyze("postgresql://postgres:postgres@localhost/gridquest");
    StreamParser::runMonitors({st, qa});
    GlobalSettings::loadProfile();
}

Quest::Quest()
{
    name_ = "Quest";
    user = Cursor();
    user.setType(CursorType());

    int fillAmount = 18 + (rand() % 8); // i.e., from 18 to 25
    grid.fill(fillAmount);
    srand(time(nullptr));
    quest = rand() % 100 + 1;
    Redis::get() << "quest-start 1 "
                 << "quest-grid " << grid.toString() << " "
                 << "quest-goal " << quest;
    Redis::get().push();
}

void Quest::show() const
{
    grid.show(user.xS(), user.yS(), user.xE(), user.yE());
    std::cout << "\n\nQUEST: " << quest
              << "\t|\tHP: " << hp
              << "\t|\tREMAINING: " << grid.contRemaining();
}

void Quest::processInput(char input)
{
    std::string action;

    if (input == MOVE_UP)
    {
        action = "move-up";
        user.updateCursor(0, -1);
    }
    else if (input == MOVE_DOWN)
    {
        action = "move-down";
        user.updateCursor(0, 1);
    }
    else if (input == MOVE_LEFT)
    {
        action = "move-left";
        user.updateCursor(-1, 0);
    }
    else if (input == MOVE_RIGHT)
    {
        action = "move-right";
        user.updateCursor(1, 0);
    }
    else if (input == ROTATE_LEFT)
    {
        action = "rotate-left";
        user.rotateLeft();
    }
    else if (input == ROTATE_RIGHT)
    {
        action = "rotate-right";
        user.rotateRight();
    }
    else if (input == ESC)
    {
        Redis::get() << "input 27 "          // Input key
                     << "action quest-quit " // Input action
                     << "quest-end quit "    // Quest end reason
                     << "quest-hp " << hp;   // HP when quest ended
        Redis::get().push();
        updateDB();
        context_->transitionTo(new Menu);
        return;
    }
    else
    {
        auto hpDiff = grid.applyInput(input, user.xS(), user.yS(), user.xE(), user.yE());
        if (hpDiff.has_value())
        {
            int diff = hpDiff.value();
            int add = 0;
            hp_add = !hp_add;
            if (hp_add)
                add = hp_add_amount;
            hp += add - diff;
            Redis::get() << "hp " << hp << " "
                         << "gain " << add << " "
                         << "loose " << diff;
            Redis::get().push();
        }

        if (isEnd())
        {
            if (hp <= 0)
            {
                Redis::get() << "quest-end no-hp " // Quest end reason
                             << "quest-hp " << hp; // HP when quest ended
                Redis::get().push();
                updateDB();
                context_->transitionTo(new Defeat);
            }
            // If the optional is empty, the grid is lost. Since quest + 1 is
            // returned, and quest +1 != quest, it is in fact lost.
            else if (grid.getCell(0, 9).value_or(quest + 1) == quest)
            {
                Redis::get() << "quest-end victory " // Quest end reason
                             << "quest-hp " << hp;   // HP when quest ended
                Redis::get().push();
                updateDB();
                context_->transitionTo(new Victory);
            }
            else
            {
                Redis::get() << "quest-end no-match "    // Quest end reason
                             << "quest-hp " << hp << " " // HP when quest ended
                             << "quest-result " << grid.getCell(0, 9).value();
                Redis::get().push();
                updateDB();
                context_->transitionTo(new Defeat);
            }
        }
        return;
    }

    Redis::get() << "input " << input << " action " << action;
    Redis::get().push();
}

void Opening::show() const
{
    Redis::get() << "game-start 1";
    Redis::get().push();
    std::cout << "GRID QUEST";
    context_->transitionTo(new Menu);
}
