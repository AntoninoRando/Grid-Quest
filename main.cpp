#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <random>
#include <array>
#include "cursor.h"

// We use wasd for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN
#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define ENTER '\r'
#define ADD '+'
#define SUB '-'
#define DIV '/'
#define MUL '*'
#define EXP '^'

using namespace std;
using Scheme = std::array<std::array<int, 10>, 10>;

void showScheme(Scheme scheme, int x1, int y1, int x2, int y2)
{
    system("CLS");
    for (int row = 0; row < 10; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if ((row == y1 && col == x1) || (row == y2 && col == x2))
            {
                cout << "\u001b[40;1m";
            }

            if (scheme[row][col] == -1)
            {
                cout << ' ';
            }
            else
            {
                cout << scheme[row][col];
            }
            cout << "\u001b[0m ";
        }
        cout << endl
             << flush;
    }
}

Scheme createScheme(float emptiness = 0.1)
{
    Scheme scheme;
    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            scheme[x][y] = -1;
        }
    }

    for (int x = 0; x < 10; x++)
    {
        int skip = 0;
        for (int y = 0; y < 10; y++)
        {
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist9(0, 9);
            std::uniform_int_distribution<std::mt19937::result_type> empt(0, 100);

            if (empt(rng) < emptiness * 100)
            {
                if (skip == 0)
                {
                    skip += 1;
                    continue;
                }
                break;
            }

            scheme[x][y] = dist9(rng);
            skip += 1;
        }
    }
    return scheme;
}

Scheme applyInput(Scheme scheme, char input, int x1, int y1, int x2, int y2)
{
    int v1 = scheme[y1][x1];
    int v2 = scheme[y2][x2];
    if (v1 == -1 || v2 == -1)
    {
        return scheme;
    }

    int r = v1;
    switch (input)
    {
    case ADD:
        r = v1 + v2;
        break;
    case SUB:
        r = v1 - v2;
        break;
    case MUL:
        r = v1 * v2;
        break;
    default:
        break;
    }
    scheme[y1][x1] = r;

    while (x2 < 9 && scheme[y1][x2] != -1)
    {
        scheme[y1][x2] = scheme[y1][x2 + 1];
        x2++;
    }
    if (x2 == 9)
    {
        scheme[y1][x2] = -1;
    }

    return scheme;
}

int main()
{
    // Serve per visualizzare i caratteri speciali su vs code.
    system("chcp 65001");

    Cursor user = Cursor(10, 10);
    Scheme scheme = createScheme();

    int input = 0;
    while (input != ENTER)
    {
        showScheme(scheme, user.xS(), user.yS(), user.xE(), user.yE());

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
        default:
            scheme = applyInput(scheme, input, user.xS(), user.yS(), user.xE(), user.yE());
            break;
        }
    }
}