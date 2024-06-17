#ifndef UTILS_H_
#define UTILS_H_

#include <windows.h>

/**
 * @brief Set the cursors position on the terminal.
 * 
 * @param column The x coordinate in which the cursor will be placed. Starts
 * at 0 on the left and increases towards right.
 * @param row The y coordinate in which the cursor will be places. Starts at
 * 0 on the top and increases towards the bottom.
 */
void setCursorPosition(int column, int row);

/**
 * @brief Clear everything that is in the terminal after position (column, row).
 */
void clearConsole(SHORT column = 0, SHORT row = 0);

/**
 * @brief Perform the positive module operation between two integers.
 *
 * @param value The value to modulate.
 * @param module The module.
 * @return value % module (positive)
 */
int posMod(int value, int module);

#endif