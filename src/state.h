#include "cursor.h"
#include "grid.h"
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

class State
{
protected:
    Context *context_;

public:
    virtual void processInput(char) = 0;
    virtual void show() = 0;
    virtual void setup();
    void setContext(Context *);
};

class Context
{
    State *state_;

public:
    void transitionTo(State *state);
    void show();
    void processInput(char input);
};

class Bye : public State
{
public:
    void setup() override;
    void show() override;
    void processInput(char input) override;
};

class Victory : public State
{
public:
    void show() override;
    void processInput(char) override;
};

class Defeat : public State
{
public:
    void show() override;
    void processInput(char) override;
};

class Quest : public State
{
    Cursor user;
    Grid grid;
    int quest;
    bool isEnd();

public:
    Quest();

    void show() override;
    void processInput(char input) override;
};

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

class Opening : public State
{
public:
    void show() override;
    void processInput(char input) override;
};