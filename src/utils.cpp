#include "utils.h"
#include <iostream>
#include <windows.h>

void setCursorPosition(int column, int row)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = {(SHORT)column, (SHORT)row};
    SetConsoleCursorPosition(hOut, coord);
}

void clearConsole()
{
    HANDLE hOut;
    COORD Position;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    Position.X = 0;
    Position.Y = 0;
    SetConsoleCursorPosition(hOut, Position);
}