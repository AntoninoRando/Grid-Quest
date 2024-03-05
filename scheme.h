#include <iostream>
#include <array>
#include <optional>

using namespace std;

#define ADD '+'
#define SUB '-'
#define DIV '/'
#define MUL '*'
#define EXP '^'

class Scheme : public array<array<optional<int>, 10>, 10>
{
    /// @brief Fill a gap (hole) in the scheme by letting fall down every cell
    /// above the hole, then shifting left every cell next to the new gaps born
    /// after the fall-down. This shifting process occurs from top to bottom.
    /// @param x The x coordinate (column) of the hole.
    /// @param y The y coordinate (row) of the hole.
    void ajustHole(int x, int y)
    {
        // Fall from above.
        while (y > 0 && !(*this)[y][x].has_value())
        {
            (*this)[y][x] = (*this)[y - 1][x];
            (*this)[y - 1][x] = optional<int>{};
            y--;
        }
        // Fall from left.
        while (y < 10 && !(*this)[y][x].has_value())
        {
            for (int xNext = x + 1; xNext < 10; xNext++)
            {
                (*this)[y][xNext - 1] = (*this)[y][xNext];
                (*this)[y][xNext] = optional<int>{};
            }
            y++;
        }
    }

public:
    void show(int, int, int, int);
    void fill(float);
    void applyInput(char, int, int, int, int);
};

void Scheme::show(int xS, int yS, int xE, int yE)
{
    system("CLS");
    for (int row = 0; row < 10; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            if (row == yS && col == xS)
            {
                cout << "\u001b[4m" << "\u001b[48;5;240m";
            }
            else if (row == yE && col == xE)
            {
                cout << "\u001b[48;5;240m";
            }

            optional<int> cell = (*this)[row][col];
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
        cout << endl
             << flush;
    }
}

void Scheme::fill(float emptiness = 0.2)
{
    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            (*this)[x][y] = optional<int>{};
        }
    }

    int maxLine = -1;
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist9(0, 9);
            std::uniform_int_distribution<std::mt19937::result_type> empt(0, 100);

            if ((empt(rng) < emptiness * 100) && x > maxLine)
            {
                break;
            }

            maxLine = max(x, maxLine);
            (*this)[y][x] = dist9(rng);
        }
    }
}

void Scheme::applyInput(char input, int xS, int yS, int xE, int yE)
{
    optional<int> v1 = (*this)[yS][xS];
    optional<int> v2 = (*this)[yE][xE];

    if (!v1.has_value() || !v2.has_value())
    {
        return;
    }

    switch (input)
    {
    case ADD:
        (*this)[yS][xS] = v1.value() + v2.value();
        break;
    case SUB:
        (*this)[yS][xS] = v1.value() - v2.value();
        break;
    case MUL:
        (*this)[yS][xS] = v1.value() * v2.value();
        break;
    default:
        return;
    }
    (*this)[yE][xE] = optional<int>{};
    ajustHole(xE, yE);
}