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
                cout << "\u001b[7;1m";
            }
            else if (row == yE && col == xE)
            {
                cout << "\u001b[4;1m";
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
            cout << "\u001b[0m ";
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
            (*this)[x][y] = optional<int> {};
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

void Scheme::applyInput(char input, int x1, int y1, int x2, int y2)
{
    optional<int> v1 = (*this)[y1][x1];
    optional<int> v2 = (*this)[y2][x2];

    if (!v1.has_value()|| !v2.has_value())
    {
        return;
    }

    switch (input)
    {
    case ADD:
        (*this)[y1][x1] = v1.value() + v2.value();
        break;
    case SUB:
        (*this)[y1][x1] = v1.value() - v2.value();
        break;
    case MUL:
        (*this)[y1][x1] = v1.value() * v2.value();
        break;
    default:
        return;
    }

    while (x2 < 9 && (*this)[y1][x2].has_value())
    {
        (*this)[y1][x2] = (*this)[y1][x2 + 1];
        x2++;
    }
    if (x2 == 9)
    {
        (*this)[y1][x2] = optional<int> {};
    }
}