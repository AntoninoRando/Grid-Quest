#include "utils.h"
#include "monitors.h"
#include <iostream>
#include <chrono>
#include <sstream>

void setCursorPosition(int column, int row)
{
    std:: cout << "\033[" << row +1 << ";" << column+1 << "H";
}

void clearConsole(int column, int row)
{
    setCursorPosition(column, row);
    std::cout << std::string(2000, ' ');
    setCursorPosition(column, row);
}

int posMod(int value, int module)
{
    return (value % module + module) % module;
}
