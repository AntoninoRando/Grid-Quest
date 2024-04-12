#include "state.h"

void State::setup()
{
    clearConsole();
}

void State::setContext(Context *context)
{
    this->context_ = context;
}

void Context::transitionTo(State *state)
{
    this->state_ = state;
    this->state_->setContext(this);
    this->state_->setup();
}
void Context::show()
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
void Bye::show() {}

void Bye::processInput(char input) {}

void Victory::show()
{
    std::cout << "You won!\n"
              << "Press any key to continue...";
}

void Victory::processInput(char input)
{
    context_->transitionTo(new Menu);
}

void Defeat::show()
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

void Quest::show()
{
    grid.show(user.xS(), user.yS(), user.xE(), user.yE());
    std::cout << "\n\nQUEST: " << quest
              << "\t|\tHP: " << hp
              << "\t|\tREMAINING: " << grid.contRemaining();
}

void Quest::processInput(char input)
{
    optional<int> hp_diff = optional<int>{};

    if (input == GlobalSettings::getKey("Move up"))
    {
        user.updateCursor(0, -1);
    }
    switch (input)
    {
    case KEY_DOWN:
        user.updateCursor(0, 1);
        break;
    case KEY_LEFT:
        user.updateCursor(-1, 0);
        break;
    case KEY_RIGHT:
        user.updateCursor(1, 0);
        break;
    case ROTATE_LEFT:
        user.rotateLeft();
        break;
    case ROTATE_RIGHT:
        user.rotateRight();
        break;
    case ESC:
        context_->transitionTo(new Menu);
        return;
    default:
        hp_diff = grid.applyInput(input, user.xS(), user.yS(), user.xE(), user.yE());
        break;
    }

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

void Menu::highlightOption(int option)
{
    option %= sizeof(options) / sizeof(std::string);
    setCursorPosition(0, option);
    std::cout << "   " << "\u001b[7m" << " "
              << options[option]
              << " " << "\u001b[0m";
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

void Menu::show()
{
    highlightOption(currentOption);
}

void Menu::processInput(char input)
{
    resetOption(currentOption);
    
    if (input == GlobalSettings::getKey("Move up"))
    {
        currentOption -= 1;
    }
    switch (input)
    {
    case KEY_DOWN:
        currentOption += 1;
        break;
    case ENTER:
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
    parseSettings(sections[0], "savedSettings\\controls.txt");
    parseSettings(sections[1], "savedSettings\\graphic.txt");

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

void Settings::show()
{
    highlightSection(currentSection);
    setCursorPosition(0, 3);
    clearConsole(0, 3);

std:
    string code = selected ? "0" : "38;5;240";
    std::cout << "\u001b[" + code + "m"
              << sections[currentSection]->ToString()
              << "\u001b[0m";
}

void Settings::processInput(char input)
{
    // Reset current section highlight
    setCursorPosition(sectionsCursorOffset[currentSection], 1);
    std::cout << sections[currentSection]->getName();

    if (selected)
    {
        switch (input)
        {
        case KEY_LEFT:
            break;
        case KEY_RIGHT:
            break;
        case ESC:
            selected = false;
            return;
        }
    }
    else
    {
        switch (input)
        {
        case KEY_LEFT:
            currentSection -= 1;
            break;
        case KEY_RIGHT:
            currentSection += 1;
            break;
        case ENTER:
            selected = true;
            return;
        case ESC:
            context_->transitionTo(new Menu);
            return;
        }
        currentSection = posMod(currentSection, numberOfSections);
    }
}

void Settings::highlightSection(int section)
{
    assert(section >= 0 && section <= numberOfSections);

    setCursorPosition(sectionsCursorOffset[section], 1);
    std::cout << "\u001b[7m"
              << sections[section]->getName()
              << "\u001b[0m";
}

void Opening::show()
{
    std::cout << "GRID QUEST";
    context_->transitionTo(new Menu);
}

void Opening::processInput(char input)
{
}
