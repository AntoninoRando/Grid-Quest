#include "utils.cpp"
#include <iostream>
#include <optional>
#include <cassert>
#include <cstdlib> // for rand and srand
#include <ctime>   // for time

using std::cout, std::get, std::max, std::optional, std::tuple;

#define ADD '+'
#define SUB '-'
#define MUL '*'
#define MOD '%'
/// Merge two numbers: `A | B := AB`. If at least one of the two is negative, the
/// result is negative too.
#define MRG '|'
#define DIV '/'

class Grid
{
    optional<int> grid[10][10];
    /// @brief Fill a gap (hole) in the Grid by letting fall down every cell
    /// above the hole, then shifting left every cell next to the new gaps born
    /// after the fall-down. This shifting process occurs from top to bottom.
    /// @param x The x coordinate (column) of the hole.
    /// @param y The y coordinate (row) of the hole.
    void ajustHole(int x, int y);

    /// @brief Perform positive module operator between x, y and xMin, yMin
    /// respectively, where xMin is the maximum column with a value at row y, and
    /// yMin is the maximum row with a value at column x.
    tuple<int, int, int, int> modCursor(int, int, int, int);

public:
    void show(int, int, int, int);
    void fill(float);
    /// @brief Reduce the grid if the input correspond to an operation between
    /// valid cells.
    /// @return An optional describing the difference between the cells' values,
    /// if an operation was performed on them.
    optional<int> applyInput(char, int, int, int, int);
    /// @brief Counts how many non-empty cells are remained in the grid.
    /// @return
    int contRemaining();
    optional<int> getCell(int x, int y);
};