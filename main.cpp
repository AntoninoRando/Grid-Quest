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
    Context game;
    game.transitionTo(new Menu);
    game.show();

    // Serve per visualizzare i caratteri speciali su vs code.
    system("chcp 65001");

    char input = 0;
    while (true)
    {
        game.show();
        input = _getch();
        game.processInput(input);
    }
}