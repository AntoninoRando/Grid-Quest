#include "state.h"
#include "utils.h"
#include "monitors.h"
#include <cstdlib> // for rand and srand
#include <iostream>
#include <optional>
#include <chrono>

void State::setup()
{
    clearConsole();
}

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
    Redis::get() << "time:state-show " << showTime << "ms";
    Redis::get().push();
}

void Context::processInput(char input)
{
    state_->processInput(input);
}

void Bye::setup()
{
    clearConsole();
    std::cout << "Bye!";
    Redis::get() << "game-end 1";
    Redis::get().push();
    exit(0);
}

void Victory::show() const
{
    std::cout << "You won!\n"
              << "Press any key to continue...";
}

void Victory::processInput(char input)
{
    context_->transitionTo(new Menu);
}

void Defeat::show() const
{
    std::cout << "You lost!\n"
              << "Press any key to continue...";
}

void Defeat::processInput(char input)
{
    context_->transitionTo(new Menu);
}

bool Quest::isEnd()
{
    return grid.contRemaining() == 1 || hp <= 0;
}

Quest::Quest()
{
    user = Cursor();
    user.setType(CursorType());
    grid.fill(0.8);
    srand(time(nullptr));
    quest = rand() % 100 + 1;
    Redis::get() << "quest-start 1";
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
        Redis::get() << "input 27 action quest-quit";
        Redis::get().push();
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
            Redis::get() << "hp " << hp << " gain " << add << " loose " << diff;
            Redis::get().push();
        }

        if (isEnd())
        {
            if (hp < 0)
            {
                Redis::get() << "quest-lost no-hp";
                Redis::get().push();
                context_->transitionTo(new Defeat);
            }
            // If the optional is empty, the grid is lost. Since quest + 1 is
            // returned, and quest +1 != quest, it is in fact lost.
            else if (grid.getCell(0, 9).value_or(quest + 1) == quest)
            {
                Redis::get() << "quest-won 1";
                Redis::get().push();
                context_->transitionTo(new Victory);
            }
            else
            {
                Redis::get() << "quest-lost no-match";
                Redis::get().push();
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
