#ifndef STATE_H_
#define STATE_H_

#include "settings.h"
#include "cursor.h"
#include "grid.h"
#include "utils.h"

using opSet = std::optional<std::list<Setting *>::const_iterator>;

class Context;

/// @brief A state of the game (e.g., Menu, Profile, New Game).
class State
{
protected:
    Context *context_;

public:
    /// @brief Change the current context.
    void setContext(Context *context)
    {
        this->context_ = context;
    }
    /// @brief Modify this state based on the user input.
    virtual void processInput(char) {}
    /// @brief Display this state on the console.
    virtual void show() const {}
    virtual void setup();
};

/// @brief The current game state.
class Context
{
    State *state_;

public:
    void transitionTo(State *state);
    void show() const;
    void processInput(char input);
};

/// @brief The last state of the game, shown before the game closes.
class Bye : public State
{
public:
    void setup() override;
};

/// @brief Game state that shows after winning a game.
class Victory : public State
{
public:
    void show() const override;
    void processInput(char) override;
};

/// @brief Game state that shows after losing a game.
class Defeat : public State
{
public:
    void show() const override;
    void processInput(char) override;
};

/// @brief The main game.
class Quest : public State
{
    Cursor user;
    Grid grid;
    int quest;
    int hp = 10;
    int hp_add_amount = 5;
    bool hp_add = true;
    bool isEnd();

public:
    Quest();
    void show() const override;
    void processInput(char input) override;
};

/// @brief Game state in which is possible to navigate through other game states
/// and start a new game.
class Menu : public State
{
    int currentOption = 0;
    std::string options[3] = {"Play", "Settings", "Exit"};

    void highlightOption(int option) const;
    void resetOption(int option);

public:
    void setup() override;
    void show() const override;
    void processInput(char input) override;
};

class Settings : public State
{
    int currentSection = 0;
    int currentSectionPosition = 0;
    Category *sections[2] = {
        GlobalSettings::controls,
        GlobalSettings::graphic};
    int sectionsCursorOffset[2] = {};
    int numberOfSections = 2;
    bool selected = false;
    int currentSetting = 0;
    std::list<Setting *> settings[2] = {
        GlobalSettings::controls->GetChildrenList(),
        GlobalSettings::graphic->GetChildrenList()};
    opSet selectedSetting = {};
    std::string error = "";

    void highlightSection(int option) const;
    void highlightSetting(int ord, Setting* setting) const;

public:
    void setup() override;
    void show() const override;
    void processInput(char input) override;
};

/// @brief The first state of the game, shown when the game opens.
class Opening : public State
{
public:
    void show() const override;
};

#endif