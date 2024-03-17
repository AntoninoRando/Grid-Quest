#include <iostream>
#include <windows.h>

/// @brief Set the cursor position in the console.
/// @param column The arrival column (0 is the first of the row, on the left).
/// @param row  The arrival row (0 is the first row, on top).
void setCursorPosition(int column, int row);

void clearConsole();