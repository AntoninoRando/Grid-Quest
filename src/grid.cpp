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

int Grid::contRemaining() const { return lefts_; }

optional<int> Grid::getCell(int x, int y) const
{
    if (x < 0 || x > 9 || y < 0 || y > 9)
        return optional<int>{};
    return grid[y][x];
}

void Grid::setCell(int x, int y, int value)
{
    if (x < 0 || x > 9 || y < 0 || y > 9)
        return;
    if (!grid[y][x].has_value())
        lefts_++;
    grid[y][x] = std::clamp(value, -999, 999);
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

std::string Grid::toString(int xS, int yS, int xE, int yE) const
{    
    std::stringstream ss;

    for (int row = 0; row < 10; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (col == xS && row == yS)
                ss << "*";
            if (col == xE && row == yE)
                ss << ">";

            optional<int> cell = grid[row][col];
            if (cell.has_value())
                ss << cell.value();
            ss << ".";
        }
        ss << ":";
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
    clearConsole();

    int spaces = 0;

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
                cout << "\033[" + color + "m";
                if (cell.value() >= 0)
                {
                    if (cell.value() <= 9)
                        spaces = 3;
                    else if (cell.value() <= 99)
                        spaces = 2;
                    else
                        spaces = 1;
                }
                else
                {
                    if (cell.value() >= -9)
                        spaces = 3;
                    else if (cell.value() >= -99)
                        spaces = 2;
                    else
                        spaces = 1;
                }

                cout << std::string(spaces, ' ');
                cout << cell.value();
                cout << " ";
            }
            else
            {
                if ((row == yS && col == xS) || (row == yE && col == xE))
                    cout << "   \033[31mX" << COL_RESET << " ";
                else
                    cout << "     ";
            }
            cout << COL_RESET;
        }
        cout << '\n';
    }
}

void Grid::fill(int amount)
{
    lefts_ = 0;
    srand(time(nullptr)); // Random seed based on the current time

    int row = 9;
    int col = 0;
    while (amount > 0 && row >= 0)
    {
        grid[row][col] = optional<int>{rand() % 9};
        lefts_++;
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
        grid[yS][xS] = std::clamp(v1.value() + v2.value(), -999, 999);
        lefts_--;
    }
    else if (input == SUB)
    {
        action = "subtract";
        grid[yS][xS] = std::clamp(v1.value() - v2.value(), -999, 999);
        lefts_--;
    }
    else if (input == MUL)
    {
        action = "multiply";
        grid[yS][xS] = std::clamp(v1.value() * v2.value(), -999, 999);
        lefts_--;
    }
    else if (input == MOD)
    {
        action = "module";
        grid[yS][xS] = std::clamp(v1.value() % v2.value(), -999, 999);
        lefts_--;
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
        grid[yS][xS] = std::clamp(v1.value() / v2.value(), -999, 999);
        lefts_--;
    }
    else if (input == MRG)
    {
        action = "merge";
        sign = (v1.value() < 0 || v2.value() < 0) ? -1 : 1;
        zeros = 10;
        while (zeros < abs(v2.value()))
            zeros *= 10;
        grid[yS][xS] = std::clamp(sign * (abs(v1.value()) * zeros + abs(v2.value())), -999, 999);
        lefts_--;
    }
    else
    {
        // Redis::get() << "input " << input << " action unknown";
        // Redis::get().push();
        return optional<int>{};
    }

    // Redis::get() << "input " << input << " action " << action;
    // Redis::get().push();
    grid[yE][xE] = optional<int>{};
    adjustHole(xE, yE);

    return diff;
}

std::optional<int> Grid::applyAction(std::string action, int xS, int yS, int xE, int yE)
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
        // Redis::get() << "input " << input << " action empty-cells";
        // Redis::get().push();
        return optional<int>{};
    }

    int diff = abs(v1.value() - v2.value());

    if ("Add" == action)
    {
        grid[yS][xS] = std::clamp(v1.value() + v2.value(), -999, 999);
        lefts_--;
    }
    else if ("Subtract" == action)
    {
        grid[yS][xS] = std::clamp(v1.value() - v2.value(), -999, 999);
        lefts_--;
    }
    else if ("Multiply" == action)
    {
        grid[yS][xS] = std::clamp(v1.value() * v2.value(), -999, 999);
        lefts_--;
    }
    else if ("Module" == action)
    {
        grid[yS][xS] = std::clamp(v1.value() % v2.value(), -999, 999);
        lefts_--;
    }
    else if ("Divide" == action)
    {
        if (v2.value() == 0 || v1.value() % v2.value() != 0)
        {
            // Redis::get() << "input " << input << " action illegal-divide";
            // Redis::get().push();
            return optional<int>{};
        }
        grid[yS][xS] = std::clamp(v1.value() / v2.value(), -999, 999);
        lefts_--;
    }
    else if ("Concat" == action)
    {
        sign = (v1.value() < 0 || v2.value() < 0) ? -1 : 1;
        zeros = 10;
        while (zeros < abs(v2.value()))
            zeros *= 10;
        grid[yS][xS] = std::clamp(sign * (abs(v1.value()) * zeros + abs(v2.value())), -999, 999);
        lefts_--;
    }
    else
    {
        // Redis::get() << "input " << input << " action unknown";
        // Redis::get().push();
        return optional<int>{};
    }

    // Redis::get() << "input " << input << " action " << action;
    // Redis::get().push();
    grid[yE][xE] = optional<int>{};
    adjustHole(xE, yE);

    return diff;
}
