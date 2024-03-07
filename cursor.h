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

/// @brief The user cursor on the Grid. By deafult, it spawn at the
/// bottom-left corner of the Grid.
class Cursor
{
    int xS_ = 0;
    int yS_ = 9;
    int xE_ = 1;
    int yE_ = 9;
    CursorType cursorType;

public:
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
    xS_ = xS_ + xAdd;
    yS_ = yS_ + yAdd;
    tuple<int, int> xyE = cursorType.cursorEnd(xS_, yS_);
    xE_ = get<0>(xyE);
    yE_ = get<1>(xyE);
}