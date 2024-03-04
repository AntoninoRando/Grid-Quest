#include <tuple>
#pragma once // This header file has been included in mulitple file, thus this avoid error C2011

using namespace std; // Without this line, we have to write std::tuple instead of tuple

enum CursorDirection
{
    RGT,
    LFT,
    TOP,
    BOT,
};

class CursorType
{
protected:
    CursorDirection direction = CursorDirection::RGT;

public:
    virtual tuple<int, int> cursorEnd(int, int);
};

tuple<int, int> CursorType::cursorEnd(int xS, int yS)
{
    switch (direction)
    {
    case CursorDirection::RGT:
        return tuple<int, int>(xS + 1, yS);
    case CursorDirection::LFT:
        return tuple<int, int>(xS - 1, yS);
    case CursorDirection::TOP:
        return tuple<int, int>(xS, yS - 1);
    case CursorDirection::BOT:
        return tuple<int, int>(xS, yS + 1);
    default:
        break;
    }

    return tuple<int, int>(xS + 1, yS);
}

/// @brief The user cursor on the scheme.
class Cursor
{
    int xS_ = 0;
    int yS_ = 0;
    int xE_ = 1;
    int yE_ = 0;
    int xMax;
    int yMax;
    int clampX(int);
    int clampY(int);
    CursorType cursorType;

public:
    /// @brief Create the user cursor placed at the start of the scheme.
    /// @param schemeW Scheme max row size.
    /// @param schemeH scheme max column size.
    Cursor(int schemeW, int schemeH)
    {
        xMax = schemeW;
        yMax = schemeH;
    }
    int xS() { return xS_; }
    int yS() { return yS_; }
    int xE() { return xE_; }
    int yE() { return yE_; }
    void setType(CursorType t) { cursorType = t; }
    void updateCursor(int, int);
};

/// @brief Add (xAdd, yAdd) to the cursor start coordinate, then adjust where
/// the cursor ends.
void Cursor::updateCursor(int xAdd, int yAdd)
{
    xS_ = clampX(xS_ + xAdd);
    yS_ = clampY(yS_ + yAdd);
    tuple<int, int> xyE = cursorType.cursorEnd(xS_, yS_);
    xE_ = clampX(get<0>(xyE));
    yE_ = clampY(get<1>(xyE));
}

int Cursor::clampX(int x)
{
    if (x >= xMax)
    {
        return x = 0;
    }
    if (x < 0)
    {
        return x = xMax - 1;
    }
    return x;
}

int Cursor::clampY(int y)
{
    if (y >= yMax)
    {
        return y = 0;
    }
    if (y < 0)
    {
        return y = yMax - 1;
    }
    return y;
}