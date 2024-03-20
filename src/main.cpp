#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <random>
#include <array>
#include <windows.h>
#include <mmsystem.h>

#include "state.cpp"

// We use wasd for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN

int main()
{
    Context game;
    game.transitionTo(new Opening);

    // Serve per visualizzare i caratteri speciali su console, come ad esempio
    // i colori.
    system("chcp 65001");

    while (true)
    {
        game.show();
        game.processInput(_getch());
    }
}