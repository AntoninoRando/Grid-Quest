#include "utils.h"
#include "monitors.h"
#include <iostream>
#include <chrono>
#include <sstream>

/// @brief Set the cursors position on the console.
/// @param column The x coordinate in which the cursor will be placed. Starts
/// at 0 on the left and increases towards right.
/// @param row The y coordinate in which the cursor will be places. Starts at
/// 0 on the top and increases towards the bottom.
void setCursorPosition(int column, int row)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = {(SHORT)column, (SHORT)row};
    SetConsoleCursorPosition(hOut, coord);
}

/// @brief Clear the entire console.
void clearConsole(SHORT column, SHORT row)
{
    auto start = std::chrono::system_clock::now();

    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {column, row};

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
        return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
        return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
            hStdOut,
            (TCHAR)' ',
            cellCount,
            homeCoords,
            &count))
        return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
            hStdOut,
            csbi.wAttributes,
            cellCount,
            homeCoords,
            &count))
        return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);

    auto end = std::chrono::system_clock::now();
    auto clearTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Redis::get() << "cclear_time " << clearTime << "ms";
    Redis::get().push();
}

int posMod(int value, int module)
{
    return (value % module + module) % module;
}
