#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <random>
#include <array>
#include "cursor.h"
#include "scheme.h"

// We use wasd for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN
#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define ROTATE_LEFT 'q'
#define ROTATE_RIGHT 'e'
#define ENTER '\r'

using namespace std;

int main()
{
    // Serve per visualizzare i caratteri speciali su vs code.
    system("chcp 65001");
    Cursor user = Cursor(10, 10);
    user.setType(CursorType());
    Scheme scheme;
    scheme.fill(0.6);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> cent(0, 100);
    int objective = cent(rng);

    int input = 0;
    while (input != ENTER)
    {

        scheme.show(user.xS(), user.yS(), user.xE(), user.yE());
        cout << endl
             << endl
             << "OBJECTIVE: " << objective << endl;

        input = _getch();
        switch (input)
        {
        case KEY_UP:
        {
            user.updateCursor(0, -1);
            break;
        }
        case KEY_DOWN:
        {
            user.updateCursor(0, 1);
            break;
        }
        case KEY_LEFT:
        {
            user.updateCursor(-1, 0);
            break;
        }
        case KEY_RIGHT:
        {
            user.updateCursor(1, 0);
            break;
        }
        case ROTATE_LEFT:
        {
            user.rotateLeft();
            break;
        }
        case ROTATE_RIGHT:
        {
            user.rotateRight();
            break;
        }
        default:
            scheme.applyInput(input, user.xS(), user.yS(), user.xE(), user.yE());
            break;
        }
    }

    if (scheme[9][0].value() == objective)
    {
        cout << "VICTORY";
    }
    else
    {
        cout << "DEFEAT";
    }
}