#include "cursor.h"
#include "grid.h"

#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define ROTATE_LEFT 'q'
#define ROTATE_RIGHT 'e'
#define ENTER '\r'

class Context;
class Bye;
class Victory;
class Defeat;
class Quest;
class Menu;

class State
{
protected:
    Context *context_;

public:
    virtual void processInput(char) = 0;
    virtual void show() = 0;
    void setContext(Context *context)
    {
        this->context_ = context;
    }
};

class Context
{
    State *state_;

public:
    void transitionTo(State *state)
    {
        this->state_ = state;
        this->state_->setContext(this);
    }
    void show()
    {
        system("CLS");
        this->state_->show();
    }
    void processInput(char input)
    {
        this->state_->processInput(input);
    }
};

class Bye : public State
{
public:
    Bye()
    {
        system("CLS");
        std::cout << "Bye!";
        exit(0);
    }
    void show() override {}
    void processInput(char input) override {}
};

class Victory : public State
{
public:
    void show() override
    {
        std::cout << "You won!" << std::endl
                  << "Press any key to continue...";
    }
    void processInput(char) override;
};

class Defeat : public State
{
public:
    void show() override
    {
        std::cout << "You lost" << std::endl
                  << "Press any key to continue...";
    }
    void processInput(char) override;
};

class Quest : public State
{
    Cursor user = Cursor();
    Grid grid;
    int quest;
    bool isEnd() { return grid.contRemaining() == 1; }

public:
    Quest()
    {
        user.setType(CursorType());
        grid.fill(0.6);
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> cent(0, 100);
        quest = cent(rng);
    }

    void show() override
    {
        grid.show(user.xS(), user.yS(), user.xE(), user.yE());
        std::cout << std::endl
                  << std::endl
                  << "QUEST: " << quest << std::endl;
    }
    void processInput(char input) override
    {
        switch (input)
        {
        case KEY_UP:
            user.updateCursor(0, -1);
            break;
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
        case ENTER:
            context_->transitionTo(new Bye);
            return;
        default:
            PlaySound(TEXT("assets\\lock.wav"), NULL, SND_FILENAME | SND_ASYNC);
            grid.applyInput(input, user.xS(), user.yS(), user.xE(), user.yE());
            break;
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
};

class Menu : public State
{
    int currentOption = 0;
    std::string options[3] = {"Play", "Settings", "Exit"};

public:
    Menu()
    {
        system("CLS");
        std::cout << "GRID QUEST";
        PlaySound(TEXT("assets\\opening.wav"), NULL, SND_SYNC);
    }
    void show() override
    {
        for (size_t i = 0; i < 3; i++)
        {
            if (currentOption == i)
            {
                std::cout << "   \u001b[7m ";
            }
            std::cout << options[i];
            if (currentOption == i)
            {
                std::cout << " \u001b[0m";
            }
            std::cout << std::endl;
        }
    }
    void processInput(char input) override
    {
        switch (input)
        {
        case KEY_UP:
            currentOption -= 1;
            PlaySound(TEXT("assets\\sweep.wav"), NULL, SND_FILENAME | SND_ASYNC);
            break;
        case KEY_DOWN:
            currentOption += 1;
            PlaySound(TEXT("assets\\sweep.wav"), NULL, SND_FILENAME | SND_ASYNC);
            break;
        case ENTER:
            PlaySound(TEXT("assets\\lock.wav"), NULL, SND_FILENAME | SND_ASYNC);
            if (options[currentOption] == "Play")
            {
                context_->transitionTo(new Quest);
                return;
            }
            context_->transitionTo(new Bye);
            return;
        }
        currentOption = (currentOption % 3 + 3) % 3;
    }
};

void Victory::processInput(char input)
{
    context_->transitionTo(new Menu);
}

void Defeat::processInput(char input)
{
    context_->transitionTo(new Menu);
}