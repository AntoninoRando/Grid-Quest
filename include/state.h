#ifndef STATE_H_
#define STATE_H_

#include "settings.h"
#include "cursor.h"
#include "grid.h"
#include "utils.h"

using opSet = std::optional<std::list<Setting *>::const_iterator>;

class Context;

/**
 * @brief A state of the game (e.g., Menu, Profile, New Game). That is, a
 * recurring situation in the game that contains different visible informations
 * the user can interact with.
 */
class State
{
protected:
    /**
     * @brief The current game this scene is displayed in.
     */
    Context *context_;

    /**
     * @brief The name of the state. It SHOULD NOT contain spaces.
     */
    std::string name_;

public:
    std::string getName() { return name_; }
    void setContext(Context *context) { this->context_ = context; }

    /**
     * @brief Initialize this game state to be shown.
     */
    virtual void setup();

    /**
     * @brief Modify this states based on what the user prompted.
     *
     * @param input The user prompted char.
     */
    virtual void processInput(char input) {}

    /**
     * @brief Display this state on the current context (i.e. on the current
     * game).
     */
    virtual void show() const {}
};

/**
 * @brief The current game the user is playing.
 */
class Context
{
    /**
     * @brief The current state in which the game is in.
     */
    State *state_;

public:
    /**
     * @brief Change the game state.
     *
     * @param state The new state in which the game will enter.
     */
    void transitionTo(State *state);

    /**
     * @brief Display the game to the user.
     */
    void show() const;

    /**
     * @brief Make the user interact with the game.
     *
     * @param input The key prompted by the user.
     */
    void processInput(char input);
};

/**
 * @brief The last state of the game, shown before the game closes.
 */
class Bye : public State
{
public:
    Bye() { name_ = "Bye"; }
    void setup() override;
};

/**
 * @brief Game state that appear after winning a game.
 */
class Victory : public State
{
public:
    Victory() { name_ = "Victory"; }
    void show() const override;
    void processInput(char) override;
};

/**
 * @brief Game state that appear after losing a game.
 */
class Defeat : public State
{
public:
    Defeat() { name_ = "Defeat"; }
    void show() const override;
    void processInput(char) override;
};

/**
 * @brief The main game state in which the actual game is played.
 */
class Quest : public State
{
    int replyCheck_ = 1;
    bool end_ = false;

    std::string quest_;
    std::string hp_;
    std::string nextHp_;
    std::string remaining_;
    std::string gridString_;
    std::string finalResult_;
    
    std::string evenCellColor = "0";
    std::string oddCellColor = "0";
    std::string primaryCellColor = "0";
    std::string secondaryCellColor = "0";
    std::string colorReset = "\033[0m";

public:
    Quest();
    int  pingTolerance = 0;

    bool isEnd() const { return end_; }
    void show()  const override;
    void processInput(char input) override;
};

/**
 * @brief Game state that allows to navigate through other game states (e.g.
 * start a new quest).
 */
class Menu : public State
{
    int currentOption = 0;
    std::string options[3] = {"Play", "Settings", "Exit"};

    void highlightOption(int option) const;
    void resetOption(int option);

public:
    Menu() { name_ = "Menu"; }
    void setup() override;
    void show() const override;
    void processInput(char input) override;
};

/**
 * @brief Game state that shows the current settings configurations and allows
 * to change them.
 */
class Settings : public State
{
    int currentSection = 0;
    int currentSectionPosition = 0;
    Category *sections[3] = {
        GlobalSettings::controls,
        GlobalSettings::graphic,
        GlobalSettings::profileInfo};
    int sectionsCursorOffset[3] = {};
    int numberOfSections = 3;
    bool selected = false;
    int currentSetting = 0;
    std::list<Setting *> settings[3] = {
        GlobalSettings::controls->GetChildrenList(),
        GlobalSettings::graphic->GetChildrenList(),
        GlobalSettings::profileInfo->GetChildrenList()};
    opSet selectedSetting = {};
    std::string error = "";

    void highlightSection(int option) const;
    void highlightSetting(int ord, Setting *setting) const;

public:
    Settings() { name_ = "Settings"; }
    void setup() override;
    void show() const override;
    void processInput(char input) override;
};

/**
 * @brief The first state of the game, shown as soon as the game starts.
 */
class Opening : public State
{
public:
    Opening() { name_ = "Opening"; }
    void show() const override;
};

#endif