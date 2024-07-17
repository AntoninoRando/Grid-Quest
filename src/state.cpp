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

    // We trim the stream at the end of the code to ensure that the stream is
    // trimmed at a point that won't affect the Monitors.
    StreamParser::runMonitors({}, 0);

    exit(0); //@TODO: replace this with a more elegant solution
}

void Victory::show() const { std::cout << "You won!\nPress any key to continue..."; }

void Victory::processInput(char input) { context_->transitionTo(new Menu); }

void Defeat::show() const { std::cout << "You lost!\nPress any key to continue..."; }

void Defeat::processInput(char input) { context_->transitionTo(new Menu); }

void updateDB()
{
    SessionTracker *st = new SessionTracker("postgresql://postgres:postgres@localhost/gridquest");
    st->disablePrint();
    StreamParser::runMonitors({st});
    QuestAnalyze *qa = new QuestAnalyze("postgresql://postgres:postgres@localhost/gridquest");
    qa->disablePrint();
    StreamParser::runMonitors({qa});
    GlobalSettings::loadProfile();
}

bool Quest::isEnd() { return grid.contRemaining() == 1 || hp <= 0; }

Quest::Quest()
{
    name_ = "Quest";
    user = Cursor();
    user.setType(CursorType());

    // Obtaining the cells colors requires to look in the settings. Since they
    // can't change during a quest, we cache these values inside these variables
    // at the start of every quest.
    evenCellColor = ECELLS_COL;
    oddCellColor = OCELLS_COL;
    primaryCellColor = PCELL_COL;
    secondaryCellColor = SCELL_COL;
    colorReset = COL_RESET;

    int fillAmount = 18 + (rand() % 8); // i.e., from 18 to 25
    // int fillAmount = 2;
    grid.fill(fillAmount);
    srand(time(nullptr));
    quest = rand() % 100 + 1;
    // quest = grid.getCell(0, 9).value() + grid.getCell(1, 9).value();
    // Redis::get() << "quest-start 1 "
    //              << "quest-grid " << grid.toString() << " "
    //              << "quest-goal " << quest;
    // Redis::get().push();
}

void Quest::show() const
{    
    clearConsole();

    int cellWidth = 5; // Max num is 999, cell is " XXX ", " XX  ", or " X   "
    bool evenCell = true;
    bool cursorIn = false;
    std::string color = evenCellColor;

    for (auto c : gridString())
    {
        if (c == '*') // Next symbols are the numbers of the primary cell
        {
            color = primaryCellColor;
            cursorIn = true;
        }
        else if (c == '>') // Next symbols are the numbers of the secondary cell
        {
            color = secondaryCellColor;
            cursorIn = true;
        }
        else if (c == '.') // Previous symbols were the numbers of this cell
        {
            if (cellWidth == 5 && cursorIn)
            {
                std::cout << "\033[" << color << 'm' << 'X';
                cellWidth--;
            }

            // Fill the missing spaces of this cell, reset the color and also
            // change it, reset the cellWidth counter for the next cell.
            std::cout << std::string(cellWidth, ' ');
            std::cout << colorReset;
            evenCell = !evenCell;
            color = evenCell ? evenCellColor : oddCellColor;
            cellWidth = 5;
            cursorIn = false;
        }
        else if (c == ':')
        {
            evenCell = true;
            std::cout << '\n';
            cursorIn = false;
        }
        else
        {
            if (cellWidth == 5)
                std::cout << "\033[" + color + "m";
            std::cout << c;
            cellWidth--;
            cursorIn = false;
        }
    }
    
    std::cout << colorReset;

    auto hpString = std::to_string(hp);
    auto v1 = grid.getCell(user.xS(), user.yS());
    auto v2 = grid.getCell(user.xE(), user.yE());

    int currentHPLength = 0;

    if (v1.has_value() && v2.has_value())
    {
        int nextHp = hp - abs(v1.value() - v2.value()) + (!hp_add ? hp_add_amount : 0);
        hpString.append(" -> ");
        hpString.append((nextHp >= hp) ? "\033[32m" : "\033[31m");
        hpString.append(std::to_string(nextHp));
        currentHPLength = -5; // Removing the length of the unicode "\033..."
    }

    int maxHPLength = 9;
    int currentHpLength = currentHPLength + hpString.length();
    int padding = std::max(maxHPLength - currentHpLength, 0);

    std::cout << "\n\n"
              << "QUEST: " << quest << (quest >= 10 ? " " : "")
              << "  |  HP: " << hpString << colorReset << std::string(padding, ' ')
              << "  |  REMAINING: " << grid.contRemaining();
}

void Quest::processInput(char input)
{
    std::string action;

    if (input == MOVE_UP)
    {
        action = "move-up";
        user.updateCursor(0, -1);
        user.modOnGrid(grid);
    }
    else if (input == MOVE_DOWN)
    {
        action = "move-down";
        user.updateCursor(0, 1);
        user.modOnGrid(grid);
    }
    else if (input == MOVE_LEFT)
    {
        action = "move-left";
        user.updateCursor(-1, 0);
        user.modOnGrid(grid);
    }
    else if (input == MOVE_RIGHT)
    {
        action = "move-right";
        user.updateCursor(1, 0);
        user.modOnGrid(grid);
    }
    else if (input == ROTATE_LEFT)
    {
        action = "rotate-left";
        user.rotateLeft();
        user.modOnGrid(grid);
    }
    else if (input == ROTATE_RIGHT)
    {
        action = "rotate-right";
        user.rotateRight();
        user.modOnGrid(grid);
    }
    else if (input == ESC)
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
        auto hpDiff = grid.applyInput(input, user.xS(), user.yS(), user.xE(), user.yE());
        if (hpDiff.has_value())
        {
            int diff = hpDiff.value();
            int add = 0;
            hp_add = !hp_add;
            if (hp_add)
                add = hp_add_amount;
            hp += add - diff;
            // Redis::get() << "hp " << hp << " "
            //              << "gain " << add << " "
            //              << "loose " << diff;
            // Redis::get().push();
        }

        if (isEnd())
        {
            if (hp <= 0)
            {
                // Redis::get() << "quest-hp " << hp << " "
                //              << "quest-end no-hp";
                // Redis::get().push();
                // updateDB();
                // context_->transitionTo(new Defeat);
            }
            // If the optional is empty, the grid is lost. Since quest + 1 is
            // returned, and quest +1 != quest, it is in fact lost.
            else if (grid.getCell(0, 9).value_or(quest + 1) == quest)
            {
                // Redis::get() << "quest-result " << grid.getCell(0, 9).value() << " "
                //              << "quest-hp " << hp << " "
                //              << "quest-end victory";
                // Redis::get().push();
                // updateDB();
                // context_->transitionTo(new Victory);
            }
            else
            {
                // Redis::get() << "quest-result " << grid.getCell(0, 9).value() << " "
                //              << "quest-hp " << hp << " "
                //              << "quest-end no-match";
                // Redis::get().push();
                // updateDB();
                // context_->transitionTo(new Defeat);
            }
        }
        return;
    }

    Redis::get() << "input " << input << " action " << action;
    Redis::get().push();
}

std::string Quest::gridString() const
{
    return grid.toString(user.xS(), user.yS(), user.xE(), user.yE());
}

void Opening::show() const
{
    Redis::get() << "game-start 1";
    Redis::get().push();
    std::cout << "GRID QUEST";
    context_->transitionTo(new Menu);
}
