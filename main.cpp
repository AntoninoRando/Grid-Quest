#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <random>
#include <array>
#include <windows.h>
#include <mmsystem.h>

#include "state.h"

// We use wasd for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN

using namespace std;
#pragma comment(lib, "winmm.lib")

int main()
{
    PlaySound(TEXT("assets\\opening.wav"), NULL, SND_ASYNC);
    Context game;
    game.transitionTo(new Menu);
    game.show();

    // Serve per visualizzare i caratteri speciali su vs code.
    system("chcp 65001");

    // Cursor user = Cursor();
    // user.setType(CursorType());
    // Grid Grid;
    // Grid.fill(0.6);

    // std::random_device dev;
    // std::mt19937 rng(dev());
    // std::uniform_int_distribution<std::mt19937::result_type> cent(0, 100);
    // int objective = cent(rng);

    char input = 0;
    while (true)
    {

        // Grid.show(user.xS(), user.yS(), user.xE(), user.yE());
        // cout << endl
        //      << endl
        //      << "OBJECTIVE: " << objective << endl;
        game.show();
        input = _getch();
        PlaySound(TEXT("assets\\tap.wav"), NULL, SND_ASYNC);
        game.processInput(input);
        // switch (input)
        // {
        // case KEY_UP:
        // {
        //     user.updateCursor(0, -1);
        //     break;
        // }
        // case KEY_DOWN:
        // {
        //     user.updateCursor(0, 1);
        //     break;
        // }
        // case KEY_LEFT:
        // {
        //     user.updateCursor(-1, 0);
        //     break;
        // }
        // case KEY_RIGHT:
        // {
        //     user.updateCursor(1, 0);
        //     break;
        // }
        // case ROTATE_LEFT:
        // {
        //     user.rotateLeft();
        //     break;
        // }
        // case ROTATE_RIGHT:
        // {
        //     user.rotateRight();
        //     break;
        // }
        // default:
        //     Grid.applyInput(input, user.xS(), user.yS(), user.xE(), user.yE());
        //     break;
        // }
    }

    // if (Grid.grid[9][0].value() == objective)
    // {
    //     cout << "VICTORY";
    // }
    // else
    // {
    //     cout << "DEFEAT";
    // }
}