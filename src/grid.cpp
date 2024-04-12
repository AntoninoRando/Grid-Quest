#include "grid.h"

void Grid::ajustHole(int x, int y)
{
    // Fall from above.
    while (y > 0 && !grid[y][x].has_value())
    {
        grid[y][x] = grid[y - 1][x];
        grid[y - 1][x] = optional<int>{};
        y--;
    }
    // Fall from left.
    while (y < 10 && !grid[y][x].has_value())
    {
        for (int xNext = x + 1; xNext < 10; xNext++)
        {
            grid[y][xNext - 1] = grid[y][xNext];
            grid[y][xNext] = optional<int>{};
        }
        y++;
    }
}

int Grid::contRemaining()
{
    int count = 0;
    int strikes = 0;
    
    for (int row = 9; row >= 0; row--)
    {
        if (strikes == 2)
        {
            break;
        }

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
    {
        return optional<int>{};
    }
    return grid[y][x];
}

tuple<int, int, int, int> Grid::modCursor(int xS, int yS, int xE, int yE)
{
    int xM = 0;
    while (xM < 10 && grid[9][xM].has_value())
    {
        xM += 1;
    }
    int yM = 0;
    while (yM < 10 && !grid[yM][0].has_value())
    {
        yM += 1;
    }
    yM = 10 - yM;
    xS = (xS % xM + xM) % xM;
    xE = (xE % xM + xM) % xM;
    yS = (yS % yM + yM) % yM + (10 - yM);
    yE = (yE % yM + yM) % yM + (10 - yM);
    return std::make_tuple(xS, yS, xE, yE);
}

void Grid::show(int xS, int yS, int xE, int yE)
{
    tuple<int, int, int, int> xyMod = modCursor(xS, yS, xE, yE);
    xS = get<0>(xyMod);
    yS = get<1>(xyMod);
    xE = get<2>(xyMod);
    yE = get<3>(xyMod);

    clearConsole();
    for (int row = 0; row < 10; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == yS && col == xS)
            {
                cout << "\u001b[4m"
                     << "\u001b[41;1m";
            }
            else if (row == yE && col == xE)
            {
                cout << "\u001b[31;1m";
            }

            optional<int> cell = grid[row][col];
            if (cell.has_value())
            {
                cout << cell.value();
            }
            else
            {
                cout << ' ';
            }
            cout << "\u001b[0m\t";
        }
        cout << '\n';
    }
}

void Grid::fill(float emptiness = 0.2)
{
    int remaining = 100 - (100 * emptiness);

    // Initialize the random seed based on the current time
    srand(time(nullptr));
    for (int row = 9; row >= 0; row--)
    {
        for (int col = 0; col < 10; col++)
        {
            grid[row][col] = optional<int>{rand() % 9};
            remaining -= 1;

            if (remaining == 0)
            {
                return;
            }
        }
    }
}

optional<int> Grid::applyInput(char input, int xS, int yS, int xE, int yE)
{
    tuple<int, int, int, int> xyMod = modCursor(xS, yS, xE, yE);
    xS = get<0>(xyMod);
    yS = get<1>(xyMod);
    xE = get<2>(xyMod);
    yE = get<3>(xyMod);

    optional<int> v1 = grid[yS][xS];
    optional<int> v2 = grid[yE][xE];
    int sign = 1;
    int zeros = 1;

    if (!v1.has_value() || !v2.has_value())
    {
        return optional<int>{};
    }

    int diff = abs(v1.value() - v2.value());

    switch (input)
    {
    case ADD:
        grid[yS][xS] = v1.value() + v2.value();
        break;
    case SUB:
        grid[yS][xS] = v1.value() - v2.value();
        break;
    case MUL:
        grid[yS][xS] = v1.value() * v2.value();
        break;
    case MOD:
        grid[yS][xS] = v1.value() % v2.value();
        break;
    case DIV:
        if (v2.value() == 0 || v1.value() % v2.value() != 0)
        {
            return optional<int>{};
        }
        grid[yS][xS] = v1.value() / v2.value();
        break;
    case MRG:
        sign = (v1.value() < 0 || v2.value() < 0) ? -1 : 1;
        zeros = 10;
        while (zeros < abs(v2.value()))
        {
            zeros *= 10;
        }
        grid[yS][xS] = sign * (abs(v1.value()) * zeros + abs(v2.value()));
        break;
    default:
        return optional<int>{};
    }
    grid[yE][xE] = optional<int>{};
    ajustHole(xE, yE);

    return diff;
}