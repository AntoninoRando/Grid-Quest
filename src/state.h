#include "cursor.cpp"
#include "grid.cpp"
#include <cstdlib> // for rand and srand
#include <ctime>   // for time

#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define ROTATE_LEFT 'q'
#define ROTATE_RIGHT 'e'
#define ENTER '\r'

#pragma comment(lib, "winmm.lib")

class Context;

/// @brief A state of the game (e.g., Menu, Profile, New Game).
class State
{
protected:
    Context *context_;

public:
    /// @brief Modify this state based on the user input.
    virtual void processInput(char) = 0;
    /// @brief Display this state on the console.
    virtual void show() = 0;
    virtual void setup();
    /// @brief Change the current context.  
    void setContext(Context *);
};

/// @brief The current game state.
class Context
{
    State *state_;

public:
    void transitionTo(State *state);
    void show();
    void processInput(char input);
};

/// @brief The last state of the game, shown before the game closes.
class Bye : public State
{
public:
    void setup() override;
    void show() override;
    void processInput(char input) override;
};

/// @brief Game state that shows after winning a game.
class Victory : public State
{
public:
    void show() override;
    void processInput(char) override;
};

/// @brief Game state that shows after losing a game.
class Defeat : public State
{
public:
    void show() override;
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
    void show() override;
    void processInput(char input) override;
};

/// @brief Game state in which is possible to navigate through other game states
/// and start a new game.
class Menu : public State
{
    int currentOption = 0;
    std::string options[3] = {"Play", "Settings", "Exit"};

    void highlightOption(int option);
    void resetOption(int option);

public:
    void setup() override;
    void show() override;
    void processInput(char input) override;
};

/// @brief The first state of the game, shown when the game opens.
class Opening : public State
{
public:
    void show() override;
    void processInput(char input) override;
};