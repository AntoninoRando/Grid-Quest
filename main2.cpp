#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <random>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define ENTER '\r'
#define ADD '+'
#define SUB '-'
#define DIV '/'
#define MUL '*'
#define EXP '^'

using namespace std;

int scheme[10][10] =
    {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};

void showScheme(int x = -1, int y = -1, string dir = "lr")
{
    system("CLS");
    for (int row = 0; row < 10; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if ((row == y && col == x) || (dir == "lr" && row == y && col+1 == x))
            {
                cout << "\u001b[40;1m";
            }

            cout << scheme[row][col] << "\u001b[0m ";
        }
        cout << endl
             << flush;
    }
}

void gameLoop()
{
    while (true)
    {
        int x;
        cout << "Gioca una carta";
        cin >> x;
    }
}

int main()
{
    // Serve per visualizzare i caratteri speciali su vs code.
    system("chcp 65001");

    int x = 0;
    int y = 0;

    bool confirmed = false;
    while (!confirmed)
    {
        showScheme(x, y);

        switch (getch())
        {
        case KEY_UP:
        {
            y = (y-1) % 10;
            break;
        }
        case KEY_DOWN:
        {
            y = (y+1) % 10;
            break;
        }
        case KEY_LEFT:
        {
            x = (x-1) % 10;
            break;
        }
        case KEY_RIGHT:
        {
            x = (x+1) % 10;
            break;
        }
        // case ENTER:
        //     locked = 4 * y + x;
        //     confirmed = true;
        //     break;
        default:
            break;
        }
    }

    //gameLoop(p1, p2, p3, p4);
}