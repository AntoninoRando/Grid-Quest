#include "state.h"
#include <cassert>
#include <conio.h>

void Menu::highlightOption(int option) const
{
    option %= sizeof(options) / sizeof(std::string);
    setCursorPosition(1, option + 2); // 2 is the offset of the first option
    std::cout << "   " << "\u001b[" + SEL_COL + "m" << " "
              << options[option]
              << " " << "\u001b[0m\u001b[" + BG_COL + "m";
}

void Menu::resetOption(int option)
{
    setCursorPosition(1, currentOption + 2); // 2 is the offset of the first option
    std::cout << options[currentOption] << "         ";
}

void Menu::setup()
{
    clearConsole();
    std::string profile = CURRENT_PROFILE;
    std::cout << " " << profile << "\n\n";

    for (const auto option : options)
        std::cout << " " << option << '\n';
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
        currentOption -= 1;
    else if (input == MOVE_DOWN)
        currentOption += 1;
    else if (input == ENTER)
    {
        if (options[currentOption] == "Play")
            context_->transitionTo(new Quest);
        else if (options[currentOption] == "Settings")
            context_->transitionTo(new Settings);
        else if (options[currentOption] == "Exit")
            context_->transitionTo(new Bye);
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
        std::cout << sections[i]->GetName() << std::string(8, ' ');
        offset += sections[i]->GetName().size() + 8;
    }

    std::cout << '\n';
    for (int i = 0; i < 50; i++)
        std::cout << "—";
    std::cout << '\n';
}

void Settings::show() const
{
    highlightSection(currentSection);
    setCursorPosition(0, 3);
    clearConsole(0, 3);

    std::cout << "\u001b[" + BG_COL + (!selected ? ";2" : "") + "m";
    for (auto c : settings[currentSection])
        std::cout << c->GetName() << ": " << c->GetValue() << "\n";
    std::cout << "\u001b[0m\u001b[" + BG_COL + "m";

    if (selectedSetting.has_value())
        highlightSetting(currentSetting, *selectedSetting.value());
}

void Settings::processInput(char input)
{
    // Reset current section highlight
    setCursorPosition(sectionsCursorOffset[currentSection], 1);
    std::cout << sections[currentSection]->GetName();

    if (selected)
    {
        if (input == ESC)
        {
            selected = false;
            selectedSetting = {};
        }
        else if (input == MOVE_DOWN && currentSetting + 1 < settings[currentSection].size())
        {
            ++selectedSetting.value();
            ++currentSetting;
        }
        else if (input == MOVE_UP && currentSetting - 1 >= 0)
        {
            --selectedSetting.value();
            --currentSetting;
        }
        else if (input == ENTER)
        {
            Setting *setting = *selectedSetting.value();
            int offset = (setting->GetName() + ": " + setting->GetValue()).size();
            setCursorPosition(offset + 1, currentSetting + 3);
            std::cout << ">> ";
            setCursorPosition(offset + 4, currentSetting + 3);
            setting->ChangeWithInput();
        }
        return;
    }

    if (input == MOVE_LEFT)
        currentSection -= 1;
    else if (input == MOVE_RIGHT)
        currentSection += 1;
    else if (input == ENTER)
    {
        selected = true;
        selectedSetting = settings[currentSection].begin();
        currentSetting = 0;
        return;
    }
    else if (input == ESC)
    {
        context_->transitionTo(new Menu);
        return;
    }
    currentSection = posMod(currentSection, numberOfSections);
}

void Settings::highlightSection(int section) const
{
    assert(section >= 0 && section <= numberOfSections);

    setCursorPosition(sectionsCursorOffset[section], 1);
    std::cout << "\u001b[" + SEL_COL + "m"
              << sections[section]->GetName()
              << "\u001b[0m\u001b[" + BG_COL + "m";
}
void Settings::highlightSetting(int ord, Setting *setting) const
{
    setCursorPosition(0, ord + 3);
    std::cout << "\u001b[" + SEL_COL + "m";
    std::cout << setting->GetName() + ": " + setting->GetValue();
    std::cout << "\u001b[0m\u001b[" + BG_COL + "m";
}