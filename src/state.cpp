#include "state.h"

void State::setup()
{
    system("cls");
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
    system("cls");
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
    return grid.contRemaining() == 1;
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
    std::cout << "\n\nQUEST: " << quest;
}

void Quest::processInput(char input)
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
        // PlaySound(TEXT("assets\\lock.wav"), NULL, SND_FILENAME | SND_ASYNC);
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

void Menu::highlightOption(int option)
{
    setCursorPosition(0, currentOption);
    std::cout << "   \u001b[7m "
              << options[currentOption]
              << " \u001b[0m";
}

void Menu::resetOption(int option)
{
    setCursorPosition(0, currentOption);
    std::cout << options[currentOption] << "         ";
}

void Menu::setup()
{
    system("cls");
    for (size_t i = 0; i < 3; i++)
    {
        std::cout << options[i] << '\n';
    }
}

void Menu::show()
{
    highlightOption(currentOption);
}

void Menu::processInput(char input)
{
    resetOption(currentOption);
    switch (input)
    {
    case KEY_UP:
        currentOption -= 1;
        PlaySound(TEXT("assets\\sw.wav"), NULL, SND_FILENAME | SND_ASYNC);
        break;
    case KEY_DOWN:
        currentOption += 1;
        PlaySound(TEXT("assets\\sw.wav"), NULL, SND_FILENAME | SND_ASYNC);
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

void Opening::show()
{
    std::cout << "GRID QUEST";
    // PlaySound(TEXT("assets\\opening.wav"), NULL, SND_SYNC);
    context_->transitionTo(new Menu);
}

void Opening::processInput(char input) {}