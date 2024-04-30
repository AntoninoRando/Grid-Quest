#ifndef GRID_H_
#define GRID_H_

#include <iostream>
#include <optional>

class Grid
{
    std::optional<int> grid[10][10];
    /// @brief Fill a gap (hole) in the Grid by letting fall down every cell
    /// above the hole, then shifting left every cell next to the new gaps born
    /// after the fall-down. This shifting process occurs from top to bottom.
    /// @param x The x coordinate (column) of the hole.
    /// @param y The y coordinate (row) of the hole.
    void adjustHole(int x, int y);
    /// @brief Perform positive module operator between x, y and xMin, yMin
    /// respectively, where xMin is the maximum column with a value at row y, and
    /// yMin is the maximum row with a value at column x.
    std::tuple<int, int, int, int> modCursor(int, int, int, int) const;
    int lPadding = 2;
    int rPadding = 2;
    int cellSize = 1;

public:
    void show(int, int, int, int) const;
    void fill(float);
    /// @brief Reduce the grid if the input correspond to an operation between
    /// valid cells.
    /// @return An optional describing the difference between the cells' values,
    /// if an operation was performed on them.
    std::optional<int> applyInput(char, int, int, int, int);
    /// @brief Counts how many non-empty cells are remained in the grid.
    /// @return
    int contRemaining() const;
    std::optional<int> getCell(int x, int y);
};

#endif