#include "grid.h"
#include "utils.h"
#include "settings.h"
#include "monitors.h"
#include <cassert>
#include <cstdlib> // for rand and srand
#include <ctime>   // for time

using std::optional, std::cout;

void Grid::adjustHole(int x, int y)
{
    // Fall from above.
    while (y > 0 && !grid[y][x].has_value())
    {
        grid[y][x] = grid[y - 1][x];
        grid[y - 1][x] = std::optional<int>{};
        y--;
    }
    // Fall from left.
    while (y < 10 && !grid[y][x].has_value())
    {
        for (int xNext = x + 1; xNext < 10; xNext++)
        {
            grid[y][xNext - 1] = grid[y][xNext];
            grid[y][xNext] = std::optional<int>{};
        }
        y++;
    }
}

int Grid::contRemaining() const
{
    int count = 0;
    int strikes = 0;

    for (int row = 9; row >= 0; row--)
    {
        if (strikes == 2)
            break;

        for (int col = 0; col < 10; col++)
        {
            if (grid[row][col].has_value())
            {
                count += 1;
                strikes = 0;
                continue;
            }

            strikes++;
            break;
        }
    }
    return count;
}

optional<int> Grid::getCell(int x, int y)
{
    if (x < 0 || x > 9 || y < 0 || y > 9)
        return optional<int>{};
    return grid[y][x];
}

void Grid::setCell(int x, int y, int value)
{
    if (x < 0 || x > 9 || y < 0 || y > 9)
        return;
    grid[y][x] = value;
}

std::string Grid::toString() const
{
    std::stringstream ss;
    for (int row = 9; row >= 0; row--)
    {
        for (int col = 0; col < 10; col++)
        {
            optional<int> cell = grid[row][col];
            if (cell.has_value())
                ss << cell.value() << ".";
            else
                return ss.str();
        }
        ss << "."; // Last cell added a dot, so we add another to make two dots.
    }
    return ss.str();
}

std::tuple<int, int, int, int> Grid::modCursor(int xS, int yS, int xE, int yE) const
{
    int xM = 0;
    while (xM < 10 && grid[9][xM].has_value())
        xM += 1;

    int yM = 0;
    while (yM < 10 && !grid[yM][0].has_value())
        yM += 1;

    yM = 10 - yM;
    xS = (xS % xM + xM) % xM;
    xE = (xE % xM + xM) % xM;
    yS = (yS % yM + yM) % yM + (10 - yM);
    yE = (yE % yM + yM) % yM + (10 - yM);
    return std::make_tuple(xS, yS, xE, yE);
}

void Grid::show(int xS, int yS, int xE, int yE) const
{
    std::tuple<int, int, int, int> xyMod = modCursor(xS, yS, xE, yE);
    xS = std::get<0>(xyMod);
    yS = std::get<1>(xyMod);
    xE = std::get<2>(xyMod);
    yE = std::get<3>(xyMod);

    clearConsole();

    for (int row = 0; row < 10; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            std::string color;
            if (row == yS && col == xS)
                color = PCELL_COL;
            else if (row == yE && col == xE)
                color = SCELL_COL;
            else if ((10 * row + col) % 2 == 0)
                color = ECELLS_COL;
            else
                color = OCELLS_COL;

            optional<int> cell = grid[row][col];
            if (cell.has_value())
            {
                cout << "\u001b[" + color + "m";
                cout << std::string(2, ' ');
                cout << cell.value();
                cout << std::string(2, ' ');
            }
            else
            {
                int tot = lPadding + cellSize + rPadding;
                cout << std::string(5, ' ');
            }
            cout << "\u001b[0m\u001b[" + BG_COL + "m";
        }
        cout << '\n';
    }
}

void Grid::fill(int amount)
{
    srand(time(nullptr)); // Random seed based on the current time

    int row = 9;
    int col = 0;
    while (amount > 0 && row >= 0)
    {
        grid[row][col] = optional<int>{rand() % 9};
        amount -= 1;

        col = (col + 1) % 10;
        row = (col == 0) ? row - 1 : row;
    }
}

optional<int> Grid::applyInput(char input, int xS, int yS, int xE, int yE)
{
    std::tuple<int, int, int, int> xyMod = modCursor(xS, yS, xE, yE);
    xS = std::get<0>(xyMod);
    yS = std::get<1>(xyMod);
    xE = std::get<2>(xyMod);
    yE = std::get<3>(xyMod);

    optional<int> v1 = grid[yS][xS];
    optional<int> v2 = grid[yE][xE];
    int sign = 1;
    int zeros = 1;

    if (!v1.has_value() || !v2.has_value())
    {
        Redis::get() << "input " << input << " action empty-cells";
        Redis::get().push();
        return optional<int>{};
    }

    int diff = abs(v1.value() - v2.value());
    std::string action;

    if (input == ADD)
    {
        action = "add";
        grid[yS][xS] = v1.value() + v2.value();
    }
    else if (input == SUB)
    {
        action = "subtract";
        grid[yS][xS] = v1.value() - v2.value();
    }
    else if (input == MUL)
    {
        action = "multiply";
        grid[yS][xS] = v1.value() * v2.value();
    }
    else if (input == MOD)
    {
        action = "module";
        grid[yS][xS] = v1.value() % v2.value();
    }
    else if (input == DIV)
    {
        action = "divide";
        if (v2.value() == 0 || v1.value() % v2.value() != 0)
        {
            Redis::get() << "input " << input << " action illegal-divide";
            Redis::get().push();
            return optional<int>{};
        }
        grid[yS][xS] = v1.value() / v2.value();
    }
    else if (input == MRG)
    {
        action = "merge";
        sign = (v1.value() < 0 || v2.value() < 0) ? -1 : 1;
        zeros = 10;
        while (zeros < abs(v2.value()))
            zeros *= 10;
        grid[yS][xS] = sign * (abs(v1.value()) * zeros + abs(v2.value()));
    }
    else
    {
        Redis::get() << "input " << input << " action unknown";
        Redis::get().push();
        return optional<int>{};
    }

    Redis::get() << "input " << input << " action " << action;
    Redis::get().push();
    grid[yE][xE] = optional<int>{};
    adjustHole(xE, yE);

    return diff;
}