#include "state.h"
#include "utils.h"
#include <cstdlib> // for rand and srand
#include <iostream>
#include <optional>

void State::setup()
{
    clearConsole();
}

void Context::transitionTo(State *state)
{
    this->state_ = state;
    this->state_->setContext(this);
    this->state_->setup();
}
void Context::show() const
{
    this->state_->show();
    std::cout.flush();
}

void Context::processInput(char input)
{
    this->state_->processInput(input);
}

void Bye::setup()
{
    clearConsole();
    std::cout << "Bye!";
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
    std::cout << "You lost\n"
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
    // Initialize the random seed based on the current time
    srand(time(nullptr));
    quest = rand() % 100 + 1;
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
    std::optional<int> hp_diff = std::optional<int>{};

    if (input == MOVE_UP)
        user.updateCursor(0, -1);
    else if (input == MOVE_DOWN)
        user.updateCursor(0, 1);
    else if (input == MOVE_LEFT)
        user.updateCursor(-1, 0);
    else if (input == MOVE_RIGHT)
        user.updateCursor(1, 0);
    else if (input == ROTATE_LEFT)
        user.rotateLeft();
    else if (input == ROTATE_RIGHT)
        user.rotateRight();
    else if (input == ESC)
    {
        context_->transitionTo(new Menu);
        return;
    }
    else
        hp_diff = grid.applyInput(input, user.xS(), user.yS(), user.xE(), user.yE());

    if (hp_diff.has_value())
    {
        hp -= hp_diff.value();
        hp_add = !hp_add;
        if (hp_add)
        {
            hp += hp_add_amount;
        }
    }

    if (isEnd())
    {
        // If the optional is empty, the grid is lost. Since quest + 1 is
        // returned, and quest +1 != quest, it is in fact lost.
        if (grid.getCell(0, 9).value_or(quest + 1) == quest)
        {
            context_->transitionTo(new Victory);
            return;
        }
        context_->transitionTo(new Defeat);
    }
}

void Opening::show() const
{
    std::cout << "GRID QUEST";
    context_->transitionTo(new Menu);
}
