#ifndef GRID_H_
#define GRID_H_

#include <iostream>
#include <optional>

/**
 * @brief The grid of numbers the player have to reduce to a single number that
 * matches the quest in order to win. Each number can only has a value between
 * -999 to 999.
 */
class Grid
{
    int lPadding = 2;
    int rPadding = 2;
    int cellSize = 1;
    int lefts_ = 0;

    /**
     * @brief The actual grid of number as a 10x10 matrix of integers.
     */
    std::optional<int> grid[10][10];

    /**
     * @brief Fill a gap (hole) in the Grid by letting fall down every cell
     * above the hole, then shifting left every cell next to the new gaps born
     * after the fall-down. This shifting process occurs from top to bottom.
     * 
     * @param x The x coordinate (column) of the hole.
     * @param y The y coordinate (row) of the hole.
     */
    void adjustHole(int x, int y);

public:
    void show(int, int, int, int) const;

    /**
     * @brief Put exactly `amount` random values from 0 to 9 into the cells of
     * the grid, starting from the bottom-left corner and moving forward.
     *
     * Negative `amount` values are considered as 0; value greater than 100 are
     * considered as 100.
     *
     * @param amount The number of cells of the grid that will contain a number.
     */
    void fill(int amount = 100);

    /**
     * @brief Reduce the grid if the input correspond to an operation between
     * valid cells.
     * 
     * @return An optional describing the difference between the cells' values,
     * if an operation was performed on them.
     */
    std::optional<int> applyInput(char, int, int, int, int);

    /**
     * @brief Same as applyInput, but uses the string representation of the
     * operation instead of the char for that operation.
     */
    std::optional<int> applyAction(std::string action, int, int, int, int);

    /**
     * @brief Counts how many non-empty cells are left in the grid.
     * 
     * @return The number of non-empty cells of the grid.
     */
    int contRemaining() const;

    /**
     * @brief Get the cell value at coordinates (x, y).
     *
     * @param x The x coordinate of the cell to get (0 is leftmost).
     * @param y The y coordinate of the cell to get (0 is topmost).
     * @return std::optional<int> An optional describing the value of the cell
     * to get. If the cell is empty, so the optional will be.
     */
    std::optional<int> getCell(int x, int y) const;

    /**
     * @brief Put a value into the cell at coordinates (x, y).
     * 
     * @param x The x coordinate of the cell to set (i.e., its column).
     * @param y The y coordinate of the cell to set (i.e., its row).
     * @param value The value to put inside the cell.
     */
    void setCell(int x, int y, int value);

    /**
     * @brief Return a string representation of the grid.
     * 
     * The representation of the grid lists every cell, left-to-right and 
     * bottom-to-top, with a dot "." between each cell and two dots ".." between
     * each row.
     * 
     * @return std::string The string representation of the grid.
     */
    std::string toString() const;

    /**
     * @brief Return a string representation of the grid considering where the 
     * cursor is positioned.
     * 
     * The representation of the grid lists every cell, left-to-right and 
     * bottom-to-top, with
     * 
     * - a dot `.` between each cell;
     * 
     * - two dots `:` between each row;
     * 
     * - a star `*` before the cell where the cursor start; at `(xS, yS)`;
     * 
     * - a greater sign `>` before the cell where the cursor ends; at `(xE, yE)`.
     * 
     * @param xS The x coordinate of the cursor start.
     * @param yS The y coordinate of the cursor start.
     * @param xE The x coordinate of the cursor end.
     * @param yE The y coordinate of the cursor end.
     * @return std::string The string representation of the grid.
     */
    std::string toString(int xS, int yS, int xE, int yE) const;

    /**
     * @brief Perform positive module operator between x, y and xMin, yMin
     * respectively, where xMin is the maximum column with a value at row y, and
     * yMin is the maximum row with a value at column x.
     */
    std::tuple<int, int, int, int> modCursor(int xS, int yS, int xE, int yE) const;
};

#endif