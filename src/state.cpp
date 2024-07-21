#include "state.h"
#include "utils.h"
#include "monitors.h"
#include <cstdlib> // for rand and srand
#include <iostream>
#include <optional>
#include <chrono>
#include "monitors/sessionTrack.cpp"
#include "monitors/questAnalyze.cpp"

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
    Redis::get() << "enter-state " << state_->getName() << " "
                 << "time(ms):state-show " << showTime;
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

    // We trim the stream at the end of the code to ensure that the stream is
    // trimmed at a point that won't affect the Monitors.
    StreamParser::runMonitors({}, 0);

    exit(0); //@TODO: replace this with a more elegant solution
}

void Victory::show() const{ std::cout << "You won!\nPress any key to continue..."; }

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

Quest::Quest()
{
    name_ = "Quest";
    replyCheck_ = 1;
    end_ = false;

    // Obtaining the cells colors requires to look in the settings. Since they
    // can't change during a quest, we cache these values inside these variables
    // at the start of every quest.
    evenCellColor = ECELLS_COL;
    oddCellColor = OCELLS_COL;
    primaryCellColor = PCELL_COL;
    secondaryCellColor = SCELL_COL;
    colorReset = COL_RESET;

    Redis::get("gridquest:inputs") << "action Start-quest";
    Redis::get("gridquest:inputs").push();

    // Redis::get() << "quest-start 1 "
    //              << "quest-grid " << grid.toString() << " "
    //              << "quest-goal " << quest;
    // Redis::get().push();

    auto questReply = (redisReply *)Redis::get().runNoFree(("GET gridquest:quest"));
    quest_ = questReply->str;
    freeReplyObject(questReply);
}

void Quest::show() const
{
    std::string gridString;
    std::string currentHp;
    std::string nextHp;
    std::string remaining;

    auto gridStringReply = (redisReply *)Redis::get().runNoFree(("GET gridquest:gridString"));
    auto hpReply = (redisReply *)Redis::get().runNoFree(("GET gridquest:hp"));
    auto nextHpReply = (redisReply *)Redis::get().runNoFree(("GET gridquest:nextHp"));
    auto remainingReply = (redisReply *)Redis::get().runNoFree(("GET gridquest:remaining"));
    gridString = gridStringReply->str;
    currentHp = hpReply->str;
    nextHp = nextHpReply->str;
    remaining = remainingReply->str;
    freeReplyObject(gridStringReply);
    freeReplyObject(hpReply);
    freeReplyObject(nextHpReply);
    freeReplyObject(remainingReply);

    clearConsole();

    int cellWidth = 5; // Max num is 999, cell is " XXX ", " XX  ", or " X   "
    bool evenCell = true;
    bool cursorIn = false;
    std::string color = evenCellColor;

    for (auto c : gridString)
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

    auto hpString = currentHp;
    int currentHPLength = 0;

    if (nextHp != "")
    {
        hpString.append(" -> ");
        hpString.append((std::stoi(currentHp) > std::stoi(nextHp)) ? "\033[31m" : "\033[32m");
        hpString.append(nextHp);
        currentHPLength = -5; // Removing the length of the unicode "\033..."
    }

    int maxHPLength = 9;
    int currentHpLength = currentHPLength + hpString.length();
    int padding = std::max(maxHPLength - currentHpLength, 0);

    std::cout << "\n\n"
              << "QUEST: " << quest_
              << "  |  HP: " << hpString << colorReset << std::string(padding, ' ')
              << "  |  REMAINING: " << remaining;
}

void Quest::processInput(char input)
{
    std::string action = GlobalSettings::getActionOfKey(input);
    Redis::get("gridquest:inputs") << "input " << input << " action " << action;
    Redis::get("gridquest:inputs").push();

    // Wait for the server to process input
    int serverCheck = -1;
    while (serverCheck <= replyCheck_ - pingTolerance)
    {
        auto serverReply = (redisReply *)Redis::get().runNoFree("GET gridquest:replyCheck");
        serverCheck = std::stoi(serverReply->str);
        freeReplyObject(serverReply);
    }
    replyCheck_ = serverCheck;

    if (action == "Quit-quest" || action == "Drop-quest")
        context_->transitionTo(new Menu);

    auto reply = (redisReply *)Redis::get().runNoFree("GET gridquest:endStatus");    
    std::string endStatus = reply->str;
    freeReplyObject(reply);

    if (endStatus == "defeat")
    {
        end_ = true;
        context_->transitionTo(new Defeat);
    }
    else if (endStatus == "victory")
    {
        end_ = true;
        context_->transitionTo(new Victory);
    }
}

void Opening::show() const
{
    Redis::get() << "game-start 1";
    Redis::get().push();
    std::cout << "GRID QUEST";
    context_->transitionTo(new Menu);
}
