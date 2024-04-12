// #pragma once // This header file has been included in mulitple file, thus this avoid error C2011
#ifndef CURSOR_H_
#define CURSOR_H_

#include <tuple>

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
    virtual std::tuple<int, int> cursorEnd(int xS, int yS)
    {
        switch (direction)
        {
        case CursorDirection::RGT:
            return std::make_tuple(xS + 1, yS);
        case CursorDirection::LFT:
            return std::make_tuple(xS - 1, yS);
        case CursorDirection::TOP:
            return std::make_tuple(xS, yS - 1);
        case CursorDirection::BOT:
            return std::make_tuple(xS, yS + 1);
        default:
            break;
        }

        return std::make_tuple(xS + 1, yS);
    }
    void rotateLeft();
    void rotateRight();
};

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
    int xS() const { return xS_; }
    int yS() const { return yS_; }
    int xE() const { return xE_; }
    int yE() const { return yE_; }
    void setType(CursorType t) { cursorType = t; }

    /// @brief Add (xAdd, yAdd) to the cursor start coordinate, then adjust where
    /// the cursor ends.
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

#endif