#include <tuple>
#pragma once // This header file has been included in mulitple file, thus this avoid error C2011

using namespace std; // Without this line, we have to write std::tuple instead of tuple

/// @brief All possible cursor direction (right, bottom, left and top). They are
/// defined in clockwise order.
enum CursorDirection
{
    RGT,
    BOT,
    LFT,
    TOP,
};

class CursorType
{
protected:
    CursorDirection direction = CursorDirection::RGT;

public:
    virtual tuple<int, int> cursorEnd(int, int);
    void rotateLeft()
    {
        switch (direction)
        {
        case RGT:
            direction = TOP;
            break;
        case TOP:
            direction = LFT;
            break;
        case LFT:
            direction = BOT;
            break;
        case BOT:
            direction = RGT;
            break;
        }
    }
    void rotateRight()
    {
        switch (direction)
        {
        case RGT:
            direction = BOT;
            break;
        case BOT:
            direction = LFT;
            break;
        case LFT:
            direction = TOP;
            break;
        case TOP:
            direction = RGT;
            break;
        }
    }
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
    int modX(int);
    int modY(int);
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
    void rotateLeft()
    {
        cursorType.rotateLeft();
        updateCursor(0, 0);
    }
    void rotateRight()
    {
        cursorType.rotateRight();
        updateCursor(0, 0);
    }
};

/// @brief Add (xAdd, yAdd) to the cursor start coordinate, then adjust where
/// the cursor ends.
void Cursor::updateCursor(int xAdd, int yAdd)
{
    xS_ = modX(xS_ + xAdd);
    yS_ = modY(yS_ + yAdd);
    tuple<int, int> xyE = cursorType.cursorEnd(xS_, yS_);
    xE_ = modX(get<0>(xyE));
    yE_ = modY(get<1>(xyE));
}

///@brief Perform positive module operator between x and xMax.
int Cursor::modX(int x)
{
    return (x % xMax + xMax) % xMax;
}

///@brief Perform positive module operator between y and yMax.
int Cursor::modY(int y)
{
    return (y % yMax + yMax) % yMax;
}