#include <iostream>
#include <windows.h>

/// @brief Set the cursor position in the console.
/// @param column The arrival column (0 is the first of the row, on the left).
/// @param row  The arrival row (0 is the first row, on top).
void setCursorPosition(int column, int row);

/// @brief Clear the entire console, starting from position (column, row)
void clearConsole(SHORT column = 0, SHORT row = 0);

/// @brief Perform the positive module operation between two integers.
/// @param value The value to modulate.
/// @param module The module.
/// @return  value % module (positive)
int posMod(int value, int module);