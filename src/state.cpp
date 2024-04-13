#include "state.h"
#include "utils.h"
#include <cstdlib> // for rand and srand
#include <iostream>
#include <cassert>
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

void Menu::highlightOption(int option) const
{
    option %= sizeof(options) / sizeof(std::string);
    setCursorPosition(0, option);
    std::cout << "   " << "\u001b[" + SEL_COL + "m" << " "
              << options[option]
              << " " << "\u001b[0m\u001b[" + BG_COL + "m";
}

void Menu::resetOption(int option)
{
    setCursorPosition(0, currentOption);
    std::cout << options[currentOption] << "         ";
}

void Menu::setup()
{
    clearConsole();
    for (const auto option : options)
    {
        std::cout << option << '\n';
    }
    highlightOption(currentOption);
}

void Menu::show() const
{
    highlightOption(currentOption);
}

void Menu::processInput(char input)
{
    resetOption(currentOption);

    if (input == MOVE_UP)
    {
        currentOption -= 1;
    }
    else if (input == MOVE_DOWN)
    {
        currentOption += 1;
    }
    else if (input == ENTER)
    {
        if (options[currentOption] == "Play")
        {
            context_->transitionTo(new Quest);
            return;
        }
        if (options[currentOption] == "Settings")
        {
            context_->transitionTo(new Settings);
            return;
        }
        context_->transitionTo(new Bye);
        return;
    }
    currentOption = posMod(currentOption, 3);
}

void Settings::setup()
{
    clearConsole();

    std::cout << "<- (ESC)\n";
    int offset = 0;
    for (size_t i = 0; i < numberOfSections; i++)
    {
        sectionsCursorOffset[i] = offset;
        std::cout << sections[i]->getName() << std::string(8, ' ');
        offset += sections[i]->getName().size() + 8;
    }

    std::cout << '\n';
    for (int i = 0; i < 36; i++)
    {
        std::cout << "—";
    }
    std::cout << '\n';
}

void Settings::show() const
{
    highlightSection(currentSection);
    setCursorPosition(0, 3);
    clearConsole(0, 3);

std:
    std::string code = selected ? "0" : "38;5;240";
    std::cout << "\u001b[" + code + "m"
              << sections[currentSection]->ToString()
              << "\u001b[0m\u001b[" + BG_COL + "m";
}

void Settings::processInput(char input)
{
    // Reset current section highlight
    setCursorPosition(sectionsCursorOffset[currentSection], 1);
    std::cout << sections[currentSection]->getName();

    if (selected)
    {
        if (input == ESC)
        {
            selected = false;
            return;
        }
    }
    else
    {
        if (input == MOVE_LEFT)
            currentSection -= 1;
        else if (input == MOVE_RIGHT)
            currentSection += 1;
        else if (input == ENTER)
        {
            selected = true;
            return;
        }
        else if (input == ESC)
        {
            context_->transitionTo(new Menu);
            return;
        }
        currentSection = posMod(currentSection, numberOfSections);
    }
}

void Settings::highlightSection(int section) const
{
    assert(section >= 0 && section <= numberOfSections);

    setCursorPosition(sectionsCursorOffset[section], 1);
    std::cout << "\u001b[" + SEL_COL + "m"
              << sections[section]->getName()
              << "\u001b[0m\u001b[" + BG_COL + "m";
}

void Opening::show() const
{
    std::cout << "GRID QUEST";
    context_->transitionTo(new Menu);
}
